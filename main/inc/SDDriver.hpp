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
    // const char *MOUNT_POINT = "/sdcard";
    // const char *FILE_CYCLE = "/cycle.kno";

    int _pinMiso = CONFIG_SDD_MISO_GPIO;
    int _pinMosi = CONFIG_SDD_MOSI_GPIO;
    int _pinNumClk = CONFIG_SDD_CLK_GPIO;
    int _pinNumCs = CONFIG_SDD_CS_GPIO;

    esp_vfs_fat_sdmmc_mount_config_t mount_config;

    sdmmc_card_t *card;

    sdmmc_host_t _host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config;

    bool _sDDone = false;

    esp_err_t init()
    {
        mount_config = {
            .format_if_mount_failed = false,
            .max_files = 5,
            .allocation_unit_size = 16 * 1024};

        ESP_LOGI(TAG, "Initializing SD card");

        // Use settings defined above to initialize SD card and mount FAT filesystem.
        // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
        // Please check its source code and implement error recovery when developing
        // production applications.
        ESP_LOGI(TAG, "Using SPI peripheral");

        _host.max_freq_khz = 10; //********MODIFICATO**   SDMMC_FREQ_PROBING

        spi_bus_config_t bus_cfg = {
            .mosi_io_num = _pinMosi,
            .miso_io_num = _pinMiso,
            .sclk_io_num = _pinNumClk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
        };

        esp_err_t ret = spi_bus_initialize((spi_host_device_t)(_host.slot), &bus_cfg, SDSPI_DEFAULT_DMA);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize bus.");
            return ret;
        };

        // This initializes the slot without card detect (CD) and write protect (WP) signals.
        // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
        slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        slot_config.gpio_cs = (gpio_num_t)_pinNumCs;
        slot_config.host_id = (spi_host_device_t)_host.slot;

        ESP_LOGI(TAG, "Mounting filesystem");
        ret = esp_vfs_fat_sdspi_mount(CONFIG_SDCARD_MOUNT_POINT, &_host, &slot_config, &mount_config, &card);

        if (ret != ESP_OK)
        {
            if (ret == ESP_FAIL)
            {
                ESP_LOGE(TAG, "Failed to mount filesystem. "
                              "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
            }
            else
            {
                ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                              "Make sure SD card lines have pull-up resistors in place.",
                         esp_err_to_name(ret));
            }
            return ret;
        }
        ESP_LOGI(TAG, "Filesystem mounted");

        // Card has been initialized, print its properties
        sdmmc_card_print_info(stdout, card);

        _sDDone = false;

        return ret;
    }

    void doneSD()
    {
        // All done, unmount partition and disable SPI peripheral
        esp_vfs_fat_sdcard_unmount(CONFIG_SDCARD_MOUNT_POINT, card);
        ESP_LOGI(TAG, "Card unmounted");

        // deinitialize the bus after all devices are removed
        spi_bus_free((spi_host_device_t)_host.slot);
        _sDDone = true;
    }

public:
    SDDriver()
    {
    }

    SDDriver(int pinMiso,
             int pinMosi,
             int pinNumClk,
             int pinNumCs) : _pinMiso(pinMiso),
                             _pinMosi(pinMosi),
                             _pinNumClk(pinNumClk),
                             _pinNumCs(pinNumCs)
    {
    }

    esp_err_t saveFile(const char *text, const char *fileName)
    {
        esp_err_t ret = init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to init SDCARD.");
            return ret;
        }

        ESP_LOGI(TAG, "Opening file to save %s", fileName);
        FILE *f = fopen(fileName, "w");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file for writing: %s", esp_err_to_name(ret));
            ret = ESP_FAIL;
            doneSD();
            return ret;
        }
        fprintf(f, text);
        fclose(f);
        ESP_LOGI(TAG, "File written");
        doneSD();
        return ret;
    }

    esp_err_t readFile(char *text, const char *fileName)
    {
        esp_err_t ret = init();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to init SDCARD: %s", esp_err_to_name(ret));
            doneSD();
            return ret;
        }

        char row[64] = {0};

        ESP_LOGI(TAG, "Opening file to read %s", fileName);
        FILE *f = fopen(fileName, "r");
        if (f == NULL)
        {
            ret = ESP_ERR_NOT_FOUND;
            ESP_LOGE(TAG, "Failed to open file for reading: %s", esp_err_to_name(ret));
            doneSD();
            return ret;
        }

        while (fgets(row, sizeof(row), f) != NULL) // append one row at a time in output string
        {
            // ESP_LOGI(TAG, "**Lunghezza stringa ricevente: %d\n"
            //               "**Dimensione riga estratta: %d\n"
            //               "**Dimensione stringa ricevente: %d\n"
            //               "**Stringa letta: %s",
            //          strlen(text),
            //          sizeof(row),
            //          sizeof(text),
            //          row); // only for debug
            if ((strlen(text) + strlen(row) + 1) <= CONFIG_MAX_LENGTH_CYCLE_STRING) // check if new row fits in the receiving string
                strcat(text, row);
            else // otherwise return an error
            {
                ESP_LOGE(TAG, "Size of cycle char array is too small to memorize cycle in SDCARD file: %s",
                         esp_err_to_name(ret));
                ret = ESP_FAIL;
                doneSD();
                return ret;
            }
        }
        fclose(f);
        doneSD();
        return ret;
    }

    bool testSDCard()
    {
        ESP_LOGI(TAG, "Testing SDCARD for writing");
        bool ret = false;

        esp_err_t err = init();
        if (err == ESP_OK)
        {
            doneSD();
            // try to write a file to check if SDCARD can be written
            char SDPath[32] = {0};
            strcat(SDPath, CONFIG_SDCARD_MOUNT_POINT);
            strcat(SDPath, "/TEST.KNO");
            err = saveFile("___", SDPath);
            if (err == ESP_OK)
            {
                ESP_LOGI(TAG, "Test file was successfully written.\nSDCARD is working properly.");
                // remove(SDPath);
                ret = true;
            }
        }
        else
        {
            ESP_LOGE(TAG, "SDCARD test failed, check if SDCARD is present.");
            doneSD();
        };
        return ret;
    }

    ~SDDriver()
    {
        if (!_sDDone)
        {
            doneSD();
        }
    }
};

#endif