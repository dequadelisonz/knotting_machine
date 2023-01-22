#ifndef Functor_hpp
#define Functor_hpp

/*STL includes*/

/*ESP-IDF includes*/

/*This project includes*/

// base class
class FunctorBase
{
public:
   // two possible functions to call member function. virtual cause derived
   // classes will use a pointer to an object and a pointer to a member function
   // to make the function call
   virtual void operator()() {} //= 0; // call using operator
   virtual void Call() {}       // = 0;       // call using function
};

// derived template class
template <class TClass>
class Functor : public FunctorBase
{
private:
   void (TClass::*_fpt)() = 0; // pointer to member function
   TClass *_objPtr;            // pointer to object

public:
   Functor() {}

   // constructor - takes pointer to an object and pointer to a member and stores
   // them in two private variables
   Functor(TClass *objPtr, void (TClass::*fpt)())
   {
      _objPtr = objPtr;
      _fpt = fpt;
   }

   void setCallback(TClass *objPtr, void (TClass::*fpt)())
   {
      _objPtr = objPtr;
      _fpt = fpt;
   }

   // override operator "()"
   virtual void operator()() override
   {
      Call();
   }; // execute member function

   // override function "Call"
   inline virtual void Call() override
   {
      if (_fpt)
         (*_objPtr.*_fpt)();
   }; // execute member function
};

#endif