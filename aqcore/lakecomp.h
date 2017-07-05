///////////////////////////////   lakecomp.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    01.03.95    Peter Reichert
// revisions:   13.06.96    Peter Reichert    sediment added, TKE revised,
//                                            inflow added
//              15.11.96    Peter Reichert    pressure gradient added
//              08.01.98    Peter Reichert    epsilon equation added
//              21.05.98    Peter Reichert    sediment layers added
//              08.02.99    Peter Reichert    light intensity added
//              17.04.03    Peter Reichert    particle mobility added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "lakecomp" implements a class for lake compartments
// ===================================================
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
//                          +----LAKECOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// LAKECOMP:     class for lake compartments
// ---------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class LAKECOMP
// =============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LAKECOMP_H
#define LAKECOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////


class LAKESEDLAYER : public  FILEIO
{
      friend class LAKECOMP;

   public:

      LAKESEDLAYER();
      LAKESEDLAYER(const LAKESEDLAYER& layer);
      LAKESEDLAYER(const LAKESEDLAYER* layer);
      ~LAKESEDLAYER();

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
      BOOLEAN        Thickness(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* Thickness() const
                        { return thickness; }
      BOOLEAN        Porosity(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors);
      const FORMVAR* Porosity() const
                        { return porosity; }

	  JOBSTATUS      Load(std::istream& in,const VARLIST* varlist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      void           init();
      void           CalcVarArg();

      CARDINAL       zind;
      FORMVAR*       thickness;
      FORMVAR*       porosity;

      CARDINAL       numvararg;
      AQVAR**        vararg;
};

//////////////////////////////////////////////////////////////////////////////

class PINF : public SYMBOL
{
      friend class LAKECOMP;

   public:

      PINF();
      PINF(const PINF& pinf);
      PINF(const PINF* pinf);
      ~PINF();

      PINF*          Next() const
                        { return (PINF*)SYMBOL::Next(); }
      PINF*          Prev() const
                        { return (PINF*)SYMBOL::Prev(); }

      BOOLEAN        AllowedProgVarType(PROGVARTYPE type) const;
      BOOLEAN        AllowedVar(const AQVAR* var) const;
      BOOLEAN        AllowedReplaceVar(const AQVAR* oldvar,
                                       const AQVAR* newvar) const;
      BOOLEAN        AllowedExchangeVar(const AQVAR* var1,
                                        const AQVAR* var2) const;

      CARDINAL       NumVarArg() const
                        { return numvararg; }
      AQVAR*         VarArg(CARDINAL index) const;
      BOOLEAN        Arg(const AQVAR* var) const;

      BOOLEAN        ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN        ExchangeVar(AQVAR* var1, AQVAR* var2);

      BOOLEAN        Depth(const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors);
      FORMVAR*       Depth() const
                        { return depth; }
      BOOLEAN        Width(const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors);
      FORMVAR*       Width() const
                        { return width; }
      BOOLEAN        WaterFlow(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      FORMVAR*       WaterFlow() const
                        { return waterflow; }

      BOOLEAN        AddConc(const VARLIST* varlist,
                             AQVAR* var,
                             const char* inpline,
                             char* errorline,
                             CARDINAL& numparseerrors,
                             CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceConc(const VARLIST* varlist,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos);
      BOOLEAN        DeleteConc(CARDINAL pos);
      CARDINAL       NumConc() const
                        { return numconc; }
      AQVAR*         ConcVar(CARDINAL index) const;
      FORMVAR*       ConcVal(CARDINAL index) const;

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      FORMVAR*       depth;
      FORMVAR*       width;
      FORMVAR*       waterflow;
      CARDINAL       numconc;
      AQVAR**        concvar;
      FORMVAR**      concval;

      CARDINAL       numvararg;
      AQVAR**        vararg;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);

      void           init();
      void           CalcVarArg();
};

//////////////////////////////////////////////////////////////////////////////

class PINFLIST : private SYMBOLLIST
{
      friend   class LAKECOMP;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const PINF* pinf) const
                  { return SYMBOLLIST::Exist(pinf); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      PINF*    First() const
                  { return (PINF*)SYMBOLLIST::First(); }
      PINF*    Last() const
                  { return (PINF*)SYMBOLLIST::Last(); }
      PINF*    Current() const
                  { return (PINF*)SYMBOLLIST::Current(); }
      PINF*    Next() const
                  { return (PINF*)SYMBOLLIST::Next(); }
      PINF*    Prev() const
                  { return (PINF*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(PINF* pinf) const
                  { return SYMBOLLIST::Position(pinf); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      PINF*    Get(CARDINAL pos) const
                  { return (PINF*)SYMBOLLIST::Get(pos); }
      PINF*    Get(const char* sym) const
                  { return (PINF*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(PINF* pinf)
                  { return SYMBOLLIST::Goto(pinf); }
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

      BOOLEAN  Add(PINF* pinf)
                  { return SYMBOLLIST::InsertSorted(pinf); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(PINF* pinf)
                  { return SYMBOLLIST::Remove(pinf); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

class LAKECOMP : public AQCOMP
{
      friend class COMPSYS;

   public:

      LAKECOMP();
      LAKECOMP(const LAKECOMP& com);
      LAKECOMP(const AQCOMP* com);
      ~LAKECOMP();

      AQCOMPTYPE     Type() const
                        { return LakeComp; }
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

	  BOOLEAN        CalcAreas(VARSYS* varsys,
                               REAL* areas, 
                               REAL* sedareas,
                               REAL* sedareasproj); 

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

      BOOLEAN        AddSurfInput(const VARLIST* varlist,
                                  AQVAR* var,
                                  const char* inpline,
                                  char* errorline,
                                  CARDINAL& numparseerrors,
                                  CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceSurfInput(const VARLIST* varlist,
                                      AQVAR* var,
                                      const char* inpline,
                                      char* errorline,
                                      CARDINAL& numparseerrors,
                                      CARDINAL pos);
      BOOLEAN        DeleteSurfInput(CARDINAL pos);
      CARDINAL       NumSurfInput() const
                        { return numsurfinput; }
      AQVAR*         SurfInputVar(CARDINAL index) const;
      const FORMVAR* SurfInputFlux(CARDINAL index) const;
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

      BOOLEAN        ExistPointInf(const PINF* pinf) const
                        { return pinflist.Exist(pinf); }
      BOOLEAN        ExistPointInf(const char* s) const
                        { return ExistPointInf(GetPointInf(s)); }
      PINF*          GetPointInf(const char* s) const
                        { return pinflist.Get(s); }
      BOOLEAN        AddPointInf(PINF* pinf);
      BOOLEAN        ReplacePointInf(PINF* oldpinf, PINF* newpinf);
      BOOLEAN        DeletePointInf(PINF* pinf);
      BOOLEAN        DeletePointInf(const char* s)
                        { return DeletePointInf(GetPointInf(s)); }
      void           DeletePointInf();
      PINFLIST*      PointInflist()
                        { return &pinflist; }

      BOOLEAN        AddSedInput(const VARLIST* varlist,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceSedInput(const VARLIST* varlist,
                                     AQVAR* var,
                                     const char* inpline,
                                     char* errorline,
                                     CARDINAL& numparseerrors,
                                     CARDINAL pos);
      BOOLEAN        DeleteSedInput(CARDINAL pos);
      CARDINAL       NumSedInput() const
                        { return numsedinput; }
      AQVAR*         SedInputVar(CARDINAL index) const;
      const FORMVAR* SedInputFlux(CARDINAL index) const;

      BOOLEAN        ZTopBottom(REAL zt, REAL zb);
      REAL           ZTop() const
                        { return Zt; }
      REAL           ZBottom() const
                        { return Zb; }
      BOOLEAN        Gravit(REAL g);
      REAL           Gravit() const
                        { return gravit; }
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
      const FORMVAR* Disskdiff(CARDINAL index) const;
      BOOLEAN        AddPartVar(const VARLIST* varlist,
                                AQVAR* var,
                                const char* inprho,
                                char* errorlinerho,
                                CARDINAL& parseerrorsrho,
                                const char* inpsedvel,
                                char* errorlinesedvel,
                                CARDINAL& parseerrorssedvel,
                                const char* inpmobility,
                                char* errorlinemobility,
                                CARDINAL& parseerrorsmobility,
                                CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplacePartVar(const VARLIST* varlist,
                                    AQVAR* var,
                                    const char* inprho,
                                    char* errorlinerho,
                                    CARDINAL& parseerrorsrho,
                                    const char* inpsedvel,
                                    char* errorlinesedvel,
                                    CARDINAL& parseerrorssedvel,
                                    const char* inpmobility,
                                    char* errorlinemobility,
                                    CARDINAL& parseerrorsmobility,
                                    CARDINAL pos);
      BOOLEAN        DeletePartVar(CARDINAL pos);
      CARDINAL       NumPartVar() const
                        { return numpartvar; }
      AQVAR*         PartVar(CARDINAL index) const;
      const FORMVAR* PartRho(CARDINAL index) const;
      const FORMVAR* PartSedVeloc(CARDINAL index) const;
      const FORMVAR* PartMobility(CARDINAL index) const;
      BOOLEAN        CrossSect(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* CrossSect()
                        { return A; }
      BOOLEAN        Density(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors);
      const FORMVAR* Density()
                        { return Rho; }
      BOOLEAN        Diffusion(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* Diffusion()
                        { return D; }
      BOOLEAN        LightSurface(const char* inpline,
                                  const VARLIST* varlist,
                                  char* errorline,
                                  CARDINAL& numparseerrors);
      const FORMVAR* LightSurface()
                        { return Isurf; }
      BOOLEAN        LightExtinct(const char* inpline,
                                  const VARLIST* varlist,
                                  char* errorline,
                                  CARDINAL& numparseerrors);
      const FORMVAR* LightExtinct()
                        { return Eta; }
      void           CalcSed(BOOLEAN b);
      BOOLEAN        CalcSed() const
                        { return calcSed; }
      BOOLEAN        SedFluxVol(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors);
      const FORMVAR* SedFluxVol()
                        { return Fvol;}
      BOOLEAN        SedResusp(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* SedResusp()
                        { return kres; }
      CARDINAL       NumSedLayers() const
                        { return numsedlayers; }
      LAKESEDLAYER*  SedLayer(CARDINAL index);
      BOOLEAN        AddSedLayer(LAKESEDLAYER* layer, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceSedLayer(LAKESEDLAYER* layer, CARDINAL pos);
      BOOLEAN        DeleteSedLayer(CARDINAL pos);
      void           CalcTKE(BOOLEAN b);
      BOOLEAN        CalcTKE() const
                        { return calcTKE; }
      BOOLEAN        Prandtl(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors);
      const FORMVAR* Prandtl()
                        { return Pr; }
      BOOLEAN        SigmaTKE(REAL s);
      REAL           SigmaTKE() const
                        { return sigmaTKE; }
      BOOLEAN        SigmaEps(REAL s);
      REAL           SigmaEps() const
                        { return sigmaEps; }
      BOOLEAN        KzMin(REAL r);
      REAL           KzMin() const
                        { return kzmin; }
      BOOLEAN        SurfaceShear(const char* inpline,
                                  const VARLIST* varlist,
                                  char* errorline,
                                  CARDINAL& numparseerrors);
      const FORMVAR* SurfaceShear()
                        { return TauSurf; }
      BOOLEAN        PressureGradient(const char* inpline,
                                      const VARLIST* varlist,
                                      char* errorline,
                                      CARDINAL& numparseerrors);
      const FORMVAR* PressureGradient()
                        { return PressGrad; }
      BOOLEAN        ProdDiss(const char* inpline,
                                 const VARLIST* varlist,
                                 char* errorline,
                                 CARDINAL& numparseerrors);
      const FORMVAR* ProdDiss()
                        { return ProdDissip; }
      BOOLEAN        SeichePwind(const char* inpline,
                                 const VARLIST* varlist,
                                 char* errorline,
                                 CARDINAL& numparseerrors);
      const FORMVAR* SeichePwind()
                        { return Pwind; }
      BOOLEAN        SeichePbottom(const char* inpline,
                                   const VARLIST* varlist,
                                   char* errorline,
                                   CARDINAL& numparseerrors);
      const FORMVAR* SeichePbottom()
                        { return Pbottom; }
      BOOLEAN        SeichePint(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors);
      const FORMVAR* SeichePint()
                        { return Pint; }

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
      BOOLEAN        AccI(REAL relacc, REAL absacc);
      BOOLEAN        AccU(REAL relacc, REAL absacc);
      BOOLEAN        AccTKE(REAL relacc, REAL absacc);
      BOOLEAN        AccEPS(REAL relacc, REAL absacc);
      BOOLEAN        AccEseiche(REAL relacc, REAL absacc);
      REAL           RelAccQ() const
                        { return relaccQ; }
      REAL           AbsAccQ() const
                        { return absaccQ; }
      REAL           RelAccI() const
                        { return relaccI; }
      REAL           AbsAccI() const
                        { return absaccI; }
      REAL           RelAccU() const
                        { return relaccU; }
      REAL           AbsAccU() const
                        { return absaccU; }
      REAL           RelAccTKE() const
                        { return relaccTKE; }
      REAL           AbsAccTKE() const
                        { return absaccTKE; }
      REAL           RelAccEPS() const
                        { return relaccEPS; }
      REAL           AbsAccEPS() const
                        { return absaccEPS; }
      REAL           RelAccEseiche() const
                        { return relaccEseiche; }
      REAL           AbsAccEseiche() const
                        { return absaccEseiche; }

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist, const PROCLIST* proclist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh);

   private:

      CARDINAL       numinit;
      CARDINAL*      initzone;
      AQVAR**        initvar;
      FORMVAR**      initval;

      CARDINAL       numsurfinput;
      AQVAR**        surfinputvar;
      FORMVAR**      surfinputflux;
      FORMVAR*       latinflow;
      CARDINAL       numlatinput;
      AQVAR**        latinputvar;
      FORMVAR**      latinputconc;
      PINFLIST       pinflist;
      CARDINAL       numsedinput;
      AQVAR**        sedinputvar;
      FORMVAR**      sedinputflux;

      CARDINAL       ioff;
      CARDINAL       ised;

      REAL           Zt;
      REAL           Zb;
      CARDINAL       numpartvar;
      AQVAR**        partvar;
      FORMVAR**      partrho;
      FORMVAR**      partsedveloc;
      FORMVAR**      partmobility;
      CARDINAL       numdissvar;
      AQVAR**        dissvar;
      FORMVAR**      disskdiff;
      REAL*          gridpts;
      FORMVAR*       A;
      FORMVAR*       Rho;
      FORMVAR*       D;
      FORMVAR*       Isurf;
      FORMVAR*       Eta;
      BOOLEAN        calcSed;
      FORMVAR*       Fvol;
      FORMVAR*       kres;
      CARDINAL       numsedlayers;
      LAKESEDLAYER** sedlayers;
      BOOLEAN        calcTKE;
      FORMVAR*       Pr;
      REAL           sigmaTKE;
      REAL           sigmaEps;
      FORMVAR*       TauSurf;
	  FORMVAR*       PressGrad;
      FORMVAR*       ProdDissip;
      FORMVAR*       Pwind;
      FORMVAR*       Pbottom;
      FORMVAR*       Pint;

      BOOLEAN        highres;
      REAL           gravit;

      REAL           Qout;
      CARDINAL       numCout;
      REAL*          Cout;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        SetGridPts(CARDINAL n);
      void           CalcArg();

      PROGVAR*       Zvar;
      PROGVAR*       Avar;
      PROGVAR*       Qvar;
      PROGVAR*       Rhovar;
      PROGVAR*       dAdzvar;
      PROGVAR*       Ivar;
      PROGVAR*       N2var;
      PROGVAR*       UHvar;
      PROGVAR*       TKEvar;
      PROGVAR*       ShearPvar;
      PROGVAR*       BuoyancyPvar;
      PROGVAR*       EPSvar;
      PROGVAR*       Eseichevar;
      REAL           kzmin;

      REAL           relaccQ;
      REAL           absaccQ;
      REAL           relaccI;
      REAL           absaccI;
      REAL           relaccU;
      REAL           absaccU;
      REAL           relaccTKE;
      REAL           absaccTKE;
      REAL           relaccEPS;
      REAL           absaccEPS;
      REAL           relaccEseiche;
      REAL           absaccEseiche;

      REAL           FluxLimit(REAL s1, REAL s2, CARDINAL limiter);

      void           init();
      void           del();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
