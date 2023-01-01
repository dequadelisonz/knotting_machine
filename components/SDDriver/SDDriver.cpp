#include "SDDriver.hpp"

SDDriver::SDDriver()
{
}

SDDriver::SDDriver(int pinMiso,
                   int pinMosi,
                   int pinNumClk,
                   int pinNumCs) : _pinMiso(pinMiso),
                                   _pinMosi(pinMosi),
                                   _pinNumClk(pinNumClk),
                                   _pinNumCs(pinNumCs)
{
}

esp_err_t SDDriver::_init()
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

    _host.max_freq_khz = 10; //<<<< KEEP LOW IN CASE OF COMMUNICATION TROUBLES!! >>>>   SDMMC_FREQ_PROBING

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
        _isInit = false;
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
        _isInit = false;
        return ret;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    _isInit = true;

    return ret;
}

void SDDriver::doneSD()
{
    if (_isInit)
    {
        // All done, unmount partition and disable SPI peripheral
        esp_vfs_fat_sdcard_unmount(CONFIG_SDCARD_MOUNT_POINT, card);
        ESP_LOGI(TAG, "Card unmounted");

        // deinitialize the bus after all devices are removed
        spi_bus_free((spi_host_device_t)_host.slot);
        _isInit = false;
    }
}

esp_err_t SDDriver::saveFile(const char *text, const char *fileName)
{
    esp_err_t ret = ESP_OK;

    if (!_isInit)
    {
        ret = _init();
    }

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Opening file to save %s", fileName);
        FILE *f = fopen(fileName, "w");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open file for writing.");
            ret = ESP_FAIL;
        }
        else
        {
            fprintf(f, text);
            fclose(f);
            ESP_LOGI(TAG, "File %s written.", fileName);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to _init SDCARD: %s", esp_err_to_name(ret));
    }

    doneSD();
    return ret;
}

esp_err_t SDDriver::readFile(char *text, const char *fileName)
{
    esp_err_t ret = ESP_OK;

    if (!_isInit)
    {
        ret = _init();
    }
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Opening file to read %s", fileName);
        char row[64] = {0};
        FILE *f = fopen(fileName, "r");
        if (f == NULL)
        {
            ret = ESP_ERR_NOT_FOUND;
            ESP_LOGE(TAG, "Failed to open file for reading: %s", esp_err_to_name(ret));
        }
        else
        {
            while ((fgets(row, sizeof(row), f) != NULL) && (ret == ESP_OK)) // append one row at a time in output string
            {
                if ((strlen(text) + strlen(row) + 1) <= CONFIG_MAX_LENGTH_CYCLE_STRING) // check if new row fits in the receiving string
                    strcat(text, row);
                else // otherwise return an error
                {
                    ESP_LOGE(TAG, "Size of cycle char array is too small to memorize cycle in SDCARD file: %s",
                             esp_err_to_name(ret));
                    ret = ESP_FAIL;
                }
            }
            fclose(f);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to _init SDCARD: %s", esp_err_to_name(ret));
    }
    doneSD();
    return ret;
}

bool SDDriver::testSDCard()
{
    ESP_LOGI(TAG, "Testing SDCARD for writing");
    bool ret = false;

    // try to write a file to check if SDCARD can be written
    char SDPath[32] = {0};
    strcat(SDPath, CONFIG_SDCARD_MOUNT_POINT);
    strcat(SDPath, "/TEST.KNO");
    esp_err_t err = saveFile("___", SDPath);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Test file was successfully written.\nSDCARD is working properly.");
        ret = true;
    }
    else
    {
        ESP_LOGE(TAG, "SDCARD test failed, check if SDCARD is present and if can be written.");
    };
    return ret;
}

SDDriver::~SDDriver()
{
    doneSD();
}