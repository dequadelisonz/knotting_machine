#ifndef HMI_hpp
#define HMI_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "esp_log.h"

/*This project includes*/
#include "InputConsole.hpp"
#include "PushButton.hpp"
#include "AnalogButton.hpp"
#include "Selector.hpp"
#include "Menu.hpp"
#include "SSD1306.hpp"

class KnotEngine;

class HMI
{
private:
    const char *TAG = "HMI";
    static const int _NR_OF_ROWS = SSD1306_128x64::PAGES;
    static const int _CHARS_IN_ROW = (SSD1306_128x64::WIDTH / Font8x8Ns::WIDTH_OF_CHAR);
    char _screenContent[_NR_OF_ROWS]
                       [_CHARS_IN_ROW] = {0};

    bool _okPressed = false;

    KnotEngine &_knotEngine;
    MenuNs::Menu _menu;
    MenuNs::MenuNavigator _menuNavigator;

    // declaring remote inputs (pushbuttons, selectors, analogbuttons, etc.)
    // PushButton<HMI> _btnStart;
    // PushButton<HMI> _btnStop;
    AnalogButton<HMI> _btnUP;
    AnalogButton<HMI> _btnDOWN;
    AnalogButton<HMI> _btnOK;
    // Selector<HMI> _5selMode;

    // declaring menu entries
    MenuNs::MenuEntry<HMI> _batchQtyME;
    MenuNs::MenuEntry<HMI> _pcsPerMinME;
    MenuNs::MenuEntry<HMI> _SDCardUpdME;
    MenuNs::MenuEntry<HMI> _WifiUpdME;

    InputConsole _inputConsole;

    SSD1306_128x64 _display;

    /*functions (actions) to be associated with push buttons and menu entries*/
    // void _start();
    // void _stop();
    void _up();
    void _down();
    void _OK();
    // void _selON();
    // void _selOFF();

    void _enableWifiME()
    {
        _WifiUpdME.isActive() = !_WifiUpdME.isActive();
    }

    void printScreen();

    // void printScreen(const char *content);

    void _setScreenContent();

public:
    HMI(KnotEngine &knotEngine);

    void updateStatus();
};

#endif