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

    friend class StatusLine;

private:
    const char *TAG = "HMI";
    static const int _NR_OF_ROWS = SSD1306_128x64::PAGES;
    static const int _CHARS_IN_ROW = (SSD1306_128x64::WIDTH / Font8x8Ns::WIDTH_OF_CHAR);
    char _screenContent[_NR_OF_ROWS]
                       [_CHARS_IN_ROW] = {0};

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

    void _increaseBatch();
    void _decreaseBatch();

    void _increasePcm();
    void _decreasePcm();

    void _printScreen();

    void _setMenuCanvas();

    void _updateFromSD();
    void _updateFromWIFI();

    class StatusLine
    {
        const char *PROMPT = ">";
        char _statusLine[HMI::_CHARS_IN_ROW];
        const uint8_t _remLen;
        HMI &_parent;

        void _setStatus(const char *status)
        {
            // TODO make some helper class to clip the length
            _statusLine[0] = '\0';
            strcat(_statusLine, PROMPT);
            strncat(_statusLine, status, _remLen);
        }

    public:
        StatusLine(HMI &parent) : _remLen(HMI::_CHARS_IN_ROW - strlen(PROMPT) - 1),
                                  _parent(parent)
        {
        }

        void printStatus()
        {
            _parent._display.displayText(7, _statusLine, false);
        }

        void printStatus(const char *status)
        {
            _setStatus(status);
            _parent._display.displayText(7, _statusLine, false);
        }

    } _statusLine;

public:
    HMI(KnotEngine &knotEngine);

    void updateStatus();

    void printStatus(const char *status)
    {
        _statusLine.printStatus(status);
    }

    void freezeMenu()
    {
        _menuNavigator.freezeMenu();
    }

    void unFreezeMenu()
    {
        _menuNavigator.unFreezeMenu();
    }
};

#endif