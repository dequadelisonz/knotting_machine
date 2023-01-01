#include "ImporterSD.hpp"

ImporterSD::ImporterSD(Sequencer &sequencer) : Importer(sequencer)
{
    printf("Constructing an importerSD...\n");
    _isImported = _readSD();
    if (isImported())
        _syncCycle();
}

bool ImporterSD::_readSD()
{
    printf("ImporterSD _readSD.\n");
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    if (_isInit)
        ret = _sd.readFile(_cycleCode, _cycleFileFullPath);
    return (ret == ESP_OK) ? true : false;
}