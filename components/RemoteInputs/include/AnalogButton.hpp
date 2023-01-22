#ifndef AnalogButton_hpp
#define AnalogButton_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "driver/adc.h"
#include "esp_adc_cal.h"

/*This project includes*/
#include "RemoteInput.hpp"

template <typename TClass>
class AnalogButton : public RemoteInput<TClass>
{

protected:
    const uint64_t DEBOUNCE_TIME = 200; // set a debounce time

private:
    const char *TAG = "AnalogButton";

    esp_adc_cal_characteristics_t _adc1_chars;

    adc1_channel_t _adcChannel;

    RemoteInputBase::eBtnLevel _btnLevel;

public:
    AnalogButton(){};

    void init(TClass *context,
              const char *name,
              int adcChannel,
              RemoteInputBase::eBtnLevel btnLevel)
    {
        RemoteInput<TClass>::init(context, name);
        _adcChannel = (adc1_channel_t)adcChannel;
        _btnLevel = btnLevel;
        esp_adc_cal_characterize(ADC_UNIT_1,
                                 ADC_ATTEN_DB_11,
                                 (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT,
                                 0, &_adc1_chars);

        adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT);
        adc1_config_channel_atten(_adcChannel, ADC_ATTEN_DB_11);
    }

    void updateStatus() override
    {
        //TODO solo debug int raw = adc1_get_raw(_adcChannel);
        // printf("%d\n", raw);
        // this->_reading = (raw / 1000) == _btnLevel;
        this->_reading = (adc1_get_raw(_adcChannel)/ 1000) == _btnLevel;
        RemoteInput<TClass>::updateStatus();
    }

    void onStatusChange(bool cond) override
    {
        if (cond)
            this->_action();
    }
};

#endif