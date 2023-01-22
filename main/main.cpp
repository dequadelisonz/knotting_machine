#include "KnotEngine.hpp"
// #include "AnalogButton.hpp"
#include "esp_log.h"
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>

#define TAG "MAIN"

// class Test
// {
// public:
//     void up()
//     {
//         ESP_LOGI(TAG, "UP");
//     }

//     void down()
//     {
//         ESP_LOGI(TAG, "DOWN");
//     }

//     void ok()
//     {
//         ESP_LOGI(TAG, "OK");
//     }
// };

extern "C" void app_main(void)
{

    KnotEngine ke;
    ke.start();
}
