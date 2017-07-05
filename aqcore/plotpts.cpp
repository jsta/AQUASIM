////////////////////////////////   plotpts.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.05.93    Juerg Ruchti
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "aquasys.h"
#include "drawpts.h"
#include "vectors.h"

//////////////////////////////////////////////////////////////////////////////


static PTS pts;

static const char* /*const*/ linePattern[]={"",	// solid line
			"52",			// dash		long
			"5252",			// dash
			"52525252",		// dash		short
			"11111111111111111111",	// dotted
			":414",			// dash dot	long
			"52125212",		// dash dot
					""
				       };

static void set_pts_defaults(PLOTFILOPT* popt,	// example for implementing
                             BOOLEAN testing) {	// plot file options
int non_trivial_change=popt->NewPage();

  if (non_trivial_change && testing)
    non_trivial_change=popt->Portrait() == pts.Landscape()
                     ||popt->PaperWidth() != pts.PaperWidth()
                     ||popt->PaperHeight() != pts.PaperHeight()
                     ||popt->BorderLeft() != pts.PaperLeft()
                     ||popt->BorderRight() != pts.PaperRight()
                     ||popt->BorderBottom() != pts.PaperBottom()
                     ||popt->BorderTop() != pts.PaperTop()
               //    ||popt->WidthLegend() != pts.Flegend()
                     ||popt->NumCol() != pts.Columns()
                     ||popt->NumRow() != pts.Rows();
// First trivial change:
   pts.Flegend(popt->WidthLegend());
  
// Non-trivial changes:
   if (pts.newfile() || non_trivial_change) { // set defaults only the first time
  					  // or if format changed
  
      if (!pts.newfile()) pts.print(FALSE);  // format changed: 
   					  // set nonremovable page break;
   
      pts.paper(popt->PaperWidth(),popt->PaperHeight());

      pts.papermargins(popt->BorderLeft(),popt->BorderRight(),
                       popt->BorderBottom(),popt->BorderTop(),  0.5);
      if (!popt->Portrait()) pts.landscape();
      else                   pts.portrait();

      pts.frames(popt->NumCol(),popt->WidthLegend(),
                 popt->NumRow(), 2.0);
   
      pts.Dist(4 * popt->FontTitle() * pts.PIX());
      popt->NewPage(FALSE);

      pts.periods(1.0);
   }
// Trivial changes:
   
   pts.Tick(4);
   pts.XYn(-pts.Tick()-2);
   pts.YXn(-pts.Tick()-pts.FSnumb()-1);

   pts.FStit(popt->FontTitle());
   pts.FSaxes(popt->FontAxes());
   pts.FSnumb(popt->FontNumber());
   pts.FSlegend(popt->FontLegend());
   pts.Atit((pts.FSaxes() + pts.FSnumb())*pts.PIX() + 0.3);
   
   pts.printer(popt->Color());
}

static void set_eps_defaults(PLOTFILOPT* popt) {	// example for implementing
// First trivial change:
	pts.Flegend(popt->WidthLegend());
  
// Non-trivial changes:
	//int pwidth=popt->PaperWidth() / popt->NumCol();
	//int pheight=popt->PaperHeight() / popt->NumRow();
      pts.paper(popt->PaperWidth(),popt->PaperHeight());

      pts.papermargins(popt->BorderLeft(),popt->BorderRight(),
                       popt->BorderBottom(),popt->BorderTop(),  0.5);

	  pts.portrait();	 // NO LANDSCAPE for EPS files

	  // 1 column, 1 row:
      pts.frames(popt->NumCol(),popt->WidthLegend(),
                 popt->NumRow(), 2.0);
   
      pts.Dist(4 * popt->FontTitle() * pts.PIX());
      popt->NewPage(FALSE);

      pts.periods(1.0);

// Trivial changes:
   
   pts.Tick(4);
   pts.XYn(-pts.Tick()-2);
   pts.YXn(-pts.Tick()-pts.FSnumb()-1);

   pts.FStit(popt->FontTitle());
   pts.FSaxes(popt->FontAxes());
   pts.FSnumb(popt->FontNumber());
   pts.FSlegend(popt->FontLegend());
   pts.Atit((pts.FSaxes() + pts.FSnumb())*pts.PIX() + 0.3);
   
   pts.printer(popt->Color());
}

static void ClipXArray(REAL* array, REAL amin, REAL amax, int an,
                       int& FirstIndex, int& Number) {
// returns index 'FirstIndex' of (greatest array element) < amin
//         and   'Number' of elements including (smallest array element) > amax
   int firstindx=0;
   int lastindx=an - 1;
   
   while (array[firstindx] <= amin 
          && firstindx < lastindx)  firstindx++;
   if (0 < firstindx) firstindx--;
   while (firstindx < lastindx
          && amax <= array[lastindx]) lastindx--;
   if (lastindx + 1 < an) lastindx++;
   FirstIndex=firstindx;
   Number=lastindx - firstindx + 1;
}

JOBSTATUS AQUASYS::PlotResults(std::ifstream& oldfile, std::ofstream& ptsfile, PLOT* plot, BOOLEAN EPS)
{
   PLOTDATA data;
   JOBSTATUS status = GetAllCurvesData(plot,&data);
   if ( status != OK ) return status;
   if ( data.IsFinite() == FALSE )
   {
      *cjob << "\n\n   *** " 
            << "Undefined value in plot " 
            << plot->Symbol() << " ***";
      cjob->flush();
      return PROBLEM;
   }

   if ( plot == 0 ) return PROBLEM;
   if ( data.NumCurveData() == 0 ) return PROBLEM;
     
     int ncurves;		// number of curves in P
     int Ncurves;		// effective number of curves
     int i;
        
   ncurves=plot->NumCurves();
   Ncurves=0;
   
   for (i=0; i< ncurves; i++) {
      if ( data.IsCurve(i) ) Ncurves++;
   } // end for i

   if ( Ncurves == 0 ) { // give it up:
       return PROBLEM;
   }
   

   
   RealVec xd, xc;	// x-coordinates
   RealVec yd, yc;	// y-coordinates
   
   BOOLEAN Legend;	// only if legend text exists
/*
             |
    ...frame |............===========S===========..............
             |xsymboffset xsymbolhalf xsymbolhalf xtxtoffsymb : ytxtbelowsymb
             |  :                                              legend dtext
             |  : ylegspace
             |  :
*/
   REAL xl[2],yl[2];
   CARDINAL EvelopeColor;
   RealVec xleg, yleg;	// lines/symbols in legend
   REAL xltxt, yltxt;	// legend text

   char buf1[128]; char buf2[128];
   strcpy(buf1,ProgName());
   strcat(buf1," - ");
   strcat(buf1,ProgVersion());
   strcat(buf1,": ");
   TimeToString(ini.T(110),buf2,128);
   strcat(buf1,buf2);

   pts.SetEPS(EPS);
//   pts.startfile(oldfile,ptsfile, plot->Title(), buf2, ProgName(), ProgVersion());

   // order of the following two blocks changed PR 11.09.98
   // (first copy file, then insert page break):

	// you should not append a ps to an EPS file:
	if (-2 == pts.startfile(oldfile,ptsfile, plot->Title(),
			buf2, ProgName(), ProgVersion())) {
		// PR: please do define this PROBLEM
		// AfxMessageBox("You can not append a ps frame to an eps frame",MB_OK);
		return PROBLEM;
	}

	if (EPS) {
		set_eps_defaults(PlotFilOpt()); // for EPS files
	}
	else {  // multiframe ps file
		// did user change his options ?  testing=TRUE: no page break on trivial changes
		//set_pts_defaults(PlotFilOpt(), TRUE);  // for JR testing
		set_pts_defaults(PlotFilOpt(), FALSE); // for PR release
	}


   pts.startframe();
   
// Now, Period() is defined for this frame and may be used:
   REAL xsymboffset  =0.25*pts.Period();		// legend symbol
   REAL xsymbolhalf  =0.5*pts.Period();			// legend symbol
   REAL xtxtoffsymb  =0.25*pts.Period();		// legend text
   REAL ytxtbelowsymb=0.4*pts.FSlegend()*pts.PIX();	// legend text
   REAL ylegspace    =1.5*pts.FSlegend()*pts.PIX();	// legend text

// draw tics and write numbers to them:
    pts.axis_ticks(TRUE,				// abszissa
                  (plot->xMin()),(plot->xMax()),
                  (plot->xTickPos()),(plot->xTickDist()));
                 
    pts.axis_ticks(FALSE,				// ordinate
                  (plot->yMin()),(plot->yMax()),
                  (plot->yTickPos()),(plot->yTickDist()));
// write titles
    pts.titframe(plot->Title());
    

    pts.titabs(plot->xTitle());
    pts.titord(plot->yTitle());

// prepare legend coordinates
    xl[0]=pts.xmax() + xsymboffset; 
    xl[1]=pts.xmax() + xsymboffset + 2 * xsymbolhalf; xleg.vector(2,&xl[0]);
    xltxt=xl[1]+ xtxtoffsymb;
    
    yl[0]=yl[1]=pts.ymax() - pts.PIX()*0.4*pts.FSlegend();
    yleg.vector(2,&yl[0]);
    
    yltxt=yl[0] - ytxtbelowsymb;

// draw Legends & curves
   for (i=0; i < ncurves; i++) {
      if (data.IsCurve(i)) {
      
          if ((plot->Curve(i))->Legend()) {
           if (strlen((plot->Curve(i))->Legend())) {
              Legend=TRUE;
           }
           else Legend=FALSE;
          }
          else Legend=FALSE;
          if (Legend) {
          // legend text:
            pts.word(pts.FSlegend()," cmtimes");
            pts.number(xltxt,yltxt);
            pts.psstr((plot->Curve(i))->Legend()); pts.line(" ltext");
          }
          int Clindx;
          int NotClipped;
          ClipXArray(data.X(i), plot->xMin(), plot->xMax(), data.NumPts(i),
                    Clindx, NotClipped);
          
          xd.vector_range(NotClipped,&data.X(i)[Clindx],plot->xMin(),plot->xMax());
          xc.vector_range(NotClipped,0,0,pts.xmax());
          xd.project(&xd,&xc);
          
          yd.vector_range(NotClipped,&data.Y(i)[Clindx],plot->yMin(),plot->yMax());
          yc.vector_range(NotClipped,0,0,pts.ymax());
          yd.project(&yd,&yc);
          
          if ((plot->Curve(i))->LineActive()) {		// draw Y/X line
          // legend line:
            if (Legend)
              pts.plotline(&xleg,&yleg,(plot->Curve(i))->LineWidth(),
                                   (plot->Curve(i))->LineColor(),
                                   FALSE,
                                   linePattern[(plot->Curve(i))->LineStyle()]);
          // curve:
            if (NotClipped == 1 && !(plot->Curve(i))->MarkActive())
           pts.plotsymbols(&xc,&yc,-1, // single point
                                   (plot->Curve(i))->MarkSize(),
                                   (plot->Curve(i))->LineColor());
            else 
              pts.plotline(&xc,&yc,(plot->Curve(i))->LineWidth(),
                                   (plot->Curve(i))->LineColor(),
                                   TRUE,
                                   linePattern[(plot->Curve(i))->LineStyle()]);
          }
          if ((plot->Curve(i))->MarkActive()) {		// draw Y/X Markers
            if (Legend)
           pts.plotsymbol((xl[0]+xl[1])/2,yl[0],(plot->Curve(i))->MarkStyle(),
                                   (plot->Curve(i))->MarkSize(),
                                   (plot->Curve(i))->MarkColor(),
                                   FALSE);
          // curve:
           pts.plotsymbols(&xc,&yc,(plot->Curve(i))->MarkStyle(),
                                   (plot->Curve(i))->MarkSize(),
                                   (plot->Curve(i))->MarkColor());
          }
  // draw optional envelopes
          if ((plot->Curve(i))->LineActive())
               EvelopeColor=(plot->Curve(i))->LineColor();
          else EvelopeColor=(plot->Curve(i))->MarkColor();
          if (data.Yplus(i)) {				// draw Yplus/X line
          
            yd.vector_range(NotClipped,&data.Yplus(i)[Clindx],plot->yMin(),plot->yMax());
            yc.vector_range(NotClipped,0,0,pts.ymax());
            yd.project(&yd,&yc);
/*  before clipping:
            yd.vector_range(data.NumPts(i),data.Yplus(i),plot->yMin(),plot->yMax());
            yc.vector_range(data.NumPts(i),0,0,pts.ymax());
            yd.project(&yd,&yc);
*/
            // curve:
              pts.plotline(&xc,&yc,0.2, EvelopeColor,
                                   TRUE,
                                   linePattern[LineDotted]);
          }
          if (data.Yminus(i)) {			// draw Yminus/X line
            yd.vector_range(NotClipped,&data.Yminus(i)[Clindx],plot->yMin(),plot->yMax());
            yc.vector_range(NotClipped,0,0,pts.ymax());
            yd.project(&yd,&yc);
/*  before clipping:
            yd.vector_range(data.NumPts(i),data.Yminus(i),plot->yMin(),plot->yMax());
            yc.vector_range(data.NumPts(i),0,0,pts.ymax());
            yd.project(&yd,&yc);
*/
            // curve:
              pts.plotline(&xc,&yc,0.2, EvelopeColor,
                                   TRUE,
                                   linePattern[LineDotted]);
          }
          if (Legend) {
          // advance legend line:
             yltxt=yltxt - ylegspace;
             yl[0]=yl[0] - ylegspace;
             yl[1]=yl[1] - ylegspace;
          }
          
      }
   }

   if (!pts.print(TRUE)) return PROBLEM;
   return OK;
}

//////////////////////////////////////////////////////////////////////////////
