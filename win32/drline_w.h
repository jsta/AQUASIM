
#ifndef DRLINE_W_H
#define DRLINE_W_H

#include "dcolor_w.h"

class LineTool;
////////////////////////////////////////////////////////////////////////
class AnyCurve {	friend LineTool;
////////////////////////////////////////////////////////////////////////
// AnyCurve: J. Ruchti
// Creation: 22.05.1996
// Updated :
//
// Description:
//
//		In order to project real curves into integer
//		curves bounded by an integer rectangle.
//		Friend class LineTool can draw the integer curve.
//
public:

	AnyCurve(); ~AnyCurve();
// Usage:
//	1. Projection of curve:
//
int Set(int n, double* x, double* y, CRect target,  
				double xMin=0.0, double xMax=0.0, 
				double yMin=0.0, double yMax=0.0);
int Set(int n, int   * x, int   * y, CRect target,
				int xMin=0,      int xMax=0, 
				int yMin=0,      int yMax=0);
int Set(int n, long  * x, long  * y, CRect target,
				long xMin=0,     long xMax=0, 
				long yMin=0,     long yMax=0);
//      int Set(int n, double* x, double* y, CRect target);
//		Given two real vectors x and y of length n
//		these vectors will be projected
//		into the integer target rectangle.
//		Specifying xMin, xMax, yMin, yMax will overrule
//		automatic determination of extrema.
//		Result:	Set = 0: OK
//				Set = 1: n < 2;
//				Set = 2: zero width target
//				Set = 3: zero heigth target
//		Note:   Set may be called repeatedly, but 
//				subsequent calls will destroy the 
//				previous projection.
//
//  2. Access to curve after projection:
//
	AnyCurve* pCurve() {return this; }		
	CPoint*   Curve()  { return p; }
	const int Npts()   { return N; }
//
//	3.	Drawing the curve: 
//			LineTool::curve(AnyCurve::curve)
//
//	4.	Marking a curve:   
//			DrawMark::MarkCurve(AnyCurve* Curve());
//		e.g.	AnyCurve curve;
//				curve.Set(n, &y[0] , &y[0], crect);
//				MarkCurve(curve.pCurve());
////////////////////////////////////////////////////////////////////////

private:
	double	xmin, xmax, xdiff,
			ymin, ymax, ydiff;
	int		n;
	void	minmax(double* x, double& rmin, double& rmax);

protected:
	int		Xmin, Xmax, Xdiff,
			Ymin, Ymax, Ydiff;
 	int		N;
	CPoint*	p;
};

////////////////////////////////////////////////////////////////////////
class LineTool {
////////////////////////////////////////////////////////////////////////
// AnyCurve: J. Ruchti
// Creation: 22.05.1996
// Updated :
//
// Description:
//
public:
// Useage:
// 1. Declare one or more LineTool objects in *View.h:
//		LineTool object1;
	LineTool(CWnd* parentWindow=NULL);
	LineTool(CRect cliprect, CWnd* parentWindow=NULL);
	~LineTool();
//		cliprect defines the clipping rectangle.
// 2. In *View.cpp attach each object to the current Window.
//		You can do this once, e.g. in InitView():
//		object1.attachToWindow(this);
	void attachToWindow(CWnd* parentWindow);
//		If you declare a LineTool in *View.cpp, you can
//		attach it in the declaration:
//			LineTool(this);
//		You can also redefine the clipping rectangle
	void ClipRct(CRect cliprect);
//
// 3. Preparing to Draw
//
//		Consider to define width, color and style
//		of the pen prior to drawing: the various
//		Set... procedures let you do this.
	void SetPen(int width=1, COLORREF  color=0, int style=PS_SOLID);
	void SetPen(int width=1, Color color=black, int style=PS_SOLID);

	void SetStyle(int style);
	void SetWidth(int width);
	void SetColor(COLORREF foreground, COLORREF background);
	void SetColor(Color foreground, Color background=white);
//
//		If you want to draw REAL data, then prepare them
//		using AnyCurve::Set
//
// 4. Drawing
//
//		curve(AnyCurve curve) let's you draw REAL data
//		prepared by AnyCurve:Set.
	BOOLEAN curve    (AnyCurve& curve);
	BOOLEAN customcurve   (AnyCurve& curve,  
					  const char* pattern="", double period=1);
//
//		line, polyline, polygon and rectangle let you draw
//		integer data.
//		Note that the fill option is not yet implemented.
	BOOLEAN line     (CPoint  p,   CPoint P);
	BOOLEAN polyline (CPoint* line, int N);
	BOOLEAN custompolyline(CPoint* line, int N,  
						const char* pattern="", double period=0);
	BOOLEAN polygon  (CPoint* line, int N,  BOOLEAN fill=FALSE);
	BOOLEAN rectangle(CRect    rect,        BOOLEAN fill=FALSE);
	BOOLEAN rectangle(CPoint p0, CPoint p,  BOOLEAN fill=FALSE);
	BOOLEAN rectangle(int x0, int y0, 
		                      int x, int y, BOOLEAN fill=FALSE);
	BOOLEAN circle(CPoint center,  int radius);
	BOOLEAN circle(int cx, int cy, int radius);

//		to be used for dragging line on mousemove and mouseup
	void    InvertLine(CPoint p0, int p1, 
						BOOLEAN horizontal=TRUE,int skips=1);
	void    InvertRct(CPoint a, CPoint b, int skips=1);

////////////////////////////////////////////////////////////////////////

private:
	void		init(CWnd* parentWindow);

	CWnd*		parent;
	int			pWidth;
	int			pStyle;	// MSF styles

	DColors		C;
	COLORREF	pColor;	// pen color
	COLORREF	bColor;	// background for broken lines
	CPen*		oldpen;

	CRgn		region;
	BOOLEAN		regiondefined;
	BOOLEAN		regiontodelete; // see destructor
};

#endif // DRLINE_W_H