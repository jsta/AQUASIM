// AQUASIM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "aquasim.h"

#include "mainfr_w.h"
#include "aqdoc_w.h"
#include "aqview_w.h"
#include "waidlg_w.h"
#include "ednum_w.h"
#include "file_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// global instance of AQUASIM system

AQUASYS aqsys;

/////////////////////////////////////////////////////////////////////////////

// *JR* the following used for detecting memory leaks:
#ifdef _DEBUG

#define MAXOBSERVERS 20

static CMemoryState oldMemState[MAXOBSERVERS];
static CMemoryState newMemState[MAXOBSERVERS];
static CMemoryState diffMemState;

void CAQUASIMApp::setLeakObserver(int number) {
	if (MAXOBSERVERS <= number) {
		TRACE("Bad index %d in setLeakObserver\n", number);
		return;
	}
    oldMemState[number].Checkpoint();
}

void CAQUASIMApp::getLeakState(int number, const char* text) {
	TRACE("\n%d. ", number);
	if (MAXOBSERVERS <= number) {
		TRACE("Bad index %d in getLeakState %s\n", number, text);
		return;
	}
    newMemState[number].Checkpoint();
    if( diffMemState.Difference( oldMemState[number], newMemState[number] ) )
    {
        TRACE( "%s\n", text );
		oldMemState[number].DumpAllObjectsSince();
		diffMemState.DumpStatistics();
    }
	else
	{	TRACE( "No leak at %s\n", text);
	}
}

#endif

/////////////////////////////////////////////////////////////////////////////
// global functions

static BOOLEAN AskLoadStates();

static BOOLEAN AskSaveStates();

static BOOLEAN CancelFun();

static BOOLEAN AskLoadStates()
{
	switch ( AfxMessageBox(aqapp.ini.T(6),MB_YESNO+MB_ICONQUESTION) ) // AskLoadStates
	{
	case IDNO:
		return FALSE;
	default:
		break;
	}
	return TRUE;
}

static BOOLEAN AskSaveStates()
{
	switch ( AfxMessageBox(aqapp.ini.T(7),MB_YESNO+MB_ICONQUESTION) ) // AskSaveStates
	{
	case IDNO:
		return FALSE;
	default:
		break;
	}
	return TRUE;
}

static BOOLEAN CancelFun()
{
	aqapp.ProcessEvents();

	if ( aqapp.AskUserInterrupt() ) return TRUE;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMApp

INIT CAQUASIMApp::ini(initfile,"WINSCREEN","win_text_",5,5,5,700);


BEGIN_MESSAGE_MAP(CAQUASIMApp, CWinApp)
	//{{AFX_MSG_MAP(CAQUASIMApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EDIT_VARIABLES, OnEditVariables)
	ON_COMMAND(ID_EDIT_PROCESSES, OnEditProcesses)
	ON_COMMAND(ID_EDIT_COMPARTMENTS, OnEditCompartments)
	ON_COMMAND(ID_EDIT_LINKS, OnEditLinks)
	ON_COMMAND(ID_EDIT_SYSTEM, OnEditSystem)
	ON_COMMAND(ID_EDIT_DELETESTATES, OnEditDeletestates)
	ON_COMMAND(ID_CALC_SIMULATION, OnCalcSimulation)
	ON_COMMAND(ID_CALC_SENSITIVITYANALYSIS, OnCalcSensitivityanalysis)
	ON_COMMAND(ID_CALC_PARAMETERESTIMATION, OnCalcParameterestimation)
	ON_COMMAND(ID_VIEW_RESULTS, OnViewResults)
	ON_COMMAND(ID_VIEW_CLOSEDIALOGS, OnViewClosedialogs)
	ON_COMMAND(ID_VIEW_CLOSEPLOTS, OnViewCloseplots)
	ON_COMMAND(ID_EDIT_NUMERICALPARAMETERS, OnEditNumericalparameters)
	ON_COMMAND(ID_FILE_ABOUT, OnFileAbout)
	ON_COMMAND(ID_FILE_REVERT, OnFileRevert)
	ON_COMMAND(ID_FILE_PRINTOPT, OnFilePrintOpt)
	ON_COMMAND(ID_FILE_PRINT_AQUA, OnFilePrintAqua)
	ON_COMMAND(ID_FILE_NEW_AQUA, OnFileNewAqua)
	ON_COMMAND(ID_FILE_OPEN_AQUA, OnFileOpenAqua)
	ON_COMMAND(ID_FILE_CLOSE_AQUA, OnFileCloseAqua)
	ON_COMMAND(ID_FILE_SAVE_AQUA, OnFileSaveAqua)
	ON_COMMAND(ID_FILE_SAVE_AS_AQUA, OnFileSaveAsAqua)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMApp construction

CAQUASIMApp::CAQUASIMApp()
{
	FirstFrame=NULL;	// *JR*
	innermessageloop = 0;
	userinterrupt = FALSE;
	systemfilename[0] = '\0';
	printfilename[0] = '\0';
	plotfilename[0] = '\0';
	listfilename[0] = '\0';
	sensfilename[0] = '\0';
	fitfilename[0] = '\0';
	writedatafilename[0] = '\0';
	readdatafilename[0] = '\0';
	aqsys.SetCancelFun(CancelFun);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAQUASIMApp object

CAQUASIMApp aqapp;

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMApp initialization

BOOL CAQUASIMApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	Enable3dControls();

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAQUASIMDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAQUASIMView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();

	aqsys.SetLoadFun(AskLoadStates);
	aqsys.SetSaveFun(AskSaveStates);

	// initialize log file:
	aqualog.open("aquasim.log",std::ios::out|std::ios::trunc);
	SetFileType("aquasim.log",FTlog,FClog);
	if ( ! aqualog.fail() ) SetJobOut(&aqualog);
	aqsys.InitLogFile();

	// simple command line parsing
	if (m_lpCmdLine[0] == '\0')
	{
		// create a new (empty) document
		OnFileNew();
	}
	else
	{
		char buffer[1024];  // strip quotation marks from double clicked files
		CARDINAL i=0;       // strip following arguments
		CARDINAL j=0;
		if ( m_lpCmdLine[0] != '\"' )
		{
			buffer[i] = m_lpCmdLine[0]; i++;
		}
		j++;
		while ( m_lpCmdLine[j]!='\0' && m_lpCmdLine[j]!='\"' 
			                         && j<1023 )
		{
			if ( m_lpCmdLine[j]==' ' && i==j ) break;
			buffer[i] = m_lpCmdLine[j]; i++; j++;
		}
		buffer[i] = '\0';
		if ( ExistFile(buffer) == TRUE )
		{
			// open an existing document
			OpenDocumentFile(buffer);
			OpenFile(buffer);
		}
		else
		{
			// create a new (empty) document
			OnFileNew();
			char buffer[1024];
			strcpy(buffer,"File \"");
			strcat(buffer,m_lpCmdLine);
			strcat(buffer,"\" not found");
			AfxMessageBox(buffer,MB_OK+MB_ICONEXCLAMATION);
	        aqapp.MainFrame->SetFrameTitle("");
		}
	}

	// Enable drag/drop open
	// m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


int CAQUASIMApp::ExitInstance()
{
#ifndef AQUA_DEMO
	if ( aqsys.Saved() == FALSE )
	{
		switch ( AfxMessageBox("Do you want to save your edits?",            // Save?
							   MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			aqsys.ResetCancelFun();
            aqapp.MainFrame->InactivateRefreshFrameTitle();
			OnFileSaveAqua();
			break;
		default:
			break;
		}
	}
#endif
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_tx_version;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_TX_VERSION, m_tx_version);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAQUASIMApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	return;
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_tx_version.SetWindowText(ProgVersion());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CAQUASIMApp::ProgErr(const char* txt)
{
	char Buffer[1024];
	strcpy(Buffer,"AQUASIM PROGRAM ERROR: ");
	strcat(Buffer,txt);
	AfxMessageBox(Buffer,MB_OK+MB_ICONEXCLAMATION);
	return;
}


void CAQUASIMApp::UpdateDialogs()
{
	EdVarsDlg.UpdateDialog();
	EdProcsDlg.UpdateDialog();
	EdCompsDlg.UpdateDialog();
	EdLinksDlg.UpdateDialog();
	DelStatesDlg.UpdateDialog();
	CaSimDlg.UpdateDialog();
	CaSensAnDlg.UpdateDialog();
	CaParEstDlg.UpdateDialog();
	ViResDlg.UpdateDialog();
	return;
}


void CAQUASIMApp::ReadDialogFields()
{
	CaSimDlg.ReadFields(FALSE);
	CaSensAnDlg.ReadFields(FALSE);
	CaParEstDlg.ReadFields(FALSE);
	return;
}


void CAQUASIMApp::NoteInnerMessageLoop()
{
	Beep(4000,100);
	return;
}


void CAQUASIMApp::ProcessEvents()
{
	innermessageloop++;

	tagMSG msg;
	while ( ::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE) )
	{
		if ( !PumpMessage() )
		{
			break;
		}
	}
	LONG idle = 0;
	while ( AfxGetApp()->OnIdle(idle++) )

	innermessageloop--;
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMApp commands

void CAQUASIMApp::OnFileNewAqua() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

#ifndef AQUA_DEMO
	if ( aqsys.Saved() == FALSE )
	{
		switch ( AfxMessageBox("Do you want to save your edits?",            // Save?
							   MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			aqsys.ResetCancelFun();
			OnFileSaveAqua();
			break;
		default:
			break;
		}
	}
#endif

	aqapp.systemfilename[0] = '\0';
	aqapp.printfilename[0] = '\0';
	aqapp.plotfilename[0] = '\0';
	aqapp.listfilename[0] = '\0';
	aqapp.sensfilename[0] = '\0';
	aqapp.fitfilename[0] = '\0';
	aqsys.Delete();
	UpdateDialogs();
	aqapp.MainFrame->SetFrameTitle("");
	return;
}


void CAQUASIMApp::OpenFile(char* name)
{
	char filter[260];
	char filename[260];
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

#if AQUA_OS != AQUA_MAC
	strcpy(filter,"AQUASIM Files (*.aqu)0*.aqu0All Files (*.*)0*.*0");
	filter[21]= '\0'; filter[27] = '\0';
	filter[43]= '\0'; filter[47] = '\0';
#else
	strcpy(filter,"AQUASIM Files0AQU_0Text Files0TEXT;????;BINA;AQU_0");
	filter[13]= '\0'; filter[18] = '\0';
	filter[29]= '\0'; filter[49] = '\0';
#endif
	filename[0] = '\0';
	filetitle[0] = '\0';
	strcpy(title,"Select AQUASIM system file to open:");
	dirname[0] = '\0';
	strcpy(fileextension,"aqu");
 
	OPENFILENAME filenamestruct;
    filenamestruct.lStructSize       = sizeof(OPENFILENAME); 
    filenamestruct.hwndOwner         = NULL; 
    filenamestruct.hInstance         = NULL; 
    filenamestruct.lpstrFilter       = filter; 
    filenamestruct.lpstrCustomFilter = NULL; 
    filenamestruct.nMaxCustFilter    = 0; 
    filenamestruct.nFilterIndex      = 0; 
    filenamestruct.lpstrFile         = filename; 
    filenamestruct.nMaxFile          = sizeof(filename); 
    filenamestruct.lpstrFileTitle    = filetitle; 
    filenamestruct.nMaxFileTitle     = sizeof(filetitle); 
    filenamestruct.lpstrInitialDir   = dirname; 
    filenamestruct.lpstrTitle        = title; 
    filenamestruct.Flags             = OFN_EXPLORER |
		                               OFN_FILEMUSTEXIST |
									   OFN_HIDEREADONLY |
									   OFN_LONGNAMES |
									   OFN_PATHMUSTEXIST; 
    filenamestruct.lpstrDefExt       = fileextension; 
    filenamestruct.lCustData         = NULL; 
    filenamestruct.lpfnHook          = NULL; 
    filenamestruct.lpTemplateName    = NULL; 

	BOOLEAN doload = FALSE;
	if ( name != 0 )
	{
		if ( strlen(name) < 259 )
		{
			if ( ExistFile(name) == TRUE )
			{
				strcpy(filename,name);
				strcpy(filetitle,name);
				doload = TRUE;
			}
		}
	}
	else
	{
		innermessageloop++;
		if ( GetOpenFileName(&filenamestruct) )
		{
			doload = TRUE;
#if AQUA_OS == AQUA_MAC
			char fullfilename[260]; char** p;
			GetFullPathName(filename,260,fullfilename,p);
			strcpy(filename,fullfilename);
#else
			strcpy(filename,filenamestruct.lpstrFile);
			strcpy(filetitle,filenamestruct.lpstrFileTitle);
#endif
		}
		innermessageloop--;
	}
	if ( doload == TRUE )
	{
		aqapp.CancelDlg.Display(aqapp.ini.T(109)); // Loading
		aqapp.printfilename[0] = '\0';
		aqapp.plotfilename[0] = '\0';
		aqapp.listfilename[0] = '\0';
		aqapp.sensfilename[0] = '\0';
		aqapp.fitfilename[0] = '\0';
		std::ifstream openfile;
		openfile.open(filename,std::ios::in);
		LOADRESULT status = aqsys.Load(openfile);
		openfile.close();
		aqapp.CancelDlg.Remove();
		UpdateDialogs();
		switch ( status )
		{
			case LoadSuccess:
				strcpy(aqapp.systemfilename,filename);
				strcpy(aqapp.printfilename,filetitle);
				{
					CARDINAL i = 0;
					while ( aqapp.printfilename[i] != '\0' )
					{
						if ( aqapp.printfilename[i] == '.' )
						{
							aqapp.printfilename[i] = '\0';
							break;
						}
						i++;
					}
				}
				strcpy(aqapp.plotfilename,aqapp.printfilename);
				strcpy(aqapp.listfilename,aqapp.printfilename);
				strcpy(aqapp.sensfilename,aqapp.printfilename);
				strcpy(aqapp.fitfilename,aqapp.printfilename);
				strcat(aqapp.printfilename,".prn");
				strcat(aqapp.listfilename,".lis");
                if ( aqsys.PlotFilOpt()->Eps() == TRUE )
                {
                   strcat(aqapp.plotfilename,".eps");
                }
                else
                {
                   strcat(aqapp.plotfilename,".ps");
                }
				strcat(aqapp.sensfilename,".sen");
				strcat(aqapp.fitfilename,".fit");
				MainFrame->SetFrameTitle(filetitle, fileextension);		// *JR* aug96
            	AfxMessageBox(aqapp.ini.T(1),MB_OK+MB_ICONINFORMATION); // LoadSuccess
				break;
			case LoadInterrupt:
				aqapp.systemfilename[0] = '\0';
				AfxMessageBox(aqapp.ini.T(2),MB_OK+MB_ICONEXCLAMATION); // LoadInterrupt
				break;
			case WrongFileType:
				aqapp.systemfilename[0] = '\0';
				AfxMessageBox(aqapp.ini.T(3),MB_OK+MB_ICONERROR); // WrongFileType
				break;
			case WrongFileVersion:
				aqapp.systemfilename[0] = '\0';
				AfxMessageBox(aqapp.ini.T(4),MB_OK+MB_ICONERROR); // WrongFileVers
				break;
			case ReadError:
				aqapp.systemfilename[0] = '\0';
				AfxMessageBox(aqapp.ini.T(5),MB_OK+MB_ICONERROR); // LoadError
				break;
		}
	}
	else
	{
		// no file selected
	}
	return;
}


void CAQUASIMApp::OnFileOpenAqua() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

#ifndef AQUA_DEMO
	if ( aqsys.Saved() == FALSE )
	{
		switch ( AfxMessageBox("Do you want to save your edits?",            // Save?
							   MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			aqsys.ResetCancelFun();
			OnFileSaveAqua();
			break;
		default:
			break;
		}
	}
#endif

	OpenFile();
	return;
}


void CAQUASIMApp::OnFileCloseAqua() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

#ifndef AQUA_DEMO
	if ( aqsys.Saved() == FALSE )
	{
		switch ( AfxMessageBox("Do you want to save your edits?",            // Save?
							   MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			aqsys.ResetCancelFun();
			OnFileSaveAqua();
			break;
		default:
			break;
		}
	}
#endif

	aqapp.systemfilename[0] = '\0';
	aqapp.printfilename[0] = '\0';
	aqapp.plotfilename[0] = '\0';
	aqapp.listfilename[0] = '\0';
	aqapp.sensfilename[0] = '\0';
	aqapp.fitfilename[0] = '\0';
	aqsys.Delete();
	UpdateDialogs();
	aqapp.MainFrame->SetFrameTitle("");				// *JR* aug96
	return;
}


void CAQUASIMApp::OnFileSaveAqua() 
{
#ifdef AQUA_DEMO
   AfxMessageBox("Demo Version - Save not possible",MB_OK+MB_ICONEXCLAMATION);
   return;
#endif

	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	if ( !strlen(aqapp.systemfilename) > 0 )
	{
		OnFileSaveAsAqua();
		return;
	}

	ReadDialogFields();
	aqapp.CancelDlg.Display(aqapp.ini.T(112)); // Saving
	std::ofstream savefile;
	savefile.open(aqapp.systemfilename,std::ios::out);
    SetFileType(aqapp.systemfilename,FTaqu,FCaqu);
	JOBSTATUS status = aqsys.Save(savefile);
	savefile.close();
	aqapp.CancelDlg.Remove();
	UpdateDialogs();
	aqapp.MainFrame->RefreshFrameTitle();	// *JR* aug96
	switch ( status )
	{
		case OK:
           	AfxMessageBox(aqapp.ini.T(113),MB_OK+MB_ICONINFORMATION); // SaveSuccess
			break;
		case INTERRUPT:
			AfxMessageBox(aqapp.ini.T(114),MB_OK+MB_ICONEXCLAMATION); // SaveInterrupt
			break;
		case PROBLEM:
			AfxMessageBox(aqapp.ini.T(115),MB_OK+MB_ICONERROR); // SaveError
			break;
	}
	return;
}


void CAQUASIMApp::OnFileSaveAsAqua() 
{
#ifdef AQUA_DEMO
   AfxMessageBox("Demo Version - Save not possible",MB_OK+MB_ICONEXCLAMATION);
   return;
#endif

    if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	ReadDialogFields();
	char filter[260];
	char filename[260];
	strcpy(filename,aqapp.systemfilename);
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

#if AQUA_OS != AQUA_MAC
	strcpy(filter,"AQUASIM Files (*.aqu)0*.aqu0");
	filter[21]= '\0'; filter[27] = '\0';
#else
	strcpy(filter,"AQUASIM Files0AQU_0");
	filter[13]= '\0'; filter[18] = '\0';
#endif
	strcpy(filename,aqapp.systemfilename);
	filetitle[0] = '\0';
	strcpy(title,"Select AQUASIM system file name:");
	dirname[0] = '\0';
	strcpy(fileextension,"aqu");
 
	OPENFILENAME filenamestruct;
    filenamestruct.lStructSize       = sizeof(OPENFILENAME); 
    filenamestruct.hwndOwner         = NULL; 
    filenamestruct.hInstance         = NULL; 
    filenamestruct.lpstrFilter       = filter; 
    filenamestruct.lpstrCustomFilter = NULL; 
    filenamestruct.nMaxCustFilter    = 0; 
    filenamestruct.nFilterIndex      = 0; 
    filenamestruct.lpstrFile         = filename; 
    filenamestruct.nMaxFile          = sizeof(filename); 
    filenamestruct.lpstrFileTitle    = filetitle; 
    filenamestruct.nMaxFileTitle     = sizeof(filetitle); 
    filenamestruct.lpstrInitialDir   = dirname; 
    filenamestruct.lpstrTitle        = title; 
    filenamestruct.Flags             = OFN_EXPLORER |
									   OFN_HIDEREADONLY |
									   OFN_OVERWRITEPROMPT |
									   OFN_LONGNAMES;
    filenamestruct.lpstrDefExt       = fileextension; 
    filenamestruct.lCustData         = NULL; 
    filenamestruct.lpfnHook          = NULL; 
    filenamestruct.lpTemplateName    = NULL; 

	innermessageloop++;
	if ( GetSaveFileName(&filenamestruct) )
	{
#if AQUA_OS == AQUA_MAC
		char fullfilename[260]; char** p;
		GetFullPathName(filename,260,fullfilename,p);
		strcpy(filename,fullfilename);
#endif
		aqapp.CancelDlg.Display(aqapp.ini.T(112)); // Saving
		aqapp.printfilename[0] = '\0';
		std::ofstream savefile;
		savefile.open(filename,std::ios::out);
        SetFileType(filename,FTaqu,FCaqu);
		JOBSTATUS status = aqsys.Save(savefile);
		savefile.close();
		aqapp.CancelDlg.Remove();
		UpdateDialogs();
		switch ( status )
		{
			case OK:
				strcpy(aqapp.systemfilename,filename);
				strcpy(aqapp.printfilename,filetitle);
				{
					CARDINAL i = 0;
					while ( aqapp.printfilename[i] != '\0' )
					{
						if ( aqapp.printfilename[i] == '.' )
						{
							aqapp.printfilename[i] = '\0';
							break;
						}
						i++;
					}
				}
				strcpy(aqapp.plotfilename,aqapp.printfilename);
				strcpy(aqapp.listfilename,aqapp.printfilename);
				strcpy(aqapp.sensfilename,aqapp.printfilename);
				strcpy(aqapp.fitfilename,aqapp.printfilename);
				strcat(aqapp.printfilename,".prn");
				strcat(aqapp.listfilename,".lis");
                if ( aqsys.PlotFilOpt()->Eps() == TRUE )
                {
                   strcat(aqapp.plotfilename,".eps");
                }
                else
                {
                   strcat(aqapp.plotfilename,".ps");
                }
				strcat(aqapp.sensfilename,".sen");
				strcat(aqapp.fitfilename,".fit");
				if ( aqsys.CancelFun() != 0 )  // not at exit!
				{
					aqapp.MainFrame->SetFrameTitle(filetitle,fileextension);					// *JR* aug96
				}
				AfxMessageBox(aqapp.ini.T(113),MB_OK+MB_ICONINFORMATION); // SaveSuccess
				break;
			case INTERRUPT:
				AfxMessageBox(aqapp.ini.T(114),MB_OK+MB_ICONEXCLAMATION); // SaveInterrupt
				break;
			case PROBLEM:
				AfxMessageBox(aqapp.ini.T(115),MB_OK+MB_ICONERROR); // SaveError
				break;
		}
	}
	else
	{
		// no file selected
	}
	innermessageloop--;
	return;
}


void CAQUASIMApp::OnFileRevert() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	if ( !strlen(aqapp.systemfilename) > 0 )
	{
		aqapp.ProgErr("Do not know to which file");
		return;
	}
	switch ( AfxMessageBox(aqapp.ini.T(110),            // DelEdits?
						   MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	
	CString filetitle=aqapp.MainFrame->FileTitle();					// *JR* aug96
//	aqapp.MainFrame->plotmanager.DeleteAllViews();					// *JR* aug96
	aqapp.CancelDlg.Display(aqapp.ini.T(109)); // Loading
	std::ifstream openfile;
	openfile.open(aqapp.systemfilename,std::ios::in);
	LOADRESULT status = aqsys.Load(openfile);
	openfile.close();
	aqapp.CancelDlg.Remove();
	UpdateDialogs();
	aqapp.MainFrame->RefreshFrameTitle();					// *JR* aug96
	switch ( status )
	{
		case LoadSuccess:
           	AfxMessageBox(aqapp.ini.T(1),MB_OK+MB_ICONINFORMATION); // LoadSuccess
			break;
		case LoadInterrupt:
			aqapp.systemfilename[0] = '\0';
			AfxMessageBox(aqapp.ini.T(2),MB_OK+MB_ICONEXCLAMATION); // LoadInterrupt
			break;
		case WrongFileType:
			aqapp.systemfilename[0] = '\0';
			AfxMessageBox(aqapp.ini.T(3),MB_OK+MB_ICONERROR); // WrongFileType
			break;
		case WrongFileVersion:
			aqapp.systemfilename[0] = '\0';
			AfxMessageBox(aqapp.ini.T(4),MB_OK+MB_ICONERROR); // WrongFileVers
			break;
		case ReadError:
			aqapp.systemfilename[0] = '\0';
			AfxMessageBox(aqapp.ini.T(5),MB_OK+MB_ICONERROR); // LoadError
			break;
	}
	return;
}


void CAQUASIMApp::OnFilePrintOpt() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	CFilePrintOpt FilePrintOptDlg;
	FilePrintOptDlg.DoModal();

	return;
}


void CAQUASIMApp::OnFilePrintAqua() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	ReadDialogFields();
	char filter[260];
	char filename[260];
	strcpy(filename,aqapp.printfilename);
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

	strcpy(filter,"Print Files0*.prn0");
	filter[11] = '\0'; filter[17] = '\0';
	filetitle[0] = '\0';
	strcpy(title,"Select print file name:");
	dirname[0] = '\0';
	strcpy(fileextension,"prn");
 
	OPENFILENAME filenamestruct;
    filenamestruct.lStructSize       = sizeof(OPENFILENAME); 
    filenamestruct.hwndOwner         = NULL; 
    filenamestruct.hInstance         = NULL; 
    filenamestruct.lpstrFilter       = filter; 
    filenamestruct.lpstrCustomFilter = NULL; 
    filenamestruct.nMaxCustFilter    = 0; 
    filenamestruct.nFilterIndex      = 0; 
    filenamestruct.lpstrFile         = filename; 
    filenamestruct.nMaxFile          = sizeof(filename); 
    filenamestruct.lpstrFileTitle    = filetitle; 
    filenamestruct.nMaxFileTitle     = sizeof(filetitle); 
    filenamestruct.lpstrInitialDir   = dirname; 
    filenamestruct.lpstrTitle        = title; 
    filenamestruct.Flags             = OFN_EXPLORER |
									   OFN_HIDEREADONLY |
									   OFN_OVERWRITEPROMPT |
									   OFN_LONGNAMES;
    filenamestruct.lpstrDefExt       = fileextension; 
    filenamestruct.lCustData         = NULL; 
    filenamestruct.lpfnHook          = NULL; 
    filenamestruct.lpTemplateName    = NULL; 

	innermessageloop++;
	if ( GetSaveFileName(&filenamestruct) )
	{
		aqapp.CancelDlg.Display(aqapp.ini.T(116)); // Printing
		std::ofstream printfile;
		printfile.open(filename,std::ios::out);
		SetFileType(filename,FTprn,FCprn);
		JOBSTATUS status = aqsys.Write(printfile);
		printfile.close();
		aqapp.CancelDlg.Remove();
		UpdateDialogs();
		switch ( status )
		{
			case OK:
				strcpy(aqapp.printfilename,filename);
		    	AfxMessageBox(aqapp.ini.T(117),MB_OK+MB_ICONINFORMATION); // PrintSuccess
				break;
			case INTERRUPT:
				AfxMessageBox(aqapp.ini.T(118),MB_OK+MB_ICONEXCLAMATION); // PrintInterrupt
				break;
			case PROBLEM:
				AfxMessageBox(aqapp.ini.T(119),MB_OK+MB_ICONERROR); // PrintError
				break;
		}
	}
	else
	{
		// no file selected
	}
	innermessageloop--;
	return;
}


void CAQUASIMApp::OnFileAbout() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	OnAppAbout();
	return;
}


void CAQUASIMApp::OnEditSystem() 
{
	OnEditVariables();
	OnEditProcesses();
	OnEditCompartments();
	OnEditLinks();
	EdVarsDlg.BringWindowToTop();
	return;
}


void CAQUASIMApp::OnEditVariables() 
{
	if ( EdVarsDlg.Created() )
	{
	   EdVarsDlg.BringWindowToTop();
	}
	else
	{
	   EdVarsDlg.Create(IDD_EDIT_VARIABLES);
	}
	return;
}


void CAQUASIMApp::OnEditProcesses() 
{
	if ( EdProcsDlg.Created() )
	{
	   EdProcsDlg.BringWindowToTop();
	}
	else
	{
	   EdProcsDlg.Create(IDD_EDIT_PROCESSES);
	}
	return;
}


void CAQUASIMApp::OnEditCompartments() 
{
	if ( EdCompsDlg.Created() )
	{
	   EdCompsDlg.BringWindowToTop();
	}
	else
	{
	   EdCompsDlg.Create(IDD_EDIT_COMPARTMENTS);
	}
	return;
}


void CAQUASIMApp::OnEditLinks() 
{
	if ( EdLinksDlg.Created() )
	{
	   EdLinksDlg.BringWindowToTop();
	}
	else
	{
	   EdLinksDlg.Create(IDD_EDIT_LINKS);
	}
	return;
}


void CAQUASIMApp::OnEditNumericalparameters() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	CEdNumPar EdNumParDlg;
	EdNumParDlg.DoModal();
	return;
}


void CAQUASIMApp::OnEditDeletestates() 
{
	if ( InnerMessageLoop() )
	{
		NoteInnerMessageLoop();
		return;
	}

	if ( DelStatesDlg.Created() )
	{
	   DelStatesDlg.BringWindowToTop();
	}
	else
	{
	   DelStatesDlg.Create(IDD_EDIT_DELSTATES);
	}
	return;	
}


void CAQUASIMApp::OnCalcSimulation() 
{
	if ( CaSimDlg.Created() )
	{
	   CaSimDlg.BringWindowToTop();
	}
	else
	{
	   CaSimDlg.Create(IDD_CALC_SIMULATION);
	}
	return;	
}


void CAQUASIMApp::OnCalcSensitivityanalysis() 
{
	if ( CaSensAnDlg.Created() )
	{
	   CaSensAnDlg.BringWindowToTop();
	}
	else
	{
	   CaSensAnDlg.Create(IDD_CALC_SENSANALYSIS);
	}
	return;		
}


void CAQUASIMApp::OnCalcParameterestimation() 
{
	if ( CaParEstDlg.Created() )
	{
	   CaParEstDlg.BringWindowToTop();
	}
	else
	{
	   CaParEstDlg.Create(IDD_CALC_PARESTIMATION);
	}
	return;		
}


void CAQUASIMApp::OnViewResults() 
{
	if ( ViResDlg.Created() )
	{
	   ViResDlg.BringWindowToTop();
	}
	else
	{
	   ViResDlg.Create(IDD_VIEW_RESULTS);
	}
	return;		
}


void CAQUASIMApp::OnViewClosedialogs() 
{
	EdVarsDlg.DestroyWindow();
	EdProcsDlg.DestroyWindow();
	EdCompsDlg.DestroyWindow();
	EdLinksDlg.DestroyWindow();
	DelStatesDlg.DestroyWindow();
	CaSimDlg.DestroyWindow();
	CaSensAnDlg.DestroyWindow();
	CaParEstDlg.DestroyWindow();

	// ViResDlg is not closed intentionally!

	return;
}


void CAQUASIMApp::OnViewCloseplots() 
{
	aqapp.MainFrame->plotmanager.DeleteAllViews();
	return;
}


/////////////////////////////////////////////////////////////////////////////



