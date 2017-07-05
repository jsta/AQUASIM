////////////////////////////////   numpar.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    25.11.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "numpar.h"

//////////////////////////////////////////////////////////////////////////////

const char* NumParIdentifier = "NUMPAR";

//////////////////////////////////////////////////////////////////////////////

#ifndef USE_TYPES_H
INIT NUMPAR::ini(initfile,"NUMPAR","num_txt_",10,10,10,10);
#endif


NUMPAR::NUMPAR()
{
   dtmax       = 1.0;
   maxord      = 5;
   numcodiag   = 1000;
   maxnumsteps = 1000;
   kgrid       = 0.0;
   interpolate = TRUE;
   fittol      = 0.005;
}


BOOLEAN NUMPAR::DtMax(REAL val)
{
   if ( val <= 0.0 ) return FALSE;
   dtmax = val;
   return TRUE;
}


BOOLEAN NUMPAR::MaxOrd(CARDINAL val)
{
   if ( val<1 || val>5 ) return FALSE;
   maxord = val;
   return TRUE;
}


BOOLEAN NUMPAR::MaxNumSteps(CARDINAL val)
{
   if ( val < 10 ) return FALSE;
   maxnumsteps = val;
   return TRUE;
}


BOOLEAN NUMPAR::GridTimeConst(REAL gridrelax)
{
   if ( gridrelax < 0.0 ) return FALSE;
   kgrid = gridrelax;
   return TRUE;
}


BOOLEAN NUMPAR::FitTol(REAL tol)
{
   if ( tol <= 0.0 ) return FALSE;
   fittol = tol;
   return TRUE;
}


#ifndef USE_TYPES_H
JOBSTATUS NUMPAR::Load(std::istream& in)
{
   char Buffer[32];
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )  return PROBLEM;
   if ( strcmp(Buffer,NumParIdentifier) != 0 )           return PROBLEM;
   if ( LoadStart(in) == FALSE )                         return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )              return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            REAL r; CARDINAL c;
            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( DtMax(r) == FALSE )                     return PROBLEM;
            if ( LoadNumber(in,&numcodiag) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&kgrid) == FALSE )        return PROBLEM;
			kgrid = 0;
            if ( LoadBoolean(in,&interpolate) == FALSE ) return PROBLEM;
            if ( LoadNumber(in,&c) == FALSE )            return PROBLEM;
            if ( MaxOrd(c) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&c) == FALSE )            return PROBLEM;
            if ( MaxNumSteps(c) == FALSE )               return PROBLEM;
         }
         break;
      case 2: // aquavers=2: AQUASIM 2.0: fit tolerance added
         {
            REAL r; CARDINAL c;
            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( DtMax(r) == FALSE )                     return PROBLEM;
            if ( LoadNumber(in,&numcodiag) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&kgrid) == FALSE )        return PROBLEM;
			kgrid = 0;
            if ( LoadBoolean(in,&interpolate) == FALSE ) return PROBLEM;
            if ( LoadNumber(in,&c) == FALSE )            return PROBLEM;
            if ( MaxOrd(c) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&c) == FALSE )            return PROBLEM;
            if ( MaxNumSteps(c) == FALSE )               return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( FitTol(r) == FALSE )                    return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                           return PROBLEM;

   return OK;
}


JOBSTATUS NUMPAR::Save(std::ostream& out)
{
   if ( SaveString(out,NumParIdentifier) == FALSE ) return PROBLEM;
   if ( SaveStart(out) == FALSE )                   return PROBLEM;

   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )         return PROBLEM;
   if ( SaveNumber(out,DtMax()) == FALSE )          return PROBLEM;
   if ( SaveNumber(out,NumCoDiag()) == FALSE )      return PROBLEM;
   if ( SaveNumber(out,GridTimeConst()) == FALSE )  return PROBLEM;
   if ( SaveBoolean(out,Interpolate()) == FALSE )   return PROBLEM;
   if ( SaveNumber(out,MaxOrd()) == FALSE )         return PROBLEM;
   if ( SaveNumber(out,MaxNumSteps()) == FALSE )    return PROBLEM;
   if ( SaveNumber(out,FitTol()) == FALSE )         return PROBLEM;

   if ( SaveEnd(out) == FALSE )                     return PROBLEM;

   return OK;
}


JOBSTATUS NUMPAR::Write(std::ostream& out, BOOLEAN)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   PrintIndent(pos2-1);

   if ( Print(out,ini.T(1),1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                  return PROBLEM;

   if ( Print(out,ini.T(2),pos1) == FALSE )        return PROBLEM;
   if ( Print(out,":") == FALSE )                  return PROBLEM;
   if ( Print(out,DtMax(),pos2) == FALSE )         return PROBLEM;

   if ( Print(out,ini.T(3),pos1) == FALSE )        return PROBLEM;
   if ( Print(out,":") == FALSE )                  return PROBLEM;
   if ( Print(out,MaxOrd(),pos2) == FALSE )         return PROBLEM;

   if ( Print(out,ini.T(4),pos1) == FALSE )        return PROBLEM;
   if ( Print(out,":") == FALSE )                  return PROBLEM;
   if ( Print(out,NumCoDiag(),pos2) == FALSE )     return PROBLEM;

   if ( Print(out,ini.T(5),pos1) == FALSE )        return PROBLEM;
   if ( Print(out,":") == FALSE )                  return PROBLEM;
   if ( Print(out,MaxNumSteps(),pos2) == FALSE )   return PROBLEM;
/*
   if ( Print(out,ini.T(6),pos1) == FALSE )        return PROBLEM;
   if ( Print(out,":") == FALSE )                  return PROBLEM;
   if ( Print(out,GridTimeConst(),pos2) == FALSE ) return PROBLEM;
*/
   return OK;
}
#endif

//////////////////////////////////////////////////////////////////////////////

NUMINFO::NUMINFO()
{
   Clear();
}


void NUMINFO::Clear()
{
   actt      = 0.0;
   actdt     = 0.0;
   actord    = 0;
   nstep     = 0;
   nfun      = 0;
   njac      = 0;
   nerrfail  = 0;
   nconvfail = 0;
   ninteger  = 0;
   nreal     = 0;
}


BOOLEAN NUMINFO::Write(std::ostream* out)
{
   *out << "\n   Numerical information parameters:";
   if ( out->fail() ) return FALSE;
   *out << "\n      actual time:                      " << actt
        << "\n      actual time step:                 " << actdt
        << "\n      actual order of integration:      " << actord
        << "\n      number of steps taken:            " << nstep
        << "\n      number of function evaluations:   " << nfun
        << "\n      number of jacobian evaluations:   " << njac
        << "\n      number of error test failures:    " << nerrfail
        << "\n      number of convergence failures:   " << nconvfail
        << "\n      INTEGER space needed:             " << ninteger
        << "\n      REAL space needed:                " << nreal
        << "\n";
   if ( out->fail() ) return FALSE;
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

