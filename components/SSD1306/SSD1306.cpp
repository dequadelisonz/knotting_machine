#include "SSD1306.hpp"

SSD1306_128x64::SSD1306_128x64() : _i2cSocket(I2CMasterSocket::getIstance(_I2C_NUM,
                                                                          GPIO_NUM_16, // TODO define I2C pins somehow else
                                                                          GPIO_NUM_17,
                                                                          GPIO_NUM_15))
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // build up the command "cmd" to be sent to display for initialization
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_OFF, true);        // AE
    i2c_master_write_byte(cmd, OLED_CMD_SET_MUX_RATIO, true);      // A8
    i2c_master_write_byte(cmd, 0x3F, true);                        // 0x1F if HEIGHT is 32
    i2c_master_write_byte(cmd, OLED_CMD_SET_DISPLAY_OFFSET, true); // D3
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_DISPLAY_START_LINE, true); // 40
    i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP_1, true);    // A1
    i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true);      // C8
    i2c_master_write_byte(cmd, OLED_CMD_SET_DISPLAY_CLK_DIV, true);    // D5
    i2c_master_write_byte(cmd, 0x80, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_COM_PIN_MAP, true); // DA
    i2c_master_write_byte(cmd, 0x12, true);                     // 0x02 if HEIGHT is 32
    i2c_master_write_byte(cmd, OLED_CMD_SET_CONTRAST, true);    // 81
    i2c_master_write_byte(cmd, 0xFF, true);
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_RAM, true);       // A4
    i2c_master_write_byte(cmd, OLED_CMD_SET_VCOMH_DESELCT, true); // DB
    i2c_master_write_byte(cmd, 0x40, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_MEMORY_ADDR_MODE, true); // 20
    i2c_master_write_byte(cmd, OLED_CMD_SET_PAGE_ADDR_MODE, true);   // 02
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_write_byte(cmd, 0x10, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true); // 8D
    i2c_master_write_byte(cmd, 0x14, true);
    i2c_master_write_byte(cmd, OLED_CMD_DEACTIVE_SCROLL, true); // 2E
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_NORMAL, true);  // A6
    i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);      // AF
    i2c_master_stop(cmd);

    esp_err_t espRc = i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS);
    if (espRc == ESP_OK)
    {
        ESP_LOGI(TAG, "OLED configured successfully");
    }
    else
    {
        ESP_LOGE(TAG, "OLED configuration failed. code: 0x%.2X", espRc);
    }
    i2c_cmd_link_delete(cmd);
}

void SSD1306_128x64::_invert(uint8_t *buf, size_t blen)
{
    uint8_t wk;
    for (int i = 0; i < blen; i++)
    {
        wk = buf[i];
        buf[i] = ~wk;
    }
}

void SSD1306_128x64::_displayImage(int page, int seg, uint8_t *images, int width)
{
    i2c_cmd_handle_t cmd;

    if (page >= PAGES)
        return; // TODO find better way to manage overflows
    if (seg >= WIDTH)
        return;

    int _seg = seg; // + CONFIG_OFFSETX;
    uint8_t columLow = _seg & 0x0F;
    uint8_t columHigh = (_seg >> 4) & 0x0F;

    int _page = page;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, true);

    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    // Set Lower Column Start Address for Page Addressing Mode
    i2c_master_write_byte(cmd, (0x00 + columLow), true);
    // Set Higher Column Start Address for Page Addressing Mode
    i2c_master_write_byte(cmd, (0x10 + columHigh), true);
    // Set Page Start Address for Page Addressing Mode
    i2c_master_write_byte(cmd, 0xB0 | _page, true);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, true);

    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
    i2c_master_write(cmd, images, width, true);

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void SSD1306_128x64::contrast(int contrast)
{
    i2c_cmd_handle_t cmd;
    int _contrast = contrast;
    if (contrast < 0x0)
        _contrast = 0;
    if (contrast > 0xFF)
        _contrast = 0xFF;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
    i2c_master_write_byte(cmd, OLED_CMD_SET_CONTRAST, true); // 81
    i2c_master_write_byte(cmd, _contrast, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(_I2C_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void SSD1306_128x64::clearLine(int page, bool invert)
{
    // char space[16] = {0};
    //  displayText(page, space, /*sizeof(space)*/, invert);
    uint8_t image[8] = {0};
    for (uint8_t i = 0, seg = 0; i < 16; ++i, seg += 8)
    {
        _displayImage(page, seg, image, 8);
    }
}

void SSD1306_128x64::clearScreen(bool invert)
{
    for (int page = 0; page < PAGES; page++)
    {
        clearLine(page, invert);
    }
}

void SSD1306_128x64::displayText(int page, char *text, /*int text_len,*/ bool invert)
{
    if (page >= PAGES)
        return;

    clearLine(page, false);
    int _text_len = strlen(text);
    if (_text_len > 16)
        _text_len = 16;

    uint8_t seg = 0;
    uint8_t image[8];
    for (uint8_t i = 0; i < _text_len; i++)
    {
        memcpy(image, Font8x8Ns::font8x8_basic_tr[(uint8_t)text[i]], 8);
        if (invert)
            _invert(image, 8);
        _displayImage(page, seg, image, 8);
        seg = seg + 8;
    }
}