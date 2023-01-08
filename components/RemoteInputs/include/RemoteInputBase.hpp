#ifndef RemoteInputBase_hpp
#define RemoteInputBase_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/

class RemoteInputBase
{
private:
    const char *TAG = "RemoteInputBase";

public:
    enum eRemInputLogic
    {
        NC = 0,
        NO = 1,
    };

    enum eBtnLevel
    {
        UP = 1,
        DOWN = 2,
        OK = 3
    };
    
    virtual void updateStatus() = 0;
};

#endif