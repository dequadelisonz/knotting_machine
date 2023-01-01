#ifndef ImporterSD_hpp
#define ImporterSD_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/
#include "Importer.hpp"

class ImporterSD : public Importer
{
private:
    const char *TAG = "ImporterSD";
    bool _readSD();

public:
    ImporterSD(Sequencer &sequencer);

    ~ImporterSD()
    {
        printf("Destroying an importer SD...\n");
    }
};

#endif