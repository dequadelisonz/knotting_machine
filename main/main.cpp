#include "KnotEngine.hpp"
#include "Actuator.hpp"

#define TAG "MAIN"

extern "C" void app_main(void)
{

    ESP_LOGI(TAG,"Size of Actuator: %d",sizeof(Actuator));
    ESP_LOGI(TAG,"Size of Functor: %d",sizeof(Functor<Actuator>));
    ESP_LOGI(TAG,"Size of RemoteInputBase: %d",sizeof(RemoteInputBase));
    ESP_LOGI(TAG,"Size of RemoteInput: %d",sizeof(RemoteInput<Actuator>));
    ESP_LOGI(TAG,"Size of DigitalInput: %d",sizeof(DigitalInput<Actuator>));

    // esp_log_level_set("*", ESP_LOG_DEBUG);

    // KnotEngine ke;
    // ke.start();
}
