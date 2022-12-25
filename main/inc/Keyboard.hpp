#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "string.h"

/*
buttons UP, DOWN, OK are simulated through a voltage divider
the divider is done in a way that analog value read, divided by 1000 gives the
pressed button number
*/

// use XMACRO pattern to define enum, name array and status array in one shot
//( enum_name , "button name" , status , logic (true = NORMALLY OPEN,false = NORMALLY CLOSED))
// virtual buttons simlualted with voltage divider are considered N(ormally)C(losed)
#define BUTTONS                \
    X(NONE, "NONE", 0, false)   \
    X(UP, "UP", 0, false)       \
    X(DOWN, "DOWN", 0, false)   \
    X(OK, "OK", 0, false)       \
    X(START, "START", 0, true) \
    X(STOP, "STOP", 0, false)

class Keyboard
{
public:
#define X(but, name, status, logic) but,
    enum Button : size_t
    {
        BUTTONS
            _count // additional enum member to count the elements **NOT SAFE**
    };
#undef X

private:
    const char *TAG = "Keyboard";

    const short int DEBOUNCE_MS = 250;
    float _timer=0.0f;

    esp_adc_cal_characteristics_t adc1_chars;
    const adc1_channel_t adcChannel = ADC1_CHANNEL_0; // TODO put this setting in sdkconfig

#define X(but, name, status, logic) name,
    const char *_buttonsNames[Button::_count] = {BUTTONS};
#undef X

#define X(but, name, status, logic) status,
    uint32_t _buttonsStatus[Button::_count] = {BUTTONS};
#undef X

#define X(but, name, status, logic) logic,
    bool _buttonsLogic[Button::_count] = {BUTTONS};
#undef X

    const gpio_num_t startGPIO = (gpio_num_t)CONFIG_START_BUTTON_GPIO;
    const gpio_num_t stopGPIO = (gpio_num_t)CONFIG_STOP_BUTTON_GPIO;
    const gpio_num_t selGPIO = (gpio_num_t)CONFIG_MODE_SELECTOR_GPIO;

    unsigned char _selModeStatus = 0; // GPIO21

public:
    Keyboard()
    {
        esp_adc_cal_characterize(ADC_UNIT_1,
                                 ADC_ATTEN_DB_11,
                                 (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT,
                                 0, &adc1_chars);

        adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT);
        adc1_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);

        // Change GPIO mapping for control push buttons and selector
        gpio_set_direction(startGPIO, GPIO_MODE_INPUT); // Start Button
        ESP_ERROR_CHECK(gpio_set_pull_mode(startGPIO, GPIO_PULLUP_ONLY));
        gpio_set_direction(stopGPIO, GPIO_MODE_INPUT); // Stop Button
        ESP_ERROR_CHECK(gpio_set_pull_mode(stopGPIO, GPIO_PULLUP_ONLY));
        gpio_set_direction(selGPIO, GPIO_MODE_INPUT); // mode selector
        ESP_ERROR_CHECK(gpio_set_pull_mode(selGPIO, GPIO_PULLUP_ONLY));
    }

    void updateButtonStatus(const float &elapsedTime)
    {
        memset(_buttonsStatus,0, sizeof(_buttonsStatus));                                 // resetting buttons array
        Keyboard::Button _curButton = (Keyboard::Button)(adc1_get_raw(adcChannel) / 1000); // see comment above Button enum, - 1 to be aligned to button array index
        _buttonsStatus[_curButton] = 1;                                                    // setting to 1 corresponding analogue button value

        _buttonsStatus[START] = gpio_get_level(startGPIO);
        _buttonsStatus[STOP] = gpio_get_level(stopGPIO);


        //_timer+=elapsedTime;

        _selModeStatus = gpio_get_level(selGPIO);
    }

    const unsigned char getSelMode() const
    {
        return _selModeStatus;
    }

    const uint32_t getButtonStatus(Keyboard::Button but) const
    {
        /* with pullup button connection, pressure is felt as 0 with N.O. button
            and as 1 with N.C. button;
            N.O. button is declared with logic as true and N.C. with logic as false
            the next IF apply XOR logic and is true if button is pushed:

            status         | logic | result
            ---------------|-------|-------
             0(not pushed) | 0(NC) |   0
             1(pushed)     | 0(NC) |   1
             0(pushed)     | 1(NO) |   1
             1(not pushed) | 1(NO) |   0                
        */
        if (_buttonsStatus[but] != _buttonsLogic[but])
            vTaskDelay(DEBOUNCE_MS / portTICK_PERIOD_MS);
        return _buttonsStatus[but];
    }

};

#endif