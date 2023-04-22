#include "Actuator.hpp"

const uint8_t Actuator::_expAddresses[GPOutArray::MAX_EXPANSIONS] = {0x27U, 0x23U, 0x0U, 0x0U, 0x0U, 0x0U}; // TODO inserire in menuconfig

Actuator::Actuator(float duration,
                   const char *descr,
                   uint8_t coilPin,
                   DigitalInput<Actuator> *const trigger) : _duration(duration),
                                                            _coilPin(coilPin),
                                                            _trigger(trigger),
                                                            _gpOutArr(GPOutArray::_getInstance(_expAddresses))

{
    strncat(_description, descr, MAX_CHAR_DESCR - 1);
}

Actuator::Actuator() : Actuator(0.0, "", 0, nullptr)
{
}

void Actuator::set(uint8_t status)
{
    _gpOutArr.set(_coilPin, status);
}

void Actuator::updateStatus()
{
    _endstrokeAInput.updateStatus();
    _endstrokeBInput.updateStatus();
}

void Actuator::operator=(const Actuator &actuator)
{
    this->_duration = actuator.getDuration();
    this->_coilPin = actuator.getCoilPin();
    strcpy(this->_description, actuator.getDescription());
}