#include "stdafx.h"
#include "dcolor_w.h"

// the database of color COmponents
static UINT redCO[ColorSize] = 
{0,  0,  0,  0,  127,127,127,127,  192,  0,  0,  0,  255,255,255,255, 1};

static UINT greCO[ColorSize] = 
{0,  0,127,127,    0,  0,127,127,  192,  0,255,255,    0,  0,255,255, 1};

static UINT bluCO[ColorSize] = 
{0,127,  0,127,    0,127,  0,127,  192,255,  0,255,    0,255,  0,255, 1};

void DColors::Adjust  (Color color,    UINT Red,
								  UINT Green, 
								  UINT Blue)
{
	redCO[color]=Red;
	greCO[color]=Green;
	bluCO[color]=Blue;
}
	
void DColors::GetRGB(COLORREF color, UINT& Red, UINT& Green, UINT& Blue)
{
	RGBREF rgb;
	rgb.color=color;
	Red=rgb.ref.red;
	Green=rgb.ref.green;
	Blue=rgb.ref.blue;
}

UINT DColors::GetRed(COLORREF color)
{  
	RGBREF rgb;
	rgb.color=color;
	return rgb.ref.red; 
}

UINT DColors::GetGreen(COLORREF color)
{  
	RGBREF rgb;
	rgb.color=color;
	return rgb.ref.green; 
}

UINT DColors::GetBlue(COLORREF color)
{  
	RGBREF rgb;
	rgb.color=color; 	
	return rgb.ref.blue; 
}
				  	
COLORREF DColors::GetCREF (Color col)
{
	return RGB(redCO[col],greCO[col],bluCO[col]);
}
