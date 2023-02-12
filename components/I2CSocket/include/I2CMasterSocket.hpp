#ifndef I2CMasterSocket_hpp
#define I2CMasterSocket_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "driver/i2c.h"

/*This project includes*/


/* Singleton class - Scott Meyers docet*/
class I2CMasterSocket
{
private:
    const char *TAG = "I2CMasterSocket";
    const uint32_t I2C_MASTER_FREQ_HZ = 400000U; /*!< I2C clock of SSD1306 can run at 400 kHz max. */

    i2c_config_t _i2c_config;
    i2c_port_t _i2cPort;

    I2CMasterSocket(i2c_port_t i2cPort, gpio_num_t sda, gpio_num_t scl, gpio_num_t reset) : _i2cPort(i2cPort)
    {
        _i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = sda,
            .scl_io_num = scl,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master = {
                .clk_speed = I2C_MASTER_FREQ_HZ},
            .clk_flags = 0U};
        ESP_ERROR_CHECK(i2c_param_config(_i2cPort, &_i2c_config));
        ESP_ERROR_CHECK(i2c_driver_install(_i2cPort, I2C_MODE_MASTER, 0, 0, 0));

        if (reset >= 0)
        {
            gpio_reset_pin(reset);
            gpio_set_direction(reset, GPIO_MODE_OUTPUT);
            gpio_set_level(reset, 0);
            vTaskDelay(50 / portTICK_PERIOD_MS);
            gpio_set_level(reset, 1);
        }
    }

    I2CMasterSocket(const I2CMasterSocket &) = delete;
    I2CMasterSocket &operator=(const I2CMasterSocket) = delete;

    ~I2CMasterSocket()
    {
        ESP_ERROR_CHECK(i2c_driver_delete(_i2cPort));
    }

public:
    static I2CMasterSocket &getIstance(i2c_port_t i2cPort, gpio_num_t sda, gpio_num_t scl, gpio_num_t reset)
    {
        static I2CMasterSocket _instance(i2cPort, sda, scl, reset);
        return _instance;
    }
};

#endif