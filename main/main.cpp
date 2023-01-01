#include "Sequencer.hpp"
#include "SDDriver.hpp"
#include "Sequencer.hpp"
#include "ImporterSD.hpp"
#include "ImporterWIFI.hpp"

#define TAG "main"

extern "C" void app_main(void)
{

    ESP_LOGI(TAG, "****************START****************");
    Sequencer seq;

    ESP_LOGI(TAG, "<<<<<<<<<<<<<<<<<<<<<Reading from SD CARD...>>>>>>>>>>>>>>>>>>>>>>>>");
    ImporterSD impSD(seq);

    if (impSD.isImported())
    {
        seq.parse();
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "<<<<<<<<<<<<<<<<<<<<<Reading from WIFI...>>>>>>>>>>>>>>>>>>>>>>>>");
    ImporterWIFI impWIFI(seq);
    if (impWIFI.isImported())
    {
        seq.parse();
    }

    ESP_LOGI(TAG, "****************END****************");
}
