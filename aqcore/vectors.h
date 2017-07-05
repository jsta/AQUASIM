#ifndef VECTORS
#define VECTORS

#include "all.h"

typedef double REAL;


class IntCurve;
class RealCurve;
class DrawCurve;

////////////////////////////////////////////////////////////////////////
// class IntVec
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 12.03.1992
// Updated :
//
// Description:
//
// 1. A tool to Calculate minimum and maximum value of a vector
//
//	vector(int n, int* vector);
//
// 2. A tool to project values of a vector from one range to another one
//
//    The target vector may be user defined by one of 2 methods:
//
//	1. vector(int n, int* vector);
//	   range(int min, int max);
//	2. vector_range(int n, int* vector, int min, int max);
//
//    or it may be a private vector V defined by one of 2 methods:
//
//	1. vector(int n);
//	   range(int min, int max);
//	2. vector_range(int n, 0, int min, int max);
//
//    Projection refers to two IntVec:
//
//	project(IntVec* source, IntVec* target);
//
// 3. A scalar value may be added to all elements of a vector:
//
//	add(int value);
//
////////////////////////////////////////////////////////////////////////

class IntVec {
	
	friend class IntCurve;
	friend class DrawCurve;	// -> "DrawCurve.h"
	
	int  N;			// number of elements
	int* V;			// vector of N elements
	int  MIN, MAX;		// smallest largest element in V
	int  Rmin, Rmax;	// minimum and maximum of range
				// from/to which to project elements;
				// defaults: Rmin=MIN, Rmax=MAX, may
				// be overruled by range(min,max)
	
    BOOLEAN privatevector;		// TRUE: Destructor will delete V

    void minmax();			// Calculate MIN, MAX: useful
    					// after projections
	
  public:
	
    void reset();			// clear IntVec

    IntVec();				// Constructor

    ~IntVec() { reset(); }		// Destructor

    void vector(int n);			// Allocate vector V
    
    void vector(int n, int* vector);	// Define vector with n elements:
    					// calculate MIN, MAX
    					// No Allocation of V:
    					// reference to external vector
    
    void vector_range(int n,		// Define vector with n elements,
    		      int* vector,	// if vector=0, allocate V
    		      int imin,		// calculate MIN, MAX
    		      int imax);	// define range Rmin=min, Rmax=max
    					

    void range(int imin, int imax);	// Define  range for vector V

    int project(IntVec* from,		// projection of all elements from
                IntVec* to); 		// "from-range" to "to-range"
    					// return 0: ok
    					// return 1: from.N < 1
    					// return 2:   to.N < 1
    					// return 3: from.N != to.N
    					// return 4:   to == 0
    					// return 5: from == 0

    void add(int value);		// adds value to
    					//  V, MIN, MAX, Rmin, Rmax;
    					
    BOOLEAN is_private() { return privatevector; } // Is vector private ?

    int n()	 { return N; }		// get N

    int Min()	 { return MIN; }	// get MIN
    int rmin()	 { return Rmin;}	// get Rmin
	
    int Max()	 { return MAX; }	// get MAX
    int rmax()	 { return Rmax;}	// get Rmax
	
    int v(int i) { return V[i]; }	// get V[i]
};


////////////////////////////////////////////////////////////////////////
// class RealVec
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 12.03.1992
// Updated :
//
// Description:
//
// 1. A tool to Calculate minimum and maximum value of a vector
//
//	vector(int n, REAL* vector);
//
// 2. A tool to project values of a vector from one range to another one
//
//    The target vector may be user defined by one of 2 methods:
//
//	1. vector(int n, REAL* vector);
//	   range(REAL min, REAL max);
//	2. vector_range(int n, REAL* vector, REAL min, REAL max);
//
//    or it may be a private vector V defined by one of 2 methods:
//
//	1. vector(int n);
//	   range(REAL min, REAL max);
//	2. vector_range(int n, 0, REAL min, REAL max);
//
//    Projection refers to two IntVec:
//
//	project(RealVec* source, RealVec* target);
//
// 3. A scalar value may be added to all elements of a vector:
//
//	add(REAL value);
//
////////////////////////////////////////////////////////////////////////

class RealVec {

	friend class RealCurve;
	friend class DrawCurve;	// -> "DrawCurve.h"

	int  N;			// number of elements
	REAL* V;			// vector of N elements
	REAL  MIN, MAX;		// smallest largest element in V
	REAL  Rmin, Rmax;	// minimum and maximum of range
				// from/to which to project elements;
				// defaults: Rmin=MIN, Rmax=MAX, may
				// be overruled by range(min,max)
	
    BOOLEAN privatevector;		// TRUE: Destructor will delete V

    void minmax();			// Calculate MIN, MAX: useful
    					// after projections

  public:
	
    void reset();			// clear RealVec

    RealVec();				// Constructor

    ~RealVec() { reset(); }		// Destructor

    void vector(int n);			// Allocate vector V

    void vector(int n, REAL* vector);	// Define vector with n elements:
    					// calculate MIN, MAX
    					// No Allocation of V:
    					// reference to external vector
    
    void vector_range(int n,		// Define vector with n elements,
                      REAL* vector,	// if vector=0, allocate V
                      REAL fmin,	// calculate MIN, MAX
                      REAL fmax);	// define range Rmin=min, Rmax=max

    void range(REAL fmin, REAL fmax);	// Define  range for vector V

    int project(RealVec* from,		// projection of all elements from
                RealVec* to);		// "from-range" to "to-range"
    					// return 0: ok
    					// return 1: from.N < 1
    					// return 2:   to.N < 1
    					// return 3: from.N != to.N
    					// return 4:   to == 0
    					// return 5: from == 0

    void add(REAL value);		// adds value to
    					//  V, MIN, MAX, Rmin, Rmax;

    BOOLEAN is_private() { return privatevector; } // Is vector private ?

    int n()	 { return N; }		// get N

    REAL Min()	 { return MIN; }	// get MIN
    REAL rmin()	 { return Rmin;}	// get Rmin
	
    REAL Max()	 { return MAX; }	// get MAX
    REAL rmax()	 { return Rmax;}	// get Rmax
	
    REAL v(int i) { return V[i]; }	// get V[i]
};

////////////////////////////////////////////////////////////////////////
// class IntCurve
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 12.03.1992
// Updated :
//
// Description:
//
// A tool to do parallel and rotational transforms on a pair of vectors
// describing a curve in cartesian coordinates. Based on class IntVec.
//
// 1. Two methods for the definition of a curve:
//
//	1. curve(int n, int* x, int* y);
//	   range(int xmin, int xmax, int ymin, int ymax);
//	2. curve(int n, int xmin, int xmax, int* x,
//	                int ymin, int ymax, int* y);
//
// 2. Transforms:
//
//	relative parallel transform: relmove
//	absolute parallel transform: absmove, relative to 
//
//			   rotation: rotate
//
////////////////////////////////////////////////////////////////////////

class IntCurve {

	int     N;
	IntVec X;
	IntVec Y;
	
  public:

	IntCurve() { N=0; }			// Constructor
	~IntCurve();				// Destructor
	
    void curve(int n);				// Allocate space

    void curve(int n, int* x, int* y);	// Refer to external curve

    void curve(int n, int xmin, int xmax, int* x,  // range definition,
	              int ymin, int ymax, int* y); // external curve

    void range(int xmin, int xmax,
               int ymin, int ymax);		// range definition

    void relmove(int inc_x, int inc_y);	// parallel transform
    
    void rotate(REAL degree);			// rotation around
    						// (X.Rmin, Y.Rmin)
    
    void rotate(int x, int y, int degree);	// rotation around
    						// (x,y)
    

//    int xmin()  { return X.min;  }
//    ................................
//    int rymax() { return Y.rmax; }
};


////////////////////////////////////////////////////////////////////////
// class RealCurve
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 12.03.1992
// Updated :
//
// Description:
//
// A tool to do parallel and rotational transforms on a pair of vectors
// describing a curve in cartesian coordinates. Based on class IntVec.
//
// 1. Two methods for the definition of a curve:
//
//	1. curve(int n, REAL* x, REAL* y);
//	   range(REAL xmin, REAL xmax, REAL ymin, REAL ymax);
//	2. curve(int n, REAL xmin, REAL xmax, REAL* x,
//	                REAL ymin, REAL ymax, REAL* y);
//
// 2. Transforms:
//
//	relative parallel transform: relmove
//	absolute parallel transform: absmove, relative to X.rmin,Y.rmin
//
//			   rotation: rotate
//
////////////////////////////////////////////////////////////////////////

class RealCurve {

	friend class DrawCurve;			// -> "DrawCurve.h"
	
	int     N;
	RealVec X;
	RealVec Y;
	
  public:

	RealCurve() { N=0; }			// Constructor
	~RealCurve() {};			// Destructor
	
    void curve(int n);				// Allocate space

    void curve(int n, REAL* x, REAL* y);	// Refer to external curve

    void curve(int n, REAL xmin, REAL xmax, REAL* x,  // range definition,
	              REAL ymin, REAL ymax, REAL* y); // external curve

    void range(REAL xmin, REAL xmax,
               REAL ymin, REAL ymax);		// range definition

    void relmove(REAL inc_x, REAL inc_y);	// parallel transform
    
    void absmove(REAL to_x,  REAL to_y);	// parallel transform, moving
    						// (rxmin,rymin)->(to_x,to_y)
    
    void rotate(REAL x, REAL y, REAL degree);	// rotation around
    						// (x,y)

//    REAL xmin()  { return X.min();  }
//    ................................
//    REAL rymax() { return Y.rmax(); }
};

#endif
