/////////////////////////////////   fit.C   //////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    25.11.91    Peter Reichert
// revisions:   29.03.93    Peter Reichert    FitSecant added
// revisions:   12.04.00    Peter Reichert    MinSimplex: Decrease of stepsize
//                                            added (in case of illegal pars)
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "linpack.h"
#include "fit.h"

//////////////////////////////////////////////////////////////////////////////

const REAL factinsens = 0.01;

//////////////////////////////////////////////////////////////////////////////

class MIN_WORK
{
   public:

      MIN_WORK(CARDINAL n);
      ~MIN_WORK();

      REAL**   p;
      REAL*    pr;
      REAL*    prr;
      REAL*    pbar;
      REAL*    y;

   private:

      CARDINAL n;
};


MIN_WORK::MIN_WORK(CARDINAL npar)
{
   n    = npar;
   p    = new REAL*[n+1];
   for ( CARDINAL i=0; i<n+1; i++ ) p[i] = new REAL[n];
   pr   = new REAL[n];
   prr  = new REAL[n];
   pbar = new REAL[n];
   y    = new REAL[n+1];
}


MIN_WORK::~MIN_WORK()
{
   for ( CARDINAL i=0; i<n+1; i++ ) delete [] p[i];
   delete [] p;
   delete [] pr;
   delete [] prr;
   delete [] pbar;
   delete [] y;
}


class FIT_WORK
{
   public:

      FIT_WORK(CARDINAL np, CARDINAL nf);
      ~FIT_WORK();

      REAL**    p;
      REAL**    f;
      REAL*     chi;
      REAL*     m;
      REAL*     a;
      INTEGER*  ipvt;
      REAL*     pnew;
      REAL*     fnew;
      CARDINAL* ind;
      BOOLEAN*  low;
      CARDINAL* indold;
      BOOLEAN*  lowold;

   private:

      CARDINAL npar;
      CARDINAL nfun;
};


FIT_WORK::FIT_WORK(CARDINAL np, CARDINAL nf)
{
   CARDINAL i;
   npar   = np;
   nfun   = nf;
   p      = new REAL*[npar+1];
   for ( i=0; i<npar+1; i++ )
   {
      p[i] = 0;
      if ( npar > 0 ) p[i] = new REAL[npar];
   }
   f    = new REAL*[npar+1];
   for ( i=0; i<npar+1; i++ ) f[i] = new REAL[nfun];
   chi    = new REAL[npar+1];
   fnew   = new REAL[nfun];
   m      = 0;
   a      = 0;
   ipvt   = 0;
   pnew   = 0;
   ind    = 0;
   low    = 0;
   indold = 0;
   lowold = 0;
   if ( npar > 0 )
   {
      m      = new REAL[4*npar*npar];
      a      = new REAL[2*npar];
      ipvt   = new INTEGER[2*npar];
      pnew   = new REAL[npar];
      ind    = new CARDINAL[npar];
      low    = new BOOLEAN[npar];
      indold = new CARDINAL[npar];
      lowold = new BOOLEAN[npar];
   }
}


FIT_WORK::~FIT_WORK()
{
   CARDINAL i;
   for ( i=0; i<npar+1; i++ )
   {
      delete [] p[i]; delete [] f[i];
   }
   delete [] p; delete [] f;
   delete [] chi;
   delete [] m;
   delete [] a;
   delete [] ipvt;
   delete [] pnew;
   delete [] fnew;
   delete [] ind;
   delete [] low;
   delete [] indold;
   delete [] lowold;
}
   

//////////////////////////////////////////////////////////////////////////////

static void trans_forw(REAL par[], const REAL min[], const REAL max[],
                       CARDINAL npar, BOOLEAN transform)
{
   if ( transform == FALSE ) return;
   for ( CARDINAL i=0; i<npar; i++ )
     par[i] = tan(aq_pi*0.5*(2.0*par[i]-min[i]-max[i])/(max[i]-min[i]));
   return;
}


static void trans_back(REAL par[], const REAL min[], const REAL max[],
                       CARDINAL npar, BOOLEAN transform)
{
   if ( transform == FALSE ) return;
   for ( CARDINAL i=0; i<npar; i++ )
      par[i] = 0.5*(min[i]+max[i]) +(max[i]-min[i])/aq_pi*atan(par[i]);
   return;
}


static JOBSTATUS fun(const REAL p[], const REAL min[], const REAL max[],
                     CARDINAL n, MINFUN f, REAL* fval, REAL fref,
                     BOOLEAN transform)
{
   CARDINAL i;
   JOBSTATUS status = OK;
   if ( transform == FALSE )
   {
      for ( i=0; i<n; i++ )
      {
         if ( (p[i]<min[i]) || (p[i]>max[i]) )
         {                                         // prevent algorithm
            *fval = 2.0*fabs(fref); return status; // from going outside
         }                                         // of allowed interval
      }
      status = f(p,n,fval);
   }
   else
   {
      REAL* ptrans = new REAL[n];
      for ( i=0; i<n; i++ ) ptrans[i] = p[i];      // transform p to original
      trans_back(ptrans,min,max,n,transform);      // coordinates before
      status = f(ptrans,n,fval);                   // calling f
      delete [] ptrans;
   }
   return status;
}

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
                     BOOLEAN    writelog,
                     BOOLEAN    transform)
{
   const REAL alpha        = 1.0;          // assign parameters of algorithm
   const REAL beta         = 0.5;
   const REAL gamma        = 2.0;
   const REAL factbound    = 0.001;
   const REAL factini      = 0.1;
   const CARDINAL minsteps = 3;

   CARDINAL   i,j;                         // declare global variables
   JOBSTATUS  status;
   MIN_WORK   w(npar);
   numinfo.Clear();

/*
   if ( npar < 1 ) return PROBLEM;         // check input
*/
   for ( i=0; i<npar; i++ )
   {
      if ( minpar[i] >= maxpar[i] ) return PROBLEM;
      if ( par[i] <= minpar[i] )
         par[i] = minpar[i] + factbound*(maxpar[i]-minpar[i]);
      if ( par[i] >= maxpar[i] )
         par[i] = maxpar[i] - factbound*(maxpar[i]-minpar[i]);
   }

   if ( writelog == TRUE )
   {
      *cjob << "\n ----initialization--------------"
            << "-----------------------------------";
   }
   for ( j=0; j<npar+1; j++ )              // initialize arrays p and y
   {
      for ( i=0; i<npar; i++ ) w.p[j][i] = par[i];
   }
   for ( i=0; i<npar; i++ )
   {
      w.p[i+1][i] = par[i] + factini*(maxpar[i]-minpar[i]);
      if ( w.p[i+1][i] >= maxpar[i] )
         w.p[i+1][i] = par[i] - factini*(maxpar[i]-minpar[i]);
   }

   numinfo.nfun++;
   status = f(w.p[0],npar,&(w.y[0]));
   if ( status != OK ) return status;
   if ( maxiter==0 || npar==0 ) return OK;
   BOOLEAN notsens = FALSE;
   for ( j=1; j<npar+1; j++ )
   {
      numinfo.nfun++;
      status = f(w.p[j],npar,&(w.y[j]));
      if ( status == INTERRUPT ) return status;
      if ( status == PROBLEM )       // illegal parameters: decrease step size
      {
         w.p[j][j-1] = par[j-1] + 0.1*(w.p[j][j-1]-par[j-1]);
         numinfo.nfun++;
         status = f(w.p[j],npar,&(w.y[j]));
         if ( status == PROBLEM )
         {
            w.p[j][j-1] = par[j-1] + 0.1*(w.p[j][j-1]-par[j-1]);
            numinfo.nfun++;
            status = f(w.p[j],npar,&(w.y[j]));
            if ( status == PROBLEM )
            {
               w.p[j][j-1] = par[j-1] + 0.1*(w.p[j][j-1]-par[j-1]);
               numinfo.nfun++;
               status = f(w.p[j],npar,&(w.y[j]));
               if ( status == PROBLEM )
               {                                           
                  *cjob << "\n   *** Problem varying parameter (illegal evaluation)";
                  return PROBLEM;
               }
            }
         }
      }
      if ( 2.0*fabs(w.y[j]-w.y[0])   // not sensitive: increase step size
           < factinsens/npar*ftol*(fabs(w.y[j])+fabs(w.y[0])) )
      {                                              // try with minimum
         w.p[j][j-1] = minpar[j-1]
         + factbound*(maxpar[j-1]-minpar[j-1]);
         numinfo.nfun++;
         status = f(w.p[j],npar,&(w.y[j]));
         if ( status == INTERRUPT ) return status;
         if ( status == PROBLEM ||
              2.0*fabs(w.y[j]-w.y[0]) 
              < factinsens/npar*ftol*(fabs(w.y[j])+fabs(w.y[0])) )
         {                                           // try with maximum
            w.p[j][j-1] = maxpar[j-1]
            - factbound*(maxpar[j-1]-minpar[j-1]);
            numinfo.nfun++;
            status = f(w.p[j],npar,&(w.y[j]));
            if ( status != OK ) return status;
            if ( 2.0*fabs(w.y[j]-w.y[0]) 
                 < factinsens/npar*ftol*(fabs(w.y[j])+fabs(w.y[0])) )
            {                                           
               *cjob << "\n   *** Solution not enough sensitive to this parameter";
               notsens = TRUE;
            }
         }
      }
   }
   if ( notsens == TRUE )
   {
      *cjob << "\n   *** Solution not enough sensitive to some parameter(s)";
      return PROBLEM;
   }

   CARDINAL icalc = npar;
   for ( j=0; j<npar+1; j++ ) trans_forw(w.p[j],minpar,maxpar,npar,transform);

   CARDINAL low, hi, nhi;                  // start main iteration loop
   while (1)
   {
      low = 0;                             // determine index of highest
      if ( w.y[0] > w.y[1] )               // (hi), nexthighest (nhi) and
      {                                    // lowest (low) value of y
         hi = 0; nhi = 1;
      }
      else
      {
         hi = 1; nhi = 0;
      }
      for ( j=0; j<npar+1; j++ )
      {
         if ( w.y[j] < w.y[low] ) low = j;
         if ( w.y[j] > w.y[hi] )
         {
            nhi = hi; hi = j;
         }
         else
         {
            if ( w.y[j] > w.y[nhi] )
            {
               if ( j != hi ) nhi = j;
            }
         }
      }

      for ( i=0; i<npar; i++ ) par[i] = w.p[low][i]; // update best solution
      trans_back(par,minpar,maxpar,npar,transform);
      *fval = w.y[low];

      if ( 2.0*fabs(w.y[hi]-w.y[low]) < ftol*(fabs(w.y[hi])+fabs(w.y[low])) 
            && numinfo.nstep>=minsteps ) // check stop conditions
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n ----convergence criterion met---"
                  << "-----------------------------------";
         }
         status = OK;
         if ( icalc != low )
         {
            numinfo.nfun++;
            status = f(par,npar,&(w.y[0]));  // reevaluate best solution
         }
         return status;
      }
      if ( numinfo.NumStep() >= maxiter )
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n ----maximum number of steps exeeded"
                  << "--------------------------------";
         }
         status = PROBLEM;
         if ( icalc != low )
         {
            numinfo.nfun++;
            status = f(par,npar,&(w.y[0]));  // reevaluate best solution
         }
         if ( status == OK ) return PROBLEM;
         return status;
      }
      if ( writelog == TRUE )
      {
         *cjob << "\n ----start of iteration step-----"
               << "-----------------------------------";
         *cjob << "\n     sets of parameters:";
         for ( i=0; i<npar+1; i++ )
         {
            *cjob << "\n       " << i+1 << ":";
            for ( j=0; j<npar; j++ ) *cjob << " p" << j+1 << "=" << w.p[i][j];
            *cjob << " f=" << w.y[i];
         }
         cjob->flush();
      }

      for ( i=0; i<npar; i++ ) w.pbar[i] = 0.0;  // compute average of all
      for ( j=0; j<npar+1; j++ )           // points except the highest
      {                                    // (pbar) and and reflected point
         if ( j != hi )                    // of the highest (pr)
         {
            for ( i=0; i<npar; i++ ) w.pbar[i] += w.p[j][i];
         }
      }
      for ( i=0; i<npar; i++ )
      {
         w.pbar[i] = w.pbar[i]/npar;
         w.pr[i] = (1.0+alpha)*w.pbar[i] - alpha*w.p[hi][i];
      }

      REAL ypr;                            // compute function at pr
      numinfo.nfun++;
      status = fun(w.pr,minpar,maxpar,npar,f,&ypr,w.y[hi],transform);
      if ( status == INTERRUPT ) return status;
      if ( writelog == TRUE )
      {
         *cjob << "\n     reflected point:";
         *cjob << "\n       new:";
         for ( j=0; j<npar; j++ ) *cjob << " p" << j+1 << "=" << w.pr[j];
         *cjob << " f=" << ypr;
         cjob->flush();
      }
      icalc = npar+1;
      if ( status == OK && ypr <= w.y[low] ) 
      {                                    // if better than lowest point
         for ( i=0; i<npar; i++ )          // try further extrapolation
         {                                 // (prr)
            w.prr[i] = gamma*w.pr[i] + (1.0-gamma)*w.pbar[i];
         }
         REAL yprr;                        // calculate function at prr
         numinfo.nfun++;
         status = fun(w.prr,minpar,maxpar,npar,f,&yprr,w.y[hi],transform);
         if ( status == INTERRUPT ) return status;
         if ( writelog == TRUE )
         {
            *cjob << "\n     reflected and expanded point:";
            *cjob << "\n       new:";
            for ( j=0; j<npar; j++ ) *cjob << " p" << j+1 << "=" << w.prr[j];
            *cjob << " f=" << yprr;
            cjob->flush();
         }
         if ( status == OK && yprr < w.y[low] ) // if better than lowest
         {                                      // point accept prr
            for ( i=0; i<npar; i++ ) w.p[hi][i] = w.prr[i];
            w.y[hi] = yprr;
            icalc =hi;
            if ( writelog == TRUE )
            {
               *cjob << "\n     reflected and expanded point accepted";
               cjob->flush();
            }
         }
         else                              // else accept pr
         {
            for ( i=0; i<npar; i++ ) w.p[hi][i] = w.pr[i];
            w.y[hi] = ypr;
            if ( writelog == TRUE )
            {
               *cjob << "\n     reflected point accepted";
               cjob->flush();
            }
         }
      }
      else                                  // pr is worse than lowest point or status != OK
      {
         if ( status!=OK || ypr>=w.y[nhi] ) // pr is worse than second-highest
         {
            if ( status==OK && ypr<w.y[hi] )// if pr is better than highest
            {                               // point accept it
               for ( i=0; i<npar; i++ ) w.p[hi][i] = w.pr[i];
               w.y[hi] = ypr;
               icalc = hi;
            }
            for ( i=0; i<npar; i++ )       // try intermediate point (prr)
            {
               w.prr[i] = beta*w.p[hi][i] + (1.0-beta)*w.pbar[i];
            }
            REAL yprr;                     // calculate function at prr
            numinfo.nfun++;
            status = fun(w.prr,minpar,maxpar,npar,f,&yprr,w.y[hi],transform);
            if ( status == INTERRUPT ) return status;
            if ( writelog == TRUE )
            {
               *cjob << "\n     contracted point:";
               *cjob << "\n       new:";
               for ( j=0; j<npar; j++ ) *cjob << " p" << j+1 << "=" << w.prr[j];
               *cjob << " f=" << yprr;
               cjob->flush();
            }
            if ( status==OK && yprr < w.y[hi] ) // if prr better than 
            {                              // highest point accept it
               for ( i=0; i<npar; i++ ) w.p[hi][i] = w.prr[i];
               w.y[hi] = yprr;
               icalc = hi;
               if ( writelog == TRUE )
               {
                  *cjob << "\n     contracted point accepted";
                  cjob->flush();
               }
            }
            else                           // else contract around the
            {                              // lowest point
               if ( writelog == TRUE )
               {
                  *cjob << "\n     contract whole simplex"
                        << " towards lowest point";
                  cjob->flush();
               }
               for ( j=0; j<npar+1; j++ )
               {
                  if ( j != low )
                  {
                     for ( i=0; i<npar; i++ )
                     {
                        w.p[j][i] = 0.5*(w.p[j][i]+w.p[low][i]);
                     }
                     numinfo.nfun++;
                     status = fun(w.p[j],minpar,maxpar,npar,f,
                                  &(w.y[j]),w.y[hi],transform);
                     if ( status == INTERRUPT ) return status;
                     if ( status == PROBLEM ) // if PROBLEM contract
                     {                        // once more
                        for ( i=0; i<npar; i++ )
                        {
                           w.p[j][i] = 0.5*(w.p[j][i]+w.p[low][i]);
                        }
                        numinfo.nfun++;
                        status = fun(w.p[j],minpar,maxpar,npar,f,
                                     &(w.y[j]),w.y[hi],transform);
                        if ( status == INTERRUPT ) return status;
                        if ( status == PROBLEM ) // if PROBLEM contract
                        {                        // once more
                           for ( i=0; i<npar; i++ )
                           {
                              w.p[j][i] = 0.5*(w.p[j][i]+w.p[low][i]);
                           }
                           numinfo.nfun++;
                           status = fun(w.p[j],minpar,maxpar,npar,f,
                                        &(w.y[j]),w.y[hi],transform);
                           if ( status != OK ) return status;
                        }
                     }
                     icalc = j;
                  }
               }
            }
         }
         else                              // pr is an intermediate point,
         {                                 // accept it
            for ( i=0; i<npar; i++ ) w.p[hi][i] = w.pr[i];
            w.y[hi] = ypr;
            icalc = hi;
            if ( writelog == TRUE )
            {
               *cjob << "\n     reflected point accepted";
               cjob->flush();
            }
         }
      }
      numinfo.nstep++;                     // update counter
   }
}

//////////////////////////////////////////////////////////////////////////////

static const REAL* glob_fval;
static const REAL* glob_fsig;
static CARDINAL    glob_nf;
static FITFUN      glob_f;

static JOBSTATUS chi2calc(const REAL par[], CARDINAL numpar, REAL* chi2)
{
   REAL* fval = new REAL[glob_nf];
   JOBSTATUS status = glob_f(par,numpar,fval,glob_nf);
   if ( status == OK )
   {
      *chi2 = 0.0;
      for ( CARDINAL i=0; i<glob_nf; i++ )
      {
         *chi2 += (glob_fval[i]-fval[i])*(glob_fval[i]-fval[i])
                  /(glob_fsig[i]*glob_fsig[i]);
      }
   }
   delete [] fval;
   return status;
}


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
                     BOOLEAN    writelog)
{
   numinfo.Clear();

   if ( npar >= nf ) return PROBLEM;

   for ( CARDINAL i=0; i<nf; i++ ) if ( !(fsig[i] > 0.0) ) return PROBLEM;

   glob_fval = fval;
   glob_fsig = fsig;
   glob_nf   = nf;
   glob_f    = f;

   return MinSimplex(par,minpar,maxpar,npar,chi2calc,
                     ftol,maxiter,chi2,numinfo,writelog,TRUE);
}

//////////////////////////////////////////////////////////////////////////////

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
                     BOOLEAN    writelog,
                     BOOLEAN*   covcalc,
                     REAL*      cov)
{
   const REAL     factini   = 0.01;       // assign parameters of algorithm
   const REAL     amin      = 0.1;
   const REAL     maxstep   = 2.0;
   const REAL     maxspread = 2.0;
   const CARDINAL maxcount  = 50;
   const CARDINAL minsteps  = 3;

   CARDINAL i,j,k;                        // declare global variables
   JOBSTATUS status;
   FIT_WORK w(npar,nf);
   numinfo.Clear();
   if ( covcalc != 0 ) *covcalc = FALSE;
   INTEGER info = 0;                   

   if ( npar >= nf ) return PROBLEM;      // check consistency of input
/*
   if ( npar == 0 )  return PROBLEM;
*/
   for ( i=0; i<nf; i++ )
   {
      if ( !( fsig[i] > 0.0 ) ) return PROBLEM;
   }
   for ( i=0; i<npar; i++ )
   {
      if ( minpar[i] >= maxpar[i] ) return PROBLEM;
      if ( par[i] <= minpar[i] ) par[i] = minpar[i];
      if ( par[i] >= maxpar[i] ) par[i] = maxpar[i];
   }

   // initialization:
   // ---------------

   if ( writelog == TRUE )
   {
      *cjob << "\n ----initialization--------------"
            << "-----------------------------------";
   }
   for ( j=0; j<npar+1; j++ )             // initialize parameter arrays
   {
      for ( i=0; i<npar; i++ ) w.p[j][i] = par[i];
   }
   for ( i=0; i<npar; i++ )
   {
      w.p[i+1][i] = par[i] + factini*(maxpar[i]-minpar[i]);
      if ( w.p[i+1][i] >= maxpar[i] )
         w.p[i+1][i] = par[i] - factini*(maxpar[i]-minpar[i]);
   }

   REAL chinew = 0.0;
   BOOLEAN notsens = FALSE;
   for ( j=0; j<npar+1; j++ )             // initialize function arrays
   {
      numinfo.nfun++;
      status = f(w.p[j],npar,w.f[j],nf);
      if ( status == OK )
      {
         w.chi[j] = 0.0;
         for ( i=0; i<nf; i++ )
         {
            w.chi[j] += (fval[i]-w.f[j][i])*(fval[i]-w.f[j][i])
                        / (fsig[i]*fsig[i]);
         }
      }
      else
      {
         if ( status == INTERRUPT ) return status;
      }
      if ( j == 0 )
      {
         if ( status == PROBLEM ) return status;
         *chi2  = w.chi[j];
         chinew = w.chi[j];
         if ( maxiter==0 || npar==0 ) return OK;
      }
      else
      {
         if ( status == PROBLEM ||
              2.0*fabs(w.chi[j]-w.chi[0]) 
              < factinsens/npar*ftol*fabs(w.chi[j]+w.chi[0]) )
         {                                               // increase increment
            w.p[j][j-1] = par[j-1] + sqrt(factini)*(maxpar[j-1]-minpar[j-1]);
            if ( w.p[j][j-1] >= maxpar[j-1] )
               w.p[j][j-1] = par[j-1] - sqrt(factini)*(maxpar[j-1]-minpar[j-1]);
            numinfo.nfun++;
            status = f(w.p[j],npar,w.f[j],nf);
            if ( status == INTERRUPT ) return status;
            if ( status == OK )
            {
               w.chi[j] = 0.0;
               for ( i=0; i<nf; i++ )
               {
                  w.chi[j] += (fval[i]-w.f[j][i])*(fval[i]-w.f[j][i])
                              / (fsig[i]*fsig[i]);
               }
            }
            if ( status == PROBLEM ||
                 2.0*fabs(w.chi[j]-w.chi[0]) 
                 < factinsens/npar*ftol*fabs(w.chi[j]+w.chi[0]) )
            {                                            // try with minimum
               w.p[j][j-1] = minpar[j-1];
               numinfo.nfun++;
               status = f(w.p[j],npar,w.f[j],nf);
               if ( status == INTERRUPT ) return status;
               if ( status == OK )
               {
                  w.chi[j] = 0.0;
                  for ( i=0; i<nf; i++ )
                  {
                     w.chi[j] += (fval[i]-w.f[j][i])*(fval[i]-w.f[j][i])
                                 / (fsig[i]*fsig[i]);
                  }
               }
               if ( status == PROBLEM ||
                    2.0*fabs(w.chi[j]-w.chi[0]) 
                    < factinsens/npar*ftol*fabs(w.chi[j]+w.chi[0]) )
               {                                         // try with maximum
                  w.p[j][j-1] = maxpar[j-1];
                  numinfo.nfun++;
                  status = f(w.p[j],npar,w.f[j],nf);
                  if ( status != OK ) return status;
                  w.chi[j] = 0.0;
                  for ( i=0; i<nf; i++ )
                  {
                     w.chi[j] += (fval[i]-w.f[j][i])*(fval[i]-w.f[j][i])
                                 / (fsig[i]*fsig[i]);
                  }
                  if ( 2.0*fabs(w.chi[j]-w.chi[0]) 
                       < factinsens/npar*ftol*fabs(w.chi[j]+w.chi[0]) )
                  {
                     *cjob << "\n   *** Solution not enough sensitive to this parameter";
         	         notsens = TRUE;
                  }
               }
            }
         }
         if ( w.chi[j] > chinew ) chinew = w.chi[j];
         if ( w.chi[j] < *chi2  ) *chi2  = w.chi[j];
      }
   }
   if ( notsens == TRUE )
   {
      *cjob << "\n   *** Solution not enough sensitive to some parameter(s)";
      return PROBLEM;
   }

   // main iteration loop:
   // --------------------

   REAL     chiref = *chi2;    // best chi2 from last step
   CARDINAL icalc = npar;      // index of last function evaluation
   CARDINAL nb = 0;            // number of active bounds
   BOOLEAN  nearlow = FALSE;   // indicator for new sol. near to low sol.
   CARDINAL largestep = 0;     // counter for reduced step size
   CARDINAL ftol_ok = 0;       // counter for stop criterion
   while ( 1 )
   {
      // reorder solutions and determine largest chi^2:
      // ----------------------------------------------

      k = 0;                        // change best solution to last position
      for ( i=1; i<npar+1; i++ )
      {
         if ( w.chi[i] < w.chi[k]  ) k = i;
      }
      if ( k != npar )
      {
         REAL r;
         for ( i=0; i<npar; i++ )
         {
            r = w.p[k][i]; w.p[k][i] = w.p[npar][i]; w.p[npar][i] = r;
         }
         for ( i=0; i<nf; i++ )
         {
            r = w.f[k][i]; w.f[k][i] = w.f[npar][i]; w.f[npar][i] = r;
         }
         r = w.chi[k]; w.chi[k] = w.chi[npar]; w.chi[npar] = r;
         if ( k == icalc )
         {
            icalc = npar;
         }
         else
         {
            if ( icalc == npar ) icalc = k;
         }
      }

      CARDINAL hi = 0;                // determine largest chi^2
      for ( i=1; i<npar+1; i++ )
      {
         if ( w.chi[i] > w.chi[hi] ) hi = i;
      }

      *chi2 = w.chi[npar];             // update best solution
      for ( i=0; i<npar; i++ ) par[i] = w.p[npar][i];
      if ( nearlow == TRUE )
      {
         ftol_ok++;
      }
      else
      {
         if ( 2.0*fabs(chinew-chiref) < ftol*fabs(chinew+chiref) )
         {
            ftol_ok++;
         }
         else
         {
            ftol_ok = 0;
         }
      }
	  chiref = w.chi[npar];

      // check stop criteria:
      // --------------------

      if ( ftol_ok>convcrit && numinfo.nstep>=minsteps )
      {
         if ( w.chi[hi]/(*chi2) > maxspread )   // spread of solution set too large
         {
            // contract high solution:
            // -----------------------

            if ( writelog == TRUE )
            {
               *cjob << "\n     spread of solution set too large,"
                     << " contraction of high solution";
               cjob->flush();
            }
            for ( i=0; i<npar; i++ )
            {
               w.p[hi][i] = 0.1*(w.p[hi][i]+9.0*w.p[npar][i]);
            }

            // calculate new chi^2's:
            // ----------------------

            numinfo.nfun++;
            status = f(w.p[hi],npar,w.f[hi],nf);
            if ( status == INTERRUPT ) return status;
            if ( status == PROBLEM ) // if PROBLEM contract again
            {
               for ( i=0; i<npar; i++ )
               {
                  w.p[hi][i] = 0.1*(w.p[hi][i]+9.0*w.p[npar][i]);
               }
               numinfo.nfun++;
               status = f(w.p[hi],npar,w.f[hi],nf);
			   if ( status == INTERRUPT ) return status;
               if ( status == PROBLEM ) // if PROBLEM contract again
               {
                  for ( i=0; i<npar; i++ )
                  {
                     w.p[hi][i] = 0.1*(w.p[hi][i]+9.0*w.p[npar][i]);
                  }
                  numinfo.nfun++;
                  status = f(w.p[hi],npar,w.f[hi],nf);
                  if ( status != OK ) return status;
               }
            }
            w.chi[hi] = 0.0;
            for ( i=0; i<nf; i++ )
            {
               w.chi[hi] += (fval[i]-w.f[hi][i])*(fval[i]-w.f[hi][i])
                            / (fsig[i]*fsig[i]);
            }
            icalc = hi;
         }
         else                            // convergence criterion met
         {
            if ( writelog == TRUE )
            {
               *cjob << "\n ----convergence criterion met---"
                     << "-----------------------------------";
            }
            status = OK;
            if ( icalc != npar )
            {
               numinfo.nfun++;
               status = f(par,npar,w.f[npar],nf);  // reevaluate best solution
            }
            if ( covcalc!=0 && cov!=0 && nb==0 ) // calculate covariance matrix
            {
               for ( i=0; i<npar; i++ )    // calculate matrix m = DF'*DF
               {
                  for ( j=0; j<npar; j++ )
                  {
                     cov[i+npar*j] = 0.0;
                     for ( k=0; k<nf; k++ )
                     {
                        cov[i+npar*j] +=  (w.f[i][k]-w.f[npar][k])
                                         *(w.f[j][k]-w.f[npar][k])
                                         / (fsig[k]*fsig[k]);
                     }
                  }
               }
               dgefa(cov,npar,npar,w.ipvt,&info);
               for ( j=0; j<npar; j++ )       // calculate (DF'*DF)^(-1)*DP'
               {
                  for ( i=0; i<npar; i++ )
                  {
                     w.m[i+npar*j] = w.p[i][j]-w.p[npar][j];
                  }
                  dgesl(cov,npar,npar,w.ipvt,&w.m[npar*j],0);
               }
               for ( i=0; i<npar; i++ )      // calculate DP*(DF'*DF)^(-1)*DP'
               {
                  for ( j=0; j<npar; j++ )
                  {
                     cov[i+npar*j] = 0.0;
                     for ( k=0; k<npar; k++ )
                     {
                        cov[i+npar*j] += (w.p[k][i]-w.p[npar][i])*w.m[k+npar*j];
                     }
                     cov[i+npar*j] *= (*chi2)/((REAL)(nf-npar));
                  }
               }
               *covcalc = TRUE;
            }
            return status;
         }
      }
      if ( numinfo.nstep >= maxiter )
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n ----maximum number of steps exeeded"
                  << "--------------------------------";
         }
         status = PROBLEM;
         if ( icalc != npar )
         {
            numinfo.nfun++;
            status = f(par,npar,w.f[npar],nf);  // reevaluate best solution
         }
         if ( status == OK ) return PROBLEM;
         return status;
      }
      if ( writelog == TRUE )
      {
         *cjob << "\n ----start of iteration step-----"
               << "-----------------------------------";
         *cjob << "\n     sets of parameters:";
         for ( i=0; i<npar+1; i++ )
         {
            *cjob << "\n       " << i+1 << ":";
            for ( j=0; j<npar; j++ ) *cjob << " p" << j+1 << "=" << w.p[i][j];
            *cjob << " chi^2=" << w.chi[i];
         }
         cjob->flush();
      }

      // loop for exactly solving linearized inequality constrained problem:
      // -------------------------------------------------------------------

      CARDINAL nbold = 0; REAL Qold = 0.0; CARDINAL count = 0;
      while ( 1 )
      {
         // exactly solve linearized equality constrained problem:
         // ------------------------------------------------------

         if ( writelog == TRUE )
         {
            *cjob << "\n     active set of constraints:";
            if ( nb == 0 )
            {
               *cjob << " -";
            }
            else
            {
               for ( i=0; i<nb; i++ )
               {
                  *cjob << " " << w.ind[i]+1;
                  if ( w.low[i] == TRUE ) *cjob << "dn";
                  else                    *cjob << "up";
               }
            }
            cjob->flush();
         }
                                      //                      ( DF'*DF  DP )
         for ( i=0; i<npar; i++ )     // calculate matrix M = (            )
         {                            //                      (  DP'    0  )
            for ( j=0; j<npar; j++ )
            {
               w.m[i+(npar+nb)*j] = 0.0;
               for ( k=0; k<nf; k++ )    // DF`*DF:
               {
                  w.m[i+(npar+nb)*j] +=  (w.f[i][k]-w.f[npar][k])
                                         *(w.f[j][k]-w.f[npar][k])
                                        / (fsig[k]*fsig[k]);
               }
            }
         }
         for ( i=0; i<npar; i++ )
         {
            for ( j=0; j<nb; j++ )       // DP, DP':
            {
               w.m[i+(npar+nb)*(npar+j)] = w.p[i][w.ind[j]]
                                           -w.p[npar][w.ind[j]];
               w.m[npar+j+(npar+nb)*i]   = w.m[i+(npar+nb)*(npar+j)];
            }
         }
         for ( i=0; i<nb; i++ )
         {
            for ( j=0; j<nb; j++ )       // 0:
            {
               w.m[npar+i+(npar+nb)*(npar+j)] = 0.0;
            }
         }
                                      //               ( DF'*(y-f(m+1)) )
         for ( i=0; i<npar; i++ )     // calculate B = (                )
         {                            //               (    p - p(m+1)  )
            w.a[i] = 0.0;
            for ( k=0; k<nf; k++ )    // DF'*(y-f(n+1)):
            {
               w.a[i] += (w.f[i][k]-w.f[npar][k])*(fval[k]-w.f[npar][k])
                         / (fsig[k]*fsig[k]);
            }
         }
         for ( i=0; i<nb; i++ )       // p-p(m+1):
         {
            if ( w.low[i] == TRUE )
            {
               w.a[npar+i] = minpar[w.ind[i]]-w.p[npar][w.ind[i]];
            }
            else
            {
               w.a[npar+i] = maxpar[w.ind[i]]-w.p[npar][w.ind[i]];
            }
         }

         dgefa(w.m,npar+nb,npar+nb,w.ipvt,&info);    // solve M*a = B
         dgesl(w.m,npar+nb,npar+nb,w.ipvt,w.a,0);

         for ( i=0; i<npar; i++ )            // calculate new solution
         {
            w.pnew[i] = w.p[npar][i];
            for ( j=0; j<npar; j++ )
            {
               w.pnew[i] += (w.p[j][i]-w.p[npar][i])*w.a[j];
            }
         }
         if ( writelog == TRUE )
         {
            *cjob << "\n     proposed solution:";
            *cjob << "\n       new:";
            for ( i=0; i<npar; i++ )
            {
               *cjob << " p" << i+1 << "=" << w.pnew[i];
            }
            cjob->flush();
         }
         for ( i=0; i<nb; i++ )           // eliminate rounding errors
         {
            if ( w.low[i] == TRUE ) w.pnew[w.ind[i]] = minpar[w.ind[i]];
            else                    w.pnew[w.ind[i]] = maxpar[w.ind[i]];
         }

         // add possibly needed constraints to active set:
         // ----------------------------------------------

         BOOLEAN bound_added = FALSE;
         for ( i=0; i<npar; i++ )
         {
            if ( w.pnew[i]<minpar[i] || w.pnew[i]>maxpar[i] )
            {
               BOOLEAN active = FALSE; k = 0;
               for ( j=0; j<nb; j++ )
               {
                  if ( w.ind[j] == i )
                  {
                     active = TRUE; break;
                  }
                  else
                  {
                     if ( w.ind[j] < i ) k = j+1;
                  }
               }
               if ( active == FALSE )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n     add constraint " << i+1;
                     if ( w.pnew[i] < minpar[i] ) *cjob << "dn";
                     else                         *cjob << "up";
                     cjob->flush();
                  }
                  for ( j=0; j<nb-k; j++ )
                  {
                     w.ind[nb-j] = w.ind[nb-j-1];
                     w.low[nb-j] = w.low[nb-j-1];
                  }
                  w.ind[k] = i;
                  if ( w.pnew[i] < minpar[i] ) w.low[k] = TRUE;
                  else                         w.low[k] = FALSE;
                  nb++;
                  bound_added = TRUE;
               }
            }
         }

         if ( nb == 0 ) break; // unconstrained linear problem solved
         if ( count >= maxcount )
         {
            if ( bound_added == TRUE )
            {
               *cjob << "\n   *** FitSecant: solution to linearized "
                     << "problem not found";
               cjob->flush();
               return PROBLEM;
            }
            break;    // accept best solution found so far
         }

         // remove unnecessary constraints from active set:
         // -----------------------------------------------

         if ( bound_added == FALSE )
         {
            if ( nb == nbold )
            {
               BOOLEAN equal = TRUE;
               for ( i=0; i<nb; i++ )
               {
                  if ( w.ind[i]!=w.indold[i] || w.low[i]!=w.lowold[i] )
                  {
                     equal = FALSE; break;
                  }
               }
               if ( equal == TRUE ) break; // solution accepted
            }
            for ( i=0; i<nf; i++ )
            {
               w.fnew[i] = w.f[npar][i];
               for ( j=0; j<npar; j++ )
               {
                  w.fnew[i] += (w.f[j][i]-w.f[npar][i])*w.a[j];
               }
            }
            REAL Q = 0.0;
            for ( i=0; i<nf; i++ )
            {
               Q += (fval[i]-w.fnew[i])*(fval[i]-w.fnew[i])
                    / (fsig[i]*fsig[i]);
            }
            if ( writelog == TRUE )
            {
               *cjob << " Q=" << Q;
               cjob->flush();
            }
            if ( nbold==0 || Q<Qold )
            {
               nbold = nb;
               for ( i=0; i<nb; i++ )
               {
                  w.indold[i] = w.ind[i];
                  w.lowold[i] = w.low[i];
               }
               Qold = Q;
            }
            BOOLEAN bound_removed = FALSE;
            for ( i=0; i<nb; i++ )
            {
               if ( (w.low[i]==TRUE && w.a[npar+i]>0.0) ||
                    (w.low[i]!=TRUE && w.a[npar+i]<0.0)    )
               {                      // remove constraint
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n     constraint " << w.ind[i]+1;
                     if ( w.low[i] == TRUE ) *cjob << "dn";
                     else                    *cjob << "up";
                     *cjob << " removed";
                     cjob->flush();
                  }
                  for ( j=i+1; j<nb; j++ )
                  {
                     w.ind[j-1] = w.ind[j];
                     w.low[j-1] = w.low[j];
                  }
                  nb--;
                  bound_removed = TRUE;
                  break;
               }
            }
            if ( bound_removed == FALSE ) break; // constrained linear 
         }                                       // problem solved
         count++;
         if ( count >= maxcount ) // recalculate best solution found so far
         {
            nb = nbold;
            for ( i=0; i<nb; i++ )
            {
               w.ind[i] = w.indold[i];
               w.low[i] = w.lowold[i];
            }
         }
      }
      if ( writelog == TRUE )
      {
         *cjob << "\n     solution to linear problem accepted";
         *cjob << "\n      ";
         for ( i=0; i<npar; i++ ) *cjob << " a" << i+1 << "=" << w.a[i];
         cjob->flush();
      }

      // check distance to low solution and step size:
      // ---------------------------------------------

      nearlow = TRUE;       // check distance to low solution
      for ( i=0; i<npar; i++ )
      {
         if ( fabs(w.a[i]) > amin )
         {
            nearlow = FALSE; break;
         }
      }
      if ( nearlow == TRUE )  // solution too near to best solution
      {
         // contract solution set:
         // ----------------------

         largestep = 0;
         if ( writelog == TRUE )
         {
            *cjob << "\n     solution near to best solution,"
                  << " contraction of solution set";
            cjob->flush();
         }
         for ( j=0; j<npar; j++ )
         {
            for ( i=0; i<npar; i++ )
            {
               w.p[j][i] = 0.1*(w.p[j][i]+9.0*w.p[npar][i]);
            }
         }

         // calculate new chi^2's:
         // ----------------------

         for ( j=0; j<npar; j++ )
         {
            numinfo.nfun++;
            status = f(w.p[j],npar,w.f[j],nf);
            if ( status == INTERRUPT ) return status;
            if ( status == PROBLEM ) // if PROBLEM contract again
            {
               for ( i=0; i<npar; i++ )
               {
                  w.p[j][i] = 0.5*(w.p[j][i]+w.p[npar][i]);
               }
               numinfo.nfun++;
               status = f(w.p[j],npar,w.f[j],nf);
               if ( status == INTERRUPT ) return status;
               if ( status == PROBLEM ) // if PROBLEM contract again
               {
                  for ( i=0; i<npar; i++ )
                  {
                     w.p[j][i] = 0.5*(w.p[j][i]+w.p[npar][i]);
                  }
                  numinfo.nfun++;
                  status = f(w.p[j],npar,w.f[j],nf);
                  if ( status != OK ) return status;
               }
            }
            w.chi[j] = 0.0;
            for ( i=0; i<nf; i++ )
            {
               w.chi[j] += (fval[i]-w.f[j][i])*(fval[i]-w.f[j][i])
                           / (fsig[i]*fsig[i]);
            }
         }
         icalc = npar-1;
      }
      else                   // solution different from low solution
      {
         // check step size:
         // ----------------

         REAL step = 0.0;
         for ( i=0; i<npar; i++ )
         {
            REAL stepi = 0.0;
            for ( j=0; j<npar; j++ )
            {
               if ( fabs(w.p[j][i]-w.p[npar][i]) > stepi )
                  stepi = fabs(w.p[j][i]-w.p[npar][i]);
            }
            if ( fabs(w.pnew[i]-w.p[npar][i])/stepi > step )
               step = fabs(w.pnew[i]-w.p[npar][i])/stepi;
         }
         if ( step > maxstep )
         {
            largestep++;
            for ( i=0; i<npar; i++ )
            {
               w.pnew[i] = w.p[npar][i]
                           + maxstep/step*(w.pnew[i]-w.p[npar][i]);
            }
            if ( writelog == TRUE )
            {
               *cjob << "\n     step size too large, step size reduced;"
                     << " proposed solution:";
               *cjob << "\n       new:";
               for ( i=0; i<npar; i++ )
               {
                  *cjob << " p" << i+1 << "=" << w.pnew[i];
               }
               cjob->flush();
            }
         }
         else
         {
            largestep = 0;
         }

         // calculate chi^2:
         // ----------------

         numinfo.nfun++;
         status = f(w.pnew,npar,w.fnew,nf);
         if ( status == INTERRUPT ) return status;
         if ( status == PROBLEM ) // if PROBLEM contract to low solution
         {
            for ( i=0; i<npar; i++ )
            {
               w.pnew[i] = 0.5*(w.pnew[i]+w.p[npar][i]);
            }
            numinfo.nfun++;
            status = f(w.pnew,npar,w.fnew,nf);
            if ( status != OK ) return status;
         }
         chinew = 0.0;
         for ( i=0; i<nf; i++ )
         {
            chinew += (fval[i]-w.fnew[i])*(fval[i]-w.fnew[i])
                      / (fsig[i]*fsig[i]);
         }

         // replace one parameter set by new solution:
         // ------------------------------------------

         for ( i=0; i<npar; i++ )   // recalculate a-values of modified solution
         {
            w.a[i] = w.pnew[i]-w.p[npar][i];
            for ( j=0; j<npar; j++ )
            {
               w.m[i+npar*j] = w.p[j][i]-w.p[npar][i];
            }
         }
         dgefa(w.m,npar,npar,w.ipvt,&info);
         dgesl(w.m,npar,npar,w.ipvt,w.a,0);

         icalc = npar+1;             // (correct after shift at the end)
         if ( fabs(w.a[0]) < amin )  // do not loose important dimension
         {                           // (replace set [0] if possible)
            k = 1;
            for ( i=2; i<npar; i++ )
            {
               if ( fabs(w.a[k]) > fabs(w.a[k]) ) k = i;
            }
            if ( writelog == TRUE )
            {
               *cjob << "\n     dimension problem; additional point replaced";
               cjob->flush();
            }
            for ( i=0; i<npar; i++ ) w.p[k][i] = 0.5*(w.p[0][i]+w.pnew[i]);
            numinfo.nfun++;
            status = f(w.p[k],npar,w.f[k],nf);
            if ( status == INTERRUPT ) return status;
            if ( status == PROBLEM ) // if PROBLEM contract	to low solution
            {
               for ( i=0; i<npar; i++ )
               {
                  w.p[k][i] = 0.5*(w.p[k][i]+w.p[npar][i]);
               }
               numinfo.nfun++;
               status = f(w.p[k],npar,w.f[k],nf);
               if ( status != OK ) return status;
            }
            w.chi[k] = 0.0;
            for ( i=0; i<nf; i++ )
            {
               w.chi[k] += (fval[i]-w.f[k][i])*(fval[i]-w.f[k][i])
                           / (fsig[i]*fsig[i]);
            }
            icalc = k;
         }

         for ( j=0; j<npar; j++ )      // shift solutions
         {
            for ( i=0; i<npar; i++ ) w.p[j][i] = w.p[j+1][i];
            for ( i=0; i<nf;   i++ ) w.f[j][i] = w.f[j+1][i];
            w.chi[j] = w.chi[j+1];
         }
         icalc--;
         for ( i=0; i<npar; i++ ) w.p[npar][i] = w.pnew[i];  // replace solution
         for ( i=0; i<nf;   i++ ) w.f[npar][i] = w.fnew[i];
         w.chi[npar] = chinew;
      }
      numinfo.nstep++;
   }
}

//////////////////////////////////////////////////////////////////////////////

