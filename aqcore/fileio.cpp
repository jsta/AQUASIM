////////////////////////////////   fileio.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    27.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    FILEIO::FILEIO(const FILEIO&)
//                                            constructor and
//                                            FILEIO::operator=(const FILEIO&)
//                                            operator added.
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "fileio.h"

//////////////////////////////////////////////////////////////////////////////

extern std::ostream* ctest;       // test output stream pointer (all.C)

//////////////////////////////////////////////////////////////////////////////

// constants and variables

const char StartDelimiter = '{';
const char EndDelimiter   = '}';

const char firstchar = ' ';
const char length    = 62;

const char* str_true  = "TRUE";
const char* str_false = "FALSE";

//////////////////////////////////////////////////////////////////////////////

// static procedures

CARDINAL FILEIO::printwidth  = 0;     // zero means not limited
CARDINAL FILEIO::printindent = 0;     // no default indent
CARDINAL FILEIO::printcount  = 0;     // counter is zero

CARDINAL FILEIO::savecount   = 0;     // counter is zero


FILEIO::FILEIO()
{
   saved = TRUE;
}


FILEIO::FILEIO(const FILEIO&)
{
   saved = FALSE;
}


FILEIO::FILEIO(const FILEIO*)
{
   saved = FALSE;
}


FILEIO& FILEIO::operator=(const FILEIO&)
{
   saved = FALSE;
   return *this;
}


//////////////////////////////////////////////////////////////////////////////

void FILEIO::PrintWidth(CARDINAL width)
{
   if ( width <= printindent )
   {
      if ( width > 0 ) return;
   }
   printwidth = width;
   printcount = 0;
   return;
}


void FILEIO::PrintIndent(CARDINAL indent)
{
   if ( indent >= printwidth )
   {
      if ( printwidth > 0 ) return;
   }
   printindent = indent;
   return;
}


BOOLEAN FILEIO::PrintLn(std::ostream& out, char c)
{
   if ( c >= ' ' )
   {
      CARDINAL length = printwidth;
      if ( length == 0 ) length = 72;
      if ( printcount > 0 )
      {
         if ( PrintLn(out) == FALSE ) return FALSE;
      }
      while ( printcount < length )
      {
         out << c; printcount++;
      }
   }
   out << "\n"; if ( out.fail() ) return FALSE;
   printcount = 0;
   return TRUE;
}


BOOLEAN FILEIO::Print(std::ostream& out, const char* string,
                      CARDINAL pos)
{
   CARDINAL i;
   Print(out,'\0',pos);
   if ( string == 0 ) return TRUE;
   i = 0;
   while ( string[i] != '\0' )
   {
      Print(out,string[i]);
      i++;
   }
   return TRUE;
}


BOOLEAN FILEIO::Print(std::ostream& out, char c,
                      CARDINAL pos, CARDINAL repeat)
{
   if ( pos > 0 )
   {
      if ( pos <= printcount ) PrintLn(out);
      while ( printcount+1 < pos )
      {
         out << " "; printcount++;
      }
   }
   for ( CARDINAL i=0; i<repeat; i++ )
   {
      if ( c >= ' ' )
      {
         if ( printwidth>0 && printcount>=printwidth )
         {
            if ( PrintLn(out) == FALSE )      return FALSE;
            while ( printcount < printindent )
            {
               out << " "; if ( out.fail() )  return FALSE;
               printcount++;
            }
         }
         out << c; if ( out.fail() )          return FALSE;
         printcount++;
      }
      else
      {
         switch ( c )
         {
            case '\n':
               if ( PrintLn(out) == FALSE )   return FALSE;
               break;
            case '\0':
               break;
            default:
               if ( printwidth == 0 )
                  out << c; if ( out.fail() ) return FALSE;
               break;
         }
      }
   }
   return TRUE;
}


BOOLEAN FILEIO::Print(std::ostream& out, CARDINAL num,
                      CARDINAL pos)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize);
   return Print(out,buf,pos);
}


BOOLEAN FILEIO::Print(std::ostream& out, CARDLONG num,
                      CARDINAL pos)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize);
   return Print(out,buf,pos);
}


BOOLEAN FILEIO::Print(std::ostream& out, INTEGER num,
                      CARDINAL pos)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize);
   if ( pos == 1 ) pos++;
   if ( num<0 && pos>1 ) pos--;
   return Print(out,buf,pos);
}


BOOLEAN FILEIO::Print(std::ostream& out, INTLONG num,
                      CARDINAL pos)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize);
   if ( pos == 1 ) pos++;
   if ( num<0 && pos>1 ) pos--;
   return Print(out,buf,pos);
}


BOOLEAN FILEIO::Print(std::ostream& out, REAL num,
                      CARDINAL pos)
{
   if ( !aqfinite(num) )
   {
      return Print(out,NotAvail,pos);
   }
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize,0,out.precision());
   if ( pos == 1 ) pos++;
   if ( num<0 && pos>1 ) pos--;
   return Print(out,buf,pos);
}


BOOLEAN FILEIO::PrintTime(std::ostream& out, const char* form,
                          CARDINAL pos)
{
   const CARDINAL bufsize = 1024; char buf[bufsize];
   if ( TimeToString(form,buf,bufsize) == FALSE ) return FALSE;
   if ( Print(out,buf,pos) == FALSE )             return FALSE;
   return TRUE;
}


BOOLEAN FILEIO::PrintBuffer(std::ostream& out)
{
   out.flush(); if ( out.fail() ) return FALSE;
   return TRUE;
}


//////////////////////////////////////////////////////////////////////////////


BOOLEAN FILEIO::SaveStart(std::ostream& out)
{
   out.put(StartDelimiter); if ( out.fail() ) return FALSE;
   savecount++;
   if ( savecount >= length )
   {
      out.put('\n'); if ( out.fail() ) return FALSE;
      savecount = 0;
   }
   return TRUE;
}


BOOLEAN FILEIO::SaveEnd(std::ostream& out)
{
   out.put(EndDelimiter); if ( out.fail() ) return FALSE;
   savecount++;
   if ( savecount >= length )
   {
      out.put('\n'); if ( out.fail() ) return FALSE;
      savecount = 0;
   }
   return TRUE;
}


BOOLEAN FILEIO::SaveString(std::ostream& out, const char* str)
{
   if ( SaveStart(out) == FALSE )            return FALSE;
   if ( str != 0 )
   {
      CARDINAL i = 0;
      while ( str[i] != '\0' )
      {
         out.put(str[i]);  if ( out.fail() ) return FALSE;
         savecount++;
         if ( savecount >= length )
         {
            out.put('\n'); if ( out.fail() ) return FALSE;
            savecount = 0;
         }
         i++;
      }
   }
   if ( SaveEnd(out) == FALSE )              return FALSE;
   return TRUE;
}


BOOLEAN FILEIO::SaveNumber(std::ostream& out, REAL num)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize,0,out.precision());
   return SaveString(out,buf);
}


BOOLEAN FILEIO::SaveNumber(std::ostream& out, CARDINAL num)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize);
   return SaveString(out,buf);
}


BOOLEAN FILEIO::SaveNumber(std::ostream& out, INTEGER num)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   NumToString(num,buf,bufsize);
   return SaveString(out,buf);
}


BOOLEAN FILEIO::SaveBoolean(std::ostream& out, BOOLEAN b)
{
   if ( b == TRUE )
   {
      return SaveString(out,str_true);
   }
   else
   {
      return SaveString(out,str_false);
   }
}

//////////////////////////////////////////////////////////////////////////////

BOOLEAN FILEIO::LoadStart(std::istream& in)
{
   char C;
   while ( in.get(C) )
   {
      if ( C == StartDelimiter ) return TRUE;
      if ( C == EndDelimiter   ) return FALSE;
   }
   return FALSE;
}


BOOLEAN FILEIO::LoadEnd(std::istream& in)
{
   char C;
   while ( in.get(C) )
   {
      if ( C == EndDelimiter   ) return TRUE;
      if ( C == StartDelimiter ) return FALSE;
   }
   return FALSE;
}


BOOLEAN FILEIO::LoadString(std::istream& in, char* str, CARDINAL dim)
{
   if ( LoadStart(in) == FALSE ) return FALSE;
   char     C;
   CARDINAL i=0;
   while ( in.get(C) )
   {
      if ( C == StartDelimiter ) return FALSE;
      if ( C == EndDelimiter )
      {
         str[i] = '\0';
         return TRUE;
      }
      else
      {
         if ( C >= firstchar )  // (eliminate control characters)
         {
            str[i] = C; i++;
            if ( i >= dim ) { str[i-1]='\0'; return FALSE; }
         }
      }
   }
   return FALSE;
}


BOOLEAN FILEIO::LoadNumber(std::istream& in, REAL* num)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   if ( LoadString(in,buf,bufsize) == FALSE ) return FALSE;
   if ( StringToNum(buf,num) == FALSE ) return FALSE;
   return TRUE;
}


BOOLEAN FILEIO::LoadNumber(std::istream& in, CARDINAL* num)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   if ( LoadString(in,buf,bufsize) == FALSE ) return FALSE;
   if ( StringToNum(buf,num) == FALSE ) return FALSE;
   return TRUE;
}


BOOLEAN FILEIO::LoadNumber(std::istream& in, INTEGER* num)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   if ( LoadString(in,buf,bufsize) == FALSE ) return FALSE;
   if ( StringToNum(buf,num) == FALSE ) return FALSE;
   return TRUE;
}


BOOLEAN FILEIO::LoadBoolean(std::istream& in, BOOLEAN* b)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   if ( LoadString(in,buf,bufsize) == FALSE ) return FALSE;
   if ( strcmp(buf,str_true) == 0 ) *b = TRUE;
   else                             *b = FALSE;
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

JOBSTATUS FILEIO::Save(std::ostream& out)
{
   if ( out.bad() ) return PROBLEM;
   saved = TRUE;
   return OK;
}


JOBSTATUS FILEIO::Write(std::ostream& out, BOOLEAN sh)
{
   if ( out.bad() ) return PROBLEM;
   return OK;
}


void FILEIO::Test()
{
   *ctest << "\nclass FILEIO:     saved       = ";
   switch (saved)
   {
      case TRUE:  *ctest << "TRUE";      break;
      case FALSE: *ctest << "FALSE";     break;
      default:    *ctest << "unknown";   break;
   }
}

//////////////////////////////////////////////////////////////////////////////
