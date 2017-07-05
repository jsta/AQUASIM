/////////////////////////////////   das.h   //////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   16.10.91    Peter Reichert    InitDAS added
//              25.11.91    Peter Reichert    numpar.h extracted
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// C++ interface for Differential Algebraic System Solver DASSL
//
// original FORTRAN version due to Linda R. Petzold
//                                 Sandia National Laboratories
//                                 Livermore, Ca. 94550
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DAS_H
#define DAS_H

#include "all.h"
#include "numpar.h"

//////////////////////////////////////////////////////////////////////////////

// C++ interface to the differential algebraic system solver DASSL:

JOBSTATUS DasInit(DASFUN        das,      // definition of the system
                  REAL          t,        // initial time
                  REAL*         y,        // initial values of y (changed to
                                          // consistent initial values)
                  REAL*         yt,       // initial values of yt (changed to
                                          // consistent initial values)
                  const REAL*   relacc,   // relative accuracies
                  const REAL*   absacc,   // absolute accuracies
                  const EQTYPE* eqtype,   // type of equations (needed to de-
                                          // cide if y or yt has to be
                                          // changed, see EQTYPE)
                  CARDINAL      neq,      // number of equations (dimension
                                          // of y,yt,relacc,absacc,eqtype)
                  BOOLEAN       steady,   // FALSE: initial solution as
                                          //        given is made consistent
                                          // TRUE:  steady state solution is
                                          //        calculated
                  const NUMPAR& numpar,   // numerical parameters
                  NUMINFO&      numinfo); // information on number of steps
                                          // taken


JOBSTATUS DasGear(DASFUN        das,      // definition of the system
                  REAL&         t,        // actual time (changed according
                                          // to the progress of integration)
                  REAL*         y,        // actual values of y (changed by
                                          // integrator)
                  REAL*         yt,       // actual values of yt (changed by
                                          // integrator)
                  REAL*         relacc,   // relative accuracies
                  REAL*         absacc,   // absolute accuracies
                  CARDINAL      neq,      // number of equations (dimension
                                          // of y, yt, relacc, absacc)
                  REAL          tout,     // time of next output
                  BOOLEAN&      start,    // if TRUE integration is restarted
                                          // and start is set to FALSE
				  std::ostream* errmes,   // ostream for error messages
                  const NUMPAR& numpar,   // numerical parameters
                  NUMINFO&      numinfo); // information on progress of the 
                                          // integration process

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

