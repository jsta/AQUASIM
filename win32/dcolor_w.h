#ifndef DCOLOR_W_H
#define DCOLOR_W_H
////////////////////////////////////////////////////////////////////////
// class DColors
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 21.06.1996
// Updated : 
//
// Description:	Class DColors defines a palette of 16 colors
//				and functions to obtain their COLORREF and
//              their RGB values.
//              A function is supplied to adjust the palette.
//
// Useage:
//		
//		Color enumerates 16 by RGB components predefined 
//			colors.
//		Adjust(color, RGB-Components) may be used to
//			correct the predefined colors.
//	
//		GetRed, GetGreen, GetBlue and GetRGB extract the
//			RBG components of a COLORREF.
//
//		GetCREF gets the COLORREF for any of the 16 predefined
//			colors.
////////////////////////////////////////////////////////////////////////

static enum Color{	black,		dark_blue,	dark_green,	dark_cyan,
/* do        */		dark_red,	purple,		brown,		gray,
/*  NEVER    */		light_gray,	blue,		green,		cyan,
/*   change  */		red,		magenta,	yellow,		white,
/*    this   */		CustomColor,	// loop delimiter; may hold a color
/*     order */ 	ColorSize};		// array size

struct ColorRGB{
	UINT red   : 8;
	UINT green : 8;
	UINT blue  : 8;
	UINT unused: 8;
};

union RGBREF{
	COLORREF color;
	ColorRGB ref;
};

class DColors
{
public:
	void Adjust  (Color color,    UINT Red,
								  UINT Green, 
								  UINT Blue);
	
	void GetRGB  (COLORREF color, UINT& Red, 
		                          UINT& Green, 
								  UINT& Blue);
	UINT GetRed  (COLORREF color);
	UINT GetGreen(COLORREF color);
	UINT GetBlue (COLORREF color);
				  	
COLORREF GetCREF (Color color);

};

#endif // DCOLOR_W_H
