#ifndef MENUENTRY_HPP
#define MENUENTRY_HPP

class MenuEntry

{
private:
    const char *_descr;
    MenuEntry *_child, *_parent, *_prev, *_next;

    bool _isActive = false;

protected:
    int &_value;

public:
    MenuEntry(char descr[], int &value) : _descr(descr),
                                          _child(nullptr),
                                          _parent(nullptr),
                                          _prev(nullptr),
                                          _next(nullptr),
                                          _value(value)
    {
    }

    void setChild(MenuEntry *child)
    {
        _child = child;
        _child->_parent = this;
    }

    const MenuEntry *getChild() const
    {
        return _child;
    }

    // void setParent(MenuEntry *parent)
    // {
    //     _parent = parent;
    // }

    const MenuEntry *getParent() const
    {
        return _parent;
    }

    // void setPrev(MenuEntry *prev)
    // {
    //     _prev = prev;
    // }

    MenuEntry *getPrev() const
    {
        return _prev;
    }

    void setNext(MenuEntry *next)
    {
        _next = next;
        _next->_prev = this;
    }

    MenuEntry *const getNext() const
    {
        return _next;
    }

    void setActive(bool status)
    {
        _isActive = status;
    }

    bool isActive() const
    {
        return _isActive;
    }

    const char *getDescription() const
    {
        return _descr;
    }

    virtual void updateValue(int delta)
    {
        _value += delta;
    }

    const int getValue() const
    {
        return _value;
    }
};

// specific MenuEntry for HrlyPieces
class HrlyPieces : public MenuEntry
{
    const int _maxProd;

public:
    HrlyPieces(char descr[], int &prod) : MenuEntry(descr, prod),
                                          _maxProd(prod)
    {
    }

    void updateValue(int delta)
    {
        int tempProd = _value + delta;
        _value = (tempProd <= _maxProd) ? tempProd : _value;
    }
};

// specific MenuEntry for SDCardUpdate
class SDCardUpdate : public MenuEntry
{
    int _dummyRef = 0;

public:
    SDCardUpdate(char descr[]) : MenuEntry(descr, _dummyRef)
    {
    }

    // void updateValue(int delta)
    // {
    // }
};

// specific MenuEntry for WiFiUpdate
class WifiUpdate : public MenuEntry
{
    int _dummyRef = 0;

public:
    WifiUpdate(char descr[]) : MenuEntry(descr, _dummyRef)
    {
    }

    // void updateValue(int delta)
    // {
    // }
};

#endif