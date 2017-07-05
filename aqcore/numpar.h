////////////////////////////////   numpar.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    25.11.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#ifndef NUMPAR_H
#define NUMPAR_H

#ifndef USE_TYPES_H
#include "all.h"
#include "init.h"
#include "fileio.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////

// Type of equations

enum EQTYPE    {
                  DIFF,   //  differential or differential-algebraic equation
                  ALG     //  pure algebraic equation
               };

//////////////////////////////////////////////////////////////////////////////

// Type of function to define system of nonlinear equations:
//
// JOBSTATUS zero(const REAL x[], CARDINAL nx, REAL delta[])
//
// zero uses the array x to calculate the deviations delta for all nx 
// equations

typedef JOBSTATUS (*ZEROFUN)(const REAL[], CARDINAL, REAL[]);

//////////////////////////////////////////////////////////////////////////////

// Type of function to define a differential algebraic system:
//
// JOBSTATUS das(const REAL& t, const REAL* y, const REAL* yt, REAL* delta)
//
// das uses t and the arrays y and yt to calculate the deviations from zero
// of the differential algebraic system delta(t,y,yt) = 0

typedef JOBSTATUS (*DASFUN)(const REAL&, const REAL*,
                            const REAL*, REAL* delta);

//////////////////////////////////////////////////////////////////////////////

// Type of function to be minimized in a fit problem
//
// JOBSTATUS minfun(const REAL par[], CARDINAL npar, REAL* fun)
//
// minfun uses then parameters par[0], ... , par[npar-1] to calculate the
// value fun to be minimized.

typedef JOBSTATUS (*MINFUN)(const REAL[], CARDINAL, REAL*);

//////////////////////////////////////////////////////////////////////////////

// Type of function for least square parameter estimation
//
// JOBSTATUS fitfun(const REAL par[], CARDINAL npar,
//                  REAL fun[], CARDINAL nfun)
//
// fitfun uses then parameters par[0], ... , par[npar-1] to calculate the
// values fun[0], ... , fun[nfun-1] to be compared with data.

typedef JOBSTATUS (*FITFUN)(const REAL[], CARDINAL, REAL[], CARDINAL);

//////////////////////////////////////////////////////////////////////////////

// Class for numeric parameters:

#ifndef USE_TYPES_H
class NUMPAR : public FILEIO
#else
class NUMPAR
#endif
{
   public:

      NUMPAR();

      BOOLEAN     DtMax(REAL val);            // maximum internal timestep
      REAL        DtMax() const
                     { return dtmax; }
      BOOLEAN     MaxOrd(CARDINAL val);       // maximum order
      CARDINAL    MaxOrd() const
                     { return maxord; }
      void        NumCoDiag(CARDINAL val)     // number of codiagonals of
                     { numcodiag = val; }     // jacobian to be considered
      CARDINAL    NumCoDiag() const
                     { return numcodiag; }
      BOOLEAN     MaxNumSteps(CARDINAL val);  // maximum number of steps
      CARDINAL    MaxNumSteps() const         // between output intervals
                     { return maxnumsteps; }
      REAL        GridTimeConst() const       // grid relaxation time constant
                     { return kgrid; }        // (for external use only)
      BOOLEAN     GridTimeConst(REAL gridrelax);
      void        Interpolate(BOOLEAN val)    // TRUE:  solution is inter-
                     { interpolate = val; }   //        polated at tout
      BOOLEAN     Interpolate() const         // FALSE: integration is per-
                     { return interpolate; }  //        formed exactly to tout
      REAL        FitTol() const              // tolerance for convergence of fit
                     { return fittol; }       // (for external use only)
      BOOLEAN     FitTol(REAL tol);

#ifndef USE_TYPES_H
	  JOBSTATUS   Load(std::istream& in);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);
#endif

   private:

      REAL        dtmax;
      CARDINAL    maxord;
      CARDINAL    numcodiag;
      CARDINAL    maxnumsteps;
      REAL        kgrid;
      BOOLEAN     interpolate;
      REAL        fittol;

#ifndef USE_TYPES_H
      static INIT ini;
#endif
};

//////////////////////////////////////////////////////////////////////////////

// Class for observation of the progress of the integration process:

class NUMINFO
{
      friend JOBSTATUS ZeroNewton(REAL[],const REAL[],const REAL[],CARDINAL,
                                  ZEROFUN,CARDINAL,REAL[],const NUMPAR&,
                                  NUMINFO&);
      friend JOBSTATUS DasInit   (DASFUN,REAL,REAL*,REAL*,const REAL*,
                                  const REAL*,const EQTYPE*, CARDINAL,
                                  BOOLEAN,const NUMPAR&, NUMINFO&);
      friend JOBSTATUS DasGear   (DASFUN,REAL&,REAL*,REAL*,REAL*,REAL*,
                                  CARDINAL,REAL,BOOLEAN&,std::ostream*,
                                  const NUMPAR&,NUMINFO&);
      friend JOBSTATUS MinSimplex(REAL[],const REAL[],const REAL[],CARDINAL,
                                  MINFUN,REAL,CARDINAL,REAL*,NUMINFO&,
                                  BOOLEAN,BOOLEAN);
      friend JOBSTATUS FitSimplex(REAL[],const REAL[],const REAL[],CARDINAL,
                                  const REAL[],const REAL[], CARDINAL,FITFUN,
                                  REAL,CARDINAL,CARDINAL,REAL*,NUMINFO&,BOOLEAN);
      friend JOBSTATUS FitSecant (REAL[],const REAL[],const REAL[],CARDINAL,
                                  const REAL[],const REAL[], CARDINAL,FITFUN,
                                  REAL,CARDINAL,CARDINAL,REAL*,NUMINFO&,BOOLEAN,
                                  BOOLEAN*,REAL*);

   public:

      NUMINFO();

      BOOLEAN  Write(std::ostream* out);

      REAL     ActT() const               // actual time
                  { return actt; }
      REAL     ActDt() const              // actual time step
                  { return actdt; }
      CARDINAL ActOrd() const             // actual order of integration
                  { return actord; }

      CARDINAL NumStep() const            // number of time steps
                  { return nstep; }
      CARDINAL NumFun() const             // number of function evaluations
                  { return nfun; }
      CARDINAL NumJac() const             // number of jacobi matrix evalu-
                  { return njac; }        // ations
      CARDINAL NumErrFail() const         // number of error test failures
                  { return nerrfail; }
      CARDINAL NumConvFail() const        // number of convergence failures
                  { return nconvfail; }

      CARDINAL NumInteger() const         // INTEGER space needed
                  { return ninteger; }
      CARDINAL NumReal() const            // REAL space needed
                  { return nreal; }

   private:

      void     Clear();

      REAL     actt;
      REAL     actdt;
      CARDINAL actord;

      CARDINAL nstep;
      CARDINAL nfun;
      CARDINAL njac;
      CARDINAL nerrfail;
      CARDINAL nconvfail;

      CARDINAL ninteger;
      CARDINAL nreal;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////




