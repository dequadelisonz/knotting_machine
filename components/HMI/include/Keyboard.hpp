#ifndef Keyboard_hpp
#define Keyboard_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "Button.hpp"

class Keyboard
{
private:
    const char *TAG = "Keyboard";

    static const uint8_t MAX_BUTTONS = 16; // max nr. of buttons in keyboard

    uint8_t _top = 0;

    Button *_butArr[MAX_BUTTONS];

public:
    Keyboard()
    {
    }

    // return false if there's no more space for further buttons (default max = 10)
    bool addButton(Button *button)
    {
        bool ret = false;
        if (_top < MAX_BUTTONS)
        {
            _butArr[_top] = button;
            ++_top;
            ret = true;
        }
        return ret;
    }

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