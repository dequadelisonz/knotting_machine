#ifndef Actuator_hpp
#define Actuator_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/

/*This project includes*/
#include "DigitalInput.hpp"
#include "GPOut.hpp"

class Actuator
{

public:
    static const uint8_t MAX_CHAR_DESCR = 29U; // define max number of chars for description (including final '\0')

private:
    // declaration order is considering the best memory padding
    const char *TAG = "Actuator";

    float _duration = 0.0f; // time assumed for movement
    char _description[MAX_CHAR_DESCR] = {0};
    uint8_t _coilPin = 0U; // pin number to drive coil
    DigitalInput<Actuator> *const _trigger;

    static const uint8_t _expAddresses[GPOutArray::MAX_EXPANSIONS];
    GPOutArray &_gpOutArr;

    /*pin to read as active when actuator is in A position
    for linear cylinder A is typically the bore side while B is rod side
    for rotary actuators or motors A is CW rotation and B is CCW rotation seen from shaft */

    DigitalInput<Actuator> _endstrokeAInput;
    DigitalInput<Actuator> _endstrokeBInput;

public:
    Actuator(float duration,
             const char *descr,
             uint8_t coilPin,
             DigitalInput<Actuator> *const trigger);
    Actuator();

    void set(uint8_t status);

    void updateStatus();

    void operator=(const Actuator &actuator);

    float getDuration() const { return _duration; }
    uint8_t getCoilPin() const { return _coilPin; }
    const char *getDescription() const { return _description; }
};

#endif