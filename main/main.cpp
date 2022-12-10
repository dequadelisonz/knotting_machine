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

    ESP_LOGW(TAG, "*EXECUTION END*");
}

#undef TAG
