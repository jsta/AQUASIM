////////////////////////////////   scrio_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    21.08.91    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "scrio_c.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufLen = 512;
static char    Buf[BufLen];

//////////////////////////////////////////////////////////////////////////////

// text for character oriented screen interface

INIT scr(initfile,"CHARSCREEN","char_text_",5,5,5,1600);

//////////////////////////////////////////////////////////////////////////////

CARDINAL fail = 0;

const char prompt[] = "   >";

//////////////////////////////////////////////////////////////////////////////

void Write(const char* string)
{
   std::cout << string;
   return;
}


void Write(char c, INTEGER num)
{
   for ( INTEGER i=0; i<num; i++ ) std::cout << c;
   return;
}


void Write(CARDINAL num)
{
   std::cout << num;
   return;
}


void Write(CARDLONG num)
{
   std::cout << num;
   return;
}


void Write(INTEGER num)
{
   std::cout << num;
   return;
}


void Write(REAL num)
{
   std::cout << num;
   return;
}


void WriteYesNo()
{
   std::cout << " " << scr.T(14)[0] << " " << scr.T(16) << " " << scr.T(15)[0];
   return;
}


void WriteBuffer()
{
   std::cout.flush();
   return;
}


void Read(char* string, CARDINAL size)
{
   if ( size < 1 ) return;
   std::cout << "\n" << prompt;
   CARDINAL i=0; char c;
   while ( i<size )
   {
      std::cin.get(c);
      if ( std::cin.fail() || c<32 )     // ignore control characters
      {
         if ( c!='\n' && c!='\r' ) fail++;
         break;
      }
      fail = 0;
      if ( c==32 && i==0 )
      {
         ;                  // ignore leading blancs
      }
      else
      {
         string[i] = c;     // store character
         i++;
      }
   }
   if ( fail > 10 )
   {
      Write("\n"); Write(scr.T(419)); Write("\n\n");
      aqexit();
   }
   string[i] = '\0';
   if ( strcmp(string,prompt) == 0 ) string[0] = '\0'; // Mac returns prompt
   return;                                             // in case of no input
}


void Read(char* string, CARDINAL size, const char* def)
{
   std::cout << " ["; if ( def != 0 ) std::cout << def; std::cout << "]";
   Read(string,size);
   if ( (string[0]=='\0') && (def!=0) ) strncpy(string,def,size-1);
   return;
}


void Read(char* string, CARDINAL size, char def)
{
   std::cout << " [" << def << "]";
   Read(string,size);
   if ( (string[0]=='\0') && (size>1) )
   {
      string[0] = def; string[1] = '\0';
   }
   return;
}


void Read(CARDINAL& c, CARDINAL def)
{
   std::cout << " [" << def << "]";
   Read(Buf,sizeof(Buf));
   if ( StringToNum(Buf,&c) == FALSE ) c = def;
   return;
}


void Read(CARDLONG& c, CARDINAL def)
{
   std::cout << " [" << def << "]";
   Read(Buf,sizeof(Buf));
   if ( StringToNum(Buf,&c) == FALSE ) c = def;
   return;
}


void Read(INTEGER& i, INTEGER def)
{
   std::cout << " [" << def << "]";
   Read(Buf,sizeof(Buf));
   if ( StringToNum(Buf,&i) == FALSE ) i = def;
   return;
}


void Read(REAL& r, REAL def)
{
   std::cout << " [" << def << "]";
   Read(Buf,sizeof(Buf));
   if ( StringToNum(Buf,&r) == FALSE ) r = def;
   return;
}

//////////////////////////////////////////////////////////////////////////////

