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

  uint8_t _curPassIndex = 0U;

  uint8_t _curId = 0U;
  char _curDecription[Pass::MAX_CHAR_DESCR] = {0};
  float _curDuration = 0.0f, _curOffset = 0.0f, _totalDuration = 0.0f;
  uint8_t _curPin = 0U;
  bool _curStatus = false;

  bool _isAtBegin = true;

public:
  Sequencer();

  void parse();

  bool const advance();

  float const getCurDuration() const { return this->_curDuration; }

  float const getCurOffset() const { return this->_curOffset; }

  uint8_t const getCurPin() const { return this->_curPin; }

  bool const getCurStatus() const { return this->_curStatus; }

  float const getTotalDuration() const { return _totalDuration; }

  uint8_t const getCurId() const { return _curId; }

  char const *getCurDescription() const { return _curDecription; }
};

#endif
