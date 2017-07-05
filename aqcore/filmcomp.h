///////////////////////////////   filmcomp.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.04.92    Peter Reichert
// revisions:   26.10.92    Peter Reichert    redesign of links
//              23.12.92    Peter Reichert    reorganization of numerics
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "filmcomp" implements a class for biofilm reactor compartments
// ==============================================================
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
//                          +----FILMCOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// FILMCOMP:     class for biofilm reactor compartments
// ---------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class FILMCOMP
// =============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FILMCOMP_H
#define FILMCOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

class FILMCOMP : public AQCOMP
{
      friend class COMPSYS;

   public:

      FILMCOMP();
      FILMCOMP(const FILMCOMP& com);
      FILMCOMP(const AQCOMP* com);
      ~FILMCOMP();

      AQCOMPTYPE     Type() const
                        { return FilmComp; }
      const char*    TypeName() const;

      AQVAR*         SpaceVar(const VARSYS* varsys) const;
      BOOLEAN        AllowedSpaceVal(REAL z) const;

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

      BOOLEAN        AddX(AQVAR* var);
      BOOLEAN        AddS(AQVAR* var);
      BOOLEAN        DelX(CARDINAL i);
      BOOLEAN        DelX(AQVAR* var);
      BOOLEAN        DelS(CARDINAL i);
      BOOLEAN        DelS(AQVAR* var);
      BOOLEAN        Rho(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors);
      BOOLEAN        SurfKat(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors);
      BOOLEAN        SurfKde(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors);
      BOOLEAN        VolKat(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors);
      BOOLEAN        VolKde(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors);
      BOOLEAN        ResX(AQVAR* var,
                          const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors);
      BOOLEAN        PoreDiffX(AQVAR* var,
                           const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors);
      BOOLEAN        MatrixDiffX(AQVAR* var,
                           const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors);
      BOOLEAN        ResS(AQVAR* var,
                          const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors);
      BOOLEAN        PoreDiffS(AQVAR* var,
                           const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors);
      CARDINAL       NX() const
                        { return nX; }
      CARDINAL       NS() const
                        { return nS; }
      AQVAR*         Part(CARDINAL index) const;
      AQVAR*         Diss(CARDINAL index) const;
      const FORMVAR* Rho(CARDINAL index) const;
      const FORMVAR* SurfKat(CARDINAL index) const;
      const FORMVAR* SurfKde(CARDINAL index) const;
      const FORMVAR* VolKat(CARDINAL index) const;
      const FORMVAR* VolKde(CARDINAL index) const;
      const FORMVAR* ResX(CARDINAL index) const;
      const FORMVAR* PoreDiffX(CARDINAL index) const;
      const FORMVAR* MatrixDiffX(CARDINAL index) const;
      const FORMVAR* ResS(CARDINAL index) const;
      const FORMVAR* PoreDiffS(CARDINAL index) const;

      void           Confined(BOOLEAN b)
                        { confined = b; }
      BOOLEAN        Confined() const
                        { return confined; }
      BOOLEAN        ReactVol(REAL v);
      REAL           ReactVol() const
                        { return V; }
      BOOLEAN        BulkVol(REAL vb);
      REAL           BulkVol() const
                        { return VB; }
      void           PoreParticles(BOOLEAN b);
      BOOLEAN        PoreParticles() const
                        { return porepart; }
      BOOLEAN        MatrixDiff(BOOLEAN b);
      BOOLEAN        MatrixDiff() const
                        { return matrixdiff; }
      BOOLEAN        DetachGlobal(BOOLEAN b);
      BOOLEAN        DetachGlobal() const
                        { return detachglobal; }
      BOOLEAN        Surface(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors);
      BOOLEAN        Udet(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors);
      const FORMVAR* Udet() const
                        { return udet; }
      const FORMVAR* Surface()
                        { return A; }
      BOOLEAN        RateEps(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors);
      const FORMVAR* RateEps() const
                        { return reps; }

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
                        { highres = FALSE; }  // high res not yet implemented

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
	  BOOLEAN        AccZ(REAL relacc, REAL absacc);
      BOOLEAN        AccEPS(REAL relacc, REAL absacc);
      REAL           RelAccQ() const
                        { return relaccQ; }
      REAL           AbsAccQ() const
                        { return absaccQ; }
      REAL           RelAccV() const
                        { return relaccV; }
      REAL           AbsAccV() const
                        { return absaccV; }
      REAL           RelAccZ() const
                        { return relaccZ; }
      REAL           AbsAccZ() const
                        { return absaccZ; }
      REAL           RelAccEPS() const
                        { return relaccEPS; }
      REAL           AbsAccEPS() const
                        { return absaccEPS; }

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist, const PROCLIST* proclist);
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

      REAL*          gridpts;

      CARDINAL       nX;
      CARDINAL       nS;
      AQVAR**        Xvar;
      AQVAR**        Svar;
      FORMVAR**      rho;
      FORMVAR**      surfkat;
      FORMVAR**      surfkde;
      FORMVAR**      volkat;
      FORMVAR**      volkde;
      FORMVAR**      resX;
      FORMVAR**      resS;
      FORMVAR**      DXpore;
      FORMVAR**      DXmatrix;
      FORMVAR**      DSpore;

      BOOLEAN        confined;
      REAL           V;
      REAL           VB;
      BOOLEAN        porepart;
      BOOLEAN        matrixdiff;
      BOOLEAN        detachglobal;
      FORMVAR*       udet;
      FORMVAR*       A;
      FORMVAR*       reps;
      BOOLEAN        highres;

      REAL           relaccQ;
      REAL           absaccQ;
      REAL           relaccV;
      REAL           absaccV;
      REAL           relaccZ;
      REAL           absaccZ;
      REAL           relaccEPS;
      REAL           absaccEPS;

      BOOLEAN*       part;
      INTEGER*       ind;
      REAL*          Cbulk;
      CARDINAL       numCbulk;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        SetGridPts(CARDINAL n);
      void           PartDiss();
      void           CalcArg();

      void           CalcGeometry(VARSYS* varsys, const REAL* y,
                                  REAL* Area, REAL* Vol);
      void           CalcConc(CARDINAL j, BOOLEAN bfmatrix,
		                      const REAL* y, const REAL* Vol,
                              REAL* conc, REAL* eps, REAL* por);

      void           Zequilibrium(REAL LF, REAL** C, REAL* grid0);

      REAL           FluxLimit(REAL theta);

      void           init();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
