#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

/*STD library includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "sdkconfig.h"

/*This project includes*/
#include "Importer.hpp"
#include "Cycle.hpp"

// /* only for testing */
// static const char CYCLE_LIT[] = "GROUP:0;0.2\n"
//                                 "PASS:1;1-Led bianco Accensione A+;0.2;4;1\n"
//                                 "GROUP:0.2;0.2\n"
//                                 "PASS:2;2-Led bianco Spegnimento A-;0.2;4;0\n"
//                                 "GROUP:0.4;0.2\n"
//                                 "PASS:3;3-Led giallo Accensione A+;0.2;5;1\n"
//                                 "GROUP:0.6;0.2\n"
//                                 "PASS:4;4-Led giallo Spegnimento A-;0.2;5;0\n"
//                                 "GROUP:0.8;0.2\n"
//                                 "PASS:5;5-Led verde Accensione A+;0.2;6;1\n"
//                                 "GROUP:1;0.2\n"
//                                 "PASS:6;6-Led verde Spegnimento A-;0.2;6;0\n"
//                                 "GROUP:1.2;0.2\n"
//                                 "PASS:7;7-Led rosso Accensione A+;0.2;7;1\n"
//                                 "GROUP:1.4;0.2\n"
//                                 "PASS:8;8-Led rosso Spegnimento A-;0.2;7;0\n"
//                                 "GROUP:1.6;0.2\n"
//                                 "PASS:9;9-Led blu Accensione A+;0.2;15;1\n"
//                                 "GROUP:1.8;0.2\n"
//                                 "PASS:10;10-Led blu Spegnimento A-;0.2;15;0\n"
//                                 "END\n";

// /**/

class Sequencer
{

private:
    const char *TAG = "Sequencer";

    const char *GROUP = "GROUP";
    const char *PASS = "PASS";
    const char *ON = "1";
    const char *END = "END";
    const char *DELIMITER = ":;\n";

    Cycle _cycle; // Sequencer owns a Cycle object

    char _cycleCode[CONFIG_MAX_LENGTH_CYCLE_STRING] = {0}; // string containing the cycle code

    Importer _importer; // Sequencer owns a Importer object

    float _curDuration = 0.0f, _curOffset = 0.0f, _totalDuration = 0.0f;
    Pass *_curPass;

    unsigned int _curGroupIndex = 0, _curPassIndex = 0;

    bool _isAtBegin = true;

public:
    Sequencer() : _importer(_cycleCode) // share the cycle code with the importer
    {
    }

    esp_err_t init()
    {
        ESP_LOGI(TAG, "Initializing Sequencer.");
        esp_err_t ret = _importer.init();
        return ret;
    }

    bool advance()
    {
        // advance current pass index
        ++_curPassIndex;
        _isAtBegin = false; // after the first advance we are no more at the beginning

        // if current pass index is over the content limit (i.e. equal to _lastId)
        // then reset to 0 to restart
        if (_curPassIndex == _cycle._groups[_curGroupIndex]._lastId)
        {
            _curPassIndex = 0;

            // do the same for the current group index
            ++_curGroupIndex;
            if (_curGroupIndex == _cycle._lastId)
            {
                _curGroupIndex = 0;
                _isAtBegin = true; // if current group index is reset to 0 then we are at the beginning
            }
        };

        // update current duration and current pass
        _curDuration = _cycle._groups[_curGroupIndex]._groupDuration;
        _curOffset = _cycle._groups[_curGroupIndex]._groupOffset;
        _curPass = &(_cycle._groups[_curGroupIndex]._passes[_curPassIndex]);
        return _isAtBegin;
    }

    void parse()
    {
        Group group;

        Pass pass;

        // strcpy(_cycleCode, CYCLE_LIT);
        ESP_LOGI(TAG, "Cycle string length: %d;\t"
                      "Available length: %d",
                 strlen(_cycleCode), CONFIG_MAX_LENGTH_CYCLE_STRING);

        char *row = strtok(_cycleCode, DELIMITER);

        bool isEnd = false;

        while ((row != NULL) && !isEnd)
        {
            while (strcmp(row, GROUP) == 0)
            {
                ESP_LOGI(TAG, "....GROUP found");

                row = strtok(NULL, DELIMITER);  // extract group offset
                group._groupOffset = atof(row); // convert to float (or double?)
                ESP_LOGI(TAG, ".....GroupOffset: %3.2f",
                         group.getGroupOffset()); // log the found value

                row = strtok(NULL, DELIMITER);    // extract group duration
                group._groupDuration = atof(row); // convert to float (or double?)
                ESP_LOGI(TAG, ".....GroupDuration: %3.2f",
                         group.getGroupDuration()); // log the found value

                row = strtok(NULL, DELIMITER);

                while (strcmp(row, PASS) == 0)
                {
                    ESP_LOGI(TAG, "\t....PASS found");

                    row = strtok(NULL, DELIMITER);                // extract pass id
                    pass._id = atoi(row);                         // convert to int
                    ESP_LOGI(TAG, "\t.....Id: %d", pass.getId()); // log found value

                    row = strtok(NULL, DELIMITER);  // extract pass description
                    strcpy(pass._description, row); // copy the string into the pass field
                    ESP_LOGI(TAG, "\t.....DESC: %s",
                             pass.getDescription()); // log found value

                    row = strtok(NULL, DELIMITER); // extract pass duration
                    pass._duration = atof(row);    // convert to float (or double?)
                    ESP_LOGI(TAG, "\t.....DUR: %3.2f",
                             pass.getDuration()); // log found value

                    row = strtok(NULL, DELIMITER);                  // extract pass pin
                    pass._pin = atoi(row);                          // convert to int
                    ESP_LOGI(TAG, "\t.....PIN: %d", pass.getPin()); // log found value

                    row = strtok(NULL, DELIMITER);                        // extrat pin status
                    pass._status = strcmp(row, ON) ? false : true;        // convert to bool
                    ESP_LOGI(TAG, "\t.....STATUS: %d", pass.getStatus()); // log found value

                    ESP_ERROR_CHECK(group.pushPass(pass)); // insert the pass just extracted into the group

                    row = strtok(NULL, DELIMITER); // extract next token --> a GROUP
                }
                ESP_ERROR_CHECK(_cycle.pushGroup(group));
                group.resetGroup(); // resetting internal index to push new group
            }
            isEnd = (strcmp(row, END) == 0);
        }

        //_cycle.logContent();

        // update current duration and current pass
        _curDuration = _cycle._groups[_curGroupIndex]._groupDuration;
        _curOffset = _cycle._groups[_curGroupIndex]._groupOffset;
        _totalDuration = _curDuration + _curOffset;
        _curPass = &(_cycle._groups[_curGroupIndex]._passes[_curPassIndex]);
    }

    float getCurDuration() const
    {
        return this->_curDuration;
    }

    float getCurOffset() const
    {
        return this->_curOffset;
    }

    float getTotalDuration() const
    {
        return _totalDuration;
    }

    Pass *getCurPass() const
    {
        return this->_curPass;
    }
};

#endif