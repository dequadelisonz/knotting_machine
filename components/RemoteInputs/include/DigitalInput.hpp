#ifndef DigitalInput_hpp
#define DigitalInput_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "RemoteInput.hpp"

/*This class defines an object DigitalInput with PULLUP internal setting*/
template <typename TClass>
class DigitalInput : public RemoteInput<TClass>
{

private:
    const char *TAG = "DigitalInput";

    gpio_num_t _pin; // not inherited because could not always be a pin

    RemoteInputBase::eRemInputLogic _logic;

protected:
    static const uint32_t DEBOUNCE_TIME = 200U; // set a debounce time

public:
    DigitalInput(){};

    void init(gpio_num_t pin,
              RemoteInputBase::eRemInputLogic logic)

    {
        _logic = logic;
        _pin = pin;
        // Change GPIO mapping for control push buttons and selector
        gpio_set_direction(_pin, GPIO_MODE_INPUT); // setting pin as input
        ESP_ERROR_CHECK(gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY));
    }

    void updateStatus() override
    {

        /* with pullup button connection, pressure is felt as 0 with N.O. button
        and as 1 with N.C. button;
        N.O. button is declared with logic as true and N.C. with logic as false
        the next IF apply XOR logic and is true if button is pushed:

        status        | logic | result
        --------------|-------|-------
        0(not pushed) | 0(NC) |   0
        1(pushed)     | 0(NC) |   1
        0(pushed)     | 1(NO) |   1
        1(not pushed) | 1(NO) |   0

        first update temporary reading then call base class for debouncing
        */
        // ESP_LOGI(TAG,"pin status %d",gpio_get_level(_pin));
        this->_reading = (gpio_get_level(_pin) != _logic);
        RemoteInput<TClass>::updateStatus();
        // ESP_LOGI(TAG, "updateStatus of %s, _status %d", getName(),_status);
    }

    void onStatusChange(bool cond) override
    {
        if (cond)
            this->_action();
    }
};

#endif