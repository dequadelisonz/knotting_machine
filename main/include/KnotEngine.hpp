#ifndef KnotEngine_hpp
#define KnotEngine_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "SequenceEngine.hpp"
#include "HMI.hpp"
#include "Sequencer.hpp"
#include "ImporterSD.hpp"
#include "ImporterWIFI.hpp"

class KnotEngine : public SequenceEngine
{

    friend class HMI;

private:
    const char *TAG = "KnotEngine";
    HMI _hmi;
    Sequencer _sequencer;

    int _batchQty = 250;
    int _pcsPerMin = 0;
    uint8_t _nomPcsPerMin = 0;
    uint8_t _runningStatus = 0;
    uint8_t _selModeStatus = 0;
    float _kSpeed = 1.0f;

public:
    KnotEngine() : _hmi(*this)
    {
        ESP_LOGI(TAG, "Creating new knotting cycle engine");
    }

    virtual bool onControlCreate()
    {
        bool ret = true;
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        return ret;
    }
    virtual bool onControlUpdate(uint64_t elapsedTime)
    {
        // ESP_LOGI(TAG,"onControlUpdate");
        _hmi.updateStatus();
        vTaskDelay(60 / portTICK_PERIOD_MS); // TODO solo per regolare refresh schermo, da rivedere con display finale
        return true;
    }

    virtual bool onSequenceCreate()
    {
        ImporterSD impSD(_sequencer);//TODO controllo sorgente ciclo, implementare messaggio errore
        bool ret = impSD.isImported();
        if (ret)
        {
            _sequencer.parse();                                              // TODO implementare un controllo di errore di parsing?
            _nomPcsPerMin = _pcsPerMin = 60 / _sequencer.getTotalDuration(); // calculate pcs/min from sequencer cycle total duration
            _kSpeed = _nomPcsPerMin / _pcsPerMin;                            // set _kSpeed proportionally to real pcs/min
        }
        return ret;
    }

    virtual bool onSequenceUpdate(uint64_t elapsedTime)
    {
        return true;
    }

    int &getBatchQty()
    {
        return _batchQty;
    }

    int &getPcsPerMin()
    {
        return _pcsPerMin;
    }
};

#endif