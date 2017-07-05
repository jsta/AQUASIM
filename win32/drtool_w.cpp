#include "stdafx.h"
#include "DRTOOL_W.H"

DrawTools::DrawTools()
{
	pStyle=PS_SOLID;
	pWidth=1;
	pColor=RGB(100,100,100);
}


void DrawTools::DefinePen(int style, int width, COLORREF color)
{
	pStyle=style;
	pWidth=width;
	pColor=color;
}

void DrawTools::SetStyle(int style)
{
	pStyle=style;
}

void DrawTools::SetWidth(int width)
{
	pWidth=width;
}

void DrawTools::SetColor(COLORREF color)
{
	pColor=color;
}

void DrawTools::SetColor(Color color)
{
	switch(color)
	{
		case black:	pColor=RGB(0,0,0);
			break;
		case red:	pColor=RGB(0,0,0);
			break;
		default:	pColor=RGB(0,0,0);
			break;
	}
}



BOOLEAN DrawTools::polyline(CDC* pDC, Pnt* line, int N)
{
	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=pDC->SelectObject(&pen);
	

	pDC->MoveTo(line[0].x(),line[0].y());
	for (int n=1; n < N; n++)
	{
		pDC->LineTo(line[n].x(),line[n].y());
	}

	pDC->SelectObject(oldpen);
	return TRUE;
}

BOOLEAN DrawTools::polygon(CDC* pDC, Pnt* line, int N)
{
	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=pDC->SelectObject(&pen);
	

	pDC->MoveTo(line[0].x(),line[0].y());
	for (int n=1; n < N; n++)
	{
		pDC->LineTo(line[n].x(),line[n].y());
	}
	pDC->LineTo(line[0].x(),line[0].y());

	pDC->SelectObject(oldpen);
	return TRUE;
}

BOOLEAN DrawTools::rectangle(CDC* pDC, Rct rect)
{
	CPen pen;
	if (!pen.CreatePen(pStyle, pWidth, pColor))	return FALSE;
	oldpen=pDC->SelectObject(&pen);

	pDC->MoveTo(rect.left(),rect.top());

	pDC->LineTo(rect.right(),rect.top());
	pDC->LineTo(rect.right(),rect.bottom());
	pDC->LineTo(rect.left(),rect.bottom());
	pDC->LineTo(rect.left(),rect.top());

	pDC->SelectObject(oldpen);
	return TRUE;
}
