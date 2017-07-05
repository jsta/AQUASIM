////////////////////////////////   aqcomp.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.09.91    Peter Reichert
// revisions:   22.10.92    Peter Reichert    redesign of links
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//              04.02.01    Peter Reichert    SEDCOMP added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "aqcomp" implements classes for aquatic compartments
// ====================================================
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
//   NODE
//     |
//     +------+
//            |
//   FILEIO   |
//     |      |
//     +------+----SYMBOL
//                   |
//                   +----AQCOMP
//
//   LIST
//     |
//     +----SYMBOLLIST
//             |
//             +----COMPLIST
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following classes are implemented in this file:
//
// AQCOMP:       virtual base class for compartments
// -------
//
//
// COMPLIST:
// ---------
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class AQCOMP
// ===========================
//
//
//
// Methods of the class COMPLIST
// =============================
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AQCOMP_H
#define AQCOMP_H

#include "all.h"
#include "symbol.h"
#include "aqproc.h"
#include "procsys.h"
#include "numpar.h"

//////////////////////////////////////////////////////////////////////////////

enum AQCOMPTYPE   {
                     MixComp,
                     FilmComp,
                     RivComp,
                     AdvComp,
                     LakeComp,
                     SoilComp,
                     SedComp
                  };

enum RIVCOMPLEVEL {
                     NormalLevel,
                     CriticalLevel,
                     GivenLevel
                  };

enum RIVCOMPMETH  {
                     KinWave,
                     DiffWave
                  };

//////////////////////////////////////////////////////////////////////////////

class ADVLINK;

class DIFFLINK;

//////////////////////////////////////////////////////////////////////////////

class AQCOMP : public SYMBOL
{
      friend class COMPSYS;
      friend class AQUASYS;

   public:

      AQCOMP();
      AQCOMP(const AQCOMP& com);
      AQCOMP(const AQCOMP* com);
      ~AQCOMP();

      AQCOMP*               Next() const
                               { return (AQCOMP*)SYMBOL::Next(); }
      AQCOMP*               Prev() const
                               { return (AQCOMP*)SYMBOL::Prev(); }

      virtual AQCOMPTYPE    Type() const = 0;
      virtual const char*   TypeName() const = 0;

      virtual AQVAR*        SpaceVar(const VARSYS* varsys) const = 0;
      virtual BOOLEAN       AllowedSpaceVal(REAL x) const = 0;

      virtual BOOLEAN       AllowedProgVarType(PROGVARTYPE type) const = 0;
      BOOLEAN               AllowedVar(const AQVAR* var) const;
      virtual BOOLEAN       AllowedReplaceVar(const AQVAR* oldvar,
                                              const AQVAR* newvar) const;
      virtual BOOLEAN       AllowedExchangeVar(const AQVAR* var1,
                                               const AQVAR* var2) const;
      virtual BOOLEAN       AllowedProc(const AQPROC* proc) const;
      virtual BOOLEAN       AllowedReplaceProc(const AQPROC* oldproc,
                                               const AQPROC* newproc) const;
      virtual BOOLEAN       AllowedComp() const = 0;

      CARDINAL              NumVarArg() const
                               { return numvararg; }
      AQVAR*                VarArg(CARDINAL index) const;
      BOOLEAN               Arg(const AQVAR* var) const;

      CARDINAL              NumProcArg() const
                               { return numactproc; }
      AQPROC*               ProcArg(CARDINAL index) const;
      BOOLEAN               Arg(const AQPROC* proc) const;

      BOOLEAN               ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN               ExchangeVar(AQVAR* var1, AQVAR* var2);

      BOOLEAN               ReplaceProc(AQPROC* oldproc, AQPROC* newproc);

      CARDINAL              NumStateVar() const
                               { return numstatevar; }
      CARDINAL              NumVolStateVar() const
                               { return numvolstatevar; }
      CARDINAL              NumSurfStateVar() const
                               { return numsurfstatevar; }
      CARDINAL              NumDynStateVar() const
                               { return numdynstatevar; }
      CARDINAL              NumEquStateVar() const
                               { return numequstatevar; }
      STATEVAR* const*      StateVars() const
                               { return statevar; }

      CARDINAL              NumActVar() const
                               { return numactvar; }
      AQVAR*                ActVar(CARDINAL index) const;
      BOOLEAN               AddActVar(AQVAR* var, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN               RemoveActVar(CARDINAL pos);
      BOOLEAN               RemoveActVar(AQVAR* var);

      CARDINAL              NumActProc() const
                               { return numactproc; }
      AQPROC*               ActProc(CARDINAL index) const;
      BOOLEAN               AddActProc(AQPROC* proc, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN               RemoveActProc(CARDINAL pos);
      BOOLEAN               RemoveActProc(AQPROC* proc);

      void                  CmpIndex(CARDINAL i)
                               { cind = i; }
      CARDINAL              CmpIndex() const
                               { return cind; }
      virtual CARDINAL      NumGridPts() const
                               { return numgrid; }
      virtual BOOLEAN       NumGridPts(CARDINAL n) = 0;
      virtual REAL          GridPt(const REAL* Y, CARDINAL index) = 0;

      virtual CARDINAL      NumZones() const = 0;
      virtual const char*   ZoneName(CARDINAL index) const = 0;

      virtual CARDINAL      NumZoneGridPts(CARDINAL zone) = 0;
      virtual REAL          ZoneGridPt(const REAL* Y, CARDINAL zone,
                                       CARDINAL index) = 0;

      virtual CARDINAL      NumEq() const = 0;
      virtual REAL          RelAcc(CARDINAL i) const = 0;
      virtual REAL          AbsAcc(CARDINAL i) const = 0;
      virtual EQTYPE        EqType(CARDINAL i) const = 0;

      virtual BOOLEAN       InitCond(VARSYS* varsys, REAL* Y,
                                     CARDINAL callnum) = 0;
      virtual BOOLEAN       Delta(const NUMPAR& numpar, VARSYS* varsys,
                                  const REAL* Y, const REAL* YT,
                                  REAL* DELTA) = 0;

      virtual BOOLEAN       GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                                      REAL* t_ptr, const REAL* Y,
                                      CARDINAL zone, CARDINAL pt, AQVAR* var,
                                      REAL& value) = 0;
      virtual BOOLEAN       SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                                       REAL* t_ptr, const REAL* Y,
                                       CARDINAL zone, REAL x, BOOLEAN xrel, 
                                       AQVAR* var,
                                       REAL& value) = 0;

      CARDINAL              NumAdvInConn() const
                               { return numadvinconn; }
      CARDINAL              NumAdvOutConn() const
                               { return numadvoutconn; }
      CARDINAL              NumDiffConn() const
                               { return numdiffconn; }
      virtual const char*   AdvInName(CARDINAL conn) const;
      virtual const char*   AdvOutName(CARDINAL conn) const;
      virtual const char*   DiffName(CARDINAL conn) const;
      virtual BOOLEAN       AdvInExZ(CARDINAL conn) const;
      virtual REAL          AdvInZ(const REAL* Y, CARDINAL conn) const;
      virtual REAL          AdvOutQ(VARSYS* varsys, const REAL* Y,
                                    CARDINAL conn);
      virtual REAL          AdvOutJ(VARSYS* varsys, const REAL* Y,
                                    CARDINAL conn,
                                    const AQVAR* var);
      virtual REAL          DiffC(const REAL* Y,
                                  CARDINAL conn,
                                  const AQVAR* var) const;

      BOOLEAN               Active() const
                               { return active; }
      BOOLEAN               Active(BOOLEAN act);

	  JOBSTATUS             Load(std::istream& in,
                                 const VARLIST* varlist,
                                 const PROCLIST* proclist,
                                 CARDINAL compvers);
      JOBSTATUS             Save(std::ostream& out);
      JOBSTATUS             Write(std::ostream& out, BOOLEAN sh=FALSE);

      static const INIT*    Ini()
                               { return &ini; }

   protected:

      static CARDINAL       number;

      CARDINAL              cind;

      CARDINAL              numgrid;

      CARDINAL              numvararg;
      AQVAR**               vararg;
      virtual void          CalcArg();

      virtual void          ReplVar(AQVAR* oldvar, AQVAR* newvar);
      virtual void          ExchVar(AQVAR* var1, AQVAR* var2);

      void                  ReplProc(AQPROC* oldproc, AQPROC* newproc);

      CARDINAL              numstatevar;
      CARDINAL              numvolstatevar;
      CARDINAL              numsurfstatevar;
      CARDINAL              numdynstatevar;
      CARDINAL              numequstatevar;
      STATEVAR**            statevar;

      CARDINAL              numactvar;
      AQVAR**               actvar;
      void                  CalcStateVar();

      void                  CalcRates(REAL* rates);

      CARDINAL              numactproc;
      AQPROC**              actproc;

      virtual BOOLEAN       SetGridPts(CARDINAL n) = 0;

	  CARDINAL              numadvinconn;
      CARDINAL*             advinnum;
      CARDINAL**            advinindex;
      ADVLINK***            advinlink;

	  CARDINAL              numadvoutconn;
      ADVLINK**             advoutlink;

	  CARDINAL              numdiffconn;
      CARDINAL*             diffnum;
      CARDINAL**            diffindex;
      DIFFLINK***           difflink;

      BOOLEAN               active;
      CARDINAL              ind_y;

      static INIT           ini;

   private:

      void                  init();
};

//////////////////////////////////////////////////////////////////////////////

class COMPLIST : private SYMBOLLIST
{
      friend class COMPSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const AQCOMP* com) const
                  { return SYMBOLLIST::Exist(com); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      AQCOMP*  First() const
                  { return (AQCOMP*)SYMBOLLIST::First(); }
      AQCOMP*  Last() const
                  { return (AQCOMP*)SYMBOLLIST::Last(); }
      AQCOMP*  Current() const
                  { return (AQCOMP*)SYMBOLLIST::Current(); }
      AQCOMP*  Next() const
                  { return (AQCOMP*)SYMBOLLIST::Next(); }
      AQCOMP*  Prev() const
                  { return (AQCOMP*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const AQCOMP* com) const
                  { return SYMBOLLIST::Position(com); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      AQCOMP*  Get(CARDINAL pos) const
                  { return (AQCOMP*)SYMBOLLIST::Get(pos); }
      AQCOMP*  Get(const char* sym) const
                  { return (AQCOMP*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(AQCOMP* com)
                  { return SYMBOLLIST::Goto(com); }
      BOOLEAN  Goto(const char* sym)
                  { return SYMBOLLIST::Goto(sym); }
      BOOLEAN  Goto(CARDINAL pos)
                  { return SYMBOLLIST::Goto(pos); }
      BOOLEAN  GotoFirst()
                  { return SYMBOLLIST::GotoFirst(); }
      BOOLEAN  GotoNext()
                  { return SYMBOLLIST::GotoNext(); }
      BOOLEAN  GotoPrev()
                  { return SYMBOLLIST::GotoPrev(); }
      BOOLEAN  GotoNextOrFirst()
                  { return SYMBOLLIST::GotoNextOrFirst(); }
      BOOLEAN  GotoPrevOrLast()
                  { return SYMBOLLIST::GotoPrevOrLast(); }
      BOOLEAN  GotoLast()
                  { return SYMBOLLIST::GotoLast(); }

   private:

      BOOLEAN  Add(AQCOMP* com)
                  { return SYMBOLLIST::InsertSorted(com); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(AQCOMP* com)
                  { return SYMBOLLIST::Remove(com); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
