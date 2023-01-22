#include "Downloader.hpp"

Downloader::Downloader(const uint8_t (&wifiSSID)[_WIFI_SSID_LEN],
                       const uint8_t (&wifiPWD)[_WIFI_PWD_LEN],
                       const char (&appScriptUrl)[_APP_SCR_LEN],
                       char (&cycleCode)[CONFIG_MAX_LENGTH_CYCLE_STRING]) : _WIFI_SSID(wifiSSID),
                                                                            _WIFI_PWD(wifiPWD),
                                                                            _APP_SCRIPT_URL(appScriptUrl),
                                                                            _cycleCode(cycleCode)
{
    // memcpy(_WIFI_SSID, wifiSSID, strlen(wifiSSID) + 1);
    // memcpy(_WIFI_PWD, wifiPWD, strlen(wifiPWD) + 1);
    // memcpy(_APP_SCRIPT_URL, appScriptUrl, strlen(appScriptUrl) + 1);
    // ESP_LOGI(TAG, "SSID: %s\nPWD: %s\nURL: %s", _WIFI_SSID, _WIFI_PWD, _APP_SCRIPT_URL);
    _connect();
    _download();
}

void Downloader::_wifi_init_sta(void) // helper to startup WIFI
{
    s_wifi_event_group = xEventGroupCreate();

    _ret = esp_netif_init();

    if (_ret == ESP_OK)
    {
        _ret = esp_event_loop_create_default();

        if (_ret == ESP_OK)
        {
            esp_netif_create_default_wifi_sta();
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

            _ret = esp_wifi_init(&cfg);

            if (_ret == ESP_OK)
            {
                esp_event_handler_instance_t instance_any_id;
                esp_event_handler_instance_t instance_got_ip;
                _ret = esp_event_handler_instance_register(WIFI_EVENT,
                                                           ESP_EVENT_ANY_ID,
                                                           &_wifi_event_handler,
                                                           NULL,
                                                           &instance_any_id);
                if (_ret == ESP_OK)
                {
                    _ret = esp_event_handler_instance_register(IP_EVENT,
                                                               IP_EVENT_STA_GOT_IP,
                                                               &_wifi_event_handler,
                                                               NULL,
                                                               &instance_got_ip);
                    if (_ret == ESP_OK)
                    {
                        wifi_config_t wifi_config = {
                            .sta = {
                                {},
                                {},// CONFIG_ESP_WIFI_PASSWORD,
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
                                    ESP_LOGI(TAG, "_wifi_init_sta finished.");

                                    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
                                     * number of re-tries (WIFI_FAIL_BIT). The bits are set by _wifi_event_handler() (see above) */
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

void Downloader::_connect()
{
    // Initialize NVS
    _ret = nvs_flash_init();
    if (_ret == ESP_ERR_NVS_NO_FREE_PAGES || _ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        _ret = nvs_flash_init();
    }
    if (_ret == ESP_OK)
    {
        ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
        _wifi_init_sta(); // set Downloader::_ret depending on result of wifi connection

        if (_ret == ESP_OK)
        {
            esp_http_client_config_t config = {
                .url = CONFIG_APPSCRIPT_URL,
                .event_handler = _http_event_handler,
                .crt_bundle_attach = esp_crt_bundle_attach};
            ESP_LOGI(TAG, "Initing _client.");
            _client = esp_http_client_init(&config);
            ESP_LOGI(TAG, "_client initialized.");
        }
        else
        {
            ESP_LOGE(TAG, "Error _wifi_init_sta: %s", esp_err_to_name(_ret));
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
        printf("Dentro _download....\n\n\n\n\n");
        // TODO da sistemare dopo aver capito il problema con la connessione
        // ESP_ERROR_CHECK(esp_http_client_perform(_client));
        _ret = esp_http_client_perform(_client);

        if (_ret == ESP_OK)
        {
            int sc = esp_http_client_get_status_code(_client);
            ESP_LOGI(TAG, "HTTP chunk encoding Status = %d, content_length = %lld",
                     sc,
                     esp_http_client_get_content_length(_client));
            strcpy(_cycleCode, s_output_buffer);
            // ESP_LOGI(TAG, "Downloaded cycle: %s\n", _cycleCode);
        }
        else
        {
            ESP_LOGE(TAG, "Error perform http request: %s", esp_err_to_name(_ret));
            _cycleCode[0] = '\0'; // empty _cycleString if download went wrong
        }
        esp_http_client_cleanup(_client);
        esp_wifi_disconnect();
        esp_wifi_stop();
    }
}
