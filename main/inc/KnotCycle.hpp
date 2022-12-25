#ifndef KNOTCYCLE_HPP
#define KNOTCYCLE_HPP

/*ESP-IDF includes*/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/*This project includes*/
#include "SequenceEngine.hpp"
#include "Sequencer.hpp"
#include "LedDriver.hpp"
#include "Keyboard.hpp"
#include "HMI.hpp"

class KnotCycle : public SequenceEngine
{

    friend class HMI;

private:
    const char *TAG = "KnotCycle";

    Sequencer _sequencer; // KnotCycle owns a sequencer

    float _curDuration,
        _curOffset,
        kSpeed = 1.0f; // TODO think something to drive kSpeed from interface
    int _prod, _maxProd = 500, _batch = 250;

    uint64_t _timer = 0;
    bool _isAtBegin = true;

    LedDriver _ledDriver; // TODO make LedDriver owned by a global object

    unsigned char _runningStatus = 0;
    unsigned char _selModeStatus = 0;

    HMI _hmi;

public:
    KnotCycle() : _hmi(*this, _maxProd)
    {
        ESP_LOGI(TAG, "Creating knot cycle....");

        ESP_ERROR_CHECK(_sequencer.init());

        _sequencer.parse();

        _maxProd = 3600 / _sequencer.getTotalDuration();
        _prod = _maxProd;

        // ESP_LOGI(TAG, "Hourly production got from SDCARD: %d", _prod);

        // _hmi.setMaxProd(_prod);
        // _hmi.setBatch(_batch);
    }

    bool onControlCreate()
    {
        // ESP_LOGI(TAG, "Setting up control task");
        return true;
    }

    bool onControlUpdate(float elapsedTime)
    {
        _hmi.updateHMI(elapsedTime);

        return true;
    }

    bool onSequenceCreate()
    {
        // ESP_LOGI(TAG, "onSequenceCreate....");
        return true;
    }

    bool onSequenceUpdate(float elapsedTime)
    {
        switch (_selModeStatus)
        {
        case 0:
            if (_runningStatus) // if _runningStatus = 1
            {
                while (_curOffset == _sequencer.getCurOffset()) // then cycle through a group to perform passes
                {
                    gpio_num_t led = (gpio_num_t)_sequencer.getCurPass()->getPin(); // TODO put this into a LedDriver method
                    uint32_t status = (uint32_t)_sequencer.getCurPass()->getStatus();
                    //_sequencer.getCurPass()->logContent();
                    _ledDriver.setLed(led, status);
                    _sequencer.advance();
                }
                _curOffset = _sequencer.getCurOffset(); // set the current offset ready for the next group of passes
                vTaskDelay(300 / portTICK_PERIOD_MS);
                _runningStatus = 0;
            }
            break;
        case 1:
            // if running status is no more 1 and cycle arrives at the beginning (!_isAtBegin == 0) then stop the cycle because user pushed the stop button
            if (_runningStatus || !_isAtBegin)
            {
                if (_timer <= _curDuration)
                {
                    while (_curOffset == _sequencer.getCurOffset())
                    {
                        gpio_num_t led = (gpio_num_t)_sequencer.getCurPass()->getPin(); // TODO put this into a LedDriver method
                        uint32_t status = (uint32_t)_sequencer.getCurPass()->getStatus();
                        //_sequencer.getCurPass()->logContent();
                        _ledDriver.setLed(led, status);
                        _isAtBegin = _sequencer.advance();
                    };
                }
                else
                {
                    _timer = 0;
                    _curDuration = _sequencer.getCurDuration() * 1000000 * kSpeed;
                    _curOffset = _sequencer.getCurOffset();
                }
            }
            break;
        }
        _timer += elapsedTime;

        return true;
    }

    // int getProd() const
    // {
    //     return _prod;
    // }

    // void setMaxProd(int prod)
    // {
    //     _prod = prod;
    // }

    // int getBatch() const
    // {
    //     return _batch;
    // }

    // void setBatch(int batch)
    // {
    //     _batch = batch;
    // }
};

#endif