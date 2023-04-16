#include "ImporterSD.hpp"

ImporterSD::ImporterSD(Sequencer &sequencer) : Importer(sequencer)
{
    ESP_LOGI(TAG,"Initializing an importer SD...");
    _isImported = _readSD();
    if (isImported())
        _syncCycle();
}

bool ImporterSD::_readSD()
{
    ESP_LOGI(TAG,"Reading SD card...");
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    if (_isInit)
        ret = _sd.readFile(_cycleCode, _cycleFileFullPath);
    return (ret == ESP_OK) ? true : false;
}