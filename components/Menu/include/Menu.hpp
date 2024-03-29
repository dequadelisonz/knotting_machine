#ifndef Menu_hpp
#define Menu_hpp

/*STL includes*/
#include "string.h"

/*ESP-IDF includes*/

/*This project includes*/
#include "Functor.hpp"

namespace MenuNs
{
    const uint8_t MAX_NAME_LENGTH = 17U; // including final null char - related to display available length
    const uint8_t MAX_CHILDS = 16U;      // max nr. of menu entries in menu

    /*******MenuBase******/

    class MenuBase
    {
        friend class Menu;

    private:
        const char *TAG = "MenuBase";

    protected:
        char _descr[MAX_NAME_LENGTH] = {0};
        char _printOut[MAX_CHILDS * MAX_NAME_LENGTH] = {0};

        bool _isActive = true;
        MenuBase *_prev, *_next, *_parent;

        void _setDescription(const char *descr)
        {
            // clipping name to max allowed length
            // TODO make some helper class
            // int len = (strlen(descr) >= (MAX_NAME_LENGTH - 1)) ? (MAX_NAME_LENGTH - 1) : strlen(descr);
            // memcpy(_descr, descr, len);
            // _descr[len] = '\0';
            strncpy(_descr, descr, MAX_NAME_LENGTH - 1);
            //ESP_LOGI(TAG, "Now ME descr is: %s", _descr); // TODO solo debug
        }

    public:
        MenuBase() : _prev(this), _next(this), _parent(this) {}

        const char *getDescription() const { return _descr; }

        virtual bool isActive() const { return _isActive; }
        virtual void setIsActive(bool status) { _isActive = status; }

        virtual MenuBase *up() = 0;
        virtual MenuBase *down() = 0;
        virtual MenuBase *ok() = 0;
        virtual MenuBase *getSelf() = 0;
        virtual const char *printOut() = 0;
    };

    /*******MenuEntry******/

    template <typename TClass>
    class MenuEntry : public MenuBase
    {

    private:
        const char *TAG = "MenuEntry";

        TClass *_context;

        Functor<TClass> _functorUp;
        Functor<TClass> _functorDown;
        Functor<TClass> _functorOK;

    public:
        void init(TClass *context, const char *descr)
        {
            _context = context;
            _setDescription(descr);
        }

        void setActionUp(void (TClass::*fpt)())
        {
            _functorUp.setCallback(_context, fpt);
        }

        void setActionDown(void (TClass::*fpt)())
        {
            _functorDown.setCallback(_context, fpt);
        }

        void setActionOK(void (TClass::*fpt)())
        {
            _functorOK.setCallback(_context, fpt);
        }

        virtual void setIsActive(bool status)
        {
            if (status != _isActive)
            {
                char temp[MAX_NAME_LENGTH] = {0};
                if (!status)
                {
                    strcat(temp, "%"); // put '%' at beginning to mark as inactive menu entry, will be used for display rendering
                    strcat(temp, getDescription());
                    _setDescription(temp);
                }
                else
                {
                    strcpy(temp, &getDescription()[1]);
                    _setDescription(temp);
                }
            }
            _isActive = status;
        }

        MenuBase *up() override
        {
            if (this->_isActive)
                _functorUp();
            return this;
        }

        MenuBase *down() override
        {
            if (this->_isActive)
                _functorDown();
            return this;
        }

        MenuBase *ok() override
        {
            if (this->_isActive)
                _functorOK();
            return _parent;
        }

        MenuBase *getSelf() override { return this; };

        const char *printOut() override
        {
            _printOut[0] = '\0';
            strcat(this->_printOut, this->getDescription());
            strcat(this->_printOut, "\n");
            return this->_printOut;
        }
    };

    /*******Menu******/
    class Menu : public MenuBase
    {
    private:
        const char *TAG = "Menu";

        MenuBase *_childsHead = nullptr;
        MenuBase *_childsTail = nullptr;
        MenuBase *_currentChild = nullptr;

        unsigned short int _entriesCount = 0; // this istance is an entry in the menu

        const bool _isRoot;

        /*******QuiteEntry******/
        class QuitEntry : public MenuBase
        {

        private:
            const char *TAG = "QuitEntry";

        public:
            QuitEntry() { strcpy(_descr, "<- Exit"); }

            virtual bool isActive() //_isaActive of quiteEntry must be always true
            {
                this->_isActive = true;
                return _isActive;
            }

            virtual void setIsActive(bool status) //_isaActive of quiteEntry must be always true
            {
                _isActive = true;
            }

            MenuBase *up() override { return this; };
            MenuBase *down() override { return this; };
            MenuBase *ok() override { return this; };
            MenuBase *getSelf() override { return _parent->_parent; };

            const char *printOut() { return _descr; };
        } _quitEntry;

    public:
        Menu(const char *descr, bool isRoot) : _isRoot(isRoot)
        {
            _setDescription(descr);
            if (!_isRoot)
                pushEntry(&_quitEntry);
        }

        MenuBase *up() override
        {
            do
            {
                _currentChild = _currentChild->_prev;
            } while (!(_currentChild->isActive()));
            return this;
        };

        MenuBase *down() override
        {
            do
            {
                _currentChild = _currentChild->_next;
            } while (!(_currentChild->isActive()));
            return this;
        };

        MenuBase *ok() override
        {
            return _currentChild->getSelf();
        };

        MenuBase *getSelf() override { return this; };

        unsigned short int getCount() const { return _entriesCount; }

        const char *printOut() override
        {
            _printOut[0] = '\0';
            MenuBase *cur = _childsHead;
            do
            {
                if (cur == _currentChild) // surround with square brackets current selection
                {
                    strcat(_printOut, ">");
                    strcat(_printOut, cur->getDescription());
                    strcat(_printOut, "\n");
                }
                // else if (!cur->isActive())
                // {
                //     strcat(_printOut, "%"); // put '%' at beginning to mark as inactive menu entry, will be used to invert background
                //     strcat(_printOut, cur->getDescription());
                //     strcat(_printOut, "\n");
                // }
                else
                {
                    strcat(_printOut, cur->getDescription());
                    strcat(_printOut, "\n");
                }
                cur = cur->_next;
            } while (cur->_prev != _childsTail);
            //ESP_LOGI(TAG, "\n%s", _printOut); // TODO rimuovere dopo debug
            return _printOut;
        };

        bool pushEntry(MenuBase *entry)
        {
            bool ret = false;
            if (getCount() < MAX_CHILDS)
            {
                entry->_parent = this;
                if (_childsHead == nullptr)

                    _childsTail = _childsHead = entry;

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
                ret = true;
            }
            return ret;
        }
    };

    /*******MenuNavigator******/
    class MenuNavigator
    {
    private:
        const char *TAG = "MenuNavigator";

        MenuNs::MenuBase *_cursor;

        bool _isFrozen = false;

    public:
        MenuNavigator(MenuNs::Menu *cursor) : _cursor(cursor)
        {
        }

        void up()
        {
            if (!_isFrozen)
                _cursor = _cursor->up();
        }

        void down()
        {
            if (!_isFrozen)
                _cursor = _cursor->down();
        }

        void ok()
        {
            if (!_isFrozen)
                _cursor = _cursor->ok();
        }

        const char *getPrintout() const
        {
            return _cursor->printOut();
        }

        void freezeMenu()
        {
            _isFrozen = true;
        }

        void unFreezeMenu()
        {
            _isFrozen = false;
        }
    };

}

#endif