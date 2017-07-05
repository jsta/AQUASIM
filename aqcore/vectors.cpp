#include "math.h"
#include "vectors.h"

////////////////////////////////////////////////////////////////////////

// to be improved/cleaned up:
const REAL pi45=atan(1.0)/45.0;
static REAL rad(REAL degree);

////////////////////////////////////////////////////////////////////////

static REAL rad(REAL degree) { return pi45 * degree; }

////////////////////////////////////////////////////////////////////////
// class IntVec
////////////////////////////////////////////////////////////////////////

void IntVec::reset() {

	if (privatevector && 0 < N) delete [] V;
	N=0;
	V=0;
	MAX=0;
	MIN=0;
	Rmin=0;
	Rmax=0;
	privatevector=FALSE;
}

IntVec::IntVec() {
	privatevector=FALSE;
	N=0;
	reset();
}

void IntVec::minmax() {

	if (0 < N) {
	  MIN=V[0];
	  MAX=MIN;
	  for (int i=1; i < N; i++) {
	    if (V[i] < MIN) MIN=V[i];
	    else if (MAX < V[i]) MAX=V[i];
	  }
	}
}

void IntVec::vector(int n) {

	reset();
        N=n;
        V=new int [N]; privatevector=TRUE;
}

void IntVec::vector(int n, int* ivector) {

	V=ivector;
	N=n;
	minmax();
	Rmin=MIN;
	Rmax=MAX;
}

void IntVec::vector_range(int n, int* ivector, int imin, int imax) {

	if (privatevector) reset();
	
	N=n;
	if (ivector) {
	   V=ivector;
	   minmax();
	}
	else
	{ reset(); N=n; V=new int [N]; privatevector=TRUE;
	}
	Rmin=imin;
	Rmax=imax;
}

void IntVec::range(int imin, int imax) {

	Rmin=imin;
	Rmax=imax;
}

int IntVec::project(IntVec* from, IntVec* to) {
  REAL f;
  if (!from) return 5;
  if (!to) return 4;
  if (from->N != to->N || from->N == 0) return 2;
  f=REAL(from->Rmax - from->Rmin);
  if (f == 0.0) return 1;
  f=REAL(to->Rmax - to->Rmin)/f;
  
  for (int i=0; i < from->N; i++ ) {
    to->V[i]=int(REAL(from->V[i] - from->Rmin) * f) + to->Rmin;
  }
  return 0;
}

void IntVec::add(int value) {
   if (N) {
     for (int i=0; i < N; i++) {
       V[i]=V[i] + value;
     }
     MIN=MIN + value;
     MAX=MAX + value;
     Rmin=Rmin + value;
     Rmax=Rmax + value;
   }
}

////////////////////////////////////////////////////////////////////////
// class RealVec
////////////////////////////////////////////////////////////////////////

void RealVec::reset() {

	if (privatevector && 0 < N) delete [] V;
	N=0;
	V=0;
	MAX=0;
	MIN=0;
	Rmin=0;
	Rmax=0;
	privatevector=FALSE;

}

RealVec::RealVec() {
	privatevector=FALSE;
	N=0;
	reset();
}

void RealVec::minmax() {

	if (0 < N) {
	  MIN=V[0];
	  MAX=MIN;
	  for (int i=1; i < N; i++) {
	    if (V[i] < MIN) MIN=V[i];
	    else if (MAX < V[i]) MAX=V[i];
	  }
	}
}

void RealVec::vector(int n) {

	reset();
        N=n;
        V=new REAL [N]; privatevector=TRUE;
}

void RealVec::vector(int n, REAL* rvector) {

	V=rvector;
	N=n;
	minmax();
	Rmin=MIN;
	Rmax=MAX;
}

void RealVec::vector_range(int n, REAL* rvector, REAL fmin, REAL fmax) {

	if (privatevector) reset();
	
	N=n;
	if (rvector) {
	  V=rvector;
	  minmax();
	}
	else
	{ reset(); N=n; V=new REAL [N]; privatevector=TRUE;
	}
	Rmin=fmin;
	Rmax=fmax;
}

void RealVec::range(REAL fmin, REAL fmax) {

	Rmin=fmin;
	Rmax=fmax;
}

int  RealVec::project(RealVec* from, RealVec* to) {
  REAL f;
  if (!from) return 5;
  if (!to) return 4;
  if (from->N < 1) return 1;
  if (to->N < 1) return 2;
  if (from->N != to->N) return 3;
  f=from->Rmax - from->Rmin;
  if (f == 0.0) return 1;
  f=(to->Rmax - to->Rmin)/f;
  
  for (int i=0; i < from->N; i++ ) {
    to->V[i]=(from->V[i] - from->Rmin) * f + to->Rmin;
  }
  return 0;
}

void RealVec::add(REAL value) {
   if (N) {
     for (int i=0; i < N; i++) {
       V[i]=V[i] + value;
     }
     MIN=MIN + value;
     MAX=MAX + value;
     Rmin=Rmin + value;
     Rmax=Rmax + value;
   }
}

////////////////////////////////////////////////////////////////////////
// class IntCurve
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// class RealCurve
////////////////////////////////////////////////////////////////////////

void RealCurve::curve(int n) {

    N=n;
    X.vector(n);
    Y.vector(n);
}

void RealCurve::curve(int n, REAL* x, REAL* y) {

    N=n;
    X.vector(n, x);
    Y.vector(n, y);
}

void RealCurve::curve(int n, REAL xmin, REAL xmax, REAL* x,
	                     REAL ymin, REAL ymax, REAL* y) {

    N=n;
    X.vector_range(n, x, xmin, xmax);
    Y.vector_range(n, y, ymin, ymax);
}
	          
void RealCurve::range(REAL xmin, REAL xmax,
                      REAL ymin, REAL ymax) {

    X.range(xmin,xmax);
    Y.range(ymin,ymax);
}

void RealCurve::relmove(REAL inc_x, REAL inc_y) {

    X.add(inc_x);
    Y.add(inc_y);
}

void RealCurve::absmove(REAL to_x,  REAL to_y) {

    to_x=to_x - X.Rmin;
    to_y=to_y - Y.Rmin;
    relmove(to_x, to_y);
}

void RealCurve::rotate(REAL x0, REAL y0, REAL degree) {

    REAL x, y;
    REAL deg=rad(degree);
    REAL cosd=cos(deg);
    REAL sind=sin(deg);
    int i;
    
    for (i=0; i < N; i++) {
      x=X.V[i] - x0;
      y=Y.V[i] - y0;
      X.V[i]= x * cosd + y * sind + x0;
      Y.V[i]=-x * sind + y * cosd + y0;
    }
    
    x=X.Rmin - x0;
    y=Y.Rmin - y0;
    X.Rmin= x * cosd + y * sind + x0;
    Y.Rmin=-x * sind + y * cosd + y0;
    
    x=X.Rmax - x0;
    y=Y.Rmax - y0;
    X.Rmax= x * cosd + y * sind + x0;
    Y.Rmax=-x * sind + y * cosd + y0;
    
    x=X.MIN - x0;
    y=Y.MIN - y0;
    X.MIN= x * cosd + y * sind + x0;
    Y.MIN=-x * sind + y * cosd + y0;
    
    x=X.MAX - x0;
    y=Y.MAX - y0;
    X.MAX= x * cosd + y * sind + x0;
    Y.MAX=-x * sind + y * cosd + y0;
}

