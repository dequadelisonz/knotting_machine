#ifndef HMI_hpp
#define HMI_hpp

/*STL includes*/

/*ESP-IDF includes*/
#include "esp_log.h"
#include <esp_pthread.h>
#include <thread>

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

    /*OLED display section*/
    static const int _NR_OF_ROWS = SSD1306_128x64::PAGES;
    static const int _CHARS_IN_ROW = (SSD1306_128x64::WIDTH / Font8x8Ns::WIDTH_OF_CHAR);
    char _screenContent[_NR_OF_ROWS]
                       [_CHARS_IN_ROW] = {0};

    SSD1306_128x64 _display;

    static pthread_mutex_t _rePaintStatusM;
    bool _rePaint = false;

    KnotEngine &_knotEngine;

    MenuNs::Menu _menu;
    MenuNs::MenuNavigator _menuNavigator;

    // declaring remote inputs (pushbuttons, selectors, analogbuttons, etc.)

    // analog buttons on MCU
    AnalogButton<HMI> _btnUP;
    AnalogButton<HMI> _btnDOWN;
    AnalogButton<HMI> _btnOK;

    // discrete buttons on panel
    PushButton<HMI> _btnStart;
    PushButton<HMI> _btnStop;
    Selector<HMI> _selMode;

    // declaring menu entries
    MenuNs::MenuEntry<HMI> _batchQtyME;
    MenuNs::MenuEntry<HMI> _pcsPerMinME;
    MenuNs::MenuEntry<HMI> _SDCardUpdME;
    MenuNs::MenuEntry<HMI> _WifiUpdME;

    InputConsole _inputConsole;

    /*functions (actions) to be associated with push buttons and menu entries*/

    // anaog buttons on MCU
    void _up();
    void _down();
    void _OK();

    // discrete buttons on panel
    void _start();
    void _stop();
    void _selON();
    void _selOFF();

    /*functions (actions) to be associated with menu entries */

    void _increaseBatch();
    void _decreaseBatch();

    void _increasePcm();
    void _decreasePcm();

    void _updateFromSD();
    void _updateFromWIFI();

    /* display update helper functions */
    void _printScreen();

    void _setMenuCanvas();

    class StatusLine
    {
    private:
        const char *TAG = "StatusLine";
        const char *PROMPT = ">";
        char _statusLine[HMI::_CHARS_IN_ROW];
        const uint8_t _remLen;
        HMI &_parent;
        const uint8_t _row;

        void _setStatus(const char *status)
        {
            // TODO make some helper class to clip the length
            _statusLine[0] = '\0';
            strcat(_statusLine, PROMPT);
            strncat(_statusLine, status, _remLen);
        }

    public:
        StatusLine(HMI &parent, uint8_t row) : _remLen(HMI::_CHARS_IN_ROW - strlen(PROMPT) - 1),
                                               _parent(parent),
                                               _row(row)
        {
        }

        void printStatus()
        {
            //ESP_LOGI(TAG, "%s", _statusLine);
            _parent._display.displayText(_row, _statusLine, false);
        }

        void printStatus(const char *status)
        {
            _setStatus(status);
            printStatus();
        }
    };

    StatusLine _sl6;
    StatusLine _sl7;

public:
    HMI(KnotEngine &knotEngine);

    void updateStatus();

    void printStatus(const char *status6, const char *status7);

    void setRePaintStatus(bool status);

    bool getRePaintStatus() const { return _rePaint; }
};

#endif