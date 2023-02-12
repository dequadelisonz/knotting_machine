#ifndef Downloader_hpp
#define Downloader_hpp

/*STL includes*/

/*ESP-IDF includes*/

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

#define CONFIG_ESP_MAXIMUM_RETRY 5

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

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

class Downloader
{

public:
  static const uint8_t _WIFI_SSID_LEN = 32U;
  static const uint8_t _WIFI_PWD_LEN = 64U;
  static const uint8_t _APP_SCR_LEN = 255U;

private:
  static char TAG[];
  static char HAS_MOVED[];

  const char *const _WIFI_SSID;
  const char *const _WIFI_PWD;
  const char *const _APP_SCRIPT_URL;
  char *const _cycleCode;

  static EventGroupHandle_t s_wifi_event_group;
  static char s_output_buffer[CONFIG_MAX_LENGTH_CYCLE_STRING];
  static uint8_t s_retry_num;

  esp_http_client_handle_t _client;

  esp_err_t _ret = ESP_ERR_INVALID_STATE;

  static void _wifiEventHandler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);

  void _wifiInitSta(void); // helper to startup WIFI

  static esp_err_t _httpEventHandler(esp_http_client_event_t *evt);

  void _connect();

  void _download();

public:
  Downloader(const char *const wifiSSID,
             const char *const wifiPWD,
             const char *const appScriptUrl,
             char *const cycleCode);

  bool _downloaded() const { return (_ret == ESP_OK) ? true : false; }
};

#endif