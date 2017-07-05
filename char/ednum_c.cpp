///////////////////////////////   ednum_c.C   ////////////////////////////////
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
#include "ednum_c.h"

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

static void DtMax(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(300)); // EnterDtMax
   REAL r;
   Read(r,system.DtMax());
   system.DtMax(r);
   return;
}


static void MaxOrd(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(306)); // EnterMaxOrd
   CARDINAL c;
   Read(c,system.MaxOrd());
   system.MaxOrd(c);
   return;
}


static void NumCoDiag(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(299)); // EnterNumCoDiag
   CARDINAL c;
   Read(c,system.NumCoDiag());
   system.NumCoDiag(c);
   return;
}


static void MaxNumSteps(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(308)); // EnterMaxSteps
   CARDINAL c;
   Read(c,system.MaxNumSteps());
   system.MaxNumSteps(c);
   return;
}


static void GridTimeConst(AQUASYS& system)
{
	/*
   Write("\n   "); Write(scr.T(538)); // EnterGridRelax
   REAL r;
   Read(r,system.GridTimeConst());
   system.GridTimeConst(r);
   */
   return;
}


static void FitTol(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(1393)); // EnterFitTol
   REAL r;
   Read(r,system.FitTol());
   system.FitTol(r);
   return;
}


void EditNumericParameters(AQUASYS& system)
{
   while (1)
   {
      Write("\n"); Write(scr.T(296)); Write("\n");  // EditNumParMenu
      Write('-',strlen(scr.T(296)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(298));    // EditDtMax
      Write("\n   2   =   "); Write(scr.T(441));    // EditMaxOrd
      Write("\n   3   =   "); Write(scr.T(297));    // EditNumCoDiag
      Write("\n   4   =   "); Write(scr.T(440));    // EditMaxSteps
	  /*
      Write("\n   5   =   "); Write(scr.T(537));    // EditGridTimeConst
	  */
      Write("\n   5   =   "); Write(scr.T(1394));   // EditFitTol
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return;
      switch (Buf1[0])
      {
         case '1':
            DtMax(system);
            break;
         case '2':
            MaxOrd(system);
            break;
         case '3':
            NumCoDiag(system);
            break;
         case '4':
            MaxNumSteps(system);
            break;
		/*
         case '5':
            GridTimeConst(system);
            break;
		*/
         case '5':
            FitTol(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////

