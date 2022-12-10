#ifndef GROUP_HPP
#define GROUP_HPP

#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"

#include "string.h"
#include "Pass.hpp"

class Group
{

    friend class Sequencer;

private:
    const char *TAG = "Group";
    static const int MAX_PASSES = 8; // define max number of passes in group

    unsigned int _lastId, _myId;
    double _groupOffset, _groupDuration;
    Pass _passes[MAX_PASSES];

public:
    Group() : _lastId(0),
              _groupOffset(0.0),
              _groupDuration(0.0)
    {
    }

    void setMyId(unsigned int id)
    {
        this->_myId = id;
    }

    esp_err_t pushPass(Pass &pass)
    {
        esp_err_t ret = ESP_OK;
        if (_lastId > MAX_PASSES)
        {
            ret = ESP_ERR_INVALID_SIZE;
            ESP_LOGE(TAG, "Max nr. of passes in group array was exceeded,"
                          " current _lastId is: %d and max available items is: %d",
                     _lastId, MAX_PASSES);
        }
        else
        {
            _passes[_lastId] = pass;
            ++_lastId;
            ESP_LOGI(TAG, "Pushing a pass, _lastId now is: %d", _lastId);
        }
        return ret;
    }

    double getGroupDuration() const
    {
        return this->_groupDuration;
    }

    double getGroupOffset() const
    {
        return this->_groupOffset;
    }

    unsigned int getLastId() const
    {
        return this->_lastId;
    }

    void resetGroup()
    {
        this->_lastId = 0;
    }

    void operator=(const Group &group)
    {
        ESP_LOGI(TAG, "Copying...");
        this->_groupDuration = group.getGroupDuration();
        this->_groupOffset = group.getGroupOffset();
        this->_myId = group._myId;

        unsigned int last = group.getLastId() + 1;
        for (int i = 0; i <= last; ++i)
        {
            this->_passes[i] = group._passes[i];
        }
        this->_lastId = group._lastId;
    }

    void logContent()
    {
        ESP_LOGI(TAG, "Logging...Group _lastId: %d", _lastId);
        ESP_LOGI(TAG, "\tGroup Offset: %4.3f", _groupOffset);
        ESP_LOGI(TAG, "\tGroup Duration: %4.3f", _groupDuration);
        for (int i = 0; i <= _lastId - 1; ++i)
        {
            ESP_LOGI(TAG, "Logging pass %d\n", i);
            _passes[i].logContent();
        }
    }

    // ~Group()
    // {
    //     ESP_LOGI(TAG, "Killing group... %d", _myId);
    //     vTaskDelay(1);
    // }
};

#endif