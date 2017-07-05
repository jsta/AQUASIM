///////////////////////////////   advcomp.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.07.94    Peter Reichert
// revisions:   16.06.95    Peter Reichert    Q=0 enabled, A,z eliminated
//              19.06.95    Peter Reichert    individual diffusivities added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "advcomp" implements a class for advective-diffusive compartments
// =====================================================-===========
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
//                          +----ADVCOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// ADVCOMP:     class for adv column compartments
// ---------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class ADVCOMP
// =============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ADVCOMP_H
#define ADVCOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class ADVCOMP : public AQCOMP
{
      friend class COMPSYS;

   public:

      ADVCOMP();
      ADVCOMP(const ADVCOMP& com);
      ADVCOMP(const AQCOMP* com);
      ~ADVCOMP();

      AQCOMPTYPE      Type() const
                         { return AdvComp; }
      const char*     TypeName() const;

      AQVAR*          SpaceVar(const VARSYS* varsys) const;
      BOOLEAN         AllowedSpaceVal(REAL x) const;

      BOOLEAN         AllowedProgVarType(PROGVARTYPE type) const;
      BOOLEAN         AllowedReplaceVar(const AQVAR* oldvar,
                                        const AQVAR* newvar) const;
      BOOLEAN         AllowedExchangeVar(const AQVAR* var1,
                                         const AQVAR* var2) const;
      BOOLEAN         AllowedComp() const;

      CARDINAL        NumEq() const;
      REAL            RelAcc(CARDINAL i) const;
      REAL            AbsAcc(CARDINAL i) const;
      EQTYPE          EqType(CARDINAL i) const;

      BOOLEAN         InitCond(VARSYS* varsys, REAL* Y,
                               CARDINAL callnum);
      BOOLEAN         Delta(const NUMPAR& numpar, VARSYS* varsys,
                            const REAL* Y, const REAL* YT, REAL* DELTA);

      BOOLEAN         GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                                REAL* t_ptr, const REAL* Y,
                                CARDINAL zone, CARDINAL pt, AQVAR* var,
                                REAL& value);
      BOOLEAN         SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                                 REAL* t_ptr, const REAL* Y,
                                 CARDINAL zone, REAL x, BOOLEAN xrel,
                                 AQVAR* var,
                                 REAL& value);

      BOOLEAN         AddInitCond(const VARLIST* varlist,
                                  CARDINAL zone,
                                  AQVAR* var,
                                  const char* inpline,
                                  char* errorline,
                                  CARDINAL& numparseerrors,
                                  CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         ReplaceInitCond(const VARLIST* varlist,
                                      CARDINAL zone,
                                      AQVAR* var,
                                      const char* inpline,
                                      char* errorline,
                                      CARDINAL& numparseerrors,
                                      CARDINAL pos);
      BOOLEAN         DeleteInitCond(CARDINAL pos);
      CARDINAL        NumInitCond() const
                         { return numinit; }
      CARDINAL        InitZone(CARDINAL index) const;
      AQVAR*          InitVar(CARDINAL index) const;
      const FORMVAR*  InitVal(CARDINAL index) const;

      BOOLEAN         Inflow(const VARLIST* varlist,
                             const char* inpline,
                             char* errorline);
      const FORMVAR*  Inflow() const
                         { return inflow; }
      BOOLEAN         AddInput(const VARLIST* varlist,
                               AQVAR* var,
                               const char* inpline,
                               char* errorline,
                               CARDINAL& numparseerrors,
                               CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         ReplaceInput(const VARLIST* varlist,
                                   AQVAR* var,
                                   const char* inpline,
                                   char* errorline,
                                   CARDINAL& numparseerrors,
                                   CARDINAL pos);
      BOOLEAN         DeleteInput(CARDINAL pos);
      CARDINAL        NumInput() const
                         { return numinput; }
      AQVAR*          InputVar(CARDINAL index) const;
      const FORMVAR*  InputFlux(CARDINAL index) const;
      BOOLEAN         LatInflow(const VARLIST* varlist,
                                const char* inpline,
                                char* errorline);
      const FORMVAR*  LatInflow() const
                         { return latinflow; }
      BOOLEAN         AddLatInput(const VARLIST* varlist,
                                  AQVAR* var,
                                  const char* inpline,
                                  char* errorline,
                                  CARDINAL& numparseerrors,
                                  CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         ReplaceLatInput(const VARLIST* varlist,
                                      AQVAR* var,
                                      const char* inpline,
                                      char* errorline,
                                      CARDINAL& numparseerrors,
                                      CARDINAL pos);
      BOOLEAN         DeleteLatInput(CARDINAL pos);
      CARDINAL        NumLatInput() const
                         { return numlatinput; }
      AQVAR*          LatInputVar(CARDINAL index) const;
      const FORMVAR*  LatInputConc(CARDINAL index) const;

      BOOLEAN         XStartEnd(REAL xs, REAL xe);
      REAL            XStart() const
                         { return Xs; }
      REAL            XEnd() const
                         { return Xe; }
      BOOLEAN         CrossSect(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors);
      const FORMVAR*  CrossSect()
                         { return A; }

      BOOLEAN         WithDiffusion() const
                         { return withdiffusion; }
      BOOLEAN         WithDiffusion(BOOLEAN b);
      BOOLEAN         GlobDiff(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR*  GlobDiff()
                         { return globdiff; }
      BOOLEAN         AddIndDiff(const VARLIST* varlist,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         ReplaceIndDiff(const VARLIST* varlist,
                                     AQVAR* var,
                                     const char* inpline,
                                     char* errorline,
                                     CARDINAL& numparseerrors,
                                     CARDINAL pos);
      BOOLEAN         DeleteIndDiff(CARDINAL pos);
      CARDINAL        NumIndDiff() const
                         { return numinddiff; }
      AQVAR*          IndDiffVar(CARDINAL index) const;
      const FORMVAR*  IndDiffVal(CARDINAL index) const;

      CARDINAL        NumGridPts() const
                         { return numgrid; }
      BOOLEAN         NumGridPts(CARDINAL n);
      REAL            GridPt(const REAL* Y, CARDINAL index);

      CARDINAL        NumZones() const;
      const char*     ZoneName(CARDINAL index) const;

      CARDINAL        NumZoneGridPts(CARDINAL zone);
      REAL            ZoneGridPt(const REAL* Y, CARDINAL zone,
                                 CARDINAL index);

      BOOLEAN         HighRes() const
                         { return highres; }
      void            HighRes(BOOLEAN high)
                         { highres = high; }

      const char*     AdvInName(CARDINAL conn) const;
      const char*     AdvOutName(CARDINAL conn) const;
      const char*     DiffName(CARDINAL conn) const;
      BOOLEAN         AdvInExZ(CARDINAL conn) const;
      REAL            AdvInZ(const REAL* Y, CARDINAL conn) const;
      REAL            AdvOutQ(VARSYS* varsys,
                              const REAL* Y,
                              CARDINAL conn);
      REAL            AdvOutJ(VARSYS* varsys,
                              const REAL* Y,
                              CARDINAL conn,
                              const AQVAR* var);
      REAL            DiffC(const REAL* Y,
                            CARDINAL conn,
                            const AQVAR* var) const;

	  BOOLEAN         AccQ(REAL relacc, REAL absacc);
      BOOLEAN         AccD(REAL relacc, REAL absacc);
      REAL            RelAccQ() const
                         { return relaccQ; }
      REAL            AbsAccQ() const
                         { return absaccQ; }
      REAL            RelAccD() const
                         { return relaccD; }
      REAL            AbsAccD() const
                         { return absaccD; }

      JOBSTATUS       Load(std::istream& in,
                           const VARLIST* varlist, const PROCLIST* proclist);
      JOBSTATUS       Save(std::ostream& out);
      JOBSTATUS       Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      CARDINAL        numinit;
      CARDINAL*       initzone;
      AQVAR**         initvar;
      FORMVAR**       initval;

      FORMVAR*        inflow;
      CARDINAL        numinput;
      AQVAR**         inputvar;
      FORMVAR**       inputflux;
      FORMVAR*        latinflow;
      CARDINAL        numlatinput;
      AQVAR**         latinputvar;
      FORMVAR**       latinputconc;

      static CARDINAL ioff;

      REAL            Xs;
      REAL            Xe;
      REAL*           gridpts;
      FORMVAR*        A;

      BOOLEAN         withdiffusion;
      FORMVAR*        globdiff;
      CARDINAL        numinddiff;
      AQVAR**         inddiffvar;
      FORMVAR**       inddiffval;

      BOOLEAN         highres;
      REAL            g;

      REAL            relaccQ;
      REAL            absaccQ;
      REAL            relaccD;
      REAL            absaccD;

      REAL            Qout;
      CARDINAL        numCout;
      REAL*           Cout;

      void            ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void            ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN         SetGridPts(CARDINAL n);
      void            CalcArg();

      PROGVAR*        Xvar;
      PROGVAR*        Avar;
      PROGVAR*        Qvar;

      REAL            FluxLimit(REAL s1, REAL s2, CARDINAL limiter);

      void            init();
      void            del();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
