/*ESP-IDF includes*/

#include "esp_err.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "inc/LedDriver.hpp"
#include "sdkconfig.h"

/*This project includes*/
// #include "inc/Sequencer.hpp"
// #include "inc/TestEngine.hpp"
#include "inc/KnotCycle.hpp"

#define TAG "MAIN"

extern "C" void app_main(void)
{
    ESP_LOGW(TAG, "*******START******");

    KnotCycle knotCycle;

    knotCycle.start();

    // printf("\033[2J"); /*  clear the screen  */
    // printf("\033[H");  /*  position cursor at top-left corner */
    // for (int i = 0; i < 10; ++i)
    // {
    //     printf("stampo numero: %d\n", i);
    //     vTaskDelay(100);
    //     printf("\033[2J"); /*  clear the screen  */
    //     printf("\033[H");  /*  position cursor at top-left corner */
    // }
    // printf("\n");

    // Keyboard keyboard;

    // Keyboard::Button keystroke;

    // while (1)
    // {
    //     keyboard.updateButtonStatus();
    //     printf("Stato pulsante DOWN: %d\n", keyboard.getButtonStatus(Keyboard::DOWN));
    //     printf("Stato pulsante UP: %d\n", keyboard.getButtonStatus(Keyboard::UP));
    //     printf("Stato pulsante OK: %d\n", keyboard.getButtonStatus(Keyboard::OK));
    //     // printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    //     // printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    //     // printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    //     vTaskDelay(100 / portTICK_PERIOD_MS);
    // }

    ESP_LOGW(TAG, "*EXECUTION END*");
}

#undef TAG
