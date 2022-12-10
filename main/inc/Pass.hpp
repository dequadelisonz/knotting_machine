#ifndef PASS_HPP
#define PASS_HPP

//TODO clean up some includes
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "string.h"

class Pass

{
    friend class Sequencer;

private:
    const char *TAG = "Pass";
    static const int MAX_CHAR_DESCR = 31; // define max number of chars for description (including final '\0')

    unsigned int _id;
    unsigned int _pin;
    double _duration;
    char _description[MAX_CHAR_DESCR];
    bool _status;

public:
    Pass() : _id(0),
             _pin(0),
             _duration(0.0),
             _description("EMPTY"),
             _status(false)
    {
    }

    unsigned int getId() const
    {
        return this->_id;
    }

    unsigned int getPin() const
    {
        return this->_pin;
    }

    double getDuration() const
    {
        return this->_duration;
    }

    bool getStatus() const
    {
        return this->_status;
    }

    const char *getDescription() const
    {
        return this->_description;
    }

    void operator=(const Pass &pass)
    {
        //ESP_LOGI(TAG, "Copying...");
        this->_id = pass.getId();
        this->_pin = pass.getPin();
        this->_duration = pass.getDuration();
        strcpy(this->_description, pass.getDescription());
        this->_status = pass.getStatus();
    }

    void logContent()
    {
        ESP_LOGI(TAG, "\t\tPass id: %d", _id);
        ESP_LOGI(TAG, "\t\tPin: %d", _pin);
        ESP_LOGI(TAG, "\t\tDuration: %4.3f", _duration);
        ESP_LOGI(TAG, "\t\tDescription: %s", _description);
        ESP_LOGI(TAG, "\t\tStatus: %d", _status);
    }

    // ~Pass()
    // {
    //     ESP_LOGI(TAG, "Killing a pass...");
    //     vTaskDelay(1);
    // }
};

#endif