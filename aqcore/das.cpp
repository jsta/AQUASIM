/////////////////////////////////   das.C   //////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   16.10.91    Peter Reichert
//              25.11.91    Peter Reichert    numpar.h extracted
//              25.02.98    Peter Reichert    init. of alg. equations improved
//              25.05.98    Peter Reichert    aqfinite-tests implemented
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "all.h"
#include "linpack.h"
#include "dassl.h"
#include "das.h"

//////////////////////////////////////////////////////////////////////////////

// global variables and constants

class WORKSPACE
{
   public:

      WORKSPACE();
      ~WORKSPACE();

      INTEGER  Lrw;
      INTEGER  Liw;
      REAL*    rw;
      INTEGER* iw;
};

WORKSPACE::WORKSPACE()
{
   Lrw = 0; Liw = 0; rw = 0; iw = 0;
}

WORKSPACE::~WORKSPACE()
{
   delete rw; rw = 0; Lrw = 0;
   delete iw; iw = 0; Liw = 0;
}

static WORKSPACE work;

const CARDINAL  Linfo = 15;
static INTEGER  info[Linfo];

static DASFUN   diff    = 0;
static INTEGER  neqdiff = 0;

//////////////////////////////////////////////////////////////////////////////


static void jac(REAL*, REAL[], REAL[], REAL[], REAL*, REAL[], INTEGER[])
{
   FatalError("solvedas::jac: function jac called");
}

// function for DASSL-interface:

static void res(REAL* t, REAL y[], REAL yt[], REAL delta[], INTEGER* ires,
                REAL[], INTEGER[])
{
   JOBSTATUS stat = diff(*t,y,yt,delta);

   // section added 25.05.98:
   for ( CARDINAL i=0; i<neqdiff; i++ )
   {
      if ( !aqfinite(delta[i]) )
      {
         stat = PROBLEM; break;
      }
   }
   // end of section added 25.05.98

   switch (stat)
   {
      case PROBLEM:
         *ires = -1; break;
      case INTERRUPT:
         *ires = -2; break;
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

// implementation of the C++ interface routines

static struct DASINITGLOB 
{
   CARDINAL      neq;
   REAL          t;
   REAL*         y;
   REAL*         yt;
   const EQTYPE* eqtype;
   BOOLEAN       steady;
   DASFUN        das;
} globvar;


// function for initialization of differential equations with ZeroNewton:

static JOBSTATUS fun(const REAL x[], CARDINAL neq, REAL delta[])
{
   CARDINAL i;
   if ( globvar.steady == TRUE )
   {
      for ( i=0; i<neq; i++ )
      {
         globvar.y[i] = x[i];
      }
   }
   else
   {
      for ( i=0; i<neq; i++ )
      {
         if ( globvar.eqtype[i] == ALG )
         {
            globvar.y[i] = x[i];
         }
         else
         {
            globvar.yt[i] = x[i];
         }
      }
   }
   JOBSTATUS status = (globvar.das)(globvar.t,globvar.y,globvar.yt,delta);

   // section added 25.05.98:
   for ( i=0; i<neq; i++ )
   {
      if ( !aqfinite(delta[i]) )
      {
         status = PROBLEM; break;
      }
   }
   // end of section added 25.05.98

   return status;
}


// function for initialization of algebraic equations with ZeroNewton:

static JOBSTATUS funalg(const REAL x[], CARDINAL neq, REAL delta[])
{
   CARDINAL i;
   CARDINAL k=0;
   for ( i=0; i<globvar.neq; i++ )
   {
      if ( globvar.eqtype[i] == ALG )
      {
         globvar.y[i] = x[k];
         k++;
      }
   }
   REAL* deltatmp = new REAL[globvar.neq];
   JOBSTATUS status = (globvar.das)(globvar.t,globvar.y,globvar.yt,deltatmp);
   k=0;
   for ( i=0; i<globvar.neq; i++ )
   {
      if ( globvar.eqtype[i] == ALG )
      {
         delta[k] = deltatmp[i];
         k++;
      }
   }

   // section added 25.05.98:
   for ( i=0; i<globvar.neq; i++ )
   {
      if ( !aqfinite(deltatmp[i]) )
      {
         status = PROBLEM; break;
      }
   }
   // end of section added 25.05.98

   delete [] deltatmp;
   return status;
}


JOBSTATUS DasInit(DASFUN        das,
                  REAL          t,
                  REAL*         y,
                  REAL*         yt,
                  const REAL*   relacc,
                  const REAL*   absacc,
                  const EQTYPE* eqtype,
                  CARDINAL      neq,
                  BOOLEAN       steady,
                  const NUMPAR& numpar,
                  NUMINFO&      numinfo)
{
   delete work.iw; work.iw = 0; work.Liw = 0;
   delete work.rw; work.rw = 0; work.Lrw = 0;
   CARDINAL i;
   JOBSTATUS status;
   REAL* x     = new REAL[neq];
   REAL* delta = new REAL[neq];
   globvar.neq    = neq;
   globvar.t      = t;
   globvar.y      = y;
   globvar.yt     = yt;
   globvar.eqtype = eqtype;
   globvar.steady = steady;
   globvar.das    = das;
   if ( steady == TRUE )
   {
      for ( i=0; i<neq; i++ )
      {
         yt[i] = 0.0;
         x[i] = y[i];
      }
   }
   else
   {
      for ( i=0; i<neq; i++ )
      {
         if ( eqtype[i] == ALG )
         {
            x[i] = y[i];
         }
         else
         {
            //x[i] = yt[i];
            x[i] = 1.0;
         }
      }
   }

   // section added 25.02.98:
   // initialization of algebraic equations:
   if ( steady == FALSE )
   {
      CARDINAL neqalg = 0;
      for ( i=0; i<neq; i++ )
      {
         if ( eqtype[i] == ALG ) neqalg++;
      }
      if ( neqalg > 0 )
      {
         REAL* xalg      = new REAL[neqalg];
         REAL* absaccalg = new REAL[neqalg];
         REAL* relaccalg = new REAL[neqalg];
         CARDINAL k=0;
         for ( i=0; i<neq; i++ )
         {
            if ( eqtype[i] == ALG )
            {
               xalg[k]      = x[i];
               relaccalg[k] = relacc[i];
               absaccalg[k] = absacc[i];
               k++;
            }
         }
         status = ZeroNewton(xalg,relaccalg,absaccalg,neqalg,funalg,
                             100,delta,numpar,numinfo);
         if ( status == OK )
         {
            CARDINAL k=0;
            for ( i=0; i<neq; i++ )
            {
               if ( eqtype[i] == ALG )
               {
                  x[i]      = xalg[k];
                  k++;
               }
            }
         }
         delete [] xalg;
         delete [] absaccalg;
         delete [] relaccalg;
         if ( status != OK ) return status;
      }
   }
   // end of section added 25.02.98

   // section added 15.08.97:
   // initialization of derivatives of differential equations:
   if ( steady == FALSE )
   {
      status = fun(x,neq,delta);
      if ( status != OK ) return status;
      for ( i=0; i<neq; i++ )
      {
         if ( eqtype[i] == DIFF ) x[i] = x[i]+delta[i];
      }
   }
   // end of section added 15.08.97
      
   status = ZeroNewton(x,relacc,absacc,neq,fun,
                       100,delta,numpar,numinfo);
   if ( steady == TRUE )
   {
      for ( i=0; i<neq; i++ )
      {
         y[i] = x[i];
      }
   }
   else
   {
      for ( i=0; i<neq; i++ )
      {
         if ( eqtype[i] == ALG )
         {
            y[i] = x[i];
         }
         else
         {
            yt[i] = x[i];
         }
      }
   }
   delete x;
   delete delta;
   return status;
}


JOBSTATUS DasGear(DASFUN        das,
                  REAL&         t,
                  REAL*         y,
                  REAL*         yt,
                  REAL*         relacc,
                  REAL*         absacc,
                  CARDINAL      neq,
                  REAL          tout,
                  BOOLEAN&      start,
				  std::ostream*      errmes,
                  const NUMPAR& numpar,
                  NUMINFO&      numinfo)
{
   diff = das;                              // set global variables
   neqdiff = neq;

   INTEGER idid = 1;                        // convert variable types
   INTEGER n = neq;
   REAL    tend = tout;

   if ( start == TRUE )                     // initialize global arrays
   {
      CARDINAL Lold = work.Liw;
      work.Liw = 20 + neq;
      if ( work.Liw != Lold )
      {
         delete work.iw; work.iw = new INTEGER[work.Liw];
      }
      for ( CARDINAL i=0; i<Linfo; i++ ) info[i] = 0;
      Lold = work.Lrw;
      if ( 3*numpar.NumCoDiag() < neq )     // banded jacobian
      {
         info[5] = 1;
         work.iw[0] = numpar.NumCoDiag(); work.iw[1] = numpar.NumCoDiag();
         work.Lrw = 40 + 9*neq + (3*numpar.NumCoDiag()+1)*neq
                             + 2*(neq/(2*numpar.NumCoDiag()+1)+1);
      }
      else                                  // full jacobian
      {
         work.Lrw = 40 + 9*neq + neq*neq;
      }
      if ( work.Lrw != Lold )
      {
         delete work.rw; work.rw = new REAL[work.Lrw];
      }
      info[1] = 1;                               // tolerances as vectors
      if ( numpar.Interpolate() == FALSE )
      {
         info[3] = 1; work.rw[0] = tout;         // no interpolation
      }
      info[6] = 1; work.rw[1] = numpar.DtMax();  // maximum step size
      info[8] = 1; work.iw[2] = numpar.MaxOrd(); // maximum order
      start = FALSE;
   }

   if ( numpar.Interpolate() == FALSE ) work.rw[0] = tout; // set tstop

   REAL*    rpar = 0;                          // call dassl
   INTEGER* ipar = 0;
   dassl(res,&n,&t,y,yt,&tend,info,relacc,absacc,&idid,
         work.rw,&work.Lrw,work.iw,&work.Liw,rpar,ipar,jac,
         *errmes,numpar.MaxNumSteps());

   numinfo.actt      = work.rw[ 3];            // assign variables of numinfo
   numinfo.actdt     = work.rw[ 6];
   numinfo.actord    = work.iw[ 7];
   numinfo.nstep     = work.iw[10];
   numinfo.nfun      = work.iw[11];
   numinfo.njac      = work.iw[12];
   numinfo.nerrfail  = work.iw[13];
   numinfo.nconvfail = work.iw[14];
   numinfo.ninteger  = work.Liw;
   numinfo.nreal     = work.Lrw;

   if ( idid >    0 ) return OK;
   if ( idid == -11 )
   {
      info[0] = 1;
      return INTERRUPT;
   }
   return PROBLEM;
}

//////////////////////////////////////////////////////////////////////////////
