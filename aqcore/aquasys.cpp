///////////////////////////////   aquasys.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   08.07.93    Peter Reichert    Sens. Fcns.(t) + Plot Opt. added
//              05.08.93    Peter Reichert    Uncertainty limits(t) added
//              10.11.99    Peter Reichert    Final version of sens. ranking
//              26.03.00    Peter Reichert    CalcChi2 adapted to UNCSIM
//              27.03.00    Peter Reichert    MaxPost and MCMC added
//              24.12.01    Peter Reichert    SetFit added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <time.h>

#include "all.h"
#include "init.h"
#include "aquasys.h"
#include "das.h"
#include "fit.h"
#include "fortran.h"
#include "linpack.h"
#include "sort.h"
#include "stat.h"
#include "readwords.h"

//////////////////////////////////////////////////////////////////////////////

// global constants, variables and functions:

const REAL MaxSensRel = 10000;

const char* AquasysIdentifier = "AQUASYS";
const char* OptionsIdentifier = "OPTIONS";


                                                        // enum FITMETHOD

const char* const FitMethIdentifier[] = {
                                           "SECANT",    // Secant
                                           "SIMPLEX"    // Simplex
                                        };

const CARDINAL NumFitMeth = sizeof(FitMethIdentifier)/sizeof(char*);

const CARDINAL space = 50;

const CARDINAL BufSize = 1024;

char globversion[BufSize];

static void Space(std::ostream& out, char c, int repeat);

static clock_t       lasttime = 0;    // last processing of events
static REAL          mintime  = 0.1;  // process events only ten times per sec

CANCELFUN            providedcancelfun = 0;
static BOOLEAN       SelectiveCancelFun();

static char          calcname[256];

static AQUASYS*      aquafitsys = 0;
static std::ostream*      fitout     = 0;
static CONSTVAR**    fitvars    = 0;
static FIT**         fits       = 0;
static CARDINAL      nfits      = 0;

static INTEGER**     datind     = 0;
static CARDINAL**    numel      = 0;
static REAL*         fval       = 0;
static REAL*         fsig       = 0;
static REAL*         fcal       = 0;
static REAL*         fcalini    = 0;
static CARDINAL      ndat       = 0;

static CARDINAL      numfun     = 0;

static BOOLEAN       glob_transfersamppdf = FALSE;
static REAL          glob_samppdfval;

static REAL          chicurrent; // global for transfer from fitfun to 
                                 // MinusLogPostPDF and MCMC
static REAL          chiend;     // global for transfer from Fit to IdAnal

static DISTRIBUTION  dist;
static CARDLONG      numvarsets;
static REAL**        varvals;
static REAL*         pripdfvals;
static REAL*         loglikelivals;

//////////////////////////////////////////////////////////////////////////////

BOOLEAN SelectiveCancelFun()
{
   if ( providedcancelfun != 0 )
   {
      clock_t currtime = clock();
      if ( ( (double) (currtime-lasttime) ) / CLOCKS_PER_SEC > mintime )
      {
         // process events
         lasttime = currtime;
         return providedcancelfun();
      }
   }
   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

static void Space(std::ostream& out, char c, int repeat)
{
   for ( int i=0; i<repeat; i++ ) out << c;
}

//////////////////////////////////////////////////////////////////////////////

class XY_WORK
{
   public:

      XY_WORK(CARDINAL n);
      ~XY_WORK();

      REAL*   x;
      REAL*   x1;
      REAL*   x2;
      REAL*   y;
      REAL*   y1;
      REAL*   y2;
      REAL*   sigma;
      REAL*   sigma1;
      REAL*   sigma2;
      BOOLEAN errors;
};


XY_WORK::XY_WORK(CARDINAL n)
{
#ifdef HEAPTEST
HeapFileMessage("start XY_WORK::XY_WORK(CARDINAL n)");
#endif

   if ( n == 0 )
   {
      x = 0; x1 = 0; x2 = 0;
      y = 0; y1 = 0; y2 = 0;
      sigma = 0; sigma1 = 0; sigma2 = 0;
   }
   else
   {
      x1     = new REAL[n];
      x2     = new REAL[n];
      y1     = new REAL[n];
      y2     = new REAL[n];
      sigma1 = new REAL[n];
      sigma2 = new REAL[n];
      for ( CARDINAL i=0; i<n; i++ )
      {
         x1[i]     = 0.0;
         x2[i]     = 0.0;
         y1[i]     = 0.0;
         y2[i]     = 0.0;
         sigma1[i] = 0.0;
         sigma2[i] = 0.0;
      }
      x = x1;
      y = y1;
      sigma = sigma1;
   }
   errors = FALSE;

#ifdef HEAPTEST
HeapFileMessage("end   XY_WORK::XY_WORK(CARDINAL n)");
#endif
}


XY_WORK::~XY_WORK()
{
#ifdef HEAPTEST
HeapFileMessage("start XY_WORK::~XY_WORK()");
#endif

   delete [] x1;      // x must not be deleted because x = x1 !
   delete [] x2;
   delete [] y1;      // y must not be deleted because y = y1 !
   delete [] y2;
   delete [] sigma1;  // sigma must not be deleted because sigma = sigma1 !
   delete [] sigma2;

#ifdef HEAPTEST
HeapFileMessage("end   XY_WORK::~XY_WORK()");
#endif
}

//////////////////////////////////////////////////////////////////////////////

static AQUASYS* aquacalcsys = 0;

JOBSTATUS rhsfun(const REAL& t, const REAL* y, const REAL* yt,
                 REAL* delta)
{
   if ( aquacalcsys == 0 ) FatalError("AQUASYS::rhsfun: No system assigned");
   if ( aquacalcsys->calcoutfun != 0 )
   {
      // process pending events only after an interval of size mintime 

      clock_t currtime = clock();
      if ( ( (double) (currtime-lasttime) ) / CLOCKS_PER_SEC > mintime )
      {
         // process events
         lasttime = currtime;
         if ( aquacalcsys->calcoutfun(calcname,t) == FALSE )
            return INTERRUPT;
      }
   }
   (aquacalcsys->varsys).SetProgVar(Time,&t);
   if ( (aquacalcsys->compsys).Delta(aquacalcsys->numpar,
                                     &(aquacalcsys->varsys),
                                     y,yt,delta) == FALSE )
   {
      return PROBLEM;
   }
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

JOBSTATUS fitfun(const REAL par[], CARDINAL npar, REAL fcalc[], CARDINAL nf)
{
   if ( aquafitsys == 0 ) FatalError("AQUASYS::fitfun: No system assigned");
   COMPSYS* compsys = &(aquafitsys->compsys);
   VARSYS*  varsys  = &(aquafitsys->varsys);
   AQVAR*   timevar = varsys->GetProgVar(Time);
   CARDINAL i;
   CARDINAL idat = 0;
   CARDINAL field = 10;
   if ( fitout != 0 ) field = abs(fitout->precision())+8;
   CALC     calc;

   if ( fitout != 0 )
   {
      FILEIO::PrintLn(*fitout);
      for ( i=0; i<npar; i++ )
      {
         if ( aquafitsys->FitIter() > 0 )
         {
            FILEIO::Print(*fitout,par[i],i*field+2);
         }
      }
      FILEIO::PrintBuffer(*fitout);
   }

   for ( i=0; i<npar; i++ )          // assign current parameter values
   {
      if ( fitvars[i]->SetValue(par[i]) == FALSE )
      {
         // handling rounding problems:
         if ( par[i] > fitvars[i]->Maximum() )
         {
            if ( fitvars[i]->SetValue(fitvars[i]->Maximum()) == FALSE )
               FatalError("aquasys::fitfun: fitvar-maximum cannot be set");
         }
         if ( par[i] < fitvars[i]->Minimum() )
         {
            if ( fitvars[i]->SetValue(fitvars[i]->Minimum()) == FALSE )
               FatalError("aquasys::fitfun: fitvar-minimum cannot be set");
         }
      }
   }

   for ( i=0; i<nfits; i++ )         // loop over different fit calculations
   {
      FIT*     fit     = fits[i];
      CARDINAL calcnum = fit->CalcNum();    // initialize calculations
      REAL     t       = fit->InitTime();
      BOOLEAN  steady  = fit->InitSteady();
      calc.CalcNum(calcnum);
      calc.InitTime(t);
      calc.InitSteady(steady);
      JOBSTATUS status = aquafitsys->InitCalc(&calc,FALSE);
      if ( status != OK )
      {
         if ( status == PROBLEM )
         {
            *cjob << "\n\n   *** " 
                  << "Initialization problem for calculation number " 
                  << calcnum << " ***";
            cjob->flush();
            if ( fitout != 0 )
            {
               if ( glob_transfersamppdf == FALSE )
               {
                  FILEIO::Print(*fitout,NotAvail,npar*field+2);
               }
               else
               {
                  if ( glob_samppdfval < 0 )
                  {
                     FILEIO::Print(*fitout,NotAvail,npar*field+2);
                  }
                  else
                  {
                     FILEIO::Print(*fitout,glob_samppdfval,npar*field+2);
                  }
                  FILEIO::Print(*fitout,NotAvail,(npar+1)*field+2);
               }
               fitout->flush();
            }
         }
         return status;
      }

      INTEGER* index = new INTEGER[fit->NumData()];
      for ( CARDINAL j=0; j<fit->NumData(); j++ )  // initialize indices and
      {                                            // start calc. of results
         index[j] = 0;
         AQVAR*   var       = (fit->Data(j)).var;
         AQCOMP*  com       = (fit->Data(j)).com;
         CARDINAL zone      = (fit->Data(j)).zone;
         REAL     spacetime = (fit->Data(j)).spacetime;
         BOOLEAN  spacerel  = (fit->Data(j)).spacerel;
         AQVAR*   data      = (fit->Data(j)).data;
         if ( com->Active() == TRUE )
         {
            if ( data->Type() == RealListVar )
            {
               REALLISTVAR* datalist = (REALLISTVAR*)data;
               AQVAR*       arg      = datalist->GetArg();
               if ( arg == com->SpaceVar(varsys) )   // spatial profile
               {                                     // spacetime = time
                  if ( spacetime == t )
                  {
                     for ( CARDINAL k=0; k<datalist->NumPairs(); k++ )
                     {
                        REAL x  = datalist->X()[k];
                        REAL value = 0;
                        if ( compsys->SpaceValue(varsys,&calcnum,&t,aquafitsys->Y,
                                                 com,zone,x,spacerel,var,value) == FALSE )
                           FatalError("aquasys::fitfun: cannot obtain value");
                        fcalc[datind[i][j]+index[j]] = value;
                        idat++;
                        index[j]++;
                     }
                  }
               }
               else
               {
                  if ( arg == timevar )
                  {
                     if ( index[j] < datalist->NumPairs() )
                     {
                        REAL x = datalist->X()[index[j]];
                        while ( x < t )
                        {
                           index[j]++;
                           if ( index[j] >= datalist->NumPairs() ) break;
                           x = datalist->X()[index[j]];
                        }
                        if ( x == t )
                        {
                           REAL value = 0;
                           if ( compsys->SpaceValue(varsys,&calcnum,&t,
                                                    aquafitsys->Y,
                                                    com,zone,spacetime,spacerel,var,value)
                                                                    == FALSE )
                              FatalError("aquasys::fitfun: cannot get value");
                           fcalc[datind[i][j]+index[j]] = value;
                           idat++;
                           index[j]++;
                        }
                     }
                  }
               }
            }
         }
      }

      while (1)                      // perform calculations
      {
         REAL tout = t;              // determine next output interval
         CARDINAL j;
         for ( j=0; j<fit->NumData(); j++ )
         {
            AQCOMP* com       = (fit->Data(j)).com;
            REAL    spacetime = (fit->Data(j)).spacetime;
            BOOLEAN spacerel  = (fit->Data(j)).spacerel;
            AQVAR*  data      = (fit->Data(j)).data;
            if ( com->Active() == TRUE )
            {
               if ( data->Type() == RealListVar )
               {
                  REALLISTVAR* datalist = (REALLISTVAR*)data;
                  AQVAR*       arg      = datalist->GetArg();
                  if ( arg == com->SpaceVar(varsys) )  // spatial profile
                  {                                    // spacetime = time
                     if ( spacetime > t )
                     {
                        if ( (tout==t) || (spacetime<tout) ) tout = spacetime;
                     }
                  }
                  else
                  {
                     if ( arg == timevar )             // time series
                     {                                 // val = space
                        if ( index[j] < datalist->NumPairs() )
                        {
                           REAL x = datalist->X()[index[j]];
                           if ( x > t )
                           {
                              if ( (tout==t) || (x<tout) ) tout = x;
                           }
                        }
                     }
                  }
               }
            }
         }
         REAL dt = tout-t;
         if ( dt <= 0.0 ) break;                   // no more data points

         calc.CalcNum(calcnum);
         calc.DelStepSeries(0);
		 calc.AddStepSeries(dt,1,0);
         status = aquafitsys->Calculate(&calc,FALSE);
         if ( status != OK )
         {
            if ( status == PROBLEM )
            {
               *cjob << "\n   " 
                     << "Integration problem for calculation number " 
                     << calcnum;
               cjob->flush();
               if ( fitout != 0 )
               {
                  if ( glob_transfersamppdf == FALSE )
                  {
                     FILEIO::Print(*fitout,NotAvail,npar*field+2);
                  }
                  else
                  {
                     if ( glob_samppdfval < 0 )
                     {
                        FILEIO::Print(*fitout,NotAvail,npar*field+2);
                     }
                     else
                     {
                        FILEIO::Print(*fitout,glob_samppdfval,npar*field+2);
                     }
                     FILEIO::Print(*fitout,NotAvail,(npar+1)*field+2);
                  }
                  fitout->flush();
               }
            }
            return status;
         }
         t = aquafitsys->t;
         if ( t != tout )
         {
            if ( fabs(t-tout)/fmax(fabs(t),fabs(tout)) > 1.0e-10 )
               FatalError("aquasys::fitfun: tend not equal tout");
            t = tout;                             // correct rounding errors
         }
         for ( j=0; j<fit->NumData(); j++ ) // calculate results
         {
            AQVAR*   var       = (fit->Data(j)).var;
            AQCOMP*  com       = (fit->Data(j)).com;
            CARDINAL zone      = (fit->Data(j)).zone;
            REAL     spacetime = (fit->Data(j)).spacetime;
            BOOLEAN  spacerel  = (fit->Data(j)).spacerel;
            AQVAR*   data      = (fit->Data(j)).data;
            if ( com->Active() == TRUE )
            {
               if ( data->Type() == RealListVar )
               {
                  REALLISTVAR* datalist = (REALLISTVAR*)data;
                  AQVAR*       arg      = datalist->GetArg();
                  if ( arg == com->SpaceVar(varsys) )     // spatial profile
                  {                                       // spacetime = time
                     if ( spacetime == t )
                     {
                        for ( CARDINAL k=0; k<datalist->NumPairs(); k++ )
                        {
                           REAL x  = datalist->X()[k];
                           REAL value = 0;
                           if ( compsys->SpaceValue(varsys,&calcnum,&t,
                                                    aquafitsys->Y,com,zone,
                                                    x,spacerel,var,value) == FALSE )
                              FatalError("aquasys::fitfun: cannot get value");
                           fcalc[datind[i][j]+index[j]] = value;
                           idat++;
                           index[j]++;
                        }
                     }
                  }
                  else
                  {
                     if ( arg == timevar )             // time series
                     {                                 // val = space
                        if ( index[j] < datalist->NumPairs() )
                        {
                           REAL x = datalist->X()[index[j]];
                           if ( x == t )
                           {
                              REAL value = 0;
                              if ( compsys->SpaceValue(varsys,&calcnum,&t,
                                                       aquafitsys->Y,com,zone,
                                                       spacetime,spacerel,var,value)
                                                                     == FALSE )
                                 FatalError("aquasys::fitfun: cannot get value");
                              fcalc[datind[i][j]+index[j]] = value;
                              idat++;
                              index[j]++;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      delete index;
   }
   if ( idat != nf )
      FatalError("aquasys::fitfun: data points not correctly evaluated");

   chicurrent = 0.0;
   for ( i=0; i<nf; i++ )
   {
      chicurrent += (fval[i]-fcalc[i])*(fval[i]-fcalc[i])/(fsig[i]*fsig[i]);
      fcal[i] = fcalc[i];
   }
   if ( numfun == 0 )
   {
      for ( i=0; i<nf; i++ ) fcalini[i] = fcal[i];
   }
   numfun++;

   char Buf[BufSize];
   TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
   *cjob << "\n   " << Buf << "  "    // write log file
         << (aquafitsys->ini).T(5) << " = " << chicurrent;
   cjob->flush();
   if ( fitout != 0 )
   {
      if ( aquafitsys->FitIter()>0 && npar>0 )
      {
         if ( glob_transfersamppdf == FALSE )
         {
            FILEIO::Print(*fitout,chicurrent,npar*field+2);
         }
         else
         {
            if ( glob_samppdfval < 0 )
            {
               FILEIO::Print(*fitout,NotAvail,npar*field+2);
            }
            else
            {
               FILEIO::Print(*fitout,glob_samppdfval,npar*field+2);
            }
            FILEIO::Print(*fitout,chicurrent,(npar+1)*field+2);
         }
         fitout->flush();
      }
   }
   if ( aquafitsys->fitoutfun != 0 ) aquafitsys->fitoutfun(chicurrent);
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

JOBSTATUS MinusLogPostPDF(const REAL curvals[], CARDINAL num, 
                          REAL* minuslogpostpdf)
{
   CARDLONG i;
   REAL pripdfval;
   REAL loglikelival;
   REAL logpostpdfval;
   REAL* locvals = new REAL[num];
   for ( i=0; i<num; i++ ) locvals[i] = curvals[i];

   // calculate prior:
   // ----------------

   if ( mv_pdf(dist,locvals,&pripdfval,cjob) == FALSE )
      return PROBLEM;

   // calculate chi2 and loglikelihood:
   // ---------------------------------

   JOBSTATUS status =  fitfun(locvals,num,fcal,ndat);
   if ( status != OK )
   {
      delete [] locvals;
      return status;
   }
   loglikelival = -0.5*chicurrent;

   // calculate post-pdf:
   // -------------------

   logpostpdfval = log(pripdfval)+loglikelival;
   *cjob << "   pri-pdf=" <<  pripdfval << "   log(post-pdf)=" << logpostpdfval;
   cjob->flush();

   // store results:
   // --------------

   REAL** newvarvals       = new REAL*[numvarsets+1];
   REAL*  newpripdfvals    = new REAL[numvarsets+1];
   REAL*  newloglikelivals = new REAL[numvarsets+1];
   for ( i=0; i<numvarsets; i++ )
   {
      newvarvals[i]       = varvals[i];
      newpripdfvals[i]    = pripdfvals[i];
      newloglikelivals[i] = loglikelivals[i];
   }
   newvarvals[numvarsets] = new REAL[num];
   for ( i=0; i<num; i++ ) newvarvals[numvarsets][i] = curvals[i];
   newpripdfvals[numvarsets] = pripdfval;
   newloglikelivals[numvarsets] = loglikelival;
   delete [] varvals;       varvals       = newvarvals;
   delete [] pripdfvals;    pripdfvals    = newpripdfvals;
   delete [] loglikelivals; loglikelivals = newloglikelivals;
   numvarsets++;

   // return result:
   // --------------

   *minuslogpostpdf = -logpostpdfval;

   delete [] locvals;
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

INIT AQUASYS::ini(initfile,"AQUASYS","aqua_txt_",10,10,5,150);

AQUASYS::AQUASYS()
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::AQUASYS()");
#endif

   saved = TRUE;

   fitmeth = Secant;
   fititer = 100;

   printshort = FALSE;

   Ycalcnum = 0;
   init = FALSE;
   start = TRUE;
   senspar = 0;
   t = 0.0;
   numeq = 0;
   relacc = 0;
   absacc = 0;
   eqtype = 0;
   Y = 0;
   YT = 0;
   calcoutfun = 0;
   sensoutfun = 0;
   fitoutfun  = 0;
   cancelfun  = 0;
   savefun    = 0;
   loadfun    = 0;

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::AQUASYS()");
#endif
}


AQUASYS::~AQUASYS()
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::~AQUASYS()");
#endif

   numeq = 0;
   delete [] relacc;
   delete [] absacc;
   delete [] eqtype;
   delete [] Y;
   delete [] YT;

#ifdef HEAPTEST
HeapFileMessage("end AQUASYS::~AQUASYS()");
ListHeap("destructor of class AQUASYS");
#endif
}


void AQUASYS::SetCancelFun(CANCELFUN fun)
{
   providedcancelfun = fun;
   cancelfun = SelectiveCancelFun;
   varsys.SetCancelFun(SelectiveCancelFun);
   procsys.SetCancelFun(SelectiveCancelFun);
   compsys.SetCancelFun(SelectiveCancelFun);
   linksys.SetCancelFun(SelectiveCancelFun);
   calcsys.SetCancelFun(SelectiveCancelFun);
   fitsys.SetCancelFun(SelectiveCancelFun);
   plotsys.SetCancelFun(SelectiveCancelFun);
   statesys.SetCancelFun(SelectiveCancelFun);
   return;
}


void AQUASYS::ResetCancelFun()
{
   providedcancelfun = 0;
   cancelfun = 0;
   varsys.ResetCancelFun();
   procsys.ResetCancelFun();
   compsys.ResetCancelFun();
   linksys.ResetCancelFun();
   calcsys.ResetCancelFun();
   fitsys.ResetCancelFun();
   plotsys.ResetCancelFun();
   statesys.ResetCancelFun();
   return;
}


void AQUASYS::InitLogFile()
{
   Space(*cjob,'*',72);
   *cjob << "\n\n"
         << ProgName() << " " << ProgVersion() << " - " << ini.T(8)
         << "\n\n";
   Space(*cjob,'*',72);
   *cjob << "\n\n";
   char Buf[BufSize];
   TimeToString(ini.T(9),Buf,BufSize);
   *cjob << Buf << "\n";
   cjob->flush();
   return;
}


BOOLEAN AQUASYS::Activate(AQCOMP* com)
{
   if ( compsys.Exist(com) == FALSE ) return FALSE;
   if ( com->active == FALSE ) DeleteStates();
   com->active = TRUE;
   compsys.CalcIndex();
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::Inactivate(AQCOMP* com)
{
   if ( compsys.Exist(com) == FALSE ) return FALSE;
   if ( com->active == TRUE ) DeleteStates();
   com->active = FALSE;
   compsys.CalcIndex();
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::AddVar(AQVAR* var)
{
   if ( var == 0 )                   return FALSE;
   if ( var->AllowedVar() == FALSE ) return FALSE;
   if ( varsys.Add(var) == FALSE )   return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( (oldvar==0) || (newvar==0) )                        return FALSE;
   if ( newvar->AllowedVar() == FALSE )                     return FALSE;
   if ( compsys.AllowedReplaceVar(oldvar,newvar) == FALSE ) return FALSE;
   if ( linksys.AllowedReplaceVar(oldvar,newvar) == FALSE ) return FALSE;
   if ( varsys.Replace(oldvar,newvar) == FALSE )            return FALSE;
   procsys.ReplaceVar(oldvar,newvar);
   compsys.ReplaceVar(oldvar,newvar);
   linksys.ReplaceVar(oldvar,newvar);
   fitsys.ReplaceVar(oldvar,newvar);
   plotsys.ReplaceVar(oldvar,newvar);

   if ( oldvar->Type() == StateVar )
   {
      if ( compsys.Arg(oldvar) == TRUE )
      {
         if ( newvar->Type() == StateVar )
         {
            STATEVAR* ov = (STATEVAR*)oldvar;
            STATEVAR* nv = (STATEVAR*)newvar;
            if ( ov->StateVarType() != nv->StateVarType() )
            {
               DeleteStates();
            }
         }
         else
         {
            DeleteStates();
         }
      }
   }
   else
   {
      if ( newvar->Type() == StateVar )
      {
         if ( compsys.Arg(oldvar) == TRUE )
         {
            DeleteStates();
         }
      }
   }
   delete oldvar;
   start = TRUE;  // restart integration

   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( (var1==0) || (var2==0) )                         return FALSE;
   if ( compsys.AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
   if ( linksys.AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
   if ( varsys.Exchange(var1,var2) == FALSE )            return FALSE;
   procsys.ExchangeVar(var1,var2);
   compsys.ExchangeVar(var1,var2);
   linksys.ExchangeVar(var1,var2);
   fitsys.ExchangeVar(var1,var2);
   plotsys.ExchangeVar(var1,var2);

   if ( var1->Type() == StateVar )
   {
      if ( compsys.Arg(var1)==TRUE || compsys.Arg(var2)==TRUE )
      {
         if ( var2->Type() == StateVar )
         {
            STATEVAR* v1 = (STATEVAR*)var1;
            STATEVAR* v2 = (STATEVAR*)var2;
            if ( v1->StateVarType() != v2->StateVarType() )
            {
               DeleteStates();
            }
         }
         else
         {
            DeleteStates();
         }
      }
   }
   else
   {
      if ( var1->Type() == StateVar )
      {
         if ( compsys.Arg(var1)==TRUE || compsys.Arg(var2)==TRUE )
         {
            DeleteStates();
         }
      }
   }
   start = TRUE;  // restart integration

   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteVar(AQVAR* var)
{
   if ( var == 0 )                    return FALSE;
   if ( procsys.Arg(var) == TRUE )    return FALSE;
   if ( compsys.Arg(var) == TRUE )    return FALSE;
   if ( linksys.Arg(var) == TRUE )    return FALSE;
   if ( fitsys.Arg(var) == TRUE )     return FALSE;
   plotsys.DeleteCurves(var);
   Changed();
   if ( varsys.Delete(var) == FALSE ) return FALSE;
   return TRUE;
}


BOOLEAN AQUASYS::AddProc(AQPROC* proc)
{
   if ( proc == 0 )                                  return FALSE;
   for ( CARDINAL i=0; i<proc->NumVarArg(); i++ )
   {
      if ( varsys.Exist(proc->VarArg(i)) == FALSE )  return FALSE;
   }
   if ( procsys.Add(proc) == FALSE )                 return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteProc(AQPROC* proc)
{
   if ( proc == 0 )                     return FALSE;
   if ( compsys.Arg(proc) == TRUE )     return FALSE;
   if ( procsys.Delete(proc) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplaceProc(AQPROC* oldproc, AQPROC* newproc)
{
   if ( (oldproc==0) || (newproc==0) )                         return FALSE;
   for ( CARDINAL i=0; i<newproc->NumVarArg(); i++ )
   {
      if ( varsys.Exist(newproc->VarArg(i)) == FALSE )         return FALSE;
   }
   if ( compsys.AllowedReplaceProc(oldproc,newproc) == FALSE ) return FALSE;
   if ( procsys.Replace(oldproc,newproc) == FALSE )            return FALSE;
   compsys.ReplaceProc(oldproc,newproc);
   delete oldproc;

   start = TRUE;  // restart integration

   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::AddComp(AQCOMP* com)
{
   if ( com == 0 )                                   return FALSE;
   CARDINAL i;
   for ( i=0; i<com->NumVarArg(); i++ )
   {
      if ( varsys.Exist(com->VarArg(i)) == FALSE )   return FALSE;
   }
   for ( i=0; i<com->NumActProc(); i++ )
   {
      if ( procsys.Exist(com->ActProc(i)) == FALSE ) return FALSE;
   }
   if ( compsys.Add(com) == FALSE )                  return FALSE;

   DeleteStates();

   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   if ( (oldcomp==0) || (newcomp==0) )                         return FALSE;
   if ( linksys.AllowedReplaceComp(oldcomp,newcomp) == FALSE ) return FALSE;
   CARDINAL i;
   for ( i=0; i<newcomp->NumVarArg(); i++ )
   {
      if ( varsys.Exist(newcomp->VarArg(i)) == FALSE )         return FALSE;
   }
   for ( i=0; i<newcomp->NumActProc(); i++ )
   {
      if ( procsys.Exist(newcomp->ActProc(i)) == FALSE )       return FALSE;
   }
   if ( compsys.Replace(oldcomp,newcomp) == FALSE )            return FALSE;
   varsys.ReplaceComp(oldcomp,newcomp);
   linksys.ReplaceComp(oldcomp,newcomp);
   fitsys.ReplaceComp(oldcomp,newcomp);
   plotsys.ReplaceComp(oldcomp,newcomp);

   if ( oldcomp->Type() != newcomp->Type() )   DeleteStates();
   if ( oldcomp->NumEq() != newcomp->NumEq() ) DeleteStates();
   if ( oldcomp->NumZones() == newcomp->NumZones() )
   {
      for ( i=0; i<oldcomp->NumZones(); i++ )
      {
         if ( oldcomp->NumZoneGridPts(i) != newcomp->NumZoneGridPts(i) )
         {
            DeleteStates();
         }
      }
   }
   else
   {
      DeleteStates();
   }
   if ( oldcomp->NumActVar() == newcomp->NumActVar() )
   {
      for ( i=0; i<oldcomp->NumActVar(); i++ )
      {
         if ( oldcomp->ActVar(i) != newcomp->ActVar(i) )
         {
            DeleteStates();
         }
      }
   }
   else
   {
      DeleteStates();
   }
   delete oldcomp;
   start = TRUE;  // restart integration

   RebuildConnections();
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteComp(AQCOMP* com)
{
   if ( com == 0 )                     return FALSE;
   if ( varsys.Arg(com) == TRUE )      return FALSE;
   if ( linksys.Arg(com) == TRUE )     return FALSE;
   if ( fitsys.Arg(com) == TRUE )      return FALSE;
   plotsys.DeleteCurves(com);

   DeleteStates();
   Changed();

   if ( compsys.Delete(com) == FALSE ) return FALSE;

   return TRUE;
}


BOOLEAN AQUASYS::AddLink(AQLINK* link)
{
   if ( link == 0 )                                   return FALSE;
   CARDINAL i;
   for ( i=0; i<link->NumVarArg(); i++ )
   {
      if ( varsys.Exist(link->VarArg(i)) == FALSE )   return FALSE;
   }
   for ( i=0; i<link->NumCompArg(); i++ )
   {
      if ( compsys.Exist(link->CompArg(i)) == FALSE ) return FALSE;
   }
   if ( link->Type() == AdvLink )
   {
      ADVLINK* advlink = (ADVLINK*)link;
      AQCOMP* com = advlink->GetCompIn();
      if ( com != 0 )
      {
         CARDINAL conn = advlink->GetConnIn();
         if ( com->advoutlink[conn] != 0 )             return FALSE;
      }
   }
   if ( linksys.Add(link) == FALSE )                   return FALSE;
   RebuildConnections();
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplaceLink(AQLINK* oldlink, AQLINK* newlink)
{
   if ( (oldlink==0) || (newlink==0) )                        return FALSE;
   CARDINAL i;
   for ( i=0; i<newlink->NumVarArg(); i++ )
   {
      if ( varsys.Exist(newlink->VarArg(i)) == FALSE )        return FALSE;
   }
   for ( i=0; i<newlink->NumCompArg(); i++ )
   {
      if ( compsys.Exist(newlink->CompArg(i)) == FALSE )      return FALSE;
   }
   if ( oldlink->Type() != newlink->Type() )                  return FALSE;
   if ( oldlink->Type() == AdvLink )
   {
      ADVLINK* advlink = (ADVLINK*)newlink;
      AQCOMP* com = advlink->GetCompIn();
      if ( com != 0 )
      {
         CARDINAL conn = advlink->GetConnIn();
         if ( com->advoutlink[conn] != 0 )
         {
            if ( com->advoutlink[conn] != (ADVLINK*)oldlink ) return FALSE;
         }
      }
   }
   if ( linksys.Replace(oldlink,newlink) == FALSE )           return FALSE;
   delete oldlink;
   RebuildConnections();
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteLink(AQLINK* link)
{
   if ( link == 0 )                     return FALSE;
   if ( linksys.Delete(link) == FALSE ) return FALSE;
   RebuildConnections();
   Changed();
   return TRUE;
}


void AQUASYS::RebuildConnections()
{
   AQCOMP* com = (compsys.Complist())->First();
   while ( com != 0 )
   {
      CARDINAL conn;
      for ( conn=0; conn<com->NumAdvInConn(); conn++ )
      {
         if ( com->advinnum[conn] > 0 )
         {
            delete [] com->advinindex[conn]; com->advinindex[conn] = 0;
            delete [] com->advinlink[conn];  com->advinlink[conn]  = 0;
            com->advinnum[conn] = 0;
         }
      }
      for ( conn=0; conn<com->NumAdvOutConn(); conn++ )
      {
         com->advoutlink[conn] = 0;
      }
      for ( conn = 0; conn<com->NumDiffConn(); conn++ )
      {
         if ( com->diffnum[conn] > 0 )
         {
            delete [] com->diffindex[conn]; com->diffindex[conn] = 0;
            delete [] com->difflink[conn];  com->difflink[conn]  = 0;
            com->diffnum[conn] = 0;
         }
      }
      com = com->Next();
   }
   AQLINK* link = (linksys.Linklist())->First();
   while ( link != 0 )
   {
      switch ( link->Type() )
      {
         case AdvLink:
         {
            ADVLINK* advlink = (ADVLINK*)link;
            com = advlink->GetCompIn();
            if ( com != 0 )
            {
               CARDINAL conn = advlink->GetConnIn();
               com->advoutlink[conn] = advlink;
            }
            com = advlink->GetCompOut();
            if ( com != 0 )
            {
               CARDINAL conn = advlink->GetConnOut();
               CARDINAL* newindex = new CARDINAL[com->advinnum[conn]+1];
               ADVLINK** newlink  = new ADVLINK*[com->advinnum[conn]+1];
               for ( CARDINAL i=0; i<com->advinnum[conn]; i++ )
               {
                  newindex[i] = com->advinindex[conn][i];
                  newlink[i]  = com->advinlink[conn][i];
               }
               newindex[com->advinnum[conn]] = 0;
               newlink[com->advinnum[conn]]  = advlink;
               delete [] com->advinindex[conn];
               com->advinindex[conn] = newindex;
               delete [] com->advinlink[conn];
               com->advinlink[conn]  = newlink;
               com->advinnum[conn]++;
            }
            CARDINAL k = 0;
            BIF* bif = advlink->Biflist()->First();
            while ( bif != 0 )
            {
               k++;
               com = bif->GetBifComp();
               if ( com != 0 )
               {
                  CARDINAL conn = bif->GetBifConn();
                  CARDINAL* newindex = new CARDINAL[com->advinnum[conn]+1];
                  ADVLINK** newlink  = new ADVLINK*[com->advinnum[conn]+1];
                  for ( CARDINAL i=0; i<com->advinnum[conn]; i++ )
                  {
                     newindex[i] = com->advinindex[conn][i];
                     newlink[i]  = com->advinlink[conn][i];
                  }
                  newindex[com->advinnum[conn]] = k;
                  newlink[com->advinnum[conn]]  = advlink;
                  delete [] com->advinindex[conn];
                  com->advinindex[conn] = newindex;
                  delete [] com->advinlink[conn];
                  com->advinlink[conn]  = newlink;
                  com->advinnum[conn]++;
               }
               bif = bif->Next();
            }
            break;
         }
         case DiffLink:
         {
            DIFFLINK* difflink = (DIFFLINK*)link;
            com = difflink->GetComp1();
            if ( com != 0 )
            {
               CARDINAL conn = difflink->GetConn1();
               CARDINAL* newindex = new CARDINAL[com->diffnum[conn]+1];
               DIFFLINK** newlink = new DIFFLINK*[com->diffnum[conn]+1];
               for ( CARDINAL i=0; i<com->diffnum[conn]; i++ )
               {
                  newindex[i] = com->diffindex[conn][i];
                  newlink[i]  = com->difflink[conn][i];
               }
               newindex[com->diffnum[conn]] = 0;
               newlink[com->diffnum[conn]]  = difflink;
               delete [] com->diffindex[conn];
               com->diffindex[conn] = newindex;
               delete [] com->difflink[conn];
               com->difflink[conn]  = newlink;
               com->diffnum[conn]++;
            }
            com = difflink->GetComp2();
            if ( com != 0 )
            {
               CARDINAL conn = difflink->GetConn2();
               CARDINAL* newindex = new CARDINAL[com->diffnum[conn]+1];
               DIFFLINK** newlink = new DIFFLINK*[com->diffnum[conn]+1];
               for ( CARDINAL i=0; i<com->diffnum[conn]; i++ )
               {
                  newindex[i] = com->diffindex[conn][i];
                  newlink[i]  = com->difflink[conn][i];
               }
               newindex[com->diffnum[conn]] = 1;
               newlink[com->diffnum[conn]]  = difflink;
               delete [] com->diffindex[conn];
               com->diffindex[conn] = newindex;
               delete [] com->difflink[conn];
               com->difflink[conn]  = newlink;
               com->diffnum[conn]++;
            }
            break;
         }
      }
      link = link->Next();
   }
   return;
}


BOOLEAN AQUASYS::AddCalc(CALC* calc)
{
   if ( calcsys.Add(calc) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplaceCalc(CALC* oldcalc, CALC* newcalc)
{
   if ( calcsys.Replace(oldcalc,newcalc) == FALSE ) return FALSE;
   delete oldcalc;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteCalc(CALC* calc)
{
   if ( calcsys.Delete(calc) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::SimActive(CALC* calc, BOOLEAN act)
{
   if ( calcsys.SimActive(calc,act) == TRUE )
   {
      Changed();
      return TRUE;
   }
   return FALSE;
}


BOOLEAN AQUASYS::SensActive(CALC* calc, BOOLEAN act)
{
   if ( calcsys.SensActive(calc,act) == TRUE )
   {
      Changed();
      return TRUE;
   }
   return FALSE;
}


BOOLEAN AQUASYS::AddFit(FIT* fit)
{
   if ( fit == 0 ) return FALSE;
   CARDINAL i;
   for ( i=0; i<fit->NumVarArg(); i++ )
   {
      if ( varsys.Exist(fit->VarArg(i)) == FALSE ) return FALSE;
   }
   for ( i=0; i<fit->NumCompArg(); i++ )
   {
      if ( compsys.Exist(fit->CompArg(i)) == FALSE ) return FALSE;
   }
   if ( fitsys.Add(fit) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplaceFit(FIT* oldfit, FIT* newfit)
{
   if ( (oldfit==0) || (newfit==0) )                    return FALSE;
   CARDINAL i;
   for ( i=0; i<newfit->NumVarArg(); i++ )
   {
      if ( varsys.Exist(newfit->VarArg(i)) == FALSE )   return FALSE;
   }
   for ( i=0; i<newfit->NumCompArg(); i++ )
   {
      if ( compsys.Exist(newfit->CompArg(i)) == FALSE ) return FALSE;
   }
   if ( fitsys.Replace(oldfit,newfit) == FALSE ) return FALSE;
   delete oldfit;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteFit(FIT* fit)
{
   if ( fitsys.Delete(fit) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::FitActive(FIT* fit, BOOLEAN act)
{
   if ( fitsys.FitActive(fit,act) == TRUE )
   {
      Changed();
      return TRUE;
   }
   return FALSE;
}


BOOLEAN AQUASYS::AddPlot(PLOT* plot)
{
   if ( plot == 0 ) return FALSE;
   CARDINAL i;
   for ( i=0; i<plot->NumVarArg(); i++ )
   {
      if ( varsys.Exist(plot->VarArg(i)) == FALSE ) return FALSE;
   }
   for ( i=0; i<plot->NumCompArg(); i++ )
   {
      if ( compsys.Exist(plot->CompArg(i)) == FALSE ) return FALSE;
   }
   if ( plotsys.Add(plot) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::ReplacePlot(PLOT* oldplot, PLOT* newplot)
{
   if ( (oldplot==0) || (newplot==0) )                   return FALSE;
   CARDINAL i;
   for ( i=0; i<newplot->NumVarArg(); i++ )
   {
      if ( varsys.Exist(newplot->VarArg(i)) == FALSE )   return FALSE;
   }
   for ( i=0; i<newplot->NumCompArg(); i++ )
   {
      if ( compsys.Exist(newplot->CompArg(i)) == FALSE ) return FALSE;
   }
   if ( plotsys.Replace(oldplot,newplot) == FALSE )      return FALSE;
   delete oldplot;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeletePlot(PLOT* plot)
{
   if ( plotsys.Delete(plot) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::Consolidate(REAL* a, CARDINAL n)
{
   if ( n < 3 ) return TRUE;   // correction 27.7.97
   if ( a[0] < a[n-1] )
   {
      if ( a[1] > a[n-2] ) return FALSE;
      CARDINAL j = 1;
      while ( a[j] < a[0] ) j++;
      if ( j > 1 )
      {
         for ( CARDINAL i=1; i<j; i++ )
         {
            a[i] = a[0] + (a[j]-a[0])/((REAL)j)*((REAL)i);
         }
      }
      j = n-2;
      while ( a[j] > a[n-1] ) j--;
      if ( j < n-2 )
      {
         for ( CARDINAL i=n-2; i>j; i-- )
         {
            a[i] = a[n-1] - (a[n-1]-a[j])/((REAL)(n-j-1))*((REAL)(n-i-1));
         }
      }
   }
   else
   {
      if ( a[1] < a[n-2] ) return FALSE;
      CARDINAL j = 1;
      while ( a[j] > a[0] ) j++;
      if ( j > 1 )
      {
         for ( CARDINAL i=1; i<j; i++ )
         {
            a[i] = a[0] + (a[j]-a[0])/((REAL)j)*((REAL)i);
         }
      }
      j = n-2;
      while ( a[j] < a[n-1] ) j--;
      if ( j < n-2 )
      {
         for ( CARDINAL i=n-2; i>j; i-- )
         {
            a[i] = a[n-1] - (a[n-1]-a[j])/((REAL)(n-j-1))*((REAL)(n-i-1));
         }
      }
   }
   return TRUE;
}


void AQUASYS::PlotLimits(REAL DataMin, REAL DataMax,
                         REAL* Min, REAL* Max, REAL* Pos, REAL* Dist) const
{
   REAL MinMax = fabs(DataMax-DataMin);
   if ( fabs(DataMin+DataMax) < 1e-30 )
   {
      DataMin = -1e-6;
      DataMax =  1e-6;
      MinMax = fabs(DataMax-DataMin);
   }
   else
   {
      if ( MinMax < 1e-6*fabs(DataMin+DataMax) )
      {
         if ( DataMin > 0.0 )
         {
            DataMin = 0.0; DataMax = 1.1*DataMax;
         }
         else
         {
            DataMax = 0.0; DataMin = 1.1*DataMin;
         }
         MinMax = fabs(DataMax-DataMin);
      }
   }
   REAL r = log10(MinMax);
   INTEGER i = (INTEGER)r; if ( r < 0.0 ) i--;
   *Dist = pow(10.0,(REAL)i);
   switch ( (CARDINAL)(MinMax/(*Dist)) )
   {
      case 0:
         *Dist = 0.2*(*Dist);
         break;
      case 1: case 2: case 3:
         *Dist = 0.5*(*Dist);
         break;
      case 4: case 5: case 6: case 7: case 8: case 9: case 10:
         break;
      default:
         *Dist = 2.0*(*Dist);
         break;
   }
   if ( DataMin >= 0.0 )
   {
      i = (INTEGER)(DataMin/(*Dist));
      if ( i > 2 )
      {
         *Min = i*(*Dist);
      }
      else
      {
         *Min = 0.0;
      }
   }
   else
   {
      i = (INTEGER)(-DataMin/(*Dist));
      if ( i*(*Dist) < -DataMin ) i++;
      if ( i<9 || i>10 )
      {
         *Min = -i*(*Dist);
      }
      else
      {
         *Min = -10*(*Dist);
      }
   }
   if ( DataMax >= 0.0 )
   {
      i = (INTEGER)(DataMax/(*Dist));
      if ( i*(*Dist) < DataMax ) i++;
      if ( i<9 || i>10 )
      {
         *Max = i*(*Dist);
      }
      else
      {
         *Max = 10*(*Dist);
      }
   }
   else
   {
      i = (INTEGER)(-DataMax/(*Dist));
      if ( i > 2 )
      {
         *Max = -i*(*Dist);
      }
      else
      {
         *Max = 0.0;
      }
   }
   *Pos = *Min;
   return;
}


JOBSTATUS AQUASYS::GetCurveData(ABSCTYPE at, const CURVE* curve,
                                CARDINAL* n, REAL** x, REAL** y,
                                REAL** yminus, REAL** yplus)
{
   CARDINAL i;
   if ( n==0 || x==0 || y==0 )                  return PROBLEM;
   *n = 0; *x = 0; *y = 0;
   AQVAR*    var  = curve->Variable();
   AQCOMP*   com  = curve->Compartment();
   CARDINAL  zone = curve->Zone();
   CARDINAL  cn   = curve->CalcNum();
   AQVAR*    par  = curve->Parameter();
   CURVETYPE ct   = curve->Type();
   if ( var==0 || com==0 )                      return PROBLEM;
   // added 28.8.98:
   if ( zone >= com->NumZones() )               return PROBLEM;
   // end of addition 28.8.98
   if ( ct != CurveVal )
   {
      if ( par == 0 )                           return PROBLEM;
      if ( statesys.ExistPar(cn,par) == FALSE ) return PROBLEM;
   }
   if ( ct!=CurveVal && par==0 )                return PROBLEM;
   if ( yminus != 0 ) *yminus = 0;
   if ( yplus  != 0 ) *yplus  = 0;
   switch ( at )
   {
      case AbscTime:
      {
         if ( ct==CurveVal && var->Type()==RealListVar )
         {
            REALLISTVAR* rl = (REALLISTVAR*)var;
            AQVAR* arg = rl->GetArg();
            if ( arg->Type() == ProgVar )
            {
               PROGVAR* pv = (PROGVAR*)arg;
               if ( pv->ProgVarType() == Time )
               {
                  *n = rl->NumPairs();
                  if ( *n == 0 ) return PROBLEM;
                  *x = new REAL[*n];
                  *y = new REAL[*n];
                  for ( i=0; i<*n; i++ )
                  {
                     (*x)[i] = rl->X()[i];
                     (*y)[i] = rl->Y()[i];
                  }
                  if ( cancelfun != 0 )
                  {
                     if ( cancelfun() == TRUE )
                     {
                        *n = 0;
                        delete [] *x; *x = 0;
                        delete [] *y; *y = 0;
                        return INTERRUPT;
                     }
                  }
                  return OK;
               }
            }
         }
         CARDINAL nt = statesys.NumStates(cn);
         if ( nt == 0 ) return PROBLEM;
         XY_WORK wt(nt);
         switch ( ct )
         {
            case CurveVal:
               for ( i=0; i<nt; i++ )    // values
               {
                  const STATE* state = statesys.GetState(cn,i);
                  if ( state == 0 )
                     FatalError("AQUASYS::GetCurveData: illegal state");
                  wt.x[i] = state->Time();
                  REAL f;
                  if ( com->SpaceValue(&varsys,&cn,&wt.x[i],
                                       state->Y(),zone,curve->XorT(),
                                       curve->SpaceRel(),var,f) == FALSE )
                     return PROBLEM;
                  wt.y[i] = f;
                  if ( cancelfun != 0 )
                  {
                     if ( cancelfun() == TRUE ) return INTERRUPT;
                  }
               }
               if ( statesys.NumPar(cn) > 0 )   // calculate standard deviations
               {
                  for ( i=0; i<nt; i++ )
                  {
                     for ( CARDINAL j=0; j<statesys.NumPar(cn); j++ )
                     {
                        AQVAR* par = statesys.Par(cn,j);
                        const STATE* state = statesys.GetState(cn,i);
                        if ( state == 0 )
                           FatalError("AQUASYS::GetCurveData: illegal state");
                        REAL p;
						REAL time = state->Time();
                        if ( com->SpaceValue(&varsys,&cn,&time,
                                             state->Y(),zone,curve->XorT(),
                                             curve->SpaceRel(),par,p) == FALSE )
                           return PROBLEM;
                        const STATE* statepar = statesys.GetState(cn,i,par);
                        if ( statepar == 0 ) return PROBLEM;
                        if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                        REAL ff;
						time = statepar->Time();
                        if ( com->SpaceValue(&varsys,&cn,&time,
                                             statepar->Y(),zone,curve->XorT(),
                                             curve->SpaceRel(),var,ff) == FALSE )
                        {
                           par->SetSensValue(FALSE); return PROBLEM;
                        }
                        REAL pp;
						time = statepar->Time();
                        if ( com->SpaceValue(&varsys,&cn,&time,
                                             statepar->Y(),zone,curve->XorT(),
                                             curve->SpaceRel(),par,pp) == FALSE )
                        {
                           par->SetSensValue(FALSE); return PROBLEM;
                        }
                        par->SetSensValue(FALSE);
                        REAL s = 0;
                        if ( par->Type() == ConstVar )
                        {
                           CONSTVAR* cv = (CONSTVAR*)par;
                           s = cv->StDev();
                        }
                        else
                        {
                           if ( par->Type() == RealListVar )
                           {
                              REALLISTVAR* rlv = (REALLISTVAR*)par;
                              REAL arg;
							  time = state->Time();
                              if ( com->SpaceValue(&varsys,&cn,&time,
                                                   state->Y(),zone,curve->XorT(),
                                                   curve->SpaceRel(),rlv->GetArg(),arg) == FALSE )
                                 return PROBLEM;
                              s = rlv->StDev(arg);
                           }
                           else
                           {
                              FatalError("AQUASYS::GetCurveData: illegal par type");
                           }
                        }
                        wt.sigma[i] += (wt.y[i]-ff)*(wt.y[i]-ff)
                                       /((p-pp)*(p-pp))*s*s;
                     }
                     wt.sigma[i] = sqrt(wt.sigma[i]);
                     if ( cancelfun != 0 )
                     {
                        if ( cancelfun() == TRUE ) return INTERRUPT;
                     }
                  }
                  wt.errors = TRUE;
               }
               break;
            default:
               for ( i=0; i<nt; i++ )
               {
                  const STATE* state = statesys.GetState(cn,i);
                  if ( state == 0 )
                     FatalError("AQUASYS::GetCurveData: illegal state");
                  wt.x[i] = state->Time();
                  REAL f;
                  if ( com->SpaceValue(&varsys,&cn,&wt.x[i],
                                       state->Y(),zone,curve->XorT(),
                                       curve->SpaceRel(),var,f) == FALSE )
                     return PROBLEM;
                  REAL p;
                  if ( com->SpaceValue(&varsys,&cn,&wt.x[i],
                                       state->Y(),zone,curve->XorT(),
                                       curve->SpaceRel(),par,p) == FALSE )
                     return PROBLEM;
                  const STATE* statepar = statesys.GetState(cn,i,par);
                  if ( statepar == 0 ) return PROBLEM;
                  if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                  REAL ff;
				  REAL time = statepar->Time();
                  if ( com->SpaceValue(&varsys,&cn,&time,
                                       statepar->Y(),zone,curve->XorT(),
                                       curve->SpaceRel(),var,ff) == FALSE )
                  {
                     par->SetSensValue(FALSE); return PROBLEM;
                  }
                  REAL pp;
				  time = statepar->Time();
                  if ( com->SpaceValue(&varsys,&cn,&time,
                                       statepar->Y(),zone,curve->XorT(),
                                       curve->SpaceRel(),par,pp) == FALSE )
                  {
                     par->SetSensValue(FALSE); return PROBLEM;
                  }
                  par->SetSensValue(FALSE);
                  switch ( ct )
                  {
                     case CurveSensErr:
                        {
                           REAL s = 0;
                           if ( par->Type() == ConstVar )
                           {
                              CONSTVAR* cv = (CONSTVAR*)par;
                              s = cv->StDev();
                           }
                           else
                           {
                              if ( par->Type() == RealListVar )
                              {
                                 REALLISTVAR* rlv = (REALLISTVAR*)par;
                                 REAL arg;
								 time = state->Time();
                                 if ( com->SpaceValue(&varsys,&cn,&time,
                                                      state->Y(),zone,
                                                      curve->XorT(),
                                                      curve->SpaceRel(),
                                                      rlv->GetArg(),arg)
                                                                    == FALSE )
                                    return PROBLEM;
                                 s = rlv->StDev(arg);
                              }
                              else
                              {
                                 FatalError("AQUASYS::GetCurveData: illegal par type");
                              }
                           }
                           wt.y[i] = (f-ff)/(p-pp)*s;
                        }
                        break;
                     case CurveSensAbsAbs:
                        wt.y[i] = (f-ff)/(p-pp);
                        break;
                     case CurveSensRelAbs:
                        {
                           REAL a = (f-ff)/(p-pp);
                           if ( a == 0.0 )
                           {
                              wt.y[i] = 0.0;
                           }
                           else
                           {
                              if ( fabs(a) > MaxSensRel*fabs(f) )
                              {
                                 wt.y[i] = 2*MaxSensRel;
                              }
                              else
                              {
                                 wt.y[i] = a/f;
                              }
                           }
                        }
                        break;
                     case CurveSensAbsRel:
                        wt.y[i] = p*(f-ff)/(p-pp);
                        break;
                     case CurveSensRelRel:
                        {
                           REAL a = p*(f-ff)/(p-pp);
                           if ( a == 0.0 )
                           {
                              wt.y[i] = 0.0;
                           }
                           else
                           {
                              if ( fabs(a) > MaxSensRel*fabs(f) )
                              {
                                 wt.y[i] = 2*MaxSensRel;
                              }
                              else
                              {
                                 wt.y[i] = a/f;
                              }
                           }
                        }
                        break;
                     default:
                        FatalError("AQUASYS::GetCurveData: illegal curve type");
                        break;
                  }
                  if ( cancelfun != 0 )
                  {
                     if ( cancelfun() == TRUE ) return INTERRUPT;
                  }
               }
               if ( ct==CurveSensRelAbs || ct==CurveSensRelRel )
               {
                  for ( i=0; i<nt; i++ )
                  {
                     if ( wt.y[i] > MaxSensRel )    // look at neighbours
                     {                              // to find sign
                        BOOLEAN positive = TRUE;
                        for ( CARDINAL j=1; j<nt; j++ )
                        {
                           if ( i+j < nt )
                           {
                              if ( wt.y[i+j] < MaxSensRel )
                              {
                                 if ( wt.y[i+j] < 0.0 ) positive = FALSE;
                                 break;
                              }
                           }
                           if ( j <= i )
                           {
                              if ( wt.y[i-j] < MaxSensRel )
                              {
                                 if ( wt.y[i-j] < 0.0 ) positive = FALSE;
                                 break;
                              }
                           }
                        }
                        if ( positive == TRUE ) wt.y[i] = MaxSensRel;
                        else                    wt.y[i] = -MaxSensRel;
                     }
                  }
               }
               break;
         }
         *n = nt;
         *x = new REAL[*n];
         *y = new REAL[*n];
         for ( i=0; i<*n; i++ )
         {
            (*x)[i] = wt.x[i];
            (*y)[i] = wt.y[i];
         }
         if ( wt.errors==TRUE && yminus!=0 && yplus!=0 )
         {
            *yminus = new REAL[*n];
            *yplus  = new REAL[*n];
            for ( i=0; i<*n; i++ )
            {
               (*yminus)[i] = wt.y[i]-wt.sigma[i];
               (*yplus)[i]  = wt.y[i]+wt.sigma[i];
            }
         }
      }
      break;
      case AbscSpace:
      {
         if ( ct==CurveVal && var->Type()==RealListVar )
         {
            REALLISTVAR* rl = (REALLISTVAR*)var;
            AQVAR* arg = rl->GetArg();
            if ( arg == com->SpaceVar(&varsys) )
            {
               *n = rl->NumPairs();
               if ( *n == 0 ) return PROBLEM;
               *x = new REAL[*n];
               *y = new REAL[*n];
               for ( i=0; i<*n; i++ )
               {
                  (*x)[i] = rl->X()[i];
                  (*y)[i] = rl->Y()[i];
               }
               if ( cancelfun != 0 )
               {
                  if ( cancelfun() == TRUE )
                  {
                     *n = 0;
                     delete [] *x; *x = 0;
                     delete [] *y; *y = 0;
                     return INTERRUPT;
                  }
               }
               return OK;
            }
         }
         CARDINAL nt = statesys.NumStates(cn);
         if ( nt == 0 ) return PROBLEM;
         REAL time = curve->XorT();
         if ( time < (statesys.GetState(cn,0))->Time() )    return PROBLEM;
         if ( time > (statesys.GetState(cn,nt-1))->Time() ) return PROBLEM;
         CARDINAL stateindex = 0;
         if ( nt > 1 )
         {
            while ( time > (statesys.GetState(cn,stateindex+1))->Time() )
            {
               stateindex++;
               if ( stateindex+1 >= nt )
                  FatalError("AQUASYS::GetCurveData: illegal state");
            }
         }
         REAL t1,t2;
         const STATE* state1 = statesys.GetState(cn,stateindex);
         t1 = state1->Time();
         const STATE* state2 = 0;
         if ( stateindex+1 < nt )
         {
            state2 = statesys.GetState(cn,stateindex+1);
            t2     = state2->Time();
         }
         CARDINAL nx = com->NumZoneGridPts(zone);
         XY_WORK wx(nx);
         switch ( ct )
         {
            case CurveVal:
               if ( statesys.NumPar(cn) == 0 )  // without conf. int.
               {
                  for ( i=0; i<nx; i++ )  // calculate values
                  {
                     REAL x1,x2,f1,f2;
                     x1 = com->ZoneGridPt(state1->Y(),zone,i);
                     if ( com->GridValue(&varsys,&cn,&t1,state1->Y(),
                                         zone,i,var,f1) == FALSE )
                        return PROBLEM;
                     if ( state2 == 0 )
                     {
                        wx.x[i] = x1;
                        wx.y[i] = f1;
                     }
                     else
                     {
                        x2 = com->ZoneGridPt(state2->Y(),zone,i);
                        if ( com->GridValue(&varsys,&cn,&t2,state2->Y(),
                                            zone,i,var,f2) == FALSE )
                           return PROBLEM;
                        wx.x[i] = ((t2-time)*x1+(time-t1)*x2)/(t2-t1);
                        wx.y[i] = ((t2-time)*f1+(time-t1)*f2)/(t2-t1);
                     }
                     if ( cancelfun != 0 )
                     {
                        if ( cancelfun() == TRUE ) return INTERRUPT;
                     }
                  }
               }
               else   // calculate values and standard deviations
               {
                  for ( i=0; i<nx; i++ )  // calculate x-coord.
                  {
                     wx.x1[i] = com->ZoneGridPt(state1->Y(),zone,i);
                     if ( state2 != 0 )
                     {
                        wx.x2[i] = com->ZoneGridPt(state2->Y(),zone,i);
                     }
                  }
                  for ( i=0; i<statesys.NumPar(cn); i++ )  // consol. x
                  {
                     const STATE* statepar1
                            = statesys.GetState(cn,stateindex,
                                                statesys.Par(cn,i));
                     if ( statepar1 == 0 ) return PROBLEM;
                     REAL xs = com->ZoneGridPt(statepar1->Y(),zone,0);
                     REAL xe = com->ZoneGridPt(statepar1->Y(),zone,nx-1);
                     if ( wx.x1[0] < wx.x1[nx-1] )
                     {
                        if ( wx.x1[0] < xs )    wx.x1[0]    = xs;
                        if ( wx.x1[nx-1] > xe ) wx.x1[nx-1] = xe;
                     }
                     else
                     {
                        if ( wx.x1[0] > xs )    wx.x1[0]    = xs;
                        if ( wx.x1[nx-1] < xe ) wx.x1[nx-1] = xe;
                     }
                     if ( state2 != 0 )
                     {
                        const STATE* statepar2
                               = statesys.GetState(cn,stateindex+1,
                                                   statesys.Par(cn,i));
                        if ( statepar2 == 0 ) return PROBLEM;
                        REAL xs = com->ZoneGridPt(statepar2->Y(),zone,0);
                        REAL xe = com->ZoneGridPt(statepar2->Y(),zone,nx-1);
                        if ( wx.x2[0] < wx.x2[nx-1] )
                        {
                           if ( wx.x2[0] < xs )    wx.x2[0]    = xs;
                           if ( wx.x2[nx-1] > xe ) wx.x2[nx-1] = xe;
                        }
                        else
                        {
                           if ( wx.x2[0] > xs )    wx.x2[0]    = xs;
                           if ( wx.x2[nx-1] < xe ) wx.x2[nx-1] = xe;
                        }
                     }
                  }
                  if ( Consolidate(wx.x1,nx) == FALSE ) return PROBLEM;
                  if ( state2 != 0 )
                     if ( Consolidate(wx.x2,nx) == FALSE ) return PROBLEM;

                  for ( i=0; i<nx; i++ )  // calculate values
                  {
                     if ( com->SpaceValue(&varsys,&cn,&t1,state1->Y(),
                                          zone,wx.x1[i],FALSE,var,wx.y1[i])
                                                                   == FALSE )
                        return PROBLEM;
                     if ( state2 != 0 )
                     {
                        if ( com->SpaceValue(&varsys,&cn,&t2,state2->Y(),
                                             zone,wx.x2[i],FALSE,var,wx.y2[i])
                                                                   == FALSE )
                           return PROBLEM;
                     }
                     if ( cancelfun != 0 )
                     {
                        if ( cancelfun() == TRUE ) return INTERRUPT;
                     }
                  }

                  for ( CARDINAL j=0; j<statesys.NumPar(cn); j++ )  // calc. st.dev.
                  {
                     AQVAR* par = statesys.Par(cn,j);
                     const STATE* statepar1
                            = statesys.GetState(cn,stateindex,par);
                     if ( statepar1 == 0 ) return PROBLEM;
                     for ( i=0; i<nx; i++ )
                     {
                        REAL p;
                        if ( com->SpaceValue(&varsys,&cn,&t1,state1->Y(),
                                             zone,wx.x1[i],FALSE,par,p) == FALSE )
                           return PROBLEM;
                        if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                        REAL ff;
                        if ( com->SpaceValue(&varsys,&cn,&t1,statepar1->Y(),
                                             zone,wx.x1[i],FALSE,var,ff) == FALSE )
                        {
                           par->SetSensValue(FALSE); return PROBLEM;
                        }
                        REAL pp;
                        if ( com->SpaceValue(&varsys,&cn,&t1,statepar1->Y(),
                                             zone,wx.x1[i],FALSE,par,pp) == FALSE )
                        {
                           par->SetSensValue(FALSE); return PROBLEM;
                        }
                        par->SetSensValue(FALSE);
                        REAL s = 0;
                        if ( par->Type() == ConstVar )
                        {
                           CONSTVAR* cv = (CONSTVAR*)par;
                           s = cv->StDev();
                        }
                        else
                        {
                           if ( par->Type() == RealListVar )
                           {
                              REALLISTVAR* rlv = (REALLISTVAR*)par;
                              REAL arg;
                              if ( com->SpaceValue(&varsys,&cn,&t1,
                                                   state1->Y(),zone,wx.x1[i],
                                                   FALSE,
                                                   rlv->GetArg(),arg) == FALSE )
                                 return PROBLEM;
                              s = rlv->StDev(arg);
                           }
                           else
                           {
                              FatalError("AQUASYS::GetCurveData: illegal par type");
                           }
                        }
                        wx.sigma1[i] += (wx.y1[i]-ff)*(wx.y1[i]-ff)
                                       /((p-pp)*(p-pp))*s*s;
                     }   // end for ( i )
                     if ( state2 != 0 )
                     {
                        const STATE* statepar2
                               = statesys.GetState(cn,stateindex+1,par);
                        if ( statepar2 == 0 ) return PROBLEM;
                        for ( i=0; i<nx; i++ )
                        {
                           REAL p;
                           if ( com->SpaceValue(&varsys,&cn,&t2,
                                                state2->Y(),zone,wx.x2[i],
                                                FALSE,par,p) == FALSE )
                              return PROBLEM;
                           if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                           REAL ff;
                           if ( com->SpaceValue(&varsys,&cn,&t2,
                                                statepar2->Y(),zone,wx.x2[i],
                                                FALSE,var,ff) == FALSE )
                           {
                              par->SetSensValue(FALSE); return PROBLEM;
                           }
                           REAL pp;
                           if ( com->SpaceValue(&varsys,&cn,&t2,
                                                statepar2->Y(),zone,wx.x1[i],
                                                FALSE,par,pp) == FALSE )
                           {
                              par->SetSensValue(FALSE); return PROBLEM;
                           }
                           par->SetSensValue(FALSE);
                           REAL s = 0;
                           if ( par->Type() == ConstVar )
                           {
                              CONSTVAR* cv = (CONSTVAR*)par;
                              s = cv->StDev();
                           }
                           else
                           {
                              if ( par->Type() == RealListVar )
                              {
                                 REALLISTVAR* rlv = (REALLISTVAR*)par;
                                 REAL arg;
                                 if ( com->SpaceValue(&varsys,&cn,&t2,
                                                      state2->Y(),zone,wx.x2[i],
                                                      FALSE,rlv->GetArg(),arg) == FALSE )
                                    return PROBLEM;
                                 s = rlv->StDev(arg);
                              }
                              else
                              {
                                 FatalError("AQUASYS::GetCurveData: illegal par type");
                              }
                           }
                           wx.sigma2[i] += (wx.y2[i]-ff)*(wx.y2[i]-ff)
                                           /((p-pp)*(p-pp))*s*s;
                        }  // end for ( i )
                        for ( i=0; i<nx; i++ )  // interpolate in time
                        {
                           wx.x[i] = ((t2-time)*wx.x1[i]+(time-t1)*wx.x2[i])
                                     /(t2-t1);
                           wx.y[i] = ((t2-time)*wx.y1[i]+(time-t1)*wx.y2[i])
                                     /(t2-t1);
                           wx.sigma[i] = ((t2-time)*wx.sigma1[i]
                                         +(time-t1)*wx.sigma2[i])/(t2-t1);
                        }
                     }  // end if ( state2 != 0 )
                     if ( cancelfun != 0 )
                     {
                        if ( cancelfun() == TRUE ) return INTERRUPT;
                     }
                  }  // end for ( j )
                  for ( i=0; i<nx; i++ )
                  {
                     wx.sigma[i] = sqrt(wx.sigma[i]);
                  }
                  wx.errors = TRUE;
               }
               break;
            default:
            {
               for ( i=0; i<nx; i++ )   // calculate x-coord.
               {
                  wx.x1[i] = com->ZoneGridPt(state1->Y(),zone,i);
                  if ( state2 != 0 )
                  {
                     wx.x2[i] = com->ZoneGridPt(state2->Y(),zone,i);
                  }
               }
               const STATE* statepar1
                      = statesys.GetState(cn,stateindex,par);
               if ( statepar1 == 0 ) return PROBLEM;
               const STATE* statepar2 = 0;
               REAL xs = com->ZoneGridPt(statepar1->Y(),zone,0);
               REAL xe = com->ZoneGridPt(statepar1->Y(),zone,nx-1);
               if ( wx.x1[0] < wx.x1[nx-1] )
               {
                  if ( wx.x1[0] < xs )    wx.x1[0]    = xs;
                  if ( wx.x1[nx-1] > xe ) wx.x1[nx-1] = xe;
               }
               else
               {
                  if ( wx.x1[0] > xs )    wx.x1[0]    = xs;
                  if ( wx.x1[nx-1] < xe ) wx.x1[nx-1] = xe;
               }
               if ( state2 != 0 )
               {
                  statepar2 = statesys.GetState(cn,stateindex+1,par);
                  if ( statepar2 == 0 ) return PROBLEM;
                  REAL xs = com->ZoneGridPt(statepar2->Y(),zone,0);
                  REAL xe = com->ZoneGridPt(statepar2->Y(),zone,nx-1);
                  if ( wx.x2[0] < wx.x2[nx-1] )
                  {
                     if ( wx.x2[0] < xs )    wx.x2[0]    = xs;
                     if ( wx.x2[nx-1] > xe ) wx.x2[nx-1] = xe;
                  }
                  else
                  {
                     if ( wx.x2[0] > xs )    wx.x2[0]    = xs;
                     if ( wx.x2[nx-1] < xe ) wx.x2[nx-1] = xe;
                  }
               }
               if ( Consolidate(wx.x1,nx) == FALSE ) return PROBLEM;
               if ( state2 != 0 )
                  if ( Consolidate(wx.x2,nx) == FALSE ) return PROBLEM;

               for ( i=0; i<nx; i++ )
               {
                  REAL f1, p1, ff1, pp1;
                  if ( com->SpaceValue(&varsys,&cn,&t1,state1->Y(),
                                       zone,wx.x1[i],FALSE,var,f1) == FALSE )
                     return PROBLEM;
                  if ( com->SpaceValue(&varsys,&cn,&t1,state1->Y(),
                                       zone,wx.x1[i],FALSE,par,p1) == FALSE )
                     return PROBLEM;
                  if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                  if ( com->SpaceValue(&varsys,&cn,&t1,statepar1->Y(),
                                       zone,wx.x1[i],FALSE,var,ff1) == FALSE )
                  {
                     par->SetSensValue(FALSE); return PROBLEM;
                  }
                  if ( com->SpaceValue(&varsys,&cn,&t1,statepar1->Y(),
                                       zone,wx.x1[i],FALSE,par,pp1) == FALSE )
                  {
                     par->SetSensValue(FALSE); return PROBLEM;
                  }
                  par->SetSensValue(FALSE);
                  REAL f2, p2, ff2, pp2;
                  if ( state2 != 0 )
                  {
                     if ( com->SpaceValue(&varsys,&cn,&t2,state2->Y(),
                                          zone,wx.x2[i],FALSE,var,f2) == FALSE )
                        return PROBLEM;
                     if ( com->SpaceValue(&varsys,&cn,&t2,state2->Y(),
                                          zone,wx.x2[i],FALSE,par,p2) == FALSE )
                        return PROBLEM;
                     if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                     if ( com->SpaceValue(&varsys,&cn,&t2,statepar2->Y(),
                                          zone,wx.x2[i],FALSE,var,ff2) == FALSE )
                     {
                        par->SetSensValue(FALSE); return PROBLEM;
                     }
                     if ( com->SpaceValue(&varsys,&cn,&t2,statepar2->Y(),
                                          zone,wx.x2[i],FALSE,par,pp2) == FALSE )
                     {
                        par->SetSensValue(FALSE); return PROBLEM;
                     }
                     par->SetSensValue(FALSE);
                  }

                  switch ( ct )
                  {
                     case CurveSensErr:
                        {
                           REAL s1 = 0; REAL s2 = 0;
                           if ( par->Type() == ConstVar )
                           {
                              CONSTVAR* cv = (CONSTVAR*)par;
                              s1 = s2 = cv->StDev();
                           }
                           else
                           {
                              if ( par->Type() == RealListVar )
                              {
                                 REALLISTVAR* rlv = (REALLISTVAR*)par;
                                 REAL arg;
                                 if ( com->SpaceValue(&varsys,&cn,&t1,
                                                      state1->Y(),zone,
                                                      wx.x1[i],FALSE,rlv->GetArg(),
                                                      arg) == FALSE )
                                    return PROBLEM;
                                 s1 = rlv->StDev(arg);
                                 if ( state2 != 0 )
                                 {
                                    if ( com->SpaceValue(&varsys,&cn,&t2,
                                                         state2->Y(),zone,
                                                         wx.x2[i],FALSE,rlv->GetArg(),
                                                         arg) == FALSE )
                                       return PROBLEM;
                                    s2 = rlv->StDev(arg);
                                 }
                              }
                              else
                              {
                                 FatalError("AQUASYS::GetCurveData: illegal par type");
                              }
                           }
                           wx.y1[i] = (f1-ff1)/(p1-pp1)*s1;
                           if ( state2 != 0 )
                           {
                              wx.y2[i] = (f2-ff2)/(p2-pp2)*s2;
                           }
                        }
                        break;
                     case CurveSensAbsAbs:
                        wx.y1[i] = (f1-ff1)/(p1-pp1);
                        if ( state2 != 0 )
                        {
                           wx.y2[i] = (f2-ff2)/(p2-pp2);
                        }
                        break;
                     case CurveSensRelAbs:
                        {
                           REAL a = (f1-ff1)/(p1-pp1);
                           if ( a == 0.0 )
                           {
                              wx.y1[i] = 0.0;
                           }
                           else
                           {
                              if ( fabs(a) > MaxSensRel*fabs(f1) )
                              {
                                 wx.y1[i] = 2*MaxSensRel;
                              }
                              else
                              {
                                 wx.y1[i] = a/f1;
                              }
                           }
                           if ( state2 != 0 )
                           {
                              a = (f2-ff2)/(p2-pp2);
                              if ( a == 0.0 )
                              {
                                 wx.y2[i] = 0.0;
                              }
                              else
                              {
                                 if ( fabs(a) > MaxSensRel*fabs(f2) )
                                 {
                                    wx.y2[i] = 2*MaxSensRel;
                                 }
                                 else
                                 {
                                    wx.y2[i] = a/f2;
                                 }
                              }
                           }
                        }
                        break;
                     case CurveSensAbsRel:
                        wx.y1[i] = p1*(f1-ff1)/(p1-pp1);
                        if ( state2 != 0 )
                        {
                           wx.y2[i] = p2*(f2-ff2)/(p2-pp2);
                        }
                        break;
                     case CurveSensRelRel:
                        {
                           REAL a = p1*(f1-ff1)/(p1-pp1);
                           if ( a == 0.0 )
                           {
                              wx.y1[i] = 0.0;
                           }
                           else
                           {
                              if ( fabs(a) > MaxSensRel*fabs(f1) )
                              {
                                 wx.y1[i] = 2*MaxSensRel;
                              }
                              else
                              {
                                 wx.y1[i] = a/f1;
                              }
                           }
                           if ( state2 != 0 )
                           {
                              a = p2*(f2-ff2)/(p2-pp2);
                              if ( a == 0.0 )
                              {
                                 wx.y2[i] = 0.0;
                              }
                              else
                              {
                                 if ( fabs(a) > MaxSensRel*fabs(f2) )
                                 {
                                    wx.y2[i] = 2*MaxSensRel;
                                 }
                                 else
                                 {
                                    wx.y2[i] = a/f2;
                                 }
                              }
                           }
                        }
                        break;
                     default:
                        FatalError("AQUASYS::GetCurveData: illegal curve type");
                        break;
                  }  // end switch ( ct )

                  if ( cancelfun != 0 )
                  {
                     if ( cancelfun() == TRUE ) return INTERRUPT;
                  }
               }  // end for ( i )

               if ( state2 != 0 )
               {
                  for ( i=0; i<nx; i++ )  // interpolate in time
                  {
                     wx.x[i] = ((t2-time)*wx.x1[i]+(time-t1)*wx.x2[i])/(t2-t1);
                     wx.y[i] = ((t2-time)*wx.y1[i]+(time-t1)*wx.y2[i])/(t2-t1);
                  }
               }

               if ( ct==CurveSensRelAbs || ct==CurveSensRelRel )
               {
                  for ( i=0; i<nx; i++ )
                  {
                     if ( wx.y[i] > MaxSensRel ) // look at neigh. to find sign
                     {
                        BOOLEAN positive = TRUE;
                        for ( CARDINAL j=1; j<nx; j++ )
                        {
                           if ( i+j < nx )
                           {
                              if ( wx.y[i+j] < MaxSensRel )
                              {
                                 if ( wx.y[i+j] < 0.0 ) positive = FALSE;
                                 break;
                              }
                           }
                           if ( j <= i )
                           {
                              if ( wx.y[i-j] < MaxSensRel )
                              {
                                 if ( wx.y[i-j] < 0.0 ) positive = FALSE;
                                 break;
                              }
                           }
                        }
                        if ( positive == TRUE ) wx.y[i] = MaxSensRel;
                        else                    wx.y[i] = -MaxSensRel;
                     }
                  }
               }
            }
            break;   // end default
         }  // end switch ( ct )
         *n = nx;
         *x = new REAL[*n];
         *y = new REAL[*n];
         for ( i=0; i<*n; i++ )
         {
            (*x)[i] = wx.x[i];
            (*y)[i] = wx.y[i];
         }
         if ( wx.errors==TRUE && yminus!=0 && yplus!=0 )
         {
            *yminus = new REAL[*n];
            *yplus  = new REAL[*n];
            for ( i=0; i<*n; i++ )
            {
               (*yminus)[i] = wx.y[i]-wx.sigma[i];
               (*yplus)[i]  = wx.y[i]+wx.sigma[i];
            }
         }
      }
      break;
   }
   return OK;
}


JOBSTATUS AQUASYS::GetAllCurvesData(PLOT* plot, PLOTDATA* plotdata)
{
   CARDINAL i,j;
   if ( plot == 0 ) return PROBLEM;
   if ( plotdata == 0 ) return PROBLEM;
   plotdata->del();
   if ( plot->NumCurves() == 0 ) return PROBLEM;
   plotdata->nc = plot->NumCurves();
   plotdata->nd = 0;
   plotdata->iscurve = new BOOLEAN[plotdata->nc];
   plotdata->npts    = new CARDINAL[plotdata->nc];
   plotdata->x       = new REAL*[plotdata->nc];
   plotdata->y       = new REAL*[plotdata->nc];
   plotdata->yminus  = new REAL*[plotdata->nc];
   plotdata->yplus   = new REAL*[plotdata->nc];
   for ( i=0; i<plotdata->nc; i++ )
   {
      plotdata->iscurve[i] = FALSE;
      plotdata->npts[i] = 0;
      plotdata->x[i] = 0; plotdata->y[i] = 0;
      plotdata->yminus[i] = 0; plotdata->yplus[i] = 0;
   }
   for ( i=0; i<plotdata->nc; i++ )
   {
      JOBSTATUS status = GetCurveData(plot->AbscType(),plot->Curve(i),
                                      &plotdata->npts[i],
                                      &plotdata->x[i],&plotdata->y[i],
                                      &plotdata->yminus[i],
                                      &plotdata->yplus[i]);
      switch ( status )
      {
         case OK:
            plotdata->iscurve[i] = TRUE;
            if ( plotdata->nd == 0 )
            {
               plotdata->xmin = plotdata->x[i][0];
               plotdata->xmax = plotdata->xmin;
               plotdata->ymin = plotdata->y[i][0];
               plotdata->ymax = plotdata->ymin;
            }
            for ( j=0; j<plotdata->npts[i]; j++ )
            {
               if ( plotdata->xmin > plotdata->x[i][j] )
                  plotdata->xmin = plotdata->x[i][j];
               if ( plotdata->xmax < plotdata->x[i][j] )
                  plotdata->xmax = plotdata->x[i][j];
               if ( plotdata->ymin > plotdata->y[i][j] )
                  plotdata->ymin = plotdata->y[i][j];
               if ( plotdata->ymax < plotdata->y[i][j] )
                  plotdata->ymax = plotdata->y[i][j];
            }
            if ( plotdata->yminus[i] != 0 )
            {
               for ( j=0; j<plotdata->npts[i]; j++ )
               {
                  if ( plotdata->ymin > plotdata->yminus[i][j] ) 
                     plotdata->ymin = plotdata->yminus[i][j];
                  if ( plotdata->ymax < plotdata->yminus[i][j] )
                     plotdata->ymax = plotdata->yminus[i][j];
               }
            }
            if ( plotdata->yplus[i] != 0 )
            {
               for ( j=0; j<plotdata->npts[i]; j++ )
               {
                  if ( plotdata->ymin > plotdata->yplus[i][j] )
                     plotdata->ymin = plotdata->yplus[i][j];
                  if ( plotdata->ymax < plotdata->yplus[i][j] )
                     plotdata->ymax = plotdata->yplus[i][j];
               }
            }
            plotdata->nd++;
            break;
         case INTERRUPT:
            return INTERRUPT;
         case PROBLEM:
            plotdata->iscurve[i] = FALSE;
            break;
      }
   }
   if ( plotdata->nd == 0 ) return PROBLEM;

   REAL Min, Max, Pos, Dist;

   PlotLimits(plotdata->xMin(),plotdata->xMax(),&Min,&Max,&Pos,&Dist);
   if ( plot->xMinAuto() == FALSE )      Min  = plot->xMin();
   if ( plot->xMaxAuto() == FALSE )      Max  = plot->xMax();
   if ( plot->xTickPosAuto() == FALSE )  Pos  = plot->xTickPos();
   if ( plot->xTickDistAuto() == FALSE ) Dist = plot->xTickDist();
   plot->xMinMax(Min,Max);
   plot->xTick(Pos,Dist);

   PlotLimits(plotdata->yMin(),plotdata->yMax(),&Min,&Max,&Pos,&Dist);
   if ( plot->yMinAuto() == FALSE )      Min  = plot->yMin();
   if ( plot->yMaxAuto() == FALSE )      Max  = plot->yMax();
   if ( plot->yTickPosAuto() == FALSE )  Pos  = plot->yTickPos();
   if ( plot->yTickDistAuto() == FALSE ) Dist = plot->yTickDist();
   plot->yMinMax(Min,Max);
   plot->yTick(Pos,Dist);

   return OK;
}


BOOLEAN AQUASYS::GetResult(AQVAR* var, CARDINAL calcnum,
                           AQCOMP* com, CARDINAL zoneind,
                           REAL time, REAL space, BOOLEAN spacerel,
                           REAL& value, AQVAR* par)
{
   CARDINAL cn = calcnum;
   CARDINAL nt = statesys.NumStates(cn);
   if ( var == 0 )                                    return FALSE;
   if ( com == 0 )                                    return FALSE;
   if ( nt == 0 )                                     return FALSE;
   if ( time < (statesys.GetState(cn,0))->Time() )    return FALSE;
   if ( time > (statesys.GetState(cn,nt-1))->Time() ) return FALSE;
   CARDINAL stateindex = 0;
   if ( nt > 1 )
   {
      while ( time > (statesys.GetState(cn,stateindex+1))->Time() )
      {
         stateindex++;
         if ( stateindex+1 >= nt )
            FatalError("AQUASYS::GetResult: illegal state");
      }
   }
   REAL t1,t2;
   REAL val1, val2;
   const STATE* state1;
   if ( par == 0 ) state1 = statesys.GetState(cn,stateindex);
   else            state1 = statesys.GetState(cn,stateindex,par);
   t1 = state1->Time();
   BOOLEAN res1 = compsys.SpaceValue(&varsys,&cn,&t1,state1->Y(),
                                     com,zoneind,space,spacerel,
                                     var,val1);
   if ( res1 == FALSE )                               return FALSE;
   value = val1;
   const STATE* state2 = 0;
   if ( stateindex+1 < nt )
   {
      if ( par == 0 ) state2 = statesys.GetState(cn,stateindex+1);
      else            state2 = statesys.GetState(cn,stateindex+1,par);
      t2     = state2->Time();
      BOOLEAN res2 = compsys.SpaceValue(&varsys,&cn,&t2,state2->Y(),
                                        com,zoneind,space,spacerel,
                                        var,val2);
      if ( res2 == FALSE )                               return FALSE;
      value = ((t2-time)*val1+(time-t1)*val2)/(t2-t1);
   }
   return TRUE;
}

	  
JOBSTATUS AQUASYS::ListResults(std::ostream& out, PLOT* plot)
{
   if ( plot == 0 ) return PROBLEM;
   out.precision(PlotLisOpt()->Precision());
   CARDINAL fields = 0;
   CARDINAL field1 = ini.L(4); if ( field1 < 5 ) field1 = 5;
   CARDINAL pos1   = 0;
   char     sep[2];  sep[1] = '\0';
   switch ( PlotLisOpt()->Separator() )
   {
      case SepTab:
         sep[0] = '\t';
         break;
      case SepComma:
         sep[0] = ',';
         break;
      case SepSpace:
         sep[0] = '\0';
         fields = PlotLisOpt()->Precision()+8;
         pos1   = field1 + 1;
         break;
   }
   PrintWidth(0);

   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( Print(out,ProgName()) == FALSE )                      return PROBLEM;
   if ( Print(out," ") == FALSE )                             return PROBLEM;
   if ( Print(out,ProgVersion()) == FALSE )                   return PROBLEM;
   if ( Print(out," - ") == FALSE )                           return PROBLEM;
   if ( Print(out,ini.T(33)) == FALSE )                       return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintTime(out,ini.T(101)) == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( Print(out,ini.T(102)) == FALSE )                      return PROBLEM;
   if ( Print(out,": ") == FALSE )                            return PROBLEM;
   if ( Print(out,plot->Symbol()) == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;

   PLOTDATA data;
   JOBSTATUS status = GetAllCurvesData(plot,&data);
   if ( status != OK )                                        return status;

   char Buf[BufSize];
   CARDINAL i,j;
   CARDINAL ndatamax = 0;
   CARDINAL ncol     = 1;                 // Legend column

   // count number of columns and maximum number of data points:

   for ( i=0; i<data.NumCurveDef(); i++ )
   {
      if ( data.IsCurve(i) == TRUE )
      {
         ncol += 2;
         if ( data.NumPts(i) > ndatamax ) ndatamax = data.NumPts(i);
         if ( data.Yminus(i)!=0 && data.Yplus(i)!=0 ) ncol += 2;
      }
   }
   if ( ncol <= 1 )                                           return PROBLEM;

  // collect column headers:

   const CARDINAL numlin = 9;
   // COMPILER PROBLEM with codewarrior:
   // char*** heading = new char**[ncol];
   typedef char** charpp;
   char*** heading = new charpp[ncol];
   for ( i=0; i<ncol; i++ )
   {
      heading[i] = new char*[numlin];
      for ( j=0; j<numlin; j++ ) heading[i][j] = 0;
   }
   heading[0][0] = new char[strlen(ini.T(55))+1];         // Type
   strcpy(heading[0][0],ini.T(55));
   heading[0][1] = new char[strlen(ini.T(34))+1];         // Variable
   strcpy(heading[0][1],ini.T(34));
   heading[0][2] = new char[strlen(ini.T(56))+1];         // Parameter
   strcpy(heading[0][2],ini.T(56));
   heading[0][3] = new char[strlen(ini.T(35))+1];         // CalcNum
   strcpy(heading[0][3],ini.T(35));
   heading[0][4] = new char[strlen(ini.T(36))+1];         // Compartment
   strcpy(heading[0][4],ini.T(36));
   heading[0][5] = new char[strlen(ini.T(103))+1];        // Zone
   strcpy(heading[0][5],ini.T(103));
   heading[0][6] = new char[strlen(ini.T(54))+1];         // Time/Space
   strcpy(heading[0][6],ini.T(54));
   heading[0][7] = new char[strlen(ini.T(57))+1];         // Unit
   strcpy(heading[0][7],ini.T(57));
   heading[0][8] = new char[strlen(ini.T(62))+1];         // Legend
   strcpy(heading[0][8],ini.T(62));

   CARDINAL k = 1;
   for ( i=0; i<data.NumCurveDef(); i++ )
   {
      if ( data.IsCurve(i) == TRUE )
      {
         const CURVE* curve = plot->Curve(i);
         if ( curve == 0 )
            FatalError("AQUASYS::ListResults: curve not found");
         AQVAR*  arg = 0;
         AQVAR*  var = curve->Variable();
         AQVAR*  par = curve->Parameter();
         AQCOMP* com = curve->Compartment();
         if ( var==0 || com==0 )
            FatalError("AQUASYS::ListResults: var or comp not found");

         heading[k][0] = new char[strlen(ini.T(104))+1];  // Type
         strcpy(heading[k][0],ini.T(104));
         
         switch ( curve->Type() )
         {
            case CurveVal:
               strcpy(Buf,ini.T(58));
               break;
            case CurveSensErr:
               strcpy(Buf,ini.T(59));
               break;
            case CurveSensAbsAbs:
               strcpy(Buf,ini.T(60));
               break;
            case CurveSensRelAbs:
               strcpy(Buf,ini.T(61));
               break;
            case CurveSensAbsRel:
               strcpy(Buf,ini.T(92));
               break;
            case CurveSensRelRel:
               strcpy(Buf,ini.T(93));
               break;
         }
         heading[k+1][0] = new char[strlen(Buf)+1];
         strcpy(heading[k+1][0],Buf);

         switch ( plot->AbscType() )                         // Variable
         {
            case AbscTime:
               arg = varsys.GetProgVar(Time);
               break;
            case AbscSpace:
               arg = com->SpaceVar(&varsys);
               break;
         }
         if ( arg != 0 )
         {
            heading[k][1] = new char[strlen(arg->Symbol())+1];
            strcpy(heading[k][1],arg->Symbol());
         }
         heading[k+1][1] = new char[strlen(var->Symbol())+1];
         strcpy(heading[k+1][1],var->Symbol());

         if ( par != 0 )                                     // Parameter
         {
            heading[k+1][2] = new char[strlen(par->Symbol())+1];
            strcpy(heading[k+1][2],par->Symbol());
         }

         NumToString(curve->CalcNum(),Buf,BufSize);          // CalcNum
         heading[k+1][3] = new char[strlen(Buf)+1];
         strcpy(heading[k+1][3],Buf);

         heading[k+1][4]                                     // Compartment
            = new char[strlen(com->Symbol())+1];
         strcpy(heading[k+1][4],com->Symbol());

         heading[k+1][5]                                     // Zone
            = new char[strlen(com->ZoneName(curve->Zone()))+1];
         strcpy(heading[k+1][5],com->ZoneName(curve->Zone()));

         NumToString(curve->XorT(),Buf,BufSize,              // Time/Space
                     0,PlotLisOpt()->Precision());
         heading[k+1][6] = new char[strlen(Buf)+1];
         strcpy(heading[k+1][6],Buf);

         if ( arg != 0 )                                     // Unit
         {
            if ( arg->Unit() != 0 )
            {
               heading[k][7] = new char[strlen(arg->Unit())+1];
               strcpy(heading[k][7],arg->Unit());
            }
         }
         switch ( curve->Type() )
         {
            case CurveVal:
               if ( var->Unit() != 0 )
               {
                  strcpy(Buf,var->Unit());
               }
               break;
            case CurveSensErr: case CurveSensAbsAbs:
               if ( var->Unit() != 0 )
               {
                  strcpy(Buf,var->Unit());
               }
               if ( par->Unit() != 0 )
               {
                  if ( strlen(Buf) == 0 ) strcpy(Buf,"1/(");
                  else                       strcat(Buf,"/(");
                  strcat(Buf,par->Unit());
                  strcat(Buf,")");
               }
               break;
            case CurveSensRelAbs:
               if ( par->Unit() != 0 )
               {
                  strcpy(Buf,"1/(");
                  strcat(Buf,par->Unit());
                  strcat(Buf,")");
               }
               break;
            case CurveSensAbsRel:
               if ( var->Unit() != 0 )
               {
                  strcpy(Buf,var->Unit());
               }
               break;
            case CurveSensRelRel:
               break;
         }
         heading[k+1][7] = new char[strlen(Buf)+1];
         strcpy(heading[k+1][7],Buf);

         if ( curve->Legend() != 0 )                         // Legend
         {
            heading[k+1][8] = new char[strlen(curve->Legend())+1];
            strcpy(heading[k+1][8],curve->Legend());
         }

         k += 2;

         if ( data.Yminus(i)!=0 && data.Yplus(i)!=0 )
         {
            heading[k][0] = new char[strlen(ini.T(85))+1];
            strcpy(heading[k][0],ini.T(85));
            heading[k+1][0] = new char[strlen(ini.T(86))+1];
            strcpy(heading[k+1][0],ini.T(86));
            for ( j=1; j<8; j++ )
            {
               if ( heading[k-1][j] != 0 )
               {
                  heading[k][j]   = new char[strlen(heading[k-1][j])+1];
                  heading[k+1][j] = new char[strlen(heading[k-1][j])+1];
                  strcpy(heading[k][j],  heading[k-1][j]);
                  strcpy(heading[k+1][j],heading[k-1][j]);
               }
            }
            k += 2;
         }
      }
   }
   if ( k != ncol )
      FatalError("AQUASYS::ListResults: inconsistent number of columns");

   // shorten headers if necessary:

   if ( PlotLisOpt()->Separator() == SepSpace )
   {
      for ( i=0; i<ncol; i++ )
      {
         if ( heading[i][0] != 0 )
         {
            if ( strlen(heading[i][0]) > field1-2 )
               heading[i][0][field1-2] = '\0';
         }
         for ( j=1; j<numlin; j++ )
         {
            if ( heading[i][j] != 0 )
            {
               if ( strlen(heading[i][j]) > fields-1 )
                  heading[i][j][fields-1] = '\0';
            }
         }
      }
   }

   // write headers:

   for ( j=0; j<numlin; j++ )
   {
      if ( Print(out,heading[0][j],1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      for ( i=1; i<ncol; i++ )
      {
         if ( Print(out,sep) == FALSE )                       return PROBLEM;
         if ( heading[i][j] != 0 )
         {
            if ( Print(out,heading[i][j],pos1+(i-1)*fields) == FALSE )
                                                              return PROBLEM;
         }
      }
      if ( PrintLn(out) == FALSE )                            return PROBLEM;
   }

   // delete collected headers:

   for ( i=0; i<ncol; i++ )
   {
      for ( j=0; j<numlin; j++ ) delete [] heading[i][j];
      delete [] heading[i];
   }
   delete [] heading;

   // write data:

   for ( j=0; j<ndatamax; j++ )
   {
      k = 1;
      for ( i=0; i<data.NumCurveDef(); i++ )
      {
         if ( data.IsCurve(i) == TRUE )
         {
            if ( j<data.NumPts(i) )
            {
               if ( Print(out,sep) == FALSE )                 return PROBLEM;
               if ( Print(out,data.X(i)[j],pos1+(k-1)*fields) == FALSE )
                                                              return PROBLEM;
               if ( Print(out,sep) == FALSE )                 return PROBLEM;
               if ( Print(out,data.Y(i)[j],pos1+k*fields) == FALSE )
                                                              return PROBLEM;
            }
            else
            {
               if ( Print(out,sep) == FALSE )                 return PROBLEM;
               if ( Print(out,sep) == FALSE )                 return PROBLEM;
            }
            k += 2;

            if ( data.Yminus(i)!=0 && data.Yplus(i)!=0 )
            {
               if ( j<data.NumPts(i) )
               {
                  if ( Print(out,sep) == FALSE )              return PROBLEM;
                  if ( Print(out,data.Yminus(i)[j],pos1+(k-1)*fields)
                                                   == FALSE ) return PROBLEM;
                  if ( Print(out,sep) == FALSE )              return PROBLEM;
                  if ( Print(out,data.Yplus(i)[j],pos1+k*fields)
                                                   == FALSE ) return PROBLEM;
               }
               else
               {
                  if ( Print(out,sep) == FALSE )              return PROBLEM;
                  if ( Print(out,sep) == FALSE )              return PROBLEM;
               }
               k += 2;
            }
         }
      }
      if ( PrintLn(out) == FALSE )                            return PROBLEM;
   }

   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;

   return OK;
}


BOOLEAN AQUASYS::DtMax(REAL dtmax)
{
   if ( numpar.DtMax(dtmax) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::MaxOrd(CARDINAL maxord)
{
   if ( numpar.MaxOrd(maxord) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


void AQUASYS::NumCoDiag(CARDINAL numco)
{
   numpar.NumCoDiag(numco);
   Changed();
   return;
}


BOOLEAN AQUASYS::MaxNumSteps(CARDINAL maxnumsteps)
{
   if ( numpar.MaxNumSteps(maxnumsteps) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::GridTimeConst(REAL gridrelax)
{
   if ( numpar.GridTimeConst(gridrelax) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::FitTol(REAL tol)
{
   if ( numpar.FitTol(tol) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


void AQUASYS::Interpolate(BOOLEAN b)
{
   numpar.Interpolate(b);
   Changed();
   return;
}


void AQUASYS::FitMeth(FITMETHOD meth)
{
   if ( fitmeth == meth ) return;
   fitmeth = meth;
   Changed();
   return;
}


void AQUASYS::FitIter(CARDINAL iter)
{
   if ( iter == fititer ) return;
   fititer = iter;
   Changed();
   return;
}


void AQUASYS::PrintShort(BOOLEAN b)
{
   if ( printshort == b ) return;
   printshort = b;
   Changed();
   return;
}


JOBSTATUS AQUASYS::InitCalc(CALC* calc, BOOLEAN writelog, AQVAR* par)
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::InitCalc");
#endif

   if ( calc == 0 ) return PROBLEM;
   strcpy(calcname,calc->Symbol());
   char Buf[BufSize];
   cjob->precision(ini.I(3));
   FORMVAR::ResetIllegalValCount();

   REAL cn = (REAL)calc->CalcNum();
   varsys.SetProgVar(CalcNumber,&cn);
   linksys.SetLinkIndex(&varsys);

   NUMINFO numinfo;
   t = calc->InitTime();
   numeq = compsys.NumEq(); if ( numeq <= 0 ) return PROBLEM;

#ifdef HEAPTEST
HeapFileMessage("AQUASYS::InitCalc: start reallocate computing arrays");
#endif

   delete [] relacc; relacc = new REAL[numeq];
   delete [] absacc; absacc = new REAL[numeq];
   delete [] eqtype; eqtype = new EQTYPE[numeq];
   delete [] Y;      Y      = new REAL[numeq];
   delete [] YT;     YT     = new REAL[numeq];

#ifdef HEAPTEST
HeapFileMessage("AQUASYS::InitCalc: end   reallocate computing arrays");
#endif

   compsys.RelAcc(numeq,relacc);
   compsys.AbsAcc(numeq,absacc);
   compsys.EqType(numeq,eqtype);

   if ( writelog == TRUE )
   {
      const char* title = 0;
      if ( calc->InitSteady() == FALSE ) title = ini.T(10);      // ConsistInit
      else                               title = ini.T(11);             // SteadyInit
      *cjob << "\n\n\n" << title << "\n"; 
      Space(*cjob,'-',strlen(title));
      *cjob << "\n\n   " << ini.T(12) << ":";                   // NumCoDiag
      Space(*cjob,' ',space-strlen(ini.T(12)));
      *cjob << numpar.NumCoDiag();
      *cjob << "\n   " << ini.T(13) << ":";                     // InitTime
      Space(*cjob,' ',space-strlen(ini.T(13)));
      *cjob << t;
      TimeToString(ini.T(14),Buf,BufSize);                      // StartCalc
      *cjob << "\n\n   " << Buf << "  " << ini.T(15);
      cjob->flush();
   }

   JOBSTATUS status = OK;
   if ( par == 0 )
   {
      statesys.DeleteStates(calc->CalcNum());
   }
   else
   {
      if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
   }
   CARDINAL loccalcnum = calc->CalcNum();
   SetGlobalState(&varsys,&loccalcnum,&t,Y);
   if ( compsys.InitCond(&varsys,t,numeq,Y) == FALSE ) status = PROBLEM;
   if ( status == OK )
   {
      for (CARDINAL i=0; i<numeq; i++ ) YT[i] = 0.0;
      aquacalcsys = this;
      status = DasInit(rhsfun,t,Y,YT,relacc,absacc,eqtype,
                       numeq,calc->InitSteady(),numpar,numinfo);
      aquacalcsys = 0;
   }
   if ( status == OK )
   {
      if ( statesys.AddState(calc->CalcNum(),numeq,t,Y,YT,par) == FALSE )
      {
         status = PROBLEM;
      }
      else
      {
         Ycalcnum = calc->CalcNum();
         init  = TRUE;
         start = TRUE;
         senspar = par;
         Changed();
      }
   }
   if ( par != 0 )
   {
      if ( par->SensValue() == FALSE )
         FatalError("AQUASYS::Calculate: SensValue erraneously turned off");
      par->SetSensValue(FALSE);
   }
   if ( writelog == TRUE )
   {
      *cjob << "\n   ";                                         // EndCalc
      TimeToString(ini.T(14),Buf,BufSize);
      *cjob << Buf << "  " << ini.T(16);
      if ( status == INTERRUPT )
      {
         *cjob << "\n\n   *** " << ini.T(17) << " ***";         // Interrupt
      }
      if ( status == PROBLEM )
      {
         *cjob << "\n\n   *** " << ini.T(18) << " ***";         // Problem
      }
      *cjob << "\n\n   " << ini.T(19) << ":";                   // NumEq
      Space(*cjob,' ',space-strlen(ini.T(19)));
      *cjob << numeq;
      *cjob << "\n   " << ini.T(20) << ":";                     // NumInteger
      Space(*cjob,' ',space-strlen(ini.T(20)));
      *cjob << numinfo.NumInteger();
      *cjob << "\n   " << ini.T(21) << ":";                    // NumReal
      Space(*cjob,' ',space-strlen(ini.T(21)));
      *cjob << numinfo.NumReal();
      *cjob << "\n\n   " << ini.T(22) << ":";                   // NumStep
      Space(*cjob,' ',space-strlen(ini.T(22)));
      *cjob << numinfo.NumStep();
      *cjob << "\n   " << ini.T(24) << ":";                     // NumJac
      Space(*cjob,' ',space-strlen(ini.T(24)));
      *cjob << numinfo.NumJac();
      cjob->flush();
   }

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::InitCalc");
#endif

   return status;
}


JOBSTATUS AQUASYS::Calculate(CALC* calc, BOOLEAN writelog, AQVAR* par)
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::Calculate");
#endif

   if ( calc == 0 ) return PROBLEM;
   strcpy(calcname,calc->Symbol());
   char Buf[BufSize];
   cjob->precision(ini.I(3));
   linksys.SetLinkIndex(&varsys);

   NUMINFO numinfo;
   if ( (init==FALSE) || (calc->CalcNum()!=Ycalcnum) )
   {
      FORMVAR::ResetIllegalValCount();

      // delete states if sensitivity analysis was performed:
      if ( statesys.NumPar(calc->CalcNum()) != 0 ) DeleteStates(calc->CalcNum());

      const STATE* state
         = statesys.GetState(calc->CalcNum(),statesys.NumStates(calc->CalcNum())-1);
      if ( state != 0 )  // init. calc. with previously calculated state
      {
         if ( statesys.NumEq() != compsys.NumEq() )
            FatalError("AQUASYS::Calculate: Inconsistent number of equations");
         numeq = compsys.NumEq();

#ifdef HEAPTEST
HeapFileMessage("AQUASYS::Calculate: start reallocate computing arrays");
#endif

         delete [] relacc; relacc = new REAL[numeq];
         delete [] absacc; absacc = new REAL[numeq];
         delete [] eqtype; eqtype = new EQTYPE[numeq];
         delete [] Y;      Y      = new REAL[numeq];
         delete [] YT;     YT     = new REAL[numeq];

#ifdef HEAPTEST
HeapFileMessage("AQUASYS::Calculate: end   reallocate computing arrays");
#endif

         compsys.RelAcc(numeq,relacc);
         compsys.AbsAcc(numeq,absacc);
         compsys.EqType(numeq,eqtype);
         t = state->Time();
         for ( CARDINAL i=0; i<numeq; i++ )
         {
            Y[i]  = state->Y()[i];
            YT[i] = state->YT()[i];
         }
         Ycalcnum = calc->CalcNum();
         init  = TRUE;
         start = TRUE;
      }
      else   // initialize calculation
      {
         JOBSTATUS status = InitCalc(calc,writelog,par);
         if ( status != OK ) return status;
      }
   }
   else
   {
      if ( par != senspar ) return PROBLEM;
   }
   BOOLEAN s = start;

   if ( writelog == TRUE )
   {
      *cjob << "\n\n\n" << ini.T(27) << "\n";                 // DynCalc
      Space(*cjob,'-',strlen(ini.T(27)));
      if ( s == TRUE )
      {
         *cjob << "\n\n   " << ini.T(12) << ":";                // NumCoDiag
         Space(*cjob,' ',space-strlen(ini.T(12)));
         *cjob << numpar.NumCoDiag();
         *cjob << "\n   " << ini.T(28) << ":";                  // DtMax
         Space(*cjob,' ',space-strlen(ini.T(28)));
         *cjob << numpar.DtMax();
      }
      TimeToString(ini.T(14),Buf,BufSize);                      // StartCalc
      *cjob << "\n\n   "<< Buf << "  " << ini.T(15);
      TimeToString(ini.T(14),Buf,BufSize);                  // IntegrationAt
      *cjob << "\n   " << Buf << "  " << ini.T(29) << " " << t;
      cjob->flush();
   }

   if ( par != 0 )
   {
      if ( par->SetSensValue(TRUE) == FALSE ) return PROBLEM;
   }
   JOBSTATUS status = PROBLEM;
   REAL cn = (REAL)Ycalcnum;
   SetGlobalState(&varsys,&Ycalcnum,&t,Y);
   varsys.SetProgVar(CalcNumber,&cn);
   aquacalcsys = this;
   if ( calc->NumStepSeries() == 0 )
   {
      *cjob << "\n   Dynamic simulation problem:"
            << " No output time steps specified";
      cjob->flush();
      return PROBLEM;
   }
   for ( CARDINAL i=0; i<calc->NumStepSeries(); i++ )
   {
      for ( CARDINAL j=0; j<calc->NumSteps(i); j++ )
      {
         REAL tout = t+calc->StepSize(i);
         if ( tout <= t )
         {
             if ( par != 0 ) par->SetSensValue(FALSE); return PROBLEM;
         }
         status = DasGear(rhsfun,t,Y,YT,relacc,absacc,numeq,tout,
                          start,cjob,numpar,numinfo);
         if ( status == PROBLEM )
         {
            start = TRUE; break;
         }
         else
         {
            statesys.AddState(Ycalcnum,numeq,t,Y,YT,par);
            Changed();     
            start = FALSE;
            if ( writelog == TRUE )
            {
               TimeToString(ini.T(14),Buf,BufSize);          // IntegrationAt
               *cjob << "\n   " << Buf << "  " << ini.T(29) << " " << t;
               cjob->flush();
            }
            if ( status == INTERRUPT ) break;
         }
      }
      if ( status != OK ) break;
   }
   aquacalcsys = 0;
   if ( par != 0 )
   {
      if ( par->SensValue() == FALSE )
         FatalError("AQUASYS::Calculate: SensValue erraneously turned off");
      par->SetSensValue(FALSE);
   }

   if ( writelog == TRUE )
   {
      TimeToString(ini.T(14),Buf,BufSize);                      // EndCalc
      *cjob << "\n   " << Buf << "  " << ini.T(16);
      if ( status == INTERRUPT )
      {
         *cjob << "\n\n   *** " << ini.T(17) << " ***";         // Interrupt
      }
      if ( status == PROBLEM )
      {
         *cjob << "\n\n   *** " << ini.T(18) << " ***";         // Problem
      }
      if ( s == TRUE )
      {
         *cjob << "\n\n   " << ini.T(19) << ":";                // NumEq
         Space(*cjob,' ',space-strlen(ini.T(19)));
         *cjob << numeq;
         *cjob << "\n   " << ini.T(20) << ":";                  // NumInteger
         Space(*cjob,' ',space-strlen(ini.T(20)));
         *cjob << numinfo.NumInteger();
         *cjob << "\n   " << ini.T(21) << ":";                  // NumReal
         Space(*cjob,' ',space-strlen(ini.T(21)));
         *cjob << numinfo.NumReal();
      }
      *cjob << "\n\n   " << ini.T(30) << ":";                   // ActTime
      Space(*cjob,' ',space-strlen(ini.T(30)));
      *cjob << numinfo.ActT();
      *cjob << "\n   " << ini.T(31) << ":";                     // ActDt
      Space(*cjob,' ',space-strlen(ini.T(31)));
      *cjob << numinfo.ActDt();
      *cjob << "\n   " << ini.T(32) << ":";                     // ActOrd
      Space(*cjob,' ',space-strlen(ini.T(32)));
      *cjob << numinfo.ActOrd();
      *cjob << "\n   " << ini.T(22) << ":";                     // NumStep
      Space(*cjob,' ',space-strlen(ini.T(22)));
      *cjob << numinfo.NumStep();
      *cjob << "\n   " << ini.T(23) << ":";                     // NumFun
      Space(*cjob,' ',space-strlen(ini.T(23)));
      *cjob << numinfo.NumFun();
      *cjob << "\n   " << ini.T(24) << ":";                     // NumJac
      Space(*cjob,' ',space-strlen(ini.T(24)));
      *cjob << numinfo.NumJac();
      *cjob << "\n   " << ini.T(25) << ":";                     // NumErrFail
      Space(*cjob,' ',space-strlen(ini.T(25)));
      *cjob << numinfo.NumErrFail();
      *cjob << "\n   " << ini.T(26) << ":";                     // NumConvFail
      Space(*cjob,' ',space-strlen(ini.T(26)));
      *cjob << numinfo.NumConvFail();
      cjob->flush();
   }

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::Calculate");
#endif

   return status;
}


JOBSTATUS AQUASYS::InitSim(BOOLEAN writelog)
{
   CALC* calc = calcsys.Calclist()->First();
   JOBSTATUS status = PROBLEM;
   CARDINAL numsim = 0;
   while ( calc != 0 )
   {
      if ( calc->SimActive() == TRUE )
      {
         numsim++;
         status = InitCalc(calc,writelog);
         if ( status != OK ) break;
      }
      calc = calc->Next();
   }
   if ( numsim == 0 )
   {
      *cjob << "\n   Simulation problem:"
            << " No active calculation";
      cjob->flush();
   }
   return status;
}


JOBSTATUS AQUASYS::Simulate(BOOLEAN writelog)
{
   CALC* calc = calcsys.Calclist()->First();
   JOBSTATUS status = PROBLEM;
   CARDINAL numsim = 0;
   while ( calc != 0 )
   {
      if ( calc->SimActive() == TRUE )
      {
         numsim++;
         status = Calculate(calc,writelog);
         if ( status != OK ) break;
      }
      calc = calc->Next();
   }
   if ( numsim == 0 )
   {
      *cjob << "\n   Simulation problem:"
            << " No active calculation";
      cjob->flush();
   }
   return status;
}


JOBSTATUS AQUASYS::SensAnal(BOOLEAN writelog)
{
   char Buf[BufSize];
   cjob->precision(ini.I(3));

   if ( writelog == TRUE )
   {
      *cjob << "\n\n\n" << ini.T(81) << "\n";                // SensAnal
      Space(*cjob,'-',strlen(ini.T(81)));
   }

   // count and collect active parameters:

   CARDINAL npar  = 0;
   AQVAR* var = (varsys.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->SensActive() == TRUE ) npar++;
      var = var->Next();
   }
   if ( npar == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(89) << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   AQVAR** par = new AQVAR*[npar];
   CARDINAL i = 0;
   var = (varsys.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->SensActive() == TRUE )
      {
         par[i] = var; i++;
      }
      var = var->Next();
   }
   if ( i != npar )
      FatalError("AQUASYS::SensAnal: inconsistent number of parameters");

   // count calculations:

   CARDINAL ncalc = 0;
   CALC* calc = (calcsys.Calclist())->First();  // loop over sens. anal.
   while ( calc != 0 )
   {
      if ( calc->SensActive() == TRUE ) ncalc++;
      calc = calc->Next();
   }
   if ( ncalc == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(90) << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   if ( writelog == TRUE )
   {
      TimeToString(ini.T(14),Buf,BufSize);                    // StartCalc
      *cjob << "\n\n   " << Buf << "  " << ini.T(15);
      cjob->flush();
   }
   calc = (calcsys.Calclist())->First();  // loop over sens. anal.
   while ( calc != 0 )
   {
      if ( calc->SensActive() == TRUE )
      {
         if ( writelog == TRUE )
         {
            TimeToString(ini.T(14),Buf,BufSize);
            *cjob << "\n\n   " << ini.T(82) << " " << calc->Symbol()
                  << ":\n   " << Buf << "  " << ini.T(83);
            cjob->flush();
         }
         if ( sensoutfun != 0 ) sensoutfun();
         JOBSTATUS status = InitCalc(calc,FALSE,0);
         switch ( status )
         {
            case PROBLEM:
               *cjob << "\n\n   *** " 
                     << "Initialization problem for basic simulation"
                     << "\n   "
                     << "for calculation number " 
                     << calc->CalcNum();
               cjob->flush();
               delete [] par;
               return status;
            case INTERRUPT:
               *cjob << "\n\n   *** " << ini.T(17) << " ***";
               cjob->flush();
               delete [] par;
               return status;
            default:
               break;
         }
         status = Calculate(calc,FALSE,0);
         switch ( status )
         {
            case PROBLEM:
               *cjob << "\n   " 
                     << "Integration problem during basic simulation"
                     << "\n   "
                     << "for calculation number " 
                     << calc->CalcNum();
               cjob->flush();
               delete [] par;
               return status;
            case INTERRUPT:
               *cjob << "\n\n   *** " << ini.T(17) << " ***";
               cjob->flush();
               delete [] par;
               return status;
            default:
               break;
         }
         for ( i=0; i<npar; i++ )   // loop over par.
         {
            if ( writelog == TRUE )
            {
               TimeToString(ini.T(14),Buf,BufSize);
               *cjob << "\n   " << Buf << "  " << ini.T(84) << " "
                     << par[i]->Symbol();
               cjob->flush();
            }
            if ( sensoutfun != 0 ) sensoutfun();
            status = InitCalc(calc,FALSE,par[i]);
            switch ( status )
            {
               case PROBLEM:
                  *cjob << "\n   " 
                        << "Integration problem during initialization with incremented parameter"
               	        << "\n   "
                        << par[i]->Symbol()
                        << " for calculation number " 
                        << calc->CalcNum();
                  cjob->flush();
                  delete [] par;
                  return status;
               case INTERRUPT:
                  *cjob << "\n\n   *** " << ini.T(17) << " ***";
                  cjob->flush();
                  delete [] par;
                  return status;
               default:
                  break;
            }
            status = Calculate(calc,FALSE,par[i]);
            switch ( status )
            {
               case PROBLEM:
                  *cjob << "\n   " 
                        << "Integration problem during simulation with incremented parameter"
               	        << "\n   "
                        << par[i]->Symbol()
                        << " for calculation number " 
                        << calc->CalcNum();
                  cjob->flush();
                  delete [] par;
                  return status;
               case INTERRUPT:
                  *cjob << "\n\n   *** " << ini.T(17) << " ***";
                  cjob->flush();
                  delete [] par;
                  return status;
               default:
                  break;
            }
         }
      }
      calc = calc->Next();
   }
   if ( writelog == TRUE )
   {
      TimeToString(ini.T(14),Buf,BufSize);                    // EndCalc
      *cjob << "\n\n   " << Buf << "  " << ini.T(16);
      cjob->flush();
   }
   delete [] par;

   init = FALSE;   // new calculation requires initialization

   return OK;
}


JOBSTATUS AQUASYS::PrintRankHeader(std::ostream* out, AQVAR* var, CARDINAL num)
{
   if ( out == 0 )                               return PROBLEM;
   if ( var != 0 )
   {
      if ( Print(*out,ini.T(53),7) == FALSE )    return PROBLEM;
      if ( Print(*out,": ") == FALSE )           return PROBLEM;
      if ( Print(*out,var->Symbol()) == FALSE )  return PROBLEM;
   }
   else
   {
      if ( Print(*out,ini.T(136),3) == FALSE )   return PROBLEM;
      if ( Print(*out,": ") == FALSE )           return PROBLEM;
   }
   if ( Print(*out,"  (averages over ") == FALSE ) return PROBLEM;
   if ( Print(*out,num) == FALSE )               return PROBLEM;
   if ( Print(*out," values)") == FALSE )        return PROBLEM;
   if ( PrintLn(*out) == FALSE )                 return PROBLEM;
   if ( Print(*out,"\t\t") == FALSE )            return PROBLEM;
   if ( Print(*out,ini.T(94)) == FALSE )         return PROBLEM;
   if ( Print(*out,":") == FALSE )               return PROBLEM;
   if ( Print(*out,"\tr(av(") == FALSE )         return PROBLEM;
   if ( Print(*out,ini.T(95)) == FALSE )         return PROBLEM;
   if ( Print(*out,"^2)):") == FALSE )           return PROBLEM;
   if ( Print(*out,"\tav(|") == FALSE )          return PROBLEM;
   if ( Print(*out,ini.T(95)) == FALSE )         return PROBLEM;
   if ( Print(*out,"|):") == FALSE )             return PROBLEM;
   if ( Print(*out,"\tav(") == FALSE )           return PROBLEM;
   if ( Print(*out,ini.T(95)) == FALSE )         return PROBLEM;
   if ( Print(*out,"):") == FALSE )              return PROBLEM;
   if ( Print(*out,"\tmax(|") == FALSE )         return PROBLEM;
   if ( Print(*out,ini.T(95)) == FALSE )         return PROBLEM;
   if ( Print(*out,"|):") == FALSE )             return PROBLEM;
   if ( Print(*out,"\t") == FALSE )              return PROBLEM;
   if ( Print(*out,ini.T(94)) == FALSE )         return PROBLEM;
   if ( Print(*out,":") == FALSE )               return PROBLEM;
   if ( Print(*out,"\tr(av(") == FALSE )         return PROBLEM;
   if ( Print(*out,ini.T(96)) == FALSE )         return PROBLEM;
   if ( Print(*out,"^2)):") == FALSE )           return PROBLEM;
   if ( Print(*out,"\tav(|") == FALSE )          return PROBLEM;
   if ( Print(*out,ini.T(96)) == FALSE )         return PROBLEM;
   if ( Print(*out,"|):") == FALSE )             return PROBLEM;
   if ( Print(*out,"\tav(") == FALSE )           return PROBLEM;
   if ( Print(*out,ini.T(96)) == FALSE )         return PROBLEM;
   if ( Print(*out,"):") == FALSE )              return PROBLEM;
   if ( Print(*out,"\tmax(|") == FALSE )         return PROBLEM;
   if ( Print(*out,ini.T(96)) == FALSE )         return PROBLEM;
   if ( Print(*out,"|):") == FALSE )             return PROBLEM;
   if ( PrintLn(*out) == FALSE )                 return PROBLEM;
   if ( var != 0 )
   {
      if ( var->Unit() != 0 )
      {
         if ( Print(*out,"\t\t\t[") == FALSE )   return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t[") == FALSE )      return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t[") == FALSE )      return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t[") == FALSE )      return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t\t[") == FALSE )    return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t[") == FALSE )      return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t[") == FALSE )      return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]\t[") == FALSE )      return PROBLEM;
         if ( Print(*out,var->Unit()) == FALSE ) return PROBLEM;
         if ( Print(*out,"]") == FALSE )         return PROBLEM;
      }
      else
      {
         if ( Print(*out,"\t\t\t[]\t[]\t[]\t[]\t\t[]\t[]\t[]\t[]") == FALSE )
                                                 return PROBLEM;
      }
   }
   else
   {
      if ( Print(*out,"\t\t\t[]\t[]\t[]\t[]\t\t[]\t[]\t[]\t[]") == FALSE )
                                                 return PROBLEM;
   }
   if ( PrintLn(*out) == FALSE )                 return PROBLEM;
   out->flush();
   return OK;
}


JOBSTATUS AQUASYS::PrintSensHeader(std::ostream* out,
                                   CARDINAL numpars, AQVAR** par)
{
   if ( out == 0 ) return PROBLEM;
   CARDINAL ipar;
   *out << "variable"    << '\t'
        << "calcnum"     << '\t'
        << "compartment" << '\t'
        << "zone"        << '\t'
        << "time"        << '\t'
        << "space"       << '\t'
        << "abs/rel"     << '\t'
        << "scale"       << '\t'
        << "value"       << '\t'
        << "parameter:";
   for ( ipar=0; ipar<numpars; ipar++ ) *out << '\t' << par[ipar]->Symbol();
   *out << "\n\t\t\t\t\t\t\t\t\tvalue:";
   for ( ipar=0; ipar<numpars; ipar++ )
   {
      if ( par[ipar]->Type() == ConstVar )
      {
         CONSTVAR* cv = (CONSTVAR*)par[ipar];
         *out << '\t' << cv->Evaluate();
      }
      else
      {
         *out << '\t' << "reallist";
      }
   }
   *out << "\n\t\t\t\t\t\t\t\t\tstd.dev.:";
   for ( ipar=0; ipar<numpars; ipar++ )
   {
      if ( par[ipar]->Type() == ConstVar )
      {
         CONSTVAR* cv = (CONSTVAR*)par[ipar];
         *out << '\t' << cv->StDev();
      }
      else
      {
         *out << '\t' << "reallist";
      }
   }
   *out << "\n\t\t\t\t\t\t\t\t\t\tabsolute sensitivities";
   out->flush();

   return OK;
}


JOBSTATUS AQUASYS::SensRank(BOOLEAN writelog,
							std::ostream* rankout, std::ostream* sensout,
							std::istream* cmdfile)
{
   if ( writelog == TRUE )
   {
      *cjob << "\n\n\n" << "Sensitivity Ranking" << "\n";         // SensRank
      Space(*cjob,'-',strlen("Sensitivity Ranking"));
   }
   if ( rankout == 0 ) return PROBLEM;
   char Buf[BufSize];
   if ( sensoutfun != 0 ) sensoutfun();
   if ( writelog == TRUE )
   {
      TimeToString(ini.T(14),Buf,BufSize);
      *cjob << "\n\n   " << Buf << "  " << ini.T(128);
      cjob->flush();
   }
   rankout->precision(4);
   if ( sensout != 0 ) sensout->precision(ini.I(4));
   CARDINAL field = 12;
   if ( PrintLn(*rankout,'*') == FALSE )                   return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( Print(*rankout,ProgName()) == FALSE )              return PROBLEM;
   if ( Print(*rankout," ") == FALSE )                     return PROBLEM;
   if ( Print(*rankout,ProgVersion()) == FALSE )           return PROBLEM;
   if ( Print(*rankout," - ") == FALSE )                   return PROBLEM;
   if ( Print(*rankout,"Sensitivity Analysis File") == FALSE )
                                                           return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( PrintLn(*rankout,'*') == FALSE )                   return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( PrintTime(*rankout,ini.T(105)) == FALSE )          return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;
   if ( Print(*rankout,ini.T(49)) == FALSE )               return PROBLEM;   
   if ( Print(*rankout,":") == FALSE )                     return PROBLEM;   
   if ( PrintLn(*rankout) == FALSE )                       return PROBLEM;

   if ( cmdfile == 0 ) // perform default ranking for state variables
   {
      for ( CARDINAL icalc=0; icalc<statesys.NumCalc(); icalc++ )
      {
         CARDINAL calcnum = statesys.CalcNum(icalc);
         CARDINAL numpars = statesys.NumPar(calcnum);
         if ( numpars > 0 )
         {
            AQVAR** par = new AQVAR*[numpars];
            CARDINAL ipar;
            for ( ipar=0; ipar<numpars; ipar++ ) 
            {
               par[ipar] = statesys.Par(calcnum,ipar)  ;
            }
            if ( writelog == TRUE )
            {
               TimeToString(ini.T(14),Buf,BufSize);
               *cjob << "\n   " << Buf << "  " 
                     << ini.T(130) << ": " << calcnum;
               cjob->flush();
            }
            PrintSensHeader(sensout,numpars,par);
            if ( PrintLn(*rankout) == FALSE )                 return PROBLEM;
            if ( PrintLn(*rankout) == FALSE )                 return PROBLEM;
            if ( Print(*rankout,ini.T(50)) == FALSE )         return PROBLEM;
            if ( Print(*rankout,": ") == FALSE )              return PROBLEM;
            if ( Print(*rankout,calcnum) == FALSE )           return PROBLEM;
            CARDINAL statestartind = 0;
            if ( statesys.NumStates(calcnum) > 2 )
            {
               REAL t1 = statesys.GetState(calcnum,0)->Time();
               REAL t2 = statesys.GetState(calcnum,1)->Time();
               REAL t3 = statesys.GetState(calcnum,2)->Time();
               if ( t2-t1 > 5*(t3-t2) )
               {
                  statestartind = 1;
                  if ( Print(*rankout,"  (") == FALSE )       return PROBLEM;
                  if ( Print(*rankout,ini.T(127)) == FALSE )  return PROBLEM;
                  if ( Print(*rankout,")") == FALSE )         return PROBLEM;
               }
            }
            rankout->flush();
            AQCOMP* com = compsys.Complist()->First();
            while ( com != 0 )
            {
               if ( com->Active() == TRUE )
               {
                  if ( writelog == TRUE )
                  {
                     TimeToString(ini.T(14),Buf,BufSize);
                     *cjob << "\n   " << Buf << "  " 
                           << "  " << ini.T(131) << ": " << com->Symbol();
                     cjob->flush();
                  }
                  CARDINAL numstatevar = com->NumStateVar();
                  if ( numstatevar > 0 )
                  {
                     if ( Print(*rankout,ini.T(51),3) == FALSE )
                                                              return PROBLEM;
                     if ( Print(*rankout,": ") == FALSE )     return PROBLEM;
                     if ( Print(*rankout,com->Symbol()) == FALSE )
                                                              return PROBLEM;
                     for ( CARDINAL j=0; j<com->NumZones(); j++ )
                     {
                        if ( writelog == TRUE )
                        {
                           TimeToString(ini.T(14),Buf,BufSize);
                           *cjob << "\n   " << Buf << "  " 
                                 << "    " << ini.T(132) << ": " << com->ZoneName(j);
                           cjob->flush();
                        }
                        if ( Print(*rankout,ini.T(52),5) == FALSE )
                                                              return PROBLEM;
                        if ( Print(*rankout,": ") == FALSE )  return PROBLEM;
                        if ( Print(*rankout,com->ZoneName(j)) == FALSE )
                                                              return PROBLEM;
                        for ( CARDINAL k=0; k<numstatevar; k++ )
                        {
                           AQVAR* var = com->StateVars()[k];
                           if ( writelog == TRUE )
                           {
                              TimeToString(ini.T(14),Buf,BufSize);
                              *cjob << "\n   " << Buf << "  " 
                                    << "      " << ini.T(133) << ": " << var->Symbol();
                              cjob->flush();
                           }
                           CURVE curve;
                           if ( curve.Variable(var) == FALSE )return PROBLEM;
                           if ( curve.CalcNum(calcnum) == FALSE )
                                                              return PROBLEM;
                           if ( curve.Compartment(com) == FALSE )
                                                              return PROBLEM;
                           if ( curve.Zone(j) == FALSE )      return PROBLEM;
                           REAL*   meansqrsens = new REAL[numpars];
                           REAL*   meanabssens = new REAL[numpars];
                           REAL*   meanvalsens = new REAL[numpars];
                           REAL*   maxabssens  = new REAL[numpars];
                           REAL*   meansqrerr  = new REAL[numpars];
                           REAL*   meanabserr  = new REAL[numpars];
                           REAL*   meanvalerr  = new REAL[numpars];
                           REAL*   maxabserr   = new REAL[numpars];
                           REAL**  sensvalues  = new REAL*[numpars];
                           REAL*   varvalues   = 0;
                           REAL*   xvalues     = 0;
                           CARDINAL ispace; 
                           for ( ipar=0; ipar<numpars; ipar++ )
                           {
                              meansqrsens[ipar] = 0;
                              meanabssens[ipar] = 0;
                              meanvalsens[ipar] = 0;
                              maxabssens[ipar]  = 0;
                              meansqrerr[ipar]  = 0;
                              meanabserr[ipar]  = 0;
                              meanvalerr[ipar]  = 0;
                              maxabserr[ipar]   = 0;
                              sensvalues[ipar]  = 0;
                           }
                           CARDINAL numvals = 0;
                           for ( CARDINAL itime = statestartind; 
                                 itime<statesys.NumStates(calcnum); itime++ )
                           {
                              REAL time = 0;
                              CARDINAL n; REAL* x; REAL* val;
                              CARDINAL nspace = 1;
                              if ( curve.Parameter(par[0]) == FALSE )
                                                            return PROBLEM;
                              time = statesys.GetState(calcnum,itime)->Time();
                              if ( curve.XorT(time) == FALSE )
                                                            return PROBLEM;
                              JOBSTATUS status;
                              if ( curve.Type(CurveVal) == FALSE )
                                                            return PROBLEM;
                              status =  GetCurveData(AbscSpace,&curve,&n,&x,&val);
                              switch ( status )
                              {
                                 case PROBLEM:
                                    *cjob << "\n\n   *** "
                                          << "Problem evaluating variable value"
                                          << " ***";
                                    cjob->flush();
                                    delete [] sensvalues;
                                    delete [] meansqrsens;
                                    delete [] meanabssens;
                                    delete [] meanvalsens;
                                    delete [] maxabssens;
                                    delete [] meansqrerr;						
                                    delete [] meanabserr;						
                                    delete [] meanvalerr;						
                                    delete [] maxabserr;						
                                    delete [] par;
                                    return status;
                                 case INTERRUPT:
                                    *cjob << "\n\n   *** " << ini.T(17) << " ***";
                                    cjob->flush();
                                    delete [] sensvalues;
                                    delete [] sensvalues;
                                    delete [] meansqrsens;
                                    delete [] meanabssens;
                                    delete [] meanvalsens;
                                    delete [] maxabssens;
                                    delete [] meansqrerr;						
                                    delete [] meanabserr;						
                                    delete [] meanvalerr;						
                                    delete [] maxabserr;						
                                    delete [] par;
                                    return status;
                                 default:
                                    break;
                              }
                              nspace = n;
                              numvals += nspace;
                              xvalues = new REAL[nspace];
                              varvalues = new REAL[nspace];
                              for ( ispace=0; ispace<n; ispace++ )
                              {
                                 varvalues[ispace] = val[ispace];
                                 xvalues[ispace]   = x[ispace];
                              }
                              delete [] x;
                              delete [] val;
                              for ( ipar=0; ipar<numpars; ipar++ )
                              {
                                 if ( curve.Parameter(par[ipar]) == FALSE )
                                                            return PROBLEM;
                                 if ( curve.Type(CurveSensAbsAbs) == FALSE )
                                                            return PROBLEM;
                                 status =  GetCurveData(AbscSpace,&curve,&n,&x,&val);
                                 switch ( status )
                                 {
                                    case PROBLEM:
                                       *cjob << "\n\n   *** "
                                             << "Problem evaluating sensitvity function"
                                             << " ***";
                                       cjob->flush();
                                       delete [] sensvalues;
                                       delete [] varvalues;
                                       delete [] varvalues;
                                       delete [] xvalues;
                                       delete [] sensvalues;
                                       delete [] meansqrsens;
                                       delete [] meanabssens;
                                       delete [] meanvalsens;
                                       delete [] maxabssens;
                                       delete [] meansqrerr;						
                                       delete [] meanabserr;						
                                       delete [] meanvalerr;						
                                       delete [] maxabserr;						
                                       delete [] par;
                                       return status;
                                    case INTERRUPT:
                                       *cjob << "\n\n   *** " << ini.T(17) << " ***";
                                       cjob->flush();
                                       delete [] sensvalues;
                                       delete [] varvalues;
                                       delete [] xvalues;
                                       delete [] meansqrsens;
                                       delete [] meanabssens;
                                       delete [] meanvalsens;
                                       delete [] maxabssens;
                                       delete [] meansqrerr;						
                                       delete [] meanabserr;						
                                       delete [] meanvalerr;						
                                       delete [] maxabserr;						
                                       delete [] par;
                                       return status;
                                    default:
                                       break;
                                 }
                                 if ( n != nspace )
                                 {
                                    *cjob << "\n\n   *** "
                                          << "Problem evaluating sensitvity function"
                                          << " ***";
                                    cjob->flush();
                                    delete [] sensvalues;
                                    delete [] varvalues;
                                    delete [] xvalues;
                                    delete [] meansqrsens;
                                    delete [] meanabssens;
                                    delete [] meanvalsens;
                                    delete [] maxabssens;
                                    delete [] meansqrerr;						
                                    delete [] meanabserr;						
                                    delete [] meanvalerr;						
                                    delete [] maxabserr;						
                                    delete [] par;
                                    return PROBLEM;
                                 }
                                 sensvalues[ipar] = new REAL[nspace];
                                 for ( ispace=0; ispace<n; ispace++ )
                                 {
                                    sensvalues[ipar][ispace] = val[ispace];
                                 }
                                 if ( par[ipar]->Type() == ConstVar )
                                 {
                                    CONSTVAR* cv = (CONSTVAR*)par[ipar];
                                    for ( ispace=0; ispace<nspace; ispace++ )
                                    {
                                       REAL term = cv->Evaluate()*val[ispace];
                                       meansqrsens[ipar] += term*term;
                                       meanabssens[ipar] += fabs(term);
                                       meanvalsens[ipar] += term;
                                       if ( fabs(term) > maxabssens[ipar] )
                                          maxabssens[ipar] = fabs(term);
                                       term = cv->StDev()*val[ispace];
                                       meansqrerr[ipar]  += term*term;
                                       meanabserr[ipar]  += fabs(term);
                                       meanvalerr[ipar]  += term;
                                       if ( fabs(term) > maxabserr[ipar] )
                                          maxabserr[ipar] = fabs(term);
                                    }
                                    delete [] x;
                                    delete [] val;
                                 }
                                 else
                                 {
                                    if ( curve.Type(CurveSensAbsRel) == FALSE )
                                                           return PROBLEM;
                                    status =  GetCurveData(AbscSpace,&curve,&n,&x,&val);
                                    switch ( status )
                                    {
                                       case PROBLEM:
                                          *cjob << "\n\n   *** "
                                                << "Problem evaluating sensitvity function"
                                                << " ***";
                                          cjob->flush();
                                          for ( ipar=0; ipar<numpars; ipar++ ) delete [] sensvalues[ipar];
                                          delete [] sensvalues;
                                          delete [] varvalues;
                                          delete [] xvalues;
                                          delete [] meansqrsens;
                                          delete [] meanabssens;
                                          delete [] meanvalsens;
                                          delete [] maxabssens;
                                          delete [] meansqrerr;						
                                          delete [] meanabserr;						
                                          delete [] meanvalerr;						
                                          delete [] maxabserr;						
                                          delete [] par;
                                          return status;
                                       case INTERRUPT:
                                          *cjob << "\n\n   *** " << ini.T(17) << " ***";
                                          cjob->flush();
                                          for ( ipar=0; ipar<numpars; ipar++ ) delete [] sensvalues[ipar];
                                          delete [] sensvalues;
                                          delete [] varvalues;
                                          delete [] xvalues;
                                          delete [] meansqrsens;
                                          delete [] meanabssens;
                                          delete [] meanvalsens;
                                          delete [] maxabssens;
                                          delete [] meansqrerr;						
                                          delete [] meanabserr;						
                                          delete [] meanvalerr;						
                                          delete [] maxabserr;						
                                          delete [] par;
                                          return status;
                                       default:
                                          break;
                                    }
                                    for ( ispace=0; ispace<nspace; ispace++ )
                                    {
                                       meansqrsens[ipar] += val[ispace]*val[ispace];
                                       meanabssens[ipar] += fabs(val[ispace]);
                                       meanvalsens[ipar] += val[ispace];
                                       if ( fabs(val[ispace]) > maxabssens[ipar] )
                                          maxabssens[ipar] = fabs(val[ispace]);
                                    }
                                    delete [] x;
                                    delete [] val;
                                    if ( curve.Type(CurveSensErr) == FALSE )
                                                           return PROBLEM;
                                    status = GetCurveData(AbscSpace,&curve,&n,&x,&val);
                                    switch ( status )
                                    {
                                       case PROBLEM:
                                          *cjob << "\n\n   *** "
                                                << "Problem evaluating sensitvity function"
                                                << " ***";
                                          cjob->flush();
                                          for ( ipar=0; ipar<numpars; ipar++ ) delete [] sensvalues[ipar];
                                          delete [] sensvalues;
                                          delete [] varvalues;
                                          delete [] xvalues;
                                          delete [] meansqrsens;
                                          delete [] meanabssens;
                                          delete [] meanvalsens;
                                          delete [] maxabssens;
                                          delete [] meansqrerr;						
                                          delete [] meanabserr;						
                                          delete [] meanvalerr;						
                                          delete [] maxabserr;						
                                          delete [] par;
                                          return status;
                                       case INTERRUPT:
                                          *cjob << "\n\n   *** " << ini.T(17) << " ***";
                                          cjob->flush();
                                          for ( ipar=0; ipar<numpars; ipar++ ) delete [] sensvalues[ipar];
                                          delete [] sensvalues;
                                          delete [] varvalues;
                                          delete [] xvalues;
                                          delete [] meansqrsens;
                                          delete [] meanabssens;
                                          delete [] meanvalsens;
                                          delete [] maxabssens;
                                          delete [] meansqrerr;						
                                          delete [] meanabserr;						
                                          delete [] meanvalerr;						
                                          delete [] maxabserr;						
                                          delete [] par;
                                          return status;
                                       default:
                                          break;
                                    }
                                    for ( ispace=0; ispace<nspace; ispace++ )
                                    {
                                       meansqrerr[ipar] += val[ispace]*val[ispace];
                                       meanabserr[ipar] += fabs(val[ispace]);
                                       meanvalerr[ipar] += val[ispace];
                                       if ( fabs(val[ispace]) > maxabserr[ipar] )
                                          maxabserr[ipar] = fabs(val[ispace]);
                                    }
                                    delete [] x;
                                    delete [] val;
                                 }
                                 if ( cancelfun != 0 )
                                 {
                                    if ( cancelfun() == TRUE )
                                    {
                                       for ( ipar=0; ipar<numpars; ipar++ ) delete [] sensvalues[ipar];
                                       delete [] sensvalues;
                                       delete [] varvalues;
                                       delete [] xvalues;
                                       delete [] meansqrsens;
                                       delete [] meanabssens;
                                       delete [] meanvalsens;
                                       delete [] maxabssens;
                                       delete [] meansqrerr;						
                                       delete [] meanabserr;						
                                       delete [] meanvalerr;						
                                       delete [] maxabserr;						
                                       delete [] par;
                                       return INTERRUPT;
                                    }
                                 }
                              } // end for ipar=0; ipar<npar
                              if ( sensout != 0 )
                              {
                                 for ( ispace=0; ispace<nspace; ispace++ )
                                 {
                                    *sensout << '\n';
                                    *sensout << var->Symbol() << '\t'
                                             << calcnum << '\t'
                                             << com->Symbol() << '\t'
                                             << com->ZoneName(j) << '\t'
                                             << time << '\t'
                                             << xvalues[ispace] << '\t'
                                             << "a" << '\t'
                                             << "1" << '\t'
                                             << varvalues[ispace] << '\t';
                                    for ( ipar=0; ipar<numpars; ipar++ )
                                    {
                                       *sensout << '\t' << sensvalues[ipar][ispace];
                                    }
                                 }
                                 sensout->flush();
                              }
                              for ( ipar=0; ipar<numpars; ipar++ )
                              {
                                 delete sensvalues[ipar];
                              }
                              delete varvalues;
                              delete xvalues;
                           } // end itime=statestartind; itime<...
                           delete sensvalues;
                           for ( ipar=0; ipar<numpars; ipar++ )
                           {
                              meansqrsens[ipar] = sqrt(meansqrsens[ipar]/numvals);
                              meanabssens[ipar] = meanabssens[ipar]/numvals;
                              meanvalsens[ipar] = meanvalsens[ipar]/numvals;
                              meansqrerr[ipar]  = sqrt(meansqrerr[ipar]/numvals);
                              meanabserr[ipar]  = meanabserr[ipar]/numvals;
                              meanvalerr[ipar]  = meanvalerr[ipar]/numvals;
                           }
                           CARDINAL* indsens = new CARDINAL[numpars];
                           CARDINAL* inderr  = new CARDINAL[numpars];
                           sort(numpars,meansqrsens,indsens);
                           sort(numpars,meansqrerr ,inderr);
		 
                           JOBSTATUS status
                              = PrintRankHeader(rankout,var,numvals);
                           if ( status != OK )                return status;
                           for ( ipar=0; ipar<numpars; ipar++ )
                           {
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,ipar+1) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,par[indsens[numpars-1-ipar]]->Symbol()) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meansqrsens[indsens[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanabssens[indsens[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanvalsens[indsens[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,maxabssens[indsens[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,par[inderr[numpars-1-ipar]]->Symbol()) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meansqrerr[inderr[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanabserr[inderr[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanvalerr[inderr[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,maxabserr[inderr[numpars-1-ipar]]) == FALSE )
                                                           return PROBLEM;
                              if ( PrintLn(*rankout) == FALSE ) return PROBLEM;
                           }
                           delete [] meansqrsens;
                           delete [] meanabssens;
                           delete [] meanvalsens;
                           delete [] maxabssens;
                           delete [] meansqrerr;						
                           delete [] meanabserr;						
                           delete [] meanvalerr;						
                           delete [] maxabserr;						
                           delete [] indsens;						
                           delete [] inderr;						
                        } // end for k=0; k<numstatevar
                     }	// end for j=0; j<com->NumZones()
                  } // end if numstatevar > 0
               } // end if com->Active()==TRUE
               com = com->Next();
            } // end while com!=0
            delete [] par;
            if ( sensout != 0 ) *sensout << "\n\n";
         } // end if numpars > 0
      } // end for icalc=0; icalc<statesys.NumCalc()
   }
   else // cmdfile != 0: read output data points from command file
   {
      // read command file
      CARDINAL  size = 10;
      CARDINAL  numdef    = 0;
      CARDINAL  maxdef    = size;
      AQVAR**   def_var   = new AQVAR*[maxdef];
      CARDINAL* def_cn    = new CARDINAL[maxdef];
      AQCOMP**  def_com   = new AQCOMP*[maxdef];
      CARDINAL* def_zind  = new CARDINAL[maxdef];
      REAL*     def_time  = new REAL[maxdef];
      REAL*     def_space = new REAL[maxdef];
      BOOLEAN*  def_srel  = new BOOLEAN[maxdef];
      REAL*     def_sig   = new REAL[maxdef];
      while ( cmdfile->getline(Buf,sizeof(Buf)) )
      {
         // check heap allocation:
         if ( numdef == maxdef )
         {
            AQVAR**   newdef_var   = new AQVAR*[maxdef+size];
            CARDINAL* newdef_cn    = new CARDINAL[maxdef+size];
            AQCOMP**  newdef_com   = new AQCOMP*[maxdef+size];
            CARDINAL* newdef_zind  = new CARDINAL[maxdef+size];
            REAL*     newdef_time  = new REAL[maxdef+size];
            REAL*     newdef_space = new REAL[maxdef+size];
            BOOLEAN*  newdef_srel  = new BOOLEAN[maxdef+size];
            REAL*     newdef_sig   = new REAL[maxdef+size];
            for ( CARDINAL i=0; i<numdef; i++ )
            {
               newdef_var[i]   = def_var[i];
               newdef_cn[i]    = def_cn[i];
               newdef_com[i]   = def_com[i];
               newdef_zind[i]  = def_zind[i];
               newdef_time[i]  = def_time[i];
               newdef_space[i] = def_space[i];
               newdef_srel[i]  = def_srel[i];
               newdef_sig[i]   = def_sig[i];
            }
            delete def_var;
            delete def_cn;
            delete def_com;
            delete def_zind;
            delete def_time;
            delete def_space;
            delete def_srel;
            delete def_sig;
            def_var   = newdef_var;
            def_cn    = newdef_cn;
            def_com   = newdef_com;
            def_zind  = newdef_zind;
            def_time  = newdef_time;
            def_space = newdef_space;
            def_srel  = newdef_srel;
            def_sig   = newdef_sig;
            maxdef += size;
         } // end heap allocation
         
         //	parse input line:

         CARDINAL numarg = 8;
         char** args = new char*[numarg];
         ParseArgs(numarg,args,Buf);
         def_var[numdef] = GetVar(args[0]);
         if ( def_var[numdef] != 0 )
         {
            if ( StringToNum(args[1],&def_cn[numdef]) == TRUE )
            {
               def_com[numdef] = GetComp(args[2]);
               if ( def_com[numdef] != 0 )
               {
                  if ( StringToNum(args[3],&def_zind[numdef]) == TRUE ) 
                  {
                     if ( StringToNum(args[4],&def_time[numdef]) == TRUE ) 
                     {
                        if ( StringToNum(args[5],&def_space[numdef]) == TRUE )
                        {
                           if ( args[6][0] == 'r' || args[6][0] == 'R' ) def_srel[numdef] = TRUE;
                           else                                          def_srel[numdef] = FALSE;
                           if ( StringToNum(args[7],&def_sig[numdef]) == FALSE ) def_sig[numdef] = 1;
                           numdef++;
                        }
                     }
                  }
               }
            }
         }
         for ( CARDINAL i=0; i<numarg; i++ ) delete args[i];
         delete [] args;
      } // end while ( cmdfile->getline(Buf,sizeof(Buf)) )
      if ( writelog == TRUE )
      {
         TimeToString(ini.T(14),Buf,BufSize);
         *cjob << "\n   " << Buf << "  " 
               << "Number of output definitions read" << ": " << numdef;
         cjob->flush();
      }

      // count calculation numbers

      CARDINAL idef;
      CARDINAL numcalc = 0;
      CARDINAL maxcalc = size;
      CARDINAL* calcnums = new CARDINAL[maxcalc];
      for ( idef = 0; idef<numdef; idef++ )
      {
         // check heap allocation:
         if ( numcalc == maxcalc )
         {
            CARDINAL* newcalcnums = new CARDINAL[maxcalc+size];
            for ( CARDINAL i=0; i<numcalc; i++ )
            {
               newcalcnums[i] = calcnums[i];
            }
            delete calcnums;
            calcnums = newcalcnums;
            maxcalc += size;
		 } // end heap allocation

         // check existence
         CARDINAL k = 0;
         for ( CARDINAL i=0; i<numcalc; i++ )
         {
            if ( def_cn[idef] == calcnums[i] ) break;
            k++;
         }
         if ( k == numcalc ) // new calcnum
         {
			 calcnums[numcalc] = def_cn[idef];
             numcalc++;
         }
      }

      // loop for calculation numbers:

      for ( CARDINAL icalc=0; icalc<numcalc; icalc++ )
      {
         CARDINAL calcnum = calcnums[icalc];
         CARDINAL numpars = statesys.NumPar(calcnum);
         if ( numpars > 0 )
         {
            REAL* meansqrsensglobal = new REAL[numpars];
            REAL* meanabssensglobal = new REAL[numpars];
            REAL* meanvalsensglobal = new REAL[numpars];
            REAL* maxabssensglobal  = new REAL[numpars];
            REAL* meansqrerrglobal  = new REAL[numpars];
            REAL* meanabserrglobal  = new REAL[numpars];
            REAL* meanvalerrglobal  = new REAL[numpars];
            REAL* maxabserrglobal   = new REAL[numpars];
            CARDINAL ipar;
            for ( ipar=0; ipar<numpars; ipar++ )
            {
               meansqrsensglobal[ipar] = 0;
               meanabssensglobal[ipar] = 0;
               meanvalsensglobal[ipar] = 0;
               maxabssensglobal[ipar]  = 0;
               meansqrerrglobal[ipar]  = 0;
               meanabserrglobal[ipar]  = 0;
               meanvalerrglobal[ipar]  = 0;
               maxabserrglobal[ipar]   = 0;
            }
            CARDINAL numvalglobal = 0;
            AQVAR** par = new AQVAR*[numpars];
            for ( ipar=0; ipar<numpars; ipar++ ) 
            {
               par[ipar] = statesys.Par(calcnum,ipar)  ;
            }
            if ( writelog == TRUE )
            {
               TimeToString(ini.T(14),Buf,BufSize);
               *cjob << "\n   " << Buf << "  " 
                     << ini.T(130) << ": " << calcnum;
               cjob->flush();
            }
            PrintSensHeader(sensout,numpars,par);
            if ( PrintLn(*rankout) == FALSE )                 return PROBLEM;
            if ( PrintLn(*rankout) == FALSE )                 return PROBLEM;
            if ( Print(*rankout,ini.T(50)) == FALSE )         return PROBLEM;
            if ( Print(*rankout,": ") == FALSE )              return PROBLEM;
            if ( Print(*rankout,calcnum) == FALSE )           return PROBLEM;

            // count compartments

            CARDINAL icomp;
            CARDINAL numcomp = 0;
            CARDINAL maxcomp = size;
            AQCOMP** comps = new AQCOMP*[maxcomp];
            for ( idef = 0; idef<numdef; idef++ )
            {
               if ( def_cn[idef] == calcnum )
               {
                  // check heap allocation:
                  if ( numcomp == maxcomp )
                  {
                     AQCOMP** newcomps = new AQCOMP*[maxcomp+size];
                     for ( icomp=0; icomp<numcomp; icomp++ )
                     {
                        newcomps[icomp] = comps[icomp];
                     }
                     delete comps;
                     comps = newcomps;
                     maxcomp += size;
                  } // end heap allocation

                  // check existence
                  CARDINAL k = 0;
                  for ( icomp=0; icomp<numcomp; icomp++ )
                  {
                     if ( def_com[idef] == comps[icomp] ) break;
                     k++;
                  }
                  if ( k == numcomp ) // new compartment
                  {
                     comps[numcomp] = def_com[idef];
                     numcomp++;
                  }
               }
            }

            // loop for compartments:

            for ( icomp=0; icomp<numcomp; icomp++ )
            {
               AQCOMP* com = comps[icomp];
               if ( com->Active() == TRUE )
               {
                  if ( writelog == TRUE )
                  {
                     TimeToString(ini.T(14),Buf,BufSize);
                     *cjob << "\n   " << Buf << "  " 
                           << "  " << ini.T(131) << ": " << com->Symbol();
                     cjob->flush();
                  }
                  if ( Print(*rankout,ini.T(51),3) == FALSE )
                                                           return PROBLEM;
                  if ( Print(*rankout,": ") == FALSE )     return PROBLEM;
                  if ( Print(*rankout,com->Symbol()) == FALSE )
                                                           return PROBLEM;

                  // count zones

                  CARDINAL izone;
                  CARDINAL numzone = 0;
                  CARDINAL maxzone = size;
                  CARDINAL* zones = new CARDINAL[maxzone];
                  for ( idef = 0; idef<numdef; idef++ )
                  {
                     if ( def_cn[idef]==calcnum && def_com[idef]==com )
                     {
                        // check heap allocation:
                        if ( numzone == maxzone )
                        {
                           CARDINAL* newzones = new CARDINAL[maxzone+size];
                           for ( izone=0; izone<numzone; izone++ )
                           {
                              newzones[izone] = zones[izone];
                           }
                           delete zones;
                           zones = newzones;
                           maxzone += size;
                        } // end heap allocation

                        // check existence
                        CARDINAL k = 0;
                        for ( izone=0; izone<numzone; izone++ )
                        {
                           if ( def_zind[idef] == zones[izone] ) break;
                           k++;
                        }
                        if ( k == numzone ) // new zone
                        {
                           zones[numzone] = def_zind[idef];
                           numzone++;
                        }
                     }
                  }

                  // loop for zones:

                  for ( izone=0; izone<numzone; izone++ )
                  {
                     CARDINAL zone = zones[izone];
                     if ( zone < com->NumZones() )
                     {
                        if ( writelog == TRUE )
                        {
                           TimeToString(ini.T(14),Buf,BufSize);
                           *cjob << "\n   " << Buf << "  " 
                                 << "    " << ini.T(132) << ": " << com->ZoneName(zone);
                           cjob->flush();
                        }
                        if ( Print(*rankout,ini.T(52),5) == FALSE )
                                                              return PROBLEM;
                        if ( Print(*rankout,": ") == FALSE )  return PROBLEM;
                        if ( Print(*rankout,com->ZoneName(zone)) == FALSE )
                                                              return PROBLEM;

                        // count variables

                        CARDINAL ivar;
                        CARDINAL numvar = 0;
                        CARDINAL maxvar = size;
                        AQVAR** vars = new AQVAR*[maxvar];
                        for ( idef = 0; idef<numdef; idef++ )
                        {
                           if ( def_cn[idef]==calcnum && def_com[idef]==com
                                && def_zind[idef]==zone )
                           {
                              // check heap allocation:
                              if ( numvar == maxvar )
                              {
                                 AQVAR** newvars = new AQVAR*[maxvar+size];
                                 for ( ivar=0; ivar<numvar; ivar++ )
                                 {
                                    newvars[ivar] = vars[ivar];
                                 }
                                 delete vars;
                                 vars = newvars;
                                 maxvar += size;
                              } // end heap allocation

                              // check existence
                              CARDINAL k = 0;
                              for ( ivar=0; ivar<numvar; ivar++ )
                              {
                                 if ( def_var[idef] == vars[ivar] ) break;
                                 k++;
                              }
                              if ( k == numvar ) // new variable
                              {
                                 vars[numvar] = def_var[idef];
                                 numvar++;
                              }
                           }
                        }

                        // loop for variables:

                        for ( ivar=0; ivar<numvar; ivar++ )
                        {
                           AQVAR* var = vars[ivar];

                           if ( writelog == TRUE )
                           {
                              TimeToString(ini.T(14),Buf,BufSize);
                              *cjob << "\n   " << Buf << "  " 
                                    << "      " << ini.T(133) << ": " << var->Symbol();
                              cjob->flush();
                           }
                           REAL* meansqrsens = new REAL[numpars];
                           REAL* meanabssens = new REAL[numpars];
                           REAL* meanvalsens = new REAL[numpars];
                           REAL* maxabssens  = new REAL[numpars];
                           REAL* meansqrerr  = new REAL[numpars];
                           REAL* meanabserr  = new REAL[numpars];
                           REAL* meanvalerr  = new REAL[numpars];
                           REAL* maxabserr   = new REAL[numpars];
                           for ( ipar=0; ipar<numpars; ipar++ )
                           {
                              meansqrsens[ipar] = 0;
                              meanabssens[ipar] = 0;
                              meanvalsens[ipar] = 0;
                              maxabssens[ipar]  = 0;
                              meansqrerr[ipar]  = 0;
                              meanabserr[ipar]  = 0;
                              meanvalerr[ipar]  = 0;
                              maxabserr[ipar]   = 0;
                           }
                           CARDINAL numval = 0;
                           for ( idef=0; idef<numdef; idef++ )
                           {
                              if (    def_cn[idef]==calcnum && def_com[idef]==com 
                                   && def_zind[idef]==zone  && def_var[idef]==var )
                              {
                                 REAL time = def_time[idef];
                                 REAL space = def_space[idef];
                                 BOOLEAN spacerel = def_srel[idef];
                                 REAL f;
                                 REAL* ff = new REAL[numpars];
                                 REAL* p  = new REAL[numpars];
                                 REAL* pp = new REAL[numpars];
                                 REAL* s  = new REAL[numpars];
                                 REAL* sensaa = new REAL[numpars];
                                 BOOLEAN res_ok = GetResult(var,calcnum,com,zone,
                                                            time,space,spacerel,f);
                                 if ( res_ok == TRUE )
                                 {
                                    if ( sensout != 0 )
                                    {
                                       *sensout << '\n';
                                       *sensout << var->Symbol() << '\t'
                                                << calcnum << '\t'
                                                << com->Symbol() << '\t'
                                                << com->ZoneName(zone) << '\t'
                                                << time << '\t'
                                                << space << '\t';
                                       if ( spacerel == FALSE ) *sensout << "a" << '\t';
                                       else                     *sensout << "r" << '\t';
                                       *sensout << def_sig[idef] << '\t';
                                       *sensout << f << '\t';
                                    }
                                    for ( ipar=0; ipar<numpars; ipar++ )
                                    {
                                       if ( par[ipar]->Type() == ConstVar )
                                       {
                                          p[ipar] = par[ipar]->Evaluate();
                                          CONSTVAR* cv = (CONSTVAR*)par[ipar];
                                          s[ipar] = cv->StDev();
                                       }
                                       else
                                       {
                                          res_ok = GetResult(par[ipar],calcnum,com,zone,
                                                             time,space,spacerel,p[ipar]);
                                          if ( res_ok != TRUE ) break;
                                          if ( par[ipar]->Type() == RealListVar )
                                          {
                                             REALLISTVAR* rlv = (REALLISTVAR*)par[ipar];
                                             REAL arg;
                                             res_ok = GetResult(rlv->GetArg(),calcnum,com,zone,
                                                                time,space,spacerel,arg);
                                             if ( res_ok != TRUE ) break;
                                             s[ipar] = rlv->StDev(arg);
                                          }
                                          else
                                          {
                                             FatalError("AQUASYS::SensRank: illegal par type");
                                          }
                                       }
                                       if ( par[ipar]->SetSensValue(TRUE) == FALSE ) return PROBLEM;
                                       res_ok = GetResult(var,calcnum,com,zone,
                                                          time,space,spacerel,ff[ipar],par[ipar]);
                                       if ( res_ok != TRUE ) break;
                                       if ( par[ipar]->Type() == ConstVar )
                                       {
                                          pp[ipar] = par[ipar]->Evaluate();
                                       }
                                       else
                                       {
                                          res_ok = GetResult(par[ipar],calcnum,com,zone,
                                                             time,space,spacerel,pp[ipar],par[ipar]);
                                       }
                                       if ( res_ok != TRUE ) break;
                                       par[ipar]->SetSensValue(FALSE);
                                       sensaa[ipar] = (f-ff[ipar])/(p[ipar]-pp[ipar]);
                                    } // end for ipar=0; ipar<numpars
                                    if ( res_ok == TRUE )
                                    {
                                       if ( sensout != 0 )
                                       {
                                          for ( ipar=0; ipar<numpars; ipar++ )
                                          {
                                             *sensout << '\t' << sensaa[ipar];
                                          }
                                       }
                                       for ( ipar=0; ipar<numpars; ipar++ )
                                       {
                                          REAL term = p[ipar]*sensaa[ipar];
                                          meansqrsens[ipar] += term*term;
                                          meanabssens[ipar] += fabs(term);
                                          meanvalsens[ipar] += term;
                                          if ( fabs(term) > maxabssens[ipar] )
                                             maxabssens[ipar] = fabs(term);
                                          meansqrsensglobal[ipar] += term*term
                                                                     /(def_sig[idef]*def_sig[idef]);
                                          meanabssensglobal[ipar] += fabs(term/def_sig[idef]);
                                          meanvalsensglobal[ipar] += term/def_sig[idef];
                                          if ( fabs(term/def_sig[idef]) > maxabssensglobal[ipar] )
                                             maxabssensglobal[ipar] = fabs(term/def_sig[idef]);
                                          term = s[ipar]*sensaa[ipar];
                                          meansqrerr[ipar] += term*term;
                                          meanabserr[ipar] += fabs(term);
                                          meanvalerr[ipar] += term;
                                          if ( fabs(term) > maxabserr[ipar] )
                                             maxabserr[ipar] = fabs(term);
                                          meansqrerrglobal[ipar] += term*term
                                                                    /(def_sig[idef]*def_sig[idef]);
                                          meanabserrglobal[ipar] += fabs(term/def_sig[idef]);
                                          meanvalerrglobal[ipar] += term/def_sig[idef];
                                          if ( fabs(term/def_sig[idef]) > maxabserrglobal[ipar] )
                                             maxabserrglobal[ipar] = fabs(term/def_sig[idef]);
                                       }
                                       numval++;
                                       numvalglobal++;
                                    }
                                    else
                                    {
                                       if ( sensout != 0 )
                                       {
                                          for ( ipar=0; ipar<numpars; ipar++ )
                                          {
                                             *sensout << '\t' << NotAvail;
                                          }
                                       }
                                    }
                                 } // if ( res_ok == TRUE )
                                 delete [] ff;
                                 delete [] p;
                                 delete [] pp;
                                 delete [] s;
                                 delete [] sensaa;
                              }	// end if def_cn[idef]==calcnum ...
                           } // end for ( idef=0; idef<numdef; idef++ )
                           for ( ipar=0; ipar<numpars; ipar++ )
                           {
                              meansqrsens[ipar] = sqrt(meansqrsens[ipar]/numval);
                              meanabssens[ipar] = meanabssens[ipar]/numval;
                              meanvalsens[ipar] = meanvalsens[ipar]/numval;
                              meansqrerr[ipar]  = sqrt(meansqrerr[ipar]/numval);
                              meanabserr[ipar]  = meanabserr[ipar]/numval;
                              meanvalerr[ipar]  = meanvalerr[ipar]/numval;
                           }
                           CARDINAL* indsens = new CARDINAL[numpars];
                           CARDINAL* inderr  = new CARDINAL[numpars];
                           sort(numpars,meansqrsens,indsens);
                           sort(numpars,meansqrerr, inderr);
                           JOBSTATUS status
                              = PrintRankHeader(rankout,vars[ivar],numval);
                           if ( status != OK )                         return status;
                           for ( ipar=0; ipar<numpars; ipar++ )
                           {
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,ipar+1) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,par[indsens[numpars-1-ipar]]->Symbol()) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meansqrsens[indsens[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanabssens[indsens[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanvalsens[indsens[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,maxabssens[indsens[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,par[inderr[numpars-1-ipar]]->Symbol()) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meansqrerr[inderr[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanabserr[inderr[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,meanvalerr[inderr[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
                              if ( Print(*rankout,maxabserr[inderr[numpars-1-ipar]]) == FALSE )
                                                              return PROBLEM;
                              if ( PrintLn(*rankout) == FALSE ) return PROBLEM;
                           }
                           delete [] indsens;
                           delete [] inderr;
                           delete [] meansqrsens;
                           delete [] meanabssens;
                           delete [] meanvalsens;
                           delete [] maxabssens;
                           delete [] meansqrerr;
                           delete [] meanabserr;
                           delete [] meanvalerr;
                           delete [] maxabserr;
                        } // end for ( ivar=0; ivar<numvar; ivar++ )
                        delete [] vars;
                     } // end if zone < com->NumZones()
                  } // end for ( izone=0; izone<numzone; izone++ )
                  delete [] zones;
               } // end if com-Active() == TRUE
            } // end loop for compartments
            delete [] comps;
            if ( sensout != 0 ) *sensout << "\n\n";
            for ( ipar=0; ipar<numpars; ipar++ )
            {
               meansqrsensglobal[ipar] = sqrt(meansqrsensglobal[ipar]/numvalglobal);
               meanabssensglobal[ipar] = meanabssensglobal[ipar]/numvalglobal;
               meanvalsensglobal[ipar] = meanvalsensglobal[ipar]/numvalglobal;
               meansqrerrglobal[ipar]  = sqrt(meansqrerrglobal[ipar]/numvalglobal);
               meanabserrglobal[ipar]  = meanabserrglobal[ipar]/numvalglobal;
               meanvalerrglobal[ipar]  = meanvalerrglobal[ipar]/numvalglobal;
            }
            CARDINAL* indsens = new CARDINAL[numpars];
            CARDINAL* inderr  = new CARDINAL[numpars];
            sort(numpars,meansqrsensglobal,indsens);
            sort(numpars,meansqrerrglobal, inderr);

            JOBSTATUS status
               = PrintRankHeader(rankout,0,numvalglobal);
            if ( status != OK )                         return status;
            for ( ipar=0; ipar<numpars; ipar++ )
            {
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,ipar+1) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,par[indsens[numpars-1-ipar]]->Symbol()) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,meansqrsensglobal[indsens[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,meanabssensglobal[indsens[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,meanvalsensglobal[indsens[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,maxabssensglobal[indsens[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,par[inderr[numpars-1-ipar]]->Symbol()) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,meansqrerrglobal[inderr[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,meanabserrglobal[inderr[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,meanvalerrglobal[inderr[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( Print(*rankout,"\t") == FALSE ) return PROBLEM;
               if ( Print(*rankout,maxabserrglobal[inderr[numpars-1-ipar]]) == FALSE )
                                               return PROBLEM;
               if ( PrintLn(*rankout) == FALSE ) return PROBLEM;
            }
            delete [] par;
            delete [] indsens;
            delete [] inderr;
            delete [] meansqrsensglobal;
            delete [] meanabssensglobal;
            delete [] meanvalsensglobal;
            delete [] maxabssensglobal;
            delete [] meansqrerrglobal;
            delete [] meanabserrglobal;
            delete [] meanvalerrglobal;
            delete [] maxabserrglobal;
         } // end loop if numpars > 0
      }	// end loop calculation numbers

      delete calcnums;
      delete def_var;
      delete def_cn;
      delete def_com;
      delete def_zind;
      delete def_time;
      delete def_space;
      delete def_srel;
      delete def_sig;
   } // end cmdfile == 0 / else

   return OK;
}


JOBSTATUS AQUASYS::CollectFitData(BOOLEAN writelog)
{
   CARDINAL i;

   if ( datind != 0 )
   {
      for ( i=0; i<nfits; i++ ) delete [] datind[i];
   }
   delete [] datind; datind = 0;
   nfits = 0;                                    // count active fits
   FIT* fit = (fitsys.Fitlist())->First();
   while ( fit != 0 )
   {
      if ( fit->FitActive() == TRUE ) nfits++;
      fit = fit->Next();
   }
   if ( nfits == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(88) << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   datind = new INTEGER*[nfits];
   numel = new CARDINAL*[nfits];
   delete fits; fits = new FIT*[nfits];          // collect active fits
   ndat = 0;                                     // and check consistency
   AQVAR* timevar = varsys.GetProgVar(Time);
   i = 0;
   fit = (fitsys.Fitlist())->First();            // loop over different fits
   while ( fit != 0 )
   {
      if ( fit->FitActive() == TRUE )
      {
         fits[i] = fit;
         datind[i] = new INTEGER[fit->NumData()];
         numel[i]  = new CARDINAL[fit->NumData()];
         for ( CARDINAL j=0; j<fit->NumData(); j++ )  // loop over data se-
         {                                            // ries of current fit
            AQCOMP* com       = (fit->Data(j)).com;
            REAL    spacetime = (fit->Data(j)).spacetime;
            AQVAR*  data      = (fit->Data(j)).data;
            if ( com == 0 ) 
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << ini.T(134) << " ***";
                  cjob->flush();
               }
               return PROBLEM;
            }
            if ( com->Active() == TRUE )
            {
               if ( data->Type() != RealListVar )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << data->Symbol() << ": "
                           << ini.T(106) << " ***";
                     cjob->flush();
                  }
                  return PROBLEM;
               }
               REALLISTVAR* datalist = (REALLISTVAR*)data;
               AQVAR*       arg      = datalist->GetArg();
               datind[i][j] = ndat;
               if ( arg == com->SpaceVar(&varsys) )     // spatial profile
               {                                        // spacetime = time
                  if ( spacetime >= fit->InitTime() )
                  {
                     REAL x = datalist->X()[0];
                     if ( com->AllowedSpaceVal(x) == FALSE )
                     {
                        if ( writelog == TRUE )
                        {
                           *cjob << "\n\n*** " << data->Symbol() << ": "
                                 << ini.T(107) << " ***";
                           cjob->flush();
                        }
                        return PROBLEM;
                     }
                     x = datalist->X()[datalist->NumPairs()-1];
                     if ( com->AllowedSpaceVal(x) == FALSE )
                     {
                        if ( writelog == TRUE )
                        {
                           *cjob << "\n\n*** " << data->Symbol() << ": "
                                 << ini.T(107) << " ***";
                           cjob->flush();
                        }
                        return PROBLEM;
                     }
                     numel[i][j] = datalist->NumPairs();
                     ndat += datalist->NumPairs();
                  }
               }
               else
               {
                  if ( arg == timevar )                 // time series
                  {                                     // spacetime = space
                     if ( com->AllowedSpaceVal(spacetime) == FALSE )
                     {
                        if ( writelog == TRUE )
                        {
                           *cjob << "\n\n*** " << ini.T(109) << " ***";
                           cjob->flush();
                        }
                        return PROBLEM;
                     }
                     numel[i][j] = 0;
                     for ( CARDINAL k=0; k<datalist->NumPairs(); k++ )
                     {
                        if ( datalist->X()[k] < fit->InitTime() ) datind[i][j]--;
                        else                                      numel[i][j]++;
                     }
                     ndat += numel[i][j];
                  }
                  else
                  {
                     if ( writelog == TRUE )
                     {
                        *cjob << "\n\n*** " << data->Symbol() << ": "
                              << ini.T(108) << " ***";
                        cjob->flush();
                     }
                     return PROBLEM;
                  }
               }
            }
         }
         i++; if ( i == nfits ) break;
      }
      fit = fit->Next();
   }

   fval    = new REAL[ndat];                 // determine values and st.devs.
   fsig    = new REAL[ndat];
   fcal    = new REAL[ndat];
   fcalini = new REAL[ndat];
   numfun  = 0;
   CARDINAL idat = 0;
   for ( i=0; i<nfits; i++ )
   {
      for ( CARDINAL j=0; j<fits[i]->NumData(); j++ )    // loop over data se-
      {                                                  // ries of current fit
         if ( (fits[i]->Data(j)).com->Active() == TRUE )
         {
            AQVAR* data = (fits[i]->Data(j)).data;
            if ( data->Type() != RealListVar )
               FatalError("AQUASYS::Fit: wrong variable type");
            REALLISTVAR* datalist = (REALLISTVAR*)data;
            for ( CARDINAL k=datalist->NumPairs()-numel[i][j];
                  k<datalist->NumPairs(); k++ )
            {
               fval[idat] = datalist->Y()[k];
               fsig[idat] = datalist->GetStDev(k);
               idat++;
            }
         }
      }
   }
   if ( idat != ndat )
      FatalError("AQUASYS::CollectFitData: inconsistent number of data points");

   return OK;
}


JOBSTATUS AQUASYS::Fit(std::ostream& out,
                       CARDINAL* numstep,
                       BOOLEAN writelog)
{
   BOOLEAN extendfitlog = FALSE; if ( ini.I(5) != 0 ) extendfitlog = TRUE;
   cjob->precision(ini.I(3));
   out.precision(ini.I(4));
   PrintWidth(0);
   CARDINAL field = abs(ini.I(4))+8;
   CARDINAL i;

   CARDINAL maxstep = fititer;
   *numstep = 0;

   if ( writelog == TRUE )
   {
      *cjob << "\n\n\n" << ini.T(37) << "\n";                   // Fit
      Space(*cjob,'-',strlen(ini.T(37)));
   }

   JOBSTATUS status = CollectFitData(writelog);
   if ( status != OK ) return status;

   CARDINAL npar = 0;                            // count active parameters
   AQVAR* var = (varsys.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* fitvar = (CONSTVAR*)var;
         if ( fitvar->FitActive() == TRUE ) npar++;
      }
      var = var->Next();
   }

   if ( ndat <= npar )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(91) << " (" << ndat << "," << npar << ")" 
               << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   delete fitvars; fitvars = 0;
   if ( npar > 0 ) fitvars = new CONSTVAR*[npar];  // collect active fitvars
   i = 0;
   var = (varsys.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* fitvar = (CONSTVAR*)var;
         if ( fitvar->FitActive() == TRUE )
         {
            if ( i >= npar )
               FatalError("AQUASYS::Fit: inconsistent number of parameters");
            fitvars[i] = fitvar;
            i++;
            if ( i == npar ) break;
         }
      }         
      var = var->Next();
   }

   char Buf[BufSize];

   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( Print(out,ProgName()) == FALSE )                      return PROBLEM;
   if ( Print(out," ") == FALSE )                             return PROBLEM;
   if ( Print(out,ProgVersion()) == FALSE )                   return PROBLEM;
   if ( Print(out," - ") == FALSE )                           return PROBLEM;
   if ( Print(out,ini.T(4)) == FALSE )                        return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintTime(out,ini.T(105)) == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;

   if ( maxstep>0 && npar>0 )
   {
      Print(out,ini.T(6),1); Print(out," = "); Print(out,npar);

      Print(out,ini.T(7),1); Print(out," = "); Print(out,ndat);

      Print(out,ini.T(67),1); Print(out," = ");
      switch ( fitmeth )
      {
         case Simplex:
            Print(out,ini.T(68));
            break;
         case Secant:
            Print(out,ini.T(69));
            break;
      }
      PrintLn(out); PrintLn(out); PrintLn(out);

      Print(out,ini.T(42),1); Print(out,":");
      PrintLn(out); PrintLn(out);
      Print(out,ini.T(43),2);
      Print(out,ini.T(44),field+2);
      Print(out,ini.T(45),2*field+2);
      Print(out,ini.T(46),3*field+2);
      Print(out,ini.T(47),4*field+2);
      for ( i=0; i<npar; i++ )
      {
         Print(out,fitvars[i]->Symbol(),2);
         Print(out,fitvars[i]->Unit(),field+2);
         Print(out,fitvars[i]->Value(),2*field+2);
         Print(out,fitvars[i]->Minimum(),3*field+2);
         Print(out,fitvars[i]->Maximum(),4*field+2);
      }
      PrintLn(out); PrintLn(out); PrintLn(out);

      Print(out,ini.T(48),1); Print(out,":");
      PrintLn(out); PrintLn(out);
      for ( i=0; i<npar; i++ )
      {
         Print(out,fitvars[i]->Symbol(),i*field+2);
      }
      Print(out,ini.T(5),npar*field+2);
      for ( i=0; i<npar; i++ )
      {
         Print(out,"[",i*field+2);
         Print(out,fitvars[i]->Unit());
         Print(out,"]");
      }
      PrintLn(out);
   }

   if ( writelog == TRUE )
   {
      *cjob << "\n\n   " << ini.T(38) << ":";                   // NumPar
      Space(*cjob,' ',space-strlen(ini.T(38)));
      *cjob << npar;
      *cjob << "\n   " << ini.T(39) << ":";                     // NumData
      Space(*cjob,' ',space-strlen(ini.T(39)));
      *cjob << ndat;
      TimeToString(ini.T(14),Buf,BufSize);                      // StartCalc
      *cjob << "\n\n   " << Buf << "  " << ini.T(15);
      cjob->flush();
   }

   REAL*   par     = 0;
   REAL*   minpar  = 0;
   REAL*   maxpar  = 0;
   REAL*   cov     = 0;
   if ( npar > 0 )
   {
      par     = new REAL[npar];
      minpar  = new REAL[npar];
      maxpar  = new REAL[npar];
      cov     = new REAL[npar*npar];
      for ( i=0; i<npar; i++ )
      {
         par[i]    = fitvars[i]->Value();
         minpar[i] = fitvars[i]->Minimum();
         maxpar[i] = fitvars[i]->Maximum();
      }
   }
   BOOLEAN covcalc = FALSE;
   REAL ftol = numpar.FitTol();
   CARDINAL convcrit = ini.I(6);
   REAL chi;
   NUMINFO numinfo;
   aquafitsys = this;
   DeleteStates();
   fitout = &out;
   status = PROBLEM;
   switch ( fitmeth )
   {
      case Simplex:
         status = FitSimplex(par,minpar,maxpar,npar,fval,fsig,ndat,
                             fitfun,ftol,convcrit,maxstep,&chi,numinfo,extendfitlog);
         break;
      case Secant:
         status = FitSecant (par,minpar,maxpar,npar,fval,fsig,ndat,
                             fitfun,ftol,convcrit,maxstep,&chi,numinfo,extendfitlog,
                             &covcalc,cov);
         break;
   }
   aquafitsys = 0;
   *numstep = numinfo.NumStep();
   if ( npar > 0 )
   {
      for ( i=0; i<npar; i++ ) fitvars[i]->SetValue(par[i]);
   }

   PrintLn(out); PrintLn(out); PrintLn(out);
   if ( writelog == TRUE )
   {
      TimeToString(ini.T(14),Buf,BufSize);                      // EndCalc
      *cjob << "\n   " << Buf << "  " << ini.T(16);
   }
   switch ( status )
   {
      case OK:
         if ( *numstep>0 && npar>0 )
         {
            Print(out,ini.T(63),1);                             // Success
            PrintLn(out); PrintLn(out); PrintLn(out);
            for ( i=0; i<npar; i++ )
            {
               Print(out,fitvars[i]->Symbol(),i*field+2);
            }
            for ( i=0; i<npar; i++ )
            {
               Print(out,"[",i*field+2);
               Print(out,fitvars[i]->Unit());
               Print(out,"]");
            }
            PrintLn(out); PrintLn(out); PrintLn(out);
            Print(out,ini.T(79)); Print(out,":");               // Val
            PrintLn(out); PrintLn(out);
            for ( i=0; i<npar; i++ )
            {
               Print(out,par[i],i*field+2);
            }
            PrintLn(out); PrintLn(out); PrintLn(out);
            if ( covcalc == TRUE )
            {
               Print(out,ini.T(73),1); Print(out,":");          // StDev
               PrintLn(out); PrintLn(out);
               for ( i=0; i<npar; i++ )
               {
                  REAL r = sqrt(cov[i+npar*i]);
                  fitvars[i]->StDev(r);
                  Print(out,r,i*field+2);
               }
               PrintLn(out); PrintLn(out); PrintLn(out);
               Print(out,ini.T(74),1); Print(out,":");          // Corr
               PrintLn(out); PrintLn(out);
               for ( i=0; i<npar; i++ )
               {
                  for ( CARDINAL j=0; j<npar; j++ )
                  {
                     REAL r = cov[i+npar*j]
                              /sqrt(cov[i+npar*i]*cov[j+npar*j]);
                     Print(out,r,j*field+2);
                  }
               }
            }
            else
            {
               Print(out,ini.T(72),1);
            }
         }
         break;
      case INTERRUPT:
         Print(out,ini.T(65),1);
         if ( writelog == TRUE )
         {
            *cjob << "\n\n   *** " << ini.T(17) << " ***";
            cjob->flush();
         }
         break;
      case PROBLEM:
         if ( *numstep == maxstep )
         {
            Print(out,maxstep,1); Print(out," ");
            Print(out,ini.T(64));
            if ( writelog == TRUE )
            {
               *cjob << "\n\n   " << ini.T(41);
               cjob->flush();
            }
         }
         else
         {
            Print(out,ini.T(66),1);
            if ( writelog == TRUE )
            {
               *cjob << "\n\n   *** " << ini.T(18) << " ***";
               cjob->flush();
            }
         }
         break;
   }
   CARDINAL tabfield = 20;
   PrintLn(out); PrintLn(out); PrintLn(out);
   Print(out,ini.T(75),1); Print(out,":");            // ChiContr
   PrintLn(out); PrintLn(out);
   Print(out,ini.T(76),2); Print(out,":");            // Calc
   Print(out,ini.T(77),tabfield+2); Print(out,":");
   Print(out,ini.T(78),2*tabfield+2); Print(out,":");
   if ( *numstep > 0 )
   {
      Print(out,ini.T(80),2*tabfield+field+2); Print(out,":");
   }
   CARDINAL idat = 0;
   REAL chiini = 0; chiend = 0;
   for ( i=0; i<nfits; i++ )
   {
      Print(out,fits[i]->Symbol(),2);
      for ( CARDINAL j=0; j<fits[i]->NumData(); j++ ) // loop over
      {                                               // data series
         if ( (fits[i]->Data(j)).com->Active() == TRUE )
         {
            AQVAR* data = (fits[i]->Data(j)).data;
            Print(out,data->Symbol(),tabfield+2);
            if ( data->Type() != RealListVar )
               FatalError("AQUASYS::Fit: wrong variable type");
            REALLISTVAR* datalist = (REALLISTVAR*)data;
            REAL chipartini = 0; REAL chipartend = 0;
            for ( CARDINAL k=datalist->NumPairs()-numel[i][j];
                  k<datalist->NumPairs(); k++ )
            {
               chipartini += (fval[idat]-fcalini[idat])
                             *(fval[idat]-fcalini[idat])
                             / (fsig[idat]*fsig[idat]);
               chipartend += (fval[idat]-fcal[idat])
                             *(fval[idat]-fcal[idat])
                             / (fsig[idat]*fsig[idat]);
               idat++;
            }
            Print(out,chipartini,2*tabfield+2);
            chiini += chipartini;
            if ( status != INTERRUPT && *numstep > 0 )
            {
               Print(out,chipartend,2*tabfield+field+2);
               chiend += chipartend;
            }
         }
      }
   }
   if ( idat != ndat )
      FatalError("AQUASYS::Fit: inconsistent number of data points");
   Print(out,'-',2*tabfield+2,field-4);
   if ( status != INTERRUPT && *numstep > 0 )
   {
      Print(out,'-',2*tabfield+field+2,field-4);
   }
   else
   {
      chiend = chiini;
   }
   Print(out,chiini,2*tabfield+2);
   if ( status != INTERRUPT && *numstep > 0 )
   {
      Print(out,chiend,2*tabfield+field+2);
   }
   PrintLn(out);
   if ( *numstep > 0 ) 
   {
      PrintLn(out);  PrintLn(out);
      Print(out,ini.T(70),1); Print(out," = ");           // NumSteps
      Print(out,*numstep);
      Print(out,ini.T(71),1); Print(out," = ");           // NumFun
      Print(out,numinfo.NumFun());
      if ( writelog == TRUE )
      {
         *cjob << "\n\n   " << ini.T(40) << ":";          // NumSteps
         Space(*cjob,' ',space-strlen(ini.T(40)));
         *cjob << *numstep;
         cjob->flush();
      }
   }
   PrintLn(out); PrintLn(out); PrintLn(out); 
   PrintBuffer(out);
   delete [] par;
   delete [] minpar;
   delete [] maxpar;
   delete [] cov;
   delete [] fval;    fval = 0;
   delete [] fsig;    fsig = 0;
   delete [] fcal;    fcal = 0;
   delete [] fcalini; fcalini = 0;
   for ( i=0; i<nfits; i++ ) delete [] numel[i];
   delete [] numel;   numel = 0;

   return status;
}


JOBSTATUS AQUASYS::SetFit(std::istream& in, std::ostream& out, std::ostream& fitout,
						  BOOLEAN writelog)
{
   cjob->precision(ini.I(3));
   out.precision(ini.I(4));
   PrintWidth(0);
   CARDINAL field = abs(ini.I(4))+8;
   CARDINAL i,j;

   if ( writelog == TRUE )
   {
      *cjob << "\n\n\n" << ini.T(116) << "\n";   // SetFit
      Space(*cjob,'=',strlen(ini.T(116)));
   }

   JOBSTATUS status = CollectFitData(writelog);
   if ( status != OK ) return status;

   // collect active parameters in fitvars[npar]:

   CARDINAL npar = 0;                            // count active parameters
   AQVAR* var = (varsys.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* fitvar = (CONSTVAR*)var;
         if ( fitvar->FitActive() == TRUE ) npar++;
      }
      var = var->Next();
   }

   if ( ndat <= npar )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(91) << " (" << ndat << "," << npar << ")" 
               << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   delete fitvars; fitvars = 0;
   if ( npar == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << "no parameters found" 
               << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }
   fitvars = new CONSTVAR*[npar];            // collect active fitvars
   REAL* fitvarvals = new REAL[npar];
   i = 0;
   var = (varsys.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* fitvar = (CONSTVAR*)var;
         if ( fitvar->FitActive() == TRUE )
         {
            if ( i >= npar )
               FatalError("AQUASYS::SetFit: inconsistent number of parameters");
            fitvars[i] = fitvar;
			fitvarvals[i] = fitvar->Evaluate();
            i++;
            if ( i == npar ) break;
         }
      }         
      var = var->Next();
   }

   // collect given variables in vars[nvar]:

   CONSTVAR** vars = 0;
   CARDINAL nvar = 0;
   char** words;
   CARDINAL numwords = ReadWords(in,words);
   BOOLEAN transfersamppdf = FALSE;
   for ( i=0; i<numwords; i++ )
   {
      if ( strcmp(words[i],"samp-pdf") == 0  ) 
      {
         transfersamppdf = TRUE;
         break;
      }
      else
      {
         if ( strcmp(words[i],"pdf-val")==0 || 
              strcmp(words[i],"samp-weight")==0 ||
              strcmp(words[i],"pri-pdf")==0 )
         {
            break;
         }
         else
         {
            var = (varsys.Varlist())->Get(words[i]);  // look for parameter
            if ( var == 0 )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << ini.T(117) << ": " << words[i];
                  cjob->flush();
               }
               return PROBLEM;
            }
            if ( var->Type() != ConstVar)
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << ini.T(118) << ": " << words[i];
                  cjob->flush();
               }
               return PROBLEM;
            }
            CONSTVAR* constvar = (CONSTVAR*)var;
			CONSTVAR** newvars = new CONSTVAR*[nvar+1];
            if ( nvar > 0 )
            {
               for ( j=0; j<nvar; j++ )
               {
                  newvars[j] = vars[j];
               }
            }
            newvars[nvar] = constvar;
            nvar++;
            delete vars; vars = newvars;
         }
      }
   }
   DeleteWords(numwords,words);
   if ( nvar == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(123) << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   // write header of output file:

   for ( i=0; i<nvar; i++ )
   {
      out << vars[i]->Symbol() << "\t";
   }
   for ( i=0; i<npar; i++ )
   {
      out << fitvars[i]->Symbol() << "\t";
   }
   if ( transfersamppdf == TRUE )
   {
      out << "samp-pdf" << "\t";
   }
   out << "chi2";
   out << "\n";
   out.flush();

   aquafitsys = this;
   DeleteStates();

   CARDLONG numcalc = 0;
   while ( 1 )
   {
      for ( i=0; i<npar; i++ )
      {
         fitvars[i]->SetValue(fitvarvals[i]);
      }
      numwords = ReadWords(in,words);
      if ( numwords == 0 ) break;
      BOOLEAN calc_ok = TRUE;
      if ( numwords < nvar ) calc_ok = FALSE;
      for ( i=0; i<nvar; i++ )
      {
         REAL val;
         if ( StringToNum(words[i],&val) == FALSE )
         {
            calc_ok = FALSE;
            out << NotAvail << "\t";
            if ( writelog == TRUE )
			{
               *cjob << "\n\n\nUnable to perform parameter estimation" 
				     <<     "\n--------------------------------------";
               //*cjob << "\n\n*** " << ini.T(138) << " " 
			   //      << fitvars[i]->Symbol() << " ***";
			   // corrected 19.9.02 version 2.1d:
               *cjob << "\n\n*** " << ini.T(138) << " " 
				     << vars[i]->Symbol() << " ***";
               cjob->flush();
			}
         }
         else
         {
            if ( vars[i]->SetValue(val) == FALSE )
			{
               calc_ok = FALSE;
               out << NotAvail << "\t";
               if ( writelog == TRUE )
			   {
                  *cjob << "\n\n\nUnable to perform parameter estimation" 
                        <<     "\n--------------------------------------";
                  //*cjob << "\n\n*** " << ini.T(139) << " " 
				  //      << fitvars[i]->Symbol() 
				  //      << ": " << val << " ***";
                  // corrected 19.9.02 version 2.1d:
                  *cjob << "\n\n*** " << ini.T(139) << " " 
					    << vars[i]->Symbol() 
					    << ": " << val << " ***";
                  cjob->flush();
			   }
			}
            else
            {
               out << val << "\t";
            }
		 }
      }
      status = PROBLEM;
      CARDINAL numsteps = 0;
      if ( calc_ok == TRUE )
      {
         numsteps = FitIter();
         status = Fit(fitout,&numsteps);
         if ( status==OK || numsteps==FitIter() )
         {
            for ( i=0; i<npar; i++ )
			{
               out << fitvars[i]->Evaluate() << "\t";
			}
		 }
         else
         {
			for ( i=0; i<npar; i++ )
            {
				out << NotAvail << "\t";
			}
		 }
	  }
	  else
      {
         for ( i=0; i<npar; i++ )
         {
            out << NotAvail << "\t";
		 }
	  }
      if ( transfersamppdf == TRUE )
      {
         if ( numwords > nvar )
         {
            out << words[nvar] << "\t";
         }
         else
         {
            out << NotAvail << "\t";
         }
      }
      if ( status==OK || numsteps==FitIter() )
      {
         out << chiend;
	  }
	  else
	  {
		  out << NotAvail;
	  }
	  out << "\n";
	  out.flush();
      DeleteWords(numwords,words);
   }
   delete [] fitvars;
   delete [] fitvarvals;
   delete [] vars;

   return status;
}


JOBSTATUS AQUASYS::CalcChi2(std::istream& in, std::ostream& out, BOOLEAN writelog)
{
   cjob->precision(ini.I(3));
   out.precision(ini.I(4));
   PrintWidth(0);
   CARDINAL field = abs(ini.I(4))+8;
   CARDINAL i,j;

   if ( writelog == TRUE )
   {
      *cjob << "\n\n\n" << ini.T(111) << "\n";     // CalcChi2s
      Space(*cjob,'-',strlen(ini.T(111)));
   }

   JOBSTATUS status = CollectFitData(writelog);
   if ( status != OK ) return status;

   CONSTVAR** pars = 0;
   CARDINAL npar = 0;
   char** words;
   CARDINAL numwords = ReadWords(in,words);
   BOOLEAN transfersamppdf = FALSE;
   for ( i=0; i<numwords; i++ )
   {
      if ( strcmp(words[i],"samp-pdf") == 0  ) 
      {
         transfersamppdf = TRUE;
         break;
      }
      else
      {
         if ( strcmp(words[i],"pdf-val")==0 || 
              strcmp(words[i],"samp-weight")==0 ||
              strcmp(words[i],"pri-pdf")==0 )
         {
            break;
         }
         else
         {
            AQVAR* parvar = (varsys.Varlist())->Get(words[i]);  // look for parameter
            if ( parvar == 0 )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << ini.T(121) << ": " << words[i];
                  cjob->flush();
               }
               return PROBLEM;
            }
            if ( parvar->Type() != ConstVar)
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << ini.T(122) << ": " << words[i];
                  cjob->flush();
               }
               return PROBLEM;
            }
            CONSTVAR* par = (CONSTVAR*)parvar;
            delete fitvars; fitvars = new CONSTVAR*[npar+1];
            if ( npar > 0 )
            {
               for ( j=0; j<npar; j++ )
               {
                  fitvars[j] = pars[j];
               }
            }
            fitvars[npar] = par;
            npar++;
            delete pars; pars = new CONSTVAR*[npar];
            for ( j=0; j<npar; j++ )
            {
               pars[j] = fitvars[j];
            }
         }
      }
   }
   DeleteWords(numwords,words);
   if ( npar == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << ini.T(123) << " ***";
         cjob->flush();
      }
      return PROBLEM;
   }

   char Buf[BufSize];

   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( Print(out,ProgName()) == FALSE )                      return PROBLEM;
   if ( Print(out," ") == FALSE )                             return PROBLEM;
   if ( Print(out,ProgVersion()) == FALSE )                   return PROBLEM;
   if ( Print(out," - ") == FALSE )                           return PROBLEM;
   if ( Print(out,ini.T(115)) == FALSE )                      return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintTime(out,ini.T(124)) == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;

   Print(out,ini.T(48),1); Print(out,":");
   PrintLn(out); PrintLn(out);
   for ( i=0; i<npar; i++ )
   {
      Print(out,fitvars[i]->Symbol(),i*field+2);
   }
   if ( transfersamppdf == FALSE )
   {
      Print(out,ini.T(5),npar*field+2);
   }
   else
   {
      Print(out,"samp-pdf",npar*field+2);
      Print(out,ini.T(5),(npar+1)*field+2);
   }
   for ( i=0; i<npar; i++ )
   {
      Print(out,"[",i*field+2);
      Print(out,fitvars[i]->Unit());
      Print(out,"]");
   }
   PrintLn(out);

   if ( writelog == TRUE )
   {
      *cjob << "\n\n   " << ini.T(125) << ":";                   // NumPar
      Space(*cjob,' ',space-strlen(ini.T(125)));
      *cjob << npar;
      *cjob << "\n   " << ini.T(126) << ":";                     // NumData
      Space(*cjob,' ',space-strlen(ini.T(126)));
      *cjob << ndat;
      TimeToString(ini.T(14),Buf,BufSize);                      // StartCalc
      *cjob << "\n\n   " << Buf << "  " << ini.T(15);
      cjob->flush();
   }

   REAL*   par     = new REAL[npar];
   for ( i=0; i<npar; i++ )
   {
      par[i]    = fitvars[i]->Value();
   }
   aquafitsys = this;
   DeleteStates();
   fitout = &out;

   CARDLONG numcalc = 0;
   status = PROBLEM;
   while ( 1 )
   {
      numwords = ReadWords(in,words);
      if ( numwords == 0 ) break;
      BOOLEAN calc_ok = TRUE;
      if ( numwords < npar ) calc_ok = FALSE;
      for ( i=0; i<npar; i++ )
      {
         if ( StringToNum(words[i],&par[i]) == FALSE )
         {
            calc_ok = FALSE;
            break;
         }
      }
      if ( calc_ok == TRUE )
      {
         glob_transfersamppdf = transfersamppdf;
         glob_samppdfval = -1;
         if ( numwords > npar )
         {
            StringToNum(words[npar],&glob_samppdfval);
         }
         status = fitfun(par,npar,fcal,ndat);
         glob_transfersamppdf = FALSE;
         switch ( status ) 
         {
            case OK:
               numcalc++;
               break;
            case INTERRUPT:
               Print(out,ini.T(112),1);         // Interrupt
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n   *** " << ini.T(17) << " ***";
                  cjob->flush();
               }
               break;
            case PROBLEM:
               Print(out,ini.T(113),1);         // Failure
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n   *** " << ini.T(18) << " ***";
                  cjob->flush();
               }
               break;
         }
         PrintBuffer(out);
         if ( status != OK ) break;
      }
      DeleteWords(numwords,words);
   }
   PrintLn(out); PrintBuffer(out);
   if ( writelog == TRUE )
   {
      TimeToString(ini.T(14),Buf,BufSize);       // EndCalc
      *cjob << "\n   " << Buf << "  " << ini.T(16);
      cjob->flush();
   }   
   delete [] par;
   delete [] fval;    fval = 0;
   delete [] fsig;    fsig = 0;
   delete [] fcal;    fcal = 0;
   delete [] fcalini; fcalini = 0;
   for ( i=0; i<nfits; i++ ) delete [] numel[i];
   delete [] numel;   numel = 0;

   return status;
}


BOOLEAN AQUASYS::ExtractName(char* inpline, CARDINAL& pos,
                             char* namebuf)
{
   char c;
   CARDINAL nameind = 0;
   while ( 1 )   // search for first character of name
   {
      c = inpline[pos];
      pos++;
      if ( c == '\0' )
      {
         namebuf[0] = '\0';
         return FALSE;
      }
      if ( SYMBOL::AllowedFirstChar(c) == TRUE )
      {
         namebuf[0] = c;
         nameind = 1;
         break;
      }
   }
   while ( 1 )   // search for additional characters of name
   {
      c = inpline[pos];
      pos++;
      if ( c == '\0' )
      {
         break;
      }
      if ( SYMBOL::AllowedSubsequentChar(c) == TRUE )
      {
         namebuf[nameind] = c;
         nameind++;
      }
      else
      {
         break;
      }
   }
   namebuf[nameind] = '\0';
   if ( nameind == 0 ) return FALSE;
   return TRUE;
}


void AQUASYS::ParseArgs(CARDINAL numarg, char** args, char* linebuf)
{
   CARDINAL i;
   for ( i=0; i<numarg; i++ ) args[i] = 0;
   CARDINAL pos = 0;
   for ( i=0; i<numarg; i++ )
   {
      char buf[1024];
      char c;
      BOOLEAN endline = FALSE;
      CARDINAL ind = 0;
      while ( 1 )   // search for first character
      {
         c = linebuf[pos];
         pos++;
         if ( c == '\0' )
         {
            endline = TRUE;
            break;
         }
         if ( c > ' ' )
         {
            buf[0] = c;
            ind = 1;
            break;
         }
      }
      if ( endline == TRUE ) break;
      while ( 1 )   // search for additional characters
      {
         c = linebuf[pos];
         pos++;
         if ( c == '\0' )
         {
            endline = TRUE;
            break;
         }
         if ( c > ' ' )
         {
            buf[ind] = c;
            ind++;
         }
         else
         {
            break;
         }
      }
      buf[ind] = '\0';
      args[i] = new char[strlen(buf)+1];
      strcpy(args[i],buf);
   }
}

	  
JOBSTATUS AQUASYS::CalcRes(std::istream& inpar, std::istream& indef,
						   std::ostream& out, BOOLEAN writelog)
{
   cjob->precision(ini.I(3));
   out.precision(ini.I(4));
   PrintWidth(0);
   CARDINAL field = abs(ini.I(4))+8;
   CARDINAL i;

   CONSTVAR** pars = 0;
   CARDINAL   npars = 0;
   char linebuf[4096];
   if ( !inpar.getline(linebuf,sizeof(linebuf)) ) return PROBLEM;
   CARDINAL pos = 0;
   while ( 1 )
   {
      char namebuf[1024];
      char c;
      BOOLEAN endline = FALSE;
      CARDINAL nameind = 0;
      while ( 1 )   // search for first character of name
      {
         c = linebuf[pos];
         pos++;
         if ( c == '\0' )
         {
            endline = TRUE;
            break;
         }
         if ( SYMBOL::AllowedFirstChar(c) == TRUE )
         {
            namebuf[0] = c;
            nameind = 1;
            break;
         }
      }
      if ( endline == TRUE ) break;
      while ( 1 )   // search for additional characters of name
      {
         c = linebuf[pos];
         pos++;
         if ( c == '\0' )
         {
            endline = TRUE;
            break;
         }
         if ( SYMBOL::AllowedSubsequentChar(c) == TRUE )
         {
            namebuf[nameind] = c;
            nameind++;
         }
         else
         {
            break;
         }
      }
      namebuf[nameind] = '\0';
      AQVAR* parvar = (varsys.Varlist())->Get(namebuf);  // look for parameter
      if ( parvar == 0 )
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n\n*** " << ini.T(121) << ": " << namebuf
				  <<   "\n*** " << ini.T(140) << "\n";
            cjob->flush();
         }
         //return PROBLEM;
		 // changed for version 2.1d to ignore "samp-pdf" column:
		 break;
      }
      if ( parvar->Type() != ConstVar)
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n\n*** " << ini.T(122) << ": " << namebuf;
            cjob->flush();
         }
         return PROBLEM;
      }
      CONSTVAR** temppars = new CONSTVAR*[npars];
      for ( i=0; i<npars; i++ ) temppars[i] = pars[i];
      delete pars; pars = new CONSTVAR*[npars+1];
      for ( i=0; i<npars; i++ ) pars[i] = temppars[i];
      pars[npars] = (CONSTVAR*)parvar;
      npars++;
      if ( endline == TRUE ) break;
   }
  
   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( Print(out,ProgName()) == FALSE )                      return PROBLEM;
   if ( Print(out," ") == FALSE )                             return PROBLEM;
   if ( Print(out,ProgVersion()) == FALSE )                   return PROBLEM;
   if ( Print(out," - ") == FALSE )                           return PROBLEM;
   if ( Print(out,"Result File") == FALSE )                   return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                           return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintTime(out,ini.T(124)) == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   if ( PrintLn(out) == FALSE )                               return PROBLEM;
   Print(out,ini.T(48),1); Print(out,":");
   PrintLn(out); PrintLn(out);
   for ( i=0; i<npars; i++ )
   {
      // Print(out,pars[i]->Symbol(),i*field+2);
      out << pars[i]->Symbol() << '\t';
   }

   BOOLEAN*  plotdef_ok = 0;
   AQVAR**   vars = 0;
   CARDINAL* calcnums = 0;
   AQCOMP**  comps = 0;
   CARDINAL* zoneinds = 0;
   REAL*     times = 0;
   REAL*     spaces = 0;
   BOOLEAN*  spacerels = 0;
   CARDINAL  blocksize = 100;
   CARDINAL  size = 0;
   CARDINAL  numdefs = 0;
   while ( indef.getline(linebuf,sizeof(linebuf)) )
   {
      CARDINAL numarg = 7;
      char** args = new char*[numarg];
      ParseArgs(numarg,args,linebuf);
      if ( numdefs >= size )
      {
         BOOLEAN*  newplotdef_ok = new BOOLEAN[size+blocksize];
         AQVAR**   newvars       = new AQVAR*[size+blocksize];
         CARDINAL* newcalcnums   = new CARDINAL[size+blocksize];
         AQCOMP**  newcomps      = new AQCOMP*[size+blocksize];
         CARDINAL* newzoneinds   = new CARDINAL[size+blocksize];
         REAL*     newtimes      = new REAL[size+blocksize];
         REAL*     newspaces     = new REAL[size+blocksize];
         BOOLEAN*  newspacerels  = new BOOLEAN[size+blocksize];
         for ( i=0; i<size; i++ )
         {
            newplotdef_ok[i] = plotdef_ok[i];
            newvars[i]       = vars[i];
            newcalcnums[i]   = calcnums[i];
            newcomps[i]      = comps[i];
            newzoneinds[i]   = zoneinds[i];
            newtimes[i]      = times[i];
            newspaces[i]     = spaces[i];
            newspacerels[i]  = spacerels[i];
         }
         for ( i=size; i<size+blocksize; i++ )
         {
            newplotdef_ok[i] = FALSE;
            newvars[i]       = 0;
            newcalcnums[i]   = 0;
            newcomps[i]      = 0;
            newzoneinds[i]   = 0;
            newtimes[i]      = 0;
            newspaces[i]     = 0;
            newspacerels[i]  = FALSE;
         }
         delete [] plotdef_ok; plotdef_ok = newplotdef_ok;
         delete [] vars;       vars       = newvars;
         delete [] calcnums;   calcnums   = newcalcnums;
         delete [] comps;      comps      = newcomps;
         delete [] zoneinds;   zoneinds   = newzoneinds;
         delete [] times;      times      = newtimes;
         delete [] spaces;     spaces     = newspaces;
         delete [] spacerels;  spacerels  = newspacerels;
         size += blocksize;
      }
      plotdef_ok[numdefs] = TRUE;
      vars[numdefs] = GetVar(args[0]);
      if ( vars[numdefs] != 0 )
      {
         if ( StringToNum(args[1],&calcnums[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         comps[numdefs] = GetComp(args[2]);
         if ( comps[numdefs] == 0 )
            plotdef_ok[numdefs] = FALSE;
         if ( StringToNum(args[3],&zoneinds[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         if ( StringToNum(args[4],&times[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         if ( StringToNum(args[5],&spaces[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         if ( args[6][0] == 'r' || args[6][0] == 'R' ) 
            spacerels[numdefs] = TRUE;
         numdefs++;
      }
      for ( i=0; i<numarg; i++ ) delete args[i];
      delete [] args;
   }
   if ( numdefs == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << "No result definitions specified";
         cjob->flush();
      }
      return PROBLEM;
   }
   for ( i=0; i<numdefs; i++ )
   {
      // Print(out,vars[i]->Symbol(),(npars+i)*field+2);
      out << vars[i]->Symbol() << '\t';
   }
   // PrintLn(out);
   out << '\n'; 
   for ( i=0; i<npars; i++ ) out << '\t';
   for ( i=0; i<numdefs; i++ )
   {
      // Print(out,calcnums[i],(npars+i)*field+2);
      out << calcnums[i] << '\t';
   }
   // PrintLn(out);
   out << '\n'; 
   for ( i=0; i<npars; i++ ) out << '\t';
   for ( i=0; i<numdefs; i++ )
   {
      // Print(out,comps[i]->Symbol(),(npars+i)*field+2);
      out << comps[i]->Symbol() << '\t';
   }
   // PrintLn(out);
   out << '\n'; 
   for ( i=0; i<npars; i++ ) out << '\t';
   for ( i=0; i<numdefs; i++ )
   {
      // Print(out,zoneinds[i],(npars+i)*field+2);
      out << zoneinds[i] << '\t';
   }
   // PrintLn(out);
   out << '\n'; 
   for ( i=0; i<npars; i++ ) out << '\t';
   for ( i=0; i<numdefs; i++ )
   {
      // Print(out,times[i],(npars+i)*field+2);
      out << times[i] << '\t';
   }
   // PrintLn(out);
   out << '\n'; 
   for ( i=0; i<npars; i++ ) out << '\t';
   for ( i=0; i<numdefs; i++ )
   {
      // Print(out,spaces[i],(npars+i)*field+2);
      out << spaces[i] << '\t';
   }
   // PrintLn(out);
   out << '\n'; 
   for ( i=0; i<npars; i++ ) out << '\t';
   for ( i=0; i<numdefs; i++ )
   {
      if ( spacerels[i] == FALSE )
      {
         // Print(out,"abs",(npars+i)*field+2);
         out << "abs" << '\t';
      }
      else
      {
         // Print(out,"rel",(npars+i)*field+2);
         out << "rel" << '\t';
      }
   }
   // PrintLn(out); out.flush();
   out << '\n'; out.flush();
   if ( writelog == TRUE )
   {
       *cjob << "\n\n   "; 
       *cjob << "Number of parameters:         " << npars;
       *cjob << "\n   "; 
       *cjob << "Number of result definitions: " << numdefs;
       *cjob << "\n";
       cjob->flush();
   }
   CARDINAL numparsets = 0;
   CARDINAL numcalc = 0;
   CARDINAL numfail = 0;
   while ( inpar.getline(linebuf,sizeof(linebuf)) ) // loop over parsets
   {
      BOOLEAN read_ok = TRUE;
      BOOLEAN val_ok = TRUE;
      // PrintLn(out);
      out << '\n';
	  std::istrstream inputline(linebuf,sizeof(linebuf));
      for ( i=0; i<npars; i++ )
      {
         REAL val;
         inputline >> val;
         if ( inputline.fail() )
         {
            if ( i == 0 )
            {
               read_ok = FALSE;
               break;
            }
            else
            {
               val_ok = FALSE;
               // Print(out,NotAvail,i*field+2);
               out << NotAvail << '\t';
            }
         }
         else
         {
            // Print(out,val,i*field+2);
            out << val << '\t';
         }
         if ( pars[i]->SetValue(val) == FALSE )
         {
            val_ok = FALSE;
         }
      }
      out.flush();
      if ( read_ok == TRUE )
      {
         numparsets++;
         char Buf[BufSize];
         if ( val_ok == FALSE )
         {
            TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
            *cjob << "\n   " << Buf << "  "
                  << "data set: " << numparsets << "  "
                  << "Illegal parameter values";
            // Print(out,"Illegal parameter values",npars*field+2);
            out << "Illegal parameter values";
            numfail += numdefs;
         }
         else
         {
            if ( InitSim(FALSE) != OK )
            {
               TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
               *cjob << "\n   " << Buf << "  "
                     << "data set: " << numparsets << "  "
                     << "Problem during initialization";
               // Print(out,"Problem during initialization",npars*field+2);
               out << "Problem during initialization";
               numfail += numdefs;
            }
            else
            {
               if ( Simulate(FALSE) != OK )
               {
                  TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
                  *cjob << "\n   " << Buf << "  "
                        << "data set: " << numparsets << "  "
                        << "Problem during calculation";
                  // Print(out,"Problem during calculation",npars*field+2);
                  out << "Problem during calculation";
                  numfail += numdefs;
               }
               else
               {
                  numcalc++;
                  TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
                  *cjob << "\n   " << Buf << "  "
                        << "data set: " << numparsets << "  "
                        << "simulation: " << numcalc;
                  cjob->flush();

                  // calculate results

                  CARDINAL oldnumfail = numfail;
                  REAL value = 0;
                  for ( CARDINAL j=0; j<numdefs; j++ )
                  {
                     if ( plotdef_ok[j] == TRUE )
                     {
                        if ( GetResult(vars[j],calcnums[j],comps[j],
                                       zoneinds[j],times[j],spaces[j],
                                       spacerels[j],value) == TRUE )
                        {
                           // Print(out,value,(npars+j)*field+2);
                           out << value << '\t';
                        }
                        else
                        {
                           // Print(out,NotAvail,(npars+j)*field+2);
                           out << NotAvail << '\t';
                           numfail++;
                        }
                     }
                     else
                     {
                        // Print(out,NotAvail,(npars+j)*field+2);
                        out << NotAvail << '\t';
                        numfail++;
                     }
                  }
                  if ( numfail > oldnumfail )
                  {
                     *cjob << "  " << "(incomplete results)";
                     cjob->flush();
                  }
               }
            }
         }
      }
      out.flush();
   }
   if ( writelog == TRUE )
   {
      *cjob << "\n\n   ";
      *cjob << "Number of parameters:                     " << npars;
      *cjob << "\n   ";
      *cjob << "Number of result definitions:             " << numdefs;
      *cjob << "\n   ";
      *cjob << "Number of parameter sets:                 " << numparsets;
      *cjob << "\n   ";
      *cjob << "Number of successful calculations:        " << numcalc;
      if ( numcalc < numparsets )
      {
         *cjob << "\n** ";
         *cjob << "Number of unsuccessful calculations:      " 
               << numparsets-numcalc;
      }
      if ( numfail > 0 )
      {
         *cjob << "\n** ";
         *cjob << "Number of failures in result calculation: " 
               << numfail;
      }
   }
   cjob->flush();
   delete [] pars;
   delete [] plotdef_ok;
   delete [] vars;
   delete [] calcnums;
   delete [] comps;
   delete [] zoneinds;
   delete [] times;
   delete [] spaces;
   delete [] spacerels;
   if ( numcalc == 0 ) return PROBLEM;
   if ( numfail > 0 )  return INTERRUPT;  // calculation incomplete
   return OK;
}


JOBSTATUS AQUASYS::CalcSingleRes(std::istream& inpar, std::istream& indef,
								 std::ostream& out, BOOLEAN writelog)
{
   cjob->precision(ini.I(3));
   out.precision(ini.I(4));
   PrintWidth(0);
   CARDINAL field = abs(ini.I(4))+8;
   CARDINAL i;

   CONSTVAR** parvars = 0;
   REAL*      parvals = 0;
   CARDINAL   npars = 0;
   while ( 1 )
   {
      char** words;
      CARDINAL numwords = ReadWords(inpar,words);
      if ( numwords<2 )
	  {
         if ( npars == 0 )
         {
            *cjob << "\n*** ERROR: unable to read parameter names and values\n";
            return PROBLEM;
         }
		 else
         {
            break;
         }
	  }
      AQVAR* parvar = (varsys.Varlist())->Get(words[0]);
      if ( parvar == 0 )
      {
         {
            *cjob << "\n*** ERROR: variable \"" << words[0] << "\" not found\n";
            return PROBLEM;
         }
      }
      if ( parvar->Type() != ConstVar)
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n\n*** " << ini.T(122) << ": " << words[0];
            cjob->flush();
         }
         return PROBLEM;
      }
      CONSTVAR** newparvars = new CONSTVAR*[npars+1];
	  REAL*      newparvals = new REAL[npars+1];
      for ( i=0; i<npars; i++ )
      {
         newparvars[i] = parvars[i];
         newparvals[i] = parvals[i];
      }
	  newparvars[npars] = (CONSTVAR*)parvar;
      if ( StringToNum(words[1],&newparvals[npars]) == FALSE )
      {
         *cjob << "\n*** ERROR: unable to read value of parameter " << words[0] << "\n";
         return PROBLEM;
      }
      delete parvars; parvars = newparvars;
      delete parvals; parvals = newparvals;
      npars++;
      DeleteWords(numwords,words);
   }
  
   BOOLEAN*  plotdef_ok = 0;
   AQVAR**   vars = 0;
   CARDINAL* calcnums = 0;
   AQCOMP**  comps = 0;
   CARDINAL* zoneinds = 0;
   REAL*     times = 0;
   REAL*     spaces = 0;
   BOOLEAN*  spacerels = 0;
   CARDINAL  blocksize = 100;
   CARDINAL  size = 0;
   CARDINAL  numdefs = 0;
   char linebuf[4096];
   while ( indef.getline(linebuf,sizeof(linebuf)) )
   {
      CARDINAL numarg = 7;
      char** args = new char*[numarg];
      ParseArgs(numarg,args,linebuf);
      if ( numdefs >= size )
      {
         BOOLEAN*  newplotdef_ok = new BOOLEAN[size+blocksize];
         AQVAR**   newvars       = new AQVAR*[size+blocksize];
         CARDINAL* newcalcnums   = new CARDINAL[size+blocksize];
         AQCOMP**  newcomps      = new AQCOMP*[size+blocksize];
         CARDINAL* newzoneinds   = new CARDINAL[size+blocksize];
         REAL*     newtimes      = new REAL[size+blocksize];
         REAL*     newspaces     = new REAL[size+blocksize];
         BOOLEAN*  newspacerels  = new BOOLEAN[size+blocksize];
         for ( i=0; i<size; i++ )
         {
            newplotdef_ok[i] = plotdef_ok[i];
            newvars[i]       = vars[i];
            newcalcnums[i]   = calcnums[i];
            newcomps[i]      = comps[i];
            newzoneinds[i]   = zoneinds[i];
            newtimes[i]      = times[i];
            newspaces[i]     = spaces[i];
            newspacerels[i]  = spacerels[i];
         }
         for ( i=size; i<size+blocksize; i++ )
         {
            newplotdef_ok[i] = FALSE;
            newvars[i]       = 0;
            newcalcnums[i]   = 0;
            newcomps[i]      = 0;
            newzoneinds[i]   = 0;
            newtimes[i]      = 0;
            newspaces[i]     = 0;
            newspacerels[i]  = FALSE;
         }
         delete [] plotdef_ok; plotdef_ok = newplotdef_ok;
         delete [] vars;       vars       = newvars;
         delete [] calcnums;   calcnums   = newcalcnums;
         delete [] comps;      comps      = newcomps;
         delete [] zoneinds;   zoneinds   = newzoneinds;
         delete [] times;      times      = newtimes;
         delete [] spaces;     spaces     = newspaces;
         delete [] spacerels;  spacerels  = newspacerels;
         size += blocksize;
      }
      plotdef_ok[numdefs] = TRUE;
      vars[numdefs] = GetVar(args[0]);
      if ( vars[numdefs] != 0 )
      {
         if ( StringToNum(args[1],&calcnums[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         comps[numdefs] = GetComp(args[2]);
         if ( comps[numdefs] == 0 )
            plotdef_ok[numdefs] = FALSE;
         if ( StringToNum(args[3],&zoneinds[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         if ( StringToNum(args[4],&times[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         if ( StringToNum(args[5],&spaces[numdefs]) == FALSE ) 
            plotdef_ok[numdefs] = FALSE;
         if ( args[6][0] == 'r' || args[6][0] == 'R' ) 
            spacerels[numdefs] = TRUE;
         numdefs++;
      }
      for ( i=0; i<numarg; i++ ) delete args[i];
      delete [] args;
   }
   if ( numdefs == 0 )
   {
      if ( writelog == TRUE )
      {
         *cjob << "\n\n*** " << "No result definitions specified";
         cjob->flush();
      }
      return PROBLEM;
   }

   if ( writelog == TRUE )
   {
       *cjob << "\n\n   "; 
       *cjob << "Number of parameters:         " << npars;
       *cjob << "\n   "; 
       *cjob << "Number of result definitions: " << numdefs;
       *cjob << "\n";
       cjob->flush();
   }

   REAL*    resvals  = new REAL[numdefs];
   BOOLEAN* resavail = new BOOLEAN[numdefs];
   BOOLEAN ok = TRUE;
   for ( i=0; i<npars; i++ )
   {
      if ( parvars[i]->SetValue(parvals[i]) == FALSE )
      {
         ok = FALSE;
      }
   }
   char Buf[BufSize];
   if ( ok == TRUE )
   {
	  if ( InitSim(FALSE) != OK )
      {
         TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
         *cjob << "\n   " << Buf << "  "
               << "Problem during initialization";
         ok = FALSE;
      }
      else
      {
         if ( Simulate(FALSE) != OK )
         {
            TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
            *cjob << "\n   " << Buf << "  "
                  << "Problem during calculation";
            ok = FALSE;
         }
         else
         {
            TimeToString((aquafitsys->ini).T(14),Buf,BufSize);
            *cjob << "\n   " << Buf << "  "
                  << "simulation successfully finished";
            cjob->flush();

            // calculate results

            REAL value = 0;
            for ( i=0; i<numdefs; i++ )
            {
               if ( plotdef_ok[i] == TRUE )
               {
                  if ( GetResult(vars[i],calcnums[i],comps[i],
                                 zoneinds[i],times[i],spaces[i],
                                 spacerels[i],resvals[i]) == TRUE )
                  {
                     resavail[i] = TRUE;
                  }
                  else
                  {
                     resavail[i] = FALSE;
                  }
               }
            }
         }
      }
   }
   else
   {
      for ( i=0; i<numdefs; i++ ) resavail[i] = FALSE;
   }

   for ( i=0; i<numdefs; i++ )
   {
      out << vars[i]->Symbol()  << "_" 
		  << calcnums[i]        << "_"
		  << comps[i]->Symbol() << "_"
		  << zoneinds[i]        << "_"
		  << times[i]           << "_"
		  << spaces[i]          << "_";
	  if ( spacerels[i] == FALSE ) out << "abs";
	  else                         out << "rel";
      out << "\t";
	  if ( resavail[i] == TRUE ) out << resvals[i];
	  else                       out << "NA";
	  out << "\n";
   }
   out.flush();

   if ( writelog == TRUE )
   {
      *cjob << "\n\n   ";
      *cjob << "Number of parameters:                     " << npars;
      *cjob << "\n   ";
      *cjob << "Number of result definitions:             " << numdefs;
      *cjob << "\n   ";
   }
   cjob->flush();
   delete [] resvals;
   delete [] resavail;
   delete [] parvals;
   delete [] parvars;
   delete [] plotdef_ok;
   delete [] vars;
   delete [] calcnums;
   delete [] comps;
   delete [] zoneinds;
   delete [] times;
   delete [] spaces;
   delete [] spacerels;
   if ( ok == FALSE ) return PROBLEM;
   return OK;
}


JOBSTATUS AQUASYS::MaxPost(std::istream& inpri, std::ostream& out, 
						   std::ostream& outvar, BOOLEAN writelog)
{
   char Buf[BufSize];
   CARDLONG i,j;
   char** words;
   CARDINAL numwords = 0;

   aquafitsys = this;
   DeleteStates();
   fitout = 0;
   JOBSTATUS status = CollectFitData(writelog);
   if ( status != OK ) return status;

   /////////////////////
   // read prior file //
   /////////////////////

   // read distribution type:
   // -----------------------

   numwords = ReadWords(inpri,words);
   if ( numwords==0 )
   {
      *cjob << "\n*** ERROR: unable to read distribution type\n";
      return PROBLEM;
   }
   if ( dist.SetType(words[0]) == FALSE )
   {
      *cjob << "\n*** ERROR: unknown distribution type: \""
            << words[0] << "\"\n";
      return PROBLEM;
   }
   DeleteWords(numwords,words);

   // read distribution parameters:
   // -----------------------------

   while ( 1 )
   {
      CARDINAL numvar = dist.GetDim();
      numwords = ReadWords(inpri,words);
      if ( numwords == 0 ) break;
      REAL* pars = 0;
      char* marginal = 0;
      CARDINAL numpars = 0;
      CARDINAL offset = 1;
      if ( dist.GetType() == MV_COPULA )
      {
	 offset++;
	 if ( numwords < offset )
         {
            *cjob << "\n*** ERROR: unable to find marginal type"
	          << " of variable \"" << words[0] << "\"\n";
            return PROBLEM;
         }
         marginal = words[offset-1];
      }
      numpars = numwords - offset;
      pars = new REAL[numpars];
      for ( i=0; i<numpars; i++ )
      {
         if ( StringToNum(words[offset+i],&pars[i]) == FALSE )
         {
            *cjob << "\n*** ERROR: unable to read parameter " << i+1
        	  << " of variable \"" << words[0] << "\"\n";
            return PROBLEM;
         }
      }
      if ( dist.AddVar(words[0],marginal,numpars,pars) == FALSE )
      {
         *cjob << "\n*** ERROR: problem with definitions"
               << " for variable \"" << words[0] << "\"\n";
         return PROBLEM;
      }
      DeleteWords(numwords,words);
   }
   CARDINAL numvar = dist.GetDim();
   if ( numvar == 0 )
   {
      *cjob << "\n*** ERROR: no variables found\n";
      return PROBLEM;
   }
   *cjob << "\n\n";
   *cjob << "\nPrior distribution:";
   *cjob << "\n-------------------\n";
   dist.Write(cjob);
   *cjob << "\n\n";

   //////////////////////////////////////////////////////////////
   // check existence of constant variables and assign fitvars //
   //////////////////////////////////////////////////////////////

   REAL* curvals = new REAL[numvar];
   REAL* minvals = new REAL[numvar];
   REAL* maxvals = new REAL[numvar];
   delete fitvars; fitvars = 0;
   fitvars = new CONSTVAR*[numvar];
   for ( i=0; i<numvar; i++ )
   {
      AQVAR* var = GetVar(dist.GetVarName(i));
      if ( var == 0 )
      {
         *cjob << "\n*** ERROR: variable " << dist.GetVarName(i)
               << " not found\n";
         return PROBLEM;
      }
      if ( var->Type() != ConstVar )
      {
         *cjob << "\n*** ERROR: variable " << dist.GetVarName(i)
               << " not a Constant Variable\n";
         return PROBLEM;
      }
      fitvars[i] = (CONSTVAR*)var;
      curvals[i] = fitvars[i]->Evaluate();
      minvals[i] = fitvars[i]->Minimum();
      maxvals[i] = fitvars[i]->Maximum();
   }

   ////////////////////////////////////////////
   // calculate maximum of posterior density //
   ////////////////////////////////////////////

   *cjob << "\nPosterior calculations:";
   *cjob << "\n-----------------------";
   TimeToString(ini.T(14),Buf,BufSize);                      // StartCalc
   *cjob << "\n\n   " << Buf << "  " << ini.T(15);
   cjob->flush();
   CARDINAL maxiter = 100;
   REAL     ftol = numpar.FitTol();
   NUMINFO  numinfo;
   REAL     minuslogpostpdf;
   for ( i=0; i<numvar; i++ )
   {
      out << dist.GetVarName(i) << "\t";
   }
   numvarsets    = 0;
   varvals       = 0;
   pripdfvals    = 0;
   loglikelivals = 0;
   status = MinSimplex(curvals,minvals,maxvals,numvar,
                                 MinusLogPostPDF,ftol,maxiter,
                                 &minuslogpostpdf,numinfo);
   REAL* siginv  = new REAL[numvar*numvar];
   if ( numvarsets > 0 )
   {
      // estimate variance-covariance matrix:

      REAL  refval  = minuslogpostpdf;
      REAL* inc     = new REAL[numvar];
      for ( i=0; i<numvar; i++ ) inc[i] = 0.001*(maxvals[i]-minvals[i]);
      for ( i=0; i<numvar; i++ )
      {
         siginv[i+i*numvar] = -2*refval;
         curvals[i] += inc[i];
         if ( MinusLogPostPDF(curvals,numvar,&minuslogpostpdf) != OK )
         {
            *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
            return PROBLEM;
         }
         curvals[i] -= inc[i];
         siginv[i+i*numvar] += minuslogpostpdf;
         curvals[i] -= inc[i];
         if ( MinusLogPostPDF(curvals,numvar,&minuslogpostpdf) != OK )
         {
            *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
            return PROBLEM;
         }
         curvals[i] += inc[i];
         siginv[i+i*numvar] += minuslogpostpdf;
         siginv[i+i*numvar] = siginv[i+i*numvar]/(inc[i]*inc[i]);
         for ( j=0; j<i; j++ )
         {
            curvals[i] += inc[i];
            curvals[j] += inc[j];
            if ( MinusLogPostPDF(curvals,numvar,&minuslogpostpdf) != OK )
            {
               *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
               return PROBLEM;
            }
            curvals[i] -= inc[i];
            curvals[j] -= inc[j];
            siginv[i+j*numvar] = minuslogpostpdf;
            curvals[i] += inc[i];
            curvals[j] -= inc[j];
            if ( MinusLogPostPDF(curvals,numvar,&minuslogpostpdf) != OK )
            {
               *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
               return PROBLEM;
            }
            curvals[i] -= inc[i];
            curvals[j] += inc[j];
            siginv[i+j*numvar] -= minuslogpostpdf;
            curvals[i] -= inc[i];
            curvals[j] += inc[j];
            if ( MinusLogPostPDF(curvals,numvar,&minuslogpostpdf) != OK )
            {
               *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
               return PROBLEM;
            }
            curvals[i] += inc[i];
            curvals[j] -= inc[j];
            siginv[i+j*numvar] -= minuslogpostpdf;
            curvals[i] -= inc[i];
            curvals[j] -= inc[j];
            if ( MinusLogPostPDF(curvals,numvar,&minuslogpostpdf) != OK )
            {
               *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
               return PROBLEM;
            }
            curvals[i] += inc[i];
            curvals[j] += inc[j];
            siginv[i+j*numvar] += minuslogpostpdf;
            siginv[i+j*numvar] = siginv[i+j*numvar]/(4*inc[i]*inc[j]);
            siginv[j+i*numvar] = siginv[i+j*numvar];
         }
      }
      INTEGER*  ipvt = new INTEGER[numvar];
      REAL*     work = new REAL[numvar];
      REAL rcond;
      dgeco(siginv,numvar,numvar,ipvt,&rcond,work);
      REAL det[2];
      if ( 1 + rcond == 1 )
      {
         *cjob << "\n*** ERROR: unable to estimate covariance matrix\n";
         return PROBLEM;
      }
      dgedi(siginv,numvar,numvar,ipvt,det,work,1);

      out << "pri-pdf" << "\t"
          << "likeli"  << "\t"
          << "post-pdf" << "\t"
          << "log(post-fact)" << "\n";
      REAL logfact = -loglikelivals[0];
      for ( j=1; j<numvarsets; j++ )
      {
         if ( logfact > -loglikelivals[j] ) logfact = -loglikelivals[j];
      }
      for ( j=0; j<numvarsets; j++ )
      {
         for ( i=0; i<numvar; i++ )
         {
            out << varvals[j][i] << "\t";
         }
         out << pripdfvals[j] << "\t"
             << exp(loglikelivals[j]+logfact) << "\t"
             << pripdfvals[j]*exp(loglikelivals[j]+logfact) << "\t"
             << logfact << "\n";
      }
   }
   if ( status == OK )
   {
      TimeToString(ini.T(14),Buf,BufSize);                      // EndCalc
      *cjob << "\n   " << Buf << "  " << ini.T(16);
      *cjob << "\n\n";
      *cjob << "\nSummary statistics:";
      *cjob << "\n-------------------\n";
      *cjob << "\n   number of steps performed:      " << numinfo.NumStep();
      *cjob << "\n   number of function evaluations: " << numinfo.NumFun()+2*numvar*numvar;
   }
   else
   {
      *cjob << "\n*** ERROR: problem during maximization\n";
      return PROBLEM;
   }

   outvar << "maxpost";
   for ( i=0; i<numvar; i++ ) outvar << "\t" << dist.GetVarName(i);
   outvar << "\nargmax";
   for ( i=0; i<numvar; i++ ) outvar << "\t" << curvals[i];
   outvar << "\nstdev";
   for ( i=0; i<numvar; i++ ) outvar << "\t" << sqrt(siginv[i+i*numvar]);
   for ( i=0; i<numvar; i++ )
   {
      outvar << "\ncorr_" << dist.GetVarName(i); 
      for ( j=0; j<numvar; j++ )
      {
         outvar << "\t" << siginv[i+j*numvar]
                           /sqrt(siginv[i+i*numvar]*siginv[j+j*numvar]);
      }
   }
   for ( i=0; i<numvar; i++ )
   {
      outvar << "\nkendcorr_" << dist.GetVarName(i); 
      for ( j=0; j<numvar; j++ )
      {
         outvar << "\t" << 2/3.1415926
                           *asin(siginv[i+j*numvar]
                                 /sqrt(siginv[i+i*numvar]
                                 *siginv[j+j*numvar]));
      }
   }

   return OK;
}


JOBSTATUS AQUASYS::MCMC(std::istream& inpri, std::istream& injmp,
						std::ostream& out, BOOLEAN writelog)
{
   char Buf[BufSize];
   CARDLONG i,j;
   char** words;
   CARDINAL numwords = 0;

   aquafitsys = this;
   DeleteStates();
   fitout = 0;
   JOBSTATUS status = CollectFitData(writelog);
   if ( status != OK ) return status;

   /////////////////////
   // read prior file //
   /////////////////////

   // read distribution type:
   // -----------------------

   numwords = ReadWords(inpri,words);
   if ( numwords==0 )
   {
      *cjob << "\n*** ERROR: unable to read distribution type\n";
      return PROBLEM;
   }
   if ( dist.SetType(words[0]) == FALSE )
   {
      *cjob << "\n*** ERROR: unknown distribution type: \""
            << words[0] << "\"\n";
      return PROBLEM;
   }
   DeleteWords(numwords,words);

   // read distribution parameters:
   // -----------------------------

   while ( 1 )
   {
      CARDINAL numvar = dist.GetDim();
      numwords = ReadWords(inpri,words);
      if ( numwords == 0 ) break;
      REAL* pars = 0;
      char* marginal = 0;
      CARDINAL numpars = 0;
      CARDINAL offset = 1;
      if ( dist.GetType() == MV_COPULA )
      {
	 offset++;
	 if ( numwords < offset )
         {
            *cjob << "\n*** ERROR: unable to find marginal type"
	          << " of variable \"" << words[0] << "\"\n";
            return PROBLEM;
         }
         marginal = words[offset-1];
      }
      numpars = numwords - offset;
      pars = new REAL[numpars];
      for ( i=0; i<numpars; i++ )
      {
         if ( StringToNum(words[offset+i],&pars[i]) == FALSE )
         {
            *cjob << "\n*** ERROR: unable to read parameter " << i+1
        	  << " of variable \"" << words[0] << "\"\n";
            return PROBLEM;
         }
      }
      if ( dist.AddVar(words[0],marginal,numpars,pars) == FALSE )
      {
         *cjob << "\n*** ERROR: problem with definitions"
               << " for variable \"" << words[0] << "\"\n";
         return PROBLEM;
      }
      DeleteWords(numwords,words);
   }
   CARDINAL numvar = dist.GetDim();
   if ( numvar == 0 )
   {
      *cjob << "\n*** ERROR: no variables found\n";
      return PROBLEM;
   }
   *cjob << "\n\n";
   *cjob << "\nPrior distribution:";
   *cjob << "\n-------------------\n";
   dist.Write(cjob);
   *cjob << "\n\n";
   cjob->flush();

   //////////////////////////////////////////////////////////////
   // check existence of constant variables and assign fitvars //
   //////////////////////////////////////////////////////////////

   REAL* inivals = new REAL[numvar];
   REAL* minvals = new REAL[numvar];
   REAL* maxvals = new REAL[numvar];
   delete fitvars; fitvars = 0;
   fitvars = new CONSTVAR*[numvar];
   for ( i=0; i<numvar; i++ )
   {
      AQVAR* var = GetVar(dist.GetVarName(i));
      if ( var == 0 )
      {
         *cjob << "\n*** ERROR: variable " << dist.GetVarName(i)
               << " not found\n";
         return PROBLEM;
      }
      if ( var->Type() != ConstVar )
      {
         *cjob << "\n*** ERROR: variable " << dist.GetVarName(i)
               << " not a Constant Variable\n";
         return PROBLEM;
      }
      fitvars[i] = (CONSTVAR*)var;
      inivals[i] = fitvars[i]->Evaluate();
      minvals[i] = fitvars[i]->Minimum();
      maxvals[i] = fitvars[i]->Maximum();
   }

   /////////////////////////////////
   // read jump distribution file //
   /////////////////////////////////

   // read to varnames:
   // -----------------

   while ( 1 )
   {
      numwords = ReadWords(injmp,words);
      if ( numwords == 0 )
      {
         *cjob << "\n*** ERROR: variables not found on jump data file\n";
         return PROBLEM;
      }
      if ( numwords >= numvar )
      {
         BOOLEAN varnamesfound = TRUE;
         for ( i=0; i<numvar; i++ )
         {
            if ( strcmp(words[i],dist.GetVarName(i)) != 0 )
            {
               varnamesfound = FALSE;
               break;
            }
         }
         if ( varnamesfound == TRUE ) break;
      }
      DeleteWords(numwords,words);
   }

   // read jump data sets:
   // --------------------

   CARDLONG numjumpsets = 0;
   CARDLONG chunksize = 100;
   REAL** jumpvals = new REAL*[chunksize];
   for ( i=0; i<chunksize; i++ ) jumpvals[i] = new REAL[numvar];
   CARDLONG maxsets = chunksize;
   while ( 1 )
   {
      numwords = ReadWords(injmp,words);
      if ( numwords == 0 ) break;
      if ( numjumpsets == maxsets )
      {
         REAL** newjumpvals = new REAL*[numjumpsets+chunksize];
         for ( i=0; i<numjumpsets; i++ )
         {
            newjumpvals[i] = jumpvals[i];
         }
         for ( i=numjumpsets; i<numjumpsets+chunksize; i++ ) newjumpvals[i] = new REAL[numvar];
         delete [] jumpvals; jumpvals = newjumpvals;
         maxsets = numjumpsets + chunksize;
      }
      if ( numwords < numvar )
      {
         *cjob << "\n*** ERROR: jump data set " << numjumpsets+1
	       << " does not contain enough values\n";
         return PROBLEM;
      }
      for ( i=0; i<numvar; i++ )
      {
         if ( StringToNum(words[i],&jumpvals[numjumpsets][i]) == FALSE )
         {
            *cjob << "\n*** ERROR: jump data set " << numjumpsets+1
                  << " contains an unreadable value\n";
            return PROBLEM;
         }
      }
      numjumpsets++;
      DeleteWords(numwords,words);
   }
   *cjob << "\nJump sample:";
   *cjob << "\n------------\n";
   *cjob << "\n   number of jump samples read: " << numjumpsets;
   cjob->flush();
   if ( numjumpsets == 0 )
   {
      *cjob << "\n*** ERROR: no jump data found\n";
      return PROBLEM;
   }

   ////////////////////////////////////////////
   // Calculation and output of Markov chain //
   ////////////////////////////////////////////

   REAL priold, prinew;
   REAL chiold, chinew;
   REAL loglikeliold, loglikelinew;
   REAL logpostold, logpostnew;
   
   *cjob << "\n\n";
   *cjob << "\nPosterior calculations:";
   *cjob << "\n-----------------------";
   TimeToString(ini.T(14),Buf,BufSize);                      // StartCalc
   *cjob << "\n\n   " << Buf << "  " << ini.T(15);
   cjob->flush();
   for ( i=0; i<numvar; i++ ) 
   {
      out << dist.GetVarName(i) << "\t";
   }
   out << "pri-pdf" << "\t" << "likeli" << "\t" << "post-pdf"
       << "\t" << "log(post-fact)";
   out << "\n";
   out.flush();

   if ( mv_pdf(dist,inivals,&priold,cjob) == FALSE )
   {
      return PROBLEM;
   }

   status = fitfun(inivals,numvar,fcal,ndat);
   if ( status != OK )
   {
      *cjob << "\n*** ERROR: problem calculating initial chi^2\n";
      return PROBLEM;
   }
   chiold = chicurrent;
   loglikeliold = -0.5*chiold;

   logpostold = log(priold)+loglikeliold;

   *cjob << "   pri-pdf=" <<  priold << "   log(post-pdf)=" << logpostold;
   cjob->flush();

   for ( i=0; i<numvar; i++ ) out << inivals[i] << "\t";
   out << priold << "\t" << 1 << "\t" << priold << "\t" << -loglikeliold;
   out << "\n"; out.flush();

   CARDLONG numsteps = 0;
   CARDLONG numsucc  = 0;
   CARDLONG numprob  = 0;
   REAL* oldvals = new REAL[numvar];
   REAL* newvals = new REAL[numvar];
   for ( i=0; i<numvar; i++ ) oldvals[i] = inivals[i];
   for ( j=0; j<numjumpsets; j++ )
   {
      for ( i=0; i<numvar; i++ ) newvals[i] = oldvals[i]+jumpvals[j][i];
      BOOLEAN withinrange = TRUE;
      for ( i=0; i<numvar; i++ )
      {
         if ( newvals[i]<minvals[i] || newvals[i]>maxvals[i] )
         {
            withinrange = FALSE; break;
         }
      }
      BOOLEAN noproblem = TRUE;
      if ( withinrange == FALSE )
      {
         prinew = 0;
      }
      else
      {
		  noproblem = mv_pdf(dist,newvals,&prinew,&std::cout);
      }
      if ( noproblem==TRUE && prinew>0 )
      {
         if ( fitfun(newvals,numvar,fcal,ndat) != OK ) noproblem = FALSE;
         chinew = chicurrent;
         if ( noproblem == TRUE )
         {
            loglikelinew = -0.5*chinew;
            logpostnew = log(prinew)+loglikelinew;

            *cjob << "   pri-pdf=" <<  prinew << "   log(post-pdf)=" << logpostnew;
            cjob->flush();

            BOOLEAN success = FALSE;
            if ( logpostnew >= logpostold )
            {
               success = TRUE;
            }
            else
            {
               REAL r;
               uniform_randnum(&r);
               if ( r <= prinew/priold*exp(loglikelinew-loglikeliold) )
               {
                  success = TRUE;
               }
            }
            if ( success == TRUE )
            {
               for ( i=0; i<numvar; i++ ) oldvals[i] = newvals[i];
               priold       = prinew;
               chiold       = chinew;
               loglikeliold = loglikelinew;
               logpostold   = logpostnew;
               numsucc++;
            }
         }
      }
      if ( noproblem == TRUE )
      {
         numsteps++;
         for ( i=0; i<numvar; i++ ) out << oldvals[i] << "\t";
         out << priold << "\t" << 1 << "\t" << priold << "\t" << -loglikeliold;
         out << "\n"; out.flush();
      }
      else
      {
         numprob++;
      }
   }
   delete [] oldvals;
   delete [] newvals;

   TimeToString(ini.T(14),Buf,BufSize);                      // EndCalc
   *cjob << "\n   " << Buf << "  " << ini.T(16);
   *cjob << "\n\n";
   *cjob << "\nSummary statistics:";
   *cjob << "\n-------------------\n";
   *cjob << "\n   number of steps:                              " << numsteps;
   *cjob << "\n   number of accepted proposals:                 " << numsucc;
   *cjob << "\n   number of problems evaluating prior or chi^2: " << numprob;
   *cjob << "\n   acceptance ratio:                             " << (REAL)numsucc/(REAL)numsteps;
   cjob->flush();

   return OK;
}


JOBSTATUS AQUASYS::IdAnal(std::istream& cmdfile, std::ostream& fitfile, 
                          BOOLEAN writelog)
{
   cjob->precision(ini.I(3));
   fitfile.precision(ini.I(4));
   PrintWidth(0);
   CARDINAL field = abs(ini.I(4))+8;
   CARDINAL numjobs = 0;

   while ( 1 )  // loop for job sections
   {
	  // turn off active variables:
      AQVAR* var = varsys.Varlist()->First();
      while ( var != 0 )
      {
         if ( var->Type() == ConstVar )
         {
            CONSTVAR* cv = (CONSTVAR*)var;
            cv->FitActive(FALSE);
         }
         var = var->Next();
      }

      // open output file
      char linebuf[4096];
      BOOLEAN found = FALSE;
      while ( 1 )
      {
         if ( !cmdfile.getline(linebuf,sizeof(linebuf)) ) break;
         if ( linebuf[0] > ' ' )
         {
            found = TRUE;
            for ( CARDINAL i=0; i<strlen(linebuf); i++ )
            {
               if ( linebuf[i] == ' ' )
               {
                  linebuf[i] = '\0';
                  break;
               }
            }
            break;
         }
      }
      if ( found == FALSE ) break;
	  std::ofstream out;
	  out.open(linebuf,std::ios::out|std::ios::trunc);
      if ( out.fail() )
      {
         if ( writelog == TRUE )
         {
            *cjob << "\n\n*** " << "error opening file" << ": " << linebuf;
            cjob->flush();
         }
         return PROBLEM;
      }

      CARDINAL   numvars = 0;
      CARDINAL   numvarvars = 0;
      CARDINAL   numfitvars = 0;
      CONSTVAR** vars = 0;
      CONSTVAR** varvars = 0;
      CONSTVAR** fitvars = 0;
      REAL*      vals = 0;
      REAL*      mins = 0;
      REAL*      maxs = 0;
      CARDINAL*  numvals = 0;
      BOOLEAN ok = TRUE;
      while ( 1 )  // loop for parameters
      {
         if ( !cmdfile.getline(linebuf,sizeof(linebuf)) ) break;
         if ( linebuf[0] <= ' ' ) break;
         if ( linebuf[0]=='V' || linebuf[0]=='v' )
         {
            char namebuf[2048];
            CARDINAL pos = 1;
            if ( ExtractName(linebuf,pos,namebuf) == FALSE )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "illegal command line: "
                        << linebuf;
                  cjob->flush();
               }
               ok = FALSE; break;
            }
            AQVAR* var = (varsys.Varlist())->Get(namebuf);
            if ( var == 0 )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "illegal command line: "
                        << linebuf;
                  cjob->flush();
               }
               ok = FALSE; break;
            }
            if ( var->Type() != ConstVar )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "variable "
                        << namebuf << " is not a constant variable";
                  cjob->flush();
               }
               ok = FALSE; break;
            }
            CONSTVAR* cv = (CONSTVAR*)var;
			REAL mini, maxi;
            CARDINAL num;
            CARDINAL i = 0;
            while ( linebuf[pos+i] != '\0' )
            {
               linebuf[i] = linebuf[pos+i];
               i++;
            }
            linebuf[i] = '\0';
			std::istrstream inputline(linebuf,sizeof(linebuf));
            inputline >> mini;
            if ( inputline.fail() )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "unable to read minimum of variable "
                        << var->Symbol();
                  cjob->flush();
               }
               ok = FALSE; break;
            }
            inputline >> maxi;
            if ( inputline.fail() )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "unable to read maximum of variable "
                        << var->Symbol();
                  cjob->flush();
               }
               ok = FALSE; break;
            }
            inputline >> num;
            if ( inputline.fail() )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "unable to read number of values of variable "
                        << var->Symbol();
                  cjob->flush();
               }
               ok = FALSE; break;
            }
            CONSTVAR** newvars    = new CONSTVAR*[numvars+1];
            REAL*      newvals    = new REAL[numvars+1];
            CONSTVAR** newvarvars = new CONSTVAR*[numvarvars+1];
            REAL*      newmins    = new REAL[numvarvars+1];
            REAL*      newmaxs    = new REAL[numvarvars+1];
            CARDINAL*  newnumvals = new CARDINAL[numvarvars+1];
            for ( i=0; i<numvars; i++ )
            {
               newvars[i] = vars[i];
               newvals[i] = vals[i];
            }
            newvars[numvars] = cv;
            newvals[numvars] = cv->Evaluate();
            delete [] vars; vars = newvars;
            delete [] vals; vals = newvals;
            numvars++;
            for ( i=0; i<numvarvars; i++ )
            {
               newvarvars[i] = varvars[i];
               newmins[i]    = mins[i];
               newmaxs[i]    = maxs[i];
               newnumvals[i] = numvals[i];
            }
            newvarvars[numvarvars] = cv;
            newmins[numvarvars]    = mini;
            newmaxs[numvarvars]    = maxi;
            newnumvals[numvarvars] = num;
            delete [] varvars; varvars = newvarvars;
            delete [] mins; mins = newmins;
            delete [] maxs; maxs = newmaxs;
            delete [] numvals; numvals = newnumvals;
            numvarvars++;
         }
         else
         {
            if ( linebuf[0]=='C' || linebuf[0]=='c' )
            {
               char namebuf[2048];
               CARDINAL pos = 1;
               if ( ExtractName(linebuf,pos,namebuf) == FALSE )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "illegal command line: "
                           << linebuf;
                     cjob->flush();
                  }
                  ok = FALSE; break;
               }
               AQVAR* var = (varsys.Varlist())->Get(namebuf);
               if ( var == 0 )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "illegal command line: "
                           << linebuf;
                     cjob->flush();
                  }
                  ok = FALSE; break;
               }
               if ( var->Type() != ConstVar )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "variable "
                           << namebuf << " is not a constant variable";
                     cjob->flush();
                  }
                  ok = FALSE; break;
               }
               CONSTVAR* cv = (CONSTVAR*)var;
               CONSTVAR** newvars    = new CONSTVAR*[numvars+1];
               REAL*      newvals    = new REAL[numvars+1];
               for ( CARDINAL i=0; i<numvars; i++ )
               {
                  newvars[i] = vars[i];
                  newvals[i] = vals[i];
               }
               newvars[numvars] = cv;
               newvals[numvars] = cv->Evaluate();
               delete [] vars; vars = newvars;
               delete [] vals; vals = newvals;
               numvars++;
			}
            else
            {
               if ( linebuf[0]=='F' || linebuf[0]=='f' )
               {
                  char namebuf[2048];
                  CARDINAL pos = 1;
                  if ( ExtractName(linebuf,pos,namebuf) == FALSE )
                  {
                     if ( writelog == TRUE )
                     {
                        *cjob << "\n\n*** " << "illegal command line: "
                              << linebuf;
                        cjob->flush();
                     }
                     ok = FALSE; break;
                  }
                  AQVAR* var = (varsys.Varlist())->Get(namebuf);
                  if ( var == 0 )
                  {
                     if ( writelog == TRUE )
                     {
                        *cjob << "\n\n*** " << "illegal command line: "
                              << linebuf;
                        cjob->flush();
                     }
                     ok = FALSE; break;
                  }
                  if ( var->Type() != ConstVar )
                  {
                     if ( writelog == TRUE )
                     {
                        *cjob << "\n\n*** " << "variable "
                              << namebuf << " is not a constant variable";
                        cjob->flush();
                     }
                     ok = FALSE; break;
                  }
                  CONSTVAR* cv = (CONSTVAR*)var;
                  cv->FitActive(TRUE);
                  CONSTVAR** newvars    = new CONSTVAR*[numvars+1];
                  REAL*      newvals    = new REAL[numvars+1];
                  CONSTVAR** newfitvars = new CONSTVAR*[numfitvars+1];
                  CARDINAL i;
                  for ( i=0; i<numvars; i++ )
                  {
                     newvars[i] = vars[i];
                     newvals[i] = vals[i];
                  }
                  newvars[numvars] = cv;
                  newvals[numvars] = cv->Evaluate();
                  delete [] vars; vars = newvars;
                  delete [] vals; vals = newvals;
                  numvars++;
                  for ( i=0; i<numfitvars; i++ )
                  {
                     newfitvars[i] = fitvars[i];
                  }
                  newfitvars[numfitvars] = cv;
                  delete [] fitvars; fitvars = newfitvars;
                  numfitvars++;
			   }
               else
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "illegal command line: "
                            << linebuf;
                     cjob->flush();
                  }
                  ok = FALSE; break;
               }
            }
         }
      }	 // end loop for parameters

      // check parameters:
      CARDINAL i,j;
      for ( i=0; i<numvars; i++ )
      {
         for ( j=i+1; j<numvars; j++ )
         {
            if ( vars[i] == vars[j] )
            {
               if ( writelog == TRUE )
               {
                  *cjob << "\n\n*** " << "variable " << vars[i]->Symbol()
                        << " appears more than once";
                  cjob->flush();
               }
               ok = FALSE;
            }
         }
      }
      CARDINAL varind1 = numvarvars;
      CARDINAL varind2 = numvarvars;
      for ( i=0; i<numvarvars; i++ )
      {
         if ( numvals[i] < 1 )
         {
            if ( writelog == TRUE )
            {
               *cjob << "\n\n*** " << "number of values less than 1";
               cjob->flush();
            }
            ok = FALSE;
         }
         else
         {
            if ( numvals[i] == 1 )
            {
               if ( varvars[i]->SetValue(0.5*(mins[i]+maxs[i])) == FALSE )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "value for variable "
                           << varvars[i]->Symbol() << " out of range";
                     cjob->flush();
                  }
                  ok = FALSE;
               }
            }
            else
            {
               if ( varind2 < numvarvars )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "only two varying variables with more than one value allowed!";
                     cjob->flush();
                  }
                  ok = FALSE;
               }
               else
               {
                  if ( varind1 == numvarvars )
                  {
                     varind1 = i;
                  }
                  else
                  {
                     varind2 = i;
                  }
               }
            }
         }
      }
      
	  // perform calculations:
      if ( ok == TRUE )
      {
         if ( PrintLn(out,'*') == FALSE )                     return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;
         if ( Print(out,ProgName()) == FALSE )                return PROBLEM;
         if ( Print(out," ") == FALSE )                       return PROBLEM;
         if ( Print(out,ProgVersion()) == FALSE )             return PROBLEM;
         if ( Print(out," - ") == FALSE )                     return PROBLEM;
         if ( Print(out,"Identifiability Analysis") == FALSE )return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;
         if ( PrintLn(out,'*') == FALSE )                     return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;
         if ( PrintTime(out,ini.T(124)) == FALSE )            return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;
         if ( PrintLn(out) == FALSE )                         return PROBLEM;

         Print(out,"Results",1); Print(out,":");
         PrintLn(out); PrintLn(out);
         for ( i=0; i<numvars; i++ )
         {
            Print(out,vars[i]->Symbol(),i*field+2);
         }
         Print(out,ini.T(5),numvars*field+2);
         for ( i=0; i<numvars; i++ )
         {
            Print(out,"[",i*field+2);
            Print(out,vars[i]->Unit());
            Print(out,"]");
         }
         PrintLn(out);

         CARDINAL numsteps;
         JOBSTATUS status;
         if ( varind1 < numvarvars )
         {
            for ( i=0; i<numvals[varind1]; i++ )
            {
               REAL val = mins[varind1]
                          +i*(maxs[varind1]-mins[varind1])
                             /(numvals[varind1]-1);
               if ( varvars[varind1]->SetValue(val) == FALSE )
               {
                  if ( writelog == TRUE )
                  {
                     *cjob << "\n\n*** " << "value for variable "
                           << varvars[varind1]->Symbol() << " out of range";
                     cjob->flush();
                  }
                  ok = FALSE; break;
               }
               if ( varind2 == numvarvars )
               {
                  numsteps = FitIter();
                  status = Fit(fitfile,&numsteps);
                  for ( j=0; j<numvars; j++ )
                  {
                     Print(out,vars[j]->Evaluate(),j*field+2);
                  }
                  if ( status==OK || numsteps==FitIter() )
                  {
                     Print(out,chiend,numvars*field+2);
                  }
                  else
                  {
                     Print(out,NotAvail,numvars*field+2);
                  }
                  PrintLn(out);
               }
               else
               {
                  for ( j=0; j<numvals[varind2]; j++ )
                  {
                     REAL val = mins[varind2]
                                +j*(maxs[varind2]-mins[varind2])
                                   /(numvals[varind2]-1);
                     if ( varvars[varind2]->SetValue(val) == FALSE )
                     {
                        if ( writelog == TRUE )
                        {
                           *cjob << "\n\n*** " << "value for variable "
                                 << varvars[varind2]->Symbol() << " out of range";
                           cjob->flush();
                        }
                        ok = FALSE; break;
                     }
                     numsteps = FitIter();
                     status = Fit(fitfile,&numsteps);
                     for ( CARDINAL k=0; k<numvars; k++ )
                     {
                        Print(out,vars[k]->Evaluate(),k*field+2);
                     }
                     if ( status==OK || numsteps==FitIter() )
                     {
                        Print(out,chiend,numvars*field+2);
                     }
                     else
                     {
                        Print(out,NotAvail,numvars*field+2);
                     }
			         PrintLn(out);
                  }
               if ( ok == FALSE ) break;
               }
            }
         }
         else
         {
            numsteps = FitIter();
            status = Fit(fitfile,&numsteps);
            for ( i=0; i<numvars; i++ )
            {
               Print(out,vars[i]->Evaluate(),i*field+2);
            }
            if ( status==OK || numsteps==FitIter() )
            {
               Print(out,chiend,numvars*field+2);
            }
            else
            {
               Print(out,NotAvail,numvars*field+2);
            }
         }
      }

      // clean up
      for ( i=0; i<numvars; i++ )
      {
         vars[i]->SetValue(vals[i]);
      }
      if ( numvars > 0 )
      {
         delete [] vars; vars = 0;
         delete [] vals; vals = 0;
      }
      if ( numvarvars > 0 )
      {
         delete [] varvars; varvars = 0;
         delete [] mins; mins = 0;
         delete [] maxs; maxs = 0;
         delete [] numvals; numvals = 0;
      }
      if ( numvarvars > 0 )
      {
         delete [] fitvars; fitvars = 0;
      }
	  out.close();
      if ( ok == FALSE ) return PROBLEM;
      numjobs++;
   }  // end loop for job sections

   if ( numjobs == 0 ) return PROBLEM;
   return OK;	
}

	
BOOLEAN AQUASYS::DeleteState(CARDINAL cn, CARDINAL statenum)
{
   if ( statesys.DeleteState(cn,statenum) == FALSE ) return FALSE;
   if ( statesys.NumStates(cn) == statenum ) init = FALSE;
   Changed();
   return TRUE;
}


BOOLEAN AQUASYS::DeleteStates(CARDINAL cn)
{
   if ( statesys.DeleteStates(cn) == FALSE ) return FALSE;
   init = FALSE;
   Changed();
   return TRUE;
}


void AQUASYS::DeleteStates()
{
   statesys.Delete();
   init = FALSE;
   Changed();
   return;
}


void AQUASYS::Delete()
{
   plotsys.Delete();
   statesys.Delete();
   fitsys.Delete();
   calcsys.Delete();
   linksys.Delete();
   compsys.Delete();
   procsys.Delete();
   varsys.Delete();
   saved = TRUE;
}


BOOLEAN AQUASYS::PlotLisOpt(PLOTLISOPT* lisopt)
{
   if ( plotsys.LisOpt(lisopt) == FALSE ) return FALSE;
   Changed();
   return TRUE;
}


CARDINAL AQUASYS::PlotFilOpt(PLOTFILOPT* filopt)
{
   CARDINAL valid=plotsys.FilOpt(filopt);
   if ( valid ) return valid;
   Changed();
   return valid;
}


CARDINAL AQUASYS::PlotScrOpt(PLOTSCROPT* scropt)
{
   CARDINAL valid=plotsys.ScrOpt(scropt);
   if ( valid ) return valid;
   Changed();
   return valid;
}


BOOLEAN AQUASYS::CheckSysFile(std::istream& in)
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::CheckSysFile");
#endif

   const char firstchar = ' ';
   char Buffer[BufSize]; Buffer[0] = '\0';

   // check program identifier:

   CARDINAL i;
   while ( 1 )
   {
      i = 0;
      while ( 1 )
      {
         char c;
         if ( !in.get(c) )                       return FALSE;
         if ( c<firstchar || i==BufSize-1 )
         {
            Buffer[i] = '\0'; break;
         }
         Buffer[i] = c;
         i++;
      }
      if ( strcmp(Buffer,ProgName()) == 0 ) break;
   }
   i = 0;
   while ( 1 )
   {
      char c;
      if ( !in.get(c) )                          return FALSE;
      if ( c<firstchar || i==BufSize-1 )
      {
         if ( i > 0 )
         {
            globversion[i] = '\0'; break;
         }
      }
      else
      {
         globversion[i] = c;
         i++;
      }
   }
   if ( strncmp(globversion,ProgVersion(),8) != 0 ) return FALSE;

   // read identifier:

   if ( LoadString(in,Buffer,BufSize) == FALSE ) return FALSE;
   if ( strcmp(Buffer,AquasysIdentifier) != 0 )  return FALSE;

   // read start marker

   if ( LoadStart(in) == FALSE )                 return FALSE;

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::CheckSysFile");
#endif

   return TRUE;
}


LOADRESULT AQUASYS::Load(std::istream& in)
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::Load");
#endif

   char Buffer[BufSize]; Buffer[0] = '\0';

   if ( CheckSysFile(in) != TRUE)                     return WrongFileType;

   if ( strncmp(globversion,ProgVersion(),11)!=0 
        && strncmp(globversion,"Version 1.1",11)!=0 
        && strncmp(globversion,"Version 2.0",11)!=0 ) return WrongFileVersion;

   Delete();
   Changed();

   // read options:

   if ( LoadString(in,Buffer,BufSize) == FALSE )        return ReadError;
   if ( strcmp(Buffer,OptionsIdentifier) != 0 )         return ReadError;
   if ( LoadStart(in) == FALSE )                        return ReadError;

   INTEGER aquavers = 0;
   CARDINAL type;
   if ( LoadNumber(in,&aquavers) == FALSE )             return ReadError;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
      {
         CARDINAL c; REAL r; BOOLEAN b;
         calcsys.Delete();
         CALC* calc = new CALC;
         calc->Symbol("OldSimDef");
         if ( LoadNumber(in,&c) == FALSE )              return ReadError;
         calc->CalcNum(c);
         if ( LoadNumber(in,&r) == FALSE )              return ReadError;
         calc->InitTime(r);
         if ( LoadBoolean(in,&b) == FALSE )             return ReadError;
         calc->InitSteady(b);
         if ( LoadNumber(in,&r) == FALSE )              return ReadError;
         if ( LoadNumber(in,&c) == FALSE )              return ReadError;
         if ( calc->AddStepSeries(r,c,calc->NumStepSeries())
                                             == FALSE ) return ReadError;
         calc->SimActive(TRUE);
         if ( AddCalc(calc) == FALSE )
         {
            delete calc;                                return ReadError;
         }
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                        return ReadError;
         type = 0;
         while ( type < NumFitMeth )
         {
            if ( strcmp(Buffer,FitMethIdentifier[type]) == 0 ) break;
            type++;
         }
         switch ( type )
         {
            case Secant:
               fitmeth = Secant;
               break;
            case Simplex:
               fitmeth = Simplex;
               break;
            default:
               return ReadError;
         }
         if ( LoadNumber(in,&fititer) == FALSE )        return ReadError;
         break;
      }
      case 2: // aquavers=2: AQUASIM 2.0delta  printshort added
      {
         CARDINAL c; REAL r; BOOLEAN b;
         calcsys.Delete();
         CALC* calc = new CALC;
         calc->Symbol("OldSimDef");
         if ( LoadNumber(in,&c) == FALSE )              return ReadError;
         calc->CalcNum(c);
         if ( LoadNumber(in,&r) == FALSE )              return ReadError;
         calc->InitTime(r);
         if ( LoadBoolean(in,&b) == FALSE )             return ReadError;
         calc->InitSteady(b);
         if ( LoadNumber(in,&r) == FALSE )              return ReadError;
         if ( LoadNumber(in,&c) == FALSE )              return ReadError;
         if ( calc->AddStepSeries(r,c,calc->NumStepSeries())
                                             == FALSE ) return ReadError;
         calc->SimActive(TRUE);
         if ( AddCalc(calc) == FALSE )
         {
            delete calc;                                return ReadError;
         }
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                        return ReadError;
         type = 0;
         while ( type < NumFitMeth )
         {
            if ( strcmp(Buffer,FitMethIdentifier[type]) == 0 ) break;
            type++;
         }
         switch ( type )
         {
            case Secant:
               fitmeth = Secant;
               break;
            case Simplex:
               fitmeth = Simplex;
               break;
            default:
               return ReadError;
         }
         if ( LoadNumber(in,&fititer) == FALSE )        return ReadError;
         if ( LoadBoolean(in,&printshort) == FALSE )    return ReadError;
         break;
      }
      case 3: // aquavers=3: AQUASIM 2.0delta  calculation step series added
      {
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                        return ReadError;
         type = 0;
         while ( type < NumFitMeth )
         {
            if ( strcmp(Buffer,FitMethIdentifier[type]) == 0 ) break;
            type++;
         }
         switch ( type )
         {
            case Secant:
               fitmeth = Secant;
               break;
            case Simplex:
               fitmeth = Simplex;
               break;
            default:
               return ReadError;
         }
         if ( LoadNumber(in,&fititer) == FALSE )        return ReadError;
         if ( LoadBoolean(in,&printshort) == FALSE )    return ReadError;
         break;
      }
      default:
         return ReadError;
   }
   if ( LoadEnd(in) == FALSE )                          return ReadError;

   // read variables:

   switch ( varsys.Load(in) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   // read processes:

   switch ( procsys.Load(in,varsys.Varlist()) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   // read compartments:

   switch ( compsys.Load(in,varsys.Varlist(),procsys.Proclist()) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }
   AQVAR* var = varsys.Varlist()->First();
   while ( var != 0 )
   {
      if ( var->UpdateComp(compsys.Complist()) == FALSE ) return ReadError;
      var = var->Next();
   }

   // read links:

   switch ( linksys.Load(in,varsys.Varlist(),compsys.Complist()) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }
   RebuildConnections();

   // read calculation system:

   switch ( calcsys.Load(in) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   // read fit system:

   switch ( fitsys.Load(in,varsys.Varlist(),compsys.Complist()) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   // read numeric parameters:

   switch ( numpar.Load(in) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   // read plot variables:

   switch ( plotsys.Load(in,varsys.Varlist(),compsys.Complist()) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   // read states:

   switch ( statesys.LoadStates(in,varsys.Varlist(),loadfun) )
   {
      case OK:
         break;
      case INTERRUPT:
         return LoadInterrupt;
      case PROBLEM:
         return ReadError;
   }

   init = FALSE; start = TRUE; saved = TRUE;

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::Load");
#endif

   return LoadSuccess;
}


JOBSTATUS AQUASYS::Save(std::ostream& out)
{
#ifdef AQUA_DEMO
	return PROBLEM;
#endif

#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::Save");
#endif

   out.precision(ini.I(1));

   // write program identifier and version:

   out << "\n" << ProgName() << "\n" << ProgVersion() << "\n\n";
                                             if ( out.fail() ) return PROBLEM;
   ResetSaveCount();

   // save start delimiter and general data:

   if ( SaveString(out,AquasysIdentifier) == FALSE )           return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   if ( SaveString(out,OptionsIdentifier) == FALSE )           return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   INTEGER aquavers = 3;
   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   if ( SaveString(out,FitMethIdentifier[FitMeth()]) == FALSE )
                                                               return PROBLEM;
   if ( SaveNumber(out,FitIter()) == FALSE )                   return PROBLEM;
   if ( SaveBoolean(out,PrintShort()) == FALSE )               return PROBLEM;
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   JOBSTATUS status;

   // save variables:

   status = varsys.Save(out); if ( status != OK )              return status;

   // save processes:

   status = procsys.Save(out); if ( status != OK )             return status;

   // save compartments:

   status = compsys.Save(out); if ( status != OK )             return status;

   // save links:

   status = linksys.Save(out); if ( status != OK )             return status;

   // save sensitivity analysis system:

   status = calcsys.Save(out);
   if ( status != OK )                                         return status;

   // save fit system:

   status = fitsys.Save(out); if ( status != OK )              return status;

   // save numeric parameters:

   status = numpar.Save(out); if ( status != OK )              return status;

   // save plot variables:

   status = plotsys.Save(out); if ( status != OK )             return status;

   // save states:

   status = statesys.SaveStates(out,savefun);
   if ( status != OK )                                         return status;

   // save end delimiter:

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   out << "\n";
   saved = TRUE;

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::Save");
#endif

   return OK;
}


JOBSTATUS AQUASYS::Write(std::ostream& out, BOOLEAN)
{
#ifdef HEAPTEST
HeapFileMessage("start AQUASYS::Write");
#endif

   out.precision(ini.I(2));
   PrintWidth(ini.L(3));

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   PrintIndent(pos2-1);

   // write header:

   if ( PrintLn(out,'*') == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( Print(out,ProgName()) == FALSE )             return PROBLEM;
   if ( Print(out," ") == FALSE )                    return PROBLEM;
   if ( Print(out,ProgVersion()) == FALSE )          return PROBLEM;
   if ( Print(out," - ") == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(1)) == FALSE )               return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                  return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintTime(out,ini.T(2)) == FALSE )           return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;

   JOBSTATUS status;

   // write variables:

   status = varsys.Write(out,printshort); 
                                if ( status != OK )  return status;

   // write processes:

   status = procsys.Write(out,printshort); 
                                if ( status != OK )  return status;

   // write compartments:

   status = compsys.Write(out,printshort);
                                if ( status != OK )  return status;

   // write links:

   status = linksys.Write(out,printshort); 
                                if ( status != OK )  return status;

   if ( printshort == TRUE ) return OK;

   // write senssys:

   status = calcsys.Write(out); if ( status != OK )  return status;

   // write fitsys:

   status = fitsys.Write(out); if ( status != OK )   return status;

   // write plotsys:

   status = plotsys.Write(out); if ( status != OK )  return status;

   // write calc pars:

   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out) == FALSE )                      return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(3)) == FALSE )               return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                  return PROBLEM;

   status = numpar.Write(out); if ( status != OK )   return status;
   if ( PrintLn(out,'-') == FALSE )                  return PROBLEM;

   if ( Print(out,ini.T(97),pos1) == FALSE )         return PROBLEM;
   if ( Print(out,":") == FALSE )                    return PROBLEM;
   if ( FitMeth() == Secant )
   {
      if ( Print(out,ini.T(98),pos2) == FALSE )      return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(99),pos2) == FALSE )      return PROBLEM;
   }

   if ( Print(out,ini.T(100),pos1) == FALSE )        return PROBLEM;
   if ( Print(out,":") == FALSE )                    return PROBLEM;
   if ( Print(out,FitIter(),pos2) == FALSE )         return PROBLEM;

   if ( PrintLn(out,'*') == FALSE )                  return PROBLEM;

   if ( PrintLn(out) == FALSE )                      return PROBLEM;

   // write statesys:

   status = statesys.Write(out); if ( status != OK ) return status;

   if ( PrintLn(out) == FALSE )                      return PROBLEM;

#ifdef HEAPTEST
HeapFileMessage("end   AQUASYS::Write");
#endif

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
