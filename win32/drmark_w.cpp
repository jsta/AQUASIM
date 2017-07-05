#include "stdafx.h"

#include "drmark_w.h"

DrawMark::DrawMark() {
  N=0;
  pattern=NULL;
  mark=NULL;
  shift=NULL;
}

DrawMark::DrawMark(int n, CPoint* pat) {
  pattern=NULL;
  mark=NULL;
  shift=NULL;
  Define(n,pat);
}

DrawMark::~DrawMark() {
  Define(0);
}

void DrawMark::Define(int n, CPoint* pat) {
   if (N) { delete [] pattern; delete [] mark; delete [] shift; N=0; }
   if (0 < n) {
      N=n;
      pattern=new CPoint [n];
      mark   =new CPoint [n];
      shift  =new CPoint [n];
      for (int i=0; i < n; i++) {
         mark[i]=pattern[i]=pat[i];
      }
   }
}

void DrawMark::Set(int size, BOOLEAN fill) {
   if (N) {
      Fill=fill;
		for (int i=0; i < N; i++) {
			(mark[i]).x=size*pattern[i].x;
			(mark[i]).y=size*pattern[i].y;
		}
   }
}

void DrawMark::Mark(CPoint p)
{
	Mark(p.x, p.y);
}

void DrawMark::Mark(int x, int y) {
//   CRect circlebox;

   
   if (N) {
      for (int i=0; i< N; i++) {
         shift[i].x=x + mark[i].x;
		 shift[i].y=y + mark[i].y;
//         shift[i].set(x + mark[i].x(),y + mark[i].y());
      }
      if (oval) {
		  circle(shift[1].x,shift[1].y,shift[0].y-shift[1].y);
      }
      else
	  {
		  polygon(shift, N);
/*
		  dc.MoveTo(shift[0].x,shift[0].y);
		  for (int n=0; n < N; n++)
		  {
			  dc.LineTo(shift[n].x,shift[n].y);
		  }
*/
	  }
   }
//	dc.SelectObject(oldpen);
}

void DrawMark::MarkCurve( AnyCurve*  curve, int next, int first, CRect cliprct)
{
	const int n=curve->Npts();
	MarkCurve(n,curve->Curve(), next, first, cliprct);
}

void DrawMark::MarkCurve(int n,  CPoint* curve, int next, int mark,
						 CRect cliprct) 
{
	BOOLEAN clip=(0 < cliprct.Width()) && (0 < cliprct.Height());
	if (clip) ClipRct(cliprct);
   for (mark=0; mark < n; mark+=next) { // step on curve
      Mark(curve[mark].x,curve[mark].y);
   }
}

void DrawMark::def_ord_tic() {
  CPoint a[2];
  a[0].x=-1; a[0].y= 0;
  a[1].x= 0; a[1].y= 0;
  Define(2,a);
}

void DrawMark::def_abs_tic() {
  CPoint a[2];
  a[0].x= 0; a[0].y= 1;
  a[1].x= 0; a[1].y= 0;
  Define(2,a);
}

void DrawMark::def_square() {
  CPoint a[4];
  a[0].x=-1; a[0].y=-1;
  a[1].x= 1; a[1].y=-1;
  a[2].x= 1; a[2].y= 1;
  a[3].x=-1; a[3].y= 1;
  oval=FALSE;
  Define(4,a);
}

void DrawMark::def_triangle_up() {
  CPoint a[3];
  a[0].x= 0; a[0].y=-1;
  a[1].x=-1; a[1].y= 1;
  a[2].x= 1; a[2].y= 1;
  oval=FALSE;
  Define(3,a);
}

void DrawMark::def_triangle_down() {
  CPoint a[3];
  a[0].x= 0; a[0].y= 1;
  a[1].x=-1; a[1].y=-1;
  a[2].x= 1; a[2].y=-1;
  oval=FALSE;
  Define(3,a);
}

void DrawMark::def_diamond() {
  CPoint a[4];
  a[0].x= 0; a[0].y=-1;
  a[1].x= 1; a[1].y= 0;
  a[2].x= 0; a[2].y= 1;
  a[3].x=-1; a[3].y= 0;
  oval=FALSE;
  Define(4,a);
}

void DrawMark::def_dot() {
  CPoint a[5];

	a[0].x=0; a[0].y=-1;
	a[1].x=1; a[1].y= 0;
	a[2].x=0; a[2].y= 1;
	a[3].x=1; a[3].y= 0;
	a[4].x=0; a[4].y= 0;

  oval=FALSE;
  Define(5,a);
}

void DrawMark::def_circle() {
  def_abs_tic();
  oval=TRUE;
}

/////////////////////////////////////////////////////////////////////////////

