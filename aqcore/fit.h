/////////////////////////////////   fit.h   //////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    25.11.91    Peter Reichert
// revisions:   29.03.93    Peter Reichert    FitSecant added
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FIT_H
#define FIT_H

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif
#include "numpar.h"

//////////////////////////////////////////////////////////////////////////////

JOBSTATUS MinSimplex(REAL       par[],
                     const REAL minpar[],
                     const REAL maxpar[],
                     CARDINAL   npar,
                     MINFUN     f,
                     REAL       ftol,
                     CARDINAL   maxiter,
                     REAL*      fval,
                     NUMINFO&   numinfo,
                     BOOLEAN    writelog=FALSE,
                     BOOLEAN    transform=TRUE);

JOBSTATUS FitSimplex(REAL       par[],
                     const REAL minpar[],
                     const REAL maxpar[],
                     CARDINAL   npar,
                     const REAL fval[],
                     const REAL fsig[],
                     CARDINAL   nf,
                     FITFUN     f,
                     REAL       ftol,
                     CARDINAL   convcrit,
                     CARDINAL   maxiter,
                     REAL*      chi2,
                     NUMINFO&   numinfo,
                     BOOLEAN    writelog=FALSE);

JOBSTATUS FitSecant (REAL       par[],
                     const REAL minpar[],
                     const REAL maxpar[],
                     CARDINAL   npar,
                     const REAL fval[],
                     const REAL fsig[],
                     CARDINAL   nf,
                     FITFUN     f,
                     REAL       ftol,
                     CARDINAL   convcrit,
                     CARDINAL   maxiter,
                     REAL*      chi2,
                     NUMINFO&   numinfo,
                     BOOLEAN    writelog=FALSE,
                     BOOLEAN*   covcalc=0,
                     REAL*      cov=0);

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

