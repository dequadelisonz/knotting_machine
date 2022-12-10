#ifndef KNOTCYCLE_HPP
#define KNOTCYCLE_HPP

/*ESP-IDF includes*/
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/*This project includes*/
#include "SequenceEngine.hpp"
#include "Sequencer.hpp"
#include "LedDriver.hpp"

class KnotCycle : public SequenceEngine
{

private:
    const char *TAG = "KnotCycle";

    Sequencer _sequencer; // KnotCycle owns a sequencer

    float _curDuration, _curOffset, kSpeed = 1.0f; // TODO think something to drive kSpeed from interface
    uint64_t _timer = 0;
    // bool _fired = false;
    bool _isAtBegin = true;

    LedDriver _ledDriver; // TODO make LedDriver owned by a global object

    unsigned char runningStatus = 0; // GPIO47
    // unsigned char stopStatus = 0;    // GPIO48
    unsigned char selModeStatus = 0; // GPIO21

public:
    KnotCycle()
    {
        ESP_LOGI(TAG, "Creating knot cycle....");

        ESP_ERROR_CHECK(_sequencer.init());

        _sequencer.parse();

        //_sequencer.advance();

        // TODO - add GPIO/BUTTON mapping in SDKCONFIG
        gpio_set_direction(GPIO_NUM_47, GPIO_MODE_INPUT); // Start Button
        ESP_ERROR_CHECK(gpio_set_pull_mode(GPIO_NUM_47, GPIO_PULLUP_ONLY));
        gpio_set_direction(GPIO_NUM_48, GPIO_MODE_INPUT); // Stop Button
        ESP_ERROR_CHECK(gpio_set_pull_mode(GPIO_NUM_48, GPIO_PULLUP_ONLY));
        gpio_set_direction(GPIO_NUM_21, GPIO_MODE_INPUT); // mode selector
        ESP_ERROR_CHECK(gpio_set_pull_mode(GPIO_NUM_21, GPIO_PULLUP_ONLY));
    }

    bool onControlCreate()
    {
        // ESP_LOGI(TAG, "Setting up control task");
        return true;
    }

    bool onControlUpdate()
    {
        if (!gpio_get_level(GPIO_NUM_47))
            runningStatus = 1;
        if (gpio_get_level(GPIO_NUM_48))
        {
            runningStatus = 0;
            //_isAtBegin =  true;
        }
        selModeStatus = gpio_get_level(GPIO_NUM_21);
        if (selModeStatus == 0)
            _isAtBegin = true;
        // printf("Is begin status: %s",_isAtBegin?"TRUE":"FALSE");
        return true;
    }

    bool onSequenceCreate()
    {
        // ESP_LOGI(TAG, "onSequenceCreate....");
        return true;
    }

    bool onSequenceUpdate(float elapsedTime)
    {
        switch (selModeStatus)
        {
        case 0:
            if (runningStatus)
            {

                gpio_num_t led = (gpio_num_t)_sequencer.getCurPass()->getPin();
                uint32_t status = (uint32_t)_sequencer.getCurPass()->getStatus();
                //_sequencer.getCurPass()->logContent();
                _ledDriver.setLed(led, status);
                _sequencer.advance();

                vTaskDelay(300 / portTICK_PERIOD_MS);
                runningStatus = 0;
            }
            break;
        case 1:

            if (runningStatus || !_isAtBegin)
            {
                if (_timer <= _curDuration)
                {
                    while (_curOffset == _sequencer.getCurOffset())
                    {
                        gpio_num_t led = (gpio_num_t)_sequencer.getCurPass()->getPin();
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

        // if (!_fired)
        // {
        //     while (_curOffset == _sequencer.getCurOffset())
        //     {
        //         // ESP_LOGI(TAG, "\nCurrent duration (s): %3.2f", _curDuration);
        //         //_sequencer.getCurPass().logContent();
        //         // printf("TS: %lld\n", esp_timer_get_time());
        //         gpio_num_t led = (gpio_num_t)_sequencer.getCurPass()->getPin();
        //         uint32_t status = (uint32_t)_sequencer.getCurPass()->getStatus();
        //         _ledDriver.setLed(led, status);
        //         _isAtBegin = _sequencer.advance();
        //     };
        //     _curOffset = _sequencer.getCurOffset();
        //     _fired = true;
        //     _timer = 0;
        // }

        // if (_timer >= _curDuration) // converting group duration from secs to millisecs
        // {
        //     // printf("New Group\n");
        //     _fired = false;
        //     _curDuration = _sequencer.getCurDuration() * 1000000 * kSpeed;
        // }

        return true;
    }
};

#endif