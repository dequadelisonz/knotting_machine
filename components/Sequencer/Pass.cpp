#include "Pass.hpp"

Pass::Pass()
{
}

Pass::Pass(int id,
           float grDur,
           float offset,
           float dur,
           gpio_num_t pin,
           const char *descr,
           bool status) : _id(id),
                          _groupDuration(grDur),
                          _offset(offset),
                          _duration(dur),
                          _pin(pin),
                          _status(status)

{
    // clipping name to max allowed length
    int len = (strlen(descr) >= (MAX_CHAR_DESCR - 1)) ? (MAX_CHAR_DESCR - 1) : strlen(descr);
    memcpy(_description, descr, len);
    _description[len] = '\0';
    //gpio_reset_pin(_pin);
    //gpio_set_direction(_pin, GPIO_MODE_OUTPUT);
}

void Pass::setOutput()
{
    //gpio_set_level(_pin, _status);
}

void Pass::operator=(const Pass &pass)
{
    this->_id = pass.getId();
    this->_groupDuration = pass.getGroupDuration();
    this->_offset = pass.getOffset();
    this->_duration = pass.getDuration();
    this->_pin = pass.getPin();
    strcpy(this->_description, pass.getDescription());
    this->_status = pass.getStatus();
}

void Pass::logContent()
{
    ESP_LOGI(TAG, "\t\tPass id: %d", _id);
    ESP_LOGI(TAG, "\t\tGroup duration: %4.3f", _groupDuration);
    ESP_LOGI(TAG, "\t\tOffset: %4.3f", _offset);
    ESP_LOGI(TAG, "\t\tDuration: %4.3f", _duration);
    ESP_LOGI(TAG, "\t\tPin: %d", _pin);
    ESP_LOGI(TAG, "\t\tDescription: %s", _description);
    ESP_LOGI(TAG, "\t\tStatus: %d", _status);
}