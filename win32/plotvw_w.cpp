//////////////////////////////////////////////////////////////////////////////
// PlotView.cpp : implementation file
//
//              date:       person:           comments:
//
// creation:    28.05.96    Juerg Ruchti
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxwin.h>

#include <math.h>
#include "AQUASIM.h"

#include "PLOTDO_W.H"
#include "PLOTVW_W.H"
#include "mainfr_w.h"
//#include "plot_w.h"  included via mainfr_w.h

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Contents
/////////////////////////////////////////////////////////////////////////////
/*
    1. Constructor, Destructor, Message Map
	2. Initialization
	3. Drawing: OnDraw, Initialization, Layout
	4. Drawing: Curves and Legend
	5. Interaction with other modules
	6. MSC++ Diagnostics
	7. Copy to Clipboard, LButtonDblClick: OnEditCopy
	8. OmChar, OnMouse..., OnLButton..., Zooming
	9. Scrolling

*/ 
/////////////////////////////////////////////////////////////////////////////
// 1. Constructor, Destructor, Message Map
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPlotView, CScrollView)

CPlotView::CPlotView()
{
	P=NULL;
	inhibitMouse=TRUE;
}

CPlotView::~CPlotView()
{
	// see CPlotView::OnDestroy()
}


BEGIN_MESSAGE_MAP(CPlotView, CScrollView)
	//{{AFX_MSG_MAP(CPlotView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// 2. Initialization
/////////////////////////////////////////////////////////////////////////////

void CPlotView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	ArrowCursor=aqapp.LoadStandardCursor(IDC_ARROW);
	CrossCursor=aqapp.LoadStandardCursor(IDC_CROSS);
	WaitCursor =aqapp.LoadStandardCursor(IDC_WAIT);
	CurrentCursor=WaitCursor; 
	HCURSOR ok=SetCursor(CurrentCursor); ShowCursor(TRUE); 

	InitConstants();
	CPlotDoc* pDoc = (CPlotDoc*)m_pDocument;
	P=pDoc->plot;

	PlotFileName=aqapp.MainFrame->FileTitle();
	StaleFile=FALSE;
//	CString ft=aqapp.MainFrame->FileTitle();
//	pDoc->SetTitle(ft);
//	ft=ft + ": " + P->Title(); 
//	aqapp.MainFrame->SetWindowText(ft);

	SetWindowSize(aqsys.PlotScrOpt()->WinWidth(),aqsys.PlotScrOpt()->WinHeight());

	CrossON=FALSE;
	CrossCoordinates[0]=CPoint(0,0);

	PD=&pDoc->data;
	GetCurveInfo();

	aqapp.MainFrame->plotmanager.AddPlot(this);
}

/////////////////////////////////////////////////////////////////////////////
// 3. Drawing: OnDraw, Initialization, Layout
/////////////////////////////////////////////////////////////////////////////

void CPlotView::SetPlotTitle()
{
	CPlotDoc* pDoc = (CPlotDoc*)m_pDocument;
	if (!StaleFile) {
		if (PlotFileName == aqapp.MainFrame->FileTitle()) {
			PlotTitle=PlotFileName + aqapp.MainFrame->modifyflag() + " " + P->Title();
		}
		else {
			PlotTitle="[" + PlotFileName + " " + P->Title() + "]";
			StaleFile=TRUE;
		}
	}
	pDoc->SetTitle(PlotTitle);
/*
	CPlotDoc* pDoc = (CPlotDoc*)m_pDocument;
	if (setORclear)	
		 pDoc->SetTitle(aqapp.MainFrame->FileTitle()
					+   aqapp.MainFrame->modifyflag() + " " + P->Title());

	else pDoc->SetTitle("");
*/
}

void CPlotView::OnDraw(CDC* pDC)
{
	inhibitMouse=TRUE;
	Dragging=FALSE;
	Redraw();
}

void CPlotView::Redraw()
{


//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	CPlotDoc* pDoc = (CPlotDoc*)m_pDocument;

//	PLOT* oldplot=P;
	P=pDoc->plot;

//	if (oldplot && (P != oldplot)) GetCurveInfo();

	PD=&pDoc->data;

	SetPlotTitle();

	this->GetClientRect(winrct);

	CalculateLayout(aqsys.PlotScrOpt());

	if (!CrossON) //CrossCoordinates[0]==CPoint(0,0))
	{// for people without mouse, give them a start position:
		CRect sw; GetWindowRect(sw);
		lastpoint=CPoint(pltrct.left+20,pltrct.top +20);
		SCREENPNT=CPoint(sw.left, sw.top) + lastpoint;
// 		SetCursorPos(SCREENPNT.x, SCREENPNT.y);
	}

	RegisterDrawTools();
	   
		BOOLEAN TOO_SMALL=xwid < 1 || yhgt < 1;
		if (TOO_SMALL)
		{

		/*
			AfxMessageBox("Window too small to display graphic RESIZE WINDOW!",
		          MB_OK+MB_ICONEXCLAMATION);
			// no good
		*/
			TextTool error(this);
			warning_txt.text(25,20,"Window too small",Left,0,blue);
			warning_txt.text(25,35,"to display graphic",Left,0,blue);
			warning_txt.text(25,55,"RESIZE WINDOW",Left,0,red);
			return;
		}

	SetFontsAndWriteTitles();
	MouseInit();

	DrawAxes();

	pen_n.rectangle(pltrct);	// close rectangle around plot

	DrawAllCurves();

	inhibitMouse=FALSE;
	// for people without mouse, give them a start position:

#ifdef PLOTWV_DEBUG
	Prelude();
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CPlotView::RegisterDrawTools()
{
	M[MarkCircle  ]->attachToWindow(this);
	M[MarkSquare  ]->attachToWindow(this);
	M[MarkTriangle]->attachToWindow(this);
	M[MarkDiamond ]->attachToWindow(this);
	M[MarkDot     ]->attachToWindow(this);

	pen_n.attachToWindow(this);
	pen_tic.attachToWindow(this);
    plot_title.attachToWindow(this);
    plot_legend.attachToWindow(this);
    xaxis_title.attachToWindow(this);
    yaxis_title.attachToWindow(this);
	axis_label.attachToWindow(this);
	warning_txt.attachToWindow(this);
	draglines.attachToWindow(this);


#ifdef PLOTWV_DEBUG
	test_text.attachToWindow(this);
	EAWAG.attachToWindow(this);
	P_Reichert.attachToWindow(this);
#endif
}

void CPlotView::SetFontsAndWriteTitles()
{
// Plot title
	plot_title.Size(Tfontsize);
	plot_title.Bold();
	plot_title.Text(int(winrct.right/2), topmargin, 
					P->Title(), Center);
// Legend
	plot_legend.Size(Lfontsize);
	plot_legend.Italic();
// X axis
	xaxis_title.Size(Afontsize);
	xaxis_title.Text(leftframe + xwid/2,topframe + yhgt + ynum + ynuml,
					P->xTitle(),Center);
// Y axis, try to tune it using height and width
	double height=1.0;		// height = 1.0 selects default
	double width =0.6;		// width  = 0.0 selects default
	yaxis_title.Size (int(height * Afontsize));
	yaxis_title.Width(int(width * height));
	yaxis_title.Text(leftmargin,topframe  + yhgt/2,
					P->yTitle(),Center,900);
// Labels on axes
	axis_label.Size(Nfontsize);
}
void CPlotView::MouseInit()
{
// Mouse movement and coordinate display
   x_CoordText.x=pltrct.right;
   x_CoordText.y=pltrct.bottom + 3 * Nfontsize;
   y_CoordText.x=pltrct.left;
   y_CoordText.y=pltrct.top - Nfontsize;
   coord_Precision=4;
   X_factor=(PxMAX - PxMIN) / pltrct.Width();
   Y_factor=(PyMAX - PyMIN) / pltrct.Height();
}

/////////////////////////////////////////////////////////////////////////////
#ifdef PLOTWV_DEBUG
void CPlotView::Prelude()
{
	// Add here code for debugging e.g. fonts...
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CPlotView::InitConstants()
{
    crvinx=0;

#ifdef _DEBUG
    aqapp.setLeakObserver(0);
#endif
    aqcol[AquaBlack]=COL.GetCREF(black);
    aqcol[AquaRed]=COL.GetCREF(red);
    aqcol[AquaGreen]=COL.GetCREF(green);
    aqcol[AquaBlue]=COL.GetCREF(blue);
    aqcol[AquaCyan]=COL.GetCREF(cyan);
    aqcol[AquaMagenta]=COL.GetCREF(magenta);
 	COL.Adjust(yellow,0xFF,215,0);
	aqcol[AquaYellow]=COL.GetCREF(yellow);
    aqcol[AquaWhite]=COL.GetCREF(white);

   
// Make Markers:
   Nmarkers=5;
   M  =new DrawMark* [Nmarkers];
   M[MarkCircle]  =new DrawMark; M[MarkCircle]->def_circle();
   M[MarkSquare]  =new DrawMark; M[MarkSquare]->def_square();
   M[MarkTriangle]=new DrawMark; M[MarkTriangle]->def_triangle_up();
   M[MarkDiamond] =new DrawMark; M[MarkDiamond]->def_diamond();
   M[MarkDot]     =new DrawMark; M[MarkDot]->def_dot();
}

static const char* linePattern[]={"",	//	solid	line
			"52",						//	long	dash         
			"5252",						//			dash
			"52525252",					//	short	dash         
			"11111111111111111111",		//			dotted
			"6111",						//	long	dash dot     
// XVT:  	"1414",						//	long	dash dot     
			"52125212",					//	short	dash dot		
					""
};

//////////////////////////////////////////////////////////////////////////////
// Layout of plot
//////////////////////////////////////////////////////////////////////////////
// FIRST WE DEFINE THE VALUES POSITIONING PLOT IN WINDOW:

int CPlotView::ydist=10;      // Ydist
int CPlotView::ynuml=15;      // Ynuml
int CPlotView::ynum =20;      // Ynum
int CPlotView::xnuml=50;      // Xnuml
int CPlotView::xnum =10;      // Xnum
int CPlotView::xl0  =20;      // xl0
int CPlotView::xll  =60;      // Xll
int CPlotView::ydelLegend=30; // ydelt
int CPlotView::ticklen=4;     // TickLen
int CPlotView::nprecision=4;// Nprecision
int CPlotView::leftmargin=0;
int CPlotView::topmargin=0;
int CPlotView::rightmargin=0;
int CPlotView::bottommargin=0;
int CPlotView::leftframe=0;
int CPlotView::topframe=0;
int CPlotView::rightframe=0;
int CPlotView::bottomframe=0;
int CPlotView::xlT=0;
int CPlotView::Tfontsize=0;
int CPlotView::Afontsize=0;
int CPlotView::Lfontsize=0;
int CPlotView::Nfontsize=0;

void CPlotView::CalculateLayout(const PLOTSCROPT* opt)
{
		
   Tfontsize=opt->FontTitle();
   Lfontsize=opt->FontLegend();
   Afontsize=opt->FontAxes();
   Nfontsize=opt->FontNumber();
		
   
   topmargin=opt->BorderTop();
     topframe=topmargin + Tfontsize + ydist;
   bottommargin=opt->BorderBottom();
     bottomframe=bottommargin +
		 Afontsize +
		 ynuml + ynum;
   leftmargin=opt->BorderLeft();
     leftframe=leftmargin + xnuml + xnum;
   rightmargin=opt->BorderRight();
     xlT=2*xl0 + xll;
     rightframe=xlT + 
		opt->WidthLegend() +
		rightmargin;

	pltrct.left   =winrct.left   + leftframe,
	pltrct.top    =winrct.top    + topframe,
	pltrct.right  =winrct.right  - rightframe,
	pltrct.bottom =winrct.bottom - bottomframe;
	      
	PLTRCT.left   =pltrct.left;
	PLTRCT.top    =pltrct.top;
	PLTRCT.right  =pltrct.right;
	PLTRCT.bottom =pltrct.bottom;
   
	xwid=pltrct.right  - pltrct.left;
	yhgt=pltrct.bottom - pltrct.top;
   
}
//////////////////////////////////////////////////////////////////////////////
// 4. Drawing Axes, Curves and Legend
/////////////////////////////////////////////////////////////////////////////
void CPlotView::DrawAxes()
{
// DRAW AXES TICS & NUMBERS
      MakeAxis(FALSE,PyMIN,PyMAX,yTICKP,yTICKD);
      MakeAxis(TRUE ,PxMIN,PxMAX,xTICKP,xTICKD);
}

void CPlotView::MakeAxis(BOOLEAN on_X_axis,
		   REAL rmin, REAL rmax, REAL pos, REAL dist)
{
   REAL delta, fact, TickNum;
   CPoint tic[2];
   CPoint TXT;
   
   dist=fabs(dist);
   if (dist == 0.0) dist=(rmax - rmin) / 2.0;
   
   if (dist < (rmax-rmin)) delta=dist;
   else delta=rmax-rmin; delta=delta/10000;
  
   tic[0].x=leftframe;						// draw axes
   tic[0].y=topframe + yhgt;
   if (on_X_axis) {							// X-AXIS
	   tic[1].x=tic[0].x+xwid;
	   tic[1].y=tic[0].y;
   }
   else {									// Y-AXIS
	   tic[1].y=tic[0].y-yhgt;
	   tic[1].x=tic[0].x;
   }
   pen_tic.polyline(tic,2);

   while (pos < rmax+delta) pos=pos+dist;	// draw tics and numbers
   pos=pos-dist;
   
   if (on_X_axis) {
     fact=xwid/(rmax-rmin);					// calculate position
     tic[0].y=topframe + yhgt + ticklen;
     tic[1].y=topframe + yhgt;
     TXT.y   =tic[1].y + ynum;
   }
   else {
     fact=yhgt/(rmax-rmin);
     tic[0].x=leftframe - ticklen - 1;
     tic[1].x=leftframe;
     TXT.x   =leftframe - xnum;
   }
   
   rmax=pos-rmin;
   while (rmin-delta < pos) {

      if (fabs(pos) < delta) TickNum=0; 
	  else TickNum=pos;
      if (on_X_axis) {					// X-AXIS
	 tic[0].x=leftframe + int(rmax*fact);
	 tic[1].x=tic[0].x;
	 TXT.x=tic[0].x;
     pen_tic.polyline(tic,2);
	 axis_label.Number(TXT,TickNum,nprecision,Center);
      }
      else {							// Y-AXIS
	 tic[0].y=topframe + yhgt - int(rmax*fact);
	 tic[1].y=tic[0].y;
         pen_tic.polyline(tic,2); 
	 TXT.y=tic[0].y + Nfontsize/2;
	 axis_label.Number(TXT,TickNum,nprecision,Right);
      }
      pos=pos - dist; rmax=rmax - dist;
   }
}

void CPlotView::DrawAllCurves()
{

// DRAW LEGEND SYMBOLS befor clipping
	int pos=0;
	for (int j=0; j < Ncurves; j++) 
	{
		int i=crvinx[j];
		if ((P->Curve(i))->Legend())
		if (strlen((P->Curve(i))->Legend())) 
		{
			OneLegend(pltrct, pos, i, PD->Y(i));
			pos++;
		}
	}
// DRAW CURVES:
//      xvt_dwin_set_clip(get_win(),&PLTRCT);      // clip  XVT window to curves
	for (j=0; j < Ncurves; j++) 
	{
		int i=crvinx[j];
  
		// draw X/Y:
		OneCurve(pltrct, i, PD->Y(i),FALSE);
	  
		// draw optional envelopes:
		OneCurve(pltrct, i, PD->Yminus(i),TRUE);
		OneCurve(pltrct, i, PD->Yplus(i),TRUE);
	  
	}
}
/////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////
void CPlotView::OneCurve(CRect& pltrct, int icurve, REAL* Y, BOOLEAN envelope) {
	CARDINAL color;
	int linewidth;
	int markstyle;
	int singlepoint;

	LINESTYLE linestyle=(P->Curve(icurve))->LineStyle();
//	int penstyle=penStyle[linestyle];
	REAL period=70;                                      // fixed period length
//   REAL period=(pltrct.right() - pltrct.left() ) / 9; // variable period length

	if (envelope) 
	{
		linewidth=1;
		linestyle=LineDotted;
		if ((P->Curve(icurve))->LineActive())
			 color=aqcol[(P->Curve(icurve))->LineColor()];
		else color=aqcol[(P->Curve(icurve))->MarkColor()];
	}
	else 
	{
		linewidth=(P->Curve(icurve))->LineWidth();
		color=aqcol[(P->Curve(icurve))->LineColor()];
	}
   
	int Clindx, NotClipped;

	ClipXArray(PD->X(icurve),PxMIN,PxMAX,PD->NumPts(icurve),
				Clindx,NotClipped);
	singlepoint=NotClipped==1;
	if (Y) 
	{
		AnyCurve curve;
		LineTool pen(pltrct,this);

		int target=curve.Set(NotClipped,&PD->X(icurve)[Clindx],&Y[Clindx],pltrct,
			PxMIN,PxMAX,PyMIN,PyMAX);
		if (1 < target) return;

		int lineact; lineact=(P->Curve(icurve))->LineActive();
		int markact; markact=(P->Curve(icurve))->MarkActive();
		if (!singlepoint && (envelope || lineact)) 
		{
			pen.SetPen(linewidth,color);
			pen.customcurve(curve,linePattern[linestyle],period);
//			pen.curve(curve);
		}
		if (!envelope && ( markact || (singlepoint && lineact))) 
		{
			CARDINAL markcolor=aqcol[(P->Curve(icurve))->MarkColor()];
			if (markact) 
			{
				markstyle=(P->Curve(icurve))->MarkStyle();
			}
			else 
			{
				markstyle=MarkDot;
			}
			M[markstyle]->Set((P->Curve(icurve))->MarkSize());
			M[markstyle]->SetPen(1,markcolor, PS_SOLID);
			M[markstyle]->MarkCurve(curve.pCurve(),1,0,pltrct);
		}
	}	// end if Y
}
void CPlotView::OneLegend(CRect& pltrct, int& pos, int icurve, REAL* Y) 
{
	if (!Y) return;
	REAL period=70;
	CPoint symb[2];
	int offset=topframe + ydist + pos * ydelLegend;
	symb[0].x=leftframe+xwid+xl0;
	symb[0].y=offset;
	symb[1].x=symb[0].x+xll;
	symb[1].y=offset;
 	LineTool line(this);
	if ((P->Curve(icurve))->LineActive())
	{
		line.SetPen((P->Curve(icurve))->LineWidth(),
					aqcol[(P->Curve(icurve))->LineColor()]);
		line.custompolyline(&symb[0],2,linePattern[(P->Curve(icurve))->LineStyle()],period);
//		line.line(symb[0],symb[1]);
	}
	if ((P->Curve(icurve))->MarkActive()) 
	{
		int markstyle=(P->Curve(icurve))->MarkStyle();
			M[markstyle]->SetPen((P->Curve(icurve))->LineWidth(),
								 aqcol[(P->Curve(icurve))->MarkColor()],
								 PS_SOLID);
			M[markstyle]->Set((P->Curve(icurve))->MarkSize());
			symb[0].x=symb[0].x + xll/2;
			M[markstyle]->ClipRct(winrct);
			M[markstyle]->Mark(symb[0]);
	}
	if ((P->Curve(icurve))->Legend())
	{
		int x=pltrct.right + xlT;
		int y=offset;
		plot_legend.text(x,y,(P->Curve(icurve))->Legend());
	}
}
void CPlotView::GetCurveInfo()
{
   int i;
   BOOLEAN exist_curves=FALSE;
   
   RestoreExtrema();

   const buflen=100;
   char buf[buflen];
   int plotnum=1;
   std::ostrstream oss(buf,sizeof(buf),std::ios::out|std::ios::trunc);
   oss << ProgName()
       << " [" << plotnum
       << "] - "
	   << std::ends;
   strcat(buf,P->Symbol());

   
   ncurves=P->NumCurves();
   Ncurves=0;
   crvinx =new int [ncurves];
   
   for (i=0; i< ncurves; i++) {
      if ( PD->IsCurve(i) ) crvinx[Ncurves++]=i;
   } // end for i

   if ( Ncurves == 0 ) { // give it up:
//	close();
//	xvt_dm_post_note("No Curves to Plot...");
	return;
   }
}

void CPlotView::SetExtrema(REAL xmin, REAL xmax, REAL xticpos, REAL xticdist,
	                       REAL ymin, REAL ymax, REAL yticpos, REAL yticdist)
{
	PxMIN=xmin; PxMAX=xmax; PyMIN=ymin; PyMAX=ymax;
	xTICKP=xticpos; xTICKD=xticdist;
	yTICKP=yticpos; yTICKD=yticdist;
}

void CPlotView::RestoreExtrema()
{
	SetExtrema(P->xMin(), P->xMax(), P->xTickPos(),	P->xTickDist(),
		       P->yMin(), P->yMax(), P->yTickPos(), P->yTickDist());
}

//////////////////////////////////////////////////////////////////////////////
// 5. Interaction with other modules
//////////////////////////////////////////////////////////////////////////////

CARDINAL CPlotView::ValidLayout(const PLOTSCROPT* opt) {
 // returns 0 if ok, else bad margins
 ValidLayout(opt);
 int wid=opt->WinWidth();
 int hgt=opt->WinHeight();

 wid=wid - leftframe - rightframe - 40; // 40 used by create_scratch
 hgt=hgt - topframe - bottomframe - 40; // 40 used by create_scratch

 CARDINAL result=0;
 if (wid < 1) result=1 ;
 if (hgt < 1) result=result + 2;
 return result;
}

void CPlotView::SetWindowSize(int width, int height)
{
	CRect frame;
	CFrameWnd* plotframe=GetParentFrame( );
	plotframe->GetWindowRect(frame);
	aqapp.MainFrame->ScreenToClient(frame);


	CRect toolbarframe;	BOOLEAN visible=TRUE;
	aqapp.MainFrame->ToolDim(&toolbarframe, visible);
	aqapp.MainFrame->ScreenToClient(toolbarframe);
	if (visible && toolbarframe.top < 0) 
		frame.top=frame.top - toolbarframe.bottom;
	frame.right=frame.left + width;
	frame.bottom=frame.top + height;
	plotframe->MoveWindow(frame);
}

/*
int CPlotView::ReplacePlotPtr(PLOT* oldplot, PLOT* newplot)
{
	CPlotDoc* pDoc = (CPlotDoc*)m_pDocument;
	if (pDoc->plot != oldplot) return 0;
	if (newplot == NULL) return 2;
	if (pDoc->GetPlotData(newplot) == OK) return 1; else return 2;
}
*/

void CPlotView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (bActivate && pActivateView == this)
	{
		aqapp.MainFrame->plotmanager.PlotIsOnTop(this);
//		SetPlotTitle(FALSE);
	}
//	else if (pDeactiveView == this)
//		SetPlotTitle(TRUE);
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CPlotView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	if (aqapp.MainFrame->currentplotview == this)
		aqapp.MainFrame->currentplotview=NULL;
	inhibitMouse=FALSE;

	aqapp.MainFrame->plotmanager.RemovePlot(this);

	delete crvinx;
	delete M[MarkCircle];
	delete M[MarkSquare];
	delete M[MarkTriangle];
	delete M[MarkDiamond];
	delete M[MarkDot];
	delete M;
#ifdef _DEBUG
//	aqapp.getLeakState(0,"End of OnDestroy Window");
#endif
}

/////////////////////////////////////////////////////////////////////////////
// 6. CPlotView diagnostics
//////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CPlotView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPlotView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

void CPlotView::OnSize(UINT nType, int cx, int cy) 
{
//	BOOLEAN icon=IsIconic();
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
/*
	if (nType == SIZE_MINIMIZED)
		
		aqapp.MainFrame->plotmanager.AddIcon(TRUE);
	else if (icon)
		aqapp.MainFrame->plotmanager.AddIcon(FALSE);
*/
}

//////////////////////////////////////////////////////////////////////////////
// 7. Copy to Clipboard, LButtonDblClick
//////////////////////////////////////////////////////////////////////////////

void CPlotView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	BOOL t=CPlotView::IsIconic();
	if (!hasFocus || t) return;

    CBitmap     cBmp;
    CClientDC   cWndDC(this);   // View is an hWnd, so we can use "this"
    CDC         cMemDC;         // Handle to a memory DC
    CRect     rect;             // For storing the size of the window
 
    cMemDC.CreateCompatibleDC(&cWndDC); // Create the memory DC.
 
    GetWindowRect(rect);         // Get the size of the window
    // Here we are going to do a little drawing so we can see a line.
//    cWndDC.MoveTo(0,0);
//    cWndDC.LineTo(rect.Width(),rect.Height());

	// get rid of window right and bottom borders: 4 empirically found
	rect.DeflateRect(0,0,4,4); 

    cBmp.CreateCompatibleBitmap(&cMemDC, rect.Width(),rect.Height() );
    // Keep the old bitmap
    CBitmap* pOldBitmap = cMemDC.SelectObject(&cBmp);
 
    cMemDC.BitBlt(0, 0, rect.Width(),rect.Height(), &cWndDC, 0, 0, SRCCOPY);
//    cMemDC.BitBlt(0, 0, rect.Width(),rect.Height(), &cWndDC, 0, 0, SRCCOPY);
 
    // here are the actual clipboard functions.
    AfxGetApp()->m_pMainWnd->OpenClipboard() ;
    EmptyClipboard() ;
    SetClipboardData (CF_BITMAP, cBmp.GetSafeHandle() ) ;
//    SetClipboardData (CF_BITMAP, cBmp.GetSafeHandle() ) ;
    CloseClipboard () ;
        // next we select the old bitmap back into the memory DC
        // so that our bitmap is not deleted when cMemDC is destroyed.
        // Then we detach the bitmap handle from the cBmp object so that
        // the bitmap is not deleted when cBmp is destroyed.
    cMemDC.SelectObject(pOldBitmap);
    cBmp.Detach();
}

void CPlotView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPlotView::OnEditCopy();
	// CScrollView::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// 8. OnChar, OnMouse..., OnLButton..., Zooming
/////////////////////////////////////////////////////////////////////////////
static const UINT ArrowLeft =37;
static const UINT ArrowUp   =38;
static const UINT ArrowRight=39;
static const UINT ArrowDown =40;
static const UINT Control   =17;
static const UINT Shift     =16;
static const UINT Ctrlpix   =10;
static const UINT Shftpix   =25;
static       UINT Ctrl;
static       UINT Shft;
void CPlotView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int pixels=1;
	if (Ctrl)    pixels=20;
	int x=0;
	int y=0;
	GetCursorPos(&SCREENPNT);
	switch(nChar)
	{
	case ArrowLeft:	x=-pixels;
		break;
	case ArrowUp:	y=-pixels;
		break;
	case ArrowRight:x=pixels;
		break;
	case ArrowDown:	y=pixels;
		break;
	case Control: Ctrl=1;	  pixels=0;
		break;
	case Shift:	  Shft=1;	  pixels=0;
		          OnLButtonDown(0,lastpoint);
		break;
	default: Ctrl=1; Shft=1;  pixels=0;
		break;
	}
	if (pixels) 
	{
		lastpoint=lastpoint + CPoint(x,y);
		SCREENPNT=SCREENPNT + CPoint(x,y);
//		SetCursorPos(SCREENPNT.x , SCREENPNT.y);
		OnMouseMove(0, lastpoint);
	}
	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPlotView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == Control) Ctrl=0;
	if (nChar == Shift)
	{
		if (Shft) OnLButtonUp(0,lastpoint); 
		Shft=0;
	}
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

static long WCounter=0;
LRESULT CPlotView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
#ifdef _DEBUG
	BOOLEAN localdebug=FALSE;
	if (localdebug) {
		WCounter++;
 		if (message == WM_NCHITTEST) TRACE("PLOTFRAME %d WM_NCHITTEST %d wParam %d\n", WCounter,message, WM_NCHITTEST, wParam);
		if (message == WM_MOUSEMOVE) TRACE("PLOTFRAME %d WM_MOUSEMOVE %d wParam %d \n",WCounter, WM_NCHITTEST, wParam);
		if (message == WM_NCMOUSEMOVE) TRACE("PLOTFRAME %d WM_NCMOUSEMOVE %d  wParam %d\n",WCounter, WM_NCMOUSEMOVE, wParam);
	}
#endif

	if (message == WM_NCMOUSEMOVE) {
		RemoveCross(1);
	}
	if (message == WM_NCHITTEST && this != aqapp.MainFrame->currentplotview)
		if (aqapp.MainFrame->currentplotview	!= NULL) 
			aqapp.MainFrame->currentplotview->RemoveCross(2);

	LRESULT res=CScrollView::DefWindowProc(message, wParam, lParam);
	return res;
}

void CPlotView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	EraseCross();
	CrossON=FALSE;
	CrossCoordinates[0]=CPoint(0,0);
	inhibitMouse=TRUE;
	hasFocus=FALSE;
}

void CPlotView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	hasFocus=TRUE;
	aqapp.MainFrame->currentplotview=this;
}

void CPlotView::RemoveCross(int cause) {
	if (!CrossON) return;
#ifdef _DEBUG
/*
	TRACE("Remove Crose ");
	switch(cause) {
		case 1: TRACE("CPlotView WM_NCMOUSEMOVE\n");
			break;
		case 2: TRACE("CPlotView without Focus WM+NCHITTEST \n");
			break;
		case 3: TRACE("\n");
			break;
		case 4: TRACE("\n");
			break;
		case 5: TRACE("CMainFrame WM_NCHITTEST\n");
			break;
	default: TRACE(" unknown cause\n");
	}
*/
#endif

	axis_label.text(x_CoordText,"XXXXXXXX",Center,0,white);
	axis_label.text(y_CoordText,"YYYYYYYY",Right,0,white);
	// erase coordinate cross
	draglines.InvertLine(CrossCoordinates[0],CrossCoordinates[2].x, TRUE,4);
	draglines.InvertLine(CrossCoordinates[1],CrossCoordinates[2].y, FALSE,4);
	CrossON=FALSE;
	CrossCoordinates[0]=CPoint(0,0);
}

void CPlotView::EraseCross() {
	// erase coordinate numbers
	axis_label.text(x_CoordText,"XXXXXXXX",Center,0,white);
	axis_label.text(y_CoordText,"YYYYYYYY",Right,0,white);
	if (CrossON && hasFocus)
	{	// erase coordinate cross
		draglines.InvertLine(CrossCoordinates[0],CrossCoordinates[2].x, TRUE,4);
		draglines.InvertLine(CrossCoordinates[1],CrossCoordinates[2].y, FALSE,4);
		if (!aqapp.MainFrame->LinesEnabled) {
			CrossON=FALSE;
			CrossCoordinates[0]=CPoint(0,0);
		}
	}
}

void CPlotView::DrawCross() {
	if (hasFocus) {
		CrossON=TRUE;
		if (WithinPlot) {
			CrossCoordinates[0]=CPoint(pltrct.left,lastpoint.y);
			CrossCoordinates[1]=CPoint(lastpoint.x,pltrct.top);
			CrossCoordinates[2]=CPoint(pltrct.right,pltrct.bottom);
		}
		else {
			CrossCoordinates[0]=CPoint(winrct.left,lastpoint.y);
			CrossCoordinates[1]=CPoint(lastpoint.x,winrct.top);
			CrossCoordinates[2]=CPoint(winrct.right,winrct.bottom);
		}
		// draw coordinate cross
		draglines.InvertLine(CrossCoordinates[0],CrossCoordinates[2].x, TRUE,4);
		draglines.InvertLine(CrossCoordinates[1],CrossCoordinates[2].y, FALSE,4);
	}
}

void CPlotView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!hasFocus) return;

	lastpoint=point;
	if ((inhibitMouse || aqapp.MainFrame->coordMenuOff) && !CrossON) {
		return;
	}
// moving numbers ?
//	x_CoordText.x=point.x;
//	y_CoordText.y=point.y;
// clear:
	WithinPlot=!((point.x <= pltrct.left) || (pltrct.right <= point.x) 
		|| (point.y <= pltrct.top) || (pltrct.bottom <= point.y));

	Color numcolor=red;
	if ((Dragging) 
		 && aqapp.MainFrame->CoordNumbersEnabled
		 )
	{
		draglines.InvertRct(Drag0,Drag1,3);			// erase rectangle
		Drag1=point;
		draglines.InvertRct(Drag0,Drag1,3);		// draw rectangle

		numcolor=gray;
	}
//	if (aqapp.MainFrame->LinesEnabled && (CrossCoordinates[0] != CPoint(0,0)))
		// erase coordinate cross
	EraseCross();
	  
// calculate:
	double X=PxMIN + (point.x - pltrct.left)   * X_factor;
	double Y=PyMIN + (pltrct.bottom - point.y) * Y_factor;
// write:
	if (aqapp.MainFrame->CoordNumbersEnabled)
	{	// write coordinate numbers
		axis_label.number(x_CoordText, X, coord_Precision,Center,0,numcolor);
		axis_label.number(y_CoordText, Y, coord_Precision,Right,0,numcolor);
	}
	if (aqapp.MainFrame->CrossEnabled) HCURSOR ok=SetCursor(CrossCursor);
	if (aqapp.MainFrame->LinesEnabled)
	{
		DrawCross();
	}

	CView::OnMouseMove(nFlags, point);
}

void CPlotView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!hasFocus) return;
	lastpoint=point;
	Dragging=TRUE;
	Drag0=point; Drag1=point;
	draglines.InvertRct(point,point,3);

	CView::OnLButtonDown(nFlags, point);
}

void CPlotView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!hasFocus) return;
	lastpoint=point;
	CPlotDoc* D = (CPlotDoc*)m_pDocument;
	draglines.InvertRct(Drag0,Drag1,3);
	CRect drag(Drag0,Drag1);
	CrossON=FALSE;
	CrossCoordinates[0]=CPoint(0,0);
	inhibitMouse=TRUE;										 	 // JR 16.06.97

	if (drag.Width() == 0 || drag.Height() == 0 || !Dragging)	 // JR 16.06.97
//	if (drag.Width() <= 0 || drag.Height() == 0 || !Dragging)
	{
	// Unzoom:
		RestoreExtrema();
		Invalidate();
		return;
	}
	Dragging=FALSE;
	int zxmin, zxmax, zymin, zymax;
	if (Drag0.x < Drag1.x)
	     { zxmin=Drag0.x; zxmax=Drag1.x; }
	else { zxmin=Drag1.x; zxmax=Drag0.x; }

  	if (Drag0.y > Drag1.y)
	     { zymin=Drag0.y; zymax=Drag1.y; }
	else { zymin=Drag1.y; zymax=Drag0.y; }

	const int MinTicNo=2;
	const int TicDivisor=2;
	double X0=PxMIN + (zxmin - pltrct.left) * X_factor;
	double X1=PxMIN + (zxmax - pltrct.left) * X_factor;
	double fx=P->xTickDist();
	while (((X1 - X0)/fx) < MinTicNo) fx=fx/TicDivisor;
	double Y0=PyMIN + (pltrct.bottom - zymin) * Y_factor;
	double Y1=PyMIN + (pltrct.bottom - zymax) * Y_factor;
	double fy=P->yTickDist();
	while (((Y1 - Y0)/fy) < MinTicNo) fy=fy/TicDivisor;;
	SetExtrema(X0, X1, P->xTickPos(), fx,
		       Y0, Y1, P->yTickPos(), fy);
	Invalidate();
//	CView::OnLButtonUp(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
// 9. Scrolling
/////////////////////////////////////////////////////////////////////////////

void CPlotView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
//	if (pSender != this) return;
	SetScrollSizes( MM_TEXT, CSize(1,1));
}

/*
void CPlotView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CPlotDoc* D = (CPlotDoc*)m_pDocument;
	int o=D->ScrI.scrpos.x;
	int pagewidth=int(double(D->ScrI.oldwinrct.Width())/D->ScrI.xfact);
	CPoint c=GetScrollPosition();
	int n=c.x;
	int line=20;
	int last=D->ScrI.scrsize.cx - pagewidth;
	switch(nSBCode)
	{
    case SB_LEFT:		n=1;			// Scroll to far left.
		break;
    case SB_ENDSCROLL:	n=last;			// End scroll.
						     
		break;
    case SB_LINELEFT:	n=n=line;		// Scroll left.
		break;
    case SB_LINERIGHT:	n=n+line;		// Scroll right.
		break;
    case SB_PAGELEFT:	n=n-pagewidth;	// Scroll one page left.
		break;
    case SB_PAGERIGHT:	n=n+pagewidth;	// Scroll one page right.
		break;
    case SB_RIGHT:		n=last;			// Scroll to far right.
		break;
    case SB_THUMBPOSITION:	// Scroll to absolute position. The current position
							// is specified by the nPos parameter.
		break;
    case SB_THUMBTRACK:		// Drag scroll box to specified position. The 
							// current position is specified by the nPos parameter.	
		break;
	}
	if (n < 1) n=1;
	if (last < n) n=last;
	D->ScrI.scrpos.x=n;
//	ScrollToPosition(D->ScrI.scrpos);
	CView::OnHScroll(SB_THUMBPOSITION, n, pScrollBar);
}
*/

void CPlotView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlotView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}
