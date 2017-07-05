////////////////////////////////   aqproc.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    22.08.91    Peter Reichert
// revisions:   02.12.91    Peter Reichert    ?PROC::?PROC(const ?PROC&)
//                                            constructor added
//              20.01.92    Peter Reichert    members variable and expression
//                                            from DYNPROC to AQPROC moved
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "aqproc" implements classes for biochemical processes
// =====================================================
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
//                   +----AQPROC
//                          |
//                          +----DYNPROC
//                          |
//                          +----EQUPROC
//
//   LIST
//     |
//     +----SYMBOLLIST
//            |
//            +----PROCLIST
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following classes are implemented in this file:
//
// AQPROC:       virtual base class for biochemical processes
// -------
//
//
//
// DYNPROC:      class for dynamic biochemical processes
// --------      (calculated as differential equations)
//
//
//
// EQUPROC:      class for biochemical equilibrium processes
// --------      (calculated as algebraic equations)
//
//
// PROCLIST:
// ---------
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class AQPROC
// ===========================
//
//
//
// Methods of the class DYNPROC
// ============================
//
//
//
// Methods of the class EQUPROC
// ============================
//
//
//
// Methods of the class PROCLIST
// =============================
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AQPROC_H
#define AQPROC_H

#include "all.h"
#include "init.h"
#include "symbol.h"
#include "formvar.h"
#include "othervar.h"

//////////////////////////////////////////////////////////////////////////////

enum AQPROCTYPE {
                   DynProc,
                   EquProc
                };

//////////////////////////////////////////////////////////////////////////////

class AQPROC : public SYMBOL
{
      friend class PROCSYS;

   public:

      AQPROC();
      AQPROC(const AQPROC& proc);
      AQPROC(const AQPROC* proc);
      ~AQPROC();

      AQPROC*             Next() const
                             { return (AQPROC*)SYMBOL::Next(); }
      AQPROC*             Prev() const
                             { return (AQPROC*)SYMBOL::Prev(); }

      virtual AQPROCTYPE  Type() const = 0;
      virtual const char* TypeName() const = 0;

      virtual void        SumRates(STATEVAR* const* statevar,
                                   CARDINAL numstatevar,
                                   REAL* rates) = 0;

      CARDINAL            NumVar() const
                             { return numvar; }
      AQVAR*              Variable(CARDINAL index) const;
      const FORMVAR*      Expression(CARDINAL index) const;

      CARDINAL            NumVarArg() const
                             { return numvararg; }
      AQVAR*              VarArg(CARDINAL index) const;
      BOOLEAN             Arg(const AQVAR* var) const;

      BOOLEAN             ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN             ExchangeVar(AQVAR* var1, AQVAR* var2);

	  JOBSTATUS           Load(std::istream& in);
      JOBSTATUS           Save(std::ostream& out);
      JOBSTATUS           Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      CARDINAL            numvar;
      AQVAR**             variable;
      FORMVAR**           expression;

      CARDINAL            numvararg;
      AQVAR**             vararg;

      virtual void        ReplVar(AQVAR* oldvar, AQVAR* newvar);
      virtual void        ExchVar(AQVAR* var1, AQVAR* var2);

      void                init();
      virtual void        CalcArg();

      static INIT         ini;

   private:

      static CARDINAL     number;
};

//////////////////////////////////////////////////////////////////////////////

class DYNPROC : public AQPROC
{
      friend class PROCSYS;

   public:

      DYNPROC();
      DYNPROC(const DYNPROC& proc);
      DYNPROC(const AQPROC* proc);
      ~DYNPROC();

      AQPROCTYPE     Type() const
                        { return DynProc; }
      const char*    TypeName() const;

      void           SumRates(STATEVAR* const* statevar,
                              CARDINAL numstatevar,
                              REAL* rates);

      const FORMVAR* RateLaw() const
                        { return rate; }
      const FORMVAR* StoichCoeff(CARDINAL index) const;

      BOOLEAN        RateLaw(const VARLIST* varlist,
                             const char* inpline,
                             char* parseerrorline,
                             CARDINAL& numparseerrors);
      BOOLEAN        AddStoichCoeff(const VARLIST* varlist,
                                    AQVAR* var,
                                    const char* inpline,
                                    char* parseerrorline,
                                    CARDINAL& numparseerrors,
                                    CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceStoichCoeff(const VARLIST* varlist,
                                        AQVAR* var,
                                        const char* inpline,
                                        char* parseerrorline,
                                        CARDINAL& numparseerrors,
                                        CARDINAL pos);
      BOOLEAN        DeleteStoichCoeff(CARDINAL pos);

      JOBSTATUS      Load(std::istream& in, const VARLIST* varlist);
      JOBSTATUS      Save(std::ostream& out);
      JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      FORMVAR*       rate;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);

      void           init();
      void           CalcArg();
};

//////////////////////////////////////////////////////////////////////////////

class EQUPROC : public AQPROC
{
      friend class PROCSYS;

   public:

      EQUPROC();
      EQUPROC(const EQUPROC& proc);
      EQUPROC(const AQPROC* proc);
      ~EQUPROC();

      AQPROCTYPE     Type() const
                        { return EquProc; }
      const char*    TypeName() const;

      void           SumRates(STATEVAR* const* statevar,
                              CARDINAL numstatevar,
                              REAL* rates);

      const AQVAR*   Variable() const;
      const FORMVAR* Equation() const;

      BOOLEAN        Equation(AQVAR*  var,
                              const VARLIST* varlist,
                              const char* inpline,
                              char* parseerrorline,
                              CARDINAL& numparseerrors);

      JOBSTATUS      Load(std::istream& in, const VARLIST* varlist);
      JOBSTATUS      Save(std::ostream& out);
      JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      void           init();
};

//////////////////////////////////////////////////////////////////////////////

class PROCLIST : private SYMBOLLIST
{
      friend class PROCSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const AQPROC* proc) const
                  { return SYMBOLLIST::Exist(proc); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      AQPROC*  First() const
                  { return (AQPROC*)SYMBOLLIST::First(); }
      AQPROC*  Last() const
                  { return (AQPROC*)SYMBOLLIST::Last(); }
      AQPROC*  Current() const
                  { return (AQPROC*)SYMBOLLIST::Current(); }
      AQPROC*  Next() const
                  { return (AQPROC*)SYMBOLLIST::Next(); }
      AQPROC*  Prev() const
                  { return (AQPROC*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const AQPROC* proc) const
                  { return SYMBOLLIST::Position(proc); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      AQPROC*  Get(CARDINAL pos) const
                  { return (AQPROC*)SYMBOLLIST::Get(pos); }
      AQPROC*  Get(const char* sym) const
                  { return (AQPROC*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(AQPROC* proc)
                  { return SYMBOLLIST::Goto(proc); }
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

      BOOLEAN  Add(AQPROC* proc)
                  { return SYMBOLLIST::InsertSorted(proc); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(AQPROC* proc)
                  { return SYMBOLLIST::Remove(proc); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
