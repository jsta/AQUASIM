///////////////////////////////   soilcomp.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    30.06.94    Werner Simon
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "soilcomp" implements a class for soil column compartments
// ==========================================================
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
//                          +----SOILCOMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following class is implemented in this file:
//
// SOILCOMP:     class for soil column compartments
// ---------
//
//
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class SOILCOMP
// =============================
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SOILCOMP_H
#define SOILCOMP_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////


class SERZONE : public  FILEIO
{
      friend class SOILCOMP;

   public:

      SERZONE();
      SERZONE(const SERZONE& zone);
      SERZONE(const SERZONE* zone);
      ~SERZONE();

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
      BOOLEAN        VolumeFract(const char* inpline,
                                 const VARLIST* varlist,
                                 char* errorline,
                                 CARDINAL& numparseerrors);
      const FORMVAR* VolumeFract() const
                        { return volfract; }
      const CARDINAL NumExch() const
                        { return numexch; }
      const AQVAR*   ExchVar(CARDINAL index) const;
      const FORMVAR* ExchVal(CARDINAL index) const;
      const FORMVAR* ExchFac(CARDINAL index) const;
      BOOLEAN        AddExchCoeff(const VARLIST* varlist,
                                  AQVAR* var,
	                          const char* inpval,
	                          char* errorline,
	                          CARDINAL& numparseerrors,
	                          const char* inpfac,
	                          char* errorlinefac,
	                          CARDINAL& parseerrorsfac,
	                          CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceExchCoeff(const VARLIST* varlist,
                                      AQVAR* var,
                                      const char* inpval,
                                      char* errorline,
                                      CARDINAL& numparseerrors,
                                      const char* inpfac,
                                      char* errorlinefac,
                                      CARDINAL& parseerrorsfac,
                                      CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        DeleteExchCoeff(CARDINAL pos);

	  JOBSTATUS      Load(std::istream& in,const VARLIST* varlist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      void           init();
      void           CalcVarArg();

      CARDINAL       zind;
      FORMVAR*       volfract;
      CARDINAL       numexch;
      AQVAR**        exchvar;
      FORMVAR**      exchval;
      FORMVAR**      exchfac;

      CARDINAL       numvararg;
      AQVAR**        vararg;
};

//////////////////////////////////////////////////////////////////////////////


class PARZONE : public SYMBOL
{
      friend class SOILCOMP;

   public:
      PARZONE();
      PARZONE(const PARZONE& parallel);
      PARZONE(const PARZONE* parallel);
      ~PARZONE();

      PARZONE*           Next() const
                        { return (PARZONE*)SYMBOL::Next(); }
      PARZONE*           Prev() const
                        { return (PARZONE*)SYMBOL::Prev(); }

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

      CARDINAL       NumSerialZones() const
                        { return numserialzones; }
      SERZONE*       SerialZone(CARDINAL index);
      BOOLEAN        AddSerial(SERZONE* zone, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceSerial(SERZONE* zone, CARDINAL pos);
      BOOLEAN        DeleteSerial(CARDINAL pos);

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist);
	  JOBSTATUS      Save(std::ostream& out);
	  JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

 private:

      CARDINAL       numserialzones;
      SERZONE**      zones;

      CARDINAL       numvararg;
      AQVAR**        vararg;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);

      void           init();
      void           CalcVarArg();
};



//////////////////////////////////////////////////////////////////////////////
//
//  parallel Zones     +------------------+----+       +-----+
//                     |                  |    |       |     |                 
//                     |                  +----+       +-----+
//                     |                  |    |
//                     |                  |    |       +-----+
//                     |                  |    |       |     |
//                     |                  |    |       +-----+
//                     |                  |    |    
//                     |                  +----+       +-----+
//                     |                  |    |       |     |
//                     +------------------+----+       +-----+
//
//////////////////////////////////////////////////////////////////////////////

class ZONELIST : private SYMBOLLIST
{
//---------------------------------29.08.94------------------------------------
      friend   class SOILCOMP;    // ----> noch ueberpruefen ! , 06.09.1994
//-----------------------------------------------------------------------------

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const PARZONE* parallel) const
                  { return SYMBOLLIST::Exist(parallel); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      PARZONE* First() const
                  { return (PARZONE*)SYMBOLLIST::First(); }
      PARZONE* Last() const
                  { return (PARZONE*)SYMBOLLIST::Last(); }
      PARZONE* Current() const
                  { return (PARZONE*)SYMBOLLIST::Current(); }
      PARZONE* Next() const
                  { return (PARZONE*)SYMBOLLIST::Next(); }
      PARZONE* Prev() const
                  { return (PARZONE*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(PARZONE* parallel) const
                  { return SYMBOLLIST::Position(parallel); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      PARZONE* Get(CARDINAL pos) const
                  { return (PARZONE*)SYMBOLLIST::Get(pos); }
      PARZONE* Get(const char* sym) const
                  { return (PARZONE*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(PARZONE* parallel)
                  { return SYMBOLLIST::Goto(parallel); }
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

      BOOLEAN  Add(PARZONE* parallel)
                  { return SYMBOLLIST::InsertSorted(parallel); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(PARZONE* parallel)
                  { return SYMBOLLIST::Remove(parallel); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};



//-----------------------------------------------------------------------------




class SOILCOMP : public AQCOMP
{
      friend class COMPSYS;
      friend JOBSTATUS SF_S0(const REAL z0[], CARDINAL ngrid, REAL delta[]);
      friend JOBSTATUS SF_gradZ(const REAL z0[], CARDINAL ngrid, REAL delta[]);

   public:

      SOILCOMP();
      SOILCOMP(const SOILCOMP& com);
      SOILCOMP(const AQCOMP* com);
      ~SOILCOMP();

      AQCOMPTYPE     Type() const
                        { return SoilComp; }
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

      BOOLEAN        CrossSect(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* CrossSect()
                        { return A; }
      BOOLEAN        AdvVolFract(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* AdvVolFract()
                        { return advvolfract; } 
      BOOLEAN        Dispersion(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors);
      const FORMVAR* Dispersion()
                        { return E; }
      BOOLEAN        WithDispersion() const
                        { return withdispersion; }
      BOOLEAN        WithDispersion(BOOLEAN b);

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

      BOOLEAN        ExistParZone(const PARZONE* parallel) const
                        { return parzonelist.Exist(parallel); }
      BOOLEAN        ExistParZone(const char* s) const
                        { return ExistParZone(GetParZone(s)); }

      PARZONE*       GetParZone(const char* s) const
                        { return parzonelist.Get(s); }

      BOOLEAN        AddParZone(PARZONE* parallel);
      BOOLEAN        ReplaceParZone(PARZONE* oldparallel, PARZONE* newparallel);
      BOOLEAN        DeleteParZone(PARZONE* parallel);
      BOOLEAN        DeleteParZone(const char* s)
                        { return DeleteParZone(GetParZone(s)); }

      // temporary for compatibility with old files:
      BOOLEAN        AddParZone_compat(PARZONE* parallel);

      ZONELIST*      ParZoneList()
                        { return &parzonelist; }

      BOOLEAN        AccQ(REAL relacc, REAL absacc);
      BOOLEAN        AccD(REAL relacc, REAL absacc);
      REAL           RelAccQ() const
                        { return relaccQ; }
      REAL           AbsAccQ() const
                        { return absaccQ; }
      REAL           RelAccD() const
                        { return relaccD; }
      REAL           AbsAccD() const
                        { return absaccD; }

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist,const PROCLIST*);
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

      static CARDINAL ioff;

      REAL           Xs;
      REAL           Xe;
      REAL*          gridpts;
      FORMVAR*       A;
      FORMVAR*       advvolfract;

      FORMVAR*       E;

      BOOLEAN        withdispersion;
      BOOLEAN        highres;
      REAL           g;

      REAL           Qout;
      CARDINAL       numCout;
      REAL*          Cout;

      REAL           relaccQ;
      REAL           absaccQ;
      REAL           relaccD;
      REAL           absaccD;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        SetGridPts(CARDINAL n);
      void           CalcArg();

      PROGVAR*       Xvar;
      PROGVAR*       Avar;
      PROGVAR*       Qvar;

      CARDINAL       numparzones;
      CARDINAL*      numserzones;
      CARDINAL       numzones;

      void Num_of_Zones();
      REAL           FluxLimit(REAL s1, REAL s2, CARDINAL limiter);

      void           init();
      void           del();

      ZONELIST       parzonelist;

      REAL           endcrosssect;
      REAL           preendcrosssect;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
