#ifndef HMI_hpp
#define HMI_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "esp_log.h"

/*This project includes*/
#include "HMI.hpp"
#include "InputConsole.hpp"
#include "PushButton.hpp"
#include "AnalogButton.hpp"
#include "Selector.hpp"
#include "Menu.hpp"

class KnotEngine;

class HMI
{
private:
    const char *TAG = "HMI";

    KnotEngine &_knotEngine;
    Menu<HMI> _menu;

    // declaring remote inputs (pushbuttons, selectors, analogbuttons, etc.)
    PushButton<HMI> _btnStart;
    PushButton<HMI> _btnStop;
    AnalogButton<HMI> _btnUP;
    AnalogButton<HMI> _btnDOWN;
    AnalogButton<HMI> _btnOK;
    Selector<HMI> _selMode;

    // declaring menu entries
    MenuEntry<HMI> _batchQtyME;
    MenuEntry<HMI> _pcsPerMinME;
    MenuEntry<HMI> _SDCardUpdME;
    MenuEntry<HMI> _WifiUpdME;

    InputConsole _inputConsole;

    /*functions (actions) to be associated with push buttons and menu entries*/
    void _start();
    void _stop();
    void _up();
    void _down();
    void _OK();
    void _selON();
    void _selOFF();
    
    void printScreen();

public:
    HMI(KnotEngine &knotEngine);

    void updateStatus();
};

#endif