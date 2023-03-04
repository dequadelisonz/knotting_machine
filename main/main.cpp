#include "KnotEngine.hpp"

#define TAG "MAIN"

extern "C" void app_main(void)
{

    // const uint8_t exps[GPOutArray::MAX_EXPANSIONS] = {0x27, 0x00, 0x0, 0x0, 0x0, 0x0};
    // GPOutArray gpa(exps);

    // TickType_t delay=200/portTICK_PERIOD_MS;

    // while (1)
    // {
    //     for (int i = 1; i <= 8; ++i)
    //     {
    //         gpa.set(i,1);
    //     }
    //     vTaskDelay(delay);
    //     for (int i = 1; i <= 8; ++i)
    //     {
    //         gpa.set(i,0);
    //     }
    //     vTaskDelay(delay);
    //     for (int i = 9; i <= 16; ++i)
    //     {
    //         gpa.set(i,1);
    //     }
    //     vTaskDelay(delay);
    //     for (int i = 9; i <= 16; ++i)
    //     {
    //         gpa.set(i,0);
    //     }
    //     vTaskDelay(delay);

    //     // for (int i = 1; i <= 16; ++i)
    //     // {
    //     //     gpa.setOn(i);
    //     //     vTaskDelay(delay);
    //     //     gpa.setOff(i);
    //     //     vTaskDelay(delay);
    //     // }
    //     // gpa.setOn(1);
    //     // vTaskDelay(500 / portTICK_PERIOD_MS);
    //     // gpa.setOn(16);
    //     // vTaskDelay(500 / portTICK_PERIOD_MS);
    //     // gpa.setOff(1);
    //     // vTaskDelay(500 / portTICK_PERIOD_MS);
    //     // gpa.setOff(16);
    //     // vTaskDelay(500 / portTICK_PERIOD_MS);
    // }

    KnotEngine ke;
    ke.start();

    //ESP_LOGI(TAG,"Size of float: %d",sizeof(float));
}
