/////////////////////////////////   zero.C   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    09.12.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "linpack.h"
#include "zero.h"

//////////////////////////////////////////////////////////////////////////////

class ZERO_WORK
{
   public:

      ZERO_WORK(CARDINAL nrow, CARDINAL ncol);
      ~ZERO_WORK();

      REAL*    jac;
      REAL*    oldjac;
      REAL*    xsav;
      REAL*    xold;
      REAL*    delta;
      REAL*    deltaold;
      REAL*    p;
      REAL*    q;
      REAL*    inc;
      INTEGER* ipvt;
};


ZERO_WORK::ZERO_WORK(CARDINAL nrow, CARDINAL ncol)
{
   jac      = new REAL[nrow*ncol];
   oldjac   = new REAL[nrow*ncol];
   xsav     = new REAL[ncol];
   xold     = new REAL[ncol];
   delta    = new REAL[ncol];
   deltaold = new REAL[ncol];
   p        = new REAL[ncol];
   q        = new REAL[ncol];
   inc      = new REAL[ncol];
   ipvt     = new INTEGER[ncol];
}


ZERO_WORK::~ZERO_WORK()
{
   delete jac;
   delete oldjac;
   delete xsav;
   delete xold;
   delete delta;
   delete deltaold;
   delete p;
   delete q;
   delete inc;
   delete ipvt;
}



//////////////////////////////////////////////////////////////////////////////

JOBSTATUS ZeroNewton(REAL          x[],
                     const REAL    relacc[],
                     const REAL    absacc[],
                     CARDINAL      neq,
                     ZEROFUN       f,
                     CARDINAL      maxstep,
                     REAL          delta[],
                     const NUMPAR& numpar,
                     NUMINFO&      numinfo)
{
   const REAL eps     = 0.1;
   const REAL fact    = 0.1;
   BOOLEAN    fulljac = TRUE;
   INTEGER    nrow    = neq;
   INTEGER    nco     = numpar.NumCoDiag();
   if ( 3*nco < neq )
   {
      fulljac = FALSE;
      nrow = 1+3*nco;
   }
   ZERO_WORK w(nrow,neq);
   numinfo.Clear();
   numinfo.ninteger = neq;
   numinfo.nreal = 2*nrow*neq + 8*neq;
   INTEGER i,j,ii;

   if ( neq == 0 ) return PROBLEM;         // check input
   for ( i=0; i<neq; i++ )
   {
      if ( (relacc[i]<0.0) || (absacc[i]<0.0) ) return PROBLEM;
      if ( relacc[i]+absacc[i] <= 0.0 )         return PROBLEM;
   }

   ii = 10;
   numinfo.nfun++;
   JOBSTATUS status = f(x,neq,delta);
   if ( status != OK ) return status;
   REAL oldnorm = 0.0;
   for ( i=0; i<neq; i++ )
   {
      oldnorm += delta[i]*delta[i];
   }
   while ( 1 )
   {
      BOOLEAN ok = TRUE;                  // check end conditions
      for ( i=0; i<neq; i++ )
      {
         if ( !aqfinite(delta[i]) || !aqfinite(x[i]) )
         {
            ok = FALSE; break;
         }
         if ( ! ( fabs(delta[i]) < eps*(relacc[i]*fabs(x[i])+absacc[i]) ) )
         {
            ok = FALSE; break;
         }
      }
      if ( ok == TRUE ) break;
      if ( numinfo.nstep >= maxstep ) return PROBLEM;

      for ( i=0; i<neq; i++ ) w.xsav[i] = x[i];

      if ( ii > 1 )                      // calculate new jacobian
      {
         numinfo.njac++;
         for ( i=0; i<neq; i++ )
         {
            w.inc[i] = fact*(relacc[i]*fabs(x[i])+absacc[i]);
         }
         if ( fulljac == TRUE )          // full jacobian
         {
            for ( j=0; j<neq; j++ )
            {
               x[j] = x[j] + w.inc[j];              // increment x
               status = f(x,neq,w.delta);           // evaluate function
               x[j] = w.xsav[j];                    // restore x
               if ( status != OK ) return status;
               for ( i=0; i<neq; i++ )              // calculate jacobian
               {
                  w.jac[i+neq*j] = (w.delta[i]-delta[i])/w.inc[j];
/*
if ( w.jac[i+neq*j] != 0.0 )
{
   cout << "\ndiff=" << abs(i-j) << " i=" << i << " j=" << j
        << " jac=" << w.jac[i+neq*j] << " ";
   for ( int iii=0; iii<abs(i-j); iii++ ) cout << "x";
   cout.flush();
}
*/
               }
            }
         }
         else                            // banded jacobian
         {
            INTEGER k;
            for ( k=0; k<2*nco+1; k++ )
            {
               for ( j=k; j<neq; j+=2*nco+1 )       // increment x
               {
                  x[j] = x[j] + w.inc[j];
               }
               status = f(x,neq,w.delta);           // evaluate function
               for ( j=k; j<neq; j+=2*nco+1 )       // restore x
               {
                  x[j] = w.xsav[j];
               }
               if ( status != OK ) return status;
               for ( j=k; j<neq; j+=2*nco+1 )       // calculate jac
               {
                  for ( i=j-nco; i<=j+nco; i++ )
                  {
                     if ( (i>=0) && (i<neq) )
                     {
                        w.jac[2*nco+i-j+nrow*j]
                                  = (w.delta[i]-delta[i])/w.inc[j];
                     }
                  }
               }
            }
         }
      }
      else                              // derive approximate new jacobion 
      {                                 // from old jacobian
         REAL pp = 0;
         for ( i=0; i<neq; i++ )
         {
            w.p[i] = x[i] - w.xold[i];
            w.q[i] = delta[i] - w.deltaold[i];
            pp = pp + w.p[i]*w.p[i];
         }
         if ( fulljac == TRUE )          // full jacobian
         {
            for ( i=0; i<neq; i++ )
            {
               for ( j=0; j<neq; j++ )
               {
                  w.q[i] = w.q[i] - w.oldjac[i+neq*j]*w.p[j];
               }
            }
            for ( i=0; i<neq; i++ )
            {
               for ( j=0; j<neq; j++ )
               {
                  w.jac[i+neq*j] = w.oldjac[i+neq*j] + w.q[i]*w.p[j]/pp;
               }
            }
         }
         else                            // banded jacobian
         {
            INTEGER k;
            for ( k=0; k<2*nco+1; k++ )
            {
               for ( j=k; j<neq; j+=2*nco+1 )
               {
                  for ( i=j-nco; i<=j+nco; i++ )
                  {
                     if ( (i>=0) && (i<neq) )
                     {
                        w.q[i] = w.q[i] - w.oldjac[2*nco+i-j+nrow*j]*w.p[j];
                     }
                  }
               }
            }
            for ( k=0; k<2*nco+1; k++ )
            {
               for ( j=k; j<neq; j+=2*nco+1 )
               {
                  for ( i=j-nco; i<=j+nco; i++ )
                  {
                     if ( (i>=0) && (i<neq) )
                     {
                        w.jac[2*nco+i-j+nrow*j] = w.oldjac[2*nco+i-j+nrow*j]
                                                  + w.q[i]*w.p[j]/pp;
                     }
                  }
               }
            }
         }
      }

      for ( i=0; i<neq; i++ )
      {
         w.xold[i]     = x[i];
         w.deltaold[i] = delta[i];
         w.delta[i]    = delta[i];
      }
      INTEGER info;
      if ( fulljac == TRUE )             // full jacobian
      {
         for ( i=0; i<neq*neq; i++ ) w.oldjac[i] = w.jac[i];
         dgefa(w.jac,neq,neq,w.ipvt,&info);
         dgesl(w.jac,neq,neq,w.ipvt,w.delta,0);
         
      }
      else                               // banded jacobian
      {
         for ( i=0; i<nrow*neq; i++ ) w.oldjac[i] = w.jac[i];
         dgbfa(w.jac,nrow,neq,nco,nco,w.ipvt,&info);
         dgbsl(w.jac,nrow,neq,nco,nco,w.ipvt,w.delta,0);
      }
      ii = 0; REAL norm = 0;
      while ( 1 )
      {
         REAL lambda = pow(0.5,ii);
         for ( i=0; i<neq; i++ )
         {
            x[i] = w.xold[i] - lambda*w.delta[i];
         }
         numinfo.nfun++;
         status = f(x,neq,delta);          // evaluate function
         if ( status == INTERRUPT ) return status;
         if ( status == OK )
         {
            norm = 0;
            for ( i=0; i<neq; i++ )
            {
               norm += delta[i]*delta[i];
            }
            if ( norm < oldnorm )
            {
               oldnorm = norm; break;
            }
         }
         ii++;
         if ( ii > 20 )
         {
            for ( i=0; i<neq; i++ )
            {
               x[i] = x[i] - fsign(fabs(w.inc[i]),w.delta[i]);
            }
            numinfo.nfun++;
            status = f(x,neq,delta);       // evaluate function
            if ( status != OK ) return status;
            oldnorm = 0.0;
            for ( i=0; i<neq; i++ )
            {
               oldnorm += delta[i]*delta[i];
            }
            break;
         }
      }
      numinfo.nstep++;
   }
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

