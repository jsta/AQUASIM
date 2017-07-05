#include "stdafx.h"
#include "drtext_w.h"

#include <strstream>

TextTool::TextTool(CWnd* parentWindow)
{
	fontcreated=FALSE;
	fontchanged=FALSE;
	parent=parentWindow;
}

void TextTool::attachToWindow(CWnd* parentWindow)
{
	parent=parentWindow;
	CreateFont();

	AlignOnBase();
}

void TextTool::CreateFont(
		LPCTSTR lpszFacename,
		BYTE nPitchAndFamily, 
		int nWeight, 
		COLORREF rgb,
		BYTE bItalic, 
		BYTE bUnderline, 
		BYTE cStrikeOut, 
		int nHeight, 
		int nWidth, 
		int nEscapement, 
		int nOrientation, 
		BYTE nCharSet, 
		BYTE nOutPrecision, 
		BYTE nClipPrecision, 
		BYTE nQuality 
		)
{
	LFT.lfHeight=nHeight;
	LFT.lfWidth=nWidth;
	LFT.lfEscapement=nEscapement;
	LFT.lfOrientation=nOrientation;
	LFT.lfWeight=nWeight;
	LFT.lfItalic=bItalic;
	LFT.lfUnderline=bUnderline;
	LFT.lfStrikeOut=cStrikeOut;
	LFT.lfCharSet=nCharSet;
	LFT.lfOutPrecision=nOutPrecision;
	LFT.lfClipPrecision=nClipPrecision;
	LFT.lfQuality=nQuality;
	LFT.lfPitchAndFamily=nPitchAndFamily;
	strcpy(LFT.lfFaceName,lpszFacename);
	txtcolor=rgb;

	fontcreated=TRUE;
}

BOOLEAN TextTool::fontDialog()
{
	ASSERT(parent != NULL);	 \

	CClientDC dc(parent);
#if AQUA_OS != MAC_OS
	CFont* current;
	current=dc.GetCurrentFont();
	if (!fontchanged) current->GetLogFont(&LFT);
#endif

	fontchanged=TRUE;

	CFontDialog fontDlg(&LFT,CF_EFFECTS|CF_SCREENFONTS,NULL,parent);
	fontDlg.m_cf.rgbColors=txtcolor;
	if (fontDlg.DoModal() == IDOK)
	{
		txtcolor=fontDlg.GetColor();
		parent->RedrawWindow();
		return TRUE;
	}
	return FALSE;

}

////////////////////////////////////////////////////////
void TextTool::Text (CPoint p, 
			   CString  text,
			   ALIGN    align,
			   int      angle, 
			   COLORREF color)
{
	Text(p.x, p.y, text, align, angle, color);
}
void TextTool::text (CPoint p, 
			   CString  text,
			   ALIGN    align,
			   int      angle, 
			   Color    color)
{
	DColors C;
	Text(p.x, p.y, text, align, angle, C.GetCREF(color));
}
void TextTool::text (int x, int y, 
			   CString  text,
			   ALIGN    align,
			   int      angle, 
			   Color    color)
{
	DColors C;
	Text(x, y, text, align, angle, C.GetCREF(color));
}
void TextTool::Text (int x, int y, 
			   CString  text,
			   ALIGN    align,
			   int      angle, 
			   COLORREF color)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);
	switch (align)
	{
	case Center: LCR_alignment=TA_CENTER;
		break;
	case Right:	 LCR_alignment=TA_RIGHT;
		break;
	default:	 LCR_alignment=TA_LEFT;
	}
	if (VCENTER)	// calculate center for height of text
	{
		CSize s=dc.GetTextExtent(text,text.GetLength());
		int half=s.cy/2;
		int a=int((angle + 450)/900);
		switch (a)
		{
		case  1: x=x-half;
		  break;
		case  2: y=y+half;
		  break;
		case  3: x=x+half;
		  break;
		default: y=y-half;
		}
	}

	if (fontchanged || fontcreated) // || angle)
	{
		int oldAngle=LFT.lfEscapement;
		COLORREF oldcolor=txtcolor;

		if (angle) LFT.lfEscapement=angle;
		if (color != -1) txtcolor=color;
		txtcolor=dc.GetNearestColor(txtcolor);
		dc.SetTextColor(txtcolor);
		CFont font;
		BOOLEAN ok=font.CreateFontIndirect(&LFT);
		CFont* oldFont=dc.SelectObject(&font);
		dc.SetTextAlign(LineAlignment | LCR_alignment);
		dc.TextOut(x, y, text, text.GetLength());
		dc.SelectObject(oldFont);
		LFT.lfEscapement=oldAngle;
		txtcolor=oldcolor;
	}
}

///////////////////////////////////////////////////////
	
void TextTool::Number(CPoint  p, 
			   double	number,
			   int		precision,
			   ALIGN    align,
			   int      angle, 
			   COLORREF color)
{
	CString s=NumberToText(number,precision);
	Text(p.x, p.y, s, align, angle, color);
}
	
void TextTool::number(CPoint  p, 
			   double	number,
			   int		precision,
			   ALIGN    align,
			   int      angle, 
			   Color    color)
{
	CString s=NumberToText(number,precision);
	DColors C;
	Text(p.x, p.y, s, align, angle, C.GetCREF(color));
}
void TextTool::Number(int x, int y, 
			   double	number,
			   int		precision,
			   ALIGN    align,
			   int      angle, 
			   COLORREF color)
{
	CString s=NumberToText(number,precision);
	Text(x, y, s, align, angle, color);
}
void TextTool::number(int x, int y, 
			   double	number,
			   int		precision,
			   ALIGN    align,
			   int      angle, 
			   Color    color)
{
	CString s=NumberToText(number,precision);
	DColors C;
	Text(x, y, s, align, angle, C.GetCREF(color));
}

///////////////////////////////////////////////////////////

CString TextTool::NumberToText(double number, int precision)
{
   const buflen=48;
   char buf[buflen];
   std::ostrstream oss(buf,sizeof(buf),std::ios::out|std::ios::trunc);
   oss.precision(precision);
   oss << number << std::ends;

	return buf;
}


