#include "KnotEngine.hpp"

#define TAG "MAIN"

#define WITH_THREAD

extern "C" void app_main(void)
{
    // esp_log_level_set("*", ESP_LOG_DEBUG);

    const uint8_t exps[GPOutArray::MAX_EXPANSIONS] = {0x27, 0x23, 0x0, 0x0, 0x0, 0x0};
    GPOutArray gpa(exps);

    TickType_t delay = 2000 / portTICK_PERIOD_MS;

    while (1)
    {
        gpa.set(2, 1);
        gpa.set(18, 1);
        vTaskDelay(delay);
        gpa.set(2, 0);
        gpa.set(18, 0);
        vTaskDelay(delay);
    }

    // KnotEngine ke;
    // ke.start();
}
