#ifndef DRTEXT_W_H
#define DRTEXT_W_H

#include "dcolor_w.h"

static enum ALIGN{Left, Center, Right};

////////////////////////////////////////////////////////////////////////
class TextTool {
////////////////////////////////////////////////////////////////////////
// Author: J. Ruchti
// Creation: 22.05.1996
// Updated : 20.06.1996
//
// Description:
//
//	Drawing Text and Numbers with different
//
//			* alignments,
//			* angles,
//			* colors,
//			* Fonts, sizes, and styles
//
//
// Usage:
public:
// 1. Declare one or more TextTool objects in C*View.cpp:
	TextTool(CWnd* parentWindow = NULL);
//		e.g. TextTool(this) object1;
//		or in C*View.h or in C*Doc:
//			 TextTool object1;
//
// 2. In *View.cpp you should attach each object to the 
//		current Window, unless you specified the parent
//		window in the declaration.
	void attachToWindow(CWnd* parentWindow);
//		object1.attachToWindow(this);
//
// 3. Writing (for Font selection see 4. below)
//
// 3.1 "Vertical" Alignment	of Text
// 
//		default is
	void AlignOnBase() { VCENTER=FALSE; LineAlignment=TA_BASELINE; }
//		but yoy can change this, before yoy write
	void AlignOnTop()  { VCENTER=FALSE; LineAlignment=TA_TOP; }
	void AlignCenter() { VCENTER=TRUE;  LineAlignment=TA_TOP; }
	void AlignBottom() { VCENTER=FALSE; LineAlignment=TA_BOTTOM; }
//
// 3.2 Writing and "Horizontal" Alignment of Text
//
//		General write parametersw
//      p or x, y define the coordinates of the text.
//		align is given using one of the following values
//				enum ALIGN{Left, Center, Right};
//	    angle is given in tenths of degrees grad.
//      color is given as RGB(red,green,blue) or by
//		Color defined in "dcolor.h"
//
//		To write text
	void Text (CPoint   p,
			   CString  text, 
			   ALIGN	align=Left,
			   int      angle=0,
			   COLORREF color=-1);

	void Text (int x,   int y, 
			   CString  text, 
			   ALIGN    align=Left,
			   int      angle=0, 
			   COLORREF color=-1);
	void text (CPoint   p,
			   CString  text, 
			   ALIGN	align=Left,
			   int      angle=0,
			   Color    color=black);

	void text (int x,   int y, 
			   CString  text, 
			   ALIGN    align=Left,
			   int      angle=0, 
			   Color    color=black);
// Write Numbers
	void Number(CPoint  p, 
			   double	number=0,
			   int		precision=0,
			   ALIGN    align=Left,
			   int      angle=0, 
			   COLORREF color=-1);
	void Number(int x,  int y, 
			   double	number=0,
			   int		precision=0,
			   ALIGN    align=Left,
			   int      angle=0, 
			   COLORREF color=-1);
	void number(CPoint  p, 
			   double	number=0,
			   int		precision=0,
			   ALIGN    align=Left,
			   int      angle=0, 
			   Color    color=black);
	void number(int x,  int y, 
			   double	number=0,
			   int		precision=0,
			   ALIGN    align=Left,
			   int      angle=0, 
			   Color    color=black);
//
// 4. Fontselection
//	  Normally you will write with some system default font.
//    Two methods may be used to define/change the font of a
//    TextTool object:
//
// 4.1. Font dialog: 
	BOOLEAN fontDialog();
//		on a user event you may call this procedure,
//        e.g.
//          void *View::OnChangeFont()
//			{ object1.fontDialog();
//    		}
//      The user may then select a font with style, size and color.
//
// 4.2. CreateFont may be called redefining some or all of the
//      default parameters. Howwever, several characteristics
//		may be changed individually as
//
	void Normal    ()					{ LFT.lfWeight   =   400; }
	void Bold      ()					{ LFT.lfWeight   =   700; }
	void Weight    (int    maxWt=1000)	{ LFT.lfWeight   = maxWt; }
//
	void Size      (int     size=  12)	{ LFT.lfHeight   =  size; }
	void Width     (int    width=   0)	{ LFT.lfWidth    = width; }
//
	void Italic    (BOOLEAN ital=TRUE)	{ LFT.lfItalic   =  ital; }
	void Underline (BOOLEAN undr=TRUE)	{ LFT.lfUnderline=  undr; }
	void StrikeOut (BOOLEAN strk=TRUE)	{ LFT.lfStrikeOut=  strk; }
//
//      CreateFont is first called by attachToWindow. Thereafter
//		it may be called repeatedly, e.g. in OnDraw(CDC* pDC). 
	void CreateFont( 
		LPCTSTR	 lpszFacename="Times New Roman",
		BYTE	 nPitchAndFamily=FF_ROMAN, 
		int		 nWeight=FW_NORMAL, 
		COLORREF rgb = RGB(0,0,0),
		BYTE	 bItalic=0, 
		BYTE	 bUnderline=0, 
		BYTE	 cStrikeOut=0, 
		int		 nHeight=-12, 
		int		 nWidth=0, 
		int		 nEscapement=0, 
		int		 nOrientation=0, 
		BYTE	 nCharSet=ANSI_CHARSET, 
		BYTE	 nOutPrecision=OUT_DEFAULT_PRECIS, 
		BYTE	 nClipPrecision=CLIP_DEFAULT_PRECIS, 
		BYTE	 nQuality=DEFAULT_QUALITY 
	);
////////////////////////////////////////////////////////////////////////
private:

	CWnd* parent;

	COLORREF txtcolor;
	LOGFONT  LFT;	

	BOOLEAN  fontcreated;   // set by CreateFont
	BOOLEAN  fontchanged;   // set by fontDialog

	UINT     LineAlignment, LCR_alignment;
	BOOLEAN  VCENTER;

	CString NumberToText(double number, int precision);

};

#endif  // DRTEXT_W_H