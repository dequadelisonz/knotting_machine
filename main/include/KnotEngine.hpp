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
#include "GPOut.hpp"

class KnotEngine : public SequenceEngine
{

    friend class HMI;

private:
    const char *TAG = "KnotEngine";
    HMI _hmi;
    Sequencer _sequencer;
    uint64_t _timer = 0;

    bool _isAtBegin = true;
    uint8_t _runningStatus = 0;
    uint8_t _selModeStatus = 0;

    const int _expAddresses[GPOutArray::MAX_EXPANSIONS] = {0x27, 0x00, 0x0, 0x0, 0x0, 0x0}; // TODO inserire in menuconfig
    GPOutArray _gpOutArr;

    const TickType_t delay = 2000 / portTICK_PERIOD_MS; // TODO rimuovere dopo test

    uint16_t _batchQty = 250;
    uint8_t _pcsPerMin = 0;
    uint8_t _nomPcsPerMin = 0;
    const uint8_t _minPcsPerMin = 1;

    float _curDuration = 0.0f,
          _curOffset = 0.0f,
          _kSpeed = 1.0f;

    bool _updateFromSD();

    bool _updateFromWIFI();

public:
    KnotEngine();

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

        switch (_selModeStatus)
        {
        case 0:
            if (_runningStatus)
            {
                while (_curOffset == _sequencer.getCurOffset()) // then cycle through a group to perform passes
                {
                    printf("pin: %d\tstatus: %s\n", _sequencer.getCurPin(), _sequencer.getCurStatus() ? "on" : "off");
                    _sequencer.advance();
                }
                _curOffset = _sequencer.getCurOffset(); // set the current offset ready for the next group of passes
                // vTaskDelay(300 / portTICK_PERIOD_MS);
                _runningStatus = 0; // set _runningStatus to 0 after performing the pass
            }

            break;
        case 1:

            // _gpOutArr.setOn(1);
            // vTaskDelay(200 / portTICK_PERIOD_MS);
            // _gpOutArr.setOff(1);
            // vTaskDelay(200 / portTICK_PERIOD_MS);

            break;
        }

        _timer += elapsedTime;

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