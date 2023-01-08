#include "InputConsole.hpp"

// return false if there's no more space for further buttons (default max = 10)
bool InputConsole::addButton(RemoteInputBase *button)
{
    bool ret = false;
    if (_top < MAX_INPUTS)
    {
        _butArr[_top] = button;
        ++_top;
        ret = true;
    }
    return ret;
}