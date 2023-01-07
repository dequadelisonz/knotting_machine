#ifndef Pass_hpp
#define Pass_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "esp_log.h"
#include "driver/gpio.h"

class Pass
{

public:
    static const uint8_t MAX_CHAR_DESCR = 31; // define max number of chars for description (including final '\0')

private:
      const char *TAG = "Pass";

    int _id;
    float _groupDuration;
    float _offset;
    float _duration;
    gpio_num_t _pin;
    char _description[MAX_CHAR_DESCR];
    bool _status;

public:

    Pass();

    Pass(int id,
         float grDur,
         float offset,
         float dur,
         gpio_num_t pin,
         const char * descr,
         bool status);

    void setOutput();
    void operator=(const Pass &pass);
    void logContent();

    int getId() const { return _id; }
    float getGroupDuration() const { return _groupDuration; }
    float getOffset() const { return _offset; }
    const char *getDescription() const { return _description; }
    float getDuration() const { return _duration; }
    gpio_num_t getPin() const { return _pin; }
    bool getStatus() const { return _status; }
};

#endif