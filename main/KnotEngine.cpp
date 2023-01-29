#include "KnotEngine.hpp"

bool KnotEngine::_updateFromSD()
{
    _hmi.freezeMenu();
    _hmi.printStatus("Caricam.SDD...");
    ImporterSD impSD(_sequencer); // TODO controllo sorgente ciclo, implementare messaggio errore
    bool ret = impSD.isImported();
    if (ret)
    {
        _sequencer.parse();                                              // TODO implementare un controllo di errore di parsing?
        _nomPcsPerMin = _pcsPerMin = 60 / _sequencer.getTotalDuration(); // calculate pcs/min from sequencer cycle total duration
        _kSpeed = _nomPcsPerMin / _pcsPerMin;
        _hmi.printStatus("Pronto!"); // set _kSpeed proportionally to real pcs/min
    }
    else
        _hmi.printStatus("Err.caricam.!");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    _hmi.unFreezeMenu();
    return ret;
}

bool KnotEngine::_updateFromWIFI()
{
    _hmi.freezeMenu();
    _hmi.printStatus("Caricam.WIFI...");
    ImporterWIFI impWIFI(_sequencer); // TODO controllo sorgente ciclo, implementare messaggio errore
    bool ret = impWIFI.isImported();
    if (ret)
    {
        _sequencer.parse();                                              // TODO implementare un controllo di errore di parsing?
        _nomPcsPerMin = _pcsPerMin = 60 / _sequencer.getTotalDuration(); // calculate pcs/min from sequencer cycle total duration
        _kSpeed = _nomPcsPerMin / _pcsPerMin;
        _hmi.printStatus("Pronto!"); // set _kSpeed proportionally to real pcs/min
    }
    else
        _hmi.printStatus("Err.caricam.!");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    _hmi.unFreezeMenu();
    return ret;
}