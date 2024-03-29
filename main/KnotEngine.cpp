#include "KnotEngine.hpp"

pthread_mutex_t KnotEngine::_cycleStatusM = 0;

KnotEngine::KnotEngine(const char *ver) : _hmi(*this), _gpOutArr(_expAddresses)
{
    strncat(_version, ver, MAX_CHAR_VERS - 1);
    if (pthread_mutex_init(&KnotEngine::_cycleStatusM, NULL) != 0)
    {
        ESP_LOGE(TAG, "Failed to init mutex");
        _mutexReady = false;
    }
}

bool KnotEngine::_updateFromSD()
{
    _hmi.printStatus("", "Caricam.SDD...");
    ImporterSD impSD(_sequencer); // TODO controllo sorgente ciclo, implementare messaggio errore
    bool ret = impSD.isImported();
    if (ret)
    {
        _sequencer.parse();                                              // TODO implementare un controllo di errore di parsing?
        _nomPcsPerMin = _pcsPerMin = 60 / _sequencer.getTotalDuration(); // calculate pcs/min from sequencer cycle total duration
        _kSpeed = _nomPcsPerMin / _pcsPerMin;
        _hmi.printStatus("", "Pronto!"); // set _kSpeed proportionally to real pcs/min
    }
    else
        _hmi.printStatus("", "Err.caricam.!");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    return ret;
}

bool KnotEngine::_updateFromWIFI()
{

    _hmi.printStatus("", "Caricam.WIFI...");
    ImporterWIFI impWIFI(_sequencer); // TODO controllo sorgente ciclo, implementare messaggio errore
    bool ret = impWIFI.isImported();
    if (ret)
    {
        _sequencer.parse();                                              // TODO implementare un controllo di errore di parsing?
        _nomPcsPerMin = _pcsPerMin = 60 / _sequencer.getTotalDuration(); // calculate pcs/min from sequencer cycle total duration
        _kSpeed = _nomPcsPerMin / _pcsPerMin;
        _hmi.printStatus("", "Pronto!"); // set _kSpeed proportionally to real pcs/min
    }
    else
        _hmi.printStatus("", "Err.caricam.!");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    return ret;
}

void KnotEngine::setRunStatus(uint8_t status)
{
    // ESP_LOGI(TAG, "Set run status to %d from thread:%s\n", status, pcTaskGetName(nullptr));
    if (pthread_mutex_lock(&_cycleStatusM) == 0)
    {
        _run = status;
        // Unlock once operations are done
        pthread_mutex_unlock(&_cycleStatusM);
        if (_run == 1U)
            setRunningStatus(1U);
        _hmi.setRePaintStatus(true);
    }
}

void KnotEngine::setRunningStatus(uint8_t status)
{
    if ((getRunningStatus() != status) && (pthread_mutex_lock(&_cycleStatusM) == 0))
    {
        _runningStatus = status;
        pthread_mutex_unlock(&_cycleStatusM);
    }
}

void KnotEngine::setSelMode(uint8_t status)
{
    ESP_LOGI(TAG, "Set selector mode to %d from thread:%s\n", status, pcTaskGetName(nullptr));
    if (pthread_mutex_lock(&_cycleStatusM) == 0)
    {
        _selModeStatus = status;
        // Unlock once operations are done
        pthread_mutex_unlock(&_cycleStatusM);
    }
}
