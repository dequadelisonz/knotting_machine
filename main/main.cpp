#include "KnotEngine.hpp"

#define TAG "MAIN"

#define WITH_THREAD

extern "C" void app_main(void)
{
    // esp_log_level_set("*", ESP_LOG_DEBUG);
    KnotEngine ke("1.0");
    ke.start();
}
