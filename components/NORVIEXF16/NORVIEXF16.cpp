#include "NORVIEXF16.hpp"

NORVIEXF16::NORVIEXF16() : _i2cSocket(I2CMasterSocket::getIstance(_I2C_NUM,
                                                                  GPIO_NUM_16, // TODO define I2C pins somehow else
                                                                  GPIO_NUM_17,
                                                                  GPIO_NUM_15))
{
    _writeRegister(MCP23017_IODIR, GPIOA, 0x00);
    _writeRegister(MCP23017_IODIR, GPIOB, 0x00);
}

esp_err_t NORVIEXF16::_writeRegister(NORVIEXF16::registerEnum reg, NORVIEXF16::gpioGroupEnum group, uint8_t v)
{
    uint8_t r = mcpRegister(reg, group);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, r, true);
    i2c_master_write_byte(cmd, v, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t NORVIEXF16::_readRegister(NORVIEXF16::registerEnum reg, NORVIEXF16::gpioGroupEnum group, uint8_t *data)
{

    uint8_t r = mcpRegister(reg, group);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, r, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS); 
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL)
    {
        ESP_LOGE(TAG, "ERROR: unable to write address %02x to read reg %02x", _address, r);
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL)
    {
        ESP_LOGE(TAG, "ERROR: unable to read reg %02x from address %02x", r, _address);
    }
    return ret;
}
