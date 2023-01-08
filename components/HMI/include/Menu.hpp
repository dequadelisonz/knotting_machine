#ifndef Menu_hpp
#define Menu_hpp

/*STL includes*/
#include "string.h"
#include "stdio.h"

/*ESP-IDF includes*/
#include "esp_log.h"

/*This project includes*/
#include "MenuEntry.hpp"

template <typename TClass>
class Menu
{

private:
    const char *TAG = "Menu";

    static const uint8_t MAX_ENTRIES = 16; // max nr. of menu entries in menu

    TClass &_context;

    MenuEntry<TClass> *_head = nullptr;
    MenuEntry<TClass> *_tail = nullptr;
    MenuEntry<TClass> *_currentEntry = nullptr;

    uint8_t _entriesCount = 0;

    bool _errState = false;

public:
    Menu(TClass &context);

    void pushBack(MenuEntry<TClass> *entry);

    void forward()
    {
        do
        {
            _currentEntry = _currentEntry->_next;
        } while (!(_currentEntry->isActive()));
    }

    void backward()
    {
        do
        {
            _currentEntry = _currentEntry->_prev;
        } while (!(_currentEntry->isActive()));
    }

    void ok()
    {
        _currentEntry->action();
    }

    void printMenu();

    uint8_t getCount() const
    {
        return _entriesCount;
    }

    bool getErrState() const { return _errState; }
};

#include "Menu.hpp"

template <typename TClass>
Menu<TClass>::Menu(TClass &context) : _context(context)
{
}

template <typename TClass>
void Menu<TClass>::pushBack(MenuEntry<TClass> *entry)
{
    if (_entriesCount < MAX_ENTRIES)
    {
        if (_head == nullptr)
        {
            _tail = _head = entry;
        }
        else
        {
            entry->_prev = _tail;
            entry->_next = _head;
            _tail->_next = entry;
            _head->_prev = entry;
            _tail = entry;
        }
        ++_entriesCount;
        _currentEntry = _head;
    }
    else
    {
        _errState = true;
    }
}

template <typename TClass>
void Menu<TClass>::printMenu()
{

    MenuEntry<TClass> *cur = _head;

    do
    {
        if (cur == _currentEntry)
        {
            printf("> %s <\n",cur->getDescription());
        }
        else if (!cur->isActive())
        {
            printf("xx %s\n",cur->getDescription());
        }
        else
            printf("%s\n",cur->getDescription());
        cur = cur->_next;
    } while (cur->_prev != _tail);
}

#endif