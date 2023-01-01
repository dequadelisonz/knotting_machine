#ifndef Downloader_hpp
#define Downloader_hpp

/*
This class is helper class to download the cycle sequence from app script
Usage:
Instantiate new object giving WIFI SSID and PWD in constructor.
Then if _downloaded() == true retireve cycle string with getCycleCode()
else if _downloaded() == false then check for error outpu log about connection --> cycleCode string will be empty
*/

/*STL includes*/

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

// TODO encapsulate these variables somehow
// namespace DownloaderNS
//{
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;
static char s_output_buffer[256 * 50] = {0};
//}

class Downloader
{
  friend class ImporterWIFI;

public:
  static const uint8_t _WIFI_SSID_LEN = 32;
  static const uint8_t _WIFI_PWD_LEN = 64;
  static const uint8_t _APP_SCR_LEN = 255;

private:
  static constexpr char *TAG = "Downloader";

  static constexpr char *HAS_MOVED = "<H1>Moved Temporarily</H1>";

  esp_err_t _ret = ESP_ERR_INVALID_STATE;
  char _cycleFileFullPath[24] = {0}; // mount point + file name

  const uint8_t (&_WIFI_SSID)[_WIFI_SSID_LEN];
  const uint8_t (&_WIFI_PWD)[_WIFI_PWD_LEN];
  const char (&_APP_SCRIPT_URL)[_APP_SCR_LEN];
  char (&_cycleCode)[CONFIG_MAX_LENGTH_CYCLE_STRING]; // reference to string containing the cycle code

  esp_http_client_handle_t _client;

  Downloader(const uint8_t (&wifiSSID)[_WIFI_SSID_LEN],
             const uint8_t (&wifiPWD)[_WIFI_PWD_LEN],
             const char (&appScriptUrl)[_APP_SCR_LEN],
             char (&cycleCode)[CONFIG_MAX_LENGTH_CYCLE_STRING]);

  void _wifi_init_sta(void);

  static void _wifi_event_handler(void *arg, esp_event_base_t event_base,
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

  static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
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

  void _connect();

  void _download();

  bool _downloaded() const { return (_ret == ESP_OK) ? true : false; }
};

#endif
