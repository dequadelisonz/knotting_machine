#ifndef Cycle_hpp
#define Cycle_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "driver/gpio.h"

/*This project includes*/
#include "Pass.hpp"

class Cycle
{
    friend class Sequencer;

private:
    const char *TAG = "Cycle";
    static const int16_t MAX_PASSES = 150U; // define max number of groups in cycle

    uint16_t _lastId = 0U;

    Pass _passes[MAX_PASSES];

    Cycle();
    esp_err_t _pushPass(Pass const &pass);
    void _reset();
    void _logContent();

    const Pass _nullPass;

public:

};

#endif