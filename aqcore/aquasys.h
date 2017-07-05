///////////////////////////////   aquasys.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   08.07.93    Peter Reichert    Sens. Fcns.(t) + Plot Opt. added
//              05.08.93    Peter Reichert    Uncertainty limits(t) added
//              10.11.99    Peter Reichert    Final version of sens. ranking
//              27.03.00    Peter Reichert    MaxPost and MCMC added
//              24.12.01    Peter Reichert    SetFit added
//              14.09.03    Peter Reichert    CalcSingleRes added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
//   NODE
//     |
//     +------+
//            |
//   FILEIO   |
//     |      |
//     +------+----SYMBOL
//                   |
//                   +----AQUASYS
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AQUASYS_H
#define AQUASYS_H

#include "varsys.h"
#include "procsys.h"
#include "compsys.h"
#include "linksys.h"
#include "calcsys.h"
#include "fitsys.h"
#include "statesys.h"
#include "plotsys.h"

//////////////////////////////////////////////////////////////////////////////

typedef BOOLEAN (*CALCOUTFUN)(char* name, REAL t);
typedef void    (*SENSOUTFUN)();
typedef void    (*FITOUTFUN)(REAL chi);

enum LOADRESULT {
                  LoadSuccess,
                  LoadInterrupt,
                  WrongFileType,
                  WrongFileVersion,
                  ReadError
                };

enum FITMETHOD  { Secant, Simplex };

//////////////////////////////////////////////////////////////////////////////

class AQUASYS : public FILEIO
{
      friend JOBSTATUS rhsfun(const REAL& t, const REAL* y, const REAL* yt,
                              REAL* delta);
      friend JOBSTATUS fitfun(const REAL par[], CARDINAL numpar,
                              REAL fval[], CARDINAL nf);

   public:

      AQUASYS();
      ~AQUASYS();

      void               SetCancelFun(CANCELFUN fun);
      void               ResetCancelFun();
	  CANCELFUN          CancelFun()
                            { return cancelfun; }
      void               SetSaveFun(SAVELOADFUN fun)
                            { savefun = fun; }
      void               SetLoadFun(SAVELOADFUN fun)
                            { loadfun = fun; }

      void               InitLogFile();

      BOOLEAN            ExistVar(const AQVAR* var) const
                            { return varsys.Exist(var); }
      BOOLEAN            ExistVar(const char* sym) const
                            { return varsys.Exist(sym); }
      AQVAR*             GetVar(const char* sym) const
                            { return varsys.Get(sym); }
      BOOLEAN            AddVar(AQVAR* var);
      BOOLEAN            ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN            ExchangeVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN            ExchangeVar(const char* sym1, const char* sym2)
                            { return ExchangeVar(GetVar(sym1),GetVar(sym2)); }
      BOOLEAN            DeleteVar(AQVAR* var);
      BOOLEAN            DeleteVar(const char* sym)
                            { return DeleteVar(GetVar(sym)); }
      VARLIST*           Varlist()
                            { return varsys.Varlist(); }

      BOOLEAN            ExistProc(const AQPROC* proc) const
                            { return procsys.Exist(proc); }
      BOOLEAN            ExistProc(const char* sym) const
                            { return procsys.Exist(sym); }
      AQPROC*            GetProc(const char* sym) const
                            { return procsys.Get(sym); }
      BOOLEAN            AddProc(AQPROC* proc);
      BOOLEAN            ReplaceProc(AQPROC* oldproc, AQPROC* newproc);
      BOOLEAN            DeleteProc(AQPROC* proc);
      BOOLEAN            DeleteProc(const char* sym)
                            { return DeleteProc(GetProc(sym)); }
      PROCLIST*          Proclist()
                            { return procsys.Proclist(); }

      BOOLEAN            ExistComp(const AQCOMP* com) const
                            { return compsys.Exist(com); }
      BOOLEAN            ExistComp(const char* sym) const
                            { return compsys.Exist(sym); }
      AQCOMP*            GetComp(const char* sym) const
                            { return compsys.Get(sym); }
      BOOLEAN            AddComp(AQCOMP* com);
      BOOLEAN            ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);
      BOOLEAN            DeleteComp(AQCOMP* com);
      BOOLEAN            DeleteComp(const char* sym)
                            { return DeleteComp(GetComp(sym)); }
      COMPLIST*          Complist()
                            { return compsys.Complist(); }
      BOOLEAN            Activate(AQCOMP* com);
      BOOLEAN            Inactivate(AQCOMP* com);

      BOOLEAN            ExistLink(const AQLINK* link) const
                            { return linksys.Exist(link); }
      BOOLEAN            ExistLink(const char* sym) const
                            { return linksys.Exist(sym); }
      AQLINK*            GetLink(const char* sym) const
                            { return linksys.Get(sym); }
      BOOLEAN            AddLink(AQLINK* link);
      BOOLEAN            ReplaceLink(AQLINK* oldlink, AQLINK* newlink);
      BOOLEAN            DeleteLink(AQLINK* link);
      BOOLEAN            DeleteLink(const char* sym)
                            { return DeleteLink(GetLink(sym)); }
      LINKLIST*          Linklist()
                            { return linksys.Linklist(); }

      BOOLEAN            ExistCalc(const CALC* calc) const
                            { return calcsys.Exist(calc); }
      BOOLEAN            ExistCalc(const char* sym) const
                            { return calcsys.Exist(sym); }
      CALC*              GetCalc(const char* sym) const
                            { return calcsys.Get(sym); }
      BOOLEAN            AddCalc(CALC* calc);
      BOOLEAN            ReplaceCalc(CALC* oldcalc, CALC* newcalc);
      BOOLEAN            DeleteCalc(CALC* calc);
      BOOLEAN            DeleteCalc(const char* sym)
                            { return DeleteCalc(GetCalc(sym)); }
      BOOLEAN            SimActive(CALC* calc, BOOLEAN act);
      BOOLEAN            SensActive(CALC* calc, BOOLEAN act);
      CALCLIST*          Calclist()
                            { return calcsys.Calclist(); }

      BOOLEAN            ExistFit(const FIT* fit) const
                            { return fitsys.Exist(fit); }
      BOOLEAN            ExistFit(const char* sym) const
                            { return fitsys.Exist(sym); }
      FIT*               GetFit(const char* sym) const
                            { return fitsys.Get(sym); }
      BOOLEAN            AddFit(FIT* fit);
      BOOLEAN            ReplaceFit(FIT* oldfit, FIT* newfit);
      BOOLEAN            DeleteFit(FIT* fit);
      BOOLEAN            DeleteFit(const char* sym)
                            { return DeleteFit(GetFit(sym)); }
      BOOLEAN            FitActive(FIT* fit, BOOLEAN act);
      FITLIST*           Fitlist()
                            { return fitsys.Fitlist(); }

      BOOLEAN            ExistPlot(const PLOT* plot) const
                            { return plotsys.Exist(plot); }
      BOOLEAN            ExistPlot(const char* sym) const
                            { return plotsys.Exist(sym); }
      PLOT*              GetPlot(const char* sym) const
                            { return plotsys.Get(sym); }
      BOOLEAN            AddPlot(PLOT* plot);
      BOOLEAN            ReplacePlot(PLOT* oldplot, PLOT* newplot);
      BOOLEAN            DeletePlot(PLOT* plot);
      BOOLEAN            DeletePlot(const char* sym)
                            { return DeletePlot(GetPlot(sym)); }
      PLOTLIST*          Plotlist()
                            { return plotsys.Plotlist(); }
      JOBSTATUS          GetCurveData(ABSCTYPE at, const CURVE* curve,
                                      CARDINAL* n, REAL** x, REAL** y,
                                      REAL** yminus=0, REAL** yplus=0);
      JOBSTATUS          GetAllCurvesData(PLOT* plot, PLOTDATA* plotdata);
      BOOLEAN            GetResult(AQVAR* var, CARDINAL calcnum,
                                   AQCOMP* com, CARDINAL zoneind,
                                   REAL time, REAL space, BOOLEAN spacerel,
                                   REAL& value, AQVAR* par=0);
	  JOBSTATUS          ListResults(std::ostream& out, PLOT* plot);
      JOBSTATUS          PlotResults(std::ifstream& oldfile,
                                     std::ofstream& ptsfile, PLOT* plot, BOOLEAN EPS=FALSE); // *JR*

      REAL               DtMax() const
                            { return numpar.DtMax(); }
      BOOLEAN            DtMax(REAL dtmax);
      CARDINAL           MaxOrd() const
                            { return numpar.MaxOrd(); }
      BOOLEAN            MaxOrd(CARDINAL maxord);
      CARDINAL           NumCoDiag() const
                            { return numpar.NumCoDiag(); }
      void               NumCoDiag(CARDINAL numco);
      CARDINAL           MaxNumSteps() const
                            { return numpar.MaxNumSteps(); }
      BOOLEAN            MaxNumSteps(CARDINAL maxnumsteps);
      REAL               GridTimeConst() const
                            { return numpar.GridTimeConst(); }
      BOOLEAN            GridTimeConst(REAL gridrelax);
      BOOLEAN            Interpolate() const
                            { return numpar.Interpolate(); }
      void               Interpolate(BOOLEAN b);
      REAL               FitTol() const
                            { return numpar.FitTol(); }
      BOOLEAN            FitTol(REAL tol);

      FITMETHOD          FitMeth() const
                            { return fitmeth; }
      void               FitMeth(FITMETHOD meth);
      CARDINAL           FitIter() const
                            { return fititer; }
      void               FitIter(CARDINAL iter);

      void               PrintShort(BOOLEAN b);
      BOOLEAN            PrintShort() const
                            { return printshort; }

      JOBSTATUS          InitCalc(CALC* calc,
                                  BOOLEAN writelog=TRUE, AQVAR* par=0);
      JOBSTATUS          Calculate(CALC* calc,
                                   BOOLEAN writelog=TRUE, AQVAR* par=0);
      JOBSTATUS          InitSim(BOOLEAN writelog=TRUE);
      JOBSTATUS          Simulate(BOOLEAN writelog=TRUE);
      JOBSTATUS          SensAnal(BOOLEAN writelog=TRUE); 
      JOBSTATUS          SensRank(BOOLEAN writelog=TRUE, 
                                  std::ostream* rankout=0, std::ostream* sensout=0,
                                  std::istream* cmdfile=0);
      JOBSTATUS          Fit(std::ostream& out,
                             CARDINAL* numstep,
                             BOOLEAN writelog=TRUE);
      JOBSTATUS          SetFit(std::istream& in, std::ostream& out, std::ostream&fitout,
                                BOOLEAN writelog=TRUE);
      JOBSTATUS          CalcChi2(std::istream& in, std::ostream& out,
                                  BOOLEAN writelog=TRUE);
      JOBSTATUS          IdAnal(std::istream& cmdfile, std::ostream& fitfile,
                                BOOLEAN writelog=TRUE);
      JOBSTATUS          CalcRes(std::istream& inpar, std::istream& inres,
                                 std::ostream& out, BOOLEAN writelog=TRUE);
      JOBSTATUS          CalcSingleRes(std::istream& inpar, std::istream& inres,
                                       std::ostream& out, BOOLEAN writelog=TRUE);
      JOBSTATUS          MaxPost(std::istream& inpri, std::ostream& out, 
                                 std::ostream& outvar, BOOLEAN writelog=TRUE);
      JOBSTATUS          MCMC(std::istream& inpri, std::istream& injmp,
                              std::ostream& out, BOOLEAN writelog=TRUE);

      void               SetCalcOut(CALCOUTFUN out)
                            { calcoutfun = out; }
      void               SetSensOut(SENSOUTFUN out)
                            { sensoutfun = out; }
      void               SetFitOut(FITOUTFUN out)
                            { fitoutfun = out; }

      BOOLEAN            CalcInit() const
                            { return init; }
      CARDINAL           NumCalc() const
                            { return statesys.NumCalc(); }
      CARDINAL           CalcNums(CARDINAL index) const
                            { return statesys.CalcNum(index); }
      CARDINAL           NumStates(CARDINAL cn) const
                            { return statesys.NumStates(cn); }
      CARDINAL           NumPar(CARDINAL cn) const
                            { return statesys.NumPar(cn); }
      const STATE*       State(CARDINAL cn, CARDINAL statenum,
                               const AQVAR* par=0) const
                            { return statesys.GetState(cn,statenum,par); }
      BOOLEAN            DeleteState(CARDINAL cn, CARDINAL statenum);
      BOOLEAN            DeleteStates(CARDINAL cn);
      void               DeleteStates();

      BOOLEAN            PlotLisOpt(PLOTLISOPT* lisopt);
      PLOTLISOPT*        PlotLisOpt()
                            { return plotsys.LisOpt(); }
      CARDINAL           PlotFilOpt(PLOTFILOPT* filropt);
      PLOTFILOPT*        PlotFilOpt()
                            { return plotsys.FilOpt(); }
      CARDINAL           PlotScrOpt(PLOTSCROPT* scropt);
      PLOTSCROPT*        PlotScrOpt() const
                            { return plotsys.ScrOpt(); }

      void               Delete();

	  BOOLEAN            CheckSysFile(std::istream& in);
      LOADRESULT         Load(std::istream& in);
      JOBSTATUS          Save(std::ostream& out);
      JOBSTATUS          Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      void               PlotLimits(REAL DataMin, REAL DataMax,
                                    REAL* Min, REAL* Max,
                                    REAL* Pos, REAL* Dist) const;
      BOOLEAN            Consolidate(REAL* a, CARDINAL n);
      BOOLEAN            ExtractName(char* inpline, CARDINAL& pos,
                                     char* namebuf);
      void               ParseArgs(CARDINAL numarg, char** args,
                                   char* linebuf);
      JOBSTATUS          PrintRankHeader(std::ostream* out, AQVAR* var,
                                         CARDINAL num);
      JOBSTATUS          PrintSensHeader(std::ostream* out,
                                         CARDINAL numpars, AQVAR** par);
      JOBSTATUS          CollectFitData(BOOLEAN writelog=TRUE);

      VARSYS             varsys;
      PROCSYS            procsys;
      COMPSYS            compsys;
      LINKSYS            linksys;
      CALCSYS            calcsys;
      FITSYS             fitsys;
      NUMPAR             numpar;
      STATESYS           statesys;
      PLOTSYS            plotsys;

      CANCELFUN          cancelfun;
      SAVELOADFUN        savefun;
      SAVELOADFUN        loadfun;

      REAL               t;
      CARDINAL           numeq;
      REAL*              relacc;
      REAL*              absacc;
      EQTYPE*            eqtype;
      REAL*              Y;
      REAL*              YT;

      CARDINAL           Ycalcnum;
      BOOLEAN            init;         // TRUE: initial state is calculated
      BOOLEAN            start;        // TRUE: int. alg. must be restarted
      AQVAR*             senspar;

      FITMETHOD          fitmeth;
      CARDINAL           fititer;

	  BOOLEAN            printshort;

      static INIT        ini;

      CALCOUTFUN         calcoutfun;
      SENSOUTFUN         sensoutfun;
      FITOUTFUN          fitoutfun;

      void               RebuildConnections();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
