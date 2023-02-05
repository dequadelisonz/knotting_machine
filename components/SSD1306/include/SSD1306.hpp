#ifndef SSD1306_hpp
#define SSD1306_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "esp_log.h"

/*This project includes*/
#include "I2CMasterSocket.hpp"
#include "font8x8_basic.h"

/*credits to https://github.com/nopnop2002/esp-idf-ssd1306 */
/* check that repo for more details */
class SSD1306_128x64
{

public:
    static const int WIDTH = 128;
    static const int HEIGHT = 64;
    static const int PAGES = 8;

private:
    const char *TAG = "SSD1306_128x64";

    const int _address = 0x3C;
    const i2c_port_t _I2C_NUM = 0;

    I2CMasterSocket &_i2cSocket;

    // Following definitions are bollowed from
    // http://robotcantalk.blogspot.com/2015/03/interfacing-arduino-with-ssd1306-driven.html

    /* Control byte for i2c
    Co : bit 8 : Continuation Bit
     * 1 = no-continuation (only one byte to follow)
     * 0 = the controller should expect a stream of bytes.
    D/C# : bit 7 : Data/Command Select bit
     * 1 = the next byte or byte stream will be Data.
     * 0 = a Command byte or byte stream will be coming up next.
     Bits 6-0 will be all zeros.
    Usage:
    0x80 : Single Command byte
    0x00 : Command Stream
    0xC0 : Single Data byte
    0x40 : Data Stream
    */
    const int OLED_CONTROL_BYTE_CMD_SINGLE = 0x80;
    const int OLED_CONTROL_BYTE_CMD_STREAM = 0x00;
    const int OLED_CONTROL_BYTE_DATA_SINGLE = 0xC0;
    const int OLED_CONTROL_BYTE_DATA_STREAM = 0x40;

    // Fundamental commands (pg.28)
    const int OLED_CMD_SET_CONTRAST = 0x81; // follow with 0x7F
    const int OLED_CMD_DISPLAY_RAM = 0xA4;
    const int OLED_CMD_DISPLAY_ALLON = 0xA5;
    const int OLED_CMD_DISPLAY_NORMAL = 0xA6;
    const int OLED_CMD_DISPLAY_INVERTED = 0xA7;
    const int OLED_CMD_DISPLAY_OFF = 0xAE;
    const int OLED_CMD_DISPLAY_ON = 0xAF;

    // Addressing Command Table (pg.30)
    const int OLED_CMD_SET_MEMORY_ADDR_MODE = 0x20;
    const int OLED_CMD_SET_HORI_ADDR_MODE = 0x00; // Horizontal Addressing Mode
    const int OLED_CMD_SET_VERT_ADDR_MODE = 0x01; // Vertical Addressing Mode
    const int OLED_CMD_SET_PAGE_ADDR_MODE = 0x02; // Page Addressing Mode
    const int OLED_CMD_SET_COLUMN_RANGE = 0x21;   // can be used only in HORZ/VERT mode - follow with 0x00 and 0x7F = COL127
    const int OLED_CMD_SET_PAGE_RANGE = 0x22;     // can be used only in HORZ/VERT mode - follow with 0x00 and 0x07 = PAGE7

    // Hardware Config (pg.31)
    const int OLED_CMD_SET_DISPLAY_START_LINE = 0x40;
    const int OLED_CMD_SET_SEGMENT_REMAP_0 = 0xA0;
    const int OLED_CMD_SET_SEGMENT_REMAP_1 = 0xA1;
    const int OLED_CMD_SET_MUX_RATIO = 0xA8; // follow with 0x3F = 64 MUX
    const int OLED_CMD_SET_COM_SCAN_MODE = 0xC8;
    const int OLED_CMD_SET_DISPLAY_OFFSET = 0xD3; // follow with 0x00
    const int OLED_CMD_SET_COM_PIN_MAP = 0xDA;    // follow with 0x12
    const int OLED_CMD_NOP = 0xE3;                // NOP

    // Timing and Driving Scheme (pg.32)
    const int OLED_CMD_SET_DISPLAY_CLK_DIV = 0xD5; // follow with 0x80
    const int OLED_CMD_SET_PRECHARGE = 0xD9;       // follow with 0xF1
    const int OLED_CMD_SET_VCOMH_DESELCT = 0xDB;   // follow with 0x30

    // Charge Pump (pg.62)
    const int OLED_CMD_SET_CHARGE_PUMP = 0x8D; // follow with 0x14

    // Scrolling Command
    const int OLED_CMD_HORIZONTAL_RIGHT = 0x26;
    const int OLED_CMD_HORIZONTAL_LEFT = 0x27;
    const int OLED_CMD_CONTINUOUS_SCROLL = 0x29;
    const int OLED_CMD_DEACTIVE_SCROLL = 0x2E;
    const int OLED_CMD_ACTIVE_SCROLL = 0x2F;
    const int OLED_CMD_VERTICAL = 0xA3;

    void _invert(uint8_t *buf, size_t blen);

    void _displayImage(int page, int seg, uint8_t *images, int width);

public:
    SSD1306_128x64();

    void contrast(int contrast);

    void clearLine(int page, bool invert);

    void clearScreen(bool invert);

    void displayText(int page, char *text,/* int text_len,*/ bool invert);
};

#endif
