////////////////////////////////   aqlink.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    15.10.92    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "aqlink" implements classes for connections between compartments
// ================================================================
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
//   FILEIO
//     |
//     +------+
//            |
//   NODE     |
//     |      |
//     +------+----SYMBOL
//                   |
//                   +----AQLINK
//                          |
//                          +----ADVLINK
//                          |
//                          +----DIFFLINK
//
//   LIST
//     |
//     +----SYMBOLLIST
//            |
//            +----LINKLIST
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// The following classes are implemented in this file:
//
// BIF:          class for bifurcations
// ----
//
//
//
// BIFLIST:
// --------
//
//
//
//
// AQLINK:       base class for links
// -------
//
//
//
// ADVLINK:      class for advective links
// --------
//
//
//
// DIFFLINK:     class for diffusive links
// ---------
//
//
// LINKLIST:
// ---------
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class AQLINK
// ===========================
//
//
//
// Methods of the class ADVLINK
// ============================
//
//
//
// Methods of the class DIFFLINK
// =============================
//
//
//
// Methods of the class LINKLIST
// =============================
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AQLINK_H
#define AQLINK_H

#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

enum AQLINKTYPE {
                   AdvLink,
                   DiffLink
                };

enum BIFTYPE    {
                   WithWater,
                   AsGiven
                };

extern REAL linkindex;

//////////////////////////////////////////////////////////////////////////////

class BIF : public SYMBOL
{
      friend class ADVLINK;

   public:

      BIF();
      BIF(const BIF& bif);
      BIF(const BIF* bif);
      ~BIF();

      BIF*           Next() const
                        { return (BIF*)SYMBOL::Next(); }
      BIF*           Prev() const
                        { return (BIF*)SYMBOL::Prev(); }

      BOOLEAN        AllowedProgVarType(PROGVARTYPE type) const;
      BOOLEAN        AllowedVar(const AQVAR* var) const;
      BOOLEAN        AllowedReplaceVar(const AQVAR* oldvar,
                                       const AQVAR* newvar) const;
      BOOLEAN        AllowedExchangeVar(const AQVAR* var1,
                                        const AQVAR* var2) const;
      BOOLEAN        AllowedReplaceComp(const AQCOMP* oldcomp,
                                        const AQCOMP* newcomp) const;

      CARDINAL       NumVarArg() const
                        { return numvararg; }
      AQVAR*         VarArg(CARDINAL index) const;
      BOOLEAN        Arg(const AQVAR* var) const;

      BOOLEAN        ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN        ExchangeVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN        ReplaceComp(AQCOMP* oldcom, AQCOMP* newcom);

      BOOLEAN        SetBifComp(AQCOMP* com, CARDINAL conn);
      AQCOMP*        GetBifComp() const
                        { return compartment; }
      CARDINAL       GetBifConn() const
                        { return connection; }

      BOOLEAN        WaterFlow(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors);
      const FORMVAR* WaterFlow() const
                        { return waterflow; }

      void           BifType(BIFTYPE type)
                        { biftype = type; }
      BIFTYPE        BifType() const
                        { return biftype; }

      BOOLEAN        AddFlux(const VARLIST* varlist,
                             AQVAR* var,
                             const char* inpline,
                             char* errorline,
                             CARDINAL& numparseerrors,
                             CARDINAL pos=CARDINAL_MAX);
      BOOLEAN        ReplaceFlux(const VARLIST* varlist,
                                 AQVAR* var,
                                 const char* inpline,
                                 char* errorline,
                                 CARDINAL& numparseerrors,
                                 CARDINAL pos);
      BOOLEAN        DeleteFlux(CARDINAL pos);
      CARDINAL       NumFlux() const
                        { return numflux; }
      AQVAR*         FluxVar(CARDINAL index) const;
      const FORMVAR* FluxVal(CARDINAL index) const;

      REAL           AdvOutQ();
      REAL           AdvOutJ(REAL Q0, REAL J0, const AQVAR* var);

	  JOBSTATUS      Load(std::istream& in,
                          const VARLIST* varlist,
                          const COMPLIST* complist);
      JOBSTATUS      Save(std::ostream& out);
      JOBSTATUS      Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      AQCOMP*        compartment;
      CARDINAL       connection;
      FORMVAR*       waterflow;
      BIFTYPE        biftype;
      CARDINAL       numflux;
      AQVAR**        fluxvar;
      FORMVAR**      fluxval;

      CARDINAL       numvararg;
      AQVAR**        vararg;

      void           ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void           ExchVar(AQVAR* var1, AQVAR* var2);
      void           ReplComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      void           init();
      void           CalcVarArg();
};

//////////////////////////////////////////////////////////////////////////////

class BIFLIST : private SYMBOLLIST
{
      friend   class ADVLINK;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const BIF* bif) const
                  { return SYMBOLLIST::Exist(bif); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      BIF*     First() const
                  { return (BIF*)SYMBOLLIST::First(); }
      BIF*     Last() const
                  { return (BIF*)SYMBOLLIST::Last(); }
      BIF*     Current() const
                  { return (BIF*)SYMBOLLIST::Current(); }
      BIF*     Next() const
                  { return (BIF*)SYMBOLLIST::Next(); }
      BIF*     Prev() const
                  { return (BIF*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const BIF* bif) const
                  { return SYMBOLLIST::Position(bif); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      BIF*     Get(CARDINAL pos) const
                  { return (BIF*)SYMBOLLIST::Get(pos); }
      BIF*     Get(const char* sym) const
                  { return (BIF*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(BIF* bif)
                  { return SYMBOLLIST::Goto(bif); }
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

      BOOLEAN  Add(BIF* bif)
                  { return SYMBOLLIST::InsertSorted(bif); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(BIF* bif)
                  { return SYMBOLLIST::Remove(bif); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

class AQLINK : public SYMBOL
{
      friend class LINKSYS;

   public:

      AQLINK();
      AQLINK(const AQLINK& link);
      AQLINK(const AQLINK* link);
      ~AQLINK();

      AQLINK*             Next() const
                             { return (AQLINK*)SYMBOL::Next(); }
      AQLINK*             Prev() const
                             { return (AQLINK*)SYMBOL::Prev(); }

      virtual AQLINKTYPE  Type() const = 0;
      virtual const char* TypeName() const = 0;

      virtual BOOLEAN     AllowedProgVarType(PROGVARTYPE type) const = 0;
      BOOLEAN             AllowedVar(const AQVAR* var) const;
      virtual BOOLEAN     AllowedReplaceVar(const AQVAR* oldvar,
                                            const AQVAR* newvar) const;
      virtual BOOLEAN     AllowedExchangeVar(const AQVAR* var1,
                                             const AQVAR* var2) const;
      virtual BOOLEAN     AllowedReplaceComp(const AQCOMP* oldcomp,
                                             const AQCOMP* newcomp) const;

      CARDINAL            NumVarArg() const
                             { return numvararg; }
      AQVAR*              VarArg(CARDINAL index) const;
      BOOLEAN             Arg(const AQVAR* var) const;
      CARDINAL            NumCompArg() const
                             { return numcomparg; }
      AQCOMP*             CompArg(CARDINAL index) const;
      BOOLEAN             Arg(const AQCOMP* com) const;

      virtual BOOLEAN     ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      virtual BOOLEAN     ExchangeVar(AQVAR* var1, AQVAR* var2);
      BOOLEAN             ReplaceComp(AQCOMP* oldcom, AQCOMP* newcom);

      void                LnkIndex(CARDINAL i)
                             { lind = i; }
      CARDINAL            LnkIndex() const
                             { return lind; }
      JOBSTATUS           Load(std::istream& in, CARDINAL linkvers);
      JOBSTATUS           Save(std::ostream& out);
      JOBSTATUS           Write(std::ostream& out, BOOLEAN sh=FALSE);

      static const INIT*  Ini()
                             { return &ini; }

   protected:

      static CARDINAL     number;

      CARDINAL            numvararg;
      AQVAR**             vararg;

      CARDINAL            numcomparg;
      AQCOMP**            comparg;

      CARDINAL            lind;

      virtual void        ReplVar(AQVAR* oldvar, AQVAR* newvar);
      virtual void        ExchVar(AQVAR* var1, AQVAR* var2);
      virtual void        ReplComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      void                init();
      virtual void        CalcVarArg();
      virtual void        CalcCompArg();

      static INIT         ini;
};

//////////////////////////////////////////////////////////////////////////////

class ADVLINK : public AQLINK
{
      friend class LINKSYS;

   public:

      ADVLINK();
      ADVLINK(const ADVLINK& link);
      ADVLINK(const AQLINK* link);
      ~ADVLINK();

      AQLINKTYPE  Type() const
                     { return AdvLink; }
      const char* TypeName() const;

      BOOLEAN     AllowedProgVarType(PROGVARTYPE type) const;

      BOOLEAN     AdvInExZ() const;
      REAL        AdvInZ(const REAL* Y) const;
      REAL        AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL index);
      REAL        AdvOutJ(VARSYS* varsys,
                          const REAL* Y,
                          CARDINAL index,
                          const AQVAR* var);

      BOOLEAN     SetCompIn(AQCOMP* com, CARDINAL conn);
      AQCOMP*     GetCompIn() const
                     { return compin; }
      CARDINAL    GetConnIn() const
                     { return connin; }
      BOOLEAN     SetCompOut(AQCOMP* com, CARDINAL conn);
      AQCOMP*     GetCompOut() const
                     { return compout; }
      CARDINAL    GetConnOut() const
                     { return connout; }

      BOOLEAN     ExistBif(const BIF* bif) const
                     { return biflist.Exist(bif); }
      BOOLEAN     ExistBif(const char* s) const
                     { return ExistBif(GetBif(s)); }

      BIF*        GetBif(const char* s) const
                     { return biflist.Get(s); }

      BOOLEAN     AddBif(BIF* bif);
      BOOLEAN     ReplaceBif(BIF* oldbif, BIF* newbif);
      BOOLEAN     DeleteBif(BIF* bif);
      BOOLEAN     DeleteBif(const char* s)
                     { return DeleteBif(GetBif(s)); }
      void        DeleteBif();

      BIFLIST*    Biflist()
                     { return &biflist; }

      BOOLEAN     ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN     ExchangeVar(AQVAR* var1, AQVAR* var2);

      JOBSTATUS   Load(std::istream& in,
                       const VARLIST* varlist,
                       const COMPLIST* complist);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      AQCOMP*     compin;
      CARDINAL    connin;
      AQCOMP*     compout;
      CARDINAL    connout;

      BIFLIST     biflist;

      void        ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void        ExchVar(AQVAR* var1, AQVAR* var2);
      void        ReplComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      void        init();
      void        CalcVarArg();
      void        CalcCompArg();
};

//////////////////////////////////////////////////////////////////////////////

class DIFFLINK : public AQLINK
{
      friend class LINKSYS;

   public:

      DIFFLINK();
      DIFFLINK(const DIFFLINK& link);
      DIFFLINK(const AQLINK* link);
      ~DIFFLINK();

      AQLINKTYPE      Type() const
                         { return DiffLink; }
      const char*     TypeName() const;

      BOOLEAN         AllowedProgVarType(PROGVARTYPE type) const;

      BOOLEAN         SetComp1(AQCOMP* com, CARDINAL conn);
      AQCOMP*         GetComp1() const
                         { return comp1; }
      CARDINAL        GetConn1() const
                         { return conn1; }
      BOOLEAN         SetComp2(AQCOMP* com, CARDINAL conn);
      AQCOMP*         GetComp2() const
                         { return comp2; }
      CARDINAL        GetConn2() const
                         { return conn2; }

      BOOLEAN         AddExchCoeff(const VARLIST* varlist,
                                   AQVAR* var,
                                   const char* inpval,
                                   char* errorlineval,
                                   CARDINAL& parseerrorsval,
                                   const char* inpfac,
                                   char* errorlinefac,
                                   CARDINAL& parseerrorsfac,
                                   CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         ReplaceExchCoeff(const VARLIST* varlist,
                                       AQVAR* var,
                                       const char* inpval,
                                       char* errorlineval,
                                       CARDINAL& parseerrorsval,
                                       const char* inpfac,
                                       char* errorlinefac,
                                       CARDINAL& parseerrorsfac,
                                       CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         DeleteExchCoeff(CARDINAL pos);
      CARDINAL        NumExch() const
                         { return numexch; }
      AQVAR*          ExchVar(CARDINAL index) const;
      const FORMVAR*  ExchVal(CARDINAL index) const;
      const FORMVAR*  ExchFac(CARDINAL index) const;

      BOOLEAN         ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      BOOLEAN         ExchangeVar(AQVAR* var1, AQVAR* var2);

      REAL            DiffJ(const REAL* Y, CARDINAL index, const AQVAR* var);

      JOBSTATUS       Load(std::istream& in,
                           const VARLIST* varlist,
                           const COMPLIST* complist);
      JOBSTATUS       Save(std::ostream& out);
      JOBSTATUS       Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      AQCOMP*         comp1;
      CARDINAL        conn1;
      AQCOMP*         comp2;
      CARDINAL        conn2;

      CARDINAL        numexch;
      AQVAR**         exchvar;
      FORMVAR**       exchval;
      FORMVAR**       exchfac;

      void            ReplVar(AQVAR* oldvar, AQVAR* newvar);
      void            ExchVar(AQVAR* var1, AQVAR* var2);
      void            ReplComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      void            init();
      void            CalcVarArg();
      void            CalcCompArg();
};

//////////////////////////////////////////////////////////////////////////////

class LINKLIST : private SYMBOLLIST
{
      friend class LINKSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const AQLINK* link) const
                  { return SYMBOLLIST::Exist(link); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      AQLINK*  First() const
                  { return (AQLINK*)SYMBOLLIST::First(); }
      AQLINK*  Last() const
                  { return (AQLINK*)SYMBOLLIST::Last(); }
      AQLINK*  Current() const
                  { return (AQLINK*)SYMBOLLIST::Current(); }
      AQLINK*  Next() const
                  { return (AQLINK*)SYMBOLLIST::Next(); }
      AQLINK*  Prev() const
                  { return (AQLINK*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const AQLINK* link) const
                  { return SYMBOLLIST::Position(link); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      AQLINK*  Get(CARDINAL pos) const
                  { return (AQLINK*)SYMBOLLIST::Get(pos); }
      AQLINK*  Get(const char* sym) const
                  { return (AQLINK*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(AQLINK* link)
                  { return SYMBOLLIST::Goto(link); }
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

      BOOLEAN  Add(AQLINK* link)
                  { return SYMBOLLIST::InsertSorted(link); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(AQLINK* link)
                  { return SYMBOLLIST::Remove(link); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
