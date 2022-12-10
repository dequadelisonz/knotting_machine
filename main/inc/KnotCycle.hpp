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


    const gpio_num_t startGPIO=(gpio_num_t)CONFIG_START_BUTTON_GPIO;
    const gpio_num_t stopGPIO=(gpio_num_t)CONFIG_STOP_BUTTON_GPIO;
    const gpio_num_t selGPIO=(gpio_num_t)CONFIG_MODE_SELECTOR_GPIO;

    unsigned char runningStatus = 0; // GPIO47
    // unsigned char stopStatus = 0;    // GPIO48
    unsigned char selModeStatus = 0; // GPIO21

public:
    KnotCycle()
    {
        ESP_LOGI(TAG, "Creating knot cycle....");

        ESP_ERROR_CHECK(_sequencer.init());

        _sequencer.parse();

        // Change GPIO mapping for control push buttons and selector
        gpio_set_direction(startGPIO, GPIO_MODE_INPUT); // Start Button
        ESP_ERROR_CHECK(gpio_set_pull_mode(startGPIO, GPIO_PULLUP_ONLY));
        gpio_set_direction(stopGPIO, GPIO_MODE_INPUT); // Stop Button
        ESP_ERROR_CHECK(gpio_set_pull_mode(stopGPIO, GPIO_PULLUP_ONLY));
        gpio_set_direction(selGPIO, GPIO_MODE_INPUT); // mode selector
        ESP_ERROR_CHECK(gpio_set_pull_mode(selGPIO, GPIO_PULLUP_ONLY));
    }

    bool onControlCreate()
    {
        // ESP_LOGI(TAG, "Setting up control task");
        return true;
    }

    bool onControlUpdate()
    {
        if (!gpio_get_level(startGPIO))
            runningStatus = 1; // if start button (NO) is read as 1 set running status as 1
        if (gpio_get_level(stopGPIO))
            runningStatus = 0; // if stop button (NC) is read as 1 set running status as 0

        // selector in mode 0 is for step by step manual mode
        // selector in mode 1 is for automatic advancing

        selModeStatus = gpio_get_level(selGPIO); // set selector mode
        if (selModeStatus == 0)
            _isAtBegin = true; // if selector is set to manual mode set begin status as true to prevent automatic restar if selector is set back to 1
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
            if (runningStatus) //if runningStatus = 1
            {
                while (_curOffset == _sequencer.getCurOffset()) //then cycle through a group to perform passes
                {
                    gpio_num_t led = (gpio_num_t)_sequencer.getCurPass()->getPin();
                    uint32_t status = (uint32_t)_sequencer.getCurPass()->getStatus();
                    //_sequencer.getCurPass()->logContent();
                    _ledDriver.setLed(led, status);
                    _sequencer.advance();
                }
                _curOffset = _sequencer.getCurOffset();//set the current offset ready for the next group of passes
                vTaskDelay(300 / portTICK_PERIOD_MS);
                runningStatus = 0;
            }
            break;
        case 1:
            // if running status is no more 1 and cycle arrives at the beginning (!_isAtBegin == 0) then stop the cycle because user pushed the stop button
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

        return true;
    }
};

#endif