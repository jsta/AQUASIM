///////////////////////////////   fortran.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    18.11.91    Peter Reichert
// revisions:   14.09.99    Peter Reichert    fmod added
//
//////////////////////////////////////////////////////////////////////////////
//
// "fortran" implements FORTRAN standard functons
// ==============================================
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FORTRAN_H
#define FORTRAN_H

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////

// Macro used by the FOR_C fortran to C translater for DO and arithmetic
// IF translations

/* formula:  DOCNT(ini,tst,inc) = max( (INTEGER)((tst-ini+inc)/inc), 0 ) */
#define DOCNT(i,t,n)    (_d_l=(n), (_d_m=(t-(i)+_d_l)/_d_l) > 0 ? _d_m : 0 )
/* NOTE: _d_l & _d_m are local INTEGERs defined whenever DOCNT() is gen'd. */

//////////////////////////////////////////////////////////////////////////////

// FORTRAN standard functions

REAL fabs(REAL);

INTEGER fmin(INTEGER,INTEGER);

REAL fmin(REAL,REAL);

INTEGER fmax(INTEGER,INTEGER);

REAL fmax(REAL,REAL);

REAL fmax(REAL,REAL,REAL);

REAL fsign(REAL,REAL);

INTEGER fmod(INTEGER,INTEGER);

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
