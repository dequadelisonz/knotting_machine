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
    static const int MAX_PASSES = 200; // define max number of groups in cycle

    unsigned int _lastId = 0;

    Pass _passes[MAX_PASSES];

    Cycle();
    esp_err_t _pushPass(Pass const &pass);
    void reset();
    void _logContent();

    Pass _nullPass;


public:
};

#endif