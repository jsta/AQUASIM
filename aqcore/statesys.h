///////////////////////////////   statesys.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    12.12.91    Peter Reichert
// revisions:   17.06.93    Peter Reichert    Sensitivity analysis added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Class hierarchy
// ===============
//
//   FILEIO
//     |
//     +----STATESYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// STATE:
// ------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef STATESYS_H
#define STATESYS_H

#include "all.h"
#include "fileio.h"
#include "aqvar.h"

//////////////////////////////////////////////////////////////////////////////

typedef BOOLEAN (*SAVELOADFUN)();

//////////////////////////////////////////////////////////////////////////////

class STATE
{
   public:

      STATE(CARDINAL neq, REAL t, const REAL* y, const REAL* yt);
      ~STATE();

      CARDINAL    NumEq() const 
                     { return s_neq; }
      REAL        Time() const
                     { return s_t; }
      const REAL* Y() const
                     { return s_y; }
      const REAL* YT() const
                     { return s_yt; }

   private:

      CARDINAL s_neq;
      REAL     s_t;
      REAL*    s_y;
      REAL*    s_yt;
};


class STATESYS : public FILEIO
{
      friend class AQUASYS;

   public:

      STATESYS();
      ~STATESYS();

      void            SetCancelFun(CANCELFUN fun)
                         { cancelfun = fun; }
      void            ResetCancelFun()
                         { cancelfun = 0; }

      CARDINAL        NumEq() const;
      CARDINAL        NumCalc() const
                         { return numcalc; }
      CARDINAL        CalcNum(CARDINAL index) const;
      CARDINAL        NumPar(CARDINAL calcnum) const;
      AQVAR*          Par(CARDINAL calcnum, CARDINAL index) const;
	  BOOLEAN         ExistPar(CARDINAL calcnum, AQVAR* par) const;
      CARDINAL        NumStates(CARDINAL calcnum) const;
      const STATE*    GetState(CARDINAL calcnum, CARDINAL statenum,
                               const AQVAR* par=0) const;

	  JOBSTATUS       LoadStates(std::istream& in, const VARLIST* varlist,
                                 SAVELOADFUN fun = 0);
      JOBSTATUS       SaveStates(std::ostream& out,
                                 SAVELOADFUN fun = 0);
      JOBSTATUS       Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      CARDINAL        numcalc;
      CARDINAL*       calcnums;
      CARDINAL*       numstates;
      CARDINAL*       numpars;
      AQVAR***        pars;
      STATE****       states;

      BOOLEAN         AddState(CARDINAL calcnum, CARDINAL neq,
                               REAL t, const REAL* y, const REAL* yt,
                               AQVAR* par = 0);
      BOOLEAN         DeleteState(CARDINAL calcnum, CARDINAL statenum);
      BOOLEAN         DeleteStates(CARDINAL calcnum);
      void            Delete();

      static INIT     ini;

      CANCELFUN       cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

