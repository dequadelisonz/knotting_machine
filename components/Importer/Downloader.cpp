#include "Downloader.hpp"

char Downloader::TAG[] = "Downloader";
char Downloader::HAS_MOVED[] = "<H1>Moved Temporarily</H1>";

char Downloader::s_output_buffer[CONFIG_MAX_LENGTH_CYCLE_STRING] = {0};

EventGroupHandle_t Downloader::s_wifi_event_group = NULL;

uint8_t Downloader::s_retry_num = 0U;

Downloader::Downloader(const char *const wifiSSID,
                       const char *const wifiPWD,
                       const char *const appScriptUrl,
                       char *const cycleCode) : _WIFI_SSID(wifiSSID),
                                                _WIFI_PWD(wifiPWD),
                                                _APP_SCRIPT_URL(appScriptUrl),
                                                _cycleCode(cycleCode)
{
    memset(s_output_buffer, 0, sizeof(s_output_buffer));
    _connect();
    _download();
}

void Downloader::_wifiEventHandler(void *arg, esp_event_base_t event_base,
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
            vTaskDelay(1000 / portTICK_PERIOD_MS);
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
        s_retry_num = 0U;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void Downloader::_wifiInitSta(void) // helper to startup WIFI
{
    s_wifi_event_group = xEventGroupCreate();

    _ret = esp_netif_init();

    if (_ret == ESP_OK)
    {
        _ret = esp_event_loop_create_default();

        if ((_ret == ESP_OK) || (_ret == ESP_ERR_INVALID_STATE))
        {
            _accessPoint = esp_netif_create_default_wifi_sta();
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

            _ret = esp_wifi_init(&cfg);

            if (_ret == ESP_OK)
            {
                esp_event_handler_instance_t instance_any_id;
                esp_event_handler_instance_t instance_got_ip;
                _ret = esp_event_handler_instance_register(WIFI_EVENT,
                                                           ESP_EVENT_ANY_ID,
                                                           &_wifiEventHandler,
                                                           NULL,
                                                           &instance_any_id);
                if (_ret == ESP_OK)
                {
                    _ret = esp_event_handler_instance_register(IP_EVENT,
                                                               IP_EVENT_STA_GOT_IP,
                                                               &_wifiEventHandler,
                                                               NULL,
                                                               &instance_got_ip);
                    if (_ret == ESP_OK)
                    {
                        wifi_config_t wifi_config = {
                            .sta = {
                                {},
                                {}, // CONFIG_ESP_WIFI_PASSWORD,
                                /* Setting a password implies station will connect to all security modes including WEP/WPA.
                                 * However these modes are deprecated and not advisable to be used. Incase your Access point
                                 * doesn't support WPA2, these mode can be enabled by commenting below line */
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                //.threshold = {0, ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD},
                                {0, ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                {},
                                //{0, {ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD}},

                            },
                        };
                        memcpy(wifi_config.sta.ssid, _WIFI_SSID, strlen((char *)_WIFI_SSID) + 1);
                        memcpy(wifi_config.sta.password, _WIFI_PWD, strlen((char *)_WIFI_PWD) + 1);
                        _ret = esp_wifi_set_mode(WIFI_MODE_STA);
                        if (_ret == ESP_OK)
                        {
                            _ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
                            if (_ret == ESP_OK)
                            {
                                _ret = esp_wifi_start();
                                if (_ret == ESP_OK)
                                {
                                    ESP_LOGI(TAG, "_wifiInitSta finished.");

                                    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
                                     * number of re-tries (WIFI_FAIL_BIT). The bits are set by _wifiEventHandler() (see above) */
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
                                        _ret = ESP_OK;
                                    }
                                    else if (bits & WIFI_FAIL_BIT)
                                    {
                                        ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                                                 (char *)_WIFI_SSID);
                                        _ret = ESP_FAIL;
                                    }
                                    else
                                    {
                                        ESP_LOGE(TAG, "UNEXPECTED EVENT");
                                        _ret = ESP_FAIL;
                                    }
                                }
                                else
                                {
                                    ESP_LOGE(TAG, "Error starting WIFI: %s", esp_err_to_name(_ret));
                                }
                            }
                            else
                            {
                                ESP_LOGE(TAG, "Error setting wifi configuration: %s", esp_err_to_name(_ret));
                            }
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Error setting WIFI mode: %s", esp_err_to_name(_ret));
                        }
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Error registering IP_EVENT: %s", esp_err_to_name(_ret));
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "Error registering WIFI_EVENT: %s", esp_err_to_name(_ret));
                }
            }
            else
            {
                ESP_LOGE(TAG, "Error initializing wifi: %s", esp_err_to_name(_ret));
            };
        }
        else
        {
            ESP_LOGE(TAG, "Error creating event loop: %s", esp_err_to_name(_ret));
        };
    }
    else
    {
        ESP_LOGE(TAG, "Error initializing netif: %s", esp_err_to_name(_ret));
    };
}

esp_err_t Downloader::_httpEventHandler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len = 0;  // Stores number of bytes read

    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    }
    case HTTP_EVENT_ON_CONNECTED:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    }
    case HTTP_EVENT_HEADER_SENT:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    }
    case HTTP_EVENT_ON_HEADER:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    }
    case HTTP_EVENT_ON_DATA:
    {
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
                    output_len = 0U;

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
        memcpy(Downloader::s_output_buffer + output_len, evt->data, evt->data_len);
        printf(Downloader::s_output_buffer);
        output_len += evt->data_len;
        if (strstr((const char *)evt->data, HAS_MOVED))
            output_len = 0U;
        break;
    }
    case HTTP_EVENT_ON_FINISH:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0U;
        break;
    }
    case HTTP_EVENT_DISCONNECTED:
    {
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
        output_len = 0U;

        break;
    }
    default:
    {
        break;
    }
    }
    return ESP_OK;
}

void Downloader::_connect()
{
    ESP_LOGI(TAG, "Connecting...");
    _ret = nvs_flash_init();

    if (_ret == ESP_ERR_NVS_NO_FREE_PAGES || _ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        _ret = nvs_flash_init();
    }

    if (_ret == ESP_OK)
    {

        ESP_LOGI(TAG, "Initing WIFI sta mode...");
        _wifiInitSta(); // set Downloader::_ret depending on result of wifi connection
        if (_ret == ESP_OK)
        {
            esp_http_client_config_t config = {
                .url = _APP_SCRIPT_URL,
                .event_handler = &Downloader::_httpEventHandler,
                .crt_bundle_attach = esp_crt_bundle_attach};

            ESP_LOGI(TAG, "Initing _client.");

            _client = esp_http_client_init(&config);

            ESP_LOGI(TAG, "_client initialized.");
        }
        else
        {
            ESP_LOGE(TAG, "Error _wifiInitSta: %s", esp_err_to_name(_ret));
            _ret = ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error nvs_flash_init: %s", esp_err_to_name(_ret));
    }
}

void Downloader::_download()
{

    if (_ret == ESP_OK)
    {
        // TODO da sistemare dopo aver capito il problema con la connessione
        // ESP_ERROR_CHECK(esp_http_client_perform(_client));
        _ret = esp_http_client_perform(_client);

        if (_ret == ESP_OK)
        {
            int sc = esp_http_client_get_status_code(_client);
            ESP_LOGI(TAG, "HTTP chunk encoding Status = %d, content_length = %lld",
                     sc,
                     esp_http_client_get_content_length(_client));
            strcpy(_cycleCode, Downloader::s_output_buffer);
        }
        else
        {
            ESP_LOGE(TAG, "Error perform http request: %s", esp_err_to_name(_ret));
            _cycleCode[0] = '\0'; // empty _cycleString if download went wrong
        }
        esp_http_client_cleanup(_client);
    }
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_netif_destroy_default_wifi(_accessPoint);
}