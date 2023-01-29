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

    uint16_t _batchQty = 250;
    uint8_t _pcsPerMin = 0;
    uint8_t _nomPcsPerMin = 0;
    const uint8_t _minPcsPerMin = 1;

    uint8_t _runningStatus = 0;
    uint8_t _selModeStatus = 0;
    float _kSpeed = 1.0f;

    bool _updateFromSD();

    bool _updateFromWIFI();

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
        // vTaskDelay(10 / portTICK_PERIOD_MS); // TODO solo per regolare refresh schermo, da rivedere con display finale
        return true;
    }

    virtual bool onSequenceCreate()
    {
        return _updateFromSD();
    }

    virtual bool onSequenceUpdate(uint64_t elapsedTime)
    {
        return true;
    }

    uint16_t &getBatchQty()
    {
        return _batchQty;
    }

    uint8_t &getPcsPerMin()
    {
        return _pcsPerMin;
    }

    const uint8_t getNomPcsPerMin() const
    {
        return _nomPcsPerMin;
    }

    const uint8_t getMinPcsPerMin() const
    {
        return _minPcsPerMin;
    }

    float &getKspeed()
    {
        return _kSpeed;
    }
};

#endif