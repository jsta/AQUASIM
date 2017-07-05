// casens_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "casens_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static CARDINAL numsim;
static CARDINAL cursim;
static CARDINAL numpar;

/////////////////////////////////////////////////////////////////////////////

static BOOLEAN ContinueSens(char*,REAL);
static void    SensOut();


static BOOLEAN ContinueSens(char*,REAL)
{
	aqapp.ProcessEvents();
	if ( aqapp.AskUserInterrupt() ) return FALSE;
	return TRUE;
}


static void SensOut()
{
	if ( cursim == 0 )
	{
		CARDINAL numsens = 0;
		CALC* calc = aqsys.Calclist()->First();
		while ( calc != 0 )
		{
			if ( calc->SensActive() == TRUE ) numsens++;
			calc = calc->Next();
		}
		numsim = numsens*(numpar+1);
	}
	cursim++;
	aqapp.CancelSensDlg.SetSim(numsim,cursim);
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CCaSensAn dialog


CCaSensAn::CCaSensAn(CWnd* pParent /*=NULL*/)
	: CDialog(CCaSensAn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaSensAn)
	//}}AFX_DATA_INIT

	created = 0;
}


void CCaSensAn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaSensAn)
	DDX_Control(pDX, IDC_LB_AVAILPAR, m_availparlist);
	DDX_Control(pDX, IDC_LB_AVAILCALC, m_availcalclist);
	DDX_Control(pDX, IDC_LB_ACTPAR, m_actparlist);
	DDX_Control(pDX, IDC_LB_ACTCALC, m_actcalclist);
	DDX_Control(pDX, IDC_BU_NEW, m_new);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_BU_INACTCALC, m_inactcalc);
	DDX_Control(pDX, IDC_BU_INACTPAR, m_inactpar);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	DDX_Control(pDX, IDC_BU_ACTCALC, m_actcalc);
	DDX_Control(pDX, IDC_BU_ACTPAR, m_actpar);
	DDX_Control(pDX, IDC_BU_START, m_start);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaSensAn, CDialog)
	//{{AFX_MSG_MAP(CCaSensAn)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_ACTPAR, OnBuActpar)
	ON_BN_CLICKED(IDC_BU_ACTCALC, OnBuActcalc)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_INACTPAR, OnBuInactpar)
	ON_BN_CLICKED(IDC_BU_INACTCALC, OnBuInactcalc)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_BN_CLICKED(IDC_BU_START, OnBuStart)
	ON_LBN_DBLCLK(IDC_LB_ACTCALC, OnDblclkLbActcalc)
	ON_LBN_SELCHANGE(IDC_LB_ACTCALC, OnSelchangeLbActcalc)
	ON_LBN_DBLCLK(IDC_LB_AVAILCALC, OnDblclkLbAvailcalc)
	ON_LBN_SELCHANGE(IDC_LB_AVAILCALC, OnSelchangeLbAvailcalc)
	ON_LBN_SELCHANGE(IDC_LB_ACTPAR, OnSelchangeLbActpar)
	ON_LBN_SELCHANGE(IDC_LB_AVAILPAR, OnSelchangeLbAvailpar)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	ON_LBN_DBLCLK(IDC_LB_ACTPAR, OnDblclkLbActpar)
	ON_LBN_DBLCLK(IDC_LB_AVAILPAR, OnDblclkLbAvailpar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCaSensAn::UpdateActivation()
{
	int max, num, numact, numavail, j, k;
	int* indices;
	char buffer[1024];

	switch ( m_actparlist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_inactpar.EnableWindow(FALSE);
		break;
	default:
		m_inactpar.EnableWindow(TRUE);
		break;
	}

	max = aqsys.Varlist()->Size();
	indices = new int[max];
	num = m_availparlist.GetSelItems(max,indices);
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_actpar.EnableWindow(FALSE);
		break;
	default:
		k = 0;
		for ( j=0; j<num; j++ )
		{
			if ( m_availparlist.GetText(indices[j],buffer) <= 0 )
			{
				aqapp.ProgErr("Illegal index");
				break;
			}
			if ( aqsys.Varlist()->Get(buffer)->SensActive() == FALSE )
			{
				k++;
			}
		}
		if ( k > 0 )
		{
			m_actpar.EnableWindow(TRUE);
		}
		else
		{
			m_actpar.EnableWindow(FALSE);
		}
		break;
	}
	delete [] indices;

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
		if ( aqsys.Calclist()->Get(indices[0]+1)->SensActive() )
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
			if ( !aqsys.Calclist()->Get(indices[0]+1)->SensActive() )
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

	if ( m_actparlist.GetCount()>0 && m_actcalclist.GetCount()>0 )
	{
		m_start.EnableWindow(TRUE);
	}
	else
	{
		m_start.EnableWindow(FALSE);
	}
	
	return;
}


void CCaSensAn::UpdateDialog()
{
	if ( created == 0 ) return;

	int acttopindex;
	int availtopindex;

	acttopindex = m_actparlist.GetTopIndex();
	availtopindex = m_availparlist.GetTopIndex();
	m_actparlist.ResetContent();
	m_availparlist.ResetContent();
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->SensActive() == TRUE )
		{
			m_actparlist.InsertString(-1,var->Symbol());
		}
		switch ( var->Type() )
		{
		case ConstVar:
			m_availparlist.InsertString(-1,var->Symbol());
			break;
		case RealListVar:
			m_availparlist.InsertString(-1,var->Symbol());
			break;
		default:
			break;
		}
		var = var->Next();
	}
	m_actparlist.SetTopIndex(acttopindex);
	m_availparlist.SetTopIndex(availtopindex);

	acttopindex = m_actcalclist.GetTopIndex();
	availtopindex = m_availcalclist.GetTopIndex();
	m_actcalclist.ResetContent();
	m_availcalclist.ResetContent();
	CALC* calc = aqsys.Calclist()->First();
	while ( calc != 0 )
	{
		if ( calc->SensActive() == TRUE )
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


BOOL CCaSensAn::ReadFields(BOOL notify)
{
	if ( created == 0 ) return FALSE;

	return TRUE;
}
	
/////////////////////////////////////////////////////////////////////////////
// CCaSensAn message handlers

BOOL CCaSensAn::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	lastselact = TRUE;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaSensAn::OnOK() // Close button
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();
	return;
}


void CCaSensAn::OnCancel() // Close button 
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();
	return;
}


void CCaSensAn::OnClose() // Close box
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();

	CDialog::OnClose();
}


void CCaSensAn::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CCaSensAn::OnBuActpar() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_availparlist.GetSelItems(max,indices);
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
			if ( m_availparlist.GetText(indices[j],buffer) <= 0 )
			{
				aqapp.ProgErr("Illegal index");
				break;
			}
			AQVAR* var = aqsys.Varlist()->Get(buffer);
			if ( var == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			var->SensActive(TRUE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaSensAn::OnBuInactpar() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_actparlist.GetSelItems(max,indices);
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
			if ( m_actparlist.GetText(indices[j],buffer) <= 0 )
			{
				aqapp.ProgErr("Illegal index");
				break;
			}
			AQVAR* var = aqsys.Varlist()->Get(buffer);
			if ( var == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			var->SensActive(FALSE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaSensAn::OnDblclkLbActpar() 
{
	OnBuInactpar();
	return;
}


void CCaSensAn::OnDblclkLbAvailpar() 
{
	OnBuActpar();
	return;
}


void CCaSensAn::OnBuNew() 
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


void CCaSensAn::OnBuDuplicate() 
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


void CCaSensAn::OnBuEdit() 
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


void CCaSensAn::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	if ( lastselact == FALSE )
	{
		switch ( MessageBox(aqapp.ini.T(23),         // DelAvailSens?
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


void CCaSensAn::OnBuActcalc() 
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
			if ( aqsys.SensActive(calc,TRUE) == FALSE )
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


void CCaSensAn::OnBuInactcalc() 
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
			aqsys.SensActive(calc,FALSE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaSensAn::OnSelchangeLbActpar() 
{
	UpdateActivation();
	return;
}


void CCaSensAn::OnSelchangeLbAvailpar() 
{
	UpdateActivation();
	return;
}


void CCaSensAn::OnDblclkLbActcalc() 
{
	lastselact = TRUE;
    OnBuEdit();
	return;
}


void CCaSensAn::OnDblclkLbAvailcalc() 
{
	lastselact = FALSE;
    OnBuEdit();
	return;
}


void CCaSensAn::OnSelchangeLbActcalc() 
{
	lastselact = TRUE;
	UpdateActivation();
	return;
}


void CCaSensAn::OnSelchangeLbAvailcalc() 
{
	lastselact = FALSE;
	UpdateActivation();
	return;
}


void CCaSensAn::OnBuStart() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	if ( ReadFields(TRUE) == FALSE ) return;

	numpar = 0;
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->SensActive() == TRUE ) numpar++;
		var = var->Next();
	}
	if ( numpar == 0 )
	{
		MessageBox(aqapp.ini.T(283),      // NoActPar
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		return;
	}

	CARDINAL numcalc = 0;
	CALC* calc = aqsys.Calclist()->First();
	while ( calc != 0 )
	{
		if ( calc->SensActive() == TRUE ) numcalc++;
		calc = calc->Next();
	}
	if ( numcalc == 0 )
	{
		MessageBox(aqapp.ini.T(284),      // NoActCalc
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		return;
	}

	char filter[260];
	char filename[260];
	strcpy(filename,aqapp.sensfilename);
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

	strcpy(filter,"AQUASIM Sensitivity Files0*.sen0");
	filter[25] = '\0'; filter[31] = '\0';
	filetitle[0] = '\0';
	strcpy(title,"Select AQUASIM sensitivity ranking file name:");
	dirname[0] = '\0';
	strcpy(fileextension,"sen");
 
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

	std::ofstream sensfile;
	cursim = 0;
	aqsys.SetCalcOut(ContinueSens);
	aqsys.SetSensOut(SensOut);
	aqapp.CancelSensDlg.Display("...calculating sensitivity analysis..."); // Calculating
	JOBSTATUS status = PROBLEM;
	if ( GetSaveFileName(&filenamestruct) )
	{
		sensfile.open(filename,std::ios::out);
        SetFileType(filename,FTsen,FCsen);
		status = aqsys.SensAnal(TRUE);
        if ( status == OK )
        {
		   status = aqsys.SensRank(TRUE,&sensfile);
/*
// batch program test:
ofstream sensrankfile;
ofstream senslistfile;
ifstream senscmdfile;
sensrankfile.open("test.ran",ios::out);
senslistfile.open("test.sen",ios::out);
senscmdfile.open("test.cmd",ios::in);
status = aqsys.SensRank(TRUE,&sensrankfile,&senslistfile,&senscmdfile);
sensrankfile.close();
senslistfile.close();
senscmdfile.close();
*/
        }
		sensfile.close();
	}
	else
	{
		status = aqsys.SensAnal(TRUE);
	}
	aqapp.CancelSensDlg.Remove();
	aqsys.SetCalcOut(0);
	aqsys.SetSensOut(0);
	aqapp.DelStatesDlg.UpdateDialog();
	Beep(4000,100);
	
	aqapp.MainFrame->RefreshFrameTitle();
	switch ( status )
	{
	case OK:
		strcpy(aqapp.sensfilename,filename);
		MessageBox(aqapp.ini.T(285),    // SensSuccess
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case INTERRUPT:
		MessageBox(aqapp.ini.T(286),    // SensInterrupt
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case PROBLEM:
		MessageBox(aqapp.ini.T(287),    // SensFail
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		break;
	}

	UpdateDialog();
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CCaSensCalc dialog


CCaSensCalc::CCaSensCalc(CWnd* pParent /*=NULL*/)
	: CDialog(CCaSensCalc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaSensCalc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCaSensCalc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaSensCalc)
	DDX_Control(pDX, IDC_BU_REPLACE, m_bu_replace);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	DDX_Control(pDX, IDC_LB_LIST, m_lb_list);
	DDX_Control(pDX, IDC_RB_STEADY, m_rb_steady);
	DDX_Control(pDX, IDC_RB_GIVEN, m_rb_given);
	DDX_Control(pDX, IDC_ED_STEPSIZE, m_ed_stepsize);
	DDX_Control(pDX, IDC_ED_NUMSTEPS, m_ed_numstep);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_INITTIME, m_ed_inittime);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_CALCNUM, m_ed_calcnum);
	DDX_Control(pDX, IDC_CB_SIMSTATUS, m_cb_simstatus);
	DDX_Control(pDX, IDC_CB_SENSSTATUS, m_cb_sensstatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaSensCalc, CDialog)
	//{{AFX_MSG_MAP(CCaSensCalc)
	ON_BN_CLICKED(IDC_RB_GIVEN, OnRbGiven)
	ON_BN_CLICKED(IDC_RB_STEADY, OnRbSteady)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_REPLACE, OnBuReplace)
	ON_LBN_SELCHANGE(IDC_LB_LIST, OnSelchangeLbList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CCaSensCalc::DoModal(CALC* c, BOOL a)
{
   oldcalc = c;
   add     = a;
   return CDialog::DoModal();
}


void CCaSensCalc::UpdateDialog()
{
	char buffer[1024], buffer1[1024];
	char dist[2]; dist[0] = ' '; dist[1] = '\0';

	int topindex = m_lb_list.GetTopIndex();
	m_lb_list.ResetContent();
	for ( CARDINAL i=0; i<newcalc->NumStepSeries(); i++ )
	{
		NumToStr(newcalc->StepSize(i),buffer,sizeof(buffer),TRUE,30);
		strcat(buffer,dist);
		NumToStr(newcalc->NumSteps(i),buffer1,sizeof(buffer1),TRUE,30);
		strcat(buffer,buffer1);
		m_lb_list.InsertString(-1,buffer);
	}
	m_lb_list.SetTopIndex(topindex);
	
	UpdateActivation();
	return;
}


void CCaSensCalc::UpdateActivation()
{
	switch ( m_lb_list.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_delete.EnableWindow(FALSE);
		m_bu_replace.EnableWindow(FALSE);
		break;
	case 1:
		m_bu_delete.EnableWindow(TRUE);
		m_bu_replace.EnableWindow(TRUE);
		break;
	default:
		m_bu_delete.EnableWindow(TRUE);
		m_bu_replace.EnableWindow(FALSE);
		break;
	}

	return;
}


/////////////////////////////////////////////////////////////////////////////
// CCaSensCalc message handlers

BOOL CCaSensCalc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	newcalc = new CALC(oldcalc);

	if ( newcalc->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(newcalc->Symbol());
	}
	if ( newcalc->Description() != 0 )
	{
		m_ed_descript.SetWindowText(newcalc->Description());
	}

	NumToString(newcalc->CalcNum(),buffer,sizeof(buffer));
	m_ed_calcnum.SetWindowText(buffer);

	NumToString(newcalc->InitTime(),buffer,sizeof(buffer));
	m_ed_inittime.SetWindowText(buffer);

	if ( newcalc->InitSteady() == TRUE )
	{
		m_rb_given.SetCheck(0);
		m_rb_steady.SetCheck(1);
	}
	else
	{
		m_rb_given.SetCheck(1);
		m_rb_steady.SetCheck(0);
	}

	UpdateDialog();

	if ( newcalc->SimActive() == TRUE )
	{
		m_cb_simstatus.SetCheck(1);
	}
	else
	{
		m_cb_simstatus.SetCheck(0);
	}
	if ( newcalc->SensActive() == TRUE )
	{
		m_cb_sensstatus.SetCheck(1);
	}
	else
	{
		m_cb_sensstatus.SetCheck(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaSensCalc::OnRbGiven() 
{
	newcalc->InitSteady(FALSE);
	return;
}


void CCaSensCalc::OnRbSteady() 
{
	newcalc->InitSteady(TRUE);
	return;
}


void CCaSensCalc::OnCancel() 
{
	delete newcalc;
	
	CDialog::OnCancel();
}


void CCaSensCalc::OnOK() 
{
	char buffer[1024];

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( newcalc->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(202),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	newcalc->Description(buffer);

	CARDINAL calcnum;
	m_ed_calcnum.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&calcnum) == FALSE )
	{
		MessageBox(aqapp.ini.T(267),     // FailCalcNum
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		newcalc->CalcNum(calcnum);
	}

	REAL inittime;
	m_ed_inittime.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&inittime) == FALSE )
	{
		MessageBox(aqapp.ini.T(268),     // FailInitTime
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		newcalc->InitTime(inittime);
	}

	if ( m_cb_simstatus.GetCheck() == TRUE )
	{
		newcalc->SimActive(TRUE);
	}
	else
	{
		newcalc->SimActive(FALSE);
	}

	if ( m_cb_sensstatus.GetCheck() == TRUE )
	{
		newcalc->SensActive(TRUE);
	}
	else
	{
		newcalc->SensActive(FALSE);
	}

	REAL stepsize;
	m_ed_stepsize.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&stepsize) == TRUE )
	{
		switch ( MessageBox(aqapp.ini.T(617),  // AddSteps?
					        aqapp.ini.T(111),
							MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			return;
		default:
			break;
		}
	}
	if ( add == TRUE )
	{
		if ( aqsys.AddCalc(newcalc) == FALSE )
		{
			MessageBox(aqapp.ini.T(264),
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceCalc(oldcalc,newcalc) == FALSE )
		{
			MessageBox(aqapp.ini.T(265),
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


void CCaSensCalc::OnSelchangeLbList() 
{
	char buffer[1024];

	int max = newcalc->NumStepSeries();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);

	if ( num == 1 )
	{
		int i = indices[0];
		if ( i>=0 && i<newcalc->NumStepSeries() )
		{
			NumToString(newcalc->StepSize(i),buffer,sizeof(buffer));
			m_ed_stepsize.SetWindowText(buffer);
			NumToString(newcalc->NumSteps(i),buffer,sizeof(buffer));
			m_ed_numstep.SetWindowText(buffer);
		}
	}
	delete [] indices;
	UpdateActivation();
	return;
}


void CCaSensCalc::OnBuAdd() 
{
	int max = newcalc->NumStepSeries();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);
	int pos = max;
	if ( num > 0 )
	{
		pos = indices[0];
	}
	delete indices;

	char buffer[1024];
	REAL stepsize; CARDINAL numsteps;
	int topindex = m_lb_list.GetTopIndex();
	m_ed_stepsize.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&stepsize) == FALSE )
	{
		MessageBox("Please specify the Step Size in the edit field below the list box",  // 
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_numstep.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&numsteps) == FALSE )
	{
		MessageBox("Please specify the Number of Steps in the edit field below the list box",  // 
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	if ( newcalc->AddStepSeries(stepsize,numsteps,pos) == TRUE )
	{
		m_ed_stepsize.SetWindowText("");
		m_ed_numstep.SetWindowText("");
		UpdateDialog();
		m_lb_list.SetTopIndex(topindex+1);
	}
	else
	{
		MessageBox("Unable to add step series",
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
	}

	return;
}


void CCaSensCalc::OnBuReplace() 
{
	char buffer[1024];

	int max = newcalc->NumStepSeries();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);
	int pos = max;
	if ( num > 0 )
	{
		pos = indices[0];
	}
	delete indices;

	REAL stepsize; CARDINAL numsteps;
	int topindex = m_lb_list.GetTopIndex();
	if ( num == 1 )
	{
		m_ed_stepsize.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&stepsize) == FALSE )
		{
			MessageBox("Please specify the Step Size in the edit field below the list box",  // 
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
		m_ed_numstep.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&numsteps) == FALSE )
		{
			MessageBox("Please specify the Number of Steps in the edit field below the list box",  // 
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
		if ( newcalc->ReplaceStepSeries(stepsize,numsteps,pos) == TRUE )
		{
			m_ed_stepsize.SetWindowText("");
			m_ed_numstep.SetWindowText("");
			UpdateDialog();
			m_lb_list.SetTopIndex(topindex);
		}
		else
		{
			MessageBox("Unable to replace step series",
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
		}
	}
	return;
}


void CCaSensCalc::OnBuDelete() 
{
	int max = newcalc->NumStepSeries();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);
	int j,k;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("Illegal list box selection");
		break;
	default:
		k = 0;
		for ( j=0; j<num; j++ )
		{
			if ( newcalc->DelStepSeries(indices[j]-k) == TRUE )
			{
				k++;
			}
		}
		m_ed_stepsize.SetWindowText(" ");
		m_ed_numstep.SetWindowText(" ");
		break;
	}
	delete [] indices;
	UpdateDialog();
	return;
}



