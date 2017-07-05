///////////////////////////////   fortran.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    18.11.91    Peter Reichert
// revisions:   14.09.99    Peter Reichert    fmod added
//
//////////////////////////////////////////////////////////////////////////////

#include "fortran.h"

//////////////////////////////////////////////////////////////////////////////

REAL fabs(REAL r)
{
   if ( r >= 0.0 ) return  r;
   else            return -r;
}

INTEGER fmin(INTEGER i1, INTEGER i2)
{
   if ( i1 < i2 ) return i1;
   else           return i2;
}

REAL fmin(REAL r1, REAL r2)
{
   if ( r1 < r2 ) return r1;
   else           return r2;   
}

INTEGER fmax(INTEGER i1, INTEGER i2)
{
   if ( i1 > i2 ) return i1;
   else           return i2;
}

REAL fmax(REAL r1, REAL r2)
{
   if ( r1 > r2 ) return r1;
   else           return r2;   
}

REAL fmax(REAL r1, REAL r2, REAL r3)
{
   if ( r1 > r2 )
   {
      if ( r1 > r3 ) return r1;
      else           return r3;
   }
   else
   {
      if ( r2 > r3 ) return r2;
      else           return r3;
   }
}

REAL fsign(REAL r1, REAL r2)
{
   if ( r2 < 0.0 ) return -fabs(r1);
   else            return  fabs(r1);
}

INTEGER fmod(INTEGER i1, INTEGER i2)  // to be replaced!
{
   INTEGER i = 0;
   while ( (i+1)*i2 <= i1 ) i++;
   return i1-i*i2;
}

//////////////////////////////////////////////////////////////////////////////

