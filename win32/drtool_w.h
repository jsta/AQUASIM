
#ifndef DRTOOL_W_H
#define DRTOOL_W_H
////////////////////////////////////////////////////////////////////////
// class DrawTools
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 22.05.1996
// Updated :
//
// Description:
//
//
////////////////////////////////////////////////////////////////////////

#include "DRPTRC_W.H"

static enum Color{black, red, green, blue, yellow, white};

class DrawTools
{
private:
	int pStyle;
	int pWidth;
	COLORREF pColor;

	CPen* oldpen;


public:
	DrawTools();

	void DefinePen(int style, int width, COLORREF color);

	void SetStyle(int style);
	void SetWidth(int width);
	void SetColor(COLORREF color);


	void SetColor(Color color);

	BOOLEAN polyline(CDC* pDC, Pnt* line, int N);
	BOOLEAN polygon(CDC* pDC, Pnt* line, int N);
	BOOLEAN rectangle(CDC* pDC, Rct rect);


};

#endif // DRTOOL_W_H