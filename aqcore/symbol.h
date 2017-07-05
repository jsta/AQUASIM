////////////////////////////////   symbol.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    23.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    SYMBOL::SYMBOL(const SYMBOL&)
//                                            constructor and
//                                            SYMBOL::operator=(const SYMBOL&)
//                                            operator added
//
//////////////////////////////////////////////////////////////////////////////
//
// "symbol" implements a base class of objects with an identifier symbol
// =====================================================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
// Various objects need a unique string as an identifier, e.g. variables. The
// class SYMBOL is a base class for such objects. To allow the use of symbols
// in formulae, the usage of special characters is not allowed. Symbols are
// strings consisting of upper case and lower case letters, underlines and 
// digits, but not starting with a digit. static functions are provided to
// test, if a given string is an allowed symbol. To allow a good documen-
// tation, the class contains an additional arbitraty string, called des-
// cription.
// To allow to add SYMBOL objects to lists, and to facilitate input and out-
// put for derived classes, SYMBOL is derived from NODE and from FILEIO.
// To prevent from inconsistencies of systems with unique symbols, it is not 
// allowed to change the symbol of an object which is element of a list.
//
//////////////////////////////////////////////////////////////////////////////
//
// Class hierarchy
// ===============
//
//   NODE
//     |
//     +------+
//            |
//   FILEIO   |
//     |      |
//     +------+----SYMBOL
//
//   LIST
//     |
//     +----SYMBOLLIST
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// SYMBOL:       object containing a string consisting of upper case and lower
// -------       case letters, numbers and underlines but not beginning with 
//               a number. The class is derived from NODE (sdlist.h) so that
//               it can be added to a list.
//
// SYMBOLLIST:
// -----------
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class SYMBOL
// ===========================
//
// static BOOLEAN AllowedFirstChar(char c)
//                                       returns TRUE if c is an allowed first
//                                       character of a symbol, FALSE other-
//                                       wise.
//
// static BOOLEAN AllowedSubsequentChar(char c)
//                                       returns TRUE if c is an allowed
//                                       character after the first character
//                                       of a symbol, FALSE otherwise.
//
// static BOOLEAN AllowedSymbol(const char* s) const  
//                                       returns TRUE if s is an allowed
//                                       symbol (see above); returns FALSE
//                                       otherwise
//
// const char* Symbol() const            returns a const pointer to the symbol
//
// BOOLEAN Symbol(const char* s)         replaces the symbol by the string s
//                                       and returns TRUE if the object is not
//                                       an element of a list and if s is an
//                                       allowed symbol (see above); returns
//                                       FALSE otherwise
//
// const char* Description() const       returns a const pointer to the
//                                       description (comment)
//
// void Description(const char* d)       replaces the description (comment)
//                                       by the string d
//
// void Test()                           prints the symbol, description and
//                                       the information of the base classes
//                                       to ctest (see all.h). Virtual member.
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class SYMBOLLIST
// ===============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SYMBOL_H
#define SYMBOL_H

#include "all.h"
#include "sdlist.h"
#include "fileio.h"

//////////////////////////////////////////////////////////////////////////////

class SYMBOL : public dNODE, public FILEIO
{
   public:

      SYMBOL();
      SYMBOL(const SYMBOL* s);
      SYMBOL(const SYMBOL& s);
      virtual ~SYMBOL();

      SYMBOL&             operator=(const SYMBOL& symbol);

      SYMBOL*             Next() const
                             { return (SYMBOL*)dNODE::Next(); }
      SYMBOL*             Prev() const
                             { return (SYMBOL*)dNODE::Prev(); }

      static BOOLEAN      AllowedFirstChar(char c);
      static BOOLEAN      AllowedSubsequentChar(char c);
      static BOOLEAN      AllowedSymbol(const char* s);

      const char*         Symbol() const
                             { return sym; }
      BOOLEAN             Symbol(const char* s);

      const char*         Description() const
                             { return des; }
      void                Description(const char* d);

      void                Test();

   private:

      char*               sym;
      char*               des;

      void                init();
};

//////////////////////////////////////////////////////////////////////////////

class SYMBOLLIST : private dLIST
{
   public:

      BOOLEAN  Empty() const
                  { return dLIST::Empty(); }
      CARDINAL Size() const
                  { return dLIST::Size(); }
      BOOLEAN  Exist(const SYMBOL* sym) const
                  { return Node(sym); }
      BOOLEAN  Exist(const char* string) const
                  { return Exist(Get(string)); }

      SYMBOL*  First() const
                  { return (SYMBOL*)dLIST::First(); }
      SYMBOL*  Last() const
                  { return (SYMBOL*)dLIST::First(); }
      SYMBOL*  Current() const
                  { return (SYMBOL*)Node(); }
      SYMBOL*  Next() const
                  { return (SYMBOL*)dLIST::Next(); }
      SYMBOL*  Prev() const
                  { return (SYMBOL*)dLIST::Prev(); }
      CARDINAL Position() const
                  { return dLIST::Position(); }
      CARDINAL Position(const SYMBOL* sym) const
                  { return dLIST::Position(sym); }
      CARDINAL Position(const char* string)
                  { return Position(Get(string)); }
      SYMBOL*  Get(CARDINAL pos) const
                  { return (SYMBOL*)Node(pos); }
      SYMBOL*  Get(const char* string) const;

      BOOLEAN  Goto(SYMBOL* sym)
                  { return dLIST::Goto(sym); }
      BOOLEAN  Goto(const char* string)
                  { return Goto(Get(string)); }
      BOOLEAN  Goto(CARDINAL pos)
                  { return dLIST::Goto(pos); }
      BOOLEAN  GotoFirst()
                  { return dLIST::GotoFirst(); }
      BOOLEAN  GotoLast()
                  { return dLIST::GotoLast(); }
      BOOLEAN  GotoNext()
                  { return dLIST::GotoNext(); }
      BOOLEAN  GotoPrev()
                  { return dLIST::GotoPrev(); }
      BOOLEAN  GotoNextOrFirst()
                  { return dLIST::GotoNextOrFirst(); }
      BOOLEAN  GotoPrevOrLast()
                  { return dLIST::GotoPrevOrLast(); }

      BOOLEAN  InsertFirst(SYMBOL* sym)
                  { return dLIST::InsertFirst(sym); }
      BOOLEAN  InsertLast(SYMBOL* sym)
                  { return dLIST::InsertLast(sym); }
      BOOLEAN  Insert(SYMBOL* sym, CARDINAL pos)
                  { return dLIST::Insert(sym,pos); }
      BOOLEAN  Insert(SYMBOL* sym)
                  { return dLIST::Insert(sym); }
      BOOLEAN  InsertAfter(SYMBOL* sym)
                  { return dLIST::InsertAfter(sym); }
      BOOLEAN  InsertSorted(SYMBOL* sym);
      BOOLEAN  Replace(SYMBOL* sym)
                  { return dLIST::Replace(sym); }
      BOOLEAN  RemoveFirst()
                  { return dLIST::RemoveFirst(); }
      BOOLEAN  RemoveLast()
                  { return dLIST::RemoveLast(); }
      BOOLEAN  Remove(SYMBOL* sym)
                  { return dLIST::Remove(sym); }
      BOOLEAN  Remove(CARDINAL pos)
                  { return dLIST::Remove(pos); }
      BOOLEAN  Remove()
                  { return dLIST::Remove(); }
      BOOLEAN  Remove(const char* string)
                  { return Remove(Get(string)); }
      void     RemoveAll()
                  { dLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
