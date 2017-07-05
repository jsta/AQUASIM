#include "stdafx.h"

#include <limits.h>
#include "math.h"
#include "DRCURV_W.H"

////////////////////////////////////////////////////////////////////////

// to be improved/cleaned up:
static REAL PI45=atan(1.0)/45.0;
static REAL RAD(REAL degree);

////////////////////////////////////////////////////////////////////////

static REAL RAD(REAL degree) { return PI45 * degree; }

////////////////////////////////////////////////////////////////////////
// class DrawCurve
////////////////////////////////////////////////////////////////////////

void DrawCurve::reset() {

    if (curve_defined) delete [] CURVE;
    CURVE=0;
    N=0;
    curve_defined=FALSE;
    extrema.define(0,0,0,0);
}

void DrawCurve::default_range() {
    RANGE.copy_from(extrema.frame());
/*
    for (int i=0; i < 5; i++) {
       range[i].x(extrema[i].x());
       range[i].y(extrema[i].y());
    }
*/
}

DrawCurve::DrawCurve(CDC* window,
		   BOOLEAN delete_as_sub) {                             // Constructor

    N=0;
    curve_defined=FALSE;
    reset();
    pDC=window;
    auto_delete=delete_as_sub;
    
}

DrawCurve::~DrawCurve() {                               // Destructor

    reset();
}

void DrawCurve::cpypntcurve(int n, Pnt* pnts) { // copy pnts to private curve
//   int x, y;
   
   for (int i=0; i < n; i++) {
//      x=pnts[i].x();
//      y=pnts[i].y();
      CURVE[i].x(pnts[i].x());
      CURVE[i].y(pnts[i].y());
   }
}

void DrawCurve::newpntcurve(int n) {    // create new privatecurve

   if (curve_defined) delete [] CURVE;
   
   CURVE=new Pnt [n]; curve_defined=TRUE;
   N=n;
}

void DrawCurve::minmax() {

    int xmin, xmax, ymin, ymax;
    
    xmin=CURVE[0].x(); xmax=xmin;
    ymin=CURVE[0].y(); ymax=ymin;
    for (int i=1; i < N; i++) {
      if (xmin > CURVE[i].x()) xmin=CURVE[i].x();
      if (xmax < CURVE[i].x()) xmax=CURVE[i].x();
      if (ymin > CURVE[i].y()) ymin=CURVE[i].y();
      if (ymax < CURVE[i].y()) ymax=CURVE[i].y();
    }
    extrema.define(xmin,xmax,ymin,ymax);
}

int DrawCurve::project_from(RealCurve* from) {
   REAL fx, fy, rr;
   REAL shortmax=float(SHRT_MAX)/2;
   REAL shortmin=float(SHRT_MIN)/2;
   int   x,  y;
   
   if (!from) return 4;
   if (!range_defined()) return 3;
//   if (from->N < 2) return 1; Single point curves allowed
   fx=from->X.rmax() - from->X.rmin();
   if (fx == 0.0) return 2;
   fx=REAL(rxmax() - rxmin() ) / fx;
   
   fy=from->Y.rmax() - from->Y.rmin();
   if (fy == 0.0) return 2;
   fy=REAL(rymax() - rymin()) / fy;

   for (int i=0; i < from->N; i++) {
     rr=(from->X.V[i] - from->X.rmin()) * fx + rxmin();
     //  will convert  to integer, therefore:
	 if (rr <= shortmin) x=SHRT_MIN/2;
	 else if (shortmax <= rr) x=SHRT_MAX/2;
	      else x=int(rr);
     rr=rymax() - (from->Y.V[i] - from->Y.rmin()) * fy;
     //  will convert  to integer, therefore:
	 if (rr <= shortmin) y=SHRT_MIN/2;
	 else if (shortmax <= rr) y=SHRT_MAX/2;
	      else y=int(rr);
     CURVE[i].x(x);
     CURVE[i].y(y);
   }
   minmax();
   return 0;
}

//DrawCurve& DrawCurve::operator=(DrawCurve& p) {
void DrawCurve::copy(DrawCurve* defined) {      // gets copy of a DrawCurve

    reset();
    newpntcurve(defined->N);
    cpypntcurve(N,defined->CURVE);
// instead of minmax():
    extrema.copy_from(defined->extrema.frame());
    RANGE.copy_from(defined->ObjRange());
    curve_defined=defined->curve_defined;
}

DrawObj* DrawCurve::copy_object() {
    DrawCurve* Copy=new DrawCurve;
    
    Copy->pDC=pDC;
    Copy->subobject=0;
    Copy->auto_delete=auto_delete;
    Copy->drag=0;
//*    Copy->dt_range_show=dt_range_show;
//*     Copy->dt_range_hide=dt_range_hide;
//*    Copy->dt_object_show=dt_object_show;
//*    Copy->dt_object_hide=dt_object_hide;
    Copy->copy(this);
    return Copy;
}

void DrawCurve::curve(int n, Pnt* existing) {

    newpntcurve(n);
    cpypntcurve(n,existing);
    minmax();
    default_range();
}

void DrawCurve::curve(int Rxmin, int Rxmax,
		     int Rymin, int Rymax) {    // range definition
    RANGE.define(Rxmin,Rxmax,Rymin,Rymax);
}

void DrawCurve::curve(int n, int Rxmin, int Rxmax,
		     int Rymin, int Rymax,
		     Pnt* existing) {

    newpntcurve(n);
    cpypntcurve(n,existing);
    minmax();
    curve(Rxmin, Rxmax, Rymin, Rymax);
}


void DrawCurve::curve(int n,
		     RealCurve* from) { // project from real curve to CURVE

    newpntcurve(n);
    project_from(from);
}

void DrawCurve::curve(int n,
	       int Rxmin, int Rxmax,
	       int Rymin, int Rymax,    // range definition,
	       RealCurve* from) {       // project from real curve to CURVE

    newpntcurve(n);
    curve(Rxmin, Rxmax, Rymin, Rymax);
    project_from(from);
}

void DrawCurve::relmove(int inc_x, int inc_y) {
    int i;

    for (i=0; i < N; i++) {
      CURVE[i].x(CURVE[i].x() + inc_x);
      CURVE[i].y(CURVE[i].y() + inc_y);
    }
    extrema.add(inc_x,inc_y);
      RANGE.add(inc_x,inc_y);
/*
    for (i=0; i< 5; i++) {
      extrema[i].x(extrema[i].x() + inc_x);
      extrema[i].y(extrema[i].y() + inc_y);
      
      range[i].x(range[i].x() + inc_x);
      range[i].y(range[i].y() + inc_y);
    }
*/
}

void DrawCurve::absmove(int to_x, int to_y) {

    to_x=to_x - rxmin();
    to_y=to_y - rymin();
    relmove(to_x, to_y);
}

void DrawCurve::rotate(int x0, int y0, REAL degrees) {
    REAL x, y, xx, yy, xm, ym;
    REAL deg=RAD(degrees);
    REAL cosd=cos(deg);
    REAL sind=sin(deg);
    int i;
    
    for (i=0; i < N; i++) {
      x=CURVE[i].x() - x0;
      y=CURVE[i].y() - y0;
      xx= x * cosd + y * sind + x0;
      yy=-x * sind + y * cosd + y0;
      CURVE[i].set(int(xx),int(yy));
    }
    x=RANGE.xmax();
    y=RANGE.ymax();
      xm= x * cosd + y * sind + x0;
      ym=-x * sind + y * cosd + y0;
    x=RANGE.xmin();
    y=RANGE.ymin();
      xx= x * cosd + y * sind + x0;
      yy=-x * sind + y * cosd + y0;
    RANGE.define(int(xx),int(xm),int(yy),int(ym));
    x=extrema.xmax();
    y=extrema.ymax();
      xm= x * cosd + y * sind + x0;
      ym=-x * sind + y * cosd + y0;
    x=extrema.xmin();
    y=extrema.ymin();
      xx= x * cosd + y * sind + x0;
      yy=-x * sind + y * cosd + y0;
    extrema.define(int(xx),int(xm),int(yy),int(ym));
}

int DrawCurve::project_to(RealCurve* to) {

    REAL fx, fy;
    if (!to) return 4;
    if (N < 1) return 1;
    if (to->N < 1) return 2;
    if (N != to->N) return 3;
    
    fx=REAL(rxmax() - rxmin());
    if (fx == 0.0) return 1;
    fx=(to->X.rmax() - to->X.rmin()) / fx;
    
    fy=REAL(rymax() - rymin());
    if (fy == 0.0) return 1;
    fy=(to->Y.rmax() - to->Y.rmin()) / fy;
    
    for (int i=0; i < N; i++) {
      to->X.V[i]=REAL(CURVE[i].x() - rxmin()) * fx + to->X.Rmin;
      to->Y.V[i]=REAL(CURVE[i].y() - rymin()) * fx + to->Y.Rmin;
    }
    to->X.minmax();
    to->Y.minmax();
    
    return 0;
}
    
Rct DrawCurve::GetExtrema()  {
     Rct r;

//     r.set(extrema[0].x(),extrema[2].y(),extrema[2].x(),extrema[0].y());
     r.set(extrema.xmin(),extrema.ymax(),extrema.xmax(),extrema.ymin());
     return r;
}
 
void DrawCurve::Draw_Object() {

	if (AREA)
	{
		dt_object_show.polygon(pDC,CURVE,N);
	}
	else
	{
		dt_object_show.polyline(pDC,CURVE,N);
	}
/*
    if (Check()) {
       win->set_tools(dt_object_show);
       if (AREA) win->polygon(CURVE,N);
	    else win->polyline(CURVE,N);
    }
    else {
       xvt_win_release_pointer();
#ifdef DEBUG
       cout << "\nCannot draw Object [DrawCurve::Draw_Obj]\n";
       cout.flush();
#endif
    }
*/
}

static REAL dist_to_next(int delx, int dely) {
  if (delx) return sqrt(delx*delx+dely*dely);
  else return dely;
}
static void find_next(int& delx, int& dely, REAL target, REAL limit,
		      int& Delx, int& Dely) {
  REAL f;
  f=target/limit;
  Delx=int(f*delx);
  Dely=int(f*dely);
}

void DrawCurve::Draw_Object(const char* pattern,  //   draw polyline with pattern
			    REAL period) {
    if (!Check()) {
// *       xvt_win_release_pointer();
#ifdef DEBUG
       cout << "\nCannot draw Object [DrawCurve::Draw_Obj with pattern]\n";
       cout.flush();
#endif
    }
    else {
//cout << "Draw_Object. pattern = >" << pattern << "<\n"; cout.flush();
      if (0 == strlen(pattern)) {
//cout << "-DRAW NO PATTERN\n";cout.flush();
	 Draw_Object();
	 return;
      }
//cout << "+DRAW PATTERN\n";cout.flush();
 int n=N;
 int ndraw=0;
 int i;         // workpoint; also aux var;
 BOOLEAN newi;
 int iout;      // used for sensing linefeeds
 int j;         // aux var
 int patlen=strlen(pattern);    // symbolic representation of pattern
 REAL* PAT=new REAL[patlen+1];  // to contain lengths of pattern
 int ipat;      // pointer to PAT
 int iaction;   // (if ipat % 2) then pendown else penup...
 REAL limit,    // distance to next point
      limitsum,// for debugging purposes
      target;   // distance to end of pattern
 REAL eps=period;
 BOOLEAN newpat;
 int Xdel,Ydel;// distance from workpoint to next neighbour
 int X0,Y0;     // workpoint <= local origin 
 int X, Y;      // coordinates of target
 Pnt PIECE[2];
 Pnt* P=&PIECE[0];
 
 int debuglimit=0; // set debuglimit to a value up to which you want debugging output
		   // note: you should redirect output to some dummy file
 
 
if (debuglimit) {
//   cout << "\nDraw_Object. pattern = " << pattern << ". Period = " << period << "\n";
//   cout << n << " points in curve. debuglimit = " << debuglimit << "points.\n";
//   cout.flush();
}
 // analyse pattern
    REAL sum=0.0;
    for (i=0; i < patlen; i++) {
       j=pattern[i] - '0';
//       if (j < 0) paterror(pattern);
       sum=sum+j;
       PAT[i]=j;
    }
    for (i=0; i < patlen; i++) {
       PAT[i]=period * PAT[i]/sum;
       if (debuglimit) {
//          cout<< i << ". pattern " << PAT[i] << "\n";
//          cout.flush();
       }
       if (PAT[i] < eps) eps=PAT[i];
    }
    if (eps < 1.0) {
	 Draw_Object();
	 return;
    }
    eps=0.00001 * eps;          // you would'nt believe it!
    if (debuglimit) {
//       cout << "eps " << eps << "\n\n";
//       cout.flush();
    }
    
// *       win->set_tools(dt_object_show);
       
 //      win->polyline(CURVE,N);
       
// draw
    i=0;
    X0=CURVE[i].x(); Xdel=CURVE[i+1].x() - X0; PIECE[0].x(X0);
    Y0=CURVE[i].y(); Ydel=CURVE[i+1].y() - Y0; PIECE[0].y(Y0);
    limit=dist_to_next(Xdel,Ydel);
    limitsum=0;
    if (debuglimit) {
       limitsum=limitsum+limit;
    }
    newi=FALSE;
    ipat=0; iout=0; iaction=ipat % 2;
    target=PAT[ipat];
    find_next(Xdel,Ydel,target,limit,X,Y);
    if (i < debuglimit) {
/*
       cout << "x/y[" << i << "]:" << CURVE[i].x()
	    << "/" << CURVE[i].y()
	    << " limit:" << limit
	    <<" target:" << target << "\n";
       cout.flush();
*/
    }
    newpat=FALSE;
//
    while (TRUE) {              // (i + 1 < n) 
      if (limit <= target) {
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
	      if (iaction == 0) {
		PIECE[1].x(X0 + X);
		PIECE[1].y(Y0 + Y);
		if (i < debuglimit) {
/*
		   cout << "E PLOT at " << i << ": "<< PIECE[0].x() << "/" << PIECE[0].y()
			<< " -->  "    << PIECE[1].x() << "/" << PIECE[1].y()
			<< " target:" << target
			<< " limit:" << limit << " limitsum = " << limitsum << "\n";
		   cout.flush();
*/
		}
		ndraw++;
// *                win->polyline(P,2);
		PIECE[0]=PIECE[1];
	      }
	      else {
		PIECE[0].x(X0 + X);
		PIECE[0].y(Y0 + Y);
		if (i < debuglimit) {
/*
		   cout << "E START at " << i << ": " << PIECE[0].x() << "/" << PIECE[0].y()
			<< " target:" << target
			<< " limit:" << limit << "\n";
		   cout.flush();
*/
		}
	      }
//            word(X0 + X,Y0 + Y, PenDownUp[iaction]);
	    X0=X0+X; Xdel=CURVE[i+1].x() - X0;
	    Y0=Y0+Y; Ydel=CURVE[i+1].y() - Y0;
	    limit=dist_to_next(Xdel,Ydel);
	    newpat=TRUE;
	 }
      } // end target <= limit
      if (newi == TRUE) {
	 i++;
//         if (i + 1 < n) {
	 if (i < n) {
	    X0=CURVE[i].x(); Xdel=CURVE[i+1].x() - X0;
	    Y0=CURVE[i].y(); Ydel=CURVE[i+1].y() - Y0;
	    if (iaction == 0) {
		PIECE[1].x(X0);
		PIECE[1].y(Y0);
		if (i < debuglimit) {
/*
		   cout << "B PLOT at " << i << ": " << PIECE[0].x() << "/" << PIECE[0].y()
			<< " -->  "    << PIECE[1].x() << "/" << PIECE[1].y()
			<< " target:" << target
			<< " limit:" << limit << " limitsum = " << limitsum << "\n";

		   cout.flush();
*/
		}
		ndraw++;
// *                win->polyline(P,2);
		PIECE[0]=PIECE[1];
	    }
	    else {
		PIECE[0].x(X0);
		PIECE[0].y(Y0);
		if (i < debuglimit) {
/*
		   cout << "B START at " << i << ": " << PIECE[0].x() << "/" << PIECE[0].y()
			<< " target:" << target
			<< " limit:" << limit << "\n";
		   cout.flush();
*/
		}
	    }
//              word(X0,Y0, PenDownUp[iaction]);
	    limit=dist_to_next(Xdel,Ydel);
	    if (debuglimit) {
		   limitsum=limitsum+limit;
	    }
	    newi=FALSE;
	    if (i + 1 == n) break;
	 }
	 else break;
      }
      if (newpat == TRUE) {
	 ipat=++ipat % patlen;
	 target=PAT[ipat];
	 iaction=ipat % 2;
	 if (i < debuglimit) {
/*
	    cout << "i=" << i <<"\ttarget = " << target
		 << " -> "<< " iaction="<<iaction << "\tnewpat"
		 << " LIMITSUM = " << limitsum << "\n";
	    cout.flush();
*/
	 }
	 limitsum=0;
	 newpat=FALSE;
      }
    } // end while i + 1 < n
    if (0 < debuglimit) {
//       cout << "----------- " << ndraw << " lines plotted ----------------\n";
//       cout.flush();
    }
    }
}

void DrawCurve::Mark(DrawMark* mark) {                  // Mark Curve points
// *    win->set_tools(dt_object_show);
    mark->MarkCurve(pDC,N,CURVE);
}
 
void DrawCurve::Hide_Object() {

    if (Check()) {
// *       win->set_tools(dt_object_hide);
	if (AREA) dt_object_hide.polygon(pDC,CURVE,N);
	else      dt_object_hide.polyline(pDC,CURVE,N);
//       if (AREA) pDC->polygon(CURVE,N);
//            else pDC->polyline(CURVE,N);
    }
    else {
// *       xvt_win_release_pointer();
#ifdef DEBUG
       cout << "\nCannot draw Object [DrawCurve::Hide_Obj]\n";
       cout.flush();
#endif
    }
}
