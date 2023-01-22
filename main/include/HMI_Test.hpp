#ifndef HMI_Test_hpp
#define HMI_Test_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "esp_log.h"

/*This project includes*/
#include "Menu.hpp"
#include "SSD1306.hpp"
#include "AnalogButton.hpp"

class HMI_Test
{
private:
    const char *TAG = "HMI_Test";

    const char _screenContent[(SSD1306_128x64::WIDTH / Font8x8Ns::WIDTH_OF_CHAR) *
                              (SSD1306_128x64::HEIGHT / SSD1306_128x64::PAGES)] = {0};

    MenuNs::Menu _menu;
    MenuNs::MenuNavigator _menuNavigator;

    // declaring remote inputs (pushbuttons, selectors, analogbuttons, etc.)
    AnalogButton<HMI> _btnUP;
    AnalogButton<HMI> _btnDOWN;
    AnalogButton<HMI> _btnOK;

    // declaring menu entries
    MenuNs::MenuEntry<HMI> _batchQtyME;
    MenuNs::MenuEntry<HMI> _pcsPerMinME;
    MenuNs::MenuEntry<HMI> _SDCardUpdME;
    MenuNs::MenuEntry<HMI> _WifiUpdME;

    InputConsole _inputConsole;

    /*functions (actions) to be associated with push buttons and menu entries*/
    void _start();
    void _stop();
    void _up();
    void _down();
    void _OK();
    void _selON();
    void _selOFF();

public:
    HMI_Test() : _menu("Main", true),
                 _menuNavigator(&_menu)

    {
        /*preparing menu entries*/
        _batchQtyME.init(this, "Q.tà lotto");
        _pcsPerMinME.init(this, "Pezzi/min");
        _SDCardUpdME.init(this, "Agg.to SDCARD");
        _WifiUpdME.init(this, "Agg.to WIFI");

        /*adding entries to menu instance*/
        bool ret = true;
        ret = ret && _menu.pushEntry(&_batchQtyME);
        ret = ret && _menu.pushEntry(&_pcsPerMinME);
        ret = ret && _menu.pushEntry(&_SDCardUpdME);
        ret = ret && _menu.pushEntry(&_WifiUpdME);

        if (!ret) // TODO implementare un controllo errori migliore
            ESP_LOGE(TAG,
                     "Some menu entries were not added, check for maximum remote availability.");

        /*preparing hardware remote inputs (buttons, etc.)*/
        _btnUP.init(this, "UP", ADC1_CHANNEL_0, RemoteInputBase::eBtnLevel::UP);
        _btnDOWN.init(this, "DOWN", ADC1_CHANNEL_0, RemoteInputBase::eBtnLevel::DOWN);
        _btnOK.init(this, "OK", ADC1_CHANNEL_0, RemoteInputBase::eBtnLevel::OK);

        /*binding to HMI's functions (actions)*/
        _btnUP.setAction(&HMI::_up);
        _btnDOWN.setAction(&HMI::_down);
        _btnOK.setAction(&HMI::_OK);

        // register remote inputs (buttons,etc.) in input console
        ret=true;
        ret = ret && _inputConsole.addButton(&_btnUP);
        ret = ret && _inputConsole.addButton(&_btnDOWN);
        ret = ret && _inputConsole.addButton(&_btnOK);

        if (!ret) // TODO implementare un controllo errori migliore
            ESP_LOGE(TAG,
                     "Some remote inputs were not added, check for maximum remote availability.");
    }
};

#endif