///////////////////////////////   compsys.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    09.08.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
//   FILEIO
//     |
//     +----COMPSYS
//
//////////////////////////////////////////////////////////////////////////////

#ifndef COMPSYS_H
#define COMPSYS_H

#include "all.h"
#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class COMPSYS : public FILEIO
{
   public:

      COMPSYS();
      ~COMPSYS();

      void         SetCancelFun(CANCELFUN fun)
                      { cancelfun = fun; }
      void         ResetCancelFun()
                      { cancelfun = 0; }

      BOOLEAN      AllowedReplaceVar(const AQVAR* oldvar,
                                     const AQVAR* newvar) const;
      BOOLEAN      AllowedExchangeVar(const AQVAR* var1,
                                      const AQVAR* var2) const;
      BOOLEAN      AllowedReplaceProc(const AQPROC* oldproc,
                                      const AQPROC* newproc) const;

      BOOLEAN      Exist(const AQCOMP* com) const
                      { return complist.Exist(com); }
      BOOLEAN      Exist(const char* sym) const
                      { return Exist(Get(sym)); }

      AQCOMP*      Get(const char* sym) const
                      { return complist.Get(sym); }

      BOOLEAN      Add(AQCOMP* com);
      BOOLEAN      Replace(AQCOMP* oldcomp, AQCOMP* newcomp);
      BOOLEAN      Delete(AQCOMP* com);
      void         ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void         ExchangeVar(AQVAR* var1, AQVAR* var2);
      void         ReplaceProc(AQPROC* oldproc, AQPROC* newproc);
      void         Delete();

      BOOLEAN      Arg(const AQVAR* var) const;
      BOOLEAN      Arg(const AQPROC* proc) const;

      CARDINAL     NumEq();
      BOOLEAN      RelAcc(CARDINAL size, REAL* Y);
      BOOLEAN      AbsAcc(CARDINAL size, REAL* Y);
      BOOLEAN      EqType(CARDINAL size, EQTYPE* Y);

      BOOLEAN      InitCond(VARSYS* varsys, REAL t,
                            CARDINAL size, REAL* Y);
      BOOLEAN      Delta(const NUMPAR& numpar, VARSYS* varsys,
                         const REAL* Y, const REAL* YT, REAL* DELTA);

      BOOLEAN      GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                             REAL* t_ptr, const REAL* Y, AQCOMP* com,
                             CARDINAL zone, CARDINAL pt, AQVAR* var,
                             REAL& value);
      BOOLEAN      SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                              REAL* t_ptr, const REAL* Y, AQCOMP* com,
                              CARDINAL zone, REAL x, BOOLEAN xrel,
                              AQVAR* var,
                              REAL& value);

      COMPLIST*    Complist()
                      { return &complist; }

					  JOBSTATUS    Load(std::istream& in,
                        const VARLIST* varlist, const PROCLIST* proclist);
      JOBSTATUS    Save(std::ostream& out);
      JOBSTATUS    Write(std::ostream& out, BOOLEAN sh=FALSE);

      void         CalcIndex();

   protected:

      COMPLIST     complist;
      CANCELFUN    cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
