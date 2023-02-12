#include "KnotEngine.hpp"

#define TAG "MAIN"

extern "C" void app_main(void)
{
    KnotEngine ke;
    ke.start();

    //ESP_LOGI(TAG,"Size of float: %d",sizeof(float));
}
