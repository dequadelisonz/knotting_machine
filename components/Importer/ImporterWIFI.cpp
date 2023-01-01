#include "ImporterWIFI.hpp"

ImporterWIFI::ImporterWIFI(Sequencer &sequencer) : Importer(sequencer)
{
    printf("Constructing an importerWIFI...\n");
    if (_isInit)
    {
        _readConnInfo();
        _isImported = _download();
        if (isImported())
        {
            _syncCycle();
            _sd.saveFile(_cycleCode, _cycleFileFullPath);
        }
    }
}

void ImporterWIFI::_readConnInfo()
{
    ESP_LOGI(TAG, "Reading connection info for WIFI connection.");
    char SSIDInfo[512] = {0};
    char SSIDPath[32] = {0};
    strcat(SSIDPath, CONFIG_SDCARD_MOUNT_POINT);
    strcat(SSIDPath, CONFIG_WIFI_DATA_FILENAME);
    esp_err_t err = _sd.readFile(SSIDInfo, SSIDPath);
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
    // ESP_LOGI(TAG, "SSID: %s\nPWD: %s\nURL: %s", _WIFI_SSID, _WIFI_PWD, _APP_SCRIPT_URL);
}

bool ImporterWIFI::_download()
{
    if (_isInit)
    {
        Downloader dl(this->_WIFI_SSID,
                      this->_WIFI_PWD,
                      this->_APP_SCRIPT_URL,
                      this->_cycleCode);
        return dl._downloaded();
    }
    else
    {
        return false;
    }
}