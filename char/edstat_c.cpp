///////////////////////////////   edstat_c.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "scrio_c.h"
#include "edstat_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters (aquasim.C)

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];
extern char            Buf3[];
extern char            Buf4[];

extern char            C1, C2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

void EditDeleteStates(AQUASYS& system)
{
   while ( 1 )
   {
      if ( system.NumCalc() == 0 )
      {
         Write("\n   "); Write(scr.T(283)); Write("\n");    // No States
         return;
      }
      Write("\n   "); Write(scr.T(820)); Write(":\n   ");   // States
      for ( CARDINAL i=0; i<system.NumCalc(); i++ )
      {
         Write(system.CalcNums(i));
         if ( i+1 < system.NumCalc() ) Write(",");
      }
      Write("\n\n   "); Write(scr.T(821));                  // Select
      Write("\n   1   =   "); Write(scr.T(822));            // Delete
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) break;
      Write("\n   "); Write(scr.T(823));                    // EnterMaxSteps
      CARDINAL calcnum = system.CalcNums(0);
      Read(calcnum,calcnum);
      if ( system.DeleteStates(calcnum) == TRUE )
      {
         Write("\n   "); Write(scr.T(824)); Write("\n");    // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(825)); Write(" ");     // Failure
         Write(calcnum); Write("\n");
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

