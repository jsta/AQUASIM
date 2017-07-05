////////////////////////////////   fitsys.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    17.12.91    Peter Reichert
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
//                   +----FIT
//
//   LIST
//     |
//     +----SYMBOLLIST
//            |
//            +----FITLIST
//
//   FILEIO
//     |
//     +----FITSYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// FIT:          class for parameter estimation calculations.
// ----
//
//
//
// FITLIST:
// --------
//
//
//
// FITSYS:
// -------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FITSYS_H
#define FITSYS_H

#include "all.h"
#include "init.h"
#include "symbol.h"
#include "othervar.h"
#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class FITDATA
{
   public:

      FITDATA()
         { var=0; com=0; zone=0; spacetime=0.0; spacerel=FALSE; data=0; }
      FITDATA(const FITDATA& d)
         { var=d.var; com=d.com; zone=d.zone; 
           spacetime=d.spacetime; spacerel=d.spacerel;
           data=d.data; }

      AQVAR*   var;
      AQCOMP*  com;
      CARDINAL zone;
      REAL     spacetime;
      BOOLEAN  spacerel;
      AQVAR*   data;
};

//////////////////////////////////////////////////////////////////////////////

class FIT : public SYMBOL
{
      friend class FITSYS;

   public:

      FIT();
      FIT(const FIT* fit);
      ~FIT();

      FIT*            Next() const
                         { return (FIT*)SYMBOL::Next(); }
      FIT*            Prev() const
                         { return (FIT*)SYMBOL::Prev(); }

      CARDINAL        NumVarArg() const;
      AQVAR*          VarArg(CARDINAL index) const;
      CARDINAL        NumCompArg() const;
      AQCOMP*         CompArg(CARDINAL index) const;

      BOOLEAN         Arg(const AQVAR* var) const;
      BOOLEAN         Arg(const AQCOMP* var) const;

      void            ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void            ExchangeVar(AQVAR* var1, AQVAR* var2);
      void            ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      CARDINAL        CalcNum() const
                         { return calcnumber; }
      BOOLEAN         CalcNum(CARDINAL calcnum);
      REAL            InitTime() const
                         { return inittime; }
      void            InitTime(REAL t)
                         { inittime = t; }
      BOOLEAN         InitSteady() const
                         { return initsteady; }
      void            InitSteady(BOOLEAN b)
                         { initsteady = b; }
      REAL            DataStartTime() const
                         { return datastarttime; }
      void            DataStartTime(REAL t)
                         { datastarttime = t; }
      REAL            DataEndTime() const
                         { return dataendtime; }
      void            DataEndTime(REAL t)
                         { dataendtime = t; }

      BOOLEAN         Add(FITDATA d, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         Replace(FITDATA d, CARDINAL pos);
      BOOLEAN         Delete(CARDINAL pos);
      void            Delete();

      CARDINAL        NumData() const
                         { return numdata; }
      const FITDATA   Data(CARDINAL index) const;

      BOOLEAN         FitActive() const
                         { return fitactive; }
      BOOLEAN         FitActive(BOOLEAN act);

	  JOBSTATUS       Load(std::istream& in,
                           const VARLIST* varlist,
                           const COMPLIST* complist);
      JOBSTATUS       Save(std::ostream& out);
      JOBSTATUS       Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      static CARDINAL number;

      CARDINAL        calcnumber;
      REAL            inittime;
      BOOLEAN         initsteady;

      REAL            datastarttime;
      REAL            dataendtime;

      CARDINAL        numdata;
      FITDATA*        data;

      BOOLEAN         fitactive;

      static INIT     ini;

      void            init();
};

//////////////////////////////////////////////////////////////////////////////

class FITLIST : private SYMBOLLIST
{
      friend class FITSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const FIT* fit) const
                  { return SYMBOLLIST::Exist(fit); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      FIT*     First() const
                  { return (FIT*)SYMBOLLIST::First(); }
      FIT*     Last() const
                  { return (FIT*)SYMBOLLIST::Last(); }
      FIT*     Current() const
                  { return (FIT*)SYMBOLLIST::Current(); }
      FIT*     Next() const
                  { return (FIT*)SYMBOLLIST::Next(); }
      FIT*     Prev() const
                  { return (FIT*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const FIT* fit) const
                  { return SYMBOLLIST::Position(fit); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      FIT*     Get(CARDINAL pos) const
                  { return (FIT*)SYMBOLLIST::Get(pos); }
      FIT*     Get(const char* sym) const
                  { return (FIT*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(FIT* fit)
                  { return SYMBOLLIST::Goto(fit); }
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

      BOOLEAN  Add(FIT* fit)
                  { return SYMBOLLIST::InsertSorted(fit); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(FIT* fit)
                  { return SYMBOLLIST::Remove(fit); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

class FITSYS : public FILEIO
{
   public:

      FITSYS();
      ~FITSYS();

      void      SetCancelFun(CANCELFUN fun)
                   { cancelfun = fun; }
      void      ResetCancelFun()
                   { cancelfun = 0; }

      BOOLEAN   Arg(const AQVAR* var) const;
      BOOLEAN   Arg(const AQCOMP* var) const;

      void      ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void      ExchangeVar(AQVAR* var1, AQVAR* var2);
      void      ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      BOOLEAN   Exist(const FIT* fit) const
                   { return fitlist.Exist(fit); }
      BOOLEAN   Exist(const char* sym) const
                   { return Exist(Get(sym)); }

      FIT*      Get(const char* sym) const
                   { return fitlist.Get(sym); }

      BOOLEAN   Add(FIT* fit);
      BOOLEAN   Replace(FIT* oldfit, FIT* newfit);
      BOOLEAN   Delete(FIT* fit);
      BOOLEAN   Delete(const char* sym)
                   { return Delete(Get(sym)); }
      void      Delete();
      BOOLEAN   FitActive(FIT* fit, BOOLEAN act);

      FITLIST*  Fitlist()
                   { return &fitlist; }

      JOBSTATUS Load(std::istream& in,
                     const VARLIST* varlist,
                     const COMPLIST* complist);
      JOBSTATUS Save(std::ostream& out);
      JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      FITLIST   fitlist;
      CANCELFUN cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
