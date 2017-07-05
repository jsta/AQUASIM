////////////////////////////////   aqvar.h   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    23.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    AQVAR::AQVAR(const AQVAR&)
//                                            constructor added
//              12.01.92    Peter Reichert    storing of values added
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
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
//                   +----AQVAR
//
//   LIST
//     |
//     +----SYMBOLLIST
//            |
//            +----VARLIST
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// AQVAR:        virtual base class for variables.
// ------
//
//
//
// VARLIST:
// --------
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class AQVAR
// ==========================
//
// virtual AQVARTYPE Type() const        returns type of variable. Virtual
//                                       member.
//
// virtual BOOLEAN AllowedVar()
//
//
//
// virtual REAL Value()                  returns the actual value of the 
//                                       variable. Virtual member.
//
// const char* Unit() const              returns a const pointer to the
//                                       unit of the variable.
//
// void Unit(const char* u)              replaces the unit of the variable
//                                       by the string u.
//
// CARDINAL NumArg() const               returns the number of arguments of
//                                       the variable.
//
// AQVAR* const* Arg() const             returns a pointer to an array of
//                                       NumArg() const pointers to the direct
//                                       arguments (of type AQVAR) of the var-
//                                       iable.
//
// BOOLEAN Arg(AQVAR* var) const         returns TRUE if var is an argument
//                                       (on any level) of the variable;
//                                       FALSE otherwise.
//
// BOOLEAN ArgVarType(AQVARTYPE type) const
//
//
//
// virtual BOOLEAN ArgProgVarType(PROGVARTYPE type,
//                                const AQVAR* besides=0) const
//
//
//
// virtual BOOLEAN ReplaceArg(AQVAR* oldvar, AQVAR* newvar)
//                                       replaces the argument oldvar of the 
//                                       variable with the argument newvar and
//                                       returns TRUE if the variable is not
//                                       an element of a list; does not change
//                                       the arguments and returns FALSE
//                                       otherwise.
//
// virtual BOOLEAN replacearg(AQVAR* oldvar, AQVAR* newvar)
//                                       (protected method) analogous to
//                                       Argument, but performs its action
//                                       also if the variable is an element of
//                                       a list.
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class VARLIST
// ============================
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AQVAR_H
#define AQVAR_H

#include "all.h"
#include "init.h"
#include "symbol.h"

//////////////////////////////////////////////////////////////////////////////

enum AQVARTYPE    {
                     StateVar,
                     ProgVar,
                     ConstVar,
                     RealListVar,
                     VarListVar,
                     FormVar,
                     ProbeVar
                  };

enum STATEVARTYPE {
                     VolStateVar,
                     SurfStateVar,
                     EquStateVar
                  };


enum PROGVARTYPE  {  // changes must be updated in PROGVAR::NumProgVarTypes !!
                     CalcNumber,           //   1
                     Time,                 //   2
                     CompIndex,            //   3
                     ZoneIndex,            //   4
                     LinkIndex,            //   5
                     Discharge,            //   6
                     WaterFraction,        //   7

                     SpaceX,               //   8
                     SpaceZ,               //   9

                     Volume,               //  10

                     BulkVolume,           //  11
                     FilmDepth,            //  12
                     FilmVelocity,         //  13
                     InterfaceVelocity,    //  14
                     DetachmentVelocity,   //  15
                     AttachmentVelocity,   //  16

                     WaterLevel,           //  17
                     CrossSection,         //  18
                     Perimeter,            //  19
                     Width,                //  20
                     FrictSlope,           //  21

                     Density,              //  22
                     AreaGradient,         //  23
                     LightIntensity,       //  24
                     BruntVaisala,         //  25
                     HorizVelocity,        //  26
                     TurbKinEnergy,        //  27
                     ShearProd,            //  28
                     BuoyancyProd,         //  29
                     Dissipation,          //  30
                     SeicheEnergy          //  31
                  };

//////////////////////////////////////////////////////////////////////////////

class AQCOMP;
class COMPLIST;
class VARSYS;

class AQVAR : public SYMBOL
{
      friend class VARSYS;

   public:

      AQVAR();
      AQVAR(const AQVAR& var);
      AQVAR(const AQVAR* var);
      ~AQVAR();

      AQVAR*              Next() const
                             { return (AQVAR*)SYMBOL::Next(); }
      AQVAR*              Prev() const
                             { return (AQVAR*)SYMBOL::Prev(); }

      virtual AQVARTYPE   Type() const = 0;
      virtual const char* TypeName() const = 0;

      virtual BOOLEAN     AllowedVar();

      virtual BOOLEAN     UpdateComp(COMPLIST* complist)
                             { return TRUE; }

      void                Reset()
                             { evaluated = FALSE; }
      virtual REAL        Evaluate() = 0;
      REAL                Value();

      const char*         Unit() const
                             { return unit; }
      void                Unit(const char* u);

      virtual BOOLEAN     SensActive() const
                             { return FALSE; }
      virtual BOOLEAN     SensActive(BOOLEAN act);
      virtual BOOLEAN     SetSensValue(BOOLEAN b=FALSE);
      virtual BOOLEAN     SensValue() const
                             { return FALSE; }

      static CARDINAL     NumTimeUnits();
      static const char*  TimeUnits(CARDINAL index);
      static BOOLEAN      TimeUnit(const char* u);
      static REAL         TimeUnitFactor(const char* u);
      static CARDINAL     NumSpaceUnits();
      static const char*  SpaceUnits(CARDINAL index);
      static BOOLEAN      SpaceUnit(const char* u);
      static REAL         SpaceUnitFactor(const char* u);

      virtual CARDINAL    NumVarArg() const
                            { return 0; }
      virtual AQVAR*      VarArg(CARDINAL index) const;
      BOOLEAN             Arg(const AQVAR* var) const;
      BOOLEAN             ReplaceVarArg(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN             ArgVarType(AQVARTYPE type) const;
      virtual BOOLEAN     ArgProgVarType(PROGVARTYPE type,
                                         const AQVAR* besides=0) const;
      BOOLEAN             VarArgSaved();
      virtual CARDINAL    NumCompArg() const
                            { return 0; }
      virtual AQCOMP*     CompArg(CARDINAL index) const;
      BOOLEAN             Arg(const AQCOMP* comp) const;
      BOOLEAN             ReplaceCompArg(AQCOMP* oldcomp, AQCOMP* newcomp);

	  JOBSTATUS           Load(std::istream& in);
      JOBSTATUS           Save(std::ostream& out);
      JOBSTATUS           Write(std::ostream& out, BOOLEAN sh=FALSE);

      void                Test();

   protected:

      virtual void        ReplVarArg(AQVAR* oldvar, AQVAR* newvar);
      virtual void        ReplCompArg(AQCOMP* oldcomp, AQCOMP* newcomp);

      static INIT         ini;

   private:

      static CARDINAL     number;

      char*               unit;

      REAL                lastvalue;
      BOOLEAN             evaluated;
      static BOOLEAN      reset;

      void                init();
};

//////////////////////////////////////////////////////////////////////////////

class VARLIST : private SYMBOLLIST
{
      friend class VARSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const AQVAR* var) const
                  { return SYMBOLLIST::Exist(var); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      AQVAR*   First() const
                  { return (AQVAR*)SYMBOLLIST::First(); }
      AQVAR*   Last() const
                  { return (AQVAR*)SYMBOLLIST::Last(); }
      AQVAR*   Current() const
                  { return (AQVAR*)SYMBOLLIST::Current(); }
      AQVAR*   Next() const
                  { return (AQVAR*)SYMBOLLIST::Next(); }
      AQVAR*   Prev() const
                  { return (AQVAR*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const AQVAR* var) const
                  { return SYMBOLLIST::Position(var); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      AQVAR*   Get(CARDINAL pos) const
                  { return (AQVAR*)SYMBOLLIST::Get(pos); }
      AQVAR*   Get(const char* sym) const
                  { return (AQVAR*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(AQVAR* var)
                  { return SYMBOLLIST::Goto(var); }
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

      BOOLEAN  Add(AQVAR* var)
                  { return SYMBOLLIST::InsertSorted(var); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(AQVAR* var)
                  { return SYMBOLLIST::Remove(var); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
