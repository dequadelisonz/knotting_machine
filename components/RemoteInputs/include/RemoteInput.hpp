#ifndef Button_hpp
#define Button_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "driver/gpio.h"
#include "esp_timer.h"

/*This project includes*/
#include "Functor.hpp"
#include "RemoteInputBase.hpp"

template <typename TClass>
class RemoteInput : public RemoteInputBase
{
private:
    const char *TAG = "RemoteInput";

protected:
    TClass *_context;

    static const uint32_t DEBOUNCE_TIME = 200U; // set a debounce time

    bool _active = true; // TODO serve?
    bool _status = false;
    bool _prevStatus = false;
    bool _reading = false;

    float _lastDebounceTime = 0.0f;

    Functor<TClass> _action;

public:
    RemoteInput(){};

    void setAction(TClass *context, void (TClass::*fpt)())
    {
        _context = context;
        _action.setCallback(_context, fpt);
    }

    /*
   Debounce algorithm --> credits to David A. Mellis, Limor Fried, Mike Walters, Arturo Guadalupi
   https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce
   Before calling in overridden methods --> update "_reading" field of derived class
   */
    virtual void updateStatus() override
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
                // ESP_LOGI(TAG,"Selector before onStatusChange\n");
                onStatusChange(_status);
            }
        }
        _prevStatus = _reading;
    };

    virtual void onStatusChange(bool cond) = 0; // to be overridden to do something real

    bool getStatus() const { return _status; }

    bool &isActive() { return _active; }
};

#endif
