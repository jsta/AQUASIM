#include "stdafx.h"
#include "DRTXT_W.H"

DrawTEXT::DrawTEXT()
{
	fontcreated=FALSE;
	fontchanged=FALSE;
	parent=NULL;
}

void DrawTEXT::attachToWindow(CWnd* parentWindow)
{
	parent=parentWindow;
}

void DrawTEXT::CreateFont(
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
	lpLogFont.lfHeight=nHeight;
	lpLogFont.lfWidth=nWidth;
	lpLogFont.lfEscapement=nEscapement;
	lpLogFont.lfOrientation=nOrientation;
	lpLogFont.lfWeight=nWeight;
	lpLogFont.lfItalic=bItalic;
	lpLogFont.lfUnderline=bUnderline;
	lpLogFont.lfStrikeOut=cStrikeOut;
	lpLogFont.lfCharSet=nCharSet;
	lpLogFont.lfOutPrecision=nOutPrecision;
	lpLogFont.lfClipPrecision=nClipPrecision;
	lpLogFont.lfQuality=nQuality;
	lpLogFont.lfPitchAndFamily=nPitchAndFamily;
	strcpy(lpLogFont.lfFaceName,lpszFacename);
	txtcolor=rgb;

	fontcreated=TRUE;
}

BOOLEAN DrawTEXT::fontDialog()
{
	ASSERT(parent != NULL);	 \

		CClientDC dc(parent);
	CFont* current=dc.GetCurrentFont();

	if (!fontchanged) current->GetLogFont(&lpLogFont);
	fontchanged=TRUE;

	CFontDialog fontDlg(&lpLogFont,CF_EFFECTS|CF_SCREENFONTS,NULL,parent);
	fontDlg.m_cf.rgbColors=txtcolor;
	if (fontDlg.DoModal() == IDOK)
	{
		txtcolor=fontDlg.GetColor();
		parent->RedrawWindow();
		return TRUE;
	}
	return FALSE;

}

void DrawTEXT::writeLeft(int x, int y, CString text)
{
	ASSERT(parent != NULL);
	CClientDC dc(parent);
	if (fontchanged || fontcreated)
	{
		dc.SetTextColor(txtcolor);
		CFont font;
		BOOLEAN ok=font.CreateFontIndirect(&lpLogFont);
		CFont* old=dc.SelectObject(&font);
	}
 	dc.TextOut(x,y,text,text.GetLength());

}
