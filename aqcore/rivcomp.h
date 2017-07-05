///////////////////////////////   rivcomp.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    10.12.91    Peter Reichert    kinematic wave, advection
// revisions:   07.04.92    Peter Reichert    dispersion added
//              26.10.92    Peter Reichert    redesign of links
//              20.11.92    Peter Reichert    diffusive wave implemented
//              24.05.94    Peter Reichert    more robust initial condition,
//                                            lateral inflow
//              28.12.98    Peter Reichert    sediment layers added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "rivcomp" implements a class for river section compartments
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
//                          +----RIVCOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// RIVCOMP:      class for river compartments
// --------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class RIVCOMP
// ============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RIVCOMP_H
#define RIVCOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////


class RIVSEDLAYER : public  FILEIO
{
      friend class RIVCOMP;

   public:

      RIVSEDLAYER();
      RIVSEDLAYER(const RIVSEDLAYER& layer);
      RIVSEDLAYER(const RIVSEDLAYER* layer);
      ~RIVSEDLAYER();

      CARDINAL       NumVarArg() const
                        { return numvararg; }
      AQVAR*         VarArg(CARDINAL index) const;
      BOOLEAN        Arg(const AQVAR* var) const;
      BOOLEAN        AllowedProgVarType(PROGVARTYPE type) const;
      BOOLEAN        AllowedVar(const AQVAR* var) const;
      BOOLEAN        ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN        ExchangeVar(AQVAR* var1, AQVAR* var2);

      BOOLEAN        ZoneIndex(CARDINAL i);
      CARDINAL       ZoneIndex() const
                        { return zind; }
      BOOLEAN        Area(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors);
      const FORMVAR* Area() const
                        { return area; }
      BOOLEAN        Porosity(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors);
      const FORMVAR* Porosity() const
                        { return porosity; }
      BOOLEAN        Discharge(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* Discharge() const
                        { return discharge; }
      BOOLEAN        Dispersion(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors);
      const FORMVAR* Dispersion() const
                        { return dispersion; }
      BOOLEAN        ExchCoeff(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors);
      const FORMVAR* ExchCoeff() const
                        { return exchcoeff; }

	  JOBSTATUS      Load(std::istream& in,const VARLIST* varlist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      void           init();
      void           CalcVarArg();

      CARDINAL       zind;
      FORMVAR*       area;
      FORMVAR*       porosity;
      FORMVAR*       discharge;
      FORMVAR*       dispersion;
      FORMVAR*       exchcoeff;

      CARDINAL       numvararg;
      AQVAR**        vararg;
};


//////////////////////////////////////////////////////////////////////////////


class RIVCOMP : public AQCOMP
{
      friend class COMPSYS;
      friend JOBSTATUS SF_S0(const REAL z0[], CARDINAL ngrid, REAL delta[]);
      friend JOBSTATUS SF_gradZ(const REAL z0[], CARDINAL ngrid, REAL delta[]);

   public:

      RIVCOMP();
      RIVCOMP(const RIVCOMP& com);
      RIVCOMP(const AQCOMP* com);
      ~RIVCOMP();

      AQCOMPTYPE     Type() const
                        { return RivComp; }
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
      BOOLEAN        LatInflow(const VARLIST* varlist,
                               const char* inpline,
                               char* errorline);
      const FORMVAR* LatInflow() const
                        { return latinflow; }
      BOOLEAN        AddLatInput(const VARLIST* varlist,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceLatInput(const VARLIST* varlist,
                                     AQVAR* var,
                                     const char* inpline,
                                     char* errorline,
                                     CARDINAL& numparseerrors,
                                     CARDINAL pos);
      BOOLEAN        DeleteLatInput(CARDINAL pos);
      CARDINAL       NumLatInput() const
                        { return numlatinput; }
      AQVAR*         LatInputVar(CARDINAL index) const;
      const FORMVAR* LatInputConc(CARDINAL index) const;

      BOOLEAN        XStartEnd(REAL xs, REAL xe);
      REAL           XStart() const
                        { return Xs; }
      REAL           XEnd() const
                        { return Xe; }
      BOOLEAN        Gravit(REAL g);
      REAL           Gravit() const
                        { return gravit; }
      BOOLEAN        CrossSect(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* CrossSect()
                        { return A; }
      BOOLEAN        Peri(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors);
      const FORMVAR* Peri()
                        { return P; }
      BOOLEAN        Wi(const char* inpline,
                        const VARLIST* varlist,
                        char* errorline,
                        CARDINAL& numparseerrors);
      const FORMVAR* Wi()
                        { return W; }
      BOOLEAN        FrictionSlope(const char* inpline,
                                   const VARLIST* varlist,
                                   char* errorline,
                                   CARDINAL& numparseerrors);
      const FORMVAR* FrictionSlope()
                        { return Sf; }
      BOOLEAN        Dispersion(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors);
      const FORMVAR* Dispersion()
                        { return E; }
      BOOLEAN        WithDispersion() const
                        { return withdispersion; }
      BOOLEAN        WithDispersion(BOOLEAN b);
      BOOLEAN        EndLevel(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors);
      const FORMVAR* EndLevel()
                        { return Zend; }
      RIVCOMPLEVEL   EndLevelType() const
                        { return endleveltype; }
      BOOLEAN        EndLevelType(RIVCOMPLEVEL type);
      RIVCOMPMETH    Method() const
                        { return method; }
      BOOLEAN        Method(RIVCOMPMETH type);

      void           CalcSed(BOOLEAN b);
      BOOLEAN        CalcSed() const
                        { return calcSed; }
	  CARDINAL       NumSedLayers() const
                        { return numsedlayers; }
      RIVSEDLAYER*   SedLayer(CARDINAL index);
      BOOLEAN        AddSedLayer(RIVSEDLAYER* layer, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceSedLayer(RIVSEDLAYER* layer, CARDINAL pos);
      BOOLEAN        DeleteSedLayer(CARDINAL pos);

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
      BOOLEAN        AccA(REAL relacc, REAL absacc);
	  BOOLEAN        AccZ(REAL relacc, REAL absacc);
      BOOLEAN        AccE(REAL relacc, REAL absacc);
      REAL           RelAccQ() const
                        { return relaccQ; }
      REAL           AbsAccQ() const
                        { return absaccQ; }
      REAL           RelAccA() const
                        { return relaccA; }
      REAL           AbsAccA() const
                        { return absaccA; }
      REAL           RelAccZ() const
                        { return relaccZ; }
      REAL           AbsAccZ() const
                        { return absaccZ; }
      REAL           RelAccE() const
                        { return relaccE; }
      REAL           AbsAccE() const
                        { return absaccE; }

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
      FORMVAR*       latinflow;
      CARDINAL       numlatinput;
      AQVAR**        latinputvar;
      FORMVAR**      latinputconc;

      REAL           Xs;
      REAL           Xe;
      REAL*          gridpts;
      FORMVAR*       A;
      FORMVAR*       P;
      FORMVAR*       W;
      FORMVAR*       Sf;
      FORMVAR*       E;
      FORMVAR*       Zend;

      BOOLEAN        withdispersion;
      RIVCOMPLEVEL   endleveltype;
      RIVCOMPMETH    method;
      BOOLEAN        highres;
      REAL           gravit;

      BOOLEAN        calcSed;
      CARDINAL       numsedlayers;
      RIVSEDLAYER**  sedlayers;

      REAL           relaccQ;
      REAL           absaccQ;
      REAL           relaccA;
      REAL           absaccA;
      REAL           relaccZ;
      REAL           absaccZ;
      REAL           relaccE;
      REAL           absaccE;

      REAL           Qout;
      CARDINAL       numCout;
      REAL*          Cout;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        SetGridPts(CARDINAL n);
      void           CalcArg();

      PROGVAR*       Xvar;
      PROGVAR*       Zvar;
      PROGVAR*       Avar;
      PROGVAR*       Pvar;
      PROGVAR*       Wvar;
      PROGVAR*       Qvar;

      REAL           FluxLimit(REAL s1, REAL s2, CARDINAL limiter);

      REAL           DeltaNorm(VARSYS* varsys, REAL* y, CARDINAL j, REAL z);
      REAL           DeltaCrit(VARSYS* varsys, REAL* y, CARDINAL j, REAL z);
      REAL           DeltaDiff(VARSYS* varsys, REAL* y, CARDINAL j, REAL z);
      BOOLEAN        NormLevel(VARSYS* varsys, REAL* y, CARDINAL j, REAL* z);
      BOOLEAN        CritLevel(VARSYS* varsys, REAL* y, CARDINAL j, REAL* z);

      void           init();
      void           del();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
