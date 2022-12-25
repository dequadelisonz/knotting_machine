#ifndef IMPORTER_HPP
#define IMPORTER_HPP

/*ESP-IDF includes*/
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "sdkconfig.h"

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/*This project includes*/
#include "SDDriver.hpp"

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

static char s_output_buffer[256 * 50] = {0};

enum CycleReadModeEnum
{
    WIFI,
    CARD
};

class Importer

{

private:
    static constexpr char *TAG = "Importer";

    static constexpr char *HAS_MOVED = "<H1>Moved Temporarily</H1>";

    char _cycleFileFullPath[24] = {0}; // mount point + file name

    char *_cycleCode; // string containing the cycle code

    uint8_t _WIFI_SSID[32] = {0};
    uint8_t _WIFI_PWD[64] = {0};
    char _APP_SCRIPT_URL[256] = {0};

    SDDriver _sdCard;

    static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
    {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY)
            {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "retry to connect to the AP");
            }
            else
            {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(TAG, "connect to the AP fail");
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }

    esp_err_t wifi_init_sta(void)
    {
        s_wifi_event_group = xEventGroupCreate();

        esp_err_t ret = esp_netif_init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error initializing netif: %s", esp_err_to_name(ret));
            return ret;
        }

        ret = esp_event_loop_create_default();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error creating event loop: %s", esp_err_to_name(ret));
            return ret;
        }

        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

        ret = esp_wifi_init(&cfg);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error initializing wifi: %s", esp_err_to_name(ret));
            return ret;
        }

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ret = esp_event_handler_instance_register(WIFI_EVENT,
                                                  ESP_EVENT_ANY_ID,
                                                  &wifi_event_handler,
                                                  NULL,
                                                  &instance_any_id);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error registering WIFI_EVENT: %s", esp_err_to_name(ret));
            return ret;
        };
        ret = esp_event_handler_instance_register(IP_EVENT,
                                                  IP_EVENT_STA_GOT_IP,
                                                  &wifi_event_handler,
                                                  NULL,
                                                  &instance_got_ip);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error registering IP_EVENT: %s", esp_err_to_name(ret));
            return ret;
        }

        wifi_config_t wifi_config = {
            .sta = {
                // CONFIG_ESP_WIFI_SSID
                // CONFIG_ESP_WIFI_PASSWORD,
                /* Setting a password implies station will connect to all security modes including WEP/WPA.
                 * However these modes are deprecated and not advisable to be used. Incase your Access point
                 * doesn't support WPA2, these mode can be enabled by commenting below line */
                {},
                {},
                {},
                {},
                {},
                {},
                .threshold = {0, ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD},
                //{0, {ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD}},

            },
        };
        memcpy(wifi_config.sta.ssid, _WIFI_SSID, strlen((char *)_WIFI_SSID) + 1);
        memcpy(wifi_config.sta.password, _WIFI_PWD, strlen((char *)_WIFI_PWD) + 1);

        ret = esp_wifi_set_mode(WIFI_MODE_STA);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error setting WIFI mode: %s", esp_err_to_name(ret));
            return ret;
        }

        ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error setting wifi configuration: %s", esp_err_to_name(ret));
            return ret;
        }

        ret = esp_wifi_start();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error starting WIFI: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI(TAG, "wifi_init_sta finished.");

        /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
         * number of re-tries (WIFI_FAIL_BIT). The bits are set by wifi_event_handler() (see above) */
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                               WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);

        /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
         * happened. */
        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "connected to ap SSID:%s",
                     (char *)_WIFI_SSID);
            ret = ESP_OK;
        }
        else if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                     (char *)_WIFI_SSID);
            ret = ESP_FAIL;
        }
        else
        {
            ESP_LOGE(TAG, "UNEXPECTED EVENT");
            ret = ESP_FAIL;
        }
        return ret;
    }

    static esp_err_t http_event_handler(esp_http_client_event_t *evt)
    {
        static char *output_buffer; // Buffer to store response of http request from event handler
        static int output_len = 0;  // Stores number of bytes read

        switch (evt->event_id)
        {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "****HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */

            if (!esp_http_client_is_chunked_response(evt->client))
            {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data)
                {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                    printf((char *)(evt->user_data + output_len));
                }
                else
                {
                    if (output_buffer == NULL)
                    {
                        output_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;

                        if (output_buffer == NULL)
                        {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }

                output_len += evt->data_len;
            }

            ESP_LOGI(TAG, "Cumulative data length: %d", output_len);
            memcpy(s_output_buffer + output_len, evt->data, evt->data_len);
            printf(s_output_buffer);
            output_len += evt->data_len;
            if (strstr((const char *)evt->data, HAS_MOVED))
                output_len = 0;
            // memset(s_output_buffer, 0, sizeof(s_output_buffer));
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL)
            {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0)
            {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL)
            {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;

            break;
        }
        return ESP_OK;
    }

    esp_err_t downloadCycle(void)
    {
        // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
        ret = wifi_init_sta();

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error wifi_init_sta: %s", esp_err_to_name(ret));
            return ret;
        }

        esp_http_client_config_t config = {
            .url = CONFIG_APPSCRIPT_URL,
            .event_handler = http_event_handler,
            .crt_bundle_attach = esp_crt_bundle_attach};
        esp_http_client_handle_t client = esp_http_client_init(&config);
        ret = esp_http_client_perform(client);

        if (ret == ESP_OK)
        {
            int sc = esp_http_client_get_status_code(client);
            ESP_LOGI(TAG, "HTTP chunk encoding Status = %d, content_length = %d",
                     sc,
                     esp_http_client_get_content_length(client));
            strcpy(_cycleCode, s_output_buffer);
            ESP_LOGI(TAG, "Saving cycle to SDCARD after download: %s", esp_err_to_name(ret));
            ret = _sdCard.saveFile(_cycleCode, _cycleFileFullPath);
        }
        else
        {
            ESP_LOGE(TAG, "Error perform http request: %s", esp_err_to_name(ret));
        }
        esp_http_client_cleanup(client);
        esp_wifi_disconnect();
        esp_wifi_stop();

        return ret;
    }

    esp_err_t readCycle(CycleReadModeEnum rMode)
    {
        esp_err_t ret = ESP_OK;

        switch (rMode)
        {
        case (CARD):
            ESP_LOGI(TAG, "Reading cycle from SDCARD.");
            ret = _sdCard.readFile(_cycleCode, _cycleFileFullPath);
            break;
        case (WIFI):
            ESP_LOGI(TAG, "Downloading cycle from WIFI.");
            ret = downloadCycle();
            break;
        };
        return ret;
    }

    void readOTAInfo() // if card or info on card not available return infor to try with WIFI
    {
        ESP_LOGI(TAG, "Reading OTA info for WIFI connection.");
        // CycleReadModeEnum ret = CARD;
        char SSIDInfo[512] = {0};
        char SSIDPath[32] = {0};
        strcat(SSIDPath, CONFIG_SDCARD_MOUNT_POINT);
        strcat(SSIDPath, CONFIG_WIFI_DATA_FILENAME);
        esp_err_t err = _sdCard.readFile(SSIDInfo, SSIDPath);
        if (err != ESP_OK)
        {
            ESP_LOGW(TAG, "Error reading SSID info from SDCARD: %s\n"
                          "Setting SSID and PWD from sdkconfig.",
                     esp_err_to_name(err));
            memcpy(_WIFI_SSID, CONFIG_ESP_WIFI_SSID, strlen(CONFIG_ESP_WIFI_SSID));
            memcpy(_WIFI_PWD, CONFIG_ESP_WIFI_PASSWORD, strlen(CONFIG_ESP_WIFI_PASSWORD));
            memcpy(_APP_SCRIPT_URL, CONFIG_APPSCRIPT_URL, strlen(CONFIG_APPSCRIPT_URL));
        }
        else
        {
            // extract WIFI auth and app url from file in SDCARD

            const char DELIMITER[] = "\n";
            char *token = strtok(SSIDInfo, DELIMITER); // extracting SSID
            memcpy(_WIFI_SSID, token, strlen(token) + 1);
            token = strtok(NULL, DELIMITER); // extracting password
            memcpy(_WIFI_PWD, token, strlen(token));
            token = strtok(NULL, DELIMITER); // extracting url
            memcpy(_APP_SCRIPT_URL, token, strlen(token));
        }
        ESP_LOGI(TAG, "SSID: %s\nPWD: %s\nURL: %s", _WIFI_SSID, _WIFI_PWD, _APP_SCRIPT_URL);
        // return ret;
    }

    bool _testSDCard()
    {
        return _sdCard.testSDCard();
    }

public:
    Importer(char *cycleCode) : _cycleCode(cycleCode)
    {
        strcat(_cycleFileFullPath, CONFIG_SDCARD_MOUNT_POINT);
        strcat(_cycleFileFullPath, CONFIG_KNOTTING_CYCLE_FILENAME);
        readOTAInfo();
    }

    esp_err_t init()
    {

        ESP_LOGI(TAG, "Initializing Importer, reading cycle.");

        esp_err_t ret = ESP_OK;

        if (!_testSDCard())
        {
            ESP_LOGE(TAG, "**SDCARD readonly or not available.**\n"
                          "Make sure an SDCARD formatted FAT32 is present in the slot,"
                          "it's not READ-ONLY and is empty, then restart the system.\nAborting sequence...");
            ret = ESP_FAIL;
        }
        else
        {
            ESP_LOGI(TAG, "SDCARD available, trying to read cycle from SDCARD...");
            ret = readCycle(CARD);
            if (ret != ESP_OK)
            {
                ESP_LOGW(TAG, "Cycle file cannot be read from SDCARD, trying to download cycle");
                ret = readCycle(WIFI);
            };
            //else
            //printf("************Cycle :%s\n",_cycleCode); //for debugging
            ret=(strlen(_cycleCode)!=0)?ESP_OK:ESP_ERR_INVALID_STATE;
        }
        //printf("***************ERROR: %d\n",ret);
        return ret;
    }

    // const char *getCycleCode() const
    // {
    //     return _cycleCode;
    // }
};

#endif