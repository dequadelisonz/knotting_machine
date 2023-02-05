#include "Sequencer.hpp"

Sequencer::Sequencer()
{
    // ESP_LOGD(TAG, "Exiting constructor."); // TODO solo per debug
}

void Sequencer::parse()
{

    _cycle._reset(); // resetting the cycle object before re-parsing

    ESP_LOGI(TAG, "Cycle string length: %d;\t"
                  "Available length: %d",
             strlen(_cycleCode), CONFIG_MAX_LENGTH_CYCLE_STRING);

    ESP_LOGI(TAG, "%s", _cycleCode);

    int id = 0;
    float groupDuration = 0.0f;
    float offset = 0.0f;
    float duration = 0.0f;
    uint8_t pin = 0;
    char description[Pass::MAX_CHAR_DESCR] = {0};
    bool status = false;

    char *row = strtok(_cycleCode, DELIMITER); // extract first id - if=="END" file is finished
    bool isEnd = !(strcmp(row, END));

    while ((row != NULL) && !isEnd)
    {
        printf("id: %s\n", row);
        id = atoi(row);

        row = strtok(NULL, DELIMITER); // extracting group duration
        printf("Duration: %s\n", row);
        groupDuration = atof(row);

        row = strtok(NULL, DELIMITER); // extracting offset
        printf("Offset: %s\n", row);
        offset = atof(row);

        row = strtok(NULL, DELIMITER); // extracting description
        printf("Descr. %s\n", row);
        strncpy(description, row, Pass::MAX_CHAR_DESCR - 1);

        row = strtok(NULL, DELIMITER); // extracting duration
        printf("Duration: %s\n", row);
        duration = atof(row);

        row = strtok(NULL, DELIMITER); // extracting pin
        printf("Pin: %s\n", row);
        pin = atoi(row);

        row = strtok(NULL, DELIMITER); // extracting status
        printf("Status: %s\n", row);
        status = (atoi(row) == 1) ? true : false;

        Pass pass(id,
                  groupDuration,
                  offset,
                  duration,
                  pin,
                  description,
                  status);
        ESP_ERROR_CHECK(_cycle._pushPass(pass));

        row = strtok(NULL, DELIMITER); // start read next row pass
        isEnd = !(strcmp(row, END));
    }
    _totalDuration = groupDuration + offset;
    _cycle._logContent();
}

bool Sequencer::advance()
{
    ++_curPassIndex;
    _isAtBegin = false;

    if (_curPassIndex == _cycle._lastId)
    {
        _curPassIndex = 0;
        _isAtBegin = true;
    }
    _curDuration = _cycle._passes[_curPassIndex].getDuration();
    _curOffset = _cycle._passes[_curPassIndex].getOffset();
    _curPin = _cycle._passes[_curPassIndex].getPin();
    _curStatus = _cycle._passes[_curPassIndex].getStatus();

    // // advance current pass index
    // ++_curPassIndex;
    // _isAtBegin = false; // after the first advance we are no more at the beginning

    // // if current pass index is over the content limit (i.e. equal to _lastId)
    // // then _reset to 0 to restart
    // if (_curPassIndex == _cycle._groups[_curGroupIndex]._lastId)
    // {
    //     _curPassIndex = 0;

    //     // do the same for the current group index
    //     ++_curGroupIndex;
    //     if (_curGroupIndex == _cycle._lastId)
    //     {
    //         _curGroupIndex = 0;
    //         _isAtBegin = true; // if current group index is reset to 0 then we are at the beginning
    //     }
    // };

    // // update current duration and current pass
    // _curDuration = _cycle._groups[_curGroupIndex]._groupDuration;
    // _curOffset = _cycle._groups[_curGroupIndex]._groupOffset;
    // _curPass = &(_cycle._groups[_curGroupIndex]._passes[_curPassIndex]);

    return  _isAtBegin;
}
