#ifndef LEDDRIVER_HPP
#define LEDDRIVER_HPP

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"

class LedDriver
{
private:
    const char *TAG = "LedDriver";

    static const int NR_OF_LED = 5;

    gpio_num_t _ledArray[NR_OF_LED] =
        {
            GPIO_NUM_4,
            GPIO_NUM_5,
            GPIO_NUM_6,
            GPIO_NUM_7,
            GPIO_NUM_15};

public:
    LedDriver()
    {
        for (int i = 0; i < NR_OF_LED; ++i)
        {
            gpio_pad_select_gpio(_ledArray[i]);
            gpio_set_direction(_ledArray[i], GPIO_MODE_OUTPUT);
        }

        blinkTest();
    }

    void setLed(gpio_num_t ledNr, uint32_t status)
    {
        gpio_set_level(ledNr, status);
    }

    void blinkTest()
    {
        ESP_LOGI(TAG, "Testing led array...");
        for (int i = 0; i < NR_OF_LED; ++i)
        {
            gpio_set_level(_ledArray[i], 1);
            vTaskDelay(200 / portTICK_PERIOD_MS);
            gpio_set_level(_ledArray[i], 0);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
};

#endif