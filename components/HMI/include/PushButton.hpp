#ifndef PushButton_hpp
#define PushButton_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "Button.hpp"

/*This class defines an object PushButton with PULLUP internal setting*/
class PushButton : public Button
{

private:
    const char *TAG = "PushButton";

    const gpio_num_t _pin;

    Button::eButtonLogic _logic;

protected:
    const uint64_t DEBOUNCE_TIME = 200; // set a debounce time
    FunctorBase &_action;

public:
    PushButton(const char *name,
               gpio_num_t pin,
               Button::eButtonLogic logic,
               FunctorBase &action) : Button(name),
                                      _pin(pin),
                                      _logic(logic),
                                      _action(action)

    {
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
        _reading = (gpio_get_level(_pin) != _logic);
        Button::updateStatus();
        // ESP_LOGI(TAG, "updateStatus of %s, _status %d", getName(),_status);
    }

    void onStatusChange(bool cond) override
    {
        if (cond)
            _action();
    }
};

#endif