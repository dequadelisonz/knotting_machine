#ifndef TestCycle_hpp
#define TestCycle_hpp

/* only for testing */
namespace TestCycle
{
    static const char CYCLE_LIT[] = "1;0.2;0;1-Inizio nd ndnd;0;-1;0\n"
                                    "2;0.2;0;2-Led giallo Accensione A+;0.2;5;1\n"
                                    "3;0.2;0;3-Led bianco Accensione A+;0.2;4;1\n"
                                    "4;0.6;0.2;4-Led bianco Spegnimento A-;0.4;4;0\n"
                                    "5;0.6;0.2;5-Led giallo Spegnimento A-;0.8;5;0\n"
                                    "8;0.6;0.2;8-Led rosso Accensione A+;0.6;7;1\n"
                                    "9;0.2;0.8;9-Led rosso Spegnimento A-;0.6;7;0\n"
                                    "6;0.8;1;6-Led verde Accensione A+;0.8;6;1\n"
                                    "7;0.2;1.8;7-Led verde Spegnimento A-;0.2;6;0\n"
                                    "10;0.3;2;10-Led blu Accensione A+;0.3;15;1\n"
                                    "11;0.5;2.3;11-Led blu Spegnimento A-;0.4;15;0\n"
                                    "12;0.5;2.3;12-Led giallo Accensione A+;0.5;5;1\n"
                                    "END\n";
}
/**/

#endif