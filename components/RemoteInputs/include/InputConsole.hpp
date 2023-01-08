#ifndef InputConsole_hpp
#define InputConsole_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "RemoteInput.hpp"

class InputConsole
{
private:
    const char *TAG = "InputConsole";

    static const uint8_t MAX_INPUTS = 16; // max nr. of buttons in keyboard

    uint8_t _top = 0;

    RemoteInputBase *_butArr[MAX_INPUTS];

public:
    InputConsole() { }

    // return false if there's no more space for further buttons (default max = 10)
    bool addButton(RemoteInputBase *button);

    void updateStatus()
    {
        //ESP_LOGI(TAG,"updateStatus");
        for (int i = 0; i < _top; ++i)
        {
            _butArr[i]->updateStatus();
        }
    }
};

#endif