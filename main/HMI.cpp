#include "HMI.hpp"
#include "KnotEngine.hpp"

pthread_mutex_t HMI::_rePaintStatusM = 0;

HMI::HMI(KnotEngine &knotEngine) : _knotEngine(knotEngine),
                                   _menu("Main", true),
                                   _menuNavigator(&_menu),
                                   _sl6(*this, 6),
                                   _sl7(*this, 7)

{
       ESP_LOGI(TAG, "Constructing HMI...");

       if (pthread_mutex_init(&HMI::_rePaintStatusM, NULL) != 0)
       {
              ESP_LOGE(TAG, "Failed to init mutex");
       }

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

       _btnUP.init(ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::UP);
       _btnDOWN.init(ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::DOWN);
       _btnOK.init(ADC1_CHANNEL_4, RemoteInputBase::eBtnLevel::OK);

       _btnStart.init((gpio_num_t)CONFIG_START_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NO);
       _btnStop.init((gpio_num_t)CONFIG_STOP_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NC);
       _selMode.init((gpio_num_t)CONFIG_MODE_SELECTOR_GPIO, RemoteInputBase::eRemInputLogic::NO);

       /*binding to HMI's functions (actions)*/
       _btnUP.setAction(this, &HMI::_up);
       _btnDOWN.setAction(this, &HMI::_down);
       _btnOK.setAction(this, &HMI::_OK);

       _btnStart.setAction(this, &HMI::_start);
       _btnStop.setAction(this, &HMI::_stop);
       _selMode.setActionOn(this, &HMI::_selON);
       _selMode.setActionOff(this, &HMI::_selOFF);

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

       _display.displayText(0, "Macchina", true);
       _display.displayText(1, "     Annodatrice", true);
       _display.displayText(3, "*** v. 1.0 ***", true);

       // ESP_LOGD(TAG, "Exiting constructor."); // TODO solo per debug
}

void HMI::updateStatus()
{
       _inputConsole.updateStatus();

       if (_knotEngine.getRunStatus() == 1U)
       //(_knotEngine.getRunningStatus() == 1U)
       {
              _WifiUpdME.setIsActive(false);
              _SDCardUpdME.setIsActive(false);
       }
       else if ( //(_knotEngine.getRunningStatus() == 0U) &&
           _knotEngine.getRunStatus() == 0U)
       {
              _WifiUpdME.setIsActive(true);
              _SDCardUpdME.setIsActive(true);
       };

       if (getRePaintStatus())
       {
              // ESP_LOGI(TAG, "Repainting...");
              _printScreen();
              setRePaintStatus(false);
       }
}

void HMI::printStatus(const char *status6, const char *status7)
{
       _sl6.printStatus(status6);
       _sl7.printStatus(status7);
}

void HMI::_updateFromSD()
{
       _menuNavigator.freezeMenu();
       _knotEngine._updateFromSD();
       _menuNavigator.unFreezeMenu();
}

void HMI::_updateFromWIFI()
{
       _menuNavigator.freezeMenu();
       _knotEngine._updateFromWIFI();
       _menuNavigator.unFreezeMenu();
}

/*functions (actions) to be associated with push buttons and menu entries*/

void HMI::_up()
{
       printf("Up\n");
       _menuNavigator.up();
       setRePaintStatus(true);
}

void HMI::_down()
{
       printf("Down\n");
       _menuNavigator.down();
       setRePaintStatus(true);
}

void HMI::_OK()
{
       printf("Ok\n");
       _menuNavigator.ok();
       setRePaintStatus(true);
}

void HMI::_start()
{
       printf("Start ON\n");
       _knotEngine.setRunStatus(1U);
       setRePaintStatus(true);
}

void HMI::_stop()
{
       printf("Stop ON\n");
       _knotEngine.setRunStatus(0U);
       setRePaintStatus(true);
}

void HMI::_selON()
{
       printf("Selector ON\n");
       _knotEngine.setSelMode(1U);
       setRePaintStatus(true);
}

void HMI::_selOFF()
{
       printf("Selector OFF\n");
       _knotEngine.setSelMode(0U);
       setRePaintStatus(true);
}

void HMI::_increaseBatch()
{
       _knotEngine.getBatchQty() += 1; // TODO rivedere in funzione di utilizzo
}

void HMI::_decreaseBatch()
{
       // if (!(_knotEngine.getBatchQty() < 6))//TODO rivedere in funzione di utilizzo
       _knotEngine.getBatchQty() -= 1;
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
       char status6[_CHARS_IN_ROW] = {0};
       char remItems[(_CHARS_IN_ROW - 1) / 2] = {0};
       char autoMode[(_CHARS_IN_ROW - 1) / 2] = {0};
       sprintf(remItems, "%d", _knotEngine._batchQty - _knotEngine._processedItems);
       bool st = false;
       if (_knotEngine.getSelMode() == 1)
              st = true;
       sprintf(autoMode, "%s", st ? "auto" : "man.");
       strcat(status6, "R:");
       strcat(status6, remItems);
       strcat(status6, "-");
       strcat(status6, "modo:");
       strcat(status6, autoMode);

       char status7[_CHARS_IN_ROW] = {0};
       char batch[(_CHARS_IN_ROW - 1) / 2] = {0};
       char ppm[(_CHARS_IN_ROW - 1) / 2] = {0};
       sprintf(batch, "%d", _knotEngine._batchQty);
       sprintf(ppm, "%d", _knotEngine._pcsPerMin);
       strcat(status7, "L:");
       strcat(status7, batch);
       strcat(status7, "-");
       strcat(status7, "p/m:");
       strcat(status7, ppm);
       printStatus(status6, status7);
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

void HMI::setRePaintStatus(bool status)
{
       // ESP_LOGI(TAG, "Set repaint status to %s from thread:%s\n", status ? "true" : "false", pcTaskGetName(nullptr));
       if (pthread_mutex_lock(&_rePaintStatusM) == 0)
       {
              _rePaint = status;
              // Unlock once operations are done
              pthread_mutex_unlock(&_rePaintStatusM);
       }
}
