////////////////////////////////   varsys.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    24.07.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
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
//     +----VARSYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// VARSYS:       class for a consistent variable system of STATVARs, PROGVARs,
// -------       FITVARs, REALLISTVARs, VARLISTVARs and FORMVARs
//               (see formvar.h and othervar.h)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef VARSYS_H
#define VARSYS_H

#include "all.h"
#include "othervar.h"

//////////////////////////////////////////////////////////////////////////////

class VARSYS : public FILEIO
{
   public:

      VARSYS();
      ~VARSYS();

      void      SetCancelFun(CANCELFUN fun)
                   { cancelfun = fun; }
      void      ResetCancelFun()
                   { cancelfun = 0; }

      void      Reset();

      BOOLEAN   Exist(const AQVAR* var) const
                   { return varlist.Exist(var); }
      BOOLEAN   Exist(const char* sym) const
                   { return Exist(Get(sym)); }

      AQVAR*    Get(const char* sym) const
                   { return varlist.Get(sym); }

      BOOLEAN   Arg(const AQVAR* var) const;
      BOOLEAN   Arg(const AQCOMP* var) const;

      BOOLEAN   Add(AQVAR* var);
      BOOLEAN   Replace(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN   Exchange(AQVAR* var1, AQVAR* var2);
      BOOLEAN   Delete(AQVAR* var);
      BOOLEAN   Delete(const char* sym)
                   { return Delete(Get(sym)); }
      void      Delete();

      void      ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      PROGVAR*  GetProgVar(PROGVARTYPE type) const;
      BOOLEAN   SetProgVar(PROGVARTYPE type, const REAL* var);

      void      StoreStateProgVars();
      void      RecallStateProgVars();

      VARLIST*  Varlist()
                   { return &varlist; }

	  JOBSTATUS Load(std::istream& in);
      JOBSTATUS Save(std::ostream& out);
      JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      VARLIST   varlist;
      CANCELFUN cancelfun;
      CARDINAL  numaddr;
      REAL**    stateprogaddr;

      BOOLEAN   CheckVarTypes(AQVAR* var);
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
