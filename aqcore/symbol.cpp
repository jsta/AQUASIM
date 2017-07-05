////////////////////////////////   symbol.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    23.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    SYMBOL::SYMBOL(const SYMBOL&)
//                                            constructor and
//                                            SYMBOL::operator=(const SYMBOL&)
//                                            operator added
//              30.09.98    Peter Reichert    no curly braces in description
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "symbol.h"

//////////////////////////////////////////////////////////////////////////////

const char* LegalFirstChars
          =           "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz_";

const char* LegalChars
          = "0123456789AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz_";


void SYMBOL::init()
{
   sym = 0;
   des = 0;
}


SYMBOL::SYMBOL()
{
   init();
}


SYMBOL::SYMBOL(const SYMBOL* s) : dNODE(s), FILEIO(s)
{
   init();
   if ( s != 0 )
   {
      if ( Symbol(s->sym) == FALSE )
         FatalError("SYMBOL::SYMBOL: illegal symbol");
      Description(s->des);
   }
}


SYMBOL::SYMBOL(const SYMBOL& s)
{
   init();
   if ( Symbol(s.sym) == FALSE )
      FatalError("SYMBOL::SYMBOL: illegal symbol");
   Description(s.des);
}


SYMBOL::~SYMBOL()
{
   delete sym; sym=0;
   delete des; des=0;
}


SYMBOL& SYMBOL::operator=(const SYMBOL& s)
{
   dNODE::operator=(s);
   FILEIO::operator=(s);
   if ( s.sym != 0 )
   {
      if ( Symbol(s.sym) == FALSE )
         FatalError("SYMBOL::operator=: illegal symbol");
   }
   Description(s.des);
   return *this;
}


BOOLEAN SYMBOL::AllowedFirstChar(char c)
{
   if ( c == '\0' )                 return FALSE;
   if ( strchr(LegalFirstChars,c) == 0 ) return FALSE;
   return TRUE;
}


BOOLEAN SYMBOL::AllowedSubsequentChar(char c)
{
   if ( c == '\0' )                      return FALSE;
   if ( strchr(LegalChars,c) == 0 ) return FALSE;
   return TRUE;
}


BOOLEAN SYMBOL::AllowedSymbol(const char* s)
{
   if ( s == 0 )                                  return FALSE;
   if ( s[0] == '\0' )                            return FALSE;
   if ( AllowedFirstChar(s[0]) == FALSE )         return FALSE;
   CARDINAL i=1;
   while ( s[i] != '\0' )
   {
      if ( AllowedSubsequentChar(s[i]) == FALSE ) return FALSE;
      i++;
   }
   return TRUE;
}


BOOLEAN SYMBOL::Symbol(const char* s)
{
   if ( InList() == TRUE ) return FALSE;
   if ( AllowedSymbol(s) == FALSE ) return FALSE;
   delete sym;
   sym = new char[strlen(s)+1];
   strcpy(sym,s);
   Changed();
   return TRUE;
}


void SYMBOL::Description(const char* d)
{
   Changed();
   delete des; des = 0;
   if ( d == 0 ) return;
   des = new char[strlen(d)+1];
   strcpy(des,d);
   for ( CARDINAL i=0; i<strlen(des); i++ )
   {
      if ( des[i] == '{' ) des[i] = '[';
      if ( des[i] == '}' ) des[i] = ']';
      if ( des[i] <  ' ' ) des[i] = ' ';
   }
}


void SYMBOL::Test()
{
   *ctest << "\nclass SYMBOL:     symbol      = ";
   if ( sym != 0 ) *ctest << sym;
   *ctest << "\n                  description = ";
   if ( des != 0 ) *ctest << des;
   FILEIO::Test();
   dNODE::Test();
}

//////////////////////////////////////////////////////////////////////////////

SYMBOL* SYMBOLLIST::Get(const char* string) const
{
   if ( string == 0 ) return 0;
   SYMBOL* sym = First();
   while ( sym != 0 )
   {
      if ( strcmp(sym->Symbol(),string) == 0 ) return sym;
      sym = sym->Next();
   }
   return 0;
}


BOOLEAN SYMBOLLIST::InsertSorted(SYMBOL* sym)
{
   const CARDINAL symsize = 1024;
   CARDINAL pos = 1;
   SYMBOL* s = First();
   char newsym[symsize]; char actsym[symsize];
   CARDINAL i = 0;
   while ( i<symsize-1 && i<strlen(sym->Symbol()) )
   {
      newsym[i] = ToUpper(sym->Symbol()[i]);
      i++;
   }
   newsym[i] = '\0';
   while ( s != 0 )
   {
      i = 0;
      while ( i<symsize-1 && i<strlen(s->Symbol()) )
      {
         actsym[i] = ToUpper(s->Symbol()[i]);
         i++;
      }
      actsym[i] = '\0';
      int strdiff = strcmp(actsym,newsym);
      if ( strdiff > 0 ) break;
      if ( strdiff == 0 )
      {
         if ( strcmp(s->Symbol(),sym->Symbol()) > 0 ) break;
      }
      s = s->Next();
      pos++;
   }
   return Insert(sym,pos);
}

//////////////////////////////////////////////////////////////////////////////
