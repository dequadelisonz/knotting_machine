#include "HMI.hpp"
#include "KnotEngine.hpp"

HMI::HMI(KnotEngine &knotEngine) : _knotEngine(knotEngine),
                                   _menu("Main", true),
                                   _menuNavigator(&_menu),
                                   _statusLine(*this)

{
       /*preparing menu entries*/
       _batchQtyME.init(this, "Q.ta lotto");
       _pcsPerMinME.init(this, "Pezzi/min");
       _SDCardUpdME.init(this, "Agg. SDCARD");
       _WifiUpdME.init(this, "Agg. WIFI");

       /*adding action to menu entries*/
       _batchQtyME.setActionUp(&HMI::_increaseBatch);
       _batchQtyME.setActionDown(&HMI::_decreaseBatch);

       _pcsPerMinME.setActionUp(&HMI::_increasePcm);
       _pcsPerMinME.setActionDown(&HMI::_decreasePcm);

       _SDCardUpdME.setActionOK(&HMI::_updateFromSD);
       _WifiUpdME.setActionOK(&HMI::_updateFromWIFI);

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

       _btnUP.init(this, "UP", ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::UP);
       _btnDOWN.init(this, "DOWN", ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::DOWN);
       _btnOK.init(this, "OK", ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::OK);

       _btnStart.init(this, "Start", (gpio_num_t)CONFIG_START_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NO);
       _btnStop.init(this, "Stop", (gpio_num_t)CONFIG_STOP_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NC);
       _selMode.init(this, "ModeSel", (gpio_num_t)CONFIG_MODE_SELECTOR_GPIO, RemoteInputBase::eRemInputLogic::NC);

       /*binding to HMI's functions (actions)*/
       _btnUP.setAction(&HMI::_up);
       _btnDOWN.setAction(&HMI::_down);
       _btnOK.setAction(&HMI::_OK);

       _btnStart.setAction(&HMI::_start);
       _btnStop.setAction(&HMI::_stop);
       _selMode.setActionOn(&HMI::_selON);
       _selMode.setActionOff(&HMI::_selOFF);

       // register remote inputs (buttons,etc.) in input console
       ret = true;
       ret = ret && _inputConsole.addButton(&_btnUP);
       ret = ret && _inputConsole.addButton(&_btnDOWN);
       ret = ret && _inputConsole.addButton(&_btnOK);

       ret = ret && _inputConsole.addButton(&_btnStart);
       ret = ret && _inputConsole.addButton(&_btnStop);
       ret = ret && _inputConsole.addButton(&_selMode);

       if (!ret) // TODO implementare un controllo errori migliore
              ESP_LOGE(TAG,
                       "Some remote inputs were not added, check for maximum remote availability.");

       _display.clearScreen(false);
       _display.contrast(0xff);

       _printScreen();
       // ESP_LOGD(TAG, "Exiting constructor."); // TODO solo per debug
}

void HMI::updateStatus()
{
       _inputConsole.updateStatus();
}

void HMI::_updateFromSD()
{
       _knotEngine._updateFromSD();
}

void HMI::_updateFromWIFI()
{
       _knotEngine._updateFromWIFI();
}

/*functions (actions) to be associated with push buttons and menu entries*/

void HMI::_up()
{
       printf("Up\n");
       _menuNavigator.up();
       _printScreen();
}

void HMI::_down()
{
       printf("Down\n");
       _menuNavigator.down();
       _printScreen();
}

void HMI::_OK()
{
       printf("Ok\n");
       _menuNavigator.ok();
       _printScreen();
}

void HMI::_start()
{
       //printf("Start\n");
       _knotEngine._runningStatus = 1;//TODO mutex!
}

void HMI::_stop()
{
       //printf("Stop\n");
       _knotEngine._runningStatus = 0;//TODO mutex!
}

void HMI::_selON()
{
       //printf("Selector ON\n");
       _knotEngine._selModeStatus = 1;//TODO mutex!
}

void HMI::_selOFF()
{
       //printf("Selector OFF\n");
       _knotEngine._selModeStatus = 0;//TODO mutex!
}

void HMI::_increaseBatch()
{
       _knotEngine.getBatchQty() += 5;
}

void HMI::_decreaseBatch()
{
       if (!(_knotEngine.getBatchQty() < 6))
              _knotEngine.getBatchQty() -= 5;
}

void HMI::_increasePcm()
{
       if (_knotEngine.getPcsPerMin() < _knotEngine.getNomPcsPerMin())
       {
              ++_knotEngine.getPcsPerMin();
              _knotEngine.getKspeed() = _knotEngine.getNomPcsPerMin() / _knotEngine.getPcsPerMin();
       }
}

void HMI::_decreasePcm()
{

       if (_knotEngine.getPcsPerMin() > _knotEngine.getMinPcsPerMin())
       {
              --_knotEngine.getPcsPerMin();
              _knotEngine.getKspeed() = _knotEngine.getNomPcsPerMin() / _knotEngine.getPcsPerMin();
       }
}

void HMI::_printScreen()
{
       _display.clearScreen(false);
       _setMenuCanvas();
       char status[_CHARS_IN_ROW] = {0};
       char batch[(_CHARS_IN_ROW - 1) / 2] = {0};
       char ppm[(_CHARS_IN_ROW - 1) / 2] = {0};
       sprintf(batch, "%d", _knotEngine._batchQty);
       sprintf(ppm, "%d", _knotEngine._pcsPerMin);
       strcat(status, "L:");
       strcat(status, batch);
       strcat(status, "-");
       strcat(status, "p/m:");
       strcat(status, ppm);
       printStatus(status);
}

void HMI::_setMenuCanvas()
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
              char disEntry[_CHARS_IN_ROW] = "[D] ";
              if (!active)
                     strcat(disEntry, &row[(uint8_t)selected + 1]); // if selected, copy the string starting from 2nd index
              strcpy(_screenContent[i],
                     (active ? &row[(uint8_t)selected] : disEntry)); // if row starts with '>' then hilight and start copy after the square bracket
              _display.displayText(i, _screenContent[i], /* strlen(_screenContent[i]),*/ selected);
              row = strtok(NULL, delimiter);
       }
}
