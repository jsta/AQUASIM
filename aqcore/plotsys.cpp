////////////////////////////////   plotsys.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    16.03.93    Peter Reichert
// revisions:   26.07.98    Peter Reichert    relative space added to ProbeVar
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "plotsys.h"
#include "drawpts.h"
#if AQUA_VERSION==AQUA_XVT
#include "plot_x.h"
#endif

//////////////////////////////////////////////////////////////////////////////

const char* PlotsysIdentifier    = "PLOTSYS";
const char* PlotIdentifier       = "PLOT";
const char* PlotLisOptIdentifier = "PLOTLISOPT";
const char* PlotFilOptIdentifier = "PLOTFILOPT";
const char* PlotScrOptIdentifier = "PLOTSCROPT";


// Identifiers for PLOTLISOPT::Save and PLOTLISOPT::Load
// (compare this constant with enum SEPARATOR from plotsys.h)

                                                 // enum SEPARATOR:
const char* SeparatorIdentifier[] = {
                                       "TAB",    //  SepTab
                                       "COMMA",  //  SepComma
                                       "SPACE"   //  SepSpace
                                    };

const CARDINAL NumSeparator = sizeof(SeparatorIdentifier)/sizeof(char*);


// Identifiers for PLOTFILOPT::Save and PLOTFILOPT::Load
// (compare this constant with enum PAPERSIZE from plotsys.h)

                                                 // enum PAPERSIZE:
const char* PaperSizeIdentifier[] = {
                                       "A4",     //  SizeA4
                                       "LET",    //  SizeLetter
                                       "LEG"     //  SizeLegal
                                    };

const CARDINAL NumPaperSize = sizeof(PaperSizeIdentifier)/sizeof(char*);


// Identifiers for PLOT::Save and PLOT::Load

// (compare this constant with enum ABSCTYPE from plotsys.h)

                                                 // enum ABSCTYPE:
const char* AbscTypeIdentifier[]  = {
                                       "TIME",   //  AbscTime
                                       "SPACE"   //  AbscSpace
                                    };

const CARDINAL NumAbscType = sizeof(AbscTypeIdentifier)/sizeof(char*);


// (compare this constant with enum CURVETYPE from plotsys.h)

                                                 // enum CURVETYPE:
const char* CurveTypeIdentifier[] = {
                                       "VAL",    //  CurveVal
                                       "SENSERR",//  CurveSensErr
                                       "SENSAA", //  CurveSensAbsAbs
                                       "SENSRA", //  CurveSensRelAbs
                                       "SENSAR", //  CurveSensAbsRel
                                       "SENSRR"  //  CurveSensRelRel
                                    };

const CARDINAL NumCurveType = sizeof(CurveTypeIdentifier)/sizeof(char*);


// (compare this constant with enum LINESTYLE from plotsys.h)

                                                 // enum LINESTYLE:
const char* LineStyleIdentifier[] = {
                                       "SOL",    //  LineSolid
                                       "LDASH",  //  LineLongDashed
                                       "DASH",   //  LineDashed
                                       "SDASH",  //  LineShortDashed
                                       "DOT",    //  LineDotted
                                       "LDD",    //  LineLongDashDotted
                                       "SDD"     //  LineShortDashDotted
                                    };

const CARDINAL NumLineStyle = sizeof(LineStyleIdentifier)/sizeof(char*);


// (compare this constant with enum MARKSTILE from plotsys.h)

                                                 // enum MARKSTILE:
const char* MarkStyleIdentifier[] = {
                                       "CIRC",   //  MarkCircle
                                       "SQARE",  //  MarkSquare
                                       "TRI",    //  MarkTriangle
                                       "DIA"     //  MarkDiamond
                                    };

const CARDINAL NumMarkStyle = sizeof(MarkStyleIdentifier)/sizeof(char*);


// (compare this constant with enum AQUACOLOR from plotsys.h)

                                                 // enum AQUACOLOR:
const char* AquaColorIdentifier[] = {
                                       "BLA",    //  AquaBlack
                                       "RED",    //  AquaRed
                                       "GRE",    //  AquaGreen
                                       "BLU",    //  AquaBlue
                                       "CYA",    //  AquaCyan
                                       "MAG",    //  AquaMagenta
                                       "YEL",    //  AquaYellow
                                       "WHI"     //  AquaWhite
                                    };

const CARDINAL NumAquaColor = sizeof(AquaColorIdentifier)/sizeof(char*);


const CARDINAL MinFont = 6;
const CARDINAL MaxFont = 30;

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

const REAL A4width       = 21.0;
const REAL A4height      = 29.7;

const REAL Letterwidth   = 21.59;
const REAL Letterheight  = 27.94;

const REAL Legalwidth    = 21.59;
const REAL Legalheight   = 35.56;

//////////////////////////////////////////////////////////////////////////////


void PLOTLISOPT::init()
{
   precision = 4;
   separator = SepTab;
   return;
}


PLOTLISOPT::PLOTLISOPT()
{
   init();
   locked = FALSE;
}


PLOTLISOPT::PLOTLISOPT(const PLOTLISOPT& lisopt)
{
   precision = lisopt.precision;
   separator = lisopt.separator;
   locked = FALSE;
}


PLOTLISOPT::PLOTLISOPT(const PLOTLISOPT* lisopt)
{
   init();
   if ( lisopt != 0 )
   {
      precision = lisopt->precision;
      separator = lisopt->separator;
   }
   locked = FALSE;
}


PLOTLISOPT::~PLOTLISOPT()
{
}


BOOLEAN PLOTLISOPT::Precision(CARDINAL prec)
{
   if ( locked == TRUE ) return FALSE;
   precision = prec;
   if ( precision > 16 ) precision = 16;
   if ( precision <  1 ) precision = 1;
   Changed();
   return TRUE;
}

BOOLEAN PLOTLISOPT::Separator(SEPARATOR sep)
{
   if ( locked == TRUE ) return FALSE;
   separator = sep;
   Changed();
   return TRUE;
}

JOBSTATUS PLOTLISOPT::Load(std::istream& in)
{
   char Buffer[BufSize];
   if ( LoadStart(in) == FALSE )                        return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )             return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            if ( LoadNumber(in,&precision) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                        return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumSeparator )
            {
               if ( strcmp(Buffer,SeparatorIdentifier[type]) == 0 ) break;
               type++;
            }
            switch ( type )
            {
               case SepTab:
                  Separator(SepTab);
                  break;
               case SepComma:
                  Separator(SepComma);
                  break;
               case SepSpace:
                  Separator(SepSpace);
                  break;
               default:
                  return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                          return PROBLEM;
   return OK;
}

JOBSTATUS PLOTLISOPT::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   if ( SaveNumber(out,precision) == FALSE )                   return PROBLEM;
   if ( Separator() >= NumSeparator )
      FatalError("PLOTLISOPT::Save: Unknown Separator");
   if ( SaveString(out,SeparatorIdentifier[separator]) == FALSE )
                                                               return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return FILEIO::Save(out);
}

JOBSTATUS PLOTLISOPT::Write(std::ostream&, BOOLEAN)
{
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

REAL PLOTFILOPT::inch = 2.54;

void PLOTFILOPT::init()
{
   newpage      = TRUE;
   papersize    = SizeA4;
   portrait     = TRUE;
   borderleft   =  1.0;
   borderright  =  1.0;
   bordertop    =  1.0;
   borderbottom =  1.0;
   widthlegend  =  2.5;
   numcol       =  2;
   numrow       =  4;
   fonttitle    = 10;
   fontaxes     =  8;
   fontnumber   =  8;
   fontlegend   =  8;
   color        = TRUE;
   cm           = TRUE;
   eps          = FALSE;		// *JR* jun97
   return;
}

PLOTFILOPT::PLOTFILOPT()
{
   init();
   locked = FALSE;
}


PLOTFILOPT::PLOTFILOPT(const PLOTFILOPT& filopt)
{
   newpage      = filopt.newpage;
   papersize    = filopt.papersize;
   portrait     = filopt.portrait;
   borderleft   = filopt.borderleft;
   borderright  = filopt.borderright;
   bordertop    = filopt.bordertop;
   borderbottom = filopt.borderbottom;
   widthlegend  = filopt.widthlegend;
   numcol       = filopt.numcol;
   numrow       = filopt.numrow;
   fonttitle    = filopt.fonttitle;
   fontaxes     = filopt.fontaxes;
   fontnumber   = filopt.fontnumber;
   fontlegend   = filopt.fontlegend;
   color        = filopt.color;
   cm           = filopt.cm;
   eps          = filopt.eps;	  	// *JR* jun97

   locked = FALSE;
}


PLOTFILOPT::PLOTFILOPT(const PLOTFILOPT* filopt)
{
   init();
   if ( filopt != 0 )
   {
      newpage      = filopt->newpage;
      papersize    = filopt->papersize;
      portrait     = filopt->portrait;
      borderleft   = filopt->borderleft;
      borderright  = filopt->borderright;
      bordertop    = filopt->bordertop;
      borderbottom = filopt->borderbottom;
      widthlegend  = filopt->widthlegend;
      numcol       = filopt->numcol;
      numrow       = filopt->numrow;
      fonttitle    = filopt->fonttitle;
      fontaxes     = filopt->fontaxes;
      fontnumber   = filopt->fontnumber;
      fontlegend   = filopt->fontlegend;
      color        = filopt->color;
      cm           = filopt->cm;
	  eps          = filopt->eps;	  	// *JR* jun97
   }
   locked = FALSE;
}


PLOTFILOPT::~PLOTFILOPT()
{
}


CARDINAL PLOTFILOPT::NumPaperSizes() const
{
   return 3;
}


const char* PLOTFILOPT::PaperSizeName(CARDINAL index) const
{
   switch ( index )
   {
      case 0:
         return PLOT::ini.T(23);
      case 1:
         return PLOT::ini.T(24);
      case 2:
         return PLOT::ini.T(25);
      default:
         FatalError("PLOTFILOPT::PaperSizeName: illegal index");
   }
   return 0;
}


REAL PLOTFILOPT::PaperWidth() const
{
   REAL wi = 0.0;
   switch ( papersize )
   {
      case SizeA4:
         wi = A4width;
         break;
      case SizeLetter:
         wi = Letterwidth;
         break;
      case SizeLegal:
         wi = Legalwidth;
         break;
   }
   return wi;
}

REAL PLOTFILOPT::PaperHeight() const
{
   REAL hi = 0.0;
   switch ( papersize )
   {
      case SizeA4:
         hi = A4height;
         break;
      case SizeLetter:
         hi = Letterheight;
         break;
      case SizeLegal:
         hi = Legalheight;
         break;
   }
   return hi;
}


BOOLEAN PLOTFILOPT::Portrait(BOOLEAN p)
{
   if ( locked == TRUE ) return FALSE;
   portrait = p;
   Changed();
   return TRUE;
}

BOOLEAN PLOTFILOPT::Measures(PAPERSIZE size,
                             CARDINAL cols, CARDINAL rows,
                             REAL     left, REAL     right,
                             REAL     top,  REAL     bottom,
                             REAL     legend)
{
   if ( locked == TRUE ) return FALSE;
   if ( left < 0.0 )     return FALSE;
   if ( right < 0.0 )    return FALSE;
   if ( top < 0.0 )      return FALSE;
   if ( bottom < 0.0 )   return FALSE;
   if ( legend < 0.0 )   return FALSE;
   if ( cols == 0 )      return FALSE;
   if ( rows == 0 )      return FALSE;
   papersize    = size;
   numcol       = cols;
   numrow       = rows;
   borderleft   = left;
   borderright  = right;
   bordertop    = top;
   borderbottom = bottom;
   widthlegend  = legend;
   Changed();
   return TRUE;
}

BOOLEAN PLOTFILOPT::Fonts(CARDINAL title,  CARDINAL axes,
                          CARDINAL number, CARDINAL legend)
{
   if ( locked == TRUE )                   return FALSE;
   if ( title <MinFont || title >MaxFont ) return FALSE;
   if ( axes  <MinFont || axes  >MaxFont ) return FALSE;
   if ( number<MinFont || number>MaxFont ) return FALSE;
   if ( legend<MinFont || legend>MaxFont ) return FALSE;
   fonttitle  = title;
   fontaxes   = axes;
   fontnumber = number;
   fontlegend = legend;
   Changed();
   return TRUE;
}


BOOLEAN PLOTFILOPT::Color(BOOLEAN col)
{
   if ( locked == TRUE ) return FALSE;
   color = col;
   Changed();
   return TRUE;
}

BOOLEAN PLOTFILOPT::Cm(BOOLEAN c)
{
   if ( locked == TRUE ) return FALSE;
   cm = c;
   Changed();
   return TRUE;
}

JOBSTATUS PLOTFILOPT::Load(std::istream& in)
{
   char Buffer[BufSize];
   if ( LoadStart(in) == FALSE )                         return PROBLEM;

   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )              return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                         return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumPaperSize )
            {
               if ( strcmp(Buffer,PaperSizeIdentifier[type]) == 0 ) break;
               type++;
            }
            switch ( type )
            {
               case SizeA4:
                  papersize = SizeA4;
                  break;
               case SizeLetter:
                  papersize = SizeLetter;
                  break;
               case SizeLegal:
                  papersize = SizeLegal;
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadBoolean(in,&portrait) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&borderleft) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&borderright) == FALSE )  return PROBLEM;
            if ( LoadNumber(in,&bordertop) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&borderbottom) == FALSE ) return PROBLEM;
            if ( LoadNumber(in,&widthlegend) == FALSE )  return PROBLEM;
            if ( LoadNumber(in,&numcol) == FALSE )       return PROBLEM;
            if ( LoadNumber(in,&numrow) == FALSE )       return PROBLEM;
            if ( LoadNumber(in,&fonttitle) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&fontaxes) == FALSE )     return PROBLEM;
            if ( LoadNumber(in,&fontnumber) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&fontlegend) == FALSE )   return PROBLEM;
            if ( LoadBoolean(in,&color) == FALSE )       return PROBLEM;
            if ( LoadBoolean(in,&cm) == FALSE )          return PROBLEM;
         }
         break;
      case 2: // aquavers=2: AQUASIM 2.0delta eps added
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                         return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumPaperSize )
            {
               if ( strcmp(Buffer,PaperSizeIdentifier[type]) == 0 ) break;
               type++;
            }
            switch ( type )
            {
               case SizeA4:
                  papersize = SizeA4;
                  break;
               case SizeLetter:
                  papersize = SizeLetter;
                  break;
               case SizeLegal:
                  papersize = SizeLegal;
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadBoolean(in,&portrait) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&borderleft) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&borderright) == FALSE )  return PROBLEM;
            if ( LoadNumber(in,&bordertop) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&borderbottom) == FALSE ) return PROBLEM;
            if ( LoadNumber(in,&widthlegend) == FALSE )  return PROBLEM;
            if ( LoadNumber(in,&numcol) == FALSE )       return PROBLEM;
            if ( LoadNumber(in,&numrow) == FALSE )       return PROBLEM;
            if ( LoadNumber(in,&fonttitle) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&fontaxes) == FALSE )     return PROBLEM;
            if ( LoadNumber(in,&fontnumber) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&fontlegend) == FALSE )   return PROBLEM;
            if ( LoadBoolean(in,&color) == FALSE )       return PROBLEM;
            if ( LoadBoolean(in,&cm) == FALSE )          return PROBLEM;
            if ( LoadBoolean(in,&eps) == FALSE )         return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                           return PROBLEM;
   return OK;
}

JOBSTATUS PLOTFILOPT::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   if ( PaperSize() >= NumPaperSize )
      FatalError("PLOTFILOPT::Save: Unknown PaperSize");
   if ( SaveString(out,PaperSizeIdentifier[papersize]) == FALSE )
                                                               return PROBLEM;
   if ( SaveBoolean(out,portrait) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,borderleft) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,borderright) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,bordertop) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,borderbottom) == FALSE )                return PROBLEM;
   if ( SaveNumber(out,widthlegend) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,numcol) == FALSE )                      return PROBLEM;
   if ( SaveNumber(out,numrow) == FALSE )                      return PROBLEM;
   if ( SaveNumber(out,fonttitle) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,fontaxes) == FALSE )                    return PROBLEM;
   if ( SaveNumber(out,fontnumber) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,fontlegend) == FALSE )                  return PROBLEM;
   if ( SaveBoolean(out,color) == FALSE )                      return PROBLEM;
   if ( SaveBoolean(out,cm) == FALSE )                         return PROBLEM;
   if ( SaveBoolean(out,eps) == FALSE )                        return PROBLEM;
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   return FILEIO::Save(out);
}

JOBSTATUS PLOTFILOPT::Write(std::ostream&, BOOLEAN)
{
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

void PLOTSCROPT::init()
{
   winwidth     = 600;
   winheight    = 400;
   borderleft   =  25;
   borderright  =  25;
   bordertop    =  25;
   borderbottom =  25;
   widthlegend  =  50;
   fonttitle    =  20;
   fontaxes     =  14;
   fontnumber   =  10;
   fontlegend   =  12;
   return;
}


PLOTSCROPT::PLOTSCROPT()
{
   init();
   locked = FALSE;
}


PLOTSCROPT::PLOTSCROPT(const PLOTSCROPT& scropt)
{
   winwidth     = scropt.winwidth;
   winheight    = scropt.winheight;
   borderleft   = scropt.borderleft;
   borderright  = scropt.borderright;
   bordertop    = scropt.bordertop;
   borderbottom = scropt.borderbottom;
   widthlegend  = scropt.widthlegend;
   fonttitle    = scropt.fonttitle;
   fontaxes     = scropt.fontaxes;
   fontnumber   = scropt.fontnumber;
   fontlegend   = scropt.fontlegend;
   locked = FALSE;
}


PLOTSCROPT::PLOTSCROPT(const PLOTSCROPT* scropt)
{
   init();
   if ( scropt != 0 )
   {
      winwidth     = scropt->winwidth;
      winheight    = scropt->winheight;
      borderleft   = scropt->borderleft;
      borderright  = scropt->borderright;
      bordertop    = scropt->bordertop;
      borderbottom = scropt->borderbottom;
      widthlegend  = scropt->widthlegend;
      fonttitle    = scropt->fonttitle;
      fontaxes     = scropt->fontaxes;
      fontnumber   = scropt->fontnumber;
      fontlegend   = scropt->fontlegend;
   }
   locked = FALSE;
}


PLOTSCROPT::~PLOTSCROPT()
{
}


BOOLEAN PLOTSCROPT::Measures(CARDINAL width, CARDINAL height,
                             CARDINAL left,  CARDINAL right,
                             CARDINAL top,   CARDINAL bottom,
                             CARDINAL legend)
{
   if ( locked == TRUE ) return FALSE;
   
// CARDINALS from xvt menu can be negative:
   int card;
   card=width;	if (card < 0) return FALSE;
   card=height;	if (card < 0) return FALSE;
   card=left;	if (card < 0) return FALSE;
   card=right;	if (card < 0) return FALSE;
   card=top;	if (card < 0) return FALSE;
   card=bottom;	if (card < 0) return FALSE;
   card=legend;	if (card < 0) return FALSE;
   
   winwidth     = width;
   winheight    = height;
   borderleft   = left;
   borderright  = right;
   bordertop    = top;
   borderbottom = bottom;
   widthlegend  = legend;
   Changed();
   return TRUE;
}


BOOLEAN PLOTSCROPT::Fonts(CARDINAL title,  CARDINAL axes,
                          CARDINAL number, CARDINAL legend)
{
   if ( locked == TRUE )                   return FALSE;
   if ( title <MinFont || title >MaxFont ) return FALSE;
   if ( axes  <MinFont || axes  >MaxFont ) return FALSE;
   if ( number<MinFont || number>MaxFont ) return FALSE;
   if ( legend<MinFont || legend>MaxFont ) return FALSE;
   fonttitle  = title;
   fontaxes   = axes;
   fontnumber = number;
   fontlegend = legend;
   Changed();
   return TRUE;
}


JOBSTATUS PLOTSCROPT::Load(std::istream& in)
{
   if ( LoadStart(in) == FALSE )                         return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )              return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            if ( LoadNumber(in,&winwidth) == FALSE )     return PROBLEM;
            if ( LoadNumber(in,&winheight) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&borderleft) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&borderright) == FALSE )  return PROBLEM;
            if ( LoadNumber(in,&bordertop) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&borderbottom) == FALSE ) return PROBLEM;
            if ( LoadNumber(in,&widthlegend) == FALSE )  return PROBLEM;
            if ( LoadNumber(in,&fonttitle) == FALSE )    return PROBLEM;
            if ( LoadNumber(in,&fontaxes) == FALSE )     return PROBLEM;
            if ( LoadNumber(in,&fontnumber) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&fontlegend) == FALSE )   return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                           return PROBLEM;
   return OK;
}

JOBSTATUS PLOTSCROPT::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )               return PROBLEM;

   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )     return PROBLEM;

   if ( SaveNumber(out,winwidth) == FALSE )     return PROBLEM;
   if ( SaveNumber(out,winheight) == FALSE )    return PROBLEM;
   if ( SaveNumber(out,borderleft) == FALSE )   return PROBLEM;
   if ( SaveNumber(out,borderright) == FALSE )  return PROBLEM;
   if ( SaveNumber(out,bordertop) == FALSE )    return PROBLEM;
   if ( SaveNumber(out,borderbottom) == FALSE ) return PROBLEM;
   if ( SaveNumber(out,widthlegend) == FALSE )  return PROBLEM;
   if ( SaveNumber(out,fonttitle) == FALSE )    return PROBLEM;
   if ( SaveNumber(out,fontaxes) == FALSE )     return PROBLEM;
   if ( SaveNumber(out,fontnumber) == FALSE )   return PROBLEM;
   if ( SaveNumber(out,fontlegend) == FALSE )   return PROBLEM;

   if ( SaveEnd(out) == FALSE )                 return PROBLEM;
   return FILEIO::Save(out);
}

JOBSTATUS PLOTSCROPT::Write(std::ostream&, BOOLEAN)
{
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

PLOTDATA::PLOTDATA()
{
#ifdef HEAPTEST
HeapFileMessage("start PLOTDATA::PLOTDATA()");
#endif

   init();

#ifdef HEAPTEST
HeapFileMessage("end   PLOTDATA::PLOTDATA()");
#endif
}


PLOTDATA::~PLOTDATA()
{
#ifdef HEAPTEST
HeapFileMessage("start PLOTDATA::~PLOTDATA()");
#endif

   del();

#ifdef HEAPTEST
HeapFileMessage("end   PLOTDATA::~PLOTDATA()");
#endif
}


void PLOTDATA::init()
{
   nc      = 0;
   nd      = 0;
   iscurve = 0;
   npts    = 0;
   x       = 0;
   y       = 0;
   yminus  = 0;
   yplus   = 0;
}


void PLOTDATA::del()
{
   if ( nc > 0 )
   {
      for ( CARDINAL i=0; i<nc; i++ )
      {
         if ( iscurve[i] == TRUE )
         {
            if ( x[i] != 0 )      delete [] x[i];
            if ( y[i] != 0 )      delete [] y[i];
            if ( yminus[i] != 0 ) delete [] yminus[i];
            if ( yplus[i]  != 0 ) delete [] yplus[i];
         }
      }
      delete [] iscurve;
      delete [] npts;
      delete [] x;
      delete [] y;
      delete [] yminus;
      delete [] yplus;
   }
   nc      = 0;
   nd      = 0;
   iscurve = 0;
   npts    = 0;
   x       = 0;
   y       = 0;
   yminus  = 0;
   yplus   = 0;
}


PLOTDATA::PLOTDATA(const PLOTDATA* data)
{
   CARDINAL i,j;
   init();
   if ( data != 0 )
   {
      nc = data->nc;
      nd = data->nd;
      if ( nc > 0 )
      {
         iscurve = new BOOLEAN[nc];
         npts    = new CARDINAL[nc];
         x       = new REAL*[nc];
         y       = new REAL*[nc];
         yminus  = new REAL*[nc];
         yplus   = new REAL*[nc];
         for ( i=0; i<nc; i++ )
         {
            iscurve[i] = data->iscurve[i];
            if ( iscurve[i] == TRUE )
            {
               npts[i]    = data->npts[i];
               x[i] = new REAL[npts[i]];
               y[i] = new REAL[npts[i]];
               for ( j=0; j<npts[i]; j++ )
               {
                  x[i][j] = data->x[i][j];
                  y[i][j] = data->y[i][j];
               }
               if ( data->yminus[i] != 0 )
               {
                  yminus[i] = new REAL[npts[i]];
                  for ( j=0; j<npts[i]; j++ )
                  {
                     yminus[i][j] = data->yminus[i][j];
                  }
               }
               else
               {
                  yminus[i] = 0;
               }
               if ( data->yplus[i] != 0 )
               {
                  yplus[i] = new REAL[npts[i]];
                  for ( j=0; j<npts[i]; j++ )
                  {
                     yplus[i][j] = data->yplus[i][j];
                  }
               }
               else
               {
                  yplus[i] = 0;
               }
            }
            else
            {
               npts[i] = 0;
               x[i] = 0; y[i] = 0; yminus[i] = 0; yplus[i] = 0;
            }
         }
      }
      xmin = data->xmin;
      xmax = data->xmax;
      ymin = data->ymin;
      ymax = data->ymax;
   }
}


BOOLEAN PLOTDATA::IsCurve(CARDINAL ic) const
{
   if ( ic >= nc ) FatalError("PLOTDATA::IsCurve: Illegal index");
   return iscurve[ic];
}


BOOLEAN PLOTDATA::IsFinite() const
{
   CARDINAL i,j;
   for ( i=0; i<nc; i++ )
   {
      if ( iscurve[i] == TRUE )
      {
         for ( j=0; j<npts[i]; j++ )
         {
            if ( !aqfinite(x[i][j]) ) return FALSE;
            if ( !aqfinite(y[i][j]) ) return FALSE;
         }
         if ( yminus[i] != 0 )
         {
            if ( !aqfinite(yminus[i][j]) ) return FALSE;
         }
         if ( yplus[i] != 0 )
         {
            if ( !aqfinite(yplus[i][j]) ) return FALSE;
         }
      }
   }
   return TRUE;
}


CARDINAL PLOTDATA::NumPts(CARDINAL ic) const
{
   if ( ic >= nc ) FatalError("PLOTDATA::NumPTs: Illegal index");
   return npts[ic];
}


REAL* PLOTDATA::X(CARDINAL ic) const
{
   if ( ic >= nc ) FatalError("PLOTDATA::X: Illegal index");
   return x[ic];
}


REAL* PLOTDATA::Y(CARDINAL ic) const
{
   if ( ic >= nc ) FatalError("PLOTDATA::Y: Illegal index");
   return y[ic];
}


REAL* PLOTDATA::Yminus(CARDINAL ic) const
{
   if ( ic >= nc ) FatalError("PLOTDATA::Yminus: Illegal index");
   return yminus[ic];
}


REAL* PLOTDATA::Yplus(CARDINAL ic) const
{
   if ( ic >= nc ) FatalError("PLOTDATA::Yplus: Illegal index");
   return yplus[ic];
}


REAL PLOTDATA::xMin() const
{
   if ( nd == 0 ) FatalError("PLOTDATA::xMin: Illegal call (no data)");
   return xmin;
}


REAL PLOTDATA::xMax() const
{
   if ( nd == 0 ) FatalError("PLOTDATA::xMax: Illegal call (no data)");
   return xmax;
}


REAL PLOTDATA::yMin() const
{
   if ( nd == 0 ) FatalError("PLOTDATA::yMin: Illegal call (no data)");
   return ymin;
}


REAL PLOTDATA::yMax() const
{
   if ( nd == 0 ) FatalError("PLOTDATA::yMax: Illegal call (no data)");
   return ymax;
}


//////////////////////////////////////////////////////////////////////////////


void CURVE::init()
{
   var        = 0;
   calcnum    = 0;
   com        = 0;
   zone       = 0;
   xort       = 0.0;
   spacerel   = FALSE;
   type       = CurveVal;
   par        = 0;
   legend     = 0;
   lineactive = TRUE;
   linestyle  = LineSolid;
   linewidth  = 2;
   linecolor  = AquaBlack;
   markactive = FALSE;
   markstyle  = MarkCircle;
   marksize   = 6;
   markcolor  = AquaBlack;
   locked     = FALSE;
   return;
}


CURVE::CURVE()
{
   init();
}


CURVE::CURVE(const CURVE* curve)
{
   init();
   if ( curve != 0 )
   {
      var        = curve->var;
      calcnum    = curve->calcnum;
      com        = curve->com;
      zone       = curve->zone;
      xort       = curve->xort;
      spacerel   = curve->spacerel;
      type       = curve->type;
      par        = curve->par;
      if ( curve->legend != 0 )
      {
         legend  = new char[strlen(curve->legend)+1];
         strcpy(legend,curve->legend);
      }
      lineactive = curve->lineactive;
      linestyle  = curve->linestyle;
      linewidth  = curve->linewidth;
      linecolor  = curve->linecolor;
      markactive = curve->markactive;
      markstyle  = curve->markstyle;
      marksize   = curve->marksize;
      markcolor  = curve->markcolor;
   }
}


CURVE::CURVE(const CURVE& curve)
{
   init();
   var        = curve.var;
   calcnum    = curve.calcnum;
   com        = curve.com;
   zone       = curve.zone;
   xort       = curve.xort;
   spacerel   = curve.spacerel;
   type       = curve.type;
   par        = curve.par;
   if ( curve.legend != 0 )
   {
      legend  = new char[strlen(curve.legend)+1];
      strcpy(legend,curve.legend);
   }
   lineactive = curve.lineactive;
   linestyle  = curve.linestyle;
   linewidth  = curve.linewidth;
   linecolor  = curve.linecolor;
   markactive = curve.markactive;
   markstyle  = curve.markstyle;
   marksize   = curve.marksize;
   markcolor  = curve.markcolor;
}


CURVE::~CURVE()
{
   delete legend;
}


BOOLEAN CURVE::Arg(const AQVAR* v) const
{
   if ( v == 0 ) return FALSE;
   if ( v == var ) return TRUE;
   if ( v == par ) return TRUE;
   return FALSE;
}


BOOLEAN CURVE::Arg(const AQCOMP* c) const
{
   if ( c == 0 ) return FALSE;
   if ( c == com ) return TRUE;
   return FALSE;
}


void CURVE::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( var == oldvar ) var = newvar;
   if ( par == oldvar ) par = newvar;
   return;
}


void CURVE::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( var == var1 )
   {
      var = var2;
   }
   else
   {
      if ( var == var2 ) var = var1;
   }
   return;
}


void CURVE::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   if ( com == oldcomp ) com = newcomp;
   return;
}


BOOLEAN CURVE::Variable(AQVAR* v)
{
   if ( locked == TRUE ) return FALSE;
   var = v;
   return TRUE;
}


BOOLEAN CURVE::CalcNum(CARDINAL cn)
{
   if ( locked == TRUE ) return FALSE;
   calcnum = cn;
   return TRUE;
}


BOOLEAN CURVE::Compartment(AQCOMP* c)
{
   if ( locked == TRUE ) return FALSE;
   com = c;
   return TRUE;
}


BOOLEAN CURVE::Zone(CARDINAL z)
{
   if ( locked == TRUE ) return FALSE;
   zone = z;
   return TRUE;
}


BOOLEAN CURVE::XorT(REAL xt)
{
   if ( locked == TRUE ) return FALSE;
   xort = xt;
   return TRUE;
}


BOOLEAN CURVE::SpaceRel(BOOLEAN b)
{
   if ( locked == TRUE ) return FALSE;
   spacerel = b;
   return TRUE;
}


BOOLEAN CURVE::Type(CURVETYPE t)
{
   if ( locked == TRUE ) return FALSE;
   type = t;
   return TRUE;
}


BOOLEAN CURVE::Parameter(AQVAR* p)
{
   if ( locked == TRUE ) return FALSE;
   par = p;
   return TRUE;
}


void CURVE::Legend(const char* text)
{
   delete legend; legend = 0;
   if ( text != 0 )
   {
      legend = new char[strlen(text)+1];
      strcpy(legend,text);
      for ( CARDINAL i=0; i<strlen(legend); i++ )
      {
         if ( legend[i] == '{' ) legend[i] = '[';
         if ( legend[i] == '}' ) legend[i] = ']';
         if ( legend[i] <  ' ' ) legend[i] = ' ';
      }
   }
   return;
}


//////////////////////////////////////////////////////////////////////////////

CARDINAL PLOT::number = 0;


INIT PLOT::ini(initfile,"PLOTSYS","plot_txt_",10,10,10,50);


void PLOT::init()
{
   number++;
   title         = 0;
   xtitle        = 0;
   ytitle        = 0;
   absctype      = AbscTime;
   xmin          = 0.0;
   xmax          = 1.0;
   xtickpos      = 0.0;
   xtickdist     = 0.1;
   ymin          = 0.0;
   ymax          = 1.0;
   ytickpos      = 0.0;
   ytickdist     = 0.1;
   xminauto      = TRUE;
   xmaxauto      = TRUE;
   xtickposauto  = TRUE;
   xtickdistauto = TRUE;
   yminauto      = TRUE;
   ymaxauto      = TRUE;
   ytickposauto  = TRUE;
   ytickdistauto = TRUE;
   numcurves     = 0;
   curves        = 0;
}


PLOT::PLOT()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
   strcpy(Buf1,ini.T(1));
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("PLOT::PLOT: illegal symbol");
   return;
}


PLOT::PLOT(const PLOT* plot) : SYMBOL(plot)
{
   init();
   if ( plot != 0 )
   {
      delete title; title = 0;
      if ( plot->title != 0 )
      {
         title = new char[strlen(plot->title)+1];
         strcpy(title,plot->title);
      }
      delete xtitle; xtitle = 0;
      if ( plot->xtitle != 0 )
      {
         xtitle = new char[strlen(plot->xtitle)+1];
         strcpy(xtitle,plot->xtitle);
      }
      delete ytitle; ytitle = 0;
      if ( plot->ytitle != 0 )
      {
         ytitle = new char[strlen(plot->ytitle)+1];
         strcpy(ytitle,plot->ytitle);
      }
      absctype = plot->absctype;
      xmin          = plot->xmin;
      xmax          = plot->xmax;
      xtickpos      = plot->xtickpos;
      xtickdist     = plot->xtickdist;
      ymin          = plot->ymin;
      ymax          = plot->ymax;
      ytickpos      = plot->ytickpos;
      ytickdist     = plot->ytickdist;
      xminauto      = plot->xminauto;
      xmaxauto      = plot->xmaxauto;
      xtickposauto  = plot->xtickposauto;
      xtickdistauto = plot->xtickdistauto;
      yminauto      = plot->yminauto;
      ymaxauto      = plot->ymaxauto;
      ytickposauto  = plot->ytickposauto;
      ytickdistauto = plot->ytickdistauto;
      if ( plot->numcurves != 0 )
      {
         numcurves = plot->numcurves;
         curves = new CURVE*[numcurves];
         for ( CARDINAL i=0; i<numcurves; i++ )
            curves[i] = new CURVE(plot->curves[i]);
      }
   }
   else
   {
      char Buf1[BufSize]; char Buf2[BufSize];
      strcpy(Buf1,ini.T(1));
      NumToString(number,Buf2,BufSize);
      strcat(Buf1,Buf2);
      if ( Symbol(Buf1) == FALSE )
         FatalError("PLOT::PLOT: illegal symbol");
   }
}


PLOT::~PLOT()
{
   if ( number == 0 )
      FatalError("PLOT::~PLOT: already all PLOTs destructed");
   number--;
   delete [] title;
   delete [] xtitle;
   delete [] ytitle;
   for ( CARDINAL i=0; i<numcurves; i++ ) delete curves[i];
   delete [] curves;
}


CARDINAL PLOT::NumVarArg() const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      if ( curves[i]->var != 0 ) num++;
      if ( curves[i]->par != 0 ) num++;
   }
   return num;
}


AQVAR* PLOT::VarArg(CARDINAL index) const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      if ( curves[i]->var != 0 )
      {
         if ( num == index ) return curves[i]->var;
         num++;
      }
      if ( curves[i]->par != 0 )
      {
         if ( num == index ) return curves[i]->par;
         num++;
      }
   }
   FatalError("PLOT::VarArg: illegal index");
   return 0;
}


CARDINAL PLOT::NumCompArg() const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      if ( curves[i]->com != 0 ) num++;
   }
   return num;
}


AQCOMP* PLOT::CompArg(CARDINAL index) const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      if ( curves[i]->com != 0 )
      {
         if ( num == index ) return curves[i]->com;
         num++;
      }
   }
   FatalError("PLOT::CompArg: illegal index");
   return 0;
}


BOOLEAN PLOT::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      if ( curves[i]->Arg(var) ) return TRUE;
   }
   return FALSE;
}


BOOLEAN PLOT::Arg(const AQCOMP* com) const
{
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      if ( curves[i]->Arg(com) ) return TRUE;
   }
   return FALSE;
}


void PLOT::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      curves[i]->ReplaceVar(oldvar,newvar);
   }
   return;
}


void PLOT::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      curves[i]->ExchangeVar(var1,var2);
   }
   return;
}


void PLOT::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   int i;
   for ( i=numcurves-1; i>=0; i-- )
   {
      if ( curves[i]->Compartment() == oldcomp )
      {
         if ( curves[i]->Zone() >= newcomp->NumZones() )
         {
            if ( newcomp->Type()!=LakeComp && newcomp->Type()!=RivComp )
               DeleteCurve(i);
         }
      }
   }
   for ( i=0; i<numcurves; i++ )
   {
      curves[i]->ReplaceComp(oldcomp,newcomp);
   }
   return;
}


void PLOT::Title(const char* text)
{
   delete title;
   title = new char[strlen(text)+1];
   strcpy(title,text);
   for ( CARDINAL i=0; i<strlen(title); i++ )
   {
      if ( title[i] == '{' ) title[i] = '[';
      if ( title[i] == '}' ) title[i] = ']';
      if ( title[i] <  ' ' ) title[i] = ' ';
   }
   Changed();
   return;
}


void PLOT::xTitle(const char* text)
{
   delete xtitle;
   xtitle = new char[strlen(text)+1];
   strcpy(xtitle,text);
   for ( CARDINAL i=0; i<strlen(xtitle); i++ )
   {
      if ( xtitle[i] == '{' ) xtitle[i] = '[';
      if ( xtitle[i] == '}' ) xtitle[i] = ']';
      if ( xtitle[i] <  ' ' ) xtitle[i] = ' ';
   }
   Changed();
   return;
}


void PLOT::yTitle(const char* text)
{
   delete ytitle;
   ytitle = new char[strlen(text)+1];
   strcpy(ytitle,text);
   for ( CARDINAL i=0; i<strlen(ytitle); i++ )
   {
      if ( ytitle[i] == '{' ) ytitle[i] = '[';
      if ( ytitle[i] == '}' ) ytitle[i] = ']';
      if ( ytitle[i] <  ' ' ) ytitle[i] = ' ';
   }
   Changed();
   return;
}


void PLOT::AbscType(ABSCTYPE type)
{
   absctype = type;
   Changed();
   return;
}


BOOLEAN PLOT::xMinMax(REAL minimum, REAL maximum)
{
   if ( minimum >= maximum ) return FALSE;
   xmin = minimum;
   xmax = maximum;
   Changed();
   return TRUE;
}


void PLOT::xTick(REAL pos, REAL dist)
{
   xtickpos  = pos;
   xtickdist = dist;
   Changed();
   return;
}


BOOLEAN PLOT::yMinMax(REAL min, REAL max)
{
   if ( min >= max ) return FALSE;
   ymin = min;
   ymax = max;
   Changed();
   return TRUE;
}


void PLOT::yTick(REAL pos, REAL dist)
{
   ytickpos  = pos;
   ytickdist = dist;
   Changed();
   return;
}


void PLOT::xMinAuto(BOOLEAN aut)
{
   xminauto = aut;
   Changed();
   return;
}


void PLOT::xMaxAuto(BOOLEAN aut)
{
   xmaxauto = aut;
   Changed();
   return;
}


void PLOT::xTickPosAuto(BOOLEAN aut)
{
   xtickposauto = aut;
   Changed();
   return;
}


void PLOT::xTickDistAuto(BOOLEAN aut)
{
   xtickdistauto = aut;
   Changed();
   return;
}


void PLOT::yMinAuto(BOOLEAN aut)
{
   yminauto = aut;
   Changed();
   return;
}


void PLOT::yMaxAuto(BOOLEAN aut)
{
   ymaxauto = aut;
   Changed();
   return;
}


void PLOT::yTickPosAuto(BOOLEAN aut)
{
   ytickposauto = aut;
   Changed();
   return;
}


void PLOT::yTickDistAuto(BOOLEAN aut)
{
   ytickdistauto = aut;
   Changed();
   return;
}


const CURVE* PLOT::Curve(CARDINAL index)
{
   if ( index >= numcurves )
      FatalError("PLOT::Curve: illegal index");
   return curves[index];
}


BOOLEAN PLOT::AddCurve(CURVE* curve, CARDINAL pos)
{
   if ( InList() == TRUE )                                    return FALSE;
   if ( curve == 0 )                                          return FALSE;
   if ( curve->Variable() == 0 )                              return FALSE;
   if ( curve->Compartment() == 0 )                           return FALSE;
   if ( curve->Type()!=CurveVal && curve->Parameter()==0 )    return FALSE;
   if ( curve->Zone() >= (curve->Compartment())->NumZones() 
        && curve->Compartment()->Type() != LakeComp 
        && curve->Compartment()->Type() != RivComp )          return FALSE;
   if ( pos > numcurves ) pos = numcurves;
   CURVE** newcurves = new CURVE*[numcurves+1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newcurves[i] = curves[i];
   }
   newcurves[pos] = curve;
   newcurves[pos]->locked = TRUE;
   for ( i=pos; i<numcurves; i++ )
   {
      newcurves[i+1] = curves[i];
   }
   numcurves++;
   delete curves; curves = newcurves;
   Changed();
   return TRUE;
}


BOOLEAN PLOT::ReplaceCurve(CURVE* curve, CARDINAL pos)
{
   if ( InList() == TRUE )                                    return FALSE;
   if ( curve == 0 )                                          return FALSE;
   if ( curve->Variable() == 0 )                              return FALSE;
   if ( curve->Compartment() == 0 )                           return FALSE;
   if ( curve->Type()!=CurveVal && curve->Parameter()==0 )    return FALSE;
   if ( curve->Zone() >= (curve->Compartment())->NumZones() 
        && curve->Compartment()->Type() != LakeComp
        && curve->Compartment()->Type() != RivComp )          return FALSE;
   if ( pos > numcurves )                                     return FALSE;
   delete curves[pos];
   curves[pos] = curve;
   curves[pos]->locked = TRUE;
   Changed();
   return TRUE;
}


BOOLEAN PLOT::DeleteCurve(CARDINAL pos)
{
   if ( pos >= numcurves ) return FALSE;
   if ( numcurves == 1 )
   {
      delete curves[0];
      delete [] curves; curves = 0; numcurves = 0;
      return TRUE;
   }
   CURVE** newcurves = new CURVE*[numcurves-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ ) newcurves[i] = curves[i];
   for ( i=pos+1; i<numcurves; i++ ) newcurves[i-1] = curves[i];
   delete curves[pos];
   delete [] curves; curves = newcurves;
   numcurves--;
   return TRUE;
}


void PLOT::DeleteCurves(const AQVAR* var)
{
   for ( INTEGER i=numcurves-1; i>=0; i-- )
   {
      if ( curves[i]->Arg(var) ) DeleteCurve(i);
   }
   return;
}


void PLOT::DeleteCurves(const AQCOMP* com)
{
   for ( INTEGER i=numcurves-1; i>=0; i-- )
   {
      if ( curves[i]->Arg(com) ) DeleteCurve(i);
   }
   return;
}


JOBSTATUS PLOT::Load(std::istream& in,
                     const VARLIST* varlist,
                     const COMPLIST* complist)
{
   char Buffer[BufSize];
   if ( InList() == TRUE )                                     return PROBLEM;

   if ( LoadStart(in) == FALSE )                               return PROBLEM;

   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                    return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            if ( Symbol(Buffer) == FALSE )                     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            Description(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            Title(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            xTitle(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            yTitle(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumAbscType )
            {
               if ( strcmp(Buffer,AbscTypeIdentifier[type]) == 0 ) break;
               type++;
            }
            switch ( type )
            {
               case AbscTime:
                  AbscType(AbscTime);
                  break;
               case AbscSpace:
                  AbscType(AbscSpace);
                  break;
               default:
                  return PROBLEM;
            }

            REAL r1, r2; BOOLEAN b1, b2;
            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            if ( xMinMax(r1,r2) == FALSE )                     return PROBLEM;
            xMinAuto(b1);
            xMaxAuto(b2);

            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            xTick(r1,r2);
            xTickPosAuto(b1);
            xTickDistAuto(b2);

            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            if ( yMinMax(r1,r2) == FALSE )                     return PROBLEM;
            yMinAuto(b1);
            yMaxAuto(b2);

            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            yTick(r1,r2);
            yTickPosAuto(b1);
            yTickDistAuto(b2);

            if ( LoadStart(in) == FALSE )                      return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               CARDINAL c; REAL r; BOOLEAN b;
               CURVE* curve = new CURVE;
               curve->Variable(varlist->Get(Buffer));
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->CalcNum(c);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               curve->Compartment(complist->Get(Buffer));
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->Zone(c);
               if ( LoadNumber(in,&r) == FALSE )               return PROBLEM;
               curve->XorT(r);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               CARDINAL type = 0; CURVETYPE ct;
               while ( type < NumCurveType )
               {
                  if ( strcmp(Buffer,CurveTypeIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case CurveVal:
                     ct = CurveVal;
                     break;
                  case CurveSensErr:
                     ct = CurveSensErr;
                     break;
                  case CurveSensAbsAbs:
                     ct = CurveSensAbsAbs;
                     break;
                  case CurveSensRelAbs:
                     ct = CurveSensRelAbs;
                     break;
                  case CurveSensAbsRel:
                     ct = CurveSensAbsRel;
                     break;
                  case CurveSensRelRel:
                     ct = CurveSensRelRel;
                     break;
                  default:
                     return PROBLEM;
               }
               curve->Type(ct);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               curve->Parameter(varlist->Get(Buffer));

               if ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
               {
                  curve->Legend(Buffer);
               }

               if ( LoadBoolean(in,&b) == FALSE )              return PROBLEM;
               curve->LineActive(b);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumLineStyle )
               {
                  if ( strcmp(Buffer,LineStyleIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case LineSolid:
                     curve->LineStyle(LineSolid);
                     break;
                  case LineLongDashed:
                     curve->LineStyle(LineLongDashed);
                     break;
                  case LineDashed:
                     curve->LineStyle(LineDashed);
                     break;
                  case LineShortDashed:
                     curve->LineStyle(LineShortDashed);
                     break;
                  case LineDotted:
                     curve->LineStyle(LineDotted);
                     break;
                  case LineLongDashDotted:
                     curve->LineStyle(LineLongDashDotted);
                     break;
                  case LineShortDashDotted:
                     curve->LineStyle(LineShortDashDotted);
                     break;
               }
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->LineWidth(c);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumAquaColor )
               {
                  if ( strcmp(Buffer,AquaColorIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case AquaBlack:
                     curve->LineColor(AquaBlack);
                     break;
                  case AquaRed:
                     curve->LineColor(AquaRed);
                     break;
                  case AquaGreen:
                     curve->LineColor(AquaGreen);
                     break;
                  case AquaBlue:
                     curve->LineColor(AquaBlue);
                     break;
                  case AquaCyan:
                     curve->LineColor(AquaCyan);
                     break;
                  case AquaMagenta:
                     curve->LineColor(AquaMagenta);
                     break;
                  case AquaYellow:
                     curve->LineColor(AquaYellow);
                     break;
                  case AquaWhite:
                     curve->LineColor(AquaWhite);
                     break;
                  default:
                     return PROBLEM;
               }

               if ( LoadBoolean(in,&b) == FALSE )              return PROBLEM;
               curve->MarkActive(b);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumMarkStyle )
               {
                  if ( strcmp(Buffer,MarkStyleIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case MarkCircle:
                     curve->MarkStyle(MarkCircle);
                     break;
                  case MarkSquare:
                     curve->MarkStyle(MarkSquare);
                     break;
                  case MarkTriangle:
                     curve->MarkStyle(MarkTriangle);
                     break;
                  case MarkDiamond:
                     curve->MarkStyle(MarkDiamond);
                     break;
                  default:
                     return PROBLEM;
               }
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->MarkSize(c);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumAquaColor )
               {
                  if ( strcmp(Buffer,AquaColorIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case AquaBlack:
                     curve->MarkColor(AquaBlack);
                     break;
                  case AquaRed:
                     curve->MarkColor(AquaRed);
                     break;
                  case AquaGreen:
                     curve->MarkColor(AquaGreen);
                     break;
                  case AquaBlue:
                     curve->MarkColor(AquaBlue);
                     break;
                  case AquaCyan:
                     curve->MarkColor(AquaCyan);
                     break;
                  case AquaMagenta:
                     curve->MarkColor(AquaMagenta);
                     break;
                  case AquaYellow:
                     curve->MarkColor(AquaYellow);
                     break;
                  case AquaWhite:
                     curve->MarkColor(AquaWhite);
                     break;
                  default:
                     return PROBLEM;
               }

               if ( AddCurve(curve) == FALSE )                 return PROBLEM;
            }
         }
         break;
      case 2: // aquavers=2: spacerel added
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            if ( Symbol(Buffer) == FALSE )                     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            Description(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            Title(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            xTitle(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            yTitle(Buffer);

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumAbscType )
            {
               if ( strcmp(Buffer,AbscTypeIdentifier[type]) == 0 ) break;
               type++;
            }
            switch ( type )
            {
               case AbscTime:
                  AbscType(AbscTime);
                  break;
               case AbscSpace:
                  AbscType(AbscSpace);
                  break;
               default:
                  return PROBLEM;
            }

            REAL r1, r2; BOOLEAN b1, b2;
            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            if ( xMinMax(r1,r2) == FALSE )                     return PROBLEM;
            xMinAuto(b1);
            xMaxAuto(b2);

            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            xTick(r1,r2);
            xTickPosAuto(b1);
            xTickDistAuto(b2);

            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            if ( yMinMax(r1,r2) == FALSE )                     return PROBLEM;
            yMinAuto(b1);
            yMaxAuto(b2);

            if ( LoadNumber(in,&r1) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b1) == FALSE )                return PROBLEM;
            if ( LoadNumber(in,&r2) == FALSE )                 return PROBLEM;
            if ( LoadBoolean(in,&b2) == FALSE )                return PROBLEM;
            yTick(r1,r2);
            yTickPosAuto(b1);
            yTickDistAuto(b2);

            if ( LoadStart(in) == FALSE )                      return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               CARDINAL c; REAL r; BOOLEAN b;
               CURVE* curve = new CURVE;
               curve->Variable(varlist->Get(Buffer));
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->CalcNum(c);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               curve->Compartment(complist->Get(Buffer));
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->Zone(c);
               if ( LoadNumber(in,&r) == FALSE )               return PROBLEM;
               curve->XorT(r);
               if ( LoadBoolean(in,&b) == FALSE )              return PROBLEM;
               curve->SpaceRel(b);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               CARDINAL type = 0; CURVETYPE ct;
               while ( type < NumCurveType )
               {
                  if ( strcmp(Buffer,CurveTypeIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case CurveVal:
                     ct = CurveVal;
                     break;
                  case CurveSensErr:
                     ct = CurveSensErr;
                     break;
                  case CurveSensAbsAbs:
                     ct = CurveSensAbsAbs;
                     break;
                  case CurveSensRelAbs:
                     ct = CurveSensRelAbs;
                     break;
                  case CurveSensAbsRel:
                     ct = CurveSensAbsRel;
                     break;
                  case CurveSensRelRel:
                     ct = CurveSensRelRel;
                     break;
                  default:
                     return PROBLEM;
               }
               curve->Type(ct);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               curve->Parameter(varlist->Get(Buffer));

               if ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
               {
                  curve->Legend(Buffer);
               }

               if ( LoadBoolean(in,&b) == FALSE )              return PROBLEM;
               curve->LineActive(b);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumLineStyle )
               {
                  if ( strcmp(Buffer,LineStyleIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case LineSolid:
                     curve->LineStyle(LineSolid);
                     break;
                  case LineLongDashed:
                     curve->LineStyle(LineLongDashed);
                     break;
                  case LineDashed:
                     curve->LineStyle(LineDashed);
                     break;
                  case LineShortDashed:
                     curve->LineStyle(LineShortDashed);
                     break;
                  case LineDotted:
                     curve->LineStyle(LineDotted);
                     break;
                  case LineLongDashDotted:
                     curve->LineStyle(LineLongDashDotted);
                     break;
                  case LineShortDashDotted:
                     curve->LineStyle(LineShortDashDotted);
                     break;
               }
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->LineWidth(c);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumAquaColor )
               {
                  if ( strcmp(Buffer,AquaColorIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case AquaBlack:
                     curve->LineColor(AquaBlack);
                     break;
                  case AquaRed:
                     curve->LineColor(AquaRed);
                     break;
                  case AquaGreen:
                     curve->LineColor(AquaGreen);
                     break;
                  case AquaBlue:
                     curve->LineColor(AquaBlue);
                     break;
                  case AquaCyan:
                     curve->LineColor(AquaCyan);
                     break;
                  case AquaMagenta:
                     curve->LineColor(AquaMagenta);
                     break;
                  case AquaYellow:
                     curve->LineColor(AquaYellow);
                     break;
                  case AquaWhite:
                     curve->LineColor(AquaWhite);
                     break;
                  default:
                     return PROBLEM;
               }

               if ( LoadBoolean(in,&b) == FALSE )              return PROBLEM;
               curve->MarkActive(b);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumMarkStyle )
               {
                  if ( strcmp(Buffer,MarkStyleIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case MarkCircle:
                     curve->MarkStyle(MarkCircle);
                     break;
                  case MarkSquare:
                     curve->MarkStyle(MarkSquare);
                     break;
                  case MarkTriangle:
                     curve->MarkStyle(MarkTriangle);
                     break;
                  case MarkDiamond:
                     curve->MarkStyle(MarkDiamond);
                     break;
                  default:
                     return PROBLEM;
               }
               if ( LoadNumber(in,&c) == FALSE )               return PROBLEM;
               curve->MarkSize(c);
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
               type = 0;
               while ( type < NumAquaColor )
               {
                  if ( strcmp(Buffer,AquaColorIdentifier[type]) == 0 ) break;
                  type++;
               }
               switch ( type )
               {
                  case AquaBlack:
                     curve->MarkColor(AquaBlack);
                     break;
                  case AquaRed:
                     curve->MarkColor(AquaRed);
                     break;
                  case AquaGreen:
                     curve->MarkColor(AquaGreen);
                     break;
                  case AquaBlue:
                     curve->MarkColor(AquaBlue);
                     break;
                  case AquaCyan:
                     curve->MarkColor(AquaCyan);
                     break;
                  case AquaMagenta:
                     curve->MarkColor(AquaMagenta);
                     break;
                  case AquaYellow:
                     curve->MarkColor(AquaYellow);
                     break;
                  case AquaWhite:
                     curve->MarkColor(AquaWhite);
                     break;
                  default:
                     return PROBLEM;
               }

               if ( AddCurve(curve) == FALSE )                 return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                 return PROBLEM;
   return OK;
}


JOBSTATUS PLOT::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   if ( SaveString(out,Symbol()) == FALSE )                    return PROBLEM;

   if ( SaveString(out,Description()) == FALSE )               return PROBLEM;

   if ( SaveString(out,Title()) == FALSE )                     return PROBLEM;

   if ( SaveString(out,xTitle()) == FALSE )                    return PROBLEM;

   if ( SaveString(out,yTitle()) == FALSE )                    return PROBLEM;

   if ( AbscType() >= NumAbscType )
      FatalError("PLOT::Save: Unknown abscissa type");
   if ( SaveString(out,AbscTypeIdentifier[absctype]) == FALSE )
                                                               return PROBLEM;

   if ( SaveNumber(out,xMin()) == FALSE )                      return PROBLEM;
   if ( SaveBoolean(out,xMinAuto()) == FALSE )                 return PROBLEM;

   if ( SaveNumber(out,xMax()) == FALSE )                      return PROBLEM;
   if ( SaveBoolean(out,xMaxAuto()) == FALSE )                 return PROBLEM;

   if ( SaveNumber(out,xTickPos()) == FALSE )                  return PROBLEM;
   if ( SaveBoolean(out,xTickPosAuto()) == FALSE )             return PROBLEM;

   if ( SaveNumber(out,xTickDist()) == FALSE )                 return PROBLEM;
   if ( SaveBoolean(out,xTickDistAuto()) == FALSE )            return PROBLEM;

   if ( SaveNumber(out,yMin()) == FALSE )                      return PROBLEM;
   if ( SaveBoolean(out,yMinAuto()) == FALSE )                 return PROBLEM;

   if ( SaveNumber(out,yMax()) == FALSE )                      return PROBLEM;
   if ( SaveBoolean(out,yMaxAuto()) == FALSE )                 return PROBLEM;

   if ( SaveNumber(out,yTickPos()) == FALSE )                  return PROBLEM;
   if ( SaveBoolean(out,yTickPosAuto()) == FALSE )             return PROBLEM;

   if ( SaveNumber(out,yTickDist()) == FALSE )                 return PROBLEM;
   if ( SaveBoolean(out,yTickDistAuto()) == FALSE )            return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( CARDINAL i=0; i<numcurves; i++ )
   {
      const CURVE* curve = Curve(i);
      if ( curve->Variable() != 0 )
      {
         if ( SaveString(out,(curve->Variable())->Symbol()) == FALSE )
                                                               return PROBLEM;
      }
      else
      {
         if ( SaveString(out,"") == FALSE )                    return PROBLEM;
      }
      if ( SaveNumber(out,curve->CalcNum()) == FALSE )         return PROBLEM;
      if ( curve->Compartment() != 0 )
      {
         if ( SaveString(out,(curve->Compartment())->Symbol()) == FALSE )
                                                               return PROBLEM;
      }
      else
      {
         if ( SaveString(out,"") == FALSE )                    return PROBLEM;
      }
      if ( SaveNumber(out,curve->Zone()) == FALSE )            return PROBLEM;
      if ( SaveNumber(out,curve->XorT()) == FALSE )            return PROBLEM;
      if ( SaveBoolean(out,curve->SpaceRel()) == FALSE )       return PROBLEM;
      if ( curve->Type() >= NumCurveType )
         FatalError("PLOT::Save: Unknown curve type");
      if ( SaveString(out,CurveTypeIdentifier[curve->Type()]) == FALSE )
                                                               return PROBLEM;

      if ( curve->Parameter() != 0 )
      {
         if ( SaveString(out,(curve->Parameter())->Symbol()) == FALSE )
                                                               return PROBLEM;
      }
      else
      {
         if ( SaveString(out,"") == FALSE )                    return PROBLEM;
      }

      if ( curve->Legend() != 0 )
      {
         if ( SaveString(out,curve->Legend()) == FALSE )       return PROBLEM;
      }
      else
      {
         if ( SaveString(out,"") == FALSE )                    return PROBLEM;
      }

      if ( SaveBoolean(out,curve->LineActive()) == FALSE )     return PROBLEM;
      if ( curve->LineStyle() >= NumLineStyle )
         FatalError("PLOT::Save: Unknown line style");
      if ( SaveString(out,LineStyleIdentifier[curve->LineStyle()]) == FALSE )
                                                               return PROBLEM;
      if ( SaveNumber(out,curve->LineWidth()) == FALSE )       return PROBLEM;
      if ( curve->LineColor() >= NumAquaColor )
         FatalError("PLOT::Save: Unknown color");
      if ( SaveString(out,AquaColorIdentifier[curve->LineColor()]) == FALSE )
                                                               return PROBLEM;

      if ( SaveBoolean(out,curve->MarkActive()) == FALSE )     return PROBLEM;
      if ( curve->MarkStyle() >= NumMarkStyle )
         FatalError("PLOT::Save: Unknown mark style");
      if ( SaveString(out,MarkStyleIdentifier[curve->MarkStyle()]) == FALSE )
                                                               return PROBLEM;
      if ( SaveNumber(out,curve->MarkSize()) == FALSE )        return PROBLEM;
      if ( curve->MarkColor() >= NumAquaColor )
         FatalError("PLOT::Save: Unknown color");
      if ( SaveString(out,AquaColorIdentifier[curve->MarkColor()]) == FALSE )
                                                               return PROBLEM;

   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS PLOT::Write(std::ostream& out, BOOLEAN)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;

   if ( Print(out,ini.T(3),pos1) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;
   if ( Description() != 0 )
   {
      if ( strlen(Description()) != 0 )
      {
         if ( Print(out,Description(),pos2) == FALSE )   return PROBLEM;
      }
   }

   if ( Print(out,ini.T(2),pos1) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;
   switch ( AbscType() )
   {
      case AbscTime:
         if ( Print(out,ini.T(4),pos2) == FALSE )        return PROBLEM;
         break;
      case AbscSpace:
         if ( Print(out,ini.T(5),pos2) == FALSE )        return PROBLEM;
         break;
   }

   if ( Print(out,ini.T(7),pos1) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;
   if ( Title() != 0 )
   {
      if ( strlen(Title()) > 0 )
      {
         if ( Print(out,Title(),pos2) == FALSE )         return PROBLEM;
      }
   }

   if ( Print(out,ini.T(8),pos1) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;
   if ( xTitle() != 0 )
   {
      if ( strlen(xTitle()) > 0 )
      {
         if ( Print(out,xTitle(),pos2) == FALSE )        return PROBLEM;
      }
   }

   if ( Print(out,ini.T(9),pos1) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;
   if ( yTitle() != 0 )
   {
      if ( strlen(yTitle()) > 0 )
      {
         if ( Print(out,yTitle(),pos2) == FALSE )        return PROBLEM;
      }
   }

   if ( Print(out,ini.T(10),pos1) == FALSE )             return PROBLEM;
   if ( Print(out,":") == FALSE )                        return PROBLEM;
   if ( NumCurves() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(11),pos3) == FALSE )          return PROBLEM;
      if ( Print(out," : ") == FALSE )                   return PROBLEM;
      if ( Print(out,ini.T(12)) == FALSE )               return PROBLEM;
      if ( Print(out," [") == FALSE )                    return PROBLEM;
      if ( Print(out,ini.T(13)) == FALSE )               return PROBLEM;
      if ( Print(out,",") == FALSE )                     return PROBLEM;
      if ( Print(out,ini.T(14)) == FALSE )               return PROBLEM;
      if ( Print(out,",") == FALSE )                     return PROBLEM;
      if ( Print(out,ini.T(15)) == FALSE )               return PROBLEM;
      if ( Print(out,",") == FALSE )                     return PROBLEM;
      if ( Print(out,ini.T(16)) == FALSE )               return PROBLEM;
      if ( Print(out,"]") == FALSE )                     return PROBLEM;
      for ( CARDINAL i=0; i<NumCurves(); i++ )
      {
         switch ( Curve(i)->Type() )
         {
            case CurveVal:
               if ( Print(out,ini.T(17),pos3) == FALSE ) return PROBLEM;
               break;
            case CurveSensErr:
               if ( Print(out,ini.T(18),pos3) == FALSE ) return PROBLEM;
               break;
            case CurveSensAbsAbs:
               if ( Print(out,ini.T(19),pos3) == FALSE ) return PROBLEM;
               break;
            case CurveSensRelAbs:
               if ( Print(out,ini.T(20),pos3) == FALSE )  return PROBLEM;
               break;
            case CurveSensAbsRel:
               if ( Print(out,ini.T(21),pos3) == FALSE ) return PROBLEM;
               break;
            case CurveSensRelRel:
               if ( Print(out,ini.T(22),pos3) == FALSE ) return PROBLEM;
               break;
         }
         if ( Print(out," : ") == FALSE )                return PROBLEM;
         if ( Print(out,(Curve(i)->Variable())->Symbol()) == FALSE )
                                                         return PROBLEM;
         if ( Curve(i)->Type() != CurveVal )
         {
            if ( Print(out,"(") == FALSE )               return PROBLEM;
            if ( Print(out,(Curve(i)->Parameter())->Symbol()) == FALSE)
                                                         return PROBLEM;
            if ( Print(out,")") == FALSE )               return PROBLEM;
         }
         if ( Print(out," [") == FALSE )                 return PROBLEM;
         if ( Print(out,Curve(i)->CalcNum()) == FALSE )  return PROBLEM;
         if ( Print(out,",") == FALSE )                  return PROBLEM;
         AQCOMP* com = Curve(i)->Compartment();
         if ( Print(out,com->Symbol()) == FALSE )        return PROBLEM;
         if ( Print(out,",") == FALSE )                  return PROBLEM;
         if ( Print(out,com->ZoneName(Curve(i)->Zone())) == FALSE )
                                                         return PROBLEM;
         if ( Print(out,",") == FALSE )                  return PROBLEM;
         if ( Print(out,Curve(i)->XorT()) == FALSE )     return PROBLEM;
         if ( Curve(i)->SpaceRel() == TRUE )
         {
            if ( Print(out,",") == FALSE )               return PROBLEM;
            if ( Print(out,ini.T(26),pos3) == FALSE )    return PROBLEM;
         }
         if ( Print(out,"]") == FALSE )                  return PROBLEM;
      }
   }

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

PLOTSYS::PLOTSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start PLOTSYS::PLOTSYS()");
#endif

   lisopt = new PLOTLISOPT;
   filopt = new PLOTFILOPT;
   scropt = new PLOTSCROPT;
   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end   PLOTSYS::PLOTSYS()");
#endif
}


PLOTSYS::~PLOTSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start PLOTSYS::~PLOTSYS()");
#endif

   delete lisopt;
   delete filopt;
   delete scropt;
   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   PLOTSYS::~PLOTSYS()");
#endif
}


BOOLEAN PLOTSYS::Arg(const AQVAR* var) const
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      if ( plot->Arg(var) == TRUE ) return TRUE;
      plot = plot->Next();
   }
   return FALSE;
}


BOOLEAN PLOTSYS::Arg(const AQCOMP* com) const
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      if ( plot->Arg(com) == TRUE ) return TRUE;
      plot = plot->Next();
   }
   return FALSE;
}


void PLOTSYS::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      plot->ReplaceVar(oldvar,newvar);
      plot = plot->Next();
   }
   return;
}


void PLOTSYS::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      plot->ExchangeVar(var1,var2);
      plot = plot->Next();
   }
   return;
}


void PLOTSYS::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      plot->ReplaceComp(oldcomp,newcomp);
      plot = plot->Next();
   }
   return;
}


BOOLEAN PLOTSYS::Add(PLOT* plot)
{
   if ( plot == 0 )                     return FALSE;
   if ( Exist(plot->Symbol()) == TRUE ) return FALSE;
   if ( plotlist.Add(plot) == FALSE )
      FatalError("PLOTSYS::Add: cannot add plot");
   return TRUE;
}


BOOLEAN PLOTSYS::Replace(PLOT* oldplot, PLOT* newplot)
{
   if ( newplot == 0 )                                        return FALSE;
   if ( Exist(oldplot) == FALSE )                             return FALSE;
   if ( ( strcmp(oldplot->Symbol(),newplot->Symbol()) != 0 )
        &&             ( Exist(newplot->Symbol()) == TRUE ) ) return FALSE;
   if ( plotlist.Add(newplot) == FALSE )
      FatalError("PLOTSYS::Replace: cannot insert new plot");
   if ( plotlist.Remove(oldplot) == FALSE )
      FatalError("PLOTSYS::Replace: cannot remove old plot");
   return TRUE;
}


BOOLEAN PLOTSYS::Delete(PLOT* plot)
{
   if ( plot == 0 )   return FALSE;
   BOOLEAN b = plotlist.Remove(plot);
   if ( b == FALSE ) return FALSE;
   delete plot;
   return TRUE;
}


void PLOTSYS::Delete()
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      if ( plotlist.Remove(plot) == FALSE )
         FatalError("PLOTSYS::Delete: unable to remove plot");
      delete plot;
      plot = plotlist.First();
   }
}


void PLOTSYS::DeleteCurves(const AQVAR* var)
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      plot->DeleteCurves(var);
      plot = plot->Next();
   }
   return;
}


void PLOTSYS::DeleteCurves(const AQCOMP* com)
{
   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      plot->DeleteCurves(com);
      plot = plot->Next();
   }
   return;
}


BOOLEAN PLOTSYS::LisOpt(PLOTLISOPT* opt)
{
   if ( opt == 0 ) return FALSE;

   if ( opt == lisopt )
      FatalError("PLOTSYS: LisOpt: illegal argument");
   delete lisopt;
   lisopt = opt;
   lisopt->locked = TRUE;
   return TRUE;
}


CARDINAL PLOTSYS::FilOpt(PLOTFILOPT* opt)
{
   int invalid=4;
   if ( opt == 0 ) return invalid;

   // test of legality:
   invalid=PTS::ValidAxes(opt);
   if (invalid) return invalid;

   delete filopt;
   filopt = opt;
   filopt->locked = TRUE;
   filopt->newpage = TRUE;
   return invalid;
}


CARDINAL PLOTSYS::ScrOpt(PLOTSCROPT* opt)
{
   int invalid=4;
   const CARDINAL minplotsize = 40;
   if ( opt == 0 ) return invalid;

   // test of legality:  not possible for aquasimc !!!
#if AQUA_VERSION==AQUA_XVT
   invalid=AquaPlotWin::ValidLayout(opt);
   if (invalid) return invalid;
   delete scropt;
   scropt = opt;
   scropt->locked = TRUE;
#else                     // provisorische Loesung fuer NT:
   invalid = 0;
   if ( opt->WinWidth()  <   opt->BorderLeft()
                           + opt->BorderRight()
                           + opt->WidthLegend()
                           + minplotsize )
   {
      invalid = invalid + 1;
   }
   if ( opt->WinHeight() <   opt->BorderTop()
                           + opt->BorderBottom()
                           + minplotsize )
   {
	  invalid = invalid + 2;
   }
   if (invalid) return invalid;
   delete scropt;
   scropt = opt;
   scropt->locked = TRUE;
#endif
   return invalid;
}


JOBSTATUS PLOTSYS::Load(std::istream& in,
                        const VARLIST* varlist,
                        const COMPLIST* complist)
{
   char Buffer[BufSize];
   // Delete();

   // read identifier:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,PlotsysIdentifier) != 0 )      return PROBLEM;
   if ( LoadStart(in) == FALSE )                        return PROBLEM;

   // read options:

   JOBSTATUS status;
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,PlotLisOptIdentifier) != 0 )   return PROBLEM;
   status = lisopt->Load(in);
   if ( status != OK )                                  return status;

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,PlotFilOptIdentifier) != 0 )   return PROBLEM;
   status = filopt->Load(in);
   if ( status != OK )                                  return status;

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,PlotScrOptIdentifier) != 0 )   return PROBLEM;
   status = scropt->Load(in);
   if ( status != OK )                                  return status;

   // read plots:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      if ( strcmp(Buffer,PlotIdentifier) != 0 ) return PROBLEM;
      PLOT* plot = new PLOT;
      JOBSTATUS status = plot->Load(in,varlist,complist);
      if ( status != OK )
      {
         delete plot; return status;
      }
      if ( Add(plot) == FALSE )
      {
         delete plot; return PROBLEM;
      }
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )              return INTERRUPT;
      }
   }
   return OK;
}


JOBSTATUS PLOTSYS::Save(std::ostream& out)
{
   if ( SaveString(out,PlotsysIdentifier) == FALSE )    return PROBLEM;
   if ( SaveStart(out) == FALSE )                       return PROBLEM;

   JOBSTATUS status;
   if ( SaveString(out,PlotLisOptIdentifier) == FALSE ) return PROBLEM;
   status = lisopt->Save(out);
   if ( status != OK )                                  return status;
   if ( SaveString(out,PlotFilOptIdentifier) == FALSE ) return PROBLEM;
   status = filopt->Save(out);
   if ( status != OK )                                  return status;
   if ( SaveString(out,PlotScrOptIdentifier) == FALSE ) return PROBLEM;
   status = scropt->Save(out);
   if ( status != OK )                                  return status;

   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      if ( SaveString(out,PlotIdentifier) == FALSE )    return PROBLEM;
      JOBSTATUS status = plot->Save(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                     return INTERRUPT;
      }
      plot = plot->Next();
   }

   if ( SaveEnd(out) == FALSE )                         return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS PLOTSYS::Write(std::ostream& out, BOOLEAN)
{
   if ( plotlist.Size() == 0 )               return OK;

   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )          return PROBLEM;
   if ( Print(out,PLOT::ini.T(6)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )          return PROBLEM;

   PLOT* plot = plotlist.First();
   while ( plot != 0 )
   {
      JOBSTATUS status = plot->Write(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )          return INTERRUPT;
      }
      plot = plot->Next();
      if ( plot != 0 ) PrintLn(out,'-');
      else             PrintLn(out,'*');
   }

   if ( PrintLn(out) == FALSE )              return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

