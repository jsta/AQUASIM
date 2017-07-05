#ifndef DRPTRCT_H
#define DRPTRCT_H

class Pnt {
	short yval;
	short xval;
public:
//	char *class_name();  // Method in kmisc.cpp
	Pnt set(int x = 0, int y = 0)
	  { xval = (short)x; yval = (short)y; return(*this); }
	Pnt(int x, int y) { set(x, y); }
	Pnt(void) { set(); }
	short x(void) { return xval; }
	void x(int xx) { xval = (short)xx; }
	short y(void) { return yval; }
	void y(int yy) { yval = (short)yy; }
};


class Rct {
	short leftval;
	short topval;
	short rightval;
	short bottomval;
public:
//	char *class_name(void);  // Method in kmisc.cpp

	Rct set(int left = 0, int top = 0, int right = 0, int bottom = 0)
		{ leftval = left; topval = top; rightval = right; bottomval = bottom;
		  return(*this); }

	Rct set(Pnt ul, Pnt lr)
		{ set(ul.x(), ul.y(), lr.x(), lr.y()); return(*this); }

	Rct(int left, int top = 0, int right = 0, int bottom = 0)
		{ set(left, top, right, bottom); }

	Rct(Pnt ul, Pnt lr) { set(ul, lr); }

	Rct(void) { set(); }

	int left(void) { return(leftval); }
	int top(void) { return(topval); }
	int right(void) { return(rightval); }
	int bottom(void) { return(bottomval); }

	BOOLEAN empty()
		{ return(leftval >= rightval || topval >= bottomval); }
};


#endif // DRPTRCT_H