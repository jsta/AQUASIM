#ifndef DRTXT_W_H
#define DRTXT_W_H

class DrawTEXT
{
private:

	CWnd* parent;

	LOGFONT lpLogFont;	
	COLORREF txtcolor;

	BOOLEAN fontcreated;   // set by CreateFont
	BOOLEAN fontchanged;   // set by fontDialog

public:
	DrawTEXT();
	~DrawTEXT() {}

	void attachToWindow(CWnd* parentWindow); // to be called e.g. in InitView

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


	BOOLEAN fontDialog();
	void writeLeft(int x, int y, CString text);



};

#endif  // DRTXT_W_H