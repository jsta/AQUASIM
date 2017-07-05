#ifndef PLOTVW_H
#define PLOTVW_H


#define PLOTWV_DEBUG

/////////////////////////////////////////////////////////////////////////////
// PlotView.h : header file
//
//
//              date:       person:           comments:
//
// creation:    28.05.96    Juerg Ruchti
// revisions:   
//
/////////////////////////////////////////////////////////////////////////////
// CPlotView view

//#include "aqcore\all.h"
#include "aqcore\aquasys.h"
#include "drmark_w.h"
#include "drtext_w.h"


class CPlotView : public CScrollView
/*
	class CPlotView displays AQUASIM data as plots.

	This header file declares variables and procedures 
	in the following sections:

	1. The relation of some variables of section 5
	   to the display screen
	2. Variables for debugging
	3. Plot and Plot Data  Variab;es
	4. Drawing: colors, pen and curve info
	5. Layout of plot: variables shown in section 1
	6. Pens	and Fonts
	7. Drawing Axes
	8. Curve drawing
	9. Text drawing
   10. Interaction with user
   11. Interaction with other modules of program
*/

// 1. Some variables and their meaning on the display screen
// =========================================================
/*
    -----------------------------------------------------------------------------
    |              |TopMargin                                                   |
    |                                                                           |
    |                             |Ydist                                        |
    |            ------------------------------------                           |
    |            |             xwid                 |  x---x legend text        |
    |LeftMargin  |                                  |                    |ydelL |
    |--          |                                  |  x---x legend text |      |
    |            |                                  |                           |
    |           -|                                  |--     --                  |
    |     TickLen|                                  |Xl0    Xl0                 |
    |            |                             yhgt |  -----                    |
    |            |                                  |  Xll                      |
    |            |                                  |-------                    |
    |     Xnum   |                                  |  XlT    ------            |
    |         ---|                                  |         Xtlen             |
    |  -------   |                                  |               ------------|
    |   Xnuml    |                                  |                RightMargin|
    |            ------------------------------------                           |
    |                                        |                                  |
    |                                        |Ynum                              |
    |                    |Ynuml                                                 |
    |                                 |BottomMargin                             |
    -----------------------------------------------------------------------------
*/
{
protected:
	CPlotView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPlotView)

// Attributes
private:

// 2. Variables for debugging
// ==========================
#ifdef PLOTWV_DEBUG
	TextTool test_text;
	TextTool EAWAG;
	TextTool P_Reichert;
	void Prelude();
#endif

// 3. Plot and Plot Data Variables
// ===============================
	PLOT* P;
	PLOTDATA* PD;

// 4. Drawing: colors, pen and curve info
// ======================================
// constants
	DColors  COL;			// color database and utilities
    COLORREF aqcol[8];		// map colors
//    int      penStyle[3];	// map plotstyle

	void InitConstants();
	void GetCurveInfo();

// data
    int ncurves;		// number of curves in P
    int Ncurves;		// effective number of curves
    int* crvinx;		// Ncurve curve indices

// 5. Layout of plot
// =================
// constants
          int xwid, yhgt;	// Width and Height of Plot Frame
   static int topmargin;	// Top Border
   static int topframe;
       CPoint ytit;			// Y Ordinate of Main Title
   static int ydist;           // space from Main Title to plot frame
   static int leftmargin;	// Left Border
   static int leftframe;
       CPoint xord;			// Origin of Ordinate Title
   static int xnuml;		// Distance of Ordinate Title to xnum
   static int xnum;			// Abscissa of right most digit in Numbers along Ordinate
   static int rightmargin;	// Right Border
   static int rightframe;
   static int xl0;			// Offset of Legend Symbol and of Legend Text
   static int xll;			// Length of Legend Symbol
   static int xlT;			// Ordinate of left most character in Legend Text
   static int xtlen;		// Length of Legend Text
   static int ydelLegend;	// vertical Distance from Legend to Legend
   static int bottommargin;	// Bottom Border
   static int bottomframe;
       CPoint yabs;			// Origin of Abszissa Title
   static int ynum;			// Y offset of numbers along Abszissa
   static int ynuml;		// Y distance between Abszissa Title and ordinate numbers
   static int ticklen;		// length of tick
   static int nprecision;	// number precision along axes
     
// data
		CRect winrct;		// client area, then reduced
		CRect pltrct;		// plot area
		CRect PLTRCT;

   void CalculateLayout(const PLOTSCROPT* opt);

// 6. Pens	and Fonts
// ==================
   LineTool	pen_n;			// normal black pen
   LineTool pen_tic;		// pen to draw tics on axes

   TextTool plot_title;
   TextTool plot_legend;
   TextTool xaxis_title;
   TextTool yaxis_title;
   TextTool axis_label;
   TextTool warning_txt;;

   void RegisterDrawTools();
   void SetFontsAndWriteTitles();
   void MouseInit();

   static int Tfontsize;	// Title
   static int Afontsize;	// Axes
   static int Nfontsize;	// Numbers
   static int Lfontsize;	// Legend 

// 7. Drawing Axes
// ===============

     DrawMark** M;
     int Nmarkers;

	 void DrawAxes();
     void MakeAxis(BOOLEAN on_X_axis,
                   REAL rmin, REAL rmax, REAL pos, REAL dist);
    
// 8. Curve drawing
// ================

     void OneLegend(CRect& pltrct, int& pos, int icurve, REAL* Y);
     void OneCurve(CRect& pltrct, int icurve, REAL* Y, BOOLEAN envelope);
	 void DrawAllCurves();
	 

// 9. Text drawing
// ===============
     
//   DrawTools DTtxt;

   char selectedfont;
   int leading, ascent,descent,
     fontheight, fsize; // current values defined by SetFont
   int fontwidth;       // current value defined by (SetFont and) PixWidth

// 10. Interaction with user
// =========================
// Mouse movement and coordinate display, zooming

   // General
   BOOLEAN inhibitMouse;	// to be set true during initialisation
   LineTool draglines;
   double  X_factor;		//  MouseInit, used in OnMouseMove
   double  Y_factor;		//  MouseInit, used in OnMouseMove
   CPoint  lastpoint,SCREENPNT;
   // Zooming
   REAL PxMIN, PxMAX, xTICKP, xTICKD, PyMIN, PyMAX, yTICKP, yTICKD;
   void SetExtrema(REAL xmin, REAL xmax, REAL xticpos, REAL xticdist,
	               REAL ymin, REAL ymax, REAL yticpos, REAL yticdist);
   void RestoreExtrema();	// unzoom
   BOOLEAN Dragging;		// drom OnMauseDown till 
   CPoint  Drag0, Drag1;	// defined while dragging mouse

   // Cordinate numbers
   BOOLEAN WithinPlot;
   CPoint  x_CoordText;		// defines place of displaying current value
   CPoint  y_CoordText;		// defines place of displaying current value
   int     coord_Precision;	// defines precision for displaying current value

   // Coordinate lines
   BOOLEAN hasFocus;
   BOOLEAN CrossON;
   void DrawCross();
   CPoint  CrossCoordinates[3];		// coordinate cross
   // Cursor(s)
   HCURSOR ArrowCursor, CrossCursor, WaitCursor, CurrentCursor;
   void EraseCross();
public:
	void RemoveCross(int cause);


// 11. Interaction with other modules of program
// =============================================
public:

	void SetWindowSize(int width, int height);
	void Redraw();
   
     static CARDINAL ValidLayout(const PLOTSCROPT* opt);
     					// returns 0: ok
     					//         1: error for x axis
     					//         2: error for y axis
     					//         3: error for x & y axis

/*
	 int ReplacePlotPtr(PLOT* oldplot, PLOT* newplot);
		// returns 0: oldplot != pDoc->plot
		// returns 1: oldplot == pDoc->plot,
	    //			  OK == pDoc->GetPlotData(newplot)
		// returns 2: oldplot == pDoc->plot,
		//			  OK != pDoc->GetPlotData(newplot)
*/
	 void SetPlotTitle();
private:
	CString PlotFileName;
	CString PlotTitle;
	BOOLEAN StaleFile;
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

protected:
	virtual ~CPlotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlotView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // PLOTVW_H
