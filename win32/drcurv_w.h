#ifndef DRCURVE_W
#define DRCURVE_W

#include "DRFRAM_W.H"
#include "DRMARK_W.H"
#include "DROBJ_W.H"
#include "aqcore\vectors.H"
#include "DRPTRC_W.H"

////////////////////////////////////////////////////////////////////////
// class DrawCurve
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 12.03.1992
// Updated :
//
// Description:
//
// A tool to define an array of Pnt* CURVE, to project a RealCurve to it
// and to do parallel and rotational transforms on CURVE.
//
// 1. Definition of CURVE, 2 Methods:
//
//	1. curve(int n, Pnt* existing);
//	   curve(int Rxmin, int Rxmax, int Rymin, int Rymax);
//	2. curve(int n, int Rxmin, int Rxmax,
//		        int Rymin, int Rymax,
//			Pnt* existing);
//
// 2. Projections:
//
//	curve(int n, int Rxmin, int Rxmax,
//		     int Rymin, int Rymax,
//		     RealCurve* source);
//
//	project_to(RealCurve* target);
//
// 3. Transformations:
//
//	relmove
//	absmove
//	rotate
//
// 4. Utilitys (to be used by class Shape)
//
//	GetRange
//	Getextrema
//
////////////////////////////////////////////////////////////////////////

class DrawCurve : public DrawObj {

   protected:
           
	Pnt* CURVE;			// array of points
	Frame  extrema;			// 0:x0,y0; 1:x0,y; 2:x,y; 3:x,y0
	void default_range();		// sets range=extrema
    
	void reset();			// clear DrawCurve
	void newpntcurve(int n);	// (delete CURVE, then)
					// create new CURVE
	BOOLEAN curve_defined;		// CURVE allocated
					
	void cpypntcurve(int n,		// copy pnts to CURVE
	                 Pnt* pnts);
	                 
	
	void minmax();			// calculate extrema
        int project_from(RealCurve* from); // project from a RealCurve
    					// to this PointCurve CURVE
    					// return 0: ok
    					// return 1: from.N < 2
    					// return 2: divide by zero
    					// return 3: window for CURVE not defined
    					// return 4: from == 0 
	
  public:
/*
    DrawCurve* Next() const
       { return (DrawCurve*)DrawObj::Next(); }
    DrawCurve* Prev() const
       { return (DrawCurve*)DrawObj::Prev(); }
*/  
    DrawCurve(CDC* window = NULL,	// Constructor: draw to window
        BOOLEAN delete_as_sub = FALSE);		// If a parent DrawObj is 
        					// deleted, this subobject
        					// should also be deleted 
    ~DrawCurve();			// Destructor
    
    void curve(int Rxmin, int Rxmax,
               int Rymin, int Rymax);	// range definition
    
    void curve(int n, Pnt* existing);	// copy external curve to CURVE
               				// Window (extrema, range)
    					// automatically defined
    
    void curve(int n,
               int Rxmin, int Rxmax,
               int Rymin, int Rymax,	// range definition,
               Pnt* existing);		// copy external curve to CURVE
  
//    BOOLEAN contains(int x, int y)		// TRUE, if (x,y) in range
//                    {return range.contains(x,y); }
    
    void curve(int n,
               RealCurve* from);	// project from real curve to CURVE
               				// range must have been defined before
    
    void curve(int n,
               int Rxmin, int Rxmax,
               int Rymin, int Rymax,	// range definition,
               RealCurve* from);	// project from real curve to CURVE


    void copy(DrawCurve* defined);	// gets copy of a DrawCurve

    DrawObj* copy_object();		// result =  new DrawObj
  						
    void relmove(int inc_x, int inc_y);	// parallel transform

    void absmove(int to_x, int to_y);	// parallel transform, moving
    					// (rxmin,rymin)->(to_x,to_y)
    
    void rotate(int x, int y, REAL degree);	// rotation around (x,y)
    
    int project_to(RealCurve* to);	// project this PointCurve CURVE
    					// to a RealCurve
    					// return 0: ok
    					// return 1:    N < 1
    					// return 2: to.N < 1
    					// return 3:  N != to.N
    					// return 4: to == 0
    
    Rct GetExtrema();
//    void GetExtrema(Rct* Extrema);
  
    void Draw_Object();			// if (AREA) draw polygon
  					// else      draw polyline
    void Draw_Object(const char* pattern,// 	//   draw polyline with pattern
                     REAL period);
  					
    void Mark(DrawMark* mark);		// Mark Curve points
  
    void Hide_Object();			// if (AREA) draw polygon
  					// else      draw polyline
    
    int xmin() { return extrema.xmin(); }
    int xmax() { return extrema.xmax(); }
    int ymin() { return extrema.ymin(); }
    int ymax() { return extrema.ymax(); }

    int rxmin() { return RANGE.xmin(); }
    int rxmax() { return RANGE.xmax(); }
    int rymin() { return RANGE.ymin(); }
    int rymax() { return RANGE.ymax(); }

};



#endif
