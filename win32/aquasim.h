// AQUASIM.h : main header file for the AQUASIM application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "./aqcore/aquasys.h"

#include "edvar_w.h"
#include "edproc_w.h"
#include "edcomp_w.h"
#include "edlink_w.h"
#include "delsta_w.h"
#include "casim_w.h"
#include "casens_w.h"
#include "caest_w.h"
#include "vires_w.h"
#include "waidlg_w.h"

class CMainFrame;				// *JR*

/////////////////////////////////////////////////////////////////////////////
// global AQUASIM system

extern AQUASYS aqsys;

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMApp:
// See AQUASIM.cpp for the implementation of this class
//

class CAQUASIMApp : public CWinApp
{
public:
	CAQUASIMApp();

	CMDIFrameWnd* FirstFrame;	// *JR*
	CMainFrame*   MainFrame;	// *JR*

// next 2 proc for debugging leaks					// *JR* 
#ifdef _DEBUG
void setLeakObserver(int number);					// *JR*
void getLeakState(int number, const char* text);	// *JR*
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAQUASIMApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//}}AFX_VIRTUAL

// Functions

	void	    ProgErr(const char* txt);
	void        UpdateDialogs();
	void	    ReadDialogFields();
	int         InnerMessageLoop()
	               { return innermessageloop; }
	void        NoteInnerMessageLoop();
	void        ProcessEvents();
	void        SetUserInterrupt()
	               { userinterrupt = TRUE; }
	void        ClearUserInterrupt()
	               { userinterrupt = FALSE; }
	BOOL        AskUserInterrupt()
	               { return userinterrupt; }
	void        OpenFile(char* filename = 0);

// Data

    CEdVars     EdVarsDlg;
    CEdProcs    EdProcsDlg;
    CEdComps    EdCompsDlg;
    CEdLinks    EdLinksDlg;
	CDelStates  DelStatesDlg;
	CCaSim      CaSimDlg;
	CCaSensAn   CaSensAnDlg;
	CCaParEst   CaParEstDlg;
	CViRes      ViResDlg;

	CWaitDlg     CancelDlg;
	CWaitSimDlg  CancelSimDlg;
	CWaitSensDlg CancelSensDlg;
	CWaitFitDlg  CancelFitDlg;

	char        systemfilename[260];
	char        printfilename[260];
	char        plotfilename[260];
	char        listfilename[260];
	char        sensfilename[260];
	char        fitfilename[260];
	char        writedatafilename[260];
	char        readdatafilename[260];

	static INIT ini;

// Implementation

	//{{AFX_MSG(CAQUASIMApp)
	afx_msg void OnAppAbout();
	afx_msg void OnEditVariables();
	afx_msg void OnEditProcesses();
	afx_msg void OnEditCompartments();
	afx_msg void OnEditLinks();
	afx_msg void OnEditSystem();
	afx_msg void OnEditDeletestates();
	afx_msg void OnCalcSimulation();
	afx_msg void OnCalcSensitivityanalysis();
	afx_msg void OnCalcParameterestimation();
	afx_msg void OnViewResults();
	afx_msg void OnViewClosedialogs();
	afx_msg void OnViewCloseplots();
	afx_msg void OnEditNumericalparameters();
	afx_msg void OnFileAbout();
	afx_msg void OnFileClose();
	afx_msg void OnFileRevert();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFilePrintOpt();
	afx_msg void OnFilePrintAqua();
	afx_msg void OnFileNewAqua();
	afx_msg void OnFileOpenAqua();
	afx_msg void OnFileCloseAqua();
	afx_msg void OnFileSaveAqua();
	afx_msg void OnFileSaveAsAqua();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	std::ofstream aqualog;
    int      innermessageloop;
	BOOL     userinterrupt;
};


/////////////////////////////////////////////////////////////////////////////
// global AQUASIM application

extern CAQUASIMApp aqapp;

/////////////////////////////////////////////////////////////////////////////
