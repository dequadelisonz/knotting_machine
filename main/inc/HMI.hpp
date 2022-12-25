#ifndef HMI_HPP
#define HMI_HPP

/*ESP-IDF includes*/

// #include "esp_err.h"
// #include "esp_system.h"
// #include "esp_log.h"

/*This project includes*/
#include "KnotCycle.hpp"
#include "MenuEntry.hpp"
#include "Keyboard.hpp"


class HMI
{
private:
    const char *TAG = "HMI";

    const int DELTA = 1;

    int _dummyRef = 0;

    int _prod = 0, _batch = 0;
    int _maxProd;

    MenuEntry _batchQty;
    HrlyPieces _hourlyPcs;
    SDCardUpdate _SDCUpd;
    WifiUpdate _wifiUpd;
    MenuEntry _blank;

    MenuEntry *_currentEntry;

    Keyboard _keyboard;

    KnotCycle &_context;

public:
    HMI(KnotCycle &context, int &maxProd) : _batchQty("Quantità lotto", _batch),
                                            _hourlyPcs("Pezzi/ora", _prod),
                                            _SDCUpd("Aggiornamento SDCARD"),
                                            _wifiUpd("Aggiornamento WIFI"),
                                            _blank("", _dummyRef),
                                            _currentEntry(&_batchQty),
                                            _context(context),
                                            _maxProd(maxProd)
    {
        // building the menu structure
        _batchQty.setNext(&_hourlyPcs);

        _hourlyPcs.setNext(&_SDCUpd);

        _SDCUpd.setNext(&_wifiUpd);

        _wifiUpd.setNext(&_batchQty);

        ESP_LOGI(TAG, "Hourly production got from cycle: %d", _prod); // debug only

        printScreen();
    }

    void updateHMI(const float &elapsedTime)
    {
        _keyboard.updateButtonStatus(elapsedTime);

        if (!_keyboard.getButtonStatus(Keyboard::Button::START))
            _context._runningStatus = 1; // if start button (NO) is read as 1 set running status as 1
        if (_keyboard.getButtonStatus(Keyboard::Button::STOP))
            _context._runningStatus = 0; // if stop button (NC) is read as 1 set running status as 0

        // selector in mode 0 is for step by step manual mode
        // selector in mode 1 is for automatic advancing

        _context._selModeStatus = _keyboard.getSelMode();
        if (_context._selModeStatus == 0)
            _context._isAtBegin = true; // if selector is set to manual mode set begin status as true to prevent automatic restart if selector is set back to 1

        if (_keyboard.getButtonStatus(Keyboard::Button::DOWN))
        {
            down();
        }

        if (_keyboard.getButtonStatus(Keyboard::Button::UP))
        {
            up();
        }

        if (_keyboard.getButtonStatus(Keyboard::Button::OK))
        {
            if (!selectEntry())
            {
                _context._batch = _batchQty.getValue();
                _context._prod = _hourlyPcs.getValue();
                // TODO implement speed control
            };
        }
    }

    void setMaxProd(int prod)
    {
        _prod = prod;
    }

    void down()
    {
        if (_currentEntry->isActive())
        {
            _currentEntry->updateValue(-1 * DELTA);
        }
        else
        {
            _currentEntry = _currentEntry->getNext();
        }
        printScreen();
    }

    void up()
    {
        if (_currentEntry->isActive())
        {
            _currentEntry->updateValue(DELTA);
        }
        else
        {
            _currentEntry = _currentEntry->getPrev();
        }
        printScreen();
    }

    bool selectEntry()
    {
        bool status = _currentEntry->isActive();
        _currentEntry->setActive(!status);
        printScreen();
        return !status;
    }

    void setBatch(int batch)
    {
        _batch = batch;
    }

    int getBatch() const
    {
        return _batch;
    }

    void printScreen()
    {

        // printf("\033[2J"); /*  clear the screen  */
        // printf("\033[H");  /*  position cursor at top-left corner */
        printf("***\n");
        MenuEntry *me = &_wifiUpd;
        do
        {
            me = me->getNext();
            if (me == _currentEntry)
                printf("*");
            printf("%s", me->getDescription());
            if (me->isActive())
                printf(" %d", _currentEntry->getValue());
            printf("\n");

        } while (me->getNext() != &_batchQty);
    }
};

#endif