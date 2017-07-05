///////////////////////////////   othervar.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    24.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    ?VAR::?VAR(const ?VAR&)
//                                            constructor added.
//              29.12.91    Peter Reichert    Interpolation methods added
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//              21.03.95    Peter Reichert    ProbeVar added
//              26.07.98    Peter Reichert    relative space added to ProbeVar
//              05.11.98    Peter Reichert    CARDLONG introduced in REALLIST
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
//                          |
//                          +----STATEVAR
//                          |
//                          +----PROGVAR
//                          |
//                          +----CONSTVAR
//                          |
//                          +----REALLISTVAR
//                          |
//                          +----VARLISTVAR
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// STATEVAR:     class for state variables to be calculated as solution of
// ---------     a system of algebraic and differential equations
//
// PROGVAR:      class for program variables defined internally by the pro-
// --------      gram
//
// CONSTVAR:     class for constants
// --------
//
// REALLISTVAR:  class for variables interpolated linearly between given
// ------------  data points
//
// VARLISTVAR:   class for variables interpolated linearly between pairs of
// -----------   data points and other variables
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class STATEVAR
// =============================
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class PROGVAR
// ============================
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class CONSTVAR
// =============================
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class REALLISTVAR
// ================================
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class VARLISTVAR
// ===============================
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef OTHERVAR_H
#define OTHERVAR_H

#include "all.h"
#include "aqvar.h"

//////////////////////////////////////////////////////////////////////////////

enum INTPOLMETH {
                   linear,
                   spline,
                   smooth
                };

//////////////////////////////////////////////////////////////////////////////

void SetGlobalState(VARSYS* varsys, CARDINAL* calcnum_ptr,
                    REAL* t_ptr, const REAL* Y);

//////////////////////////////////////////////////////////////////////////////

	  
class STATEVAR : public AQVAR 
{
      friend class VARSYS;

   public:

      STATEVAR();
      STATEVAR(const STATEVAR& var);
      STATEVAR(const AQVAR* var);
      ~STATEVAR();

      STATEVAR&          operator=(const STATEVAR& var);

      AQVARTYPE          Type() const
                            { return StateVar; }
      const char*        TypeName() const;

      void               SetStateVar(const REAL* var)
                            { statevar = var; }
      void               ResetStateVar()
                            { statevar = 0; }
      const REAL*        GetStateVar() const
                            { return statevar; }

      REAL               Evaluate();

      STATEVARTYPE       StateVarType() const
                            { return statevartype; }
      BOOLEAN            StateVarType(STATEVARTYPE type);

      REAL               RelAccuracy() const
                            { return relaccuracy; }
      BOOLEAN            RelAccuracy(REAL acc);
      REAL               AbsAccuracy() const
                            { return absaccuracy; }
      BOOLEAN            AbsAccuracy(REAL acc);
      BOOLEAN            Accuracy(REAL relacc, REAL absacc);

	  JOBSTATUS          Load(std::istream& in);
      JOBSTATUS          Save(std::ostream& out);
      JOBSTATUS          Write(std::ostream& out, BOOLEAN sh=FALSE);

      void               Test();

   protected:

      const REAL*        statevar;

      STATEVARTYPE       statevartype;

      REAL               relaccuracy;
      REAL               absaccuracy;

      void               init();
};

//////////////////////////////////////////////////////////////////////////////

class PROGVAR : public AQVAR 
{
      friend class VARSYS;

   public:

      PROGVAR();
      PROGVAR(const PROGVAR& var);
      PROGVAR(const AQVAR* var);
      ~PROGVAR();

      AQVARTYPE          Type() const
                            { return ProgVar; }
      const char*        TypeName() const;

      BOOLEAN            ArgProgVarType(PROGVARTYPE type,
                                        const AQVAR* besides=0) const;

      static CARDINAL    NumProgVarTypes();
      PROGVARTYPE        ProgVarType() const
                            { return progvartype; }
      BOOLEAN            ProgVarType(PROGVARTYPE type);
      static const char* ProgVarTypeName(PROGVARTYPE type);

      void               SetProgVar(const REAL* var)
                            { progvar = var; }
      const REAL*        GetProgVar() const
                            { return progvar; }

      REAL               Evaluate();

      JOBSTATUS          Load(std::istream& in);
      JOBSTATUS          Save(std::ostream& out);
      JOBSTATUS          Write(std::ostream& out, BOOLEAN sh=FALSE);

      void               Test();

   protected:

      PROGVARTYPE        progvartype;

      const REAL*        progvar;

      void               init();
};

//////////////////////////////////////////////////////////////////////////////

class CONSTVAR : public AQVAR 
{
   public:

      CONSTVAR();
      CONSTVAR(const CONSTVAR& var);
      CONSTVAR(AQVAR* var);
      ~CONSTVAR();

      AQVARTYPE   Type() const
                     { return ConstVar; }
      const char* TypeName() const;

      REAL        Evaluate();
      BOOLEAN     SetValue(REAL val);

      REAL        StDev() const
                     { return stdev; }
      BOOLEAN     StDev(REAL s);

      REAL        Minimum() const
                     { return minimum; }
      BOOLEAN     Minimum(REAL mini);

      REAL        Maximum() const
                     { return maximum; }
      BOOLEAN     Maximum(REAL maxi);

      BOOLEAN     SensActive() const
                     { return sensactive; }
      BOOLEAN     SensActive(BOOLEAN act);
      BOOLEAN     FitActive() const
                     { return fitactive; }
      BOOLEAN     FitActive(BOOLEAN act);

      BOOLEAN     Set(REAL val, REAL s, REAL mini, REAL maxi,
                     BOOLEAN sensact, BOOLEAN fitact);

      BOOLEAN     SetSensValue(BOOLEAN b);
      BOOLEAN     SensValue() const
                     { return sensvalue; }

      JOBSTATUS   Load(std::istream& in);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

      void        Test();

   protected:

      REAL        value;
      REAL        stdev;
      REAL        minimum;
      REAL        maximum;

      BOOLEAN     sensactive;
      BOOLEAN     fitactive;
      BOOLEAN     sensvalue;

      void        init();
};

//////////////////////////////////////////////////////////////////////////////

class REALLISTVAR : public AQVAR 
{
   public:

      REALLISTVAR();
      REALLISTVAR(REALLISTVAR& var);
      REALLISTVAR(AQVAR* var);
      ~REALLISTVAR();

      AQVARTYPE   Type() const
                     { return RealListVar; }
      const char* TypeName() const;

      BOOLEAN     AllowedVar();

      CARDINAL    NumVarArg() const;
      AQVAR*      VarArg(CARDINAL index) const;

      REAL        Evaluate();

      BOOLEAN     SetArg(AQVAR* var);
      AQVAR*      GetArg() const
                     { return listarg; }

      BOOLEAN     Add(REAL x, REAL y, REAL sy=0.0);
      BOOLEAN     Replace(CARDLONG j, REAL x, REAL y, REAL sy=0.0);
      BOOLEAN     Delete(CARDLONG j);
      BOOLEAN     Delete();

      BOOLEAN     StDevGlobal() const
                     { return stdevglobal; }
      void        StDevGlobal(BOOLEAN global);
      BOOLEAN     DeleteIndividualStDev();
      REAL        AbsStDev() const
                     { return absstdev; }
      BOOLEAN     AbsStDev(REAL s);
      REAL        RelStDev() const
                     { return relstdev; }
      BOOLEAN     RelStDev(REAL s);
      BOOLEAN     SetStDev(REAL srel, REAL sabs);
      REAL        StDev(REAL argval);

      REAL        Minimum() const
                     { return minimum; }
      BOOLEAN     Minimum(REAL mini);

      REAL        Maximum() const
                     { return maximum; }
      BOOLEAN     Maximum(REAL maxi);

      CARDLONG    NumPairs() const
                     { return n; }
      const REAL* X() const
                            { return xa; }
      const REAL* Y() const
                     { return ya; }
      REAL        GetStDev(CARDLONG i);

      INTPOLMETH  Method() const
                     { return method; }
      void        Method(INTPOLMETH m);

      REAL        SmoothWidth() const
                     { return width; }
      BOOLEAN     SmoothWidth(REAL w);

      BOOLEAN     SensActive() const
                     { return sensactive; }
      BOOLEAN     SensActive(BOOLEAN act);

      BOOLEAN     ReadData(std::istream&  in,
                           CARDLONG* numread,
                           CARDINAL  colx,
                           CARDINAL  coly,
                           CARDINAL  colsy=0,
                           CARDLONG  firstline=1,
                           CARDLONG  lastline=0);
      BOOLEAN     WriteData(std::ostream& out);

      BOOLEAN     SetSensValue(BOOLEAN b);
      BOOLEAN     SensValue() const
                     { return sensvalue; }

      JOBSTATUS   Load(std::istream& in, const VARLIST* varlist);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

	  CARDLONG    ArgIndex(REAL argval);

      void        Test();

   protected:

      AQVAR*      listarg;
      CARDLONG    n;
      CARDLONG    last;
      REAL*       xa;
      REAL*       ya;

      BOOLEAN     stdevglobal;
      REAL        absstdev;
      REAL        relstdev;
      REAL        minimum;
      REAL        maximum;
      REAL*       sya;
      BOOLEAN     sensactive;
      BOOLEAN     sensvalue;

      INTPOLMETH  method;
      REAL        width;
      REAL*       yaxx;

      REAL        Interpolate(REAL argval);

      BOOLEAN     SkipFields(std::istream& in, CARDINAL num);
      BOOLEAN     ReadField(std::istream& in, REAL* r);

      void        ReplVarArg(AQVAR* oldvar, AQVAR* newvar);

      void        init();
};

//////////////////////////////////////////////////////////////////////////////

class VARLISTVAR : public AQVAR 
{
   public:

      VARLISTVAR();
      VARLISTVAR(VARLISTVAR& var);
      VARLISTVAR(AQVAR* var);
      ~VARLISTVAR();

      AQVARTYPE   Type() const
                     { return VarListVar; }
      const char* TypeName() const;

      BOOLEAN     AllowedVar();

      CARDINAL    NumVarArg() const;
      AQVAR*      VarArg(CARDINAL index) const;

      REAL        Evaluate();

      BOOLEAN     SetArg(AQVAR* var);
      AQVAR*      GetArg() const
                     { return listarg; }

      BOOLEAN     Add(REAL x, AQVAR* yvar);
      BOOLEAN     Replace(CARDINAL j, REAL x, AQVAR* yvar);
      BOOLEAN     Delete(CARDINAL j);
      BOOLEAN     Delete();

      CARDINAL    NumPairs() const
                     { return n; }
      const REAL* X() const
                     { return xa; }
      AQVAR* const* Yvar() const
                     { return yvara; }

      INTPOLMETH  Method() const
                     { return method; }
      void        Method(INTPOLMETH m);

      REAL        SmoothWidth() const
                     { return width; }
      BOOLEAN     SmoothWidth(REAL w);

      JOBSTATUS   Load(std::istream& in, const VARLIST* varlist);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

	  CARDINAL    ArgIndex(REAL argval);

      void        Test();

   protected:

      AQVAR*      listarg;
      CARDINAL    n;
      CARDINAL    last;
      REAL*       xa;
      AQVAR**     yvara;

      INTPOLMETH  method;
      REAL        width;

      void        ReplVarArg(AQVAR* oldvar, AQVAR* newvar);

      void        init();
};

//////////////////////////////////////////////////////////////////////////////

class PROBEVAR : public AQVAR 
{
      friend class VARSYS;

   public:

      PROBEVAR();
      PROBEVAR(const PROBEVAR& var);
      PROBEVAR(const AQVAR* var);
      ~PROBEVAR();

      PROBEVAR&          operator=(const PROBEVAR& var);

      AQVARTYPE          Type() const
                            { return ProbeVar; }
      const char*        TypeName() const;

      BOOLEAN            AllowedVar();

      REAL               Evaluate();

      CARDINAL           NumVarArg() const;
      AQVAR*             VarArg(CARDINAL index) const;
      CARDINAL           NumCompArg() const;
      AQCOMP*            CompArg(CARDINAL index) const;

      AQVAR*             Variable() const
                            { return variable; }
      BOOLEAN            Variable(AQVAR* var);
      AQCOMP*            Compartment() const
                            { return compartment; }
      BOOLEAN            Compartment(AQCOMP* com);
      BOOLEAN            Zone(CARDINAL z);
      CARDINAL           Zone()
                            { return zone; }
      REAL               Space() const
                            { return space; };
      void               Space(REAL x);
      BOOLEAN            SpaceRel() const
                            { return spacerel; } 
      void               SpaceRel(BOOLEAN b);

      BOOLEAN            UpdateComp(COMPLIST* complist);

	  JOBSTATUS          Load(std::istream& in, const VARLIST* varlist);
      JOBSTATUS          Save(std::ostream& out);
      JOBSTATUS          Write(std::ostream& out, BOOLEAN sh=FALSE);

      void               Test();

   protected:

      void               ReplVarArg(AQVAR* oldvar, AQVAR* newvar);
      void               ReplCompArg(AQCOMP* oldcomp, AQCOMP* newcomp);

      AQVAR*             variable;
      AQCOMP*            compartment;
      char*              compname;
      CARDINAL           zone;
      REAL               space;
      BOOLEAN            spacerel;

      void               init();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
