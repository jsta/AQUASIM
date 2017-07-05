////////////////////////////////   main_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "fileio.h"
#include "aquasys.h"
#include "scrio_c.h"
#include "file_c.h"
#include "edvar_c.h"
#include "edproc_c.h"
#include "edcomp_c.h"
#include "edlink_c.h"
#include "ednum_c.h"
#include "edstat_c.h"
#include "calc_c.h"
#include "view_c.h"

//////////////////////////////////////////////////////////////////////////////

#define BUFLEN 512   // problems of BORLAND compiler 2.0 with extern const

extern const CARDINAL BufLen = BUFLEN;

char Buf[BUFLEN];
char Buf1[BUFLEN];
char Buf2[BUFLEN];
char Buf3[BUFLEN];
char Buf4[BUFLEN];
char Buf5[BUFLEN];
char Buf6[BUFLEN];

#undef BUFLEN

std::ofstream aqualog;

char C1 = 'C';
char C2 = 'c';
char E1 = 'E';
char E2 = 'e';
char Y1 = 'Y';
char Y2 = 'y';
char N1 = 'N';
char N2 = 'n';

const CARDINAL FileNameSize = 128;
char AQ_SaveFile[FileNameSize]  = "";
char AQ_WriteFile[FileNameSize] = "";
char AQ_SensFile[FileNameSize]  = "";
char AQ_FitFile[FileNameSize]   = "";
char AQ_PlotFile[FileNameSize]  = "";
char AQ_ListFile[FileNameSize]  = "";

//////////////////////////////////////////////////////////////////////////////

static void Edit(AQUASYS& system)
{
   while (1)
   {
      Write("\n"); Write(scr.T(33)); Write("\n");  // EditMenu
      Write('-',strlen(scr.T(33)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(34));    // EditVar
      Write("\n   2   =   "); Write(scr.T(35));    // EditProc
      Write("\n   3   =   "); Write(scr.T(36));    // EditComp
      Write("\n   4   =   "); Write(scr.T(450));   // EditLink
      Write("\n   5   =   "); Write(scr.T(301));   // EditNumPar
      Write("\n   6   =   "); Write(scr.T(819));   // DelStates
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return;
      switch (Buf1[0])
      {
         case '1':
            EditVariables(system);
            return;
         case '2':
            EditProcesses(system);
            return;
         case '3':
            EditCompartments(system);
            return;
         case '4':
            EditLinks(system);
            return;
         case '5':
            EditNumericParameters(system);
            return;
         case '6':
            EditDeleteStates(system);
            return;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
#include <floatingpoint.h>
extern "C" int ieee_handler(char*,char*,sigfpe_handler_type);
#endif

int main(int argc, char* argv[])
{
   BOOLEAN b;
   std::cout.precision(scr.I(2));

#ifdef DEBUG
ieee_handler("set","common",SIGFPE_ABORT);
#endif

   AQUASYS system;
   aqualog.open("aquasim.log",std::ios::out|std::ios::trunc);
   SetFileType("aquasim.log",FTlog,FClog);
   if ( ! aqualog.fail() ) SetJobOut(&aqualog);
   system.InitLogFile();

   if ( (strlen(scr.T(7))>0) && (strlen(scr.T(8))>0) )
   {
      char c1 = scr.T(7)[0]; char e1 = scr.T(8)[0];
      if ( (c1>='A') && (c1<='Z') && (e1>='A') && (e1<='Z') && (c1!=e1) )
      {
         C1 = c1; C2 = c1 + 'a' - 'A';
         E1 = e1; E2 = e1 + 'a' - 'A';
      }
   }
   if ( (strlen(scr.T(14))>0) && (strlen(scr.T(15))>0) )
   {
      char y1 = scr.T(14)[0]; char n1 = scr.T(15)[0];
      if ( (y1>='A') && (y1<='Z') && (n1>='A') && (n1<='Z') && (y1!=n1) )
      {
         Y1 = y1; Y2 = y1 + 'a' - 'A';
         N1 = n1; N2 = n1 + 'a' - 'A';
      }
   }

   Write("\n\n\n"); Write('*',strlen(scr.T(1))+8);
   Write("\n*"); Write(' ',strlen(scr.T(1))+6); Write("*\n*   ");
   Write(scr.T(1)); Write("   *\n*"); Write(' ',strlen(scr.T(1))+6);
   Write("*\n"); Write('*',strlen(scr.T(1))+8); Write("\n");
   WriteBuffer();
   if ( argc > 1 ) File(system,argv[1]);
   while (1)
   {
      Write("\n"); Write(scr.T(2)); Write("\n"); Write('-',strlen(scr.T(2)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(3));
      Write("\n   2   =   "); Write(scr.T(4));
      Write("\n   3   =   "); Write(scr.T(5));
      Write("\n   4   =   "); Write(scr.T(6));
      Read(Buf,BufLen);
      b = TRUE;
      switch (Buf[0])
      {
         case '1':
            b = File(system);
            break;
         case '2':
            Edit(system);
            break;
         case '3':
            Calc(system);
            break;
         case '4':
            View(system);
            break;
         default:
            break;
      }
      if ( b == FALSE ) break;
   }
   Write("\n"); Write(scr.T(18)); Write("\n\n");
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
