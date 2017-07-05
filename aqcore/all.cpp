/////////////////////////////////   all.cpp   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.12.90    Peter Reichert
// revisions:   12.06.91    Peter Reichert    ported to Sun
//              16.09.91    Peter Reichert    ostreams ctest, cjob, cfatal
//                                            introduced
//              02.11.93    Peter Reichert    SetFileType added
//              05.11.98    Peter Reichert    CARDLONG, INTLONG added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <math.h>

#include "all.h"

#if ( AQUA_OS==AQUA_MAC && AQUA_VERSION!=AQUA_VCC )
#include <Files.h>
#include <Strings.h>
#endif

#if AQUA_OS==AQUA_SUN
#include <ieeefp.h>
#elif AQUA_OS==AQUA_NT
#include <float.h>
#endif

//////////////////////////////////////////////////////////////////////////////

const char* ProgName()
{
   return "AQUASIM";
}

char aquavers[1024];

const char* ProgVersion()
{

   strcpy(aquavers,"Version 2.1g");
   strcat(aquavers," (");
   switch ( AQUA_OS )
   {
   case AQUA_SUN:
      strcat(aquavers,"solaris");
      break;
   case AQUA_NT:
      strcat(aquavers,"win");
      break;
   case AQUA_MAC:
      strcat(aquavers,"mac");
      break;
   case AQUA_HP:
      strcat(aquavers,"hp");
      break;
   case AQUA_IBM:
      strcat(aquavers,"aix");
      break;
   case AQUA_VMS:
      strcat(aquavers,"vms");
      break;
   case AQUA_DEC:
      strcat(aquavers,"dec");
      break;
   case AQUA_LNX:
      strcat(aquavers,"lnx");
      break;
   case AQUA_WIN:
      strcat(aquavers,"win");
      break;
   default:
      break;
   }
   strcat(aquavers,"/");
   switch ( AQUA_VERSION )
   {
   case AQUA_BATCH:
      strcat(aquavers,"batch");
      break;
   case AQUA_CHAR:
      strcat(aquavers,"char");
      break;
   case AQUA_XVT:
      strcat(aquavers,"xvt");
      break;
   case AQUA_VCC:
      strcat(aquavers,"mfc");
      break;
   case AQUA_MOTIF:
      strcat(aquavers,"motif");
      break;
   default:
      break;
   }
   strcat(aquavers,")");
   return aquavers;
}

//////////////////////////////////////////////////////////////////////////////

const REAL aq_pi=4.0*atan(1.0);

std::ostream* ctest  = &std::cout;
std::ostream* cjob   = &std::cout;
std::ostream* cfatal = &std::cout;

//////////////////////////////////////////////////////////////////////////////

char NotAvail[3]  = "NA";

//////////////////////////////////////////////////////////////////////////////

#if AQUA_OS==AQUA_MAC

char FTaqu[TypeLen]  = "AQU_";  // AQUASIM system file (input and output)
char FTdatR[TypeLen] = "";      // reading data into real lists (input only)
char FTdatW[TypeLen] = "TEXT";  // writing data from real lists (output only)
char FTlog[TypeLen]  = "TEXT";  // log file (output only)
char FTprn[TypeLen]  = "TEXT";  // system definition print file (output only)
char FTsen[TypeLen]  = "TEXT";  // detailed info on sens. anal. (output only)
char FTfit[TypeLen]  = "TEXT";  // detailed info on fit progress (output only)
char FTlis[TypeLen]  = "TEXT";  // listing of results (output only)
char FTpts[TypeLen]  = "TEXT";  // PostScript plot file (output only)

char FCaqu[TypeLen]  = "AQUW";
char FCdat[TypeLen]  = "XCEL";
char FClog[TypeLen]  = "ttxt";
char FCprn[TypeLen]  = "MSWD";
char FCsen[TypeLen]  = "ttxt";
char FCfit[TypeLen]  = "ttxt";
char FClis[TypeLen]  = "XCEL";
char FCpts[TypeLen]  = "ttxt";

char FEaqu[TypeLen]  = ".aqu";
char FEdat[TypeLen]  = ".txt";
char FElog[TypeLen]  = ".log";
char FEprn[TypeLen]  = ".prn";
char FEsen[TypeLen]  = ".sen";
char FEfit[TypeLen]  = ".fit";
char FElis[TypeLen]  = ".lis";
char FEpts[TypeLen]  = ".ps";

#elif AQUA_OS==AQUA_NT || AQUA_OS==AQUA_WIN

char FTaqu[TypeLen]  = "aqu";
char FTdatR[TypeLen] = "txt";
char FTdatW[TypeLen] = "txt";
char FTlog[TypeLen]  = "log";
char FTprn[TypeLen]  = "prn";
char FTsen[TypeLen]  = "sen";
char FTfit[TypeLen]  = "fit";
char FTlis[TypeLen]  = "lis";
char FTpts[TypeLen]  = "ps";

char FCaqu[TypeLen]  = "";
char FCdat[TypeLen]  = "";
char FClog[TypeLen]  = "";
char FCprn[TypeLen]  = "";
char FCsen[TypeLen]  = "";
char FCfit[TypeLen]  = "";
char FClis[TypeLen]  = "";
char FCpts[TypeLen]  = "";

char FEaqu[TypeLen]  = ".aqu";
char FEdat[TypeLen]  = ".txt";
char FElog[TypeLen]  = ".log";
char FEprn[TypeLen]  = ".prn";
char FEsen[TypeLen]  = ".sen";
char FEfit[TypeLen]  = ".fit";
char FElis[TypeLen]  = ".lis";
char FEpts[TypeLen]  = ".ps";

#else

char FTaqu[TypeLen]  = "*.aqu";
char FTdatR[TypeLen] = "*.dat";
char FTdatW[TypeLen] = "*.dat";
char FTlog[TypeLen]  = "*.log";
char FTprn[TypeLen]  = "*.prn";
char FTsen[TypeLen]  = "*.sen";
char FTfit[TypeLen]  = "*.fit";
char FTlis[TypeLen]  = "*.lis";
char FTpts[TypeLen]  = "*.ps";

char FCaqu[TypeLen]  = "";
char FCdat[TypeLen]  = "";
char FClog[TypeLen]  = "";
char FCprn[TypeLen]  = "";
char FCsen[TypeLen]  = "";
char FCfit[TypeLen]  = "";
char FClis[TypeLen]  = "";
char FCpts[TypeLen]  = "";

char FEaqu[TypeLen]  = ".aqu";
char FEdat[TypeLen]  = ".dat";
char FElog[TypeLen]  = ".log";
char FEprn[TypeLen]  = ".prn";
char FEsen[TypeLen]  = ".sen";
char FEfit[TypeLen]  = ".fit";
char FElis[TypeLen]  = ".lis";
char FEpts[TypeLen]  = ".ps";

#endif

//////////////////////////////////////////////////////////////////////////////

// program termination

static BOOLEAN doexit = FALSE;

void aqexit()
{
   if ( doexit == TRUE ) return;
   doexit = TRUE;
#if AQUA_OS==AQUA_SUN
   exit(0);
#else
   exit(1);
#endif
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

// file manipulations

int RemoveFile(const char* filename)
{
#if AQUA_OS==AQUA_SUN             // remove is not implemented in Sun OS 4.1
   char cmd[250]="rm ";
   strcat(cmd,filename);
   return system(cmd);
#else
   return remove(filename);
#endif
}

int RenameFile(const char* oldfilename, const char* newfilename)
{
#if AQUA_OS==AQUA_SUN             // rename is not implemented in Sun OS 4.1
   char cmd[250]="mv ";
   strcat(cmd,oldfilename);
   strcat(cmd," ");
   strcat(cmd,newfilename);
   return system(cmd);
#else
   return rename(oldfilename,newfilename);
#endif
}


BOOLEAN ExistFile(const char* name)
{
   FILE* file = fopen((char*)name,"r");    // ifstream.open(name,ios::in)
   if ( file == 0 ) return FALSE;          // causes creation of a file
   fclose(file);                           // on some systems -> check for
   return TRUE;                            // existence with C functions
}


#if ( AQUA_OS==AQUA_MAC && AQUA_VERSION!=AQUA_VCC )

BOOLEAN SetFileType(const char* name, const char* type, const char* creator)
{
   FInfo fi;                        // Macintosh file information
   char* ft = (char*)&fi.fdType;    // access to file type as a C string
   char* fc = (char*)&fi.fdCreator; // access to file creator as a C string
   char FN[256];                    // local buffer for file name
   ConstStr255Param fn = (ConstStr255Param)&FN[0];
				    // access to file name as a Macintosh type
   int i;                           // index

   strcpy(FN,name);                 // copy file name to buffer
   c2pstr(FN);                      // convert file name to a Pascal string
   i = GetFInfo(fn,0,&fi);          // get address of file information
   if ( i != 0 ) return FALSE;      // unable to get information

   for ( i=0; i<4; i++)
   {
      ft[i] = ' '; fc[i] = ' ';     // fill type and creator with blancs
   }
   i = 0;
   while ( i<4 && type[i] )         // specify type
   {
      ft[i] = type[i];
      i++;
   }
   i = 0;
   while ( i<4 && creator[i] )      // specify creator
   {
      fc[i] = creator[i];
      i++;
   }
   i = SetFInfo(fn,0,&fi);          // set file information
   if ( i != 0 ) return FALSE;

   return TRUE;
}

#else

BOOLEAN SetFileType(const char*, const char*, const char*)
{
   return TRUE;
}

#endif


//////////////////////////////////////////////////////////////////////////////


FILE* heapfile     = 0;

#ifdef HEAPTEST

static void OpenHeapFile();

static int     heap_dim     = 0;
static void**  heap_address = 0;
static size_t* heap_bytes   = 0;
static long    heap_size    = 0;
static long    heap_max     = 0;

static void OpenHeapFile()
{
   heapfile=fopen("aquaheap.tmp","w");
   fprintf(heapfile,"   address:  bytes:     total:");
   return;
}

void* operator new(size_t size)
{
   void* pointer = malloc(size);
   if ( pointer == 0 )
   {
      printf("\n");
      printf("\n******   AQUASIM FATAL ERROR   ******");
      printf("\n*                                   *");
      printf("\n*  operator new: not enough memory  *");
      printf("\n*                                   *");
      printf("\n*************************************\n");
      printf("\n");
      fprintf(heapfile,"\n*** HEAP ALLOCATION ERROR: ");
      fprintf(heapfile,"unable to allocate %7d bytes",size);
      aqexit();
   }
   heap_size += size;
   if ( heap_size > heap_max ) heap_max = heap_size;
   BOOLEAN OK = FALSE;
   for ( int i=0; i<heap_dim; i++ )
   {
      if ( heap_address[i] == 0 )
      {
         heap_address[i] = pointer;
         heap_bytes[i]   = size;
         OK = TRUE;
         break;
      }
   }
   if ( OK != TRUE )
   {
      int new_heap_dim = heap_dim + 1000;
      void**  new_heap_address = (void**)malloc(new_heap_dim*sizeof(void*));
      size_t* new_heap_bytes   = (size_t*)malloc(new_heap_dim*sizeof(size_t));
      for ( int i=0; i<heap_dim; i++ )
      {
         new_heap_address[i] = heap_address[i];
         new_heap_bytes[i]   = heap_bytes[i];
      }
      new_heap_address[heap_dim] = pointer;
      new_heap_bytes[heap_dim]   = size;
      for ( i=heap_dim+1; i<new_heap_dim; i++ )
      {
         new_heap_address[i] = 0;
         new_heap_bytes[i]   = 0;
      }
      free(heap_address);
      free(heap_bytes);
      heap_dim     = new_heap_dim;
      heap_address = new_heap_address;
      heap_bytes   = new_heap_bytes;
   }
   if ( heapfile == 0 ) OpenHeapFile();
   fprintf(heapfile,"\n%10x: +%6d =%9d",pointer,size,heap_size);
   return pointer;
}

void operator delete(void* pointer)
{
   if ( pointer != 0 )
   {
      BOOLEAN found = FALSE;
      for ( int i=0; i<heap_dim; i++ )
      {
         if ( heap_address[i] == pointer )
         {
            if ( heap_bytes[i] > heap_size )
            {
               printf("\n");
               printf("\n*******   AQUASIM FATAL ERROR   *******");
               printf("\n*                                     *");
               printf("\n*  operator delete: heap test failed  *");
               printf("\n*  (size to deallocate too large)     *");
               printf("\n*                                     *");
               printf("\n***************************************\n");
               printf("\n");
               fprintf(heapfile,"\n*** HEAP DEALLOCATION ERROR: ");
               fprintf(heapfile,"at address %10x: not %7d bytes",
                       pointer,heap_bytes[i]);
               aqexit();
            }
            heap_size -= heap_bytes[i];
            fprintf(heapfile,"\n%10x: -%6d =%9d",
                    pointer,heap_bytes[i],heap_size);
            heap_address[i] = 0;
            heap_bytes[i]   = 0;
            found = TRUE;
            break;
         }
      }
      if ( found != TRUE )
      {
         printf("\n");
         printf("\n*******   AQUASIM FATAL ERROR   *******");
         printf("\n*                                     *");
         printf("\n*  operator delete: heap test failed  *");
         printf("\n*  (address to deallocate not found)  *");
         printf("\n*                                     *");
         printf("\n***************************************\n");
         printf("\n");
         fprintf(heapfile,"\n*** HEAP DEALLOCATION ERROR: ");
         fprintf(heapfile," address %10x not found",pointer);
         aqexit();
      }
   }
   free(pointer);
   return;
}

#endif  /* HEAPTEST */

void HeapFileMessage(const char* txt)
{
#ifdef HEAPTEST
   if ( heapfile == 0 ) OpenHeapFile();
   fprintf(heapfile,"\n      %s",txt);
#endif
   return;
}

void ListHeap(const char* txt)
{
#ifdef HEAPTEST
   if ( heapfile == 0 ) return;
   int numptrs  = 0;
   int numbytes = 0;
   fprintf(heapfile,"\n\nHEAP LISTING at: %s",txt);
   for ( int i=0; i<heap_dim; i++ )
   {
      if ( heap_address[i] != 0 )
      {
         numptrs++;
         numbytes += heap_bytes[i];
         fprintf(heapfile,"\naddr%5d:%10x:%7d bytes",
                          numptrs,heap_address[i],heap_bytes[i]);
      }
   }
   fprintf(heapfile,"\nMAXIMUM HEAP: %10d bytes",heap_max);
   fprintf(heapfile,"\nCURRENT HEAP: %10d bytes\n",numbytes);
#endif
   return;
}

//////////////////////////////////////////////////////////////////////////////


char ToUpper(char c)
{
   if ( c < 'a' ) return c;
   if ( c > 'z' ) return c;
   return c - ('a'-'A');
}


int stringprec = 8;


int DefStringPrec()
{
   return stringprec;
}

void DefStringPrec(int prec)
{
   if ( prec>0 && prec<16 ) stringprec = prec;
   return;
}

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

void NumToStr(REAL num, char* buf, CARDINAL bufsize,
	      BOOLEAN right, INTEGER semichars)
{
   std::ostrstream buffer;
   buffer.precision(stringprec);
   buffer << num << '\0';
   char* string = buffer.str();

   if ( right == FALSE )
   {
      strncpy(buf,string,bufsize-1);
   }
   else
   {
      INTEGER i;
      INTEGER n = strlen(string);
      if ( n >= bufsize ) n = bufsize-1;
      INTEGER numsemi = 2*n;
      for ( i=0; i<n; i++ )
      {
	 if ( string[i]=='.' || string[i]=='-' ) numsemi--;
      }
      INTEGER numblanc = semichars-numsemi;
      if ( numblanc < 0 ) numblanc = 0;
      if ( n+numblanc >=bufsize ) numblanc = bufsize-n-1;
      for ( i=0; i<numblanc; i++ ) buf[i] = ' ';
      for ( i=0; i<n; i++ ) buf[numblanc+i] = string[i];
      buf[numblanc+n] = '\0';
   }
   delete string;
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

void SetTestOut(std::ostream* testout)
{
   if ( testout != 0 ) ctest = testout;
   return;
}


void SetJobOut(std::ostream* jobout)
{
   if ( jobout != 0 ) cjob = jobout;
   return;
}


void SetFatalOut(std::ostream* fatalout)
{
   if ( fatalout != 0 ) cfatal = fatalout;
   return;
}


static void stars(std::ostream* out, CARDINAL n)
{
   for ( CARDINAL i=0; i<n; i++ ) *out << "*";
   return;
}


static void WriteMessage(std::ostream* out, const char* message)
{
   *out << "\n\n";
   stars(out,16); *out << "  AQUASIM TERMINATING PROGRAM ERROR  "; 
   stars(out,17);
   *out << "\n\n";
   *out << message;
   *out << "\n\n";
   stars(out,70);
   *out << "\n\n";
   *out << "Please report the above error message with as many details of the"
	<< "\ncontext of its occurrence as possible to\n"
	<< "\n     Peter Reichert"
	<< "\n     EAWAG"
	<< "\n     CH - 8600 Duebendorf"
	<< "\n     Switzerland\n"
	<< "\n     Email:  peter.reichert@eawag.ch"
	<< "\n     Fax:    +  41 1 823 53 98";
   *out << "\n\n";
   stars(out,70);
   *out << "\n\n";
   return;
}


void FatalError(const char* message)
{
   WriteMessage(cfatal,message);
   if ( cjob != cfatal ) WriteMessage(cjob,message);
#if AQUA_VERSION==AQUA_XVT
   xvt_terminate();
#else
   aqexit();
#endif
}

//////////////////////////////////////////////////////////////////////////////

const char* initfile   = "aquasim.ini";

const char* ptstmpfile = "_aquapts.tmp";

//////////////////////////////////////////////////////////////////////////////
