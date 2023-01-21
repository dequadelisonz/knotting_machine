// #ifndef MenuEntry_hpp
// #define MenuEntry_hpp

// /*STL includes*/
// #include "string.h"

// /*ESP-IDF includes*/

// /*This project includes*/

// template <typename TClass>
// class Menu;

// template <typename TClass>
// class MenuEntry
// {

//     friend class Menu<TClass>;

// private:
//     static const uint8_t MAX_NAME_LENGTH = 20; // including final null char

//     TClass *_context;

//     char _descr[MAX_NAME_LENGTH] = {0};

//     MenuEntry<TClass> *_prev, *_next;

//     Functor<TClass> _functor;

//     bool _isActive = true;

// public:
//     MenuEntry() :  _prev(this), _next(this){};

//     MenuEntry(const char *descr);

//     bool &isActive() { return _isActive; }

//     const char *getDescription() const { return _descr; }

//     void init(TClass *context, const char *descr);

//     void setAction(void (TClass::*fpt)()) { _functor.setCallee(_context, fpt); }

//     virtual void action() { _functor(); }
// };

// template <typename TClass>
// MenuEntry<TClass>::MenuEntry(const char *descr) : _prev(this), _next(this)
// {
//     // clipping name to max allowed length
//     int len = (strlen(descr) >= (MAX_NAME_LENGTH - 1)) ? (MAX_NAME_LENGTH - 1) : strlen(descr);
//     memcpy(_descr, descr, len);
//     _descr[len] = '\0';
// }

// template <typename TClass>
// void MenuEntry<TClass>::init(TClass *context, const char *descr)
// {
//     _context = context;
//     // clipping name to max allowed length
//     int len = (strlen(descr) >= (MAX_NAME_LENGTH - 1)) ? (MAX_NAME_LENGTH - 1) : strlen(descr);
//     memcpy(_descr, descr, len);
//     _descr[len] = '\0';
// }

// #endif