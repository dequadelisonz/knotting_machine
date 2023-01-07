#ifndef Button_hpp
#define Button_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "driver/gpio.h"

/*This project includes*/
#include "Functor.hpp"

class Button
{
public:
    enum eButtonLogic
    {
        NC = 0,
        NO = 1,
    };

private:
    static const uint8_t MAX_NAME_LENGTH = 12; // inclding final null char
    const char *TAG = "Button";

    char _name[MAX_NAME_LENGTH] = {0};

protected:
    const uint64_t DEBOUNCE_TIME = 200; // set a debounce time

    bool _active = true, _status = false, _prevStatus = false, _reading;
    float _lastDebounceTime = 0.0f;

public:
    Button(const char *name)
    {
        // clipping name to max allowed length
        int len = (strlen(name) >= (MAX_NAME_LENGTH - 1)) ? (MAX_NAME_LENGTH - 1) : strlen(name);
        memcpy(_name, name, len);
        _name[len] = '\0';
    }

    /*
   Debounce algorithm --> credits to David A. Mellis, Limor Fried, Mike Walters, Arturo Guadalupi
   https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce
   Before calling in overridden methods --> update "_reading" field of derived class
   */
    virtual void updateStatus()
    {
        // ESP_LOGI(TAG,"updatestatus");
        if (_reading != _prevStatus)
        {
            _lastDebounceTime = esp_timer_get_time();
            // printf("change\n");
            //  ESP_LOGI(TAG, "detected different reading\n");
        }

        if ((esp_timer_get_time() - _lastDebounceTime) > DEBOUNCE_TIME)
        {
            if (_reading != _status)
            {
                _status = _reading;
                //ESP_LOGI(TAG,"Selector before onStatusChange\n");
                onStatusChange(_status);
            }
        }
        _prevStatus = _reading;
    };

    virtual void onStatusChange(bool cond) {} // to be overridden to do something real

    const char *getName() const { return _name; }

    bool &setActive() { return _active; }
};

#endif