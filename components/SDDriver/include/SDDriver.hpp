#ifndef SDDRIVER_HPP
#define SDDRIVER_HPP

/*STD library includes*/
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

/*ESP-IDF includes*/
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"

class SDDriver
{
private:
    const char *TAG = "SDDriver";

    int _pinMiso = CONFIG_SDD_MISO_GPIO;
    int _pinMosi = CONFIG_SDD_MOSI_GPIO;
    int _pinNumClk = CONFIG_SDD_CLK_GPIO;
    int _pinNumCs = CONFIG_SDD_CS_GPIO;

    esp_vfs_fat_sdmmc_mount_config_t mount_config;

    sdmmc_card_t *card;

    sdmmc_host_t _host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config;

    bool _isInit = false;

    esp_err_t _init();

    void doneSD();

public:
    SDDriver();

    SDDriver(int pinMiso,
             int pinMosi,
             int pinNumClk,
             int pinNumCs);

    esp_err_t saveFile(const char *text, const char *fileName);

    esp_err_t readFile(char *text, const char *fileName);

    bool testSDCard();

    bool getIsInit() const { return _isInit; }

    ~SDDriver();
};


#endif