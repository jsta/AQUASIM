///////////////////////////////   procsys.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    22.08.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
//   FILEIO
//     |
//     +----PROCSYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// PROCSYS:      class for a system of biochemical processes, where the 
// --------      stoichiometric coefficients and the process rate laws are
//               variables of type AQVAR (see aqvar.h, formvar.h and other-
//               var.h)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PROCSYS_H
#define PROCSYS_H

#include "all.h"
#include "othervar.h"
#include "aqproc.h"

//////////////////////////////////////////////////////////////////////////////

class PROCSYS : public FILEIO
{
   public:

      PROCSYS();
      ~PROCSYS();

      void        SetCancelFun(CANCELFUN fun)
                     { cancelfun = fun; }
      void        ResetCancelFun()
                     { cancelfun = 0; }

      BOOLEAN     Exist(const AQPROC* proc) const
                     { return proclist.Exist(proc); }
      BOOLEAN     Exist(const char* sym) const
                     { return Exist(Get(sym)); }

      AQPROC*     Get(const char* sym) const
                     { return proclist.Get(sym); }

      BOOLEAN     Add(AQPROC* proc);
      BOOLEAN     Replace(AQPROC* oldproc, AQPROC* newproc);
      BOOLEAN     Delete(AQPROC* proc);
      void        Delete();

      void        ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void        ExchangeVar(AQVAR* var1, AQVAR* var2);

      PROCLIST*   Proclist()
                     { return &proclist; }

      BOOLEAN     Arg(const AQVAR* var) const;

	  JOBSTATUS   Load(std::istream& in, const VARLIST* varlist);
	  JOBSTATUS   Save(std::ostream& out);
	  JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      PROCLIST    proclist;
      CANCELFUN   cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
