// casim_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "casim_w.h"
#include "waidlg_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static BOOLEAN ContinueInit(char*,REAL);
static BOOLEAN ContinueCalc(char* name, REAL t);


static BOOLEAN ContinueInit(char*,REAL)
{
	aqapp.ProcessEvents();
	if ( aqapp.AskUserInterrupt() ) return FALSE;
	return TRUE;
}


static BOOLEAN ContinueCalc(char* name, REAL t)
{
	aqapp.ProcessEvents();
	if ( aqapp.AskUserInterrupt() ) return FALSE;
	aqapp.CancelSimDlg.SetCalcname(name);
	aqapp.CancelSimDlg.SetTime(t);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCaSim dialog


CCaSim::CCaSim(CWnd* pParent /*=NULL*/)
	: CDialog(CCaSim::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaSim)
	//}}AFX_DATA_INIT

	created = 0;
}


void CCaSim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaSim)
	DDX_Control(pDX, IDC_LB_AVAILCALC, m_availcalclist);
	DDX_Control(pDX, IDC_LB_ACTCALC, m_actcalclist);
	DDX_Control(pDX, IDC_BU_NEW, m_new);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_BU_INACTCALC, m_inactcalc);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	DDX_Control(pDX, IDC_BU_ACTCALC, m_actcalc);
	DDX_Control(pDX, IDC_BU_INITIALIZE, m_initialize);
	DDX_Control(pDX, IDC_BU_START, m_start);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaSim, CDialog)
	//{{AFX_MSG_MAP(CCaSim)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_INITIALIZE, OnBuInitialize)
	ON_BN_CLICKED(IDC_BU_START, OnBuStart)
	ON_BN_CLICKED(IDC_BU_ACTCALC, OnBuActcalc)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_INACTCALC, OnBuInactcalc)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_LBN_DBLCLK(IDC_LB_ACTCALC, OnDblclkLbActcalc)
	ON_LBN_SELCHANGE(IDC_LB_ACTCALC, OnSelchangeLbActcalc)
	ON_LBN_DBLCLK(IDC_LB_AVAILCALC, OnDblclkLbAvailcalc)
	ON_LBN_SELCHANGE(IDC_LB_AVAILCALC, OnSelchangeLbAvailcalc)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCaSim::UpdateActivation()
{
	int max, numact, numavail, j, k;
	int* indices;

	max = aqsys.Calclist()->Size();
	indices = new int[max];
	numavail = m_availcalclist.GetSelItems(max,indices);
	numact   = m_actcalclist.GetSelCount();
	switch ( numavail )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_actcalc.EnableWindow(FALSE);
		switch ( numact )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		case 0:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_delete.EnableWindow(FALSE);
			m_inactcalc.EnableWindow(FALSE);
			break;
		case 1:
			m_duplicate.EnableWindow(TRUE);
			m_edit.EnableWindow(TRUE);
			m_delete.EnableWindow(TRUE);
			m_inactcalc.EnableWindow(TRUE);
			break;
		default:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_delete.EnableWindow(TRUE);
			m_inactcalc.EnableWindow(TRUE);
			break;
		}
		break;
	case 1:
		m_duplicate.EnableWindow(TRUE);
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		if ( aqsys.Calclist()->Get(indices[0]+1)->SimActive() )
		{
			m_actcalc.EnableWindow(FALSE);
		}
		else
		{
			m_actcalc.EnableWindow(TRUE);
		}
		switch ( numact )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		case 0:
			m_inactcalc.EnableWindow(FALSE);
			break;
		default:
			m_inactcalc.EnableWindow(TRUE);
			break;
		}
		break;
	default:
		k = 0;
		for ( j=0; j<numavail; j++ )
		{
			if ( !aqsys.Calclist()->Get(indices[0]+1)->SimActive() )
			{
				k++;
			}
		}
		if ( k > 0 ) m_actcalc.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		switch ( numact )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		case 0:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_inactcalc.EnableWindow(FALSE);
			break;
		case 1:
			m_duplicate.EnableWindow(TRUE);
			m_edit.EnableWindow(TRUE);
			m_inactcalc.EnableWindow(TRUE);
			break;
		default:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_inactcalc.EnableWindow(TRUE);
			break;
		}
		break;
	}
	delete [] indices;

	if ( m_actcalclist.GetCount() > 0 )
	{
		m_initialize.EnableWindow(TRUE);
		m_start.EnableWindow(TRUE);
	}
	else
	{
		m_initialize.EnableWindow(FALSE);
		m_start.EnableWindow(FALSE);
	}

	return;
}


void CCaSim::UpdateDialog()
{
	if ( created == 0 ) return;

	int acttopindex;
	int availtopindex;

	acttopindex = m_actcalclist.GetTopIndex();
	availtopindex = m_availcalclist.GetTopIndex();
	m_actcalclist.ResetContent();
	m_availcalclist.ResetContent();
	CALC* calc = aqsys.Calclist()->First();
	while ( calc != 0 )
	{
		if ( calc->SimActive() == TRUE )
		{
			m_actcalclist.InsertString(-1,calc->Symbol());
		}
		m_availcalclist.InsertString(-1,calc->Symbol());
		calc = calc->Next();
	}
	m_actcalclist.SetTopIndex(acttopindex);
	m_availcalclist.SetTopIndex(availtopindex);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


BOOL CCaSim::ReadFields(BOOL notify)
{
	if ( created == 0 ) return FALSE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCaSim message handlers

BOOL CCaSim::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	lastselact = TRUE;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaSim::OnOK() // Close button
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();
	return;
}


void CCaSim::OnCancel() // Close button 
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();
	return;
}


void CCaSim::OnClose() // Close box
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();

	CDialog::OnClose();
}


void CCaSim::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CCaSim::OnBuNew() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CCaSensCalc CCaSensCalcDlg;
	CCaSensCalcDlg.DoModal(0,TRUE);
	aqapp.UpdateDialogs();
	return;
}


void CCaSim::OnBuDuplicate() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Calclist()->Size();
	int* indices = new int[max];
	if ( lastselact == TRUE )
	{
		int num = m_actcalclist.GetSelItems(max,indices);
		if ( num == 1 )
		{
			char buffer[1024];
			int res = m_actcalclist.GetText(indices[0],buffer);
			CALC* calc = aqsys.Calclist()->Get(buffer);
			CCaSensCalc CCaSensCalcDlg;
			CCaSensCalcDlg.DoModal(calc,TRUE);
		}
		else
		{
			num = m_availcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				CALC* calc = aqsys.Calclist()->Get(indices[0]+1);
				CCaSensCalc CCaSensCalcDlg;
				CCaSensCalcDlg.DoModal(calc,TRUE);
			}
			else
			{
				aqapp.ProgErr("Illegal selection");
				delete [] indices;
				return;
			}
		}
	}
	else
	{
		int num = m_availcalclist.GetSelItems(max,indices);
		if ( num == 1 )
		{
			CALC* calc = aqsys.Calclist()->Get(indices[0]+1);
			CCaSensCalc CCaSensCalcDlg;
			CCaSensCalcDlg.DoModal(calc,TRUE);
		}
		else
		{
			num = m_actcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				char buffer[1024];
				int res = m_actcalclist.GetText(indices[0],buffer);
				CALC* calc = aqsys.Calclist()->Get(buffer);
				CCaSensCalc CCaSensCalcDlg;
				CCaSensCalcDlg.DoModal(calc,TRUE);
			}
			else
			{
				aqapp.ProgErr("Illegal selection");
				delete [] indices;
				return;
			}
		}
	}
	delete [] indices;

	aqapp.UpdateDialogs();
	return;
}


void CCaSim::OnBuEdit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Calclist()->Size();
	int* indices = new int[max];
	if ( lastselact == TRUE )
	{
		int num = m_actcalclist.GetSelItems(max,indices);
		if ( num == 1 )
		{
			char buffer[1024];
			int res = m_actcalclist.GetText(indices[0],buffer);
			CALC* calc = aqsys.Calclist()->Get(buffer);
			CCaSensCalc CCaSensCalcDlg;
			CCaSensCalcDlg.DoModal(calc,FALSE);
		}
		else
		{
			num = m_availcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				CALC* calc = aqsys.Calclist()->Get(indices[0]+1);
				CCaSensCalc CCaSensCalcDlg;
				CCaSensCalcDlg.DoModal(calc,FALSE);
			}
			else
			{
				aqapp.ProgErr("Illegal selection");
				delete [] indices;
				return;
			}
		}
	}
	else
	{
		int num = m_availcalclist.GetSelItems(max,indices);
		if ( num == 1 )
		{
			CALC* calc = aqsys.Calclist()->Get(indices[0]+1);
			CCaSensCalc CCaSensCalcDlg;
			CCaSensCalcDlg.DoModal(calc,FALSE);
		}
		else
		{
			num = m_actcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				char buffer[1024];
				int res = m_actcalclist.GetText(indices[0],buffer);
				CALC* calc = aqsys.Calclist()->Get(buffer);
				CCaSensCalc CCaSensCalcDlg;
				CCaSensCalcDlg.DoModal(calc,FALSE);
			}
			else
			{
				aqapp.ProgErr("Illegal selection");
				delete [] indices;
				return;
			}
		}
	}
	delete [] indices;

	aqapp.UpdateDialogs();
	return;
}


void CCaSim::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	if ( lastselact == FALSE )
	{
		switch ( MessageBox(aqapp.ini.T(23),         // DelAvailCalc?
					        aqapp.ini.T(111),
							MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			break;
		default:
			return;
		}
		int max = aqsys.Calclist()->Size();
		int* indices = new int[max];
		int num = m_availcalclist.GetSelItems(max,indices);
		int j;
		char buffer[1024];

		switch ( num )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		default:
			for ( j=0; j<num; j++ )
			{
				if ( m_availcalclist.GetText(indices[j],buffer) <= 0 )
				{
					aqapp.ProgErr("Illegal index");
					break;
				}
				CALC* calc = aqsys.Calclist()->Get(buffer);
				if ( calc == 0 )
				{
					aqapp.ProgErr("Illegal selection");
					break;
				}
				aqsys.DeleteCalc(calc);
			}
			break;
		}
		delete [] indices;
	}
	else
	{
		switch ( MessageBox(aqapp.ini.T(266),        // DelActSens?
					        aqapp.ini.T(111),
							MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			break;
		default:
			return;
		}
		int max = aqsys.Calclist()->Size();
		int* indices = new int[max];
		int num = m_actcalclist.GetSelItems(max,indices);
		int j;
		char buffer[1024];

		switch ( num )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		default:
			for ( j=0; j<num; j++ )
			{
				if ( m_actcalclist.GetText(indices[j],buffer) <= 0 )
				{
					aqapp.ProgErr("Illegal index");
					break;
				}
				CALC* calc = aqsys.Calclist()->Get(buffer);
				if ( calc == 0 )
				{
					aqapp.ProgErr("Illegal selection");
					break;
				}
				aqsys.DeleteCalc(calc);
			}
			break;
		}
		delete [] indices;
	}

	aqapp.UpdateDialogs();
	return; 
}


void CCaSim::OnBuActcalc() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Calclist()->Size();
	int* indices = new int[max];
	int num = m_availcalclist.GetSelItems(max,indices);
	int j;
	char buffer[1024];

	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	default:
		for ( j=0; j<num; j++ )
		{
			if ( m_availcalclist.GetText(indices[j],buffer) <= 0 )
			{
				aqapp.ProgErr("Illegal index");
				break;
			}
			CALC* calc = aqsys.Calclist()->Get(buffer);
			if ( calc == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			if ( aqsys.SimActive(calc,TRUE) == FALSE )
			{
				char buffer[1024];
				strcpy(buffer,"Unable to activate calculation ");
				strcat(buffer,calc->Symbol());
				MessageBox(buffer,
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaSim::OnBuInactcalc() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Calclist()->Size();
	int* indices = new int[max];
	int num = m_actcalclist.GetSelItems(max,indices);
	int j;
	char buffer[1024];

	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	default:
		for ( j=0; j<num; j++ )
		{
			if ( m_actcalclist.GetText(indices[j],buffer) <= 0 )
			{
				aqapp.ProgErr("Illegal index");
				break;
			}
			CALC* calc = aqsys.Calclist()->Get(buffer);
			if ( calc == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			aqsys.SimActive(calc,FALSE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaSim::OnDblclkLbActcalc() 
{
	lastselact = TRUE;
    OnBuEdit();
	return;
}


void CCaSim::OnDblclkLbAvailcalc() 
{
	lastselact = FALSE;
    OnBuEdit();
	return;
}


void CCaSim::OnSelchangeLbActcalc() 
{
	lastselact = TRUE;
	UpdateActivation();
	return;
}


void CCaSim::OnSelchangeLbAvailcalc() 
{
	lastselact = FALSE;
	UpdateActivation();
	return;
}


void CCaSim::OnBuInitialize() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	aqsys.SetCalcOut(ContinueInit);
	aqapp.CancelDlg.Display((const char*)"...calculating initial condition..."); // Calculating
	JOBSTATUS status =  aqsys.InitSim();
	aqapp.CancelDlg.Remove();
	aqsys.SetCalcOut(0);
	aqapp.DelStatesDlg.UpdateDialog();
	Beep(4000,100);	
	aqapp.MainFrame->RefreshFrameTitle();
	switch ( status )
	{
	case OK:
		MessageBox(aqapp.ini.T(277),    // InitSuccess
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case INTERRUPT:
		MessageBox(aqapp.ini.T(278),    // InitInterrupt
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case PROBLEM:
		MessageBox(aqapp.ini.T(279),    // InitFail
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		break;
	}

	return;
}


void CCaSim::OnBuStart() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	aqsys.SetCalcOut(ContinueCalc);
	aqapp.CancelSimDlg.Display("...calculating dynamic solution..."); // Calculating
	JOBSTATUS status = aqsys.Simulate();
	aqapp.CancelSimDlg.Remove();
	aqsys.SetCalcOut(0);
	aqapp.DelStatesDlg.UpdateDialog();
	Beep(4000,100);	
	aqapp.MainFrame->RefreshFrameTitle();
	switch ( status )
	{
	case OK:
		MessageBox(aqapp.ini.T(280),      // CalcSuccess
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case INTERRUPT:
		MessageBox(aqapp.ini.T(281),      // CalcInterrupt
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case PROBLEM:
		MessageBox(aqapp.ini.T(282),      // CalcFail
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		break;
	}

	return;
}



