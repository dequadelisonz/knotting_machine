#ifndef Pass_hpp
#define Pass_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "esp_log.h"

class Pass
{
    friend class Cycle;

public:
    static const uint8_t MAX_CHAR_DESCR = 31U; // define max number of chars for description (including final '\0')

private:
    const char *TAG = "Pass";

    uint8_t _id = 0U;
    float _groupDuration = 0.0f;
    float _offset = 0.0f;
    float _duration = 0.0f;
    uint8_t _pin = 0U;
    char _description[MAX_CHAR_DESCR] = {0};
    bool _status = false;

    void _logContent();

public:
    Pass();

    Pass(int id,
         float grDur,
         float offset,
         float dur,
         uint8_t pin,
         const char *descr,
         bool status);

    void operator=(const Pass &pass);

    int getId() const { return _id; }
    float getGroupDuration() const { return _groupDuration; }
    float getOffset() const { return _offset; }
    const char *getDescription() const { return _description; }
    float getDuration() const { return _duration; }
    uint8_t getPin() const { return _pin; }
    bool getStatus() const { return _status; }
};

#endif