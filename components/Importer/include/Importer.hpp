#ifndef Importer_hpp
#define Importer_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "SDDriver.hpp"
#include "Sequencer.hpp"

class Importer
{
private:
    const char *TAG = "Importer";

protected:
    char _cycleCode[CONFIG_MAX_LENGTH_CYCLE_STRING] = {0};
    char _cycleFileFullPath[24] = {0}; // mount point + file name for SD card reading

    bool _isInit = false;
    bool _isImported = false;

    SDDriver _sd;

    Sequencer &_sequencer;

    void _syncCycle();    

public:
    Importer(Sequencer &sequencer);

    ~Importer()
    {
        printf("Destroying an importer...\n");
    }

    const char *getCycleCode() const { return _cycleCode; }

    bool isImported() const
    {
        return _isImported;
    }

};

#endif
