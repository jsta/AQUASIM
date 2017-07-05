// mainfr_w.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AQUASIM.h"

#include "mainfr_w.h"

#include "PLOTDO_W.H"							// *JR*
#include "PLOTVW_W.H"							// *JR*

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)	// *JR*

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)		// *JR*
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AQUA, OnUpdateFileSaveAqua)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateFileRevert)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_AQUA, OnUpdateFileSaveAsAqua)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_AQUA, OnUpdateFilePrintAqua)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_AQUA, OnUpdateFileCloseAqua)
	ON_COMMAND(ID_WINDOW_CASCADE, OnWindowCascade)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, OnUpdateWindowCascade)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, OnUpdateWindowTileHorz)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_VERT, OnUpdateWindowTileVert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW_AQUA,
	ID_FILE_OPEN_AQUA,
	ID_FILE_SAVE_AQUA,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
		ID_SEPARATOR,
	ID_EDIT_SYSTEM,
	ID_EDIT_DELETESTATES,
		ID_SEPARATOR,
	ID_CALC_SIMULATION,
	ID_CALC_SENSITIVITYANALYSIS,
	ID_CALC_PARAMETERESTIMATION,
		ID_SEPARATOR,
	ID_VIEW_RESULTS,
		ID_SEPARATOR,
	ID_VIEW_CLOSEDIALOGS
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	TitleTimerID=998;
	titlefile="";
	wait_milliseconds=50;

	coordMenuOff=FALSE;
	CoordNumbersEnabled=TRUE;
	CrossEnabled=FALSE;
	LinesEnabled=TRUE;
	refreshactive = TRUE;
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (aqapp.FirstFrame == NULL)							// *JR*
  {														// *JR*
	currentplotview=NULL;								// *JR*
	GetWindowText(title0);								// *JR* aug96
	titlefile="";										// *JR* aug96
	aqapp.FirstFrame=this;								// *JR*
	aqapp.MainFrame =this;								// *JR*
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

  }                                                         // *JR*
	return 0;
}
///////////////////////////////////////////////////////////////////
// Window Title
CString	CMainFrame::modifyflag()
{	// CAQUASIMApp::OnFileRevert() presumes that modifyflag has
	// exactly 1 blanc and 1 star  
	if (!aqsys.Saved()) return " *";
	else return "";
}
void CMainFrame::SetFrameTitle(CString file, CString ext)
{
	if (0 == file.GetLength() ) titlefile="";
	else titlefile=file.Left(file.GetLength() - ext.GetLength() - 1);
#ifndef AQUA_DEMO
	SetWindowText(title0 + "  " + titlefile + modifyflag());
#else
	SetWindowText(title0 + " Demo " + "  " + titlefile + modifyflag());
#endif
}

BOOLEAN CMainFrame::FileIsOpen()
{
	return 0 < titlefile.GetLength();
}

void CMainFrame::InactivateRefreshFrameTitle()
{
   refreshactive = FALSE;
}

void CMainFrame::RefreshFrameTitle()
{
	if ( refreshactive == TRUE )
	{
#ifndef AQUA_DEMO
    	SetWindowText(title0 + "  " + titlefile + modifyflag());
#else
    	SetWindowText(title0 + " Demo " + "  " + titlefile + modifyflag());
#endif
	}
}

CString CMainFrame::FileTitle() 
{
	return titlefile;
//	return titlefile + modifyflag();
}
void CMainFrame::SetTitleForNoPlots()
{
	SetTimer(TitleTimerID, wait_milliseconds, NULL);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TitleTimerID)
	{
		RefreshFrameTitle();
		KillTimer(TitleTimerID);
		return;
	}
	CMDIFrameWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

JOBSTATUS CMainFrame::MakeChild() 
{
//	AfxMessageBox("Enter OnViewMakechild",MB_OK+MB_ICONEXCLAMATION)



	CDocument* pDoc=GetActiveDocument();
	if (pDoc == NULL) pDoc=new CPlotDoc();

	if (currentdocument->collect_status != OK) 
		return currentdocument->collect_status;

//	CMultiDocTemplate* pPlotViewTemplate;
	CDocTemplate* pPlotViewTemplate;
	pPlotViewTemplate = new CMultiDocTemplate(
//		IDR_MAINFRAME,
		IDR_PLOT_TYPE,
		RUNTIME_CLASS(CPlotDoc),
		RUNTIME_CLASS(CMDIChildWnd),			// main SDI frame window
		RUNTIME_CLASS(CPlotView));
	aqapp.AddDocTemplate(pPlotViewTemplate);
	CMDIChildWnd* pNewFrame 
		= (CMDIChildWnd*)(pPlotViewTemplate->CreateNewFrame(pDoc,NULL));

	pPlotViewTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	return OK;
}

void CMainFrame::ToolDim(CRect* r, BOOLEAN visible)
{ 
	m_wndToolBar.GetWindowRect(r);
	visible=m_wndToolBar.IsWindowVisible( );
}

#ifdef _DEBUG
static long WMCounter=0;	// used for debugging DefWindowProc below
#endif

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
#ifdef _DEBUG
	WMCounter++;
	BOOLEAN localdebug=FALSE;
	if (localdebug) {
		if (message == WM_NCHITTEST) TRACE("MAINFRAME %d WM_NCHITTEST %d wParam %d\n", WMCounter,message, WM_NCHITTEST, wParam);
		if (message == WM_MOUSEMOVE) TRACE("MAINFRAME %d WM_MOUSEMOVE %d wParam %d \n",WMCounter, WM_NCHITTEST, wParam);
		if (message == WM_NCMOUSEMOVE) TRACE("MAINFRAME %d WM_NCMOUSEMOVE %d  wParam %d\n",WMCounter, WM_NCMOUSEMOVE, wParam);
	}
#endif

	if (message == WM_NCHITTEST) {
		if (currentplotview	!= NULL) 
			currentplotview->RemoveCross(5);
	}
	return CMDIFrameWnd::DefWindowProc(message, wParam, lParam);
}

///////////////////////////////////
// Dimming of Popup File Menu Items
///////////////////////////////////

void CMainFrame::OnUpdateFileCloseAqua(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(strlen(aqapp.systemfilename) > 0 || !aqsys.Saved());
	
}

void CMainFrame::OnUpdateFileSaveAqua(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	// pCmdUI->Enable(strlen(aqapp.systemfilename) > 0 && !aqsys.Saved());
	pCmdUI->Enable(!aqsys.Saved());
}

void CMainFrame::OnUpdateFileSaveAsAqua(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(strlen(aqapp.systemfilename) > 0 || !aqsys.Saved());	
}

void CMainFrame::OnUpdateFileRevert(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(strlen(aqapp.systemfilename) > 0 && !aqsys.Saved());
}

void CMainFrame::OnUpdateFilePrintAqua(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(strlen(aqapp.systemfilename) > 0 || !aqsys.Saved());
	
}

void CMainFrame::OnWindowCascade() 
{
	// TODO: Add your command handler code here
	RedrawAllPlots(TRUE);
	
}

void CMainFrame::OnUpdateWindowCascade(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 < aqapp.MainFrame->plotmanager.Count());
	
}

void CMainFrame::OnUpdateWindowTileHorz(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 < aqapp.MainFrame->plotmanager.Count());
}

void CMainFrame::OnUpdateWindowTileVert(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 < aqapp.MainFrame->plotmanager.Count());
}
