#ifndef ImporterWIFI_hpp
#define ImporterWIFI_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "Importer.hpp"
#include "Downloader.hpp"

class ImporterWIFI : public Importer
{
private:
    const char *TAG = "ImporterWIFI";

    char _WIFI_SSID[Downloader::_WIFI_SSID_LEN] = {0};
    char _WIFI_PWD[Downloader::_WIFI_PWD_LEN] = {0};
    char _APP_SCRIPT_URL[Downloader::_APP_SCR_LEN] = {0};

    void _readConnInfo();
    bool _download();

public:
    ImporterWIFI(Sequencer &sequencer);

    ~ImporterWIFI()
    {
        ESP_LOGI(TAG, "\nDownloaded cycle: \n%s\n", _cycleCode);
    }
};

#endif