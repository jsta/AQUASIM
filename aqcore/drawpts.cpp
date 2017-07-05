///////////////////////////////   drawpts.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.05.93    Juerg Ruchti
// revisions:      
//
//////////////////////////////////////////////////////////////////////////////
// CONTENTS
// ========
// A. Initialization of static variables used by static ValidAxes:
//    Error handling
// B. DEFINE PAGE AND FRAME
//    1. Constructors, 2. SetEPS
//    3. Page Layout
//    4., Frame Layout, 5. Fonts 6. Precision: inline functions in class PTS
//    7. Validation
//    8. Start File
// C. REPEAT FRAME
//    1. Start a Frame
//    2. Draw Axes, Label Frame
//    3. Set Colour, Draw Curves
//    4. Print Page
// D. END JOB: see C.4.
// POSTSCRIPT: Origins, and Scaling
// POSTSCRIPT: Write File Header
// POSTSCRIPT: Procedures writing PostScrip Code to file
// POSTSCRIPT: Write PostScript definitions
//////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <ctype.h>

#include "drawpts.h"

//////////////////////////////////////////////////////////////////////////////
// A. Initialization of static variables used by static ValidAxes:
//////////////////////////////////////////////////////////////////////////////
static char atitle[128];
static char aversion[128];
static char acreator[128];
static char acreatedate[128];

REAL ptspage::pix=2.54/72.0;
REAL ptspage::DIST=2.5;
REAL ptspage::FTIT=0.2;
REAL ptspage::OTITCORR=2;
REAL ptspage::ATIT=0.5;
REAL ptspage::YXN=-3;
REAL ptspage::MINAXLEN=1;
int ptspage::PREC=3;

//////////////////////////////////////////////////////////////////////////////
//    Error handling
//////////////////////////////////////////////////////////////////////////////

static void FrameError(const char* s,const char* ss) {
   char c[255];
   
   strcpy(c,s);
   strcat(c,ss);
   FatalError(c);
}

static void paterror(const char* p) {
	std::cout << "Illegal pattern: " << p << "\n";
	std::cout.flush();
   FatalError("Implementation error in call to PTS::plotdashes:\n");
}

void ptspage::error(const char* from, const char* help) {
   char Buffer[256];
   strcpy(Buffer,"ptspage: call ");
   strcat(Buffer,help);
   strcat(Buffer," before calling ");
   strcat(Buffer,from);
   FatalError(Buffer);
}

//////////////////////////////////////////////////////////////////////////////
// B. DEFINE PAGE AND FRAME
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//    1. Constructors, Set EPS
//////////////////////////////////////////////////////////////////////////////

// Frame Constructor PTS
PTS::PTS() {ptsfile=0;  isEPS=FALSE; }
PTS::PTS(std::ifstream& oldfile, std::ofstream& outfile) {
/* This constructor is not yet mature...   
   startfile(oldfile);
   startfile(outfile);
		let's suppress compiler warnings:
*/
  oldfile.close(); outfile.close();
  isEPS=FALSE;
}
    
CARDINAL PTS::ValidAxes(BOOLEAN portrait,       // test of frame parameters:
		REAL papwidth,                  // Results: result, abszissa, ordinate
		REAL papheight,                 // result = 0: MINAXLEN<=abszissa, MINAXLEN<=ordinate
		REAL bleft, REAL bright,        // result = 1: abszissa<MINAXLEN , MINAXLEN<=ordinate
		REAL bbottom, REAL btop,        // result = 2: MINAXLEN<=abszissa, ordinate<MINAXLEN
		REAL fsaxes,                    // result = 3: abszissa<MINAXLEN,  ordinate<MINAXLEN
		REAL fsnumb,
		REAL wlegend,
		int ncol,
		int nrow,
		REAL fstitle,
		REAL& abszissa,
		REAL& ordinate) {
    CARDINAL result=0;
    REAL r;
    REAL px=papwidth - bleft - bright;
    REAL py=papheight - bbottom - btop;
    if (!portrait) {
       r=py; py=px; px=r;
    }
    int nfr=ncol * nrow;
    
    REAL ordspace=(fsaxes
		   + fsnumb * (PREC + OTITCORR) // precision + exponent=OTITCORR
		   - YXN) * pix;
    abszissa=(px - ncol * (ordspace+wlegend)) / ncol;
    if (abszissa < MINAXLEN) result=1;
    
    REAL titspace=FTIT + (fstitle + 2) * pix;
    ordinate=(py - nrow * (DIST + ATIT + titspace) + DIST) / nrow;
    if (ordinate < MINAXLEN) result=result + 2;
    return result;
}

void ptspage::SetEPS(BOOLEAN eps) {
	isEPS=eps; 
	if (eps) state=0; 
}

// Page Constructor ptspage
ptspage::ptspage() {

// static:    FTIT=0.2;
// static:    ATIT=0.5;
// static:    YXN=-3;
// static:    MINAXLEN=1;
// static:    PREC=3;
// static:    OTITCORR=2;
// static:    pix=2.54/72.0;
    cmf=72.0/2.54;
    AQ_TICK=4;
    XYN=-8;
    OTIT=0;
    FSTIT=12;
    FSAXES=10;
    FSNUMB=6;
    FSLEGEND=6;
    first_open_in_job=TRUE;
    state=0;
    paper(21.0,29.7);
    papermargins(1.0,1.0,1.5,1.0,   0.5);
    nper=1;
    curxscale=1;
    curyscale=1;
    lndscape=FALSE;
    frames(2,2.5,3,DIST);
    print_withcolours=TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//    3. Page Layout
//////////////////////////////////////////////////////////////////////////////

void ptspage::paper(REAL width,
	   REAL height) {
  if (1 < state) { error("ptspage::paper","PTS::print"); return; }
  pw=width;
  ph=height;
}
	   
void ptspage::papermargins(REAL xleft, REAL xright,
		  REAL ydown, REAL yup, REAL progidenty) {
   if (1 < state) { error("ptspage::papermargins","PTS::print"); return; }
   pxl=xleft; pxr=xright; pyd=ydown; pyu=yup;
   pyid=progidenty;
}

void ptspage::portrait() {
   if (1 < state) { error("ptspage::portrait","PTS::print"); return; }
   lndscape=FALSE;
}

BOOLEAN ptspage::Portrait()
{
   if ( lndscape == TRUE ) return FALSE;
   return TRUE;
}

void ptspage::landscape() {
   if (1 < state) { error("ptspage::landscape","PTS::print"); return; }
   lndscape=TRUE;
}
void ptspage::frames(int horizontal, REAL horidist,
	      int vertical, REAL vertdist) {
   horiframes=horizontal; FLEG=horidist;
   vertframes=vertical;   DIST=vertdist;
   nframes=horiframes * vertframes;
   curframes=0;
}

//////////////////////////////////////////////////////////////////////////////
//    4., Frame Layout, 5. Fonts 6. Precision: inline functions in class PTS
//
//    7. Validation
//////////////////////////////////////////////////////////////////////////////
CARDINAL PTS::ValidAxes(PLOTFILOPT* opt) {
   REAL absDummy, ordDummy;
   CARDINAL result;
   result=ValidAxes(opt->Portrait(),
	     opt->PaperWidth(),
	     opt->PaperHeight(),
	     opt->BorderLeft(),
	     opt->BorderRight(),
	     opt->BorderBottom(),
	     opt->BorderTop(),
	     opt->FontAxes(),
	     opt->FontNumber(),
	     opt->WidthLegend(),
	     opt->NumCol(),
	     opt->NumRow(),
	     opt->FontTitle(),
	     absDummy,
	     ordDummy);
   return result;
}

//////////////////////////////////////////////////////////////////////////////
//    8. Start File
//////////////////////////////////////////////////////////////////////////////

/* version made compatible for the mac PR 27.03.95: */

static int copy_upto(const char* mark,
					 std::ifstream& oldfile,
					 std::ofstream& outfile) {
   int i, lp, ap;
   const int buflen=512;
   const int safebuf=buflen/2;
   char buf[buflen];
   char ch;
   int nch=0;
   char* pat;
   char* pch;
   int mlen=strlen(mark);
   BOOLEAN endloop = FALSE;
   
   while ( 1 )
   {
      lp=0;
      for ( i=0; i<buflen; i++ ) { buf[i]='\0'; }
      do
      {
#if AQUA_OS==AQUA_MAC
         ch = oldfile.get();
         if ( oldfile.fail() )      // does not work on the Sun!
         {
            endloop = TRUE;
            break;
         }
#else
         if ( ! oldfile.get(ch) )   // does not work on the Mac!
         {
            endloop = TRUE;
            break;
         }
#endif
         buf[lp]=ch;
         lp++;
		 if (nch==18) { // detect EPS file
			 // check indices with Adobe header 
			 // defined in ptspage::PrintPSHeader()
			 if (buf[15]=='E' && buf[16]=='P'&& buf[17]=='S' &&
				 buf[18]=='F') {
				 return -2;
			 }
		 }
         nch++;
      } while ( ch>=' ' && lp<safebuf ); // read to ctrl char or full buffer
      if ( strlen(mark) < strlen(buf) ) pat=strstr(buf,mark);  // find mark
      else                              pat = 0;
      if ( pat == 0 )
      {
         pat = &buf[lp];
      }
      else
      {
         if (mlen+2 < strlen(pat)) pat=&buf[lp+1];
      }
      ap = 0;
      pch = &buf[0];
      while (ap < lp)
      {
         if (pch != pat)                 // copy up to mark
         {
            outfile << buf[ap];
            ap++;
            pch++;
         }
         else
         {
            outfile << "% FRAME APPENDED ";
            for (i=0; i < mlen; i++)
            {
               outfile << ToUpper(buf[ap]);
               ap++;
            }
            pch++;
            outfile << " % END COMMENT\n\n";
            return 1;	                // mark found and outcommented
         }
      }
      if ( endloop == TRUE ) break;
   }
   //  nch == -2: see return above		// EPS file found
   if (nch == 0) return -1;             // empty file
   if (pat == &buf[lp]) return 0;       // no mark found
   if (pat == &buf[lp+1]) return 2;     // mark found and not changed
   FatalError("drawpts.C: copy_upto: illegal case"); return -2;			
}

					 int ptspage::startfile(std::ifstream& oldfile,
						 std::ofstream& outfile,
			const char* title,
			const char* date,
			const char* creator,
			const char* version) {
//		      const char* text) {
    int showfound;
    
	strcpy(atitle,title);
	strcpy(acreatedate,date);
	strcpy(acreator,creator);
    strcpy(aversion,version);
    ptsfile=&outfile;
    showfound=copy_upto("showpage",oldfile,outfile);
	if (showfound == -2) return showfound;	// EPS file found
    if (-1 < showfound) {
   
       if (showfound == 1) {
	  if (curframes) {
	     *ptsfile << "%-------\n";
	     newscale(cmf,cmf);
	  }
	  else line("showpage % [page break because of new aquasim job");
       }
    }
    else {
		PrintPSHeader();
    }
    
   return 0;
}

//////////////////////////////////////////////////////////////////////////////
// C. REPEAT FRAME
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//    1. Start a Frame
//////////////////////////////////////////////////////////////////////////////

void PTS::startframe() {
   int row, column, n;
   REAL titspace;
   
   
   if (!state) start();
   if (state == 2) {
     oldorig();
   }
	       
   first_open_in_job=FALSE;
   OTIT=(FSAXES
	 + FSNUMB * (NPrecision() + OTITCORR) // precision + exponent=OTITCORR
	 - YXn()) * PIX();
   fx=(px - horiframes * (OTIT+FLEG)) / horiframes;
   period=1/nper;
   column=curframes % horiframes;
   fx0=column * (fx + OTIT + FLEG) + OTIT;
   
   titspace=FTIT + (FSTIT+2) * PIX();
   fy=(py - vertframes * (DIST + ATIT + titspace) + DIST) / vertframes;
      //                                top frame has no DIST before it
      //                             bottom frame has    ATIT after it
   n=nframes -1 - (curframes % nframes);
   row=n / horiframes;
   fy0=row * (fy + DIST + ATIT + titspace) + ATIT;
   *ptsfile << "   % Start Frame " << curframes << "\n";

   if (fy < MINAXLEN) {
      *cjob << "\n\nPlot to File:\n\tToo many Rows... \n";
   }
   if (fx < MINAXLEN) {
      *cjob << "\n\nPlot to File:\n\tToo many Columns ? Too big Legend ?\n";
   }

   neworig(fx0,fy0);
   line(oldxscale(), "setlinewidth");
   line(0,fy,fx,0, "squareshape stroke"); nl();
   curframes++;
   state=2;
}

//////////////////////////////////////////////////////////////////////////////
//    2. Draw Axes, Label Frame
//////////////////////////////////////////////////////////////////////////////

void PTS::axis_ticks(BOOLEAN on_X_axis,
		     REAL rmin, REAL rmax, REAL pos, REAL dist) {
    REAL fact, delta, tick;
    REAL ticklen=Tick() * PIX(); // pts -> cm units, length of tick
    REAL Yabsnumb=YXn() * PIX(); // pts -> cm units, Y of ordinate numbers
    REAL Xordnumb=XYn() * PIX(); // pts -> cm units, X of abszissa numbers
    
    line(FSnumb(),"cmtimes");    // size, font for axes numbers
 
    dist=fabs(dist);
    if (dist == 0.0) dist=(rmax-rmin)/2.0;
    if (dist < (rmax-rmin)) delta=dist; else delta=rmax=rmin; delta=delta/10000;
	
    while (pos < rmax+delta) pos=pos+dist;
    pos=pos-dist;

    if (on_X_axis) fact=xmax()/(rmax-rmin);
	    else fact=ymax()/(rmax-rmin);
	    
    rmax=pos-rmin;
    while (rmin-delta < pos) {
       if (fabs(pos) < delta) tick=0; else tick=pos;
       if (on_X_axis) {                                 // abscissa
	  word(0,- ticklen,  rmax*fact, 0.0, "tic ");
		// rlineto       moveto
		
	  axis(rmax*fact,Yabsnumb,tick);
       }
       else {                                           // ordinate
	  word(-ticklen,0.0, 0.0, rmax*fact, "tic ");
		// rlineto       moveto
		
	  axis(Xordnumb,rmax*fact - pix*0.4*FSNUMB,tick, -1);
//          axis(Xordnumb,rmax*fact,tick, -1);
       }
       pos=pos-dist; rmax=rmax-dist;
       
    }
}

void PTS::titframe(const char* name) {
   line(FSTIT,"cmbtimes");
   number(fx/2,fy+FTIT); psstr(name); line("ctext");
}

void PTS::titabs(const char* name) {
   line(FSAXES,"cmtimes");
   number(fx/2,-ATIT); psstr(name); line("ctext");
}

void PTS::titord(const char* name) {
   line(FSAXES,"cmtimes");
   number(-OTIT + 0.4 + FSAXES*pix,fy/2); psstr(name); line("vctext");
//               >===< on request of PR
}
//////////////////////////////////////////////////////////////////////////////
//    3. Set Colour, Draw Curves
//////////////////////////////////////////////////////////////////////////////

void PTS::setcolour(int colour) {
 if (!print_withcolours) colour=99;
 switch(colour) {
   case 0: line(" rgbblack ");
      break;
   case 1: line(" rgbred ");
      break;
   case 2: line(" rgbgreen ");
      break;
   case 3: line(" rgbblue ");
      break;
   case 4: line(" rgbcyan ");
      break;
   case 5: line(" rgbmagenta ");
      break;
   case 6: line(" rgbyellow ");
      break;
   case 7: line(" rgbwhite ");
      break;
   default: line(" rgbblack ");
      break;
 }
}

static REAL dist_to_next(REAL delx, REAL dely) {
  if (delx) return sqrt(delx*delx+dely*dely);
  else return dely;
}
static void find_next(REAL& delx, REAL& dely, REAL target, REAL limit,
		      REAL& Delx, REAL& Dely) {
  REAL f;
  f=target/limit;
  Delx=f*delx;
  Dely=f*dely;
}

static const char* const PenDownUp[]={"aql","aqm"};

void PTS::plotline(RealVec* x, RealVec* y, REAL width, int color,
		     BOOLEAN clip,
		     const char* pattern) {
 if (0 == strlen(pattern)) {
    plot1line(x, y, width, color, clip);
    return;
 }
 int n=x->n();
 int i;         // workpoint; also aux var;
 BOOLEAN newi;
 int iout;      // used for sensing linefeeds
 int j;         // aux var
 int patlen=strlen(pattern); // symbolic representation of pattern
 REAL* PAT=new REAL[patlen+1];  // to contain lengths of pattern
 int ipat;      // pointer to PAT
 int iaction;   // (if ipat % 2) then pendown else penup...
 REAL limit,    // distance to next point
      target;   // distance to end of pattern
 REAL eps=period;
 BOOLEAN newpat;
 REAL Xdel,Ydel;// distance from workpoint to next neighbour
 REAL X0,Y0;    // workpoint <= local origin 
 REAL X, Y;     // coordinates of target
 
 int debuglimit=0; // set debuglimit to inexvalue up to which you want debugging output
		   // note: you should redirect output to some dummy file
 
 
 if (state < 2) { error("PTS::plotline","PTS::startframe"); return; }
 
 // analyse pattern
    REAL sum=0.0;
    for (i=0; i < patlen; i++) {
       j=pattern[i] - '0';
       if (j < 0) paterror(pattern);
       sum=sum+j;
       PAT[i]=j;
    }
    for (i=0; i < patlen; i++) {
       PAT[i]=period * PAT[i]/sum;
       if (PAT[i] < eps) eps=PAT[i];
    }
    eps=0.00001 * eps;          // you would'nt beleive it!

// setup PostScript environment
    word(" gsave ");
    setcolour(color);
    line(width,cmf,"div setlinewidth"); 
    if (clip) line(0,fy,fx,0, "squareshape clip");
    word(" newpath ");
    line(x->v(0),y->v(0)," aqm ");
// draw
    i=0;
    X0=x->v(i); Xdel=x->v(i+1) - X0;
    Y0=y->v(i); Ydel=y->v(i+1) - Y0;
    limit=dist_to_next(Xdel,Ydel);
    newi=FALSE;
    ipat=0; iout=0; iaction=ipat % 2;
    target=PAT[ipat];
    find_next(Xdel,Ydel,target,limit,X,Y);
	if (i < debuglimit) std::cout << "x/y[" << i << "]:" << x->v(i) << " - " << y->v(i) << " limit:" << limit <<" target:" << target << "\n";
    newpat=FALSE;
//
    while (TRUE) {              // (i + 1 < n) 
      if (limit <= target) {
	 if (iout % 3 == 2) nl(); iout++;       // linefeed
	 target=target - limit;
	 if (target < eps) newpat=TRUE;
	 newi=TRUE;
      } // end limit < target
      else {            // target < limit
	 if (limit < eps) {
	    newi=TRUE;
	 }
	 else {
	    find_next(Xdel,Ydel,target,limit,X,Y);
	    word(X0 + X,Y0 + Y, PenDownUp[iaction]);
		if (i < debuglimit) std::cout << "x/y[" << i << "]:" << x->v(i) << " - " << y->v(i) << " limit:" << limit <<" target:" << target << "\t-PTS:" << X0+X << " - " << Y0+Y << " "<<PenDownUp[iaction] << "\n";
	    if (iout % 3 == 2) nl(); iout++;    // linefeed
	    X0=X0+X; Xdel=x->v(i+1) - X0;
	    Y0=Y0+Y; Ydel=y->v(i+1) - Y0;
	    limit=dist_to_next(Xdel,Ydel);
	    newpat=TRUE;
	 }
      } // end target <= limit
      if (newi == TRUE) {
	 i++;
     if (i + 1 < n) {   // changed 17.04.2003
//	 if (i < n) {
	    X0=x->v(i); Xdel=x->v(i+1) - X0;
	    Y0=y->v(i); Ydel=y->v(i+1) - Y0;
	    if (iaction == 0) {
	       word(X0,Y0, PenDownUp[iaction]);
		   if (i < debuglimit) std::cout << "x/y[" << i << "]:" << x->v(i) << " - " << y->v(i) << " limit:" << limit <<" target:" << target << "\t=PTS:" << X0 << " - " << Y0 << " "<<PenDownUp[iaction] << "\n";
	    }
	    limit=dist_to_next(Xdel,Ydel);
		if (i < debuglimit && iaction) std::cout << "x/y[" << i << "]:" << x->v(i) << " - " << y->v(i) << " limit:" << limit <<" target:" << target << "\t= newi\n\n";
	    newi=FALSE;
	    if (i + 1 == n) break;
	 }
	 else break;
      }
      if (newpat == TRUE) {
	 ipat=++ipat % patlen;
	 target=PAT[ipat];
	 iaction=ipat % 2;
	 if (i < debuglimit) std::cout << "i=" << i <<"\ttarget = " << target << " -> "<<PenDownUp[iaction] << "\tnewpat\n";
	 newpat=FALSE;
      }
    } // end while i + 1 < n
// draw line
    line(" stroke grestore");
	if (0 < debuglimit) std::cout << "----------- " << n << " points plotted ----------------\n";
}

void PTS::plot1line(RealVec* x, RealVec* y,REAL width, int color, BOOLEAN clip) {
 int n=x->n();
 int i;
 
 if (state < 2) { error("PTS::plot1line","PTS::startframe"); return; }

 word(" gsave ");
 setcolour(color);
 line(width,cmf,"div setlinewidth"); 
 if (clip) line(0,fy,fx,0, "squareshape clip");
 word(" newpath ");
 line(x->v(0),y->v(0)," aqm ");
 
 for (i=1;i < n; i++) {
    if (i % 3 == 2) nl();
    word(x->v(i),y->v(i), " aql");
 }
 line(" stroke grestore");
}

REAL PTS::symbol_size_factor=0.5;

void PTS::plotsymbols(RealVec* x, RealVec* y,
		      int symbol, int size, int color, BOOLEAN clip) {
 int n=x->n();
 int i;
 if (state < 2) { error("PTS::plotsymbols","PTS::startframe"); return; }

 REAL rsize=size * symbol_size_factor;
 word(" gsave "); 
 setcolour(color);
 line(1,cmf,"div setlinewidth"); 
 if (clip) line(0,fy,fx,0, "squareshape clip");
 word(" newpath ");
 for (i=0;i < n; i++) {
    if (i % 3 == 2) nl();
    number(x->v(i),y->v(i),rsize/cmf);
    switch(symbol) {
      case -1: word(0.5,"mul"); word(" sglpnt ");
//      case -1: word(" sglpnt ");
	break;
      case 0: word(" circle ");
	break;
      case 1: word(" square ");
	break;
      case 2: word(" uptriangle ");
	break;
      case 3: word(" diamond ");
	break;
      default: word(" circle ");
	break;
    }
 }
 line(" stroke grestore");
}

void PTS::plotsymbol(REAL x, REAL y, int symbol, int size, int color, BOOLEAN clip) {
 if (state < 2) { error("PTS::plotsymbol","PTS::startframe"); return; }

 REAL rsize=size * symbol_size_factor;
 word(" gsave "); 
 setcolour(color);
 line(1,cmf,"div setlinewidth"); 
 if (clip) line(0,fy,fx,0, "squareshape clip");
 word(" newpath ");
    number(x,y,rsize/cmf);
    switch(symbol) {
      case 0: word(" circle ");
	break;
      case 1: word(" square ");
	break;
      case 2: word(" uptriangle ");
	break;
      case 3: word(" diamond ");
	break;
      default: word(" circle ");
	break;
    }
 line(" stroke grestore");
}

//////////////////////////////////////////////////////////////////////////////
//    4. Print Page
//////////////////////////////////////////////////////////////////////////////

BOOLEAN ptspage::print(BOOLEAN removable) {

  if (state < 2 && removable) { error("ptspage::print","PTS::startframe"); return FALSE; }
  oldorig();
  oldscale();
  if (isEPS) {
	state=0;
  }
  else {
	word(0,-pyid*cmf,"aqm");
	//  word(0,(-pyid-OTIT+2)*cmf,"aqm");
	word(" /Times-Roman findfont");
	word(" 6 scalefont setfont (");
	word(acreator);
	word(" - ");
	word(aversion);
	word(": ");
	word(acreatedate);
	line(") show 0 0 aqm");
  }
  word(" showpage");
  if (removable && (curframes % nframes)) removable=TRUE; else removable=FALSE;
  if (!removable) {
     comment("compulsory");
     curframes=nframes * ((curframes+nframes-1) / nframes);
     state=0;
  }
  nl();
  ptsfile->flush();
  if (curframes) return TRUE;
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// POSTSCRIPT: Origins, and Scaling
//////////////////////////////////////////////////////////////////////////////

void ptspage::start() {
  REAL r;
  if (state) { error("ptspage::start","PTS::frames"); return; }
  state=1;
  line("%%Page:?");
  newscale(cmf,cmf);
  line(oldxscale(), "setlinewidth");
  if (isEPS) line(epsLeft,epsTop,epsRight,epsBottom,"squareshape stroke");
  line(pxl,pyd,"translate");
  px=pw - pxl - pxr;
  py=ph - pyd - pyu;
  if (lndscape) {
     line(px,0,"translate");
     line(90, "rotate");
     r=py; py=px; px=r;
  }
  neworig(0,0);
  nl();
}

void ptspage::oldorig() {
   line(-curxorig,-curyorig,"translate");
   curxorig=0.0;
   curyorig=0.0;
}

void ptspage::neworig(REAL xorig, REAL yorig) {
   curxorig=xorig;
   curyorig=yorig;
   line(curxorig,curyorig,"translate");
}

void ptspage::oldscale() {
   line(oldxscale(),oldyscale(),"scale");
   curxscale=curxscale * oldxscale();
   curyscale=curyscale * oldyscale();
}

void ptspage::newscale(REAL xscale, REAL yscale) {
   curxscale=curxscale * xscale;
   curyscale=curyscale * yscale;
   line(xscale,yscale,"scale");
}

//////////////////////////////////////////////////////////////////////////////
// POSTSCRIPT: Write File Header
//////////////////////////////////////////////////////////////////////////////


void ptspage::PrintPSHeader() {
		if (isEPS) {
			REAL w=(pw - pxl - pxr)/horiframes;
			REAL h=(ph - pyd - pyu - (vertframes - 1) * DIST) / vertframes;
			epsLeft=pxl;
			epsTop=ph - pyu;
			epsRight=epsLeft + w + DIST/4;
			epsBottom=epsTop - h - PIX() * OTITCORR;

			line("%!PS-Adobe-3.0 EPSF-3.0");
			word("%%BoundingBox: ");
			number(CM()*epsLeft, CM()*epsBottom); 
			number(CM()*epsRight, CM()*epsTop);
			nl();
		}
		else line("%!PS-Adobe-3.0");

//		if (aversion) { ptsfile->clear();
		ptsfile->clear();
//			*ptsfile << "%%Title: " << aversion; nl();
		*ptsfile << "%%Title: " << atitle; nl();
		*ptsfile << "%%CreationDate: " << acreatedate; nl();
		*ptsfile << "%%Creator: " << acreator; nl();
		*ptsfile << "%%Version: " << aversion; nl();
		*ptsfile << "%%Copyright: " << "P. Reichert & J. Ruchti"; nl();
		*ptsfile << "%%EndComments"; nl();
//		}
		ptsdefs();
		first_open_in_job=TRUE;
		state=0;
}

//////////////////////////////////////////////////////////////////////////////
// POSTSCRIPT: Procedures writing PostScrip Code to file
//////////////////////////////////////////////////////////////////////////////

static REAL pts_dbl_max=1.0e+31;   // PostScript does not like big numbers

REAL ptspage::fit(REAL r) {        // PostScript does not like big numbers
   if (r < 0) {
      if (-pts_dbl_max < r) return r;
      else return -pts_dbl_max + 1;
   }
   else if (r < pts_dbl_max) return r;
   else return pts_dbl_max -1;
}

void ptspage::nl() {
   ptsfile->clear();
   *ptsfile << "\n"; 
   if (!ptsfile->good())  std::cout << "NL() BAD"; std::cout.flush();
   ptsfile->flush();
}

void ptspage::comment(char* c) {
   word(" %");
   line(c);
}
void ptspage::number(REAL r) {
   ptsfile->clear();
   *ptsfile << " " << fit(r);
}

void ptspage::number(REAL x, REAL y) {
   ptsfile->clear();
   *ptsfile << " " << fit(x);
   *ptsfile << " " << fit(y);
}
void ptspage::number(REAL x, REAL y, REAL z) {
   ptsfile->clear();
   *ptsfile << " " << fit(x);
   *ptsfile << " " << fit(y);
   *ptsfile << " " << fit(z);
}

void ptspage::axis(REAL x, REAL y, REAL numb, int pos) {
	std::ostrstream buffer;
   buffer.precision(PREC);
   buffer << numb << '\0';
   char* string = buffer.str();
   number(x,y); psstr(string);
   if (pos < 0) line(" rtext");
   else if (!pos) line(" ctext");
   else std::cout << "\tERROR ptspage::axis: right axis text not yet implemented\n";
   delete string;
   return;
}

void ptspage::psstr(const char* s) { // write postscript string
   const int PTSBUFLEN = 250;
   char PTSBUFFER[PTSBUFLEN];
   char* ptsstrbuff=&PTSBUFFER[0];

   int i;
   int e=PTSBUFLEN -1;
   int L=strlen(s);
   int l=0;
   for (i=0; i < L; i++) {
      switch(s[i]) {
	case  '(':  // prepend '\\' to: '(' ')' '\\'
	case  ')':
	case '\\':  PTSBUFFER[l]='\\'; l++;
	 break;
	default:
	 break;
      }
      PTSBUFFER[l]=s[i];
      l++;
      PTSBUFFER[l]=0;
      if (e <= l) i=L; // buffer overflow
   }

   ptsfile->clear();
   *ptsfile << " (" << ptsstrbuff << ") ";
}

void ptspage::word(const char* s) {
   ptsfile->clear();
   *ptsfile << s;
   if (!ptsfile->good())  std::cout << "WORD BAD:" << s << "\n"; std::cout.flush();
}

void ptspage::word(REAL x, const char* s) {
   ptsfile->clear();
   *ptsfile << " " << fit(x) << " "  << s;
}

void ptspage::word(REAL x, REAL y, const char* s) {
   ptsfile->clear();
   *ptsfile << " " << x << " " << y << " " << s;
}

void ptspage::word(REAL x, REAL y, REAL w, const char* s) {
   ptsfile->clear();
   *ptsfile << " " << fit(x) << " " << fit(y) << " " << w
	    << " " << s;
}

void ptspage::word(REAL x, REAL y, REAL w, REAL z, const char* s) {
   ptsfile->clear();
   *ptsfile << " " << fit(x) << " " << fit(y) << " " << fit(w) << " " << fit(z)
	    << " " << s;
}

void ptspage::line(const char* s) {
   ptsfile->clear();
   *ptsfile << s << "\n";
}

void ptspage::line(REAL x, const char* s) {
   ptsfile->clear();
   *ptsfile << "   " << fit(x) << " "  << s; nl();
}

void ptspage::line(REAL x, REAL y, const char* s) {
   ptsfile->clear();
   *ptsfile << "   " << fit(x) << " " << fit(y) << " " << s; nl();
}

void ptspage::line(REAL x, REAL y, REAL w, const char* s) {
   ptsfile->clear();
   *ptsfile << "   " << fit(x) << " " << fit(y) << " " << fit(w)
	    << " " << s; nl();
}

void ptspage::line(REAL x, REAL y, REAL w, REAL z, const char* s) {
   ptsfile->clear();
   *ptsfile << "   " << fit(x) << " " << fit(y) << " " << fit(w) << " " << fit(z)
	    << " " << s; nl();
}

//////////////////////////////////////////////////////////////////////////////
// POSTSCRIPT: Write PostScript definitions
//////////////////////////////////////////////////////////////////////////////
void ptspage::ptsdefs() {       // write PostScript definitions

// ABBREVIATE lineto, rlineto, moveto, rmoveto
   word("/aql {lineto} def");
   word("/aqrl {rlineto} def");
   word("/aqm {moveto} def");
   word("/aqrm {rmoveto} def");
// CALL: left up right down   hape % draws a box
   word("/squareshape { ");
   word(" /down exch def ");
   word(" /right exch def");
   word(" /up   exch def");
   word(" /left  exch def");
   word(" newpath");
   word(" left down aqm");
//   word(" left down aqm");
//   word(" left up aql");
   word(" left up aql");
   word(" right up aql");
   word(" right down aql");
   word(" closepath");
   line(" } def");
   
// CALL: x y size square % unit symbol magnified by size
   word("/square {");
   word(" /size exch def");
   word(" newpath");
   word(" size add aqm");
   word(" size 0 aqrm");
   word(" 0 -2 size mul aqrl");
   word(" -2 size mul 0 aqrl");
   word(" 0 2 size mul aqrl");
   line(" closepath stroke} def");

// CALL: x y size sglpnt % unit symbol magnified by size
   word("/sglline {");
   word(" /size exch def newpath exch size add exch aqm");
   word(" -2 size mul 0 aqrl closepath stroke} def");

// CALL: x y size diamond % unit symbol magnified by size
   word("/diamond {");
   word(" /size exch def");
   word(" newpath size sub aqm");
   word(" size size aqrl");
   word(" 0 size sub size aqrl");
   word(" 0 size sub 0 size sub aqrl");
   line("  closepath stroke } def");

// CALL: x y size uptriangle % unit symbol magnified by size
   line("/uptriangle { /size exch def newpath size add aqm size -2 size mul aqrl");
   line("  -2 size mul 0 aqrl closepath stroke } def");

// CALL: x y size dntriangle % unit symbol magnified by size
   word("/dntriangle {");
   word(" /size exch def");
   word(" newpath");
   word(" size sub aqm size 2 size mul aqrl");
   word("  -2 size mul 0 aqrl closepath stroke");
   line(" } def");

// CALL: x y size circle % unit symbol magnified by size
   line("/circle { 0 360 arc stroke } def");

// CALL: x y size sglpnt % unit symbol magnified by size
   line("/sglpnt { 0 360 arc fill stroke } def");

// CALL: x y (str) ltext % horizontal text, left aligned, right of x
   word("/ltext {");
   word(" /str exch def");
   line(" aqm str show } def");

// CALL: x y (str) ctext % horizontal text, centered around x
   word("/ctext {");
   word(" /str exch def");
   word(" aqm");
   word(" 0 str stringwidth pop  2 div sub 0 aqrm str show");
   line("} def");

// CALL: x y (str) rtext % horizontal text, right aligned: left of x
   word("/rtext {");
   word(" /str exch def");
   word(" aqm");
   word(" 0 str stringwidth pop sub 0 aqrm str show");
   line("} def");
 
// CALL: x y (str) vtext % vertical text, left justified
   word("/vtext {");
   word(" /str exch def");
   word(" /y exch def");
   word(" /x exch def");
   word(" x y translate 0 0 aqm");
   word(" 90 rotate");
   word(" str show");
   word(" -90 rotate");
   word(" 0 0 aqm");
   word(" 0 x sub 0 y sub translate");
   line(" } def");

// CALL: x y (str) vctext % vertical text, centered around y
   word("/vctext {");
   word(" /str exch def");
   word(" /y exch def");
   word(" /x exch def");
   word(" x y translate 0 0 aqm");
   word(" 90 rotate");
   word(" 0 0 str ctext");
   word(" -90 rotate");
   word(" 0 0 aqm");
   word(" 0 x sub 0 y sub translate");
   line(" } def");
   
// CALL: units cmtimes % set units Times-Roman, while in cm grid
   word("/cmtimes {");
   word(" /Times-Roman findfont exch");
   word(cmf, " div scalefont setfont");
   line(" } def");

// CALL: x0 y0 x y tic % moves to x0 y0 then rline to x y
   word("/tic {");
   line(" aqm aqrl stroke } def");
   
// CALL: units cmbtimes % set units Times-Bold, while in cm grid
   word("/cmbtimes {");
   word(" /Times-Bold findfont exch");
   word(cmf, " div scalefont setfont");
   line(" } def");

// RGB COLOURS
   line("/rgbblack { 0 0 0 setrgbcolor } def");
   line("/rgbred { 1 0 0 setrgbcolor } def");
   line("/rgbgreen { 0 1 0 setrgbcolor } def");
   line("/rgbblue { 0 0 1 setrgbcolor } def");
   line("/rgbcyan { 0 1 1 setrgbcolor } def");
   line("/rgbmagenta { 1 0 1 setrgbcolor } def");
   line("/rgbyellow { 1 1 0 setrgbcolor } def");
   line("/rgbwhite { 1 1 1 setrgbcolor } def");
}
