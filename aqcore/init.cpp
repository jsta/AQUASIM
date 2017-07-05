/////////////////////////////////   init.C   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    17.09.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "fileio.h"
#include "init.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

/*#define SHOWFILESEARCH*/

INIT::INIT(const char* filename, const char* identifier, const char* deftext,
           CARDINAL nI, CARDINAL nR, CARDINAL nL, CARDINAL nT)
{
#ifdef AQUA_NOINI
   nInteger = 0;
   nReal    = 0;
   nLength  = 0;
   nText    = 0;
   return;
#endif

   nInteger = nI; nReal = nR; nLength = nL; nText = nT; // set number of items
   const char firstchar = ' ';
   char Buffer[BufSize]; Buffer[0] = '\0';

   if ( nInteger > 0 ) Integer = new INTEGER[nInteger]; // allocate arrays
   else                Integer = 0;
   if ( nReal > 0 )    Real    = new REAL[nReal];
   else                Real    = 0;
   if ( nLength > 0 )  Length  = new CARDINAL[nLength];
   else                Length  = 0;
   if ( nText > 0 )    Text    = new char*[nT];
   else                Text    = 0;

   CARDINAL index;                                      // set defaults
   for ( index=0; index<nInteger; index++ ) Integer[index] = 0;
   for ( index=0; index<nReal;    index++ ) Real[index]    = 0.0;
   for ( index=0; index<nLength;  index++ ) Length[index]  = 12;
   for ( index=0; index<nText;    index++ )
   {
	   std::ostrstream buf;
      buf << deftext << (index+1) << '\0';
      Text[index] = buf.str();
   }

   // try to open initialization file:

   BOOLEAN found    = FALSE;
   BOOLEAN readable = FALSE;
   BOOLEAN problem  = FALSE;
   const CARDINAL BufLen = 1024;
   char Buf[BufLen]; char Buf1[BufLen];
   std::ifstream in;
   char c;
#ifdef SHOWFILESEARCH
cout << "\nSearch for initialization file\n  " << filename;
#endif
   if ( ExistFile(filename) == TRUE )
   {
#ifdef SHOWFILESEARCH
cout << ": FOUND";
#endif
      found = TRUE;
	  in.open(filename,std::ios::in);        // ALL: current directory
      if ( in.get(c) )
      {
         readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
      }
   }

#if ( AQUA_OS==AQUA_SUN || AQUA_OS==AQUA_HP || AQUA_OS==AQUA_IBM || AQUA_OS==AQUA_IBM || AQUA_OS==AQUA_DEC || AQUA_OS==AQUA_LNX )

   if ( found == FALSE )             // UNIX: 1. environment variable HOME
   {
      if ( getenv("HOME") != 0 )
      {
         strcpy(Buf,getenv("HOME"));
         strcat(Buf,"/");
         strcat(Buf,filename);
#ifdef SHOWFILESEARCH
cout << "\n  " << Buf;
#endif
         if ( ExistFile(Buf) == TRUE )
         {
#ifdef SHOWFILESEARCH
cout << ": FOUND";
#endif
            found = TRUE;
            in.open(Buf,std::ios::in);
            if ( in.get(c) )
            {
               readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
            }
         }
      }
      if ( found == FALSE )          // UNIX: 2. HOME/lib
      {
         if ( getenv("HOME") != 0 )
         {
            strcpy(Buf,getenv("HOME"));
            strcat(Buf,"/lib/");
            strcat(Buf,filename);
#ifdef SHOWFILESEARCH
cout << "\n  " << Buf;
#endif
            if ( ExistFile(Buf) == TRUE )
            {
               found = TRUE;
               in.open(Buf,std::ios::in);
               if ( in.get(c) )
               {
                  readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
               }
            }
         }
      }
      if ( found == FALSE )          // UNIX: 3. environment variable XVTPATH
      {
         if ( getenv("XVTPATH") != 0 )
         {
            strcpy(Buf,getenv("XVTPATH"));
            strcat(Buf,"/");
            strcat(Buf,filename);
#ifdef SHOWFILESEARCH
cout << "\n  " << Buf;
#endif
            if ( ExistFile(Buf) == TRUE )
            {
#ifdef SHOWFILESEARCH
cout << ": FOUND";
#endif
               found = TRUE;
               in.open(Buf,std::ios::in);
               if ( in.get(c) )
               {
                  readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
               }
            }
         }
      }
      if ( found == FALSE )          // UNIX: 4. environment variable PATH
      {
         if ( getenv("PATH") != 0 )
         {
            strcpy(Buf,getenv("PATH"));
            CARDINAL pos = 0;
            while ( strlen(Buf) > pos )
            {
               CARDINAL i  = pos;
               CARDINAL i1 = 0;
               BOOLEAN complete = FALSE;
               while ( complete == FALSE )
               {
                  switch ( Buf[i] )
                  {
                     case ':': case '\0':
                        Buf1[i1] = '\0';
                        pos = i+1;
                        complete = TRUE;
                        break;
/*
                     case ' ':
                        i++;
                        break;
*/
                     default:
                        Buf1[i1] = Buf[i];
                        i++;
                        i1++;
                        break;
                  }
               }
               if ( strlen(Buf1) > 0 )
               {
                  if ( Buf1[strlen(Buf1)-1] != '/' )
                     strcat(Buf1,"/");
               }
               strcat(Buf1,filename);
#ifdef SHOWFILESEARCH
cout << "\n  " << Buf1;
#endif
               if ( ExistFile(Buf1) == TRUE )
               {
#ifdef SHOWFILESEARCH
cout << ": FOUND";
#endif
                  found = TRUE;
                  in.open(Buf1,std::ios::in);
                  if ( in.get(c) )
                  {
                     readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
                  }
                  break;
               }
            }
         }
      }
   }

#elif ( AQUA_OS==AQUA_NT || AQUA_OS==AQUA_WIN )

   if ( found == FALSE )             // NT: search path in env. var. PATH
   {
      if ( getenv("PATH") != 0 )
      {
         strcpy(Buf,getenv("PATH"));
         CARDINAL pos = 0;
         while ( strlen(Buf) > pos )
         {
            CARDINAL i  = pos;
            CARDINAL i1 = 0;
            BOOLEAN complete = FALSE;
            while ( complete == FALSE )
            {
               switch ( Buf[i] )
               {
                  case ';': case '\0':
                     Buf1[i1] = '\0';
                     pos = i+1;
                     complete = TRUE;
                     break;
/*
                  case ' ':
                     i++;
                     break;
*/
                  default:
                     Buf1[i1] = Buf[i];
                     i++;
                     i1++;
                     break;
               }
            }
            if ( strlen(Buf1) > 0 )
            {
               if ( Buf1[strlen(Buf1)-1] != '\\' )
                  strcat(Buf1,"\\");
            }
            strcat(Buf1,filename);
#ifdef SHOWFILESEARCH
cout << "\n  " << Buf1;
#endif
            if ( ExistFile(Buf1) == TRUE )
            {
#ifdef SHOWFILESEARCH
cout << ": FOUND";
#endif
               found = TRUE;
			   in.open(Buf1,std::ios::in);
               if ( in.get(c) )
               {
                  readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
               }
               break;
            }
         }
      }
   }

#elif ( AQUA_OS==AQUA_VMS )

   if ( found == FALSE )          // VMS: given path on dueb01
   {
	  strcpy(Buf,"A31:[REICHERT.BIN]");
      strcat(Buf,filename);
#ifdef SHOWFILESEARCH
cout << "\n  " << Buf;
#endif
      if ( ExistFile(Buf) == TRUE )
      {
#ifdef SHOWFILESEARCH
cout << ": FOUND";
#endif
         found = TRUE;
         in.open(Buf,std::ios::in);
         if ( in.get(c) )
         {
            readable = TRUE;
#ifdef SHOWFILESEARCH
cout << ", READABLE";
#endif
         }
      }
   }

#else

                                     // OTHER: no further search

#endif

   if ( found == FALSE )
   {
	   std::cout << "\n\nAQUASIM FATAL ERROR: file \"" << filename
           << "\" not found\n\n";
	   std::cout.flush();
      aqexit();
   }
   if ( readable == FALSE )
   {
	   std::cout << "\n\nAQUASIM FATAL ERROR: unable to read file \""
           << filename << "\"\n\n";
	   std::cout.flush();
      aqexit();
   }

   // check program identifier:

   CARDINAL i;
   while ( 1 )
   {
      i = 0;
      while ( 1 )
      {
         char c;
         if ( !in.get(c) )
         {
			 std::cout << "\n\nAQUASIM FATAL ERROR: file \""
                 << filename << "\" is of a wrong type\n\n";
            std::cout.flush();
            aqexit();
         }
         if ( c<firstchar || i==BufSize-1 )
         {
            Buffer[i] = '\0'; break;
         }
         Buffer[i] = c;
         i++;
      }
      if ( strcmp(Buffer,ProgName()) == 0 ) break;
   }
   i = 0;
   while ( 1 )
   {
      char c;
      if ( !in.get(c) )
      {
         std::cout << "\n\nAQUASIM FATAL ERROR: problem reading file \""
              << filename << "\"\n\n";
         std::cout.flush();
         aqexit();
      }
      if ( c<firstchar || i==BufSize-1 )
      {
         if ( i > 0 )
         {
            Buffer[i] = '\0'; break;
         }
      }
      else
      {
         Buffer[i] = c;
         i++;
      }
   }
   if ( strncmp(Buffer,ProgVersion(),11) != 0 )
   {
      std::cout << "\n\nAQUASIM FATAL ERROR: file \"" << filename
           << "\" is from a wrong program version\n\n";
      std::cout.flush();
      aqexit();
   }


   // read initialization file:

   found = FALSE;
   while ( FILEIO::LoadString(in,Buf,BufLen) == TRUE )
   {
      if ( strcmp(Buf,identifier) == 0 )             // check identifier
      {
         found = TRUE;
         if ( FILEIO::LoadStart(in) == TRUE )
         {
            if ( nInteger > 0 )                         // read integers
            {
               if ( FILEIO::LoadStart(in) == TRUE )
               {
                  INTEGER  i;
                  while ( FILEIO::LoadNumber(in,&index) == TRUE )
                  {
                     if ( FILEIO::LoadNumber(in,&i) == TRUE )
                     {
                        if ( (index>0) && (index<=nInteger) )
                        {
                           Integer[index-1] = i;
                        }
                     }
                  }
               }
               else
               {
                  problem = TRUE; break;
               }
            }
            if ( nReal > 0 )                            // read reals
            {
               if ( FILEIO::LoadStart(in) == TRUE )
               {
                  REAL r;
                  while ( FILEIO::LoadNumber(in,&index) == TRUE )
                  {
                     if ( FILEIO::LoadNumber(in,&r) == TRUE )
                     {
                        if ( (index>0) && (index<=nReal) )
                        {
                           Real[index-1] = r;
                        }
                     }
                  }
               }
               else
               {
                  problem = TRUE; break;
               }
            }
            if ( nLength > 0 )                          // read lengths
            {
               if ( FILEIO::LoadStart(in) == TRUE )
               {
                  CARDINAL c;
                  while ( FILEIO::LoadNumber(in,&index) == TRUE )
                  {
                     if ( FILEIO::LoadNumber(in,&c) == TRUE )
                     {
                        if ( (index>0) && (index<=nLength) )
                        {
                           Length[index-1] = c;
                        }
                     }
                  }
               }
               else
               {
                  problem = TRUE; break;
               }
            }
            if ( nText > 0 )                            // read text
            {
               if ( FILEIO::LoadStart(in) == TRUE )
               {
                  while ( FILEIO::LoadNumber(in,&index) == TRUE )
                  {
                     if ( FILEIO::LoadString(in,Buf,BufLen) == TRUE )
                     {
                        if ( (index>0) && (index<=nText) )
                        {
                           delete Text[index-1];
                           Text[index-1] = new char[strlen(Buf)+1];
                           strcpy(Text[index-1],Buf);
                        }
                     }
                  }
               }
               else
               {
                  problem = TRUE; break;
               }
            }
            FILEIO::LoadEnd(in);
            break;
         }
      }
      else                                                 // skip section
      {
         if ( FILEIO::LoadStart(in) == FALSE )  break;     // section
         while ( FILEIO::LoadStart(in) == TRUE )           // type; end section
         {
            while ( FILEIO::LoadStart(in) == TRUE )        // items; end type
            {
               if ( FILEIO::LoadEnd(in) == FALSE ) break;  // end item
            }
         }
      }
   }
   in.close();
   if ( found == FALSE )
   {
	   std::cout << "\n\nAQUASIM FATAL ERROR: section \"" << identifier
           << "\" of initialization"
           << "\n                     file \"" << filename
           << "\" not found\n\n";
	   std::cout.flush();
      aqexit();
   }
   if ( problem == TRUE )
   {
	   std::cout << "\n\nAQUASIM FATAL ERROR: problem reading section \""
           << identifier << "\""
           << "\n                     of initialization file \""
           << filename << "\" \n\n";
	   std::cout.flush();
      aqexit();
   }
}


INIT::~INIT()
{
   delete Integer; Integer = 0; nInteger = 0;
   delete Real;    Real = 0;    nReal = 0;
   delete Length;  Length = 0;  nLength = 0;
   for ( CARDINAL i=0; i<nText; i++ ) delete Text[i];
   delete Text; Text = 0; nText = 0;
}


INTEGER INIT::I(CARDINAL index) const
{
   if ( (index<1) || (index>nInteger) )
      FatalError("INIT::I: Index too large");
   return Integer[index-1];
}


REAL INIT::R(CARDINAL index) const
{
   if ( (index<1) || (index>nReal) )
      FatalError("INIT::R: Index too large");
   return Real[index-1];
}


CARDINAL INIT::L(CARDINAL index) const
{
   if ( (index<1) || (index>nLength) )
      FatalError("INIT::L: Index too large");
   return Length[index-1];
}


const char* INIT::T(CARDINAL index) const
{
   if ( (index<1) || (index>nText) )
      FatalError("INIT::T: Index too large");
   return Text[index-1];
}

//////////////////////////////////////////////////////////////////////////////

