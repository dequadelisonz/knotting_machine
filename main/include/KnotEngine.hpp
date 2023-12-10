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

public:
    static const uint8_t MAX_CHAR_VERS = 4U;

private:
    const char *TAG = "KnotEngine";
    char _version[MAX_CHAR_VERS] = {0};

    Sequencer _sequencer;
    HMI _hmi;
    uint64_t _timer = 0U;

    bool _isAtBegin = true;
    bool _mutexReady = true;

    uint8_t _run = 0U;
    uint8_t _runningStatus = 0U; // used to activate or deactivate HMI statuses
    uint8_t _selModeStatus = 0U;
    static pthread_mutex_t _cycleStatusM;

    const uint8_t _expAddresses[GPOutArray::MAX_EXPANSIONS] = {0x27U, 0x23U, 0x0U, 0x0U, 0x0U, 0x0U}; // TODO inserire in menuconfig
    GPOutArray _gpOutArr;

    // const TickType_t delay = 2000 / portTICK_PERIOD_MS; // TODO rimuovere dopo test

    uint16_t _batchQty = 100U;
    uint16_t _processedItems = 0;
    uint8_t _pcsPerMin = 0U;
    uint8_t _nomPcsPerMin = 0U;
    const uint8_t _minPcsPerMin = 1U;

    uint64_t _curDuration = 0U;
    float _curOffset = 0.0f,
          _kSpeed = 1.0f;

    bool _updateFromSD();

    bool _updateFromWIFI();

public:
    KnotEngine(const char *ver);

    /*running on control thread*/
    virtual bool onControlCreate()
    {
        return _mutexReady;
    }
    virtual bool onControlUpdate(uint64_t elapsedTime)
    {
        _hmi.updateStatus();
        return _mutexReady;
    }

    /*running on sequence thread*/
    virtual bool onSequenceCreate()
    {
        _hmi.splashScreen();
        return (this->_updateFromSD() && _mutexReady);
    }

    virtual bool onSequenceUpdate(uint64_t elapsedTime)
    {
        switch (getSelMode())
        {
        case 0U:
            if (getRunStatus() == 1U)
            {
                while (_curOffset == _sequencer.getCurOffset()) // then cycle through a group to perform passes
                {
                    printf("id: %d,\tdescr: %s,\tpin: %d\tstatus: %s\n",
                           _sequencer.getCurId(),
                           _sequencer.getCurDescription(),
                           _sequencer.getCurPin(),
                           _sequencer.getCurStatus() ? "on" : "off");
                    _gpOutArr.set(_sequencer.getCurPin(), _sequencer.getCurStatus());
                    _sequencer.advance();
                }
                _curOffset = _sequencer.getCurOffset(); // set the current offset ready for the next group of passes
                setRunStatus(0U);                       // set _run to 0 after performing the pass
            }
            break;

        case 1U:
            // if running status is no more 1 and cycle arrives at the beginning (!_isAtBegin == 0) then stop the cycle because user pushed the stop button
            if (getRunStatus() || !_isAtBegin)
            {
                // printf("_run: %d\t_isAtBegin: %s\t_remQty: %d\n", _getRunStatus(), _isAtBegin ? "true" : "false", _processedItems);
                if (_timer <= _curDuration)
                {
                    while (_curOffset == _sequencer.getCurOffset())
                    {
                        printf("id: %d,\tdescr: %s,\tpin: %d\tstatus: %s\n",
                               _sequencer.getCurId(),
                               _sequencer.getCurDescription(),
                               _sequencer.getCurPin(),
                               _sequencer.getCurStatus() ? "on" : "off");
                        _gpOutArr.set(_sequencer.getCurPin(), _sequencer.getCurStatus());
                        _isAtBegin = _sequencer.advance();
                        /*
                        After processing a group of passes, if cycle is at begin position
                        then increase processed item and if difference with _batchQty
                        (which may change over time) is =0 then stop automatic cycle
                        and reset _processedItems to 0
                        */
                        if (_isAtBegin)
                        {
                            ++_processedItems;
                            _hmi.setRePaintStatus(true);
                            if ((_batchQty - _processedItems) == 0)
                            {
                                // TODO mettere qui codice gestione sirena
                                setRunStatus(0U);
                                _processedItems = 0U;
                            }
                        }
                    };
                }
                else
                {
                    _timer = 0U;
                    _curDuration = (uint64_t)(_sequencer.getCurDuration() * 1000000U * getKspeed());
                    _curOffset = _sequencer.getCurOffset();
                }

                // ESP_LOGI(TAG, "Running status: %d\t isAtBegin: %d", _run, _isAtBegin);
            }
            else
            {
                // if getRunStatus() = 0 and _isAtBegin=0 then set _runningStatus to 0
                setRunningStatus(0U);
            }
            break;
        }

        _timer += elapsedTime;

        return _mutexReady;
    }

    void setRunStatus(uint8_t status);

    uint8_t getRunStatus() const { return _run; }

    void setRunningStatus(uint8_t status);

    uint8_t getRunningStatus() const { return _runningStatus; }

    void setSelMode(uint8_t status);

    uint8_t getSelMode() const { return _selModeStatus; }

    uint16_t &getBatchQty() { return _batchQty; }

    uint16_t const &getProcessedItems() const { return _processedItems; }

    uint8_t &getPcsPerMin() { return _pcsPerMin; }

    const uint8_t getNomPcsPerMin() const { return _nomPcsPerMin; }

    const uint8_t getMinPcsPerMin() const { return _minPcsPerMin; }

    float &getKspeed() { return _kSpeed; }
};

#endif
