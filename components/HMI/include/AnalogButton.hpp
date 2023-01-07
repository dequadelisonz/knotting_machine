#ifndef AnalogButton_hpp
#define AnalogButton_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "driver/adc.h"
#include "esp_adc_cal.h"

/*This project includes*/
#include "Button.hpp"

class AnalogButton : public Button
{

public:
    enum eBtnLevel
    {
        UP = 1,
        DOWN = 2,
        OK = 3
    };

protected:
    const uint64_t DEBOUNCE_TIME = 200; // set a debounce time

private:
    const char *TAG = "AnalogButton";

    esp_adc_cal_characteristics_t _adc1_chars;

    const adc1_channel_t _adcChannel;

    const eBtnLevel _btnLevel;
    FunctorBase &_action;

public:
    AnalogButton(const char *name,
                 int adcChannel,
                 eBtnLevel btnLevel,
                 FunctorBase &action) : Button(name),
                                        _adcChannel((adc1_channel_t)adcChannel),
                                        _btnLevel(btnLevel),
                                        _action(action)

    {
        //_adcChannel = (adc1_channel_t)adcChannel;
        esp_adc_cal_characterize(ADC_UNIT_1,
                                 ADC_ATTEN_DB_11,
                                 (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT,
                                 0, &_adc1_chars);

        adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT);
        adc1_config_channel_atten(_adcChannel, ADC_ATTEN_DB_11);
    }

    void updateStatus() override
    {
        _reading = (adc1_get_raw(_adcChannel) / 1000) == _btnLevel;
        Button::updateStatus();
    }

    void onStatusChange(bool cond) override
    {
        if (cond)
            _action();
    }
};

#endif