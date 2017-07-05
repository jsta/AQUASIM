#ifndef DRMARK_W
#define DRMARK_W

#include "drline_w.h"

/////////////////////////////////////////////////////////////////////////
class DrawMark : public LineTool {
/////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
//
// Description:
//
public:
	~DrawMark();
// Usage:
// 1. Declare one or more DrawMark objects in C*Doc.h:
	DrawMark();
	DrawMark(int n, CPoint* pat=0);
//		e.g.
//		DrawMark object1;
// 2. Define a mark for this object
//		You may do this by calling one of the predefined
//		mark constructors:
	void def_ord_tic();
	void def_abs_tic();
	void def_square();
	void def_triangle_up();
	void def_triangle_down();
	void def_diamond();
	void def_circle();
	void def_dot();
//		or you may define an array of CPoint and then pass this
//		array to Define(int n, CPoint pattern)
//		or to the constructor of this object.
	void Define(int n, CPoint* pat=0);	// Define Pattern: polygon with n points
//			copies Pattern to Mark
//			sets fill = FALSE
//
// 3. Optionally you resize the mark using
//			Set(int size).
	void Set(int size, BOOLEAN fill=FALSE);
//			makes a Mark in which each
//			coordinate of Pattern is
//			multiplied by size;
//			sets fill
//
// 4. In C*View.cpp attach each object to the current Window.
//		You can do this once, e.g. in InitView():
//			object1.attachToWindow(this);
//		Next you may define the pen, e.g.
//			object1.SetPen(width, color, style);
//
// 5. Upon an OnDraw event you may optionally redefine the pen
//		then call MarkCurve	or Mark
//
	void MarkCurve(AnyCurve* curve, 
		           int next=1, int first=0, CRect cliprct=CRect(0,0,0,0)); 
//		e.g.	AnyCurve curve;
//				curve.Set(n, &y[0] , &y[0], crect);
//				MarkCurve(curve.pCurve());
	void MarkCurve(int n,  CPoint* curve, 
		           int next=1, int first=0, CRect cliprct=CRect(0,0,0,0));
//		MarkCurve draws marks on first then on every next points,
//		skipping (next-1) points on curve
// 
	void Mark(CPoint p);
	void Mark(int x, int y);
//		shifts Mark to point p or to absolute
//		coordinates x, y and draws mark
/////////////////////////////////////////////////////////////////////////


	CWnd* parent;

   private:
      
      int N;
      CPoint* pattern;
      CPoint* mark;
      CPoint* shift;
      BOOLEAN Fill;
      BOOLEAN oval;

};

#endif
