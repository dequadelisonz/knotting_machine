#ifndef NORVIEXF16_hpp
#define NORVIEXF16_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "esp_log.h"

/*This project includes*/
#include "I2CMasterSocket.hpp"

/* Credits to NSBum https://github.com/NSBum/esp32-mcp23017-demo
    based on MCP23017.h                                          */

class NORVIEXF16
{
    friend class GPOutArray;

private:
    const char *TAG = "NORVIEXF16";

    /*
    gpioGroupEnum

    Specifies a group of GPIO pins, either
    group A or group B
    */
    enum gpioGroupEnum
    {
        GPIOA = 0x00,
        GPIOB = 0x01
    };

    /*
    registerEnum

    Specifies a generic register which
    can point to either group A or
    group B depending on an offset that
    can be applied.
    */
    enum registerEnum
    {
        MCP23017_IODIR = 0x00,
        MCP23017_IPOL = 0x01,
        MCP23017_GPINTEN = 0x02,
        MCP23017_DEFVAL = 0x03,
        MCP23017_INTCON = 0x04,
        MCP23017_IOCON = 0x05,
        MCP23017_GPPU = 0x06,
        MCP23017_INTF = 0x07,
        MCP23017_INTCAP = 0x08,
        MCP23017_GPIO = 0x09,
        MCP23017_OLAT = 0x0A
    };

    uint8_t _address = 0x27;
    const i2c_port_t _I2C_NUM = 0;

    I2CMasterSocket &_i2cSocket;

    uint8_t mcpRegister(NORVIEXF16::registerEnum reg, NORVIEXF16::gpioGroupEnum group)
    {
        return (group == GPIOA) ? (reg << 1) : (reg << 1) | 1;
    }

    void _setAddress(uint8_t address) { _address = address; };
    uint8_t _getAddress() const { return _address; };

    esp_err_t _writeRegister(NORVIEXF16::registerEnum reg, NORVIEXF16::gpioGroupEnum group, uint8_t v);

    esp_err_t _readRegister(NORVIEXF16::registerEnum reg, NORVIEXF16::gpioGroupEnum group, uint8_t *data);

public:
    NORVIEXF16();
};

#endif
