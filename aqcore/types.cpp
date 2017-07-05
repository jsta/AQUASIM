////////////////////////////////   types.cpp   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.02.00    Peter Reichert    type conversions added to 
//                                            pure header file
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <math.h>

#include "types.h"

#if AQUA_OS==AQUA_SUN
#include <ieeefp.h>
#elif AQUA_OS==AQUA_NT
#include <float.h>
#endif

//////////////////////////////////////////////////////////////////////////////

int stringprec = 8;

//////////////////////////////////////////////////////////////////////////////

const REAL aq_pi=4.0*atan(1.0);

std::ostream* cjob   = &std::cout;

//////////////////////////////////////////////////////////////////////////////

void NumToString(CARDINAL num, char* buf, CARDINAL bufsize, CARDINAL right)
{
   std::ostrstream buffer(buf,bufsize);
   buffer << num << '\0';
   if ( right > 0 )
   {
      if ( right >= bufsize ) right = bufsize-1;
      CARDINAL len = strlen(buf);
      if ( right > len )
      {
	 CARDINAL i;
	 buf[right] = '\0';
	 for ( i=0; i<len; i++ ) buf[right-1-i] = buf[len-1-i];
	 for ( i=0; i<right-len; i++ ) buf[i] = ' ';
      }
   }
   return;
}


void NumToString(INTEGER num, char* buf, CARDINAL bufsize, CARDINAL right)
{
   std::ostrstream buffer(buf,bufsize);
   buffer << num << '\0';
   if ( right > 0 )
   {
      if ( right >= bufsize ) right = bufsize-1;
      CARDINAL len = strlen(buf);
      if ( right > len )
      {
	 CARDINAL i;
	 buf[right] = '\0';
	 for ( i=0; i<len; i++ ) buf[right-1-i] = buf[len-1-i];
	 for ( i=0; i<right-len; i++ ) buf[i] = ' ';
      }
   }
   return;
}


void NumToString(CARDLONG num, char* buf, CARDINAL bufsize, CARDINAL right)
{
   std::ostrstream buffer(buf,bufsize);
   buffer << num << '\0';
   if ( right > 0 )
   {
      if ( right >= bufsize ) right = bufsize-1;
      CARDINAL len = strlen(buf);
      if ( right > len )
      {
	 CARDINAL i;
	 buf[right] = '\0';
	 for ( i=0; i<len; i++ ) buf[right-1-i] = buf[len-1-i];
	 for ( i=0; i<right-len; i++ ) buf[i] = ' ';
      }
   }
   return;
}


void NumToString(INTLONG num, char* buf, CARDINAL bufsize, CARDINAL right)
{
   std::ostrstream buffer(buf,bufsize);
   buffer << num << '\0';
   if ( right > 0 )
   {
      if ( right >= bufsize ) right = bufsize-1;
      CARDINAL len = strlen(buf);
      if ( right > len )
      {
	 CARDINAL i;
	 buf[right] = '\0';
	 for ( i=0; i<len; i++ ) buf[right-1-i] = buf[len-1-i];
	 for ( i=0; i<right-len; i++ ) buf[i] = ' ';
      }
   }
   return;
}


void NumToString(REAL num, char* buf, CARDINAL bufsize, CARDINAL right,
		 int prec)
{
   std::ostrstream buffer(buf,bufsize);
   if ( prec == 0 )
   {
      buffer.precision(stringprec);
   }
   else
   {
      buffer.precision(prec);
   }
   buffer << num << '\0';
   if ( right > 0 )
   {
      if ( right >= bufsize ) right = bufsize-1;
      CARDINAL len = strlen(buf);
      if ( right > len )
      {
	 CARDINAL i;
	 buf[right] = '\0';
	 for ( i=0; i<len; i++ ) buf[right-1-i] = buf[len-1-i];
	 for ( i=0; i<right-len; i++ ) buf[i] = ' ';
      }
   }
   return;
}


BOOLEAN StringToNum(char* buf, REAL* num)
{
   if ( buf == 0 ) return FALSE;
   if ( buf[0] == '\0' ) return FALSE;
   std::istrstream in(buf);
   in >> *num;
   if ( in.fail() ) return FALSE;
   return TRUE;
}


BOOLEAN StringToNum(char* buf, CARDINAL* num)
{
   if ( buf == 0 ) return FALSE;
   if ( buf[0] == '\0' ) return FALSE;
   if ( buf[0]=='0' && buf[1]=='\0' )   // VAX was unable to read zero !!!
   {
      *num = 0; return TRUE;
   } 
   std::istrstream in(buf);
   in >> *num;
   if ( in.fail() ) return FALSE;
   return TRUE;
}


BOOLEAN StringToNum(char* buf, CARDLONG* num)
{
   if ( buf == 0 ) return FALSE;
   if ( buf[0] == '\0' ) return FALSE;
   if ( buf[0]=='0' && buf[1]=='\0' )   // VAX was unable to read zero !!!
   {
      *num = 0; return TRUE;
   } 
   std::istrstream in(buf);
   in >> *num;
   if ( in.fail() ) return FALSE;
   return TRUE;
}


BOOLEAN StringToNum(char* buf, INTEGER* num)
{
   if ( buf == 0 ) return FALSE;
   if ( buf[0] == '\0' ) return FALSE;
   if ( buf[0]=='0' && buf[1]=='\0' )   // VAX was unable to read zero !!!
   {
      *num = 0; return TRUE;
   } 
   std::istrstream in(buf);
   in >> *num;
   if ( in.fail() ) return FALSE;
   return TRUE;
}


BOOLEAN StringToNum(char* buf, INTLONG* num)
{
   if ( buf == 0 ) return FALSE;
   if ( buf[0] == '\0' ) return FALSE;
   if ( buf[0]=='0' && buf[1]=='\0' )   // VAX was unable to read zero !!!
   {
      *num = 0; return TRUE;
   } 
   std::istrstream in(buf);
   in >> *num;
   if ( in.fail() ) return FALSE;
   return TRUE;
}


BOOLEAN TimeToString(const char* form, char* string, CARDINAL size)
{
   const CARDINAL bufsize = 30; char buf[bufsize];
   string[0] = '\0';
   if ( form == 0 )                                return TRUE;
   time_t datetime;
   time(&datetime);
   if ( datetime == -1 )                           return FALSE;
   tm* DateTime = localtime(&datetime);
   CARDINAL Y = DateTime->tm_year + 1900; if ( Y < 1990 ) Y = Y+100;
   CARDINAL M = DateTime->tm_mon + 1;
   CARDINAL D = DateTime->tm_mday;
   CARDINAL h = DateTime->tm_hour;
   CARDINAL m = DateTime->tm_min;
   CARDINAL s = DateTime->tm_sec;
   CARDINAL i = 0;
   while ( form[i] != '\0' )
   {
      if ( form[i] == '%' )
      {
	 i++;
	 switch ( form[i] )
	 {
	    case 'Y':
	       NumToString(Y,buf,bufsize);
	       strcat(string,buf);
	       break;
	    case 'M':
	       if ( M < 10 ) strcat(string,"0");
	       NumToString(M,buf,bufsize);
	       strcat(string,buf);
	       break;
	    case 'D':
	       if ( D < 10 ) strcat(string,"0");
	       NumToString(D,buf,bufsize);
	       strcat(string,buf);
	       break;
	    case 'h':
	       if ( h < 10 ) strcat(string,"0");
	       NumToString(h,buf,bufsize);
	       strcat(string,buf);
	       break;
	    case 'm':
	       if ( m < 10 ) strcat(string,"0");
	       NumToString(m,buf,bufsize);
	       strcat(string,buf);
	       break;
	    case 's':
	       if ( s < 10 ) strcat(string,"0");
	       NumToString(s,buf,bufsize);
	       strcat(string,buf);
	       break;
	    case '\0':
	       return TRUE;
	 }
      }
      else
      {
	 CARDINAL l = strlen(string);
	 string[l] = form[i]; string[l+1] = '\0';
      }
      i++;
   }
   return TRUE;
}


//////////////////////////////////////////////////////////////////////////////

int aqfinite(REAL x)
{
   int res = 1;
#if AQUA_OS==AQUA_SUN
   res = finite(x);
#elif AQUA_OS==AQUA_NT
   res = _finite(x);
#elif AQUA_OS==AQUA_MAC
   ;
#else
   res = finite(x);
#endif
   return res;
}

//////////////////////////////////////////////////////////////////////////////
