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

  float _curDuration = 0.0f, _curOffset = 0.0f, _totalDuration = 0.0f;
  Pass *_curPass;

  bool _isAtBegin = true;

public:
  Sequencer();

  void parse();

  bool advance();

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

  ~Sequencer()
  {
    printf("Destroying a sequencer....\n");
  }
};

#endif
