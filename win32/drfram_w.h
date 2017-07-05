#ifndef FRAME_W
#define FRAME_W

#include "DRPTRC_W.H"

////////////////////////////////////////////////////////////////////////
// class Frame
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 18.03.1992
// Updated :
//
// Description:
//
////////////////////////////////////////////////////////////////////////
class Frame {

	int  npoints;
	Pnt  FRAMe[5];
	void expand(Pnt* frame);
	
	int xwidth;
	int yheight;
	
  public:
  
  	
  	Frame(int xmin=0, int xmax=0,		
  	      int ymin=0, int ymax=0);
  	
  	Frame(Rct rct);
  	
  	void define(int xmin, int xmax,		// define frame
  	            int ymin, int ymax);

	void define(Rct rct);			// define frame
	
/*sorry:*/
/*const*/ Frame* frame() { return this; }	// address of this frame

        Rct GetFrame();				// return this frame as Rct

	BOOLEAN contains(int x, int y)/*const*/;// TRUE==point (x,y)in frame

	void add(int add_x, int add_y);		// add values to frame
	
	void copy_from(/*const*/ Frame* from);	// copy "from"to this frame
	
	void copy_to(Frame* to);		// copy this frame to "to"
	
	void surround(/*const*/ Frame* around);	// enlarges this frame to
						// contain another frame 

	int xval(int i) /*const*/ { return FRAMe[i].x(); }
	int yval(int i) /*const*/ { return FRAMe[i].y(); }
	
	int xmin() /*const*/ { return FRAMe[0].x(); }
	int ymin() /*const*/ { return FRAMe[0].y(); }
	int xmax() /*const*/ { return FRAMe[2].x(); }
	int ymax() /*const*/ { return FRAMe[2].y(); }
	
	int width()  /*const*/ { return xwidth;  }
	int height() /*const*/ { return yheight; }
	
	int xcenter() /*const*/ 		// returns the center points
	    { return FRAMe[0].x()+xwidth/2; }	// of this frame
	int ycenter() /*const*/ 
	    { return FRAMe[0].y()+yheight/2;}
						
	
	int xdistance(Frame* to) /*const*/;	// returns x-distance from
						// center of this frame
						// to center of "to":
						// positive if this <= to
	
	int ydistance(Frame* to) /*const*/;	// returns y-distance from
						// center of this frame
						// to center of "to":
						// positive if this <= to
	
	BOOLEAN xoverlap(Frame* to, int& xdist) /*const*/;
						// xdist measures x-distance
						// between
						// the nearest edges of this
						// frame and of the "to"
						// frame. If FALSE: 0 < xdist:
						// this is left of to
						// xdist < 0: to is left of this
	
	BOOLEAN yoverlap(Frame* to, int& ydist) /*const*/;
						// ydist measures y-distance
						// between
						// the nearest edges of this
						// frame and of the "to"
						// frame. If FALSE: 0 < ydist:
						// this is above to
						// ydist < 0: to is above this
};

#endif // FRAME_W
