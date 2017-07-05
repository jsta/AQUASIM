///////////////////////////////   sedcomp.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    03.02.01    Peter Reichert
// revisions:   23.01.02    Peter Reichert    Variable sedimentation rate and
//                                            compaction added
//              24.11.03    Peter Reichert    Bottom concentration boundary
//                                            condition added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "sedcomp" implements a class for sediment compartments
// ======================================================
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
//                          +----SEDCOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// SEDCOMP:     class for sediment compartments
// --------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class SEDCOMP
// ============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SEDCOMP_H
#define SEDCOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class SEDCOMP : public AQCOMP
{
      friend class COMPSYS;

   public:

      SEDCOMP();
      SEDCOMP(const SEDCOMP& com);
      SEDCOMP(const AQCOMP* com);
      ~SEDCOMP();

      AQCOMPTYPE     Type() const
                        { return SedComp; }
      const char*    TypeName() const;

      AQVAR*         SpaceVar(const VARSYS* varsys) const;
      BOOLEAN        AllowedSpaceVal(REAL x) const;

      BOOLEAN        AllowedProgVarType(PROGVARTYPE type) const;
      BOOLEAN        AllowedReplaceVar(const AQVAR* oldvar,
                                       const AQVAR* newvar) const;
      BOOLEAN        AllowedExchangeVar(const AQVAR* var1,
                                        const AQVAR* var2) const;
      BOOLEAN        AllowedComp() const;

      CARDINAL       NumEq() const;
      REAL           RelAcc(CARDINAL i) const;
      REAL           AbsAcc(CARDINAL i) const;
      EQTYPE         EqType(CARDINAL i) const;

      BOOLEAN        InitCond(VARSYS* varsys, REAL* Y,
                              CARDINAL callnum);
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

      BOOLEAN        AddTopBC(const VARLIST* varlist,
                              AQVAR* var,
                              const char* inpline,
                              char* errorline,
                              CARDINAL& numparseerrors,
                              CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceTopBC(const VARLIST* varlist,
                                  AQVAR* var,
                                  const char* inpline,
                                  char* errorline,
                                  CARDINAL& numparseerrors,
                                  CARDINAL pos);
      BOOLEAN        DeleteTopBC(CARDINAL pos);
      CARDINAL       NumTopBC() const
                        { return numtopbc; }
      AQVAR*         TopBCVar(CARDINAL index) const;
      const FORMVAR* TopBCCorF(CARDINAL index) const;
      BOOLEAN        AddBottomBC(const VARLIST* varlist,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceBottomBC(const VARLIST* varlist,
                                     AQVAR* var,
                                     const char* inpline,
                                     char* errorline,
                                     CARDINAL& numparseerrors,
                                     CARDINAL pos);
      BOOLEAN        DeleteBottomBC(CARDINAL pos);
      CARDINAL       NumBottomBC() const
                        { return numbottombc; }
      AQVAR*         BottomBCVar(CARDINAL index) const;
      const FORMVAR* BottomBCConc(CARDINAL index) const;

      BOOLEAN        ZTopBottom(REAL zt, REAL zb);
      REAL           ZTop() const
                        { return Zt; }
      REAL           ZBottom() const
                        { return Zb; }

      BOOLEAN        AddDissVar(const VARLIST* varlist,
                                AQVAR* var,
                                const char* inpline,
                                char* errorline,
                                CARDINAL& numparseerrors,
                                CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceDissVar(const VARLIST* varlist,
                                    AQVAR* var,
                                    const char* inpline,
                                    char* errorline,
                                    CARDINAL& numparseerrors,
                                    CARDINAL pos);
      BOOLEAN        DeleteDissVar(CARDINAL pos);
      CARDINAL       NumDissVar() const
                        { return numdissvar; }
      AQVAR*         DissVar(CARDINAL index) const;
      const FORMVAR* Dissdiff(CARDINAL index) const;
      BOOLEAN        AddPartVar(const VARLIST* varlist,
                                AQVAR* var,
                                const char* inprho,
                                char* errorlinerho,
                                CARDINAL& parseerrorsrho,
                                const char* inpbioturb,
                                char* errorlinebioturb,
                                CARDINAL& parseerrorsbioturb,
                                CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplacePartVar(const VARLIST* varlist,
                                    AQVAR* var,
                                    const char* inprho,
                                    char* errorlinerho,
                                    CARDINAL& parseerrorsrho,
                                    const char* inpbioturb,
                                    char* errorlinebioturb,
                                    CARDINAL& parseerrorsbioturb,
                                    CARDINAL pos);
      BOOLEAN        DeletePartVar(CARDINAL pos);
      CARDINAL       NumPartVar() const
                        { return numpartvar; }
      AQVAR*         PartVar(CARDINAL index) const;
      const FORMVAR* PartRho(CARDINAL index) const;
      const FORMVAR* PartBioturb(CARDINAL index) const;

      BOOLEAN        SurfPorosity(const char* inpline,
                                  const VARLIST* varlist,
                                  char* errorline,
                                  CARDINAL& numparseerrors);
      const FORMVAR* SurfPorosity()
                        { return SurfPor; }

      BOOLEAN        RatePorosity(const char* inpline,
                                  const VARLIST* varlist,
                                  char* errorline,
                                  CARDINAL& numparseerrors);
      const FORMVAR* RatePorosity()
                        { return RatePor; }

      CARDINAL       NumGridPts() const
                        { return numgrid; }
      BOOLEAN        NumGridPts(CARDINAL n);
      REAL           GridPt(const REAL* Y, CARDINAL index);

      CARDINAL       NumZones() const;
      const char*    ZoneName(CARDINAL index) const;

      CARDINAL       NumZoneGridPts(CARDINAL zone);
      REAL           ZoneGridPt(const REAL* Y, CARDINAL zone,
                                CARDINAL index);

      BOOLEAN        HighRes() const
                        { return highres; }
      void           HighRes(BOOLEAN high)
                        { highres = high; }

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
      REAL           RelAccQ() const
                        { return relaccQ; }
      REAL           AbsAccQ() const
                        { return absaccQ; }
      BOOLEAN        AccPor(REAL relacc, REAL absacc);
      REAL           RelAccPor() const
                        { return relaccPor; }
      REAL           AbsAccPor() const
                        { return absaccPor; }

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist, const PROCLIST* proclist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh);

   private:

      CARDINAL       ioff;

      CARDINAL       numinit;
      CARDINAL*      initzone;
      AQVAR**        initvar;
      FORMVAR**      initval;

      CARDINAL       numtopbc;
      AQVAR**        topbcvar;
      FORMVAR**      topbccorf;
      CARDINAL       numbottombc;
      AQVAR**        bottombcvar;
      FORMVAR**      bottombcconc;

      REAL           Zt;
      REAL           Zb;
      CARDINAL       numpartvar;
      AQVAR**        partvar;
      FORMVAR**      partrho;
      FORMVAR**      partbioturb;
      CARDINAL       numdissvar;
      AQVAR**        dissvar;
      FORMVAR**      dissdiff;
      REAL*          gridpts;

      BOOLEAN        highres;

      REAL           Qout;
      CARDINAL       numCout;
      REAL*          Cout;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        SetGridPts(CARDINAL n);
      void           CalcArg();

      PROGVAR*       Zvar;
      PROGVAR*       Qvar;
      PROGVAR*       epsvar;

      FORMVAR*       SurfPor;
      FORMVAR*       RatePor;

      REAL           relaccQ;
      REAL           absaccQ;
      REAL           relaccPor;
      REAL           absaccPor;

      REAL           FluxLimit(REAL s1, REAL s2, CARDINAL limiter);

      void           init();
      void           del();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
