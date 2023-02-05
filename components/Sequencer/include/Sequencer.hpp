#ifndef Sequencer_hpp
#define Sequencer_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/
#include "esp_log.h"
#include "esp_err.h"

/*This project includes*/
#include "Cycle.hpp"

class Sequencer
{
  friend class Importer;

private:
  const char *TAG = "Sequencer";

  const char *END = "END";
  const char *DELIMITER = ";\n";

  char _cycleCode[CONFIG_MAX_LENGTH_CYCLE_STRING] = {0}; // string containing the cycle code

  Cycle _cycle; // Sequencer owns a Cycle object

  uint8_t _curPassIndex = 0;

  float _curDuration = 0.0f, _curOffset = 0.0f, _totalDuration = 0.0f;
  uint8_t _curPin = 0;
  bool _curStatus = false;

  //Pass *_curPass;

  bool _isAtBegin = true;

public:
  Sequencer();

  void parse();

  bool advance();

  float getCurDuration() const { return this->_curDuration; }

  float getCurOffset() const { return this->_curOffset; }

  uint8_t getCurPin() const { return this->_curPin; }

  bool getCurStatus() const { return this->_curStatus; }

  float getTotalDuration() const { return _totalDuration; }

  // Pass *getCurPass() const
  // {
  //   return this->_curPass;
  // }
};

#endif
