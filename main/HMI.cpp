#include "HMI.hpp"
#include "KnotEngine.hpp"

HMI::HMI(KnotEngine &knotEngine) : _knotEngine(knotEngine),
                                   _menu("Main", true),
                                   _menuNavigator(&_menu)

{
       /*preparing menu entries*/
       _batchQtyME.init(this, "Q.ta lotto");
       _pcsPerMinME.init(this, "Pezzi/min");
       _SDCardUpdME.init(this, "Agg. SDCARD");
       _WifiUpdME.init(this, "Agg. WIFI");

       _WifiUpdME.isActive() = false;

       _batchQtyME.setActionOK(&HMI::_enableWifiME);

       /*adding entries to menu instance*/
       bool ret = true;
       ret = ret && _menu.pushEntry(&_batchQtyME);
       ret = ret && _menu.pushEntry(&_pcsPerMinME);
       ret = ret && _menu.pushEntry(&_SDCardUpdME);
       ret = ret && _menu.pushEntry(&_WifiUpdME);

       if (!ret) // TODO implementare un controllo errori migliore
              ESP_LOGE(TAG,
                       "Some menu entry were not added, check for maximum menu entry availability.");

       /*preparing hardware remote inputs (buttons, etc.)*/
       //_btnStart.init(this, "Start", (gpio_num_t)CONFIG_START_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NO);
       //_btnStop.init(this, "Stop", (gpio_num_t)CONFIG_STOP_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NC);
       _btnUP.init(this, "UP", ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::UP);
       _btnDOWN.init(this, "DOWN", ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::DOWN);
       _btnOK.init(this, "OK", ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::OK);
       //_selMode.init(this, "ModeSel", (gpio_num_t)CONFIG_MODE_SELECTOR_GPIO, RemoteInputBase::eRemInputLogic::NC);

       /*binding to HMI's functions (actions)*/
       //_btnStart.setAction(&HMI::_start);
       //_btnStop.setAction(&HMI::_stop);
       _btnUP.setAction(&HMI::_up);
       _btnDOWN.setAction(&HMI::_down);
       _btnOK.setAction(&HMI::_OK);
       //_selMode.setActionOn(&HMI::_selON);
       //_selMode.setActionOff(&HMI::_selOFF);

       // register remote inputs (buttons,etc.) in input console
       ret = true;
       // ret = ret && _inputConsole.addButton(&_btnStart);
       // ret = ret && _inputConsole.addButton(&_btnStop);
       ret = ret && _inputConsole.addButton(&_btnUP);
       ret = ret && _inputConsole.addButton(&_btnDOWN);
       ret = ret && _inputConsole.addButton(&_btnOK);
       // ret = ret && _inputConsole.addButton(&_selMode);
       if (!ret) // TODO implementare un controllo errori migliore
              ESP_LOGE(TAG,
                       "Some remote inputs were not added, check for maximum remote availability.");

       _display.clearScreen(false);
       _display.contrast(0xff);

       printScreen();
       ESP_LOGD(TAG, "Exiting constructor."); // TODO solo per debug
}

void HMI::updateStatus()
{
       _inputConsole.updateStatus();
       // printScreen(_screenContent);
}

void HMI::printScreen()
{
       _display.clearScreen(false);
       _setScreenContent();

       char status[12] = {0};
       strcat(status, "Status: ");
       strcat(status, _okPressed ? "1" : "0");

       _display.displayText(7, status, false);
       // printf(_screenContent);
       // printf("\nLotto: %d pz;\tPezzi/min:%d\n%s\t%s\n",
       //        _knotEngine._batchQty,
       //        _knotEngine._pcsPerMin,
       //        ((_knotEngine._runningStatus == 1) ? "In funzione" : "Stop"),
       //        ((_knotEngine._selModeStatus == 1) ? "Automatico" : "Manuale"));
}

void HMI::_setScreenContent()
{
       char temp[_NR_OF_ROWS * _CHARS_IN_ROW] = {0};
       strcpy(temp, _menuNavigator.getPrintout());
       const char delimiter[] = "\n";
       char *row = strtok(temp, delimiter);
       for (uint8_t i = 0;
            ((i < _NR_OF_ROWS) && (row != NULL));
            ++i)
       {
              bool selected = (row[0] == '>'); // check if menu entry is selected
              bool active = (row[0] != '%');   // check if menu entry is active
              char disEntry[16] = "[D] ";
              if (!active)
                     strcat(disEntry, &row[(uint8_t)selected]);
              strcpy(_screenContent[i],
                     (active ? &row[(uint8_t)selected] : disEntry)); // if row starts with '>' then hilight and start copy after the square bracket
              _display.displayText(i, _screenContent[i], /* strlen(_screenContent[i]),*/ selected);
              row = strtok(NULL, delimiter);
       }
}

// TODO cancellare?
//  void HMI::printScreen(const char *content)
//  {
//         clearScreen();
//         printf(content);
//  }

/*functions (actions) to be associated with push buttons and menu entries*/

// void HMI::_start()
// {
//        // printf("Start\n");
//        _knotEngine._runningStatus = 1;
// }

// void HMI::_stop()
// {
//        // printf("Stop\n");
//        _knotEngine._runningStatus = 0;
// }

void HMI::_up()
{
       printf("Up\n");
       _menuNavigator.up();
       printScreen();
}

void HMI::_down()
{
       printf("Down\n");
       _menuNavigator.down();
       printScreen();
}

void HMI::_OK()
{
       printf("Ok\n");
       _menuNavigator.ok();
       _okPressed = !_okPressed;
       printScreen();
}

// void HMI::_selON()
// {
//        // printf("Selector ON\n");
//        _knotEngine._selModeStatus = 1;
// }

// void HMI::_selOFF()
// {
//        // printf("Selector OFF\n");
//        _knotEngine._selModeStatus = 0;
// }
