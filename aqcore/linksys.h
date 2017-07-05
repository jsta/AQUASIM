////////////////////////////////   linksys.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    15.10.92    Peter Reichert
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
//     +----LINKSYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// LINKSYS:      
// -------       
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LINKSYS_H
#define LINKSYS_H

#include "all.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

class LINKSYS : public FILEIO
{
   public:

      LINKSYS();
      ~LINKSYS();

      void      SetCancelFun(CANCELFUN fun)
                   { cancelfun = fun; }
      void      ResetCancelFun()
                   { cancelfun = 0; }

      BOOLEAN   AllowedReplaceVar(const AQVAR* oldvar,
                                  const AQVAR* newvar) const;
      BOOLEAN   AllowedExchangeVar(const AQVAR* var1,
                                   const AQVAR* var2) const;
      BOOLEAN   AllowedReplaceComp(const AQCOMP* oldcomp,
                                   const AQCOMP* newcomp) const;

      BOOLEAN   Exist(const AQLINK* link) const
                   { return linklist.Exist(link); }
      BOOLEAN   Exist(const char* sym) const
                   { return Exist(Get(sym)); }

      AQLINK*   Get(const char* sym) const
                   { return linklist.Get(sym); }

      BOOLEAN   Add(AQLINK* link);
      BOOLEAN   Replace(AQLINK* oldlink, AQLINK* newlink);
      BOOLEAN   Delete(AQLINK* link);
      BOOLEAN   Delete(const char* sym)
                   { return Delete(Get(sym)); }
      void      Delete();

      void      ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);
      void      ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void      ExchangeVar(AQVAR* var1, AQVAR* var2);

      BOOLEAN   Arg(const AQVAR* var) const;
      BOOLEAN   Arg(const AQCOMP* com) const;

      void      SetLinkIndex(VARSYS* varsys);

      LINKLIST* Linklist()
                  { return &linklist; }

				  JOBSTATUS Load(std::istream& in,
                     const VARLIST* varlist,
                     const COMPLIST* complist);
				  JOBSTATUS Save(std::ostream& out);
				  JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      LINKLIST  linklist;
      CANCELFUN cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
