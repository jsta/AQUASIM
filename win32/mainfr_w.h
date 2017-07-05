// mainfr_w.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
class CPlotDoc;								// *JR*
class PLOT;									// *JR*
#include "plot_w.h"

class CMainFrame : public CMDIFrameWnd		// *JR*
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
    BOOLEAN refreshactive;

// Attributes
public:
	CPlotDoc*   currentdocument;			// *JR*
	CPlotView*	currentplotview;			// *JR*
	PLOT*       currentplot;				// *JR*
	PlotManager plotmanager;				// *JR* aug96
	BOOLEAN		coordMenuOff,				// *JR* aug96
				CoordNumbersEnabled,
				CrossEnabled, LinesEnabled;


	void ToolDim(CRect* r, BOOLEAN visible);
	//	get height, position and state of toolbar

// Operations
public:
	JOBSTATUS MakeChild();							// *JR*

	UINT TitleTimerID; int wait_milliseconds;		// *JR* aug96
	void SetTitleForNoPlots();						// *JR* aug96
	void SetFrameTitle(CString file="", CString ext="");  // *JR* aug96
	void RefreshFrameTitle();						// *JR* aug96
	CString FileTitle();							// *JR* aug96
	CString	modifyflag();							// *JR* aug96
	BOOLEAN FileIsOpen();							// *JR* aug96
	void InactivateRefreshFrameTitle();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CString		title0;								  // *JR* aug96
	CString		titlefile;							  // *JR* aug96

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateFileSaveAqua(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAsAqua(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintAqua(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileCloseAqua(CCmdUI* pCmdUI);
	afx_msg void OnWindowCascade();
	afx_msg void OnUpdateWindowCascade(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowTileHorz(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowTileVert(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
