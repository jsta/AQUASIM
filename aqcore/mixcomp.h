///////////////////////////////   mixcomp.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    10.12.91    Peter Reichert
// revisions:   26.10.92    Peter Reichert    redesign of links
//              11.12.92    Peter Reichert    variable volume added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "mixcomp" implements a class for mixed reactor compartments
// ===========================================================
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
//                          |
//                          +----MIXCOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// MIXCOMP:      class for mixed compartments
// --------
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class MIXCOMP
// ============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MIXCOMP_H
#define MIXCOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class MIXCOMP : public AQCOMP
{
      friend class COMPSYS;

   public:

      MIXCOMP();
      MIXCOMP(const MIXCOMP& com);
      MIXCOMP(const AQCOMP* com);
      ~MIXCOMP();

      AQCOMPTYPE     Type() const
                        { return MixComp; }
      const char*    TypeName() const;

      AQVAR*         SpaceVar(const VARSYS*) const
                        { return 0; }
      BOOLEAN        AllowedSpaceVal(REAL) const
                        { return TRUE; }

      BOOLEAN        AllowedProgVarType(PROGVARTYPE type) const;
      BOOLEAN        AllowedReplaceVar(const AQVAR* oldvar,
                                       const AQVAR* newvar) const;
      BOOLEAN        AllowedExchangeVar(const AQVAR* var1,
                                        const AQVAR* var2) const;
      BOOLEAN        AllowedComp() const;

      BOOLEAN        AddInitCond(const VARLIST* varlist,
                                 CARDINAL zone,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceInitCond(const VARLIST* varlist,
                                     CARDINAL zone,
                                     AQVAR* var,
                                     const char* inpline,
                                     char* errorline,
                                     CARDINAL& numparseerrors,
                                     CARDINAL pos);
      BOOLEAN        DeleteInitCond(CARDINAL pos);
      CARDINAL       NumInitCond() const
                        { return numinit; }
      CARDINAL       InitZone(CARDINAL index) const;
      AQVAR*         InitVar(CARDINAL index) const;
      const FORMVAR* InitVal(CARDINAL index) const;

      BOOLEAN        Inflow(const VARLIST* varlist,
                            const char* inpline,
                            char* errorline);
      const FORMVAR* Inflow() const
                        { return inflow; }
      BOOLEAN        AddInput(const VARLIST* varlist,
                              AQVAR* var,
                              const char* inpline,
                              char* errorline,
                              CARDINAL& numparseerrors,
                              CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceInput(const VARLIST* varlist,
                                  AQVAR* var,
                                  const char* inpline,
                                  char* errorline,
                                  CARDINAL& numparseerrors,
                                  CARDINAL pos);
      BOOLEAN        DeleteInput(CARDINAL pos);
      CARDINAL       NumInput() const
                        { return numinput; }
      AQVAR*         InputVar(CARDINAL index) const;
      const FORMVAR* InputFlux(CARDINAL index) const;

      CARDINAL       NumGridPts() const
                        { return 1; }
      BOOLEAN        NumGridPts(CARDINAL n);
      REAL           GridPt(const REAL* Y, CARDINAL index);

      CARDINAL       NumZoneGridPts(CARDINAL zone);
      REAL           ZoneGridPt(const REAL* Y, CARDINAL zone,
                                CARDINAL index);

      CARDINAL       NumEq() const;
      REAL           RelAcc(CARDINAL i) const;
      REAL           AbsAcc(CARDINAL i) const;
      EQTYPE         EqType(CARDINAL i) const;

      BOOLEAN        InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum);
      BOOLEAN        Delta(const NUMPAR& numpar, VARSYS* varsys,
                           const REAL* Y, const REAL* YT, REAL* DELTA);

      BOOLEAN        GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                               REAL* t_ptr, const REAL* Y,
                               CARDINAL zone, CARDINAL pt, AQVAR* var,
                               REAL& value);
      BOOLEAN        SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                                REAL* t_ptr, const REAL* Y,
                                CARDINAL zone, REAL x, BOOLEAN xrel,
                                AQVAR* var,
                                REAL& value);

      BOOLEAN        FixedVol() const
                        { return fixedvol; }
      BOOLEAN        FixedVol(BOOLEAN b);
      REAL           Vol() const
                        { return vol; }
      BOOLEAN        Vol(REAL v);
      BOOLEAN        Outflow(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors);
      const FORMVAR* Outflow()
                        { return Qout; }

      CARDINAL       NumZones() const;
      const char*    ZoneName(CARDINAL index) const;

      const char*    AdvInName(CARDINAL conn) const;
      const char*    AdvOutName(CARDINAL conn) const;
      const char*    DiffName(CARDINAL conn) const;
      BOOLEAN        AdvInExZ(CARDINAL conn) const;
      REAL           AdvInZ(const REAL* Y, CARDINAL conn) const;
      REAL           AdvOutQ(VARSYS* varsys,
                             const REAL* Y,
                             CARDINAL conn);
      REAL           AdvOutJ(VARSYS* varsys,
                             const REAL* Y,
                             CARDINAL conn,
                             const AQVAR* var);
      REAL           DiffC(const REAL* Y,
                           CARDINAL conn,
                           const AQVAR* var) const;

      BOOLEAN        AccQ(REAL relacc, REAL absacc);
      BOOLEAN        AccV(REAL relacc, REAL absacc);
      REAL           RelAccQ() const
                        { return relaccQ; }
      REAL           AbsAccQ() const
                        { return absaccQ; }
      REAL           RelAccV() const
                        { return relaccV; }
      REAL           AbsAccV() const
                        { return absaccV; }

	  JOBSTATUS      Load(std::istream& in,const VARLIST* varlist,
                          const PROCLIST* proclist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      CARDINAL       numinit;
      CARDINAL*      initzone;
      AQVAR**        initvar;
      FORMVAR**      initval;

      FORMVAR*       inflow;
      CARDINAL       numinput;
      AQVAR**        inputvar;
      FORMVAR**      inputflux;

      BOOLEAN        fixedvol;
      REAL           vol;
      FORMVAR*       Qout;
      REAL*          Cout;
      CARDINAL       numCout;

      REAL           relaccQ;
      REAL           absaccQ;
      REAL           relaccV;
      REAL           absaccV;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        SetGridPts(CARDINAL n);
      void           CalcArg();

      void           init();
      void           del();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
