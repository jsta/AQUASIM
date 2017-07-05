#include "stdafx.h"

#include "DRFRAM_W.H"


static void pntcpy(Pnt from, Pnt* to) {

   to->x(from.x());
   to->y(from.y());
}

////////////////////////////////////////////////////////////////////////
// class Frame
////////////////////////////////////////////////////////////////////////

void Frame::expand(Pnt* f) {
   f[1].x(f[0].x()); // xmin
   f[1].y(f[2].y()); // ymax
   f[3].x(f[2].x()); // xmax
   f[3].y(f[0].y()); // ymin
   pntcpy(f[0],&f[4]);
}

void Frame::define(int xmin, int xmax,int ymin, int ymax ) {
    
    FRAMe[0].x(xmin);
    FRAMe[0].y(ymin);
    FRAMe[2].x(xmax);
    FRAMe[2].y(ymax);
    xwidth=xmax - xmin;
    yheight=ymax - ymin;
    expand(&FRAMe[0]);
}

void Frame::define(Rct rct) {			// define frame
    define(rct.left(),rct.right(),rct.bottom(),rct.top());
}

Frame::Frame(int xmin, int xmax,
  	     int ymin, int ymax) {

    npoints=5;
    define(xmin, xmax, ymin, ymax);
}

Frame::Frame(Rct rct) {
    npoints=5;
    define(rct.left(),rct.right(),rct.bottom(),rct.top());
}

Rct Frame::GetFrame() {
     Rct r;
     
     r.set(FRAMe[0].x(),FRAMe[2].y(),FRAMe[2].x(),FRAMe[0].y());
//     r.set(RANGE.xmin(),RANGE.ymax(),RANGE.xmax(),RANGE.xmin());
     return r;
}
BOOLEAN Frame::contains(int x, int y) /*const*/ {
/*
    PNT p;
    RCT a;
    p.h=x;
    p.v=y;
    a.top   =ymax();
    a.left  =xmin();
    a.bottom=ymin();
    a.right =xmax();
    return xvt_rect_has_point(&a,p); // did not work
*/
    BOOLEAN a, b;
    a=xmin() <= x && x <= xmax();
    b=ymin() <= y && y <= ymax();
    return (a && b);
}

void Frame::copy_from(/*const*/ Frame* from) {
    
    define(from->xmin(),from->xmax(),from->ymin(),from->ymax() );
 }

void Frame::copy_to(Frame* to) {
    
    to->define(xmin(),xmax(),ymin(),ymax() );
}

void Frame::surround(/*const*/ Frame* around) {
    int x0=xmin();
    int xx=xmax();
    int y0=ymin();
    int yy=ymax();
    
    if (around->xmin() < x0) x0=around->xmin();
    if (around->xmax() > xx) xx=around->xmax();
    if (around->ymin() < y0) y0=around->ymin();
    if (around->ymax() > yy) yy=around->ymax();
    define(x0,xx,y0,yy);
}

void Frame::add(int x, int y) {

	for (int i=0; i < npoints; i++) {
	  FRAMe[i].x(FRAMe[i].x() + x);
	  FRAMe[i].y(FRAMe[i].y() + y);
	}
}

int Frame::xdistance(Frame* to) /*const*/ {
	
	return to->xcenter() - xcenter();
}

int Frame::ydistance(Frame* to) /*const*/ {
	
	return to->ycenter() - ycenter();
}

BOOLEAN Frame::xoverlap(Frame* to, int& xdist) /*const*/ {
	int d=xdistance(to);
	int f=to->width() + xwidth;
	
	xdist=d - f;
	return abs(d) <= f;
}

BOOLEAN Frame::yoverlap(Frame* to, int& ydist) /*const*/ {
	int d=ydistance(to);
	int f=to->height() + yheight;
	
	ydist=d - f;
	return abs(d) <= f;
}


