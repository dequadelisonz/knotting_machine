#include "GPOut.hpp"

GPOutArray::GPOutPin::GPOutPin(uint8_t pin) : _expansionId(pin / MAX_GPOut),
                                              _pinInExpansion(pin % MAX_GPOut),
                                              _group((NORVIEXF16::gpioGroupEnum)(_pinInExpansion / (MAX_GPOut / 2))),
                                              _bitMaskOn(swOnMap[_pinInExpansion]),
                                              _bitMaskOff(swOffMap[_pinInExpansion])
{
}

/**
 * Construct a General Purpose Output pin array given an array of expansion addresses
 * @param addresses array of uint8_t for I2C expansion addresses
 */
GPOutArray::GPOutArray(const int addresses[MAX_EXPANSIONS])
{
    // count number of expansion with initialized address
    for (int i = 0; ((i < MAX_EXPANSIONS) && (addresses[i] != 0)); ++i)
    {
        _expansions[i]._setAddress(addresses[i]);
        _expansionQty = i + 1; // store nr of expansion inialized with addresses
    }

    for (int i = 0; i < (_expansionQty * MAX_GPOut); ++i)
    {
        _pins[i] = GPOutPin(i);
    }
}

/**
 *  Helper function to write GPIO status
 * @param gpo number of expansion output (1 for Q1, 2 for Q2, etc.)
 * @param v bitmask to identify outputs to switch
 * @param on = true if switch on, = false if switch off
 * @return error of esp_err_t type. If gpo is out of range returns ESP_FAIL.
*/
esp_err_t GPOutArray::_setGPOStatus(uint8_t const gpo, uint8_t v, bool const on)
{
    esp_err_t ret = ESP_OK;
    if (!((gpo < 1) || (gpo > (MAX_GPOut * _expansionQty))))
    {
        uint8_t pinId = gpo - 1;
        uint8_t currentExp = _pins[pinId]._expansionId;
        uint8_t currentByte = 0;
        ret = _expansions[currentExp]._readRegister(NORVIEXF16::MCP23017_GPIO,
                                                    _pins[pinId]._group,
                                                    &currentByte);
        if (ret == ESP_OK)
        {
            v = on ? (v | currentByte) : (v & currentByte);
            ret = _expansions[currentExp]._writeRegister(NORVIEXF16::MCP23017_GPIO,
                                                         _pins[pinId]._group,
                                                         v);
        }
    }
    else
        ret = ESP_FAIL;
    return ret;
}

/**
 * Set to on expansion output
 * @param gpo number of output to set (1 for Q1, 2 for Q2, etc.)
 * @return error of esp_err_t type. If gpo is out of range returns ESP_FAIL.
*/
esp_err_t GPOutArray::setOn(uint8_t gpo)
{

    uint8_t v = _pins[gpo - 1].swOnMap[_pins[gpo - 1]._pinInExpansion];
    return _setGPOStatus(gpo, v, true);
}

/**
 * Set to off expansion output
 * @param gpo number of output to set (1 for Q1, 2 for Q2, etc.)
 * @return error of esp_err_t type. If gpo is out of range returns ESP_FAIL.
*/
esp_err_t GPOutArray::setOff(uint8_t gpo)
{
    uint8_t v = _pins[gpo - 1].swOffMap[_pins[gpo - 1]._pinInExpansion];
    return _setGPOStatus(gpo, v, false);
}