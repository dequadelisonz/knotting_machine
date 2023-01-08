#include "HMI.hpp"
#include "KnotEngine.hpp"

HMI::HMI(KnotEngine &knotEngine) : _knotEngine(knotEngine),
                                   _menu(*this)

{
       /*preparing menu entries*/
       _batchQtyME.init(this, "Quantità lotto");
       _pcsPerMinME.init(this, "Pezzi/min");
       _SDCardUpdME.init(this, "Aggiornamento SDCARD");
       _WifiUpdME.init(this, "Aggiornamento WIFI");

       /*adding entries to menu instance*/
       _menu.pushBack(&_batchQtyME);
       _menu.pushBack(&_pcsPerMinME);
       _menu.pushBack(&_SDCardUpdME);
       _menu.pushBack(&_WifiUpdME);

       /*preparing hardware remote inputs (buttons, etc.)*/
       _btnStart.init(this, "Start", (gpio_num_t)CONFIG_START_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NO);
       _btnStop.init(this, "Stop", (gpio_num_t)CONFIG_STOP_BUTTON_GPIO, RemoteInputBase::eRemInputLogic::NC);
       _btnUP.init(this, "UP", ADC1_CHANNEL_0, RemoteInputBase::eBtnLevel::UP);
       _btnDOWN.init(this, "DOWN", ADC1_CHANNEL_0, RemoteInputBase::eBtnLevel::DOWN);
       _btnOK.init(this, "OK", ADC1_CHANNEL_0, RemoteInputBase::eBtnLevel::OK);
       _selMode.init(this, "ModeSel", (gpio_num_t)CONFIG_MODE_SELECTOR_GPIO, RemoteInputBase::eRemInputLogic::NC);

       /*binding to HMI's functions (actions)*/
       _btnStart.setAction(&HMI::_start);
       _btnStop.setAction(&HMI::_stop);
       _btnUP.setAction(&HMI::_up);
       _btnDOWN.setAction(&HMI::_down);
       _btnOK.setAction(&HMI::_OK);
       _selMode.setActionOn(&HMI::_selON);
       _selMode.setActionOff(&HMI::_selOFF);

       // register remote inputs (buttons,etc.) in input console
       bool ret = true;
       ret = ret && _inputConsole.addButton(&_btnStart);
       ret = ret && _inputConsole.addButton(&_btnStop);
       ret = ret && _inputConsole.addButton(&_btnUP);
       ret = ret && _inputConsole.addButton(&_btnDOWN);
       ret = ret && _inputConsole.addButton(&_btnOK);
       ret = ret && _inputConsole.addButton(&_selMode);
       if (!ret) //TODO implementare un controllo errori migliore
              ESP_LOGE(TAG,
              "Some remote inputs were not added, check for maximum remote availability.");
       printScreen();
       ESP_LOGI(TAG, "Exiting constructor.");
}

void HMI::updateStatus()
{
       _inputConsole.updateStatus();
       printScreen();
}

void HMI::printScreen()
{
       printf("\033[2J"); /*  clear the screen  */
       printf("\033[H");  /*  position cursor at top-left corner */
       _menu.printMenu();
       printf("\nLotto: %d pz;\tPezzi/min:%d\n%s\t%s\n",
              _knotEngine._batchQty,
              _knotEngine._pcsPerMin,
              ((_knotEngine._runningStatus == 1) ? "In funzione" : "Stop"),
              ((_knotEngine._selModeStatus == 1) ? "Automatico" : "Manuale"));
}

/*functions (actions) to be associated with push buttons and menu entries*/
void HMI::_start()
{
       // printf("Start\n");
       _knotEngine._runningStatus = 1;
}

void HMI::_stop()
{
       printf("Stop\n");
       _knotEngine._runningStatus = 0;
}

void HMI::_up()
{
       // printf("Up\n");
       _menu.backward();
       // printScreen();
}

void HMI::_down()
{
       // printf("Down\n");
       _menu.forward();
       // printScreen();
}

void HMI::_OK()
{
       printf("Ok\n");
}

void HMI::_selON()
{
       // printf("Selector ON\n");
       _knotEngine._selModeStatus = 1;
}

void HMI::_selOFF()
{
       // printf("Selector OFF\n");
       _knotEngine._selModeStatus = 0;
}
