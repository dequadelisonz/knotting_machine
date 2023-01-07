#ifndef KnotEngine_hpp
#define KnotEngine_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "SequenceEngine.hpp"
#include "Keyboard.hpp"
#include "PushButton.hpp"
#include "AnalogButton.hpp"
#include "Selector.hpp"

class KnotEngine : public SequenceEngine
{
private:
    const char *TAG = "KnotEngine";

    Keyboard _keyboard;

    Functor<KnotEngine> _actionStart;
    Functor<KnotEngine> _actionStop;
    Functor<KnotEngine> _actionUp;
    Functor<KnotEngine> _actionDown;
    Functor<KnotEngine> _actionOK;
    Functor<KnotEngine> _actionSelOn;
    Functor<KnotEngine> _actionSelOff;

    PushButton _btnStart;
    PushButton _btnStop;
    AnalogButton _btnUP;
    AnalogButton _btnDOWN;
    AnalogButton _btnOK;
    Selector _selMode;

    void _start()
    {
        printf("Start\n");
    }

    void _stop()
    {
        printf("Stop\n");
    }

    void _up()
    {
        printf("Up\n");
    }

    void _down()
    {
        printf("Down\n");
    }

    void _OK()
    {
        printf("Ok\n");
    }

    void _selON()
    {
        printf("Selector ON\n");
    }

    void _selOFF()
    {
        printf("Selector OFF\n");
    }

public:
    KnotEngine() : _actionStart(this, &KnotEngine::_start),
                   _actionStop(this, &KnotEngine::_stop),
                   _actionUp(this, &KnotEngine::_up),
                   _actionDown(this, &KnotEngine::_down),
                   _actionOK(this, &KnotEngine::_OK),
                   _actionSelOn(this, &KnotEngine::_selON),
                   _actionSelOff(this, &KnotEngine::_selOFF),
                   _btnStart("Start",
                             (gpio_num_t)CONFIG_START_BUTTON_GPIO,
                             Button::eButtonLogic::NO,
                             _actionStart),
                   _btnStop("Stop",
                            (gpio_num_t)CONFIG_STOP_BUTTON_GPIO,
                            Button::eButtonLogic::NC,
                            _actionStop),
                   _btnUP("UP",
                          ADC1_CHANNEL_0,
                          AnalogButton::eBtnLevel::UP,
                          _actionUp),
                   _btnDOWN("DOWN",
                            ADC1_CHANNEL_0,
                            AnalogButton::eBtnLevel::DOWN,
                            _actionDown),
                   _btnOK("OK",
                          ADC1_CHANNEL_0,
                          AnalogButton::eBtnLevel::OK,
                          _actionOK),
                   _selMode("Selector",
                            (gpio_num_t)CONFIG_MODE_SELECTOR_GPIO,
                            Button::eButtonLogic::NC,
                            _actionSelOn,
                            _actionSelOff)
    {
    }

    virtual bool onControlCreate()
    {
        bool ret = true;
        ret = ret && _keyboard.addButton(&_btnStart);
        ret = ret && _keyboard.addButton(&_btnStop);
        ret = ret && _keyboard.addButton(&_btnUP);
        ret = ret && _keyboard.addButton(&_btnDOWN);
        ret = ret && _keyboard.addButton(&_btnOK);
        ret = ret && _keyboard.addButton(&_selMode);
        return ret;
    }
    virtual bool onControlUpdate(uint64_t elapsedTime)
    {
        // ESP_LOGI(TAG,"onControlUpdate");
        _keyboard.updateStatus();
        // vTaskDelay(100/portTICK_PERIOD_MS);
        return true;
    }

    virtual bool onSequenceCreate()
    {
        return true;
    }

    virtual bool onSequenceUpdate(uint64_t elapsedTime)
    {
        return true;
    }
};

#endif