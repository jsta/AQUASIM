
///////////////////////////////   drawpts.h   ////////////////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.05.93    Juerg Ruchti
// revisions:   98.10.93    Juerg Ruchti
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////////////////
//
// Classes:
// ========
//
//   ptspage
//       |
//       +---------PTS
//
//////////////////////////////////////////////////////////////////////////////////////////
/*

  I. PAGE and FRAME GEOMETRY
  ========================================================================================
     Example: horiframes = 3, vertframes = 2.

  ---------------------------------- pw---------------------------------------
  |       :PaperTop                                                          |
  |       :                                                                  |
  |         :  :Ftit                  xmax                                   |
  |      ------------         ------------ymax     ------------      .       |
  |      |          |     Otit|          |      YXn|          |              |
  |...   |          |      ...|          |       ..|          |Flegend       |
  |   ...| frame 1  |......   | frame 2  |......   | frame 3  |......        |
  |      |          |Flegend  |          |         |          |      ........|
  |      |          |         |          |        .|          |    PaperRight|
  |      ------------      fy0------------        ^------------      .       |
  |          :                fx0 :::<----Tick----|                          |
  |          :                    :: XYn                                     |
  |          :                    :                                          | 
  |          :Atit                :Atit                                      |  
  |            :Dist                                                         |
  |            :                                                             |ph
  |              :Ftit                                                       |
  |      ------------         ------------         ------------      .       |
  |      |          |         |          |         |          |              |
  |      |          |         |          |         |          |              |
  |      | frame 4  |         | frame 5  |         | frame 6  |              | 
  |      |          |         |          |         |          |              |
  |      |          |         |          |         |          |              |
  |      ------------         ------------         ------------      .       |
  |...        :                                                              |
  |PaperLeft  :                                                              | 
  |           :                                                              |
  |           :Atit                                                          |
  |             :                                                            |
  |             :PaperBottom                                                 | 
  |      :pyid  :                                                            |
  |             :                                                            |
  ---------------------------------- pw---------------------------------------

   A. FIRST OPEN A FILE      	        FUNCTION	ARGUMENT Dimension,Sign,Expl.
                                        ----------------------------------------------
   	  1. declare a file.............ifstream oldfile
   	     open file..................oldfile.open(file.name,ios::in)
   	  2. declare a file.............ofstream ptsfile
	     open file..................ptsfile.open(file.name,ios::in | ios::out|ios::trunc)
   	  
   B. THEN DEFINE PAGE AND FRAME
          1. declare a variable.........PTS pts;

		  2. Optionally define to write
			 Encapsulated PS file.......pts.SetEPS();

          3. define page layout.........
		    Paper Size		    paper		[cm] -> pw, ph
          	Page Margins		papermargins    [cm] -> pxl,pxr, pyd,pyu, pyid
          	Paper Orientation	portrait/landscape
          	Position of Frames	frames       -> horiframes, vertframes
          	Line Patterns		periods		number of dash patterns
          						per cm
          	Y-coord Program Ident.	pyid		[cm] below Bottom Margin
          	Colour or B/W Printer	printer
          	
          4. define frame layout........
          	Tick Length		        Tick		[pix].no sign
          	Frame Legend Length	    Flegend		[cm]..no sign,   rel. to xmax
          	Minimal Axes Length	    Minaxlen	[cm]..no sign
          	Vert.Space betw. Frames	Dist		[cm]..no sign,   rel. to ymax
          	Y-coord of Frame Title	Ftit		[cm]..no sign,   rel. to ymax
          	Y-coord of absc.numbers	YXn		[pix].with sign, rel. to fx0
          	X-coord of ord.numbers	XYn		[pix].with sign, rel. to fy0
          	Y-coord of Abs. Title	Atit		[cm]..no sign,   rel. to fy0
			X-coord of Ord. Title	Otit		calculated automatically by startframe
          	Empirical Correction	Otitcorr	number of characters of size FSnumb
												to  X-coord ofOtit          	
          5. define font sizes..........
          	Frame Title		FStit		[pix]
          	Axes Titles		FSaxes		[pix]
          	Numbers on Axes		FSnumb		[pix]
          	Legend			FSlegend	[pix]
          	
          6. define number precision....NPrecision
             define no of periods	periods
               of line patterns per cm
             get length of period	Period		[cm] length of a line pattern
          
          7. validate definitions,	ValidAxes	[cm]
               estimate xmax,
               estimate ymax		

          8. start PScript file........pts.startfile(ifstream& oldfile,
                                                     ofstream& ptsfile)
          
   C. REPEAT:
 	  1. start a frame..............
 	  				startframe      [cm] ->fx0=0, xmax, fy0=0, ymax
 	     get actual values		xmax,ymax, etc.
 	  2. label frame................axis_ticks
 	  				titframe	centered/(ymax + Ftit)
 	  				titabs		centered/-Atit
 	  				titord		(-Otit+FSaxes*PIX())/centered
 	  3. draw curves................setcolour
 	  				plotline........default: clip frame, no line pattern
 	  				plotsymbols.....default: clip frame
 	  				plotsymbol......default: clip frame
 	  				
 	  4. option: force page break...print(FALSE)
 	  
   D. END JOB
   	  1. set a page break...........print(TRUE).....removable on reopen
   	  				print(FALSE)....non removable on reopen
   	  2. you may go back to B.
   	     or
   	     you may close the files....oldfile.close()
   	                                ptsfile.close().sets non removable page break
   

  II. LEGEND GEOMETRY
  ========================================================================================
  Programmer must introduce his own application dependent variables in order to write
  legend(s) to his frames. Here a hint at variables required:
  
             |
    ...frame |............===========S===========..............
             |xsymboffset xsymbolhalf xsymbolhalf xtxtoffsymb : ytxtbelowsymb
             |  :                                              legend dtext
             |  : ylegspace
             |  :

  III. CONSTANT LOW LEVEL GEOMETRY
  ========================================================================================
  Some geometric elements are directly encoded in drawpts.C and must be changed there,
  if required:
  X position of Frame title: PTS::titframe
  X position of Abscissa title: PTS::titabs
  Y position of Ordinate title: PTS::titord
  X,Y position of Numbers along Abscissa: PTS::axis_ticks: call to axis(X,Y,tick)
  X,Y position of Numbers along Ordinate: PTS::axis_ticks: call to axis(X,Y,tick,-1)
  X,pyid position & Font Size of Program Identifier: ptspage::print
  
 */
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef DRAWPTS
#define DRAWPTS
#include "all.h"
#include "vectors.h"
#include "plotsys.h"


//----------------------------------------------------------------------------------------
   class ptspage {				// PAGE MANAGEMENT
//----------------------------------------------------------------------------------------
    
  public:

	  int startfile(std::ifstream& oldfile,
		  std::ofstream& outfile,
				 const char* title,
				 const char* date,
				 const char* creator,
				 const char* version="");
 //                const char* text=0);
    
	void SetEPS(BOOLEAN eps=FALSE);

    BOOLEAN newfile()				// the first call to startframe sets
       { return first_open_in_job; }		// first_open_in_job=FALSE
    
    void paper(REAL width,			// paper width in cm
               REAL height);			// paper height in cm
    REAL PaperWidth()  { return pw; }
    REAL PaperHeight() { return ph; }
    
    void papermargins(REAL xleft, REAL xright,	// margins in cm
                      REAL ydown, REAL yup,	// margins in cm
                      REAL progidenty);		// redefinition of pyid
    REAL PaperLeft() { return pxl; }
    REAL PaperRight() { return pxr; }
    REAL PaperTop() { return pyu; }
    REAL PaperBottom() { return pyd; }
                      
    void portrait();							// rotate to portrait (default)
    BOOLEAN Portrait();
    
    void landscape();							// rotate to landscape
    BOOLEAN Landscape() { return lndscape; }
    
    void printer(BOOLEAN with_colours=TRUE)		// default colour printer: try to use
         {print_withcolours=with_colours; }		// Pageview - if you have a Sun
    
    void frames(int horizontal, REAL flegend, 	// number horizontal frames,
    											// separated by flegend cm
                int vertical, REAL dist);		// number vertical   frames,
                								// separated by ftit    cm
    int Columns() { return horiframes; }
    int Rows()    { return vertframes; }
                      
// you should define periods before calling PTS::startframe
    void periods(REAL periods)					// define number of periods of
    			{ nper=periods; }				// dashed or dotted lines per width
    REAL periods() { return nper; }				// enquire number of periods
    
    BOOLEAN print(BOOLEAN removable);			// showpage
    
// NOTE: static members below may e called by static ValidAxes

// UNIT CONVERSION FACTORS:

    REAL CM() { return cmf; }					// result: pts units for 1 cm
    REAL PIX() { return pix; }					// result: cm for 1 pts unit
    

// GENERAL PURPOSE PostScript OUTPUT ROUTINES:

    void nl();								// nl: write a line feed to ptsfile
    
    void number(REAL r);					// number: write number to ptsfile,
											// no linefeed
    void number(REAL x, REAL y);
    void number(REAL x, REAL y, REAL z);
    void axis(REAL x, REAL y, REAL number,	// writes number as string, default centered at x
               int pos=0);					// pos < 0: left of x; 0 < pos: right of x
               
    void psstr(const char* s);				// psstring: write "(s)"
    void word(const char* s);				// word: write a 'word' to ptsfile, no linefeed
    void word(REAL x, const char* s);
    void word(REAL x, REAL y, const char* s);
    void word(REAL x, REAL y, REAL w, const char* s);
    void word(REAL x, REAL y, REAL w, REAL z, const char* s);
    
    void comment(char* c);					// comment: write a % line, then linefeed
    
    void line(const char* s);				// line: write a line to ptsfile, with line feed
    void line(REAL x, const char* s);
    void line(REAL x, REAL y, const char* s);
    void line(REAL x, REAL y, REAL w, const char* s);
    void line(REAL x, REAL y, REAL w, REAL z, const char* s);
    
    int status() { return state; } // used internally, you may use it for debugging
    

  protected:
	BOOLEAN isEPS;
  
    int state;                  // 0: you may call paper, papermargins,
                                //    switch to landscape/portrait,
                                //    call frames to define frames on page
                                //    then call startframe
                                // 1: startframe calls start which sets state to 1
                                //    startframe then sets state to 2
                                // 2: frame defined, you may draw text, lines
                                //    you may call startframe again
    
	std::ofstream* ptsfile;
    BOOLEAN first_open_in_job;
    
    REAL pyid;                  // program identifier will be drawn at (pxl;pud-puid);
                                // default set in ptspage, pyid redefined in papermargins
    				
    REAL pw , ph,				// paper width, height
         pxl, pxr,				//       left,  right margins
         pyd, pyu;				//       down,  upper margins
    REAL nper,					// number of periods
         period;				// length of a period, cm (= fx/nper) -> startframe
    
    REAL px,					// = local 'pw'
         py;					// = local 'ph'
    
    REAL cmf;					// pts units for 1 cm
    static REAL pix;			// cm for 1 pts unit
    
    REAL curxscale;				// current x scale factor
    REAL curyscale;				// current y scale factor
    
    REAL curxorig;				// current x origin
    REAL curyorig;				// current y origin
    
    BOOLEAN lndscape;			// landscape or default: portrait
    
    BOOLEAN print_withcolours;
    
    int horiframes; REAL FLEG;	// number of frames side by side, distance between
    int vertframes;				// number of frames below each other
    static REAL DIST;			// distance between frames
    int nframes;				// total number of frames on a page
    int curframes;
    
    static REAL FTIT;
    REAL OTIT;
    static REAL OTITCORR;
    static REAL ATIT; 
    
    REAL AQ_TICK;
    
    REAL XYN;
    static REAL YXN;
    
    static REAL MINAXLEN;		// minimal length of an axis in cm

    static int  PREC;			// number precision on axes
    
    REAL FSTIT,					// Font Size Title
         FSAXES,				//           Axes Title
         FSNUMB,				//           Numbers on Axes
         FSLEGEND;				//			 Legend
    
	REAL epsLeft, epsTop,		// BoundingBox for EPS files
		epsRight, epsBottom;

    void ptsdefs();				// write pts definitions to ptsfile
        
  
    ptspage();					// protected constructor: you should declare ptsframe;
    
    void start();    			// sets all ready for startframe

    void newscale(REAL xscale, REAL yscale);	// set new scale factors
    void oldscale();							// revert to old scale factors
    REAL oldxscale() { return 1.0/curxscale; }	// get reciprocal x scale factor
    REAL oldyscale() { return 1.0/curyscale; }	// get reciprocal y scale factor

    void neworig(REAL xorig, REAL yorig);		// translate to xorig, yorig
    void oldorig();								// translate to -curxorig, -curyorig
    
    void error(const char* from,
		const char* help);		// called from procedure "from"
    REAL fit(REAL r);			// PostScript does not like big numbers
    
  private:
	void PrintPSHeader();		// Prints PS File Header
};


//----------------------------------------------------------------------------------------
   class PTS : public ptspage {
//----------------------------------------------------------------------------------------

  private:
  
    REAL fx0, fy0,				// origin of a frame, cm, relative to (pw,ph)
         fx, fy;				// width and height of a frame, cm, relative to (fx0,fy0)
    
  public:
  
    PTS();						// constructor:	you must call startfile

	PTS(std::ifstream& old, 
		std::ofstream& out);			// constructor, not yet implemented, don't use it
    
    REAL xmax() 		{ return fx; 	}	// returns fx, cm, relative to fx0
    REAL ymax() 		{ return fy; 	}	// returns fy, cm, relative to fy0
    
    REAL Atit()         { return ATIT;  }	// returns ATIT, cm below fy0
    void Atit(REAL abs) { ATIT=abs;     }
    REAL Otit()         { return OTIT;  }	// returns OTIT, cm left of fy0
    void Otitcorr(REAL correction)			// correction to OTIT, ~characters of FSnumb
                 { OTITCORR=correction; }
    
    REAL Flegend()		{ return FLEG;  }	// returns flegend, cm right of fy0
    void Flegend(REAL l){ FLEG=l;  	}
    
    REAL YXn()       	{ return YXN; 	}	// X-coordinate of number on ordinate,
    void YXn(REAL x) 	{ YXN=x; 	}		// pts units, relative to left tick end
    
    REAL Minaxlen()  { return MINAXLEN; }	// Minimal length of an axis, cm
    void Minaxlen(REAL m) { MINAXLEN=m; }	// if 'axisunderflow', startframe will exit.
    										// Use ValidAxes to test/prevent 'axisunderflow'.
    
    
    int  NPrecision()	{ return PREC;	}	// number precision on axes
    void NPrecision(int p) {PREC=p;	}
    

    REAL Tick()         { return AQ_TICK; }	// pts units
    void Tick(REAL size){ AQ_TICK=size;   }
    
    REAL XYn()       	{ return XYN; 	}	// Y-coordinate of number on abszissa,
    void XYn(REAL y)	{ XYN=y; }			// pts units, relative to left tick end
    
    REAL Dist()         { return DIST;  }	// returns dist, cm, relative to fy0
    void Dist(REAL dis) { DIST=dis;     }
    
    REAL Ftit()         { return FTIT;  }	// returns ftit, cm, relative to fy0
    void Ftit(REAL tit) { FTIT=tit;     }
    REAL FStit()        { return FSTIT; }	// Font Size of Main Title
    void FStit(REAL s)  { FSTIT=s;   }
    
    REAL FSaxes()       { return FSAXES;}	// Font Size of Axes Titles
    void FSaxes(REAL s) { FSAXES=s;     }
    
    REAL FSnumb()       { return FSNUMB;}	// Font Size of Numbers on Axes
    void FSnumb(REAL s) { FSNUMB=s;     }
    
    REAL FSlegend()     { return FSLEGEND;}	// Font Size of Legend Text
    void FSlegend(REAL s){ FSLEGEND=s;  }
    
    static CARDINAL ValidAxes(PLOTFILOPT* opt);
    
    static CARDINAL ValidAxes(BOOLEAN portrait,// Axes Length test for frame parameters:
    		REAL papwidth,			// Results: 
    		REAL papheight,			// result = 0: MINAXLEN<=abszissa, MINAXLEN<=ordinate
    		REAL bleft, REAL bright,// result = 1: abszissa<MINAXLEN , MINAXLEN<=ordinate
    		REAL bbottom, REAL btop,// result = 2: MINAXLEN<=abszissa, ordinate<MINAXLEN
    		REAL fsaxes,			// result = 3: abszissa<MINAXLEN,  ordinate<MINAXLEN
    		REAL fsnumb,
    		REAL wlegend,
    		int ncol,
    		int nrow,
    		REAL fstitle,
    		REAL& abszissa,
    		REAL& ordinate);
       // NOTE: ValidAxes works with the current values of OTIT, FTIT, DIST, ATIT;
       //       those values may be obtained or set by Otit, Ftit, Dist, Atit.

    void startframe();						// start next frame

    REAL Period() { return period; }		// actual value calculated by startframe

    void axis_ticks(BOOLEAN on_X_axis,		// draw ticks and label them
                    REAL rmin, REAL rmax,	// with numbers
                    REAL pos, REAL dist);
                       
    void titframe(const char* name);		// write name to frame
    void titabs(const char* name);			// write name to axes
    void titord(const char* name);			// write name to axes
    
    void setcolour(int colour);				// if printer(FALSE) has been called,
    										// setcolour will set colour to black
 // plot (and clip outside of frame):
    void plot1line(RealVec* x, RealVec* y,
                    REAL width, int color, BOOLEAN clip=TRUE);
    void plotline(RealVec* x, RealVec* y,
                    REAL width, int color, BOOLEAN clip=TRUE,
                    const char* pattern=0);
                 /* valid symbols in pattern string:	0  value:  0
                 					1	   1
                 					............
                 					9	   9
                 					:	  10
                 					;	  11
                 					<	  12
                 					=	  13
                 					>	  14
                 					?	  15
                 					@	  16
                 					A	  17
                 					B	  18
                 					............
                 */
                 					
    
    static REAL symbol_size_factor;
    
    void plotsymbols(RealVec* x, RealVec* y,
                    int symbol, int size, int color, BOOLEAN clip=TRUE);
    void plotsymbol(REAL x, REAL y,
                    int symbol, int size, int color, BOOLEAN clip=TRUE);
     
};


#endif // DRAWPTS
