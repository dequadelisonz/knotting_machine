#ifndef CYCLE_HPP
#define CYCLE_HPP

// #include "esp_log.h"

// #include "esp_err.h"
// #include "esp_system.h"

#include "string.h"
#include "Group.hpp"

class Cycle
{

    friend class Sequencer;

private:
    const char *TAG = "Cycle";
    static const int MAX_GROUPS = 100; // define max number of groups in cycle

    unsigned int _lastId = 0;
    Group _groups[MAX_GROUPS];

public:
    Cycle()
    {
    }

    esp_err_t pushGroup(Group &group)
    {
        esp_err_t ret = ESP_OK;
        if (_lastId > MAX_GROUPS)
        {
            ESP_LOGE(TAG, "Max nr. of groups in cycle array was exceeded"
                          " current _lastId is: %d and max available items is: %d",
                     _lastId, MAX_GROUPS);
            ret = ESP_ERR_INVALID_SIZE;
        }
        else
        {
            _groups[_lastId] = group;
            ++_lastId;
            ESP_LOGI(TAG, "Pushing a group, _lastId now is: %d", _lastId);
        }
        return ret;
    }


    void logContent()
    {
        for (int i = 0; i < _lastId; ++i)
        {
            ESP_LOGI(TAG, "Logging group %d\n", i);
            _groups[i].logContent();
        }
        ESP_LOGI(TAG, "****************LOGGING END***************");
    }
};

#endif