#include "Importer.hpp"

Importer::Importer(Sequencer &sequencer) : _sequencer(sequencer)

{
    printf("Constructing an importer...\n");
    strcat(_cycleFileFullPath, CONFIG_SDCARD_MOUNT_POINT);
    strcat(_cycleFileFullPath, CONFIG_KNOTTING_CYCLE_FILENAME);
    _isInit = _sd.testSDCard();
}

void Importer::_syncCycle()
{
    strcpy(_sequencer._cycleCode, _cycleCode);
}
