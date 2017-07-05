////////////////////////////////   plotsys.h   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    16.03.93    Peter Reichert
// revisions:   26.07.98    Peter Reichert    relative space added to ProbeVar
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
//   NODE         			LIST
//     |       				  |
//     |       				  |
//     +------+----SYMBOL		  +----SYMBOLLIST
//            |      |			         |
//     +------+      |			         |
//     |             |			         |
//   FILEIO          +----PLOT		         +----PLOTLIST
//     |       
//     +----PLOTSCROPT
//     |       
//     +----PLOTLISOPT
//     |
//     +----PLOTSYS
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// PLOTLISOPT:	plot list options
// ----------
// 
// PLOTFILOPT:	plot file layout options
// ----------
// 
// PLOTSCROPT:	plot screen layout options
// ----------
// 
// PLOTDATA:	curve data and info functions
// ---------
// 
// CURVE:	curve info; friend class PLOT
// -----
//
// PLOT:	plot definitions; list of curves in a plot
// ----
//
// PLOTLIST:	list of plots
// --------
//
// PLOTSYS:	plot management
// -------
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PLOTSYS_H
#define PLOTSYS_H

#include "othervar.h"
#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

enum SEPARATOR  { SepTab, SepComma, SepSpace };

enum PAPERSIZE  { SizeA4, SizeLetter, SizeLegal };

//////////////////////////////////////////////////////////////////////////////

enum ABSCTYPE   { AbscTime, AbscSpace };

enum CURVETYPE  { CurveVal,
                  CurveSensErr,
                  CurveSensAbsAbs, CurveSensRelAbs,
                  CurveSensAbsRel, CurveSensRelRel  };

enum LINESTYLE  { LineSolid, LineLongDashed, LineDashed, LineShortDashed,
                  LineDotted, LineLongDashDotted, LineShortDashDotted };

enum MARKSTYLE  { MarkCircle, MarkSquare, MarkTriangle, MarkDiamond,
                  MarkDot };

enum AQUACOLOR  { AquaBlack, AquaRed,     AquaGreen,  AquaBlue,
                  AquaCyan,  AquaMagenta, AquaYellow, AquaWhite };

//////////////////////////////////////////////////////////////////////////////

class PLOTLISOPT : public FILEIO
{
      friend class PLOTSYS;

   public:

      PLOTLISOPT();
      PLOTLISOPT(const PLOTLISOPT& lisopt);
      PLOTLISOPT(const PLOTLISOPT* lisopt);
      ~PLOTLISOPT();

      CARDINAL  Precision() const
                   { return precision; }
      SEPARATOR Separator() const
                   { return separator; }

      BOOLEAN   Precision(CARDINAL prec);
      BOOLEAN   Separator(SEPARATOR sep);

	  JOBSTATUS Load(std::istream& in);
      JOBSTATUS Save(std::ostream& out);
      JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      CARDINAL  precision;
      SEPARATOR separator;

      BOOLEAN   locked;

      void      init();
};

class PLOTFILOPT : public FILEIO
{
      friend class PLOTSYS;

   public:

      PLOTFILOPT();
      PLOTFILOPT(const PLOTFILOPT& filopt);
      PLOTFILOPT(const PLOTFILOPT* filopt);
      ~PLOTFILOPT();

      BOOLEAN     NewPage() const
                     { return newpage; }
      CARDINAL    NumPaperSizes() const;
      const char* PaperSizeName(CARDINAL index) const;
      PAPERSIZE   PaperSize() const
                     { return papersize; }
      REAL        PaperWidth() const;
      REAL        PaperHeight() const;
      BOOLEAN     Portrait() const
                     { return portrait; }
      REAL        BorderLeft() const
                     { return borderleft; }
      REAL        BorderRight() const
                     { return borderright; }
      REAL        BorderTop() const
                     { return bordertop; }
      REAL        BorderBottom() const
                     { return borderbottom; }
      REAL        WidthLegend() const
                     { return widthlegend; }
      void        WidthLegend(REAL width);
      CARDINAL    NumCol() const
                     { return numcol; }
      CARDINAL    NumRow() const
                     { return numrow; }
      CARDINAL    FontTitle() const
                     { return fonttitle; }
      CARDINAL    FontAxes() const
                     { return fontaxes; }
      CARDINAL    FontNumber() const
                     { return fontnumber; }
      CARDINAL    FontLegend() const
                     { return fontlegend; }
      BOOLEAN     Color() const
                     { return color; }
      BOOLEAN     Cm() const
                     { return cm; }
      REAL        Inch() const
                     { return inch; }
      BOOLEAN     Eps() const
                     { return eps; }

      void        NewPage(BOOLEAN b)
                     { newpage = b; }
      BOOLEAN     Portrait(BOOLEAN p);
      BOOLEAN     Measures(PAPERSIZE size,
                           CARDINAL  cols, CARDINAL rows,
                           REAL      left, REAL     right,
                           REAL      top,  REAL     bottom,
                           REAL      legend);
      BOOLEAN     Fonts(CARDINAL title,  CARDINAL axes,
                        CARDINAL number, CARDINAL legend);
      BOOLEAN     Color(BOOLEAN col);
      BOOLEAN     Cm(BOOLEAN c);
	  void        Eps(BOOLEAN single_frame)
	                 { eps=single_frame; }

      JOBSTATUS   Load(std::istream& in);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      BOOLEAN     newpage;
      PAPERSIZE   papersize;
      BOOLEAN     portrait;
      REAL        borderleft;
      REAL        borderright;
      REAL        bordertop;
      REAL        borderbottom;
      REAL        widthlegend;
      CARDINAL    numcol;
      CARDINAL    numrow;
      CARDINAL    fonttitle;
      CARDINAL    fontaxes;
      CARDINAL    fontnumber;
      CARDINAL    fontlegend;
      BOOLEAN     color;
      BOOLEAN     cm;
	  BOOLEAN     eps;		// *JR* jun97

      BOOLEAN     locked;

      static REAL inch;

      void        init();
};

class PLOTSCROPT : public FILEIO
{
      friend class PLOTSYS;

   public:

      PLOTSCROPT();
      PLOTSCROPT(const PLOTSCROPT& scropt);
      PLOTSCROPT(const PLOTSCROPT* scropt);
      ~PLOTSCROPT();

      CARDINAL  WinWidth() const
                   { return winwidth; }
      CARDINAL  WinHeight() const
                   { return winheight; }
      CARDINAL  BorderLeft() const
                   { return borderleft; }
      CARDINAL  BorderRight() const
                   { return borderright; }
      CARDINAL  BorderTop() const
                   { return bordertop; }
      CARDINAL  BorderBottom() const
                   { return borderbottom; }
      CARDINAL  WidthLegend() const
                   { return widthlegend; }
      CARDINAL  FontTitle() const
                   { return fonttitle; }
      CARDINAL  FontAxes() const
                   { return fontaxes; }
      CARDINAL  FontNumber() const
                   { return fontnumber; }
      CARDINAL  FontLegend() const
                   { return fontlegend; }

      BOOLEAN   Measures(CARDINAL width, CARDINAL height,
                         CARDINAL left,  CARDINAL right,
                         CARDINAL top,   CARDINAL bottom,
                         CARDINAL legend);
      BOOLEAN   Fonts(CARDINAL title,  CARDINAL axes,
                      CARDINAL number, CARDINAL legend);
      
      JOBSTATUS Load(std::istream& in);
      JOBSTATUS Save(std::ostream& out);
      JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      CARDINAL  winwidth;
      CARDINAL  winheight;
      CARDINAL  borderleft;
      CARDINAL  borderright;
      CARDINAL  bordertop;
      CARDINAL  borderbottom;
      CARDINAL  widthlegend;
      CARDINAL  fonttitle;
      CARDINAL  fontaxes;
      CARDINAL  fontnumber;
      CARDINAL  fontlegend;
      
      BOOLEAN   locked;

      void      init();
};

//////////////////////////////////////////////////////////////////////////////

class PLOTDATA
{
      friend class AQUASYS;

   public:

      PLOTDATA();
      PLOTDATA(const PLOTDATA* data);
      ~PLOTDATA();

      CARDINAL  NumCurveDef() const
                   { return nc; }
      CARDINAL  NumCurveData() const
                   { return nd; }
      BOOLEAN   IsCurve(CARDINAL ic) const;
      BOOLEAN   IsFinite() const;
      CARDINAL  NumPts(CARDINAL ic) const;
      REAL*     X(CARDINAL ic) const;
      REAL*     Y(CARDINAL ic) const;
      REAL*     Yminus(CARDINAL ic) const;
      REAL*     Yplus (CARDINAL ic) const;
      REAL      xMin() const;
      REAL      xMax() const;
      REAL      yMin() const;
      REAL      yMax() const;
      
   private:

      CARDINAL  nc;
      CARDINAL  nd;
      BOOLEAN*  iscurve;
      CARDINAL* npts;
      REAL**    x;
      REAL**    y;
      REAL**    yminus;
      REAL**    yplus;
      REAL      xmin;
      REAL      xmax;
      REAL      ymin;
      REAL      ymax;

      void      init();
      void      del();
};

//////////////////////////////////////////////////////////////////////////////

class CURVE
{
      friend class PLOT;

   public:

      CURVE();
      CURVE(const CURVE& curve);
      CURVE(const CURVE* curve);
      ~CURVE();

      BOOLEAN     Arg(const AQVAR* var) const;
      BOOLEAN     Arg(const AQCOMP* var) const;

      void        ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void        ExchangeVar(AQVAR* var1, AQVAR* var2);
      void        ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      AQVAR*      Variable() const
                     { return var; }
      BOOLEAN     Variable(AQVAR* v);
      CARDINAL    CalcNum() const
                     { return calcnum; }
      BOOLEAN     CalcNum(CARDINAL cn);
      AQCOMP*     Compartment() const
                     { return com; }
      BOOLEAN     Compartment(AQCOMP* com);
      CARDINAL    Zone() const
                     { return zone; }
      BOOLEAN     Zone(CARDINAL z);
      REAL        XorT() const
                     { return xort; }
      BOOLEAN     XorT(REAL xt);
      BOOLEAN     SpaceRel(BOOLEAN b);
      BOOLEAN     SpaceRel() const
                     { return spacerel; } 
      CURVETYPE   Type() const
                     { return type; }
      BOOLEAN     Type(CURVETYPE t);
      AQVAR*      Parameter() const
                     { return par; }
      BOOLEAN     Parameter(AQVAR* p);
      const char* Legend() const
                     { return legend; }
      void        Legend(const char* text);
      BOOLEAN     LineActive() const
                     { return lineactive; }
      void        LineActive(BOOLEAN act)
                     { lineactive = act; }
      LINESTYLE   LineStyle() const
                     { return linestyle; }
      void        LineStyle(LINESTYLE style )
                     { linestyle = style; }
      CARDINAL    LineWidth() const
                     { return linewidth; }
      void        LineWidth(CARDINAL width)
                     { linewidth = width; }
      AQUACOLOR   LineColor() const
                     { return linecolor; }
      void        LineColor(AQUACOLOR color)
                     { linecolor = color; }
      BOOLEAN     MarkActive() const
                     { return markactive; }
      void        MarkActive(BOOLEAN act)
                     { markactive = act; }
      MARKSTYLE   MarkStyle() const
                     { return markstyle; }
      void        MarkStyle(MARKSTYLE style)
                     { markstyle = style; }
      CARDINAL    MarkSize() const
                     { return marksize; }
      void        MarkSize(CARDINAL size)
                     { marksize = size; }
      AQUACOLOR   MarkColor() const
                     { return markcolor; }
      void        MarkColor(AQUACOLOR color)
                     { markcolor = color; }

   private:

      void        init();

      AQVAR*      var;
      CARDINAL    calcnum;
      AQCOMP*     com;
      CARDINAL    zone;
      REAL        xort;
      BOOLEAN     spacerel;
      CURVETYPE   type;
      AQVAR*      par;
      char*       legend;
      BOOLEAN     lineactive;
      LINESTYLE   linestyle;
      CARDINAL    linewidth;
      AQUACOLOR   linecolor;
      BOOLEAN     markactive;
      MARKSTYLE   markstyle;
      CARDINAL    marksize;
      AQUACOLOR   markcolor;

      BOOLEAN   locked;
};

 //////////////////////////////////////////////////////////////////////////////

class PLOT : public SYMBOL
{
      friend class PLOTSYS;
      friend class PLOTLISOPT;
      friend class PLOTFILOPT;
      friend class PLOTSCROPT;

   public:

      PLOT();
      PLOT(const PLOT* plot);
      ~PLOT();

      PLOT*           Next() const
                         { return (PLOT*)SYMBOL::Next(); }
      PLOT*           Prev() const
                         { return (PLOT*)SYMBOL::Prev(); }

      CARDINAL        NumVarArg() const;
      AQVAR*          VarArg(CARDINAL index) const;
      CARDINAL        NumCompArg() const;
      AQCOMP*         CompArg(CARDINAL index) const;

      BOOLEAN         Arg(const AQVAR* var) const;
      BOOLEAN         Arg(const AQCOMP* var) const;

      void            ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void            ExchangeVar(AQVAR* var1, AQVAR* var2);
      void            ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      const char*     Title() const
                         { return title; }
      void            Title(const char* text);
      const char*     xTitle() const
                         { return xtitle; }
      void            xTitle(const char* text);
      const char*     yTitle() const
                         { return ytitle; }
      void            yTitle(const char* text);

      ABSCTYPE        AbscType() const
                         { return absctype; }
      void            AbscType(ABSCTYPE type);

      REAL            xMin() const
                         { return xmin; }
      REAL            xMax() const
                         { return xmax; }
      BOOLEAN         xMinMax(REAL minimum, REAL maximum);
      REAL            xTickPos() const
                         { return xtickpos; }
      REAL            xTickDist() const
                         { return xtickdist; }
      void            xTick(REAL pos, REAL dist);
      BOOLEAN         xMinAuto() const
                         { return xminauto; }
      void            xMinAuto(BOOLEAN aut);
      BOOLEAN         xMaxAuto() const
                         { return xmaxauto; }
      void            xMaxAuto(BOOLEAN aut);
      BOOLEAN         xTickPosAuto() const
                         { return xtickposauto; }
      void            xTickPosAuto(BOOLEAN aut);
      BOOLEAN         xTickDistAuto() const
                         { return xtickdistauto; }
      void            xTickDistAuto(BOOLEAN aut);

      REAL            yMin() const
                         { return ymin; }
      REAL            yMax() const
                         { return ymax; }
      BOOLEAN         yMinMax(REAL minimum, REAL maximum);
      REAL            yTickPos() const
                         { return ytickpos; }
      REAL            yTickDist() const
                         { return ytickdist; }
      void            yTick(REAL pos, REAL dist);
      BOOLEAN         yMinAuto() const
                         { return yminauto; }
      void            yMinAuto(BOOLEAN aut);
      BOOLEAN         yMaxAuto() const
                         { return ymaxauto; }
      void            yMaxAuto(BOOLEAN aut);
      BOOLEAN         yTickPosAuto() const
                         { return ytickposauto; }
      void            yTickPosAuto(BOOLEAN aut);
      BOOLEAN         yTickDistAuto() const
                         { return ytickdistauto; }
      void            yTickDistAuto(BOOLEAN aut);

      CARDINAL        NumCurves() const
                         { return numcurves; }
      const CURVE*    Curve(CARDINAL index);
      BOOLEAN         AddCurve(CURVE* curve, CARDINAL pos=CARDINAL_MAX);
      BOOLEAN         ReplaceCurve(CURVE* curve, CARDINAL pos);
      BOOLEAN         DeleteCurve(CARDINAL pos);

      void            DeleteCurves(const AQVAR* var);
      void            DeleteCurves(const AQCOMP* com);

      JOBSTATUS       Load(std::istream& in,
                           const VARLIST* varlist,
                           const COMPLIST* complist);
      JOBSTATUS       Save(std::ostream& out);
      JOBSTATUS       Write(std::ostream& out, BOOLEAN sh=FALSE);

   private:

      char*           title;
      char*           xtitle;
      char*           ytitle;
      ABSCTYPE        absctype;
      REAL            xmin;
      REAL            xmax;
      REAL            xtickpos;
      REAL            xtickdist;
      REAL            ymin;
      REAL            ymax;
      REAL            ytickpos;
      REAL            ytickdist;
      BOOLEAN         xminauto;
      BOOLEAN         xmaxauto;
      BOOLEAN         xtickposauto;
      BOOLEAN         xtickdistauto;
      BOOLEAN         yminauto;
      BOOLEAN         ymaxauto;
      BOOLEAN         ytickposauto;
      BOOLEAN         ytickdistauto;

      CARDINAL        numcurves;
      CURVE**         curves;

      static CARDINAL number;

      static INIT     ini;

      void            init();
};

//////////////////////////////////////////////////////////////////////////////

class PLOTLIST : private SYMBOLLIST
{
      friend class PLOTSYS;

   public:

      CARDINAL Size() const
                  { return SYMBOLLIST::Size(); }
      BOOLEAN  Exist(const PLOT* plot) const
                  { return SYMBOLLIST::Exist(plot); }
      BOOLEAN  Exist(const char* sym) const
                  { return SYMBOLLIST::Exist(sym); }

      PLOT*    First() const
                  { return (PLOT*)SYMBOLLIST::First(); }
      PLOT*    Last() const
                  { return (PLOT*)SYMBOLLIST::Last(); }
      PLOT*    Current() const
                  { return (PLOT*)SYMBOLLIST::Current(); }
      PLOT*    Next() const
                  { return (PLOT*)SYMBOLLIST::Next(); }
      PLOT*    Prev() const
                  { return (PLOT*)SYMBOLLIST::Prev(); }
      CARDINAL Position() const
                  { return SYMBOLLIST::Position(); }
      CARDINAL Position(const PLOT* plot) const
                  { return SYMBOLLIST::Position(plot); }
      CARDINAL Position(const char* sym)
                  { return SYMBOLLIST::Position(sym); }
      PLOT*    Get(CARDINAL pos) const
                  { return (PLOT*)SYMBOLLIST::Get(pos); }
      PLOT*    Get(const char* sym) const
                  { return (PLOT*)SYMBOLLIST::Get(sym); }

      BOOLEAN  Goto(PLOT* plot)
                  { return SYMBOLLIST::Goto(plot); }
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

      BOOLEAN  Add(PLOT* plot)
                  { return SYMBOLLIST::InsertSorted(plot); }
      BOOLEAN  Remove()
                  { return SYMBOLLIST::Remove(); }
      BOOLEAN  Remove(PLOT* plot)
                  { return SYMBOLLIST::Remove(plot); }
      BOOLEAN  Remove(const char* sym)
                  { return SYMBOLLIST::Remove(sym); }
      void     RemoveAll()
                  { SYMBOLLIST::RemoveAll(); }
};

//////////////////////////////////////////////////////////////////////////////

class PLOTSYS : public FILEIO
{
   public:

      PLOTSYS();
      ~PLOTSYS();

      void        SetCancelFun(CANCELFUN fun)
                     { cancelfun = fun; }
      void        ResetCancelFun()
                     { cancelfun = 0; }

      BOOLEAN     Arg(const AQVAR* var) const;
      BOOLEAN     Arg(const AQCOMP* var) const;

      void        ReplaceVar(AQVAR* oldvar, AQVAR* newvar);
      void        ExchangeVar(AQVAR* var1, AQVAR* var2);
      void        ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp);

      BOOLEAN     Exist(const PLOT* plot) const
                     { return plotlist.Exist(plot); }
      BOOLEAN     Exist(const char* sym) const
                     { return Exist(Get(sym)); }

      PLOT*       Get(const char* sym) const
                     { return plotlist.Get(sym); }

      BOOLEAN     Add(PLOT* plot);
      BOOLEAN     Replace(PLOT* oldplot, PLOT* newplot);
      BOOLEAN     Delete(PLOT* plot);
      BOOLEAN     Delete(const char* sym)
                     { return Delete(Get(sym)); }
      void        Delete();

      void        DeleteCurves(const AQVAR* var);
      void        DeleteCurves(const AQCOMP* com);

      PLOTLIST*   Plotlist()
                     { return &plotlist; }

      BOOLEAN     LisOpt(PLOTLISOPT* opt);
      PLOTLISOPT* LisOpt()
                     { return lisopt; }
      CARDINAL    FilOpt(PLOTFILOPT* opt);
      PLOTFILOPT* FilOpt()
                     { return filopt; }
      CARDINAL    ScrOpt(PLOTSCROPT* opt);
      PLOTSCROPT* ScrOpt() const
                     { return scropt; }

      JOBSTATUS   Load(std::istream& in,
                       const VARLIST* varlist,
                       const COMPLIST* complist);
      JOBSTATUS   Save(std::ostream& out);
      JOBSTATUS   Write(std::ostream& out, BOOLEAN sh=FALSE);

   protected:

      PLOTLISOPT* lisopt;
      PLOTFILOPT* filopt;
      PLOTSCROPT* scropt;

      PLOTLIST    plotlist;

      CANCELFUN   cancelfun;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
