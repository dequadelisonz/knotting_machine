#ifndef TESTENGINE_HPP
#define TESTENGINE_HPP

/*ESP-IDF includes*/
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "driver/gpio.h"

/*This project includes*/
#include "SequenceEngine.hpp"

class TestEngine : public SequenceEngine
{
    const char *TAG = "TestEngine";

    int butStart = 1;
    int butStop = 0;
    int selModeStatus = 0;
    int runningStatus = 1;
    int stopStatus = 0;
    int selModePrev = 0;

public:
    TestEngine()
    {
        ESP_LOGI(TAG, "Creating a new TestsEngine");
        gpio_set_direction(GPIO_NUM_47, GPIO_MODE_INPUT);
        ESP_ERROR_CHECK(gpio_set_pull_mode(GPIO_NUM_47, GPIO_PULLUP_ONLY));
        gpio_set_direction(GPIO_NUM_48, GPIO_MODE_INPUT);
        ESP_ERROR_CHECK(gpio_set_pull_mode(GPIO_NUM_48, GPIO_PULLUP_ONLY));
        gpio_set_direction(GPIO_NUM_21, GPIO_MODE_INPUT);
        ESP_ERROR_CHECK(gpio_set_pull_mode(GPIO_NUM_21, GPIO_PULLUP_ONLY));
    }

    bool onControlCreate()
    {
        ESP_LOGI(TAG, "Setting up control task");
        return true;
    }

    bool onControlUpdate(float elapsedTime)
    {
        butStart = gpio_get_level(GPIO_NUM_47);
        // ESP_LOGI(TAG, "Pulsante 47 stato attuale: %d", butStart);
        butStop = gpio_get_level(GPIO_NUM_48);
        // ESP_LOGI(TAG, "Pulsante 48 stato attuale: %d", butStop);
        selModeStatus = gpio_get_level(GPIO_NUM_21);
        // ESP_LOGI(TAG, "Pulsante 21 stato attuale: %d", selModeStatus);
        return true;
    }

    bool onSequenceCreate()
    {
        ESP_LOGI(TAG, "Setting up sequence task");
        return true;
    }

    bool onSequenceUpdate(float elapsedTime)
    {
        if (butStart != runningStatus)
        {
            ESP_LOGI(TAG, "Pulsante 47 premuto, valore attuale: %d", butStart);
            runningStatus = butStart;
        }
        if (butStop != stopStatus)
        {
            ESP_LOGI(TAG, "Pulsante 48 premuto, valore attuale: %d", butStop);
            stopStatus = butStop;
        }
        if (selModeStatus != selModePrev)
        {
            ESP_LOGI(TAG, "Pulsante 21 premuto, valore attuale: %d", selModeStatus);
            selModePrev = selModeStatus;
        }
        return true;
    }
};

#endif