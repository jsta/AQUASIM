////////////////////////////////   calcsys.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    15.03.93    Peter Reichert
// revisions:   20.07.97    Peter Reichert    changed from sens to calc
//              17.05.04    Peter Reichert    converted to ANSI C++
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
//                   +----CALC
//
//   LIST
//     |
//     +----SYMBOLLIST
//            |
//            +----CALCLIST
//
//   FILEIO
//     |
//     +----CALCSYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// CALC:          class for calculations.
// ----
//
//
//
// CALCLIST:
// --------
//
//
//
// CALCSYS:
// -------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef CALCSYS_H
#define CALCSYS_H

#include "othervar.h"
#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class CALC : public SYMBOL
{
      friend class CALCSYS;

   public:

      CALC();
      CALC(const CALC* calc);
      ~CALC();

      CALC*           Next() const
                         { return (CALC*)SYMBOL::Next(); }
      CALC*           Prev() const
                         { return (CALC*)SYMBOL::Prev(); }

      void            CalcNum(CARDINAL c);
      CARDINAL        CalcNum() const;
      void            InitTime(REAL t);
      REAL            InitTime() const;
      void            InitSteady(BOOLEAN s);
      BOOLEAN         InitSteady() const;
      CARDINAL        NumStepSeries() const;
      REAL            StepSize(CARDINAL index) const;
      CARDINAL        NumSteps(CARDINAL index) const;
      BOOLEAN         AddStepSeries(REAL s, CARDINAL n, 
                                    CARDINAL index);
      BOOLEAN         ReplaceStepSeries(REAL s, CARDINAL n, 
                                        CARDINAL index);
      BOOLEAN         DelStepSeries(CARDINAL index);

      BOOLEAN         SimActive() const
                         { return simactive; }
      BOOLEAN         SimActive(BOOLEAN act);
      BOOLEAN         SensActive() const
                         { return sensactive; }
      BOOLEAN         SensActive(BOOLEAN act);

	  JOBSTATUS       Load(std::istream& in);
	  JOBSTATUS       Save(std::ostream& out);
	  JOBSTATUS       Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      static CARDINAL number;

      CARDINAL        calcnum;
      REAL            inittime;
      BOOLEAN         initsteady;
      CARDINAL        numstepseries;
      REAL*           stepsize;
      CARDINAL*       numsteps;

      BOOLEAN         simactive;
      BOOLEAN         sensactive;

      static INIT     ini;

      void            init();
};

//////////////////////////////////////////////////////////////////////////////

class CALCLIST : private SYMBOLLIST
{
      friend class CALCSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const CALC* sens) const
                  { return SYMBOLLIST::Exist(sens); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      CALC*    First() const
                  { return (CALC*)SYMBOLLIST::First(); }
      CALC*    Last() const
                  { return (CALC*)SYMBOLLIST::Last(); }
      CALC*    Current() const
                  { return (CALC*)SYMBOLLIST::Current(); }
      CALC*    Next() const
                  { return (CALC*)SYMBOLLIST::Next(); }
      CALC*    Prev() const
                  { return (CALC*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const CALC* calc) const
                  { return SYMBOLLIST::Position(calc); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      CALC*    Get(CARDINAL pos) const
                  { return (CALC*)SYMBOLLIST::Get(pos); }
      CALC*    Get(const char* sym) const
                  { return (CALC*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(CALC* calc)
                  { return SYMBOLLIST::Goto(calc); }
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

      BOOLEAN  Add(CALC* calc)
                  { return SYMBOLLIST::InsertSorted(calc); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(CALC* calc)
                  { return SYMBOLLIST::Remove(calc); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

class CALCSYS : public FILEIO
{
   public:

      CALCSYS();
      ~CALCSYS();

      void      SetCancelFun(CANCELFUN fun)
                   { cancelfun = fun; }
      void      ResetCancelFun()
                   { cancelfun = 0; }

      BOOLEAN   Exist(const CALC* calc) const
                   { return calclist.Exist(calc); }
      BOOLEAN   Exist(const char* sym) const
                   { return Exist(Get(sym)); }

      CALC*     Get(const char* sym) const
                   { return calclist.Get(sym); }

      BOOLEAN   Add(CALC* calc);
      BOOLEAN   Replace(CALC* oldcalc, CALC* newcalc);
      BOOLEAN   Delete(CALC* calc);
      BOOLEAN   Delete(const char* sym)
                   { return Delete(Get(sym)); }
      void      Delete();
      BOOLEAN   SimActive(CALC* calc, BOOLEAN act);
      BOOLEAN   SensActive(CALC* calc, BOOLEAN act);

      CALCLIST* Calclist()
                   { return &calclist; }

	  JOBSTATUS Load(std::istream& in);
	  JOBSTATUS Save(std::ostream& out);
	  JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      CALCLIST  calclist;
      CANCELFUN cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
