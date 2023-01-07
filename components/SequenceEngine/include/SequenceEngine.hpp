#ifndef SEQUENCEENGINE_HPP
#define SEQUENCEENGINE_HPP

/*ESP-IDF includes*/
#include <thread>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include <esp_pthread.h>

class SequenceEngine
{

private:
    const char *TAG = "SequenceEngine";

    esp_pthread_cfg_t createConfig(const char *name, int core_id, int stack, int prio)
    {
        auto cfg = esp_pthread_get_default_config();
        cfg.thread_name = name;
        cfg.pin_to_core = core_id;
        cfg.stack_size = stack;
        cfg.prio = prio;
        return cfg;
    }

    void controlThread()
    {
        if (!onControlCreate())
        {
            _isRunning = false;
            ESP_LOGE(TAG, "Something with control initialization went wrong");
        }

        uint64_t tp1 = esp_timer_get_time();
        uint64_t tp2 = esp_timer_get_time();
        uint64_t elapsedTime;
        while (_isRunning)
        {
            tp2 = esp_timer_get_time();
            elapsedTime = tp2 - tp1;
            tp1 = tp2;
            _isRunning = onControlUpdate(elapsedTime);
            vTaskDelay(1);
        }
    }

    void sequenceThread()
    {
        if (!onSequenceCreate())
        {
            _isRunning = false;
            ESP_LOGE(TAG, "Something with sequence initialization went wrong");
        }

        uint64_t tp1 = esp_timer_get_time();
        uint64_t tp2 = esp_timer_get_time();
        uint64_t elapsedTime;
        while (_isRunning)
        {
            tp2 = esp_timer_get_time();
            elapsedTime = tp2 - tp1;
            tp1 = tp2;
            _isRunning = onSequenceUpdate(elapsedTime);
            vTaskDelay(1);
        }
    }

protected:
    bool _isRunning = true;

public:
    virtual bool onControlCreate() = 0;
    virtual bool onControlUpdate(uint64_t elapsedTime) = 0;
    virtual bool onSequenceCreate() = 0;
    virtual bool onSequenceUpdate(uint64_t elapsedTime) = 0;
    // virtual bool onUserDestroy() { return true; }

    virtual void fun(){}

    SequenceEngine()
    {
        ESP_LOGI(TAG, "Creating a new CycleEngine");
    }

    void start()
    {

        ESP_LOGI(TAG, "Starting the engine.");
        auto cfg = createConfig("Control thread", 1, 8000, 5);
        esp_pthread_set_cfg(&cfg);

        ESP_LOGI(TAG, "Creating ctrl thread");

        std::thread ctrlThread(&SequenceEngine::controlThread, this);

        cfg = createConfig("Sequence thread", 0, 12000, 5);
        esp_pthread_set_cfg(&cfg);
        ESP_LOGI(TAG, "Creating seq thread");
        std::thread seqThread(&SequenceEngine::sequenceThread, this);

        ctrlThread.join();
        seqThread.join();
    };
};

#endif