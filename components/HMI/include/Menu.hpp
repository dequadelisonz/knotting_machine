#ifndef Menu_hpp
#define Menu_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/

/*This project includes*/

namespace MenuNS
{
    const uint8_t MAX_NAME_LENGTH = 20; // including final null char
    const uint8_t MAX_CHILDS = 16;      // max nr. of menu entries in menu

    class Menu;

    class MenuBase
    {
    private:
        const char *TAG = "MenuBase";

    protected:
        char _descr[MAX_NAME_LENGTH] = {0};

        char _printOut[MAX_CHILDS * (MAX_NAME_LENGTH + 8)] = {0}; // add 8 chars for print markup if needed

        bool _isActive = true, _isSelected = false;

        MenuBase *_prev, *_next, *_current;
        Menu *_parent;

    public:
        MenuBase() : _prev(this), _next(this), _parent(this) {}

        const char *getDescription() const { return _descr; }

        bool &isActive() { return _isActive; }

        bool &isSelected() { return _isSelected; }

        MenuBase *getParent() { return _parent; }

        virtual void up() = 0;
        virtual void down() = 0;
        virtual void ok() = 0;

        virtual const char *printOut() const = 0;
    };

    class Menu : public MenuBase
    {

    private:
        const char *TAG = "Menu";

        MenuBase *_childsHead = nullptr;
        MenuBase *_childsTail = nullptr;
        MenuBase *_currentChild = nullptr;
        MenuEntry _quitEntry;

        uint8_t _entriesCount = 0; // this istance is an entry in the menu

        // class MenuQuit : public MenuBase
        // {
        // private:
        //     const char *TAG = "MenuQuit";

        // public:
        //     void up()
        //     {
        //     }

        //     void down()
        //     {
        //     }

        //     void ok()
        //     {

        //     }
        // };

    public:
        Menu();

        void pushEntry(MenuBase *entry);

        void up() override
        {
            do
            {
                _currentChild = _currentChild->_prev;
            } while (!(_currentChild->isActive()));
        }

        void down() override
        {
            do
            {
                _currentChild = _currentChild->_next;
            } while (!(_currentChild->isActive()));
        }

        void ok() override
        {
            this->_current = _currentChild;
        }

        uint8_t getCount() const
        {
            return _entriesCount;
        }

        const char *printOut() const override;
    };

    Menu::Menu() : _childsHead(this), _childsTail(this), _currentChild(this){}
    {
    }

    void Menu::pushEntry(MenuBase *entry)
    {
        if (getCount() < MAX_CHILDS)
        {
            if (_childsHead == nullptr)
            {
                _childsTail = _childsHead = entry;
            }
            else
            {
                entry->_prev = _childsTail;
                entry->_next = _childsHead;
                _childsTail->_next = entry;
                _childsHead->_prev = entry;
                _childsTail = entry;
            }
            ++_entriesCount;
            _currentChild = _childsHead;
        }
        else
        {
            _errState = true; // TODO da gestire meglio....
        }
    }

    const char *Menu::printOut() const override
    {

        memset(_printOut, 0, sizeof(_printOut));

        MenuEntry<TClass> *cur = _childsHead;
        do
        {
            if (cur == _currentChild)
            {
                strcat(_printOut, "> ");
                strcat(_printOut, cur->getDescription());
                strcat(_printOut, " <\n");
                // printf("> %s <\n",cur->getDescription());
            }
            else if (!cur->isActive())
            {
                strcat(_printOut, "xx ");
                strcat(_printOut, cur->getDescription());
                strcat(_printOut, "\n");
                // printf("xx %s\n",cur->getDescription());
            }
            else
            {
                strcat(_printOut, cur->getDescription());
                strcat(_printOut, "\n");
                // printf("%s\n",cur->getDescription());
            }
            cur = cur->_next;
        } while (cur->_prev != _childsTail);
    }

    template <typename TClass>
    class MenuEntry : public MenuBase
    {

    private:
        const char *TAG = "MenuEntry";

        TClass *_context;

        Functor<TClass> _functorUp, _functorDown, _functorOk;

    public:
        // value is a reference to the value to be changed by this menu
        MenuEntry() : _prev(this),
                      _next(this)
        {
            tempValue = value; // priming the tempValue variable
        };

        void init(TClass *context, const char *descr);

        void setActionUp(void (TClass::*fpt)()) { _functorUp.setCallee(this->_context, fpt); }

        void setActionDown(void (TClass::*fpt)()) { _functorDown.setCallee(this->_context, fpt); }

        void setActionOk(void (TClass::*fpt)()) { _functorOk.setCallee(this->_context, fpt); }

        void up() override { _functorUp.action(); }

        void down() override { _functorDown.action(); }

        void ok() override
        {
            _functorOk.action();
        }

        const char *printOut() const override
        {
            strcat(this->_printOut, this->TAG);
            return this->_printOut;
        }
    };

    template <typename TClass>
    void MenuEntry<TClass>::init(TClass *context, const char *descr)
    {
        _context = context;
        // clipping name to max allowed length
        int len = (strlen(descr) >= (MAX_NAME_LENGTH - 1)) ? (MAX_NAME_LENGTH - 1) : strlen(descr);
        memcpy(_descr, descr, len);
        _descr[len] = '\0';
    }

}

#endif