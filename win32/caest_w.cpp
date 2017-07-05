// caest_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "caest_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static CARDINAL cursim;
static REAL     chiini,chibest,chiend;
static REAL*    oldval;

/////////////////////////////////////////////////////////////////////////////

static BOOLEAN ContinueFit(char*,REAL);
static void    FitOut(REAL chi);


static BOOLEAN ContinueFit(char*,REAL)
{
	aqapp.ProcessEvents();
	if ( aqapp.AskUserInterrupt() ) return FALSE;
	return TRUE;
}


void FitOut(REAL chi)
{
	if ( cursim == 0 )
	{
		chiini  = chi;
		chibest = chi;
		chiend  = chi;
	}
	else
	if ( chi < chibest ) chibest = chi;
	chiend = chi;
	cursim++;
	aqapp.CancelFitDlg.SetChi(chiini,chibest,chi);
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CCaParEst dialog


CCaParEst::CCaParEst(CWnd* pParent /*=NULL*/)
	: CDialog(CCaParEst::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaParEst)
	//}}AFX_DATA_INIT

	created = 0;
}


void CCaParEst::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaParEst)
	DDX_Control(pDX, IDC_ED_MAXIT, m_fititer);
	DDX_Control(pDX, IDC_LB_AVAILPAR, m_availparlist);
	DDX_Control(pDX, IDC_LB_AVAILCALC, m_availcalclist);
	DDX_Control(pDX, IDC_LB_ACTPAR, m_actparlist);
	DDX_Control(pDX, IDC_LB_ACTCALC, m_actcalclist);
	DDX_Control(pDX, IDC_BU_START, m_start);
	DDX_Control(pDX, IDC_BU_NEWFIT, m_new);
	DDX_Control(pDX, IDC_BU_INACTPAR, m_inactpar);
	DDX_Control(pDX, IDC_BU_INACTFIT, m_inactfit);
	DDX_Control(pDX, IDC_BU_DUPLFIT, m_duplicate);
	DDX_Control(pDX, IDC_BU_EDFIT, m_edit);
	DDX_Control(pDX, IDC_BU_DELETEFIT, m_delete);
	DDX_Control(pDX, IDC_BU_ACTPAR, m_actpar);
	DDX_Control(pDX, IDC_BU_ACTFIT, m_actfit);
	DDX_Control(pDX, IDC_RB_SECANT, m_methsecant);
	DDX_Control(pDX, IDC_RB_SIMPLEX, m_methsimplex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaParEst, CDialog)
	//{{AFX_MSG_MAP(CCaParEst)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_ACTFIT, OnBuActfit)
	ON_BN_CLICKED(IDC_BU_ACTPAR, OnBuActpar)
	ON_BN_CLICKED(IDC_BU_DELETEFIT, OnBuDeletefit)
	ON_BN_CLICKED(IDC_BU_DUPLFIT, OnBuDuplfit)
	ON_BN_CLICKED(IDC_BU_EDFIT, OnBuEditfit)
	ON_BN_CLICKED(IDC_BU_INACTFIT, OnBuInactfit)
	ON_BN_CLICKED(IDC_BU_INACTPAR, OnBuInactpar)
	ON_BN_CLICKED(IDC_BU_NEWFIT, OnBuNewfit)
	ON_BN_CLICKED(IDC_BU_START, OnBuStart)
	ON_LBN_DBLCLK(IDC_LB_ACTCALC, OnDblclkLbActcalc)
	ON_LBN_SELCHANGE(IDC_LB_ACTCALC, OnSelchangeLbActcalc)
	ON_LBN_SELCHANGE(IDC_LB_ACTPAR, OnSelchangeLbActpar)
	ON_LBN_DBLCLK(IDC_LB_AVAILCALC, OnDblclkLbAvailcalc)
	ON_LBN_SELCHANGE(IDC_LB_AVAILCALC, OnSelchangeLbAvailcalc)
	ON_LBN_SELCHANGE(IDC_LB_AVAILPAR, OnSelchangeLbAvailpar)
	ON_BN_CLICKED(IDC_RB_SECANT, OnRbSecant)
	ON_BN_CLICKED(IDC_RB_SIMPLEX, OnRbSimplex)
	ON_LBN_DBLCLK(IDC_LB_ACTPAR, OnDblclkLbActpar)
	ON_LBN_DBLCLK(IDC_LB_AVAILPAR, OnDblclkLbAvailpar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCaParEst::UpdateActivation()
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
			AQVAR* var = aqsys.Varlist()->Get(buffer);
			if ( var == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			if ( var->Type() != ConstVar )
			{
				aqapp.ProgErr("Illegal variable type");
				break;
			}
			CONSTVAR* cv = (CONSTVAR*)var;
			if ( cv->FitActive() == FALSE )
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

	max = aqsys.Fitlist()->Size();
	indices = new int[max];
	numavail = m_availcalclist.GetSelItems(max,indices);
	numact   = m_actcalclist.GetSelCount();
	switch ( numavail )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_actfit.EnableWindow(FALSE);
		switch ( numact )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		case 0:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_delete.EnableWindow(FALSE);
			m_inactfit.EnableWindow(FALSE);
			break;
		case 1:
			m_duplicate.EnableWindow(TRUE);
			m_edit.EnableWindow(TRUE);
			m_delete.EnableWindow(TRUE);
			m_inactfit.EnableWindow(TRUE);
			break;
		default:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_delete.EnableWindow(TRUE);
			m_inactfit.EnableWindow(TRUE);
			break;
		}
		break;
	case 1:
		m_duplicate.EnableWindow(TRUE);
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		if ( aqsys.Fitlist()->Get(indices[0]+1)->FitActive() )
		{
			m_actfit.EnableWindow(FALSE);
		}
		else
		{
			m_actfit.EnableWindow(TRUE);
		}
		switch ( numact )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		case 0:
			m_inactfit.EnableWindow(FALSE);
			break;
		default:
			m_inactfit.EnableWindow(TRUE);
			break;
		}
		break;
	default:
		k = 0;
		for ( j=0; j<num; j++ )
		{
			if ( !aqsys.Fitlist()->Get(indices[0]+1)->FitActive() )
			{
				k++;
			}
		}
		if ( k > 0 ) m_actfit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		switch ( numact )
		{
		case LB_ERR:
			aqapp.ProgErr("Illegal list box selection");
			break;
		case 0:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_inactfit.EnableWindow(FALSE);
			break;
		case 1:
			m_duplicate.EnableWindow(TRUE);
			m_edit.EnableWindow(TRUE);
			m_inactfit.EnableWindow(TRUE);
			break;
		default:
			m_duplicate.EnableWindow(FALSE);
			m_edit.EnableWindow(FALSE);
			m_inactfit.EnableWindow(TRUE);
			break;
		}
		break;
	}
	delete [] indices;
	
	if ( m_actcalclist.GetCount()>0 )
	{
		m_start.EnableWindow(TRUE);
	}
	else
	{
		m_start.EnableWindow(FALSE);
	}

	return;
}


void CCaParEst::UpdateDialog()
{
	if ( created == 0 ) return;

	int acttopindex;
	int availtopindex;
	char buffer[1024];

	acttopindex = m_actparlist.GetTopIndex();
	availtopindex = m_availparlist.GetTopIndex();
	m_actparlist.ResetContent();
	m_availparlist.ResetContent();
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->Type() == ConstVar )
		{
			CONSTVAR* cv = (CONSTVAR*)var;
			if ( cv->FitActive() == TRUE )
			{
				m_actparlist.InsertString(-1,cv->Symbol());
			}
			m_availparlist.InsertString(-1,cv->Symbol());
		}
		var = var->Next();
	}
	m_actparlist.SetTopIndex(acttopindex);
	m_availparlist.SetTopIndex(availtopindex);

	acttopindex = m_actcalclist.GetTopIndex();
	availtopindex = m_availcalclist.GetTopIndex();
	m_actcalclist.ResetContent();
	m_availcalclist.ResetContent();
	FIT* fit = aqsys.Fitlist()->First();
	while ( fit != 0 )
	{
		if ( fit->FitActive() == TRUE )
		{
			m_actcalclist.InsertString(-1,fit->Symbol());
		}
		m_availcalclist.InsertString(-1,fit->Symbol());
		fit = fit->Next();
	}
	m_actcalclist.SetTopIndex(acttopindex);
	m_availcalclist.SetTopIndex(availtopindex);

	switch ( aqsys.FitMeth() )
	{
	case Secant:
		m_methsecant.SetCheck(1);
		m_methsimplex.SetCheck(0);
		break;
	case Simplex:
		m_methsecant.SetCheck(0);
		m_methsimplex.SetCheck(1);
		break;
	default:
		aqapp.ProgErr("Illegal fit method");
		break;
	}

	NumToString(aqsys.FitIter(),buffer,sizeof(buffer));
	m_fititer.SetWindowText(buffer);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


BOOL CCaParEst::ReadFields(BOOL notify)
{
	char buffer[1024];

	if ( created == 0 ) return FALSE;

	CARDINAL fititer;
	m_fititer.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fititer) == FALSE )
	{
		if ( notify )
		{
			MessageBox(aqapp.ini.T(22), // FailFitIter
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
			return FALSE;
		}
	}
	else
	{
		aqsys.FitIter(fititer);
	}

	return TRUE;
}
	

/////////////////////////////////////////////////////////////////////////////
// CCaParEst message handlers

BOOL CCaParEst::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	lastselact = TRUE;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaParEst::OnOK() // Close button
{
	ReadFields(FALSE);
	DestroyWindow();
	return;
}


void CCaParEst::OnCancel() // Close button 
{
	ReadFields(FALSE);
	DestroyWindow();
	return;
}


void CCaParEst::OnClose() // Close box
{
	ReadFields(FALSE);
	aqapp.MainFrame->RefreshFrameTitle();
	DestroyWindow();

	CDialog::OnClose();
}


void CCaParEst::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CCaParEst::OnBuActpar() 
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
			if ( var->Type() != ConstVar )
			{
				aqapp.ProgErr("Illegal variable type");
				break;
			}
			CONSTVAR* cv = (CONSTVAR*)var;
			cv->FitActive(TRUE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaParEst::OnBuInactpar() 
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
			if ( var->Type() != ConstVar )
			{
				aqapp.ProgErr("Illegal variable type");
				break;
			}
			CONSTVAR* cv = (CONSTVAR*)var;
			cv->FitActive(FALSE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaParEst::OnDblclkLbActpar() 
{
	OnBuInactpar();
	return;
}


void CCaParEst::OnDblclkLbAvailpar() 
{
	OnBuActpar();
	return;
}


void CCaParEst::OnBuNewfit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CCaParEstCalc CCaParEstCalcDlg;
	CCaParEstCalcDlg.DoModal(0,TRUE);
	UpdateDialog();
	return; 
}


void CCaParEst::OnBuDuplfit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Fitlist()->Size();
	int* indices = new int[max];
	if ( lastselact == TRUE )
	{
		int num = m_actcalclist.GetSelItems(max,indices);
		if ( num == 1 )
		{
			char buffer[1024];
			int res = m_actcalclist.GetText(indices[0],buffer);
			FIT* fit = aqsys.Fitlist()->Get(buffer);
			CCaParEstCalc CCaParEstCalcDlg;
			CCaParEstCalcDlg.DoModal(fit,TRUE);
		}
		else
		{
			num = m_availcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				FIT* fit = aqsys.Fitlist()->Get(indices[0]+1);
				CCaParEstCalc CCaParEstCalcDlg;
				CCaParEstCalcDlg.DoModal(fit,TRUE);
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
			FIT* fit = aqsys.Fitlist()->Get(indices[0]+1);
			CCaParEstCalc CCaParEstCalcDlg;
			CCaParEstCalcDlg.DoModal(fit,TRUE);
		}
		else
		{
			num = m_actcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				char buffer[1024];
				int res = m_actcalclist.GetText(indices[0],buffer);
				FIT* fit = aqsys.Fitlist()->Get(buffer);
				CCaParEstCalc CCaParEstCalcDlg;
				CCaParEstCalcDlg.DoModal(fit,TRUE);
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

	UpdateDialog();
	return;
}


void CCaParEst::OnBuEditfit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Fitlist()->Size();
	int* indices = new int[max];
	if ( lastselact == TRUE )
	{
		int num = m_actcalclist.GetSelItems(max,indices);
		if ( num == 1 )
		{
			char buffer[1024];
			int res = m_actcalclist.GetText(indices[0],buffer);
			FIT* fit = aqsys.Fitlist()->Get(buffer);
			CCaParEstCalc CCaParEstCalcDlg;
			CCaParEstCalcDlg.DoModal(fit,FALSE);
		}
		else
		{
			num = m_availcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				FIT* fit = aqsys.Fitlist()->Get(indices[0]+1);
				CCaParEstCalc CCaParEstCalcDlg;
				CCaParEstCalcDlg.DoModal(fit,FALSE);
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
			FIT* fit = aqsys.Fitlist()->Get(indices[0]+1);
			CCaParEstCalc CCaParEstCalcDlg;
			CCaParEstCalcDlg.DoModal(fit,FALSE);
		}
		else
		{
			num = m_actcalclist.GetSelItems(max,indices);
			if ( num == 1 )
			{
				char buffer[1024];
				int res = m_actcalclist.GetText(indices[0],buffer);
				FIT* fit = aqsys.Fitlist()->Get(buffer);
				CCaParEstCalc CCaParEstCalcDlg;
				CCaParEstCalcDlg.DoModal(fit,FALSE);
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

	UpdateDialog();
	return; 
}


void CCaParEst::OnBuDeletefit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	if ( lastselact == FALSE )
	{
		switch ( MessageBox(aqapp.ini.T(24),         // DelAvailFit?
					        aqapp.ini.T(111),
							MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			break;
		default:
			return;
		}
		int max = aqsys.Fitlist()->Size();
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
				FIT* fit = aqsys.Fitlist()->Get(buffer);
				if ( fit == 0 )
				{
					aqapp.ProgErr("Illegal selection");
					break;
				}
				aqsys.DeleteFit(fit);
			}
			break;
		}
		delete [] indices;
	}
	else
	{
		switch ( MessageBox(aqapp.ini.T(271),        // DelActFit?
					        aqapp.ini.T(111),
							MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			break;
		default:
			return;
		}
		int max = aqsys.Fitlist()->Size();
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
				FIT* fit = aqsys.Fitlist()->Get(buffer);
				if ( fit == 0 )
				{
					aqapp.ProgErr("Illegal selection");
					break;
				}
				aqsys.DeleteFit(fit);
			}
			break;
		}
		delete [] indices;
	}

	UpdateDialog();
	return; 
}


void CCaParEst::OnBuActfit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Fitlist()->Size();
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
			FIT* fit = aqsys.Fitlist()->Get(buffer);
			if ( fit == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			aqsys.FitActive(fit,TRUE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaParEst::OnBuInactfit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Fitlist()->Size();
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
			FIT* fit = aqsys.Fitlist()->Get(buffer);
			if ( fit == 0 )
			{
				aqapp.ProgErr("Illegal selection");
				break;
			}
			aqsys.FitActive(fit,FALSE);
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaParEst::OnSelchangeLbActpar() 
{
	UpdateActivation();
	return;
}


void CCaParEst::OnSelchangeLbAvailpar() 
{
    UpdateActivation();
	return;
}


void CCaParEst::OnDblclkLbActcalc() 
{
    lastselact = TRUE;
	OnBuEditfit();
	return;
}


void CCaParEst::OnDblclkLbAvailcalc() 
{
	lastselact = FALSE;
    OnBuEditfit();
	return;
}


void CCaParEst::OnSelchangeLbActcalc() 
{
	lastselact = TRUE;
    UpdateActivation();
	return;
}


void CCaParEst::OnSelchangeLbAvailcalc() 
{
	lastselact = FALSE;
    UpdateActivation();
	return;
}


void CCaParEst::OnRbSecant() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	aqsys.FitMeth(Secant);
	aqapp.MainFrame->RefreshFrameTitle();
	return; 
}


void CCaParEst::OnRbSimplex() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	aqsys.FitMeth(Simplex);
	aqapp.MainFrame->RefreshFrameTitle();
	return; 
}


void CCaParEst::OnBuStart() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	if ( ReadFields(TRUE) == FALSE ) return;

	CARDINAL numcalc = 0;
	FIT* fit = aqsys.Fitlist()->First();
	while ( fit != 0 )
	{
		if ( fit->FitActive() == TRUE ) numcalc++;
		fit = fit->Next();
	}
	if ( numcalc == 0 )
	{
		MessageBox(aqapp.ini.T(288),    // NoActCalc
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		return;
	}

	CARDINAL numpar = 0;
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->Type() == ConstVar )
		{
			CONSTVAR* cv = (CONSTVAR*)var;
			if ( cv->FitActive() == TRUE )
			{
				numpar++;
			}
		}
		var = var->Next();
	}
/*
	if ( numpar == 0 )
	{
		MessageBox(aqapp.ini.T(289),    // NoActPar
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		return;
	}
*/
	oldval = new REAL[numpar];
	CARDINAL i = 0;
	var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->Type() == ConstVar )
		{
			CONSTVAR* cv = (CONSTVAR*)var;
			if ( cv->FitActive() == TRUE )
			{
				oldval[i] = cv->Value();
				i++;
			}
		}
		var = var->Next();
	}

	char filter[260];
	char filename[260];
	strcpy(filename,aqapp.fitfilename);
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

	strcpy(filter,"AQUASIM Fit Files0*.fit0");
	filter[17] = '\0'; filter[23] = '\0';
	filetitle[0] = '\0';
	strcpy(title,"Select AQUASIM fit file name:");
	dirname[0] = '\0';
	strcpy(fileextension,"fit");
 
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

	std::ofstream fitfile;
	if ( GetSaveFileName(&filenamestruct) )
	{
		fitfile.open(filename,std::ios::out);
        SetFileType(filename,FTfit,FCfit);
	}
	else
	{
		delete [] oldval;
		return;
	}

	cursim = 0;
	CARDINAL numiter = 0;
	aqsys.SetCalcOut(ContinueFit);
	aqsys.SetFitOut(FitOut);
	aqapp.CancelFitDlg.Display("...estimating parameters..."); // Calculating
	JOBSTATUS status = aqsys.Fit(fitfile,&numiter);
	fitfile.close();
	aqapp.CancelFitDlg.Remove();
	aqsys.SetCalcOut(0);
	aqsys.SetFitOut(0);
	aqapp.DelStatesDlg.UpdateDialog();
	Beep(4000,100);
	
	aqapp.MainFrame->RefreshFrameTitle();
	switch ( status )
	{
	case OK:
		{
			strcpy(aqapp.fitfilename,filename);
			CCaParEstRes CCaParEstResDlg;
			CCaParEstResDlg.DoModal(FALSE,numiter);
		}
		break;
	case INTERRUPT:
		MessageBox(aqapp.ini.T(290),    // FitInterrupt
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONEXCLAMATION);
		break;
	case PROBLEM:
		if ( numiter == aqsys.FitIter() )
		{
			CCaParEstRes CCaParEstResDlg;
			CCaParEstResDlg.DoModal(TRUE,numiter);
		}
		else
		{
			MessageBox(aqapp.ini.T(291),    // FitFail
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
		}
		break;
	}
	delete [] oldval;

	UpdateDialog();
	return; 
}

/////////////////////////////////////////////////////////////////////////////
// CCaParEstRes dialog


CCaParEstRes::CCaParEstRes(CWnd* pParent /*=NULL*/)
	: CDialog(CCaParEstRes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaParEstRes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCaParEstRes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaParEstRes)
	DDX_Control(pDX, IDC_TX_STATUS, m_tx_status);
	DDX_Control(pDX, IDC_TX_NUMITER, m_tx_numiter);
	DDX_Control(pDX, IDC_TX_CHIINI, m_tx_chiini);
	DDX_Control(pDX, IDC_TX_CHIEND, m_tx_chiend);
	DDX_Control(pDX, IDC_LB_LIST, m_lb_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaParEstRes, CDialog)
	//{{AFX_MSG_MAP(CCaParEstRes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CCaParEstRes::DoModal(BOOL max, CARDINAL num)
{
   maxit  = max;
   numit  = num;
   return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CCaParEstRes message handlers

BOOL CCaParEstRes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( maxit == FALSE )
	{
		m_tx_status.SetWindowText("convergence criterion met");
	}
	else
	{
		m_tx_status.SetWindowText("maximum number of iterations reached");
	}
	char buffer[1024];
	NumToString(numit,buffer,sizeof(buffer));
    m_tx_numiter.SetWindowText(buffer);

	char buffer1[1024];
	CARDINAL i = 0;
	m_lb_list.ResetContent();
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->Type() == ConstVar )
		{
			CONSTVAR* cv = (CONSTVAR*)var;
			if ( cv->FitActive() == TRUE )
			{
				strcpy(buffer,cv->Symbol());
				strcat(buffer,":  ");
				NumToString(oldval[i],buffer1,sizeof(buffer1));
				strcat(buffer,buffer1);
				strcat(buffer," -> ");
				NumToString(cv->Value(),buffer1,sizeof(buffer1));
				strcat(buffer,buffer1);
				strcat(buffer,"  (");
				NumToString(cv->Minimum(),buffer1,sizeof(buffer1));
				strcat(buffer,buffer1);
				strcat(buffer,",");
				NumToString(cv->Maximum(),buffer1,sizeof(buffer1));
				strcat(buffer,buffer1);
				strcat(buffer,")");
				m_lb_list.InsertString(-1,buffer);
				i++;
			}
		}
		var = var->Next();
	}

	NumToString(chiini,buffer,sizeof(buffer));
    m_tx_chiini.SetWindowText(buffer);
	NumToString(chiend,buffer,sizeof(buffer));
    m_tx_chiend.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CCaParEstCalc dialog


CCaParEstCalc::CCaParEstCalc(CWnd* pParent /*=NULL*/)
	: CDialog(CCaParEstCalc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaParEstCalc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCaParEstCalc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaParEstCalc)
	DDX_Control(pDX, IDC_RB_STEADY, m_rb_steady);
	DDX_Control(pDX, IDC_RB_GIVEN, m_rb_given);
	DDX_Control(pDX, IDC_LB_TARGETS, m_lb_targets);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_INITTIME, m_ed_inittime);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_CALCNUM, m_ed_calcnum);
	DDX_Control(pDX, IDC_CB_STATUS, m_cb_status);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaParEstCalc, CDialog)
	//{{AFX_MSG_MAP(CCaParEstCalc)
	ON_BN_CLICKED(IDC_RB_GIVEN, OnRbGiven)
	ON_BN_CLICKED(IDC_RB_STEADY, OnRbSteady)
	ON_LBN_DBLCLK(IDC_LB_TARGETS, OnDblclkLbTargets)
	ON_LBN_SELCHANGE(IDC_LB_TARGETS, OnSelchangeLbTargets)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCaParEstCalc::UpdateActivation()
{
	switch ( m_lb_targets.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_edit.EnableWindow(FALSE);
		m_bu_delete.EnableWindow(FALSE);
		break;
	case 1:
		m_bu_edit.EnableWindow(TRUE);
		m_bu_delete.EnableWindow(TRUE);
		break;
	default:
		m_bu_edit.EnableWindow(FALSE);
		m_bu_delete.EnableWindow(TRUE);
		break;
	}
	return;
}

	
void CCaParEstCalc::UpdateDialog()
{
	char buffer[1024], buffer1[1024];
	int topindex = m_lb_targets.GetTopIndex();
	m_lb_targets.ResetContent();
	for ( CARDINAL i=0; i<newfit->NumData(); i++ )
	{
		const FITDATA data = newfit->Data(i);
		strcpy(buffer,(data.data)->Symbol());
		strcat(buffer,"  :  ");
		strcat(buffer,(data.var)->Symbol());
		strcat(buffer," (");
		strcat(buffer,(data.com)->Symbol());
		strcat(buffer,", ");
		strcat(buffer,(data.com)->ZoneName(data.zone));
		strcat(buffer,", ");
		NumToString(data.spacetime,buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
        if ( data.spacerel == TRUE )
        {
           strcat(buffer," rel.space");
        }
		strcat(buffer,")");
		m_lb_targets.InsertString(-1,buffer);
	}
	m_lb_targets.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


int CCaParEstCalc::DoModal(FIT* f, BOOL a)
{
   oldfit = f;
   add    = a;
   return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CCaParEstCalc message handlers

BOOL CCaParEstCalc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	newfit = new FIT(oldfit);

	if ( newfit->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(newfit->Symbol());
	}
	if ( newfit->Description() != 0 )
	{
		m_ed_descript.SetWindowText(newfit->Description());
	}

	NumToString(newfit->CalcNum(),buffer,sizeof(buffer));
	m_ed_calcnum.SetWindowText(buffer);

	NumToString(newfit->InitTime(),buffer,sizeof(buffer));
	m_ed_inittime.SetWindowText(buffer);

	if ( newfit->InitSteady() == TRUE )
	{
		m_rb_given.SetCheck(0);
		m_rb_steady.SetCheck(1);
	}
	else
	{
		m_rb_given.SetCheck(1);
		m_rb_steady.SetCheck(0);
	}

	if ( newfit->FitActive() == TRUE )
	{
		m_cb_status.SetCheck(1);
	}
	else
	{
		m_cb_status.SetCheck(0);
	}

	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaParEstCalc::OnRbGiven() 
{
	newfit->InitSteady(FALSE);
	return;
}


void CCaParEstCalc::OnRbSteady() 
{
	newfit->InitSteady(TRUE);
	return;
}


void CCaParEstCalc::OnDblclkLbTargets() 
{
	OnBuEdit();
	return;
}


void CCaParEstCalc::OnSelchangeLbTargets() 
{
	UpdateActivation();
	return;
}


void CCaParEstCalc::OnBuAdd() 
{
	int max = newfit->NumData();
	int* indices = new int[max];
	int num = m_lb_targets.GetSelItems(max,indices);
	CARDINAL pos;
	FITDATA data;
	CCaParEstTarget CCaParEstTargetDlg;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		pos = newfit->NumData();
		CCaParEstTargetDlg.DoModal(newfit,pos,data,TRUE);
		break;
	default:
		pos = indices[0];
		CCaParEstTargetDlg.DoModal(newfit,pos,data,TRUE);
		break;
	}
	delete [] indices;

	UpdateDialog();
	return;
}


void CCaParEstCalc::OnBuEdit() 
{
	int max = newfit->NumData();
	int* indices = new int[max];
	int num = m_lb_targets.GetSelItems(max,indices);
	CARDINAL pos;
	FITDATA data;
	CCaParEstTarget CCaParEstTargetDlg;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 1:
		pos = indices[0];
		if ( pos >= newfit->NumData() )
		{
			aqapp.ProgErr("Illegal list box selection");
			break;
		}
		data = newfit->Data(pos);
		CCaParEstTargetDlg.DoModal(newfit,pos,data,FALSE);
		break;
	default:
		aqapp.ProgErr("Illegal list box selection");
		break;
	}
	delete [] indices;

	UpdateDialog();
	return;
}


void CCaParEstCalc::OnBuDelete() 
{
	int max = newfit->NumData();
	int* indices = new int[max];
	int num = m_lb_targets.GetSelItems(max,indices);
	int j,k;
	FITDATA data;
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
			if ( newfit->Delete(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				aqapp.ProgErr("Unable to delete fit target");
			}
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CCaParEstCalc::OnCancel() 
{
	delete newfit;
	
	CDialog::OnCancel();
}


void CCaParEstCalc::OnOK() 
{
	char buffer[1024];

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( newfit->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(272),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	newfit->Description(buffer);

	CARDINAL calcnum;
	m_ed_calcnum.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&calcnum) == FALSE )
	{
		MessageBox(aqapp.ini.T(273),     // FailCalcNum
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		newfit->CalcNum(calcnum);
	}

	REAL inittime;
	m_ed_inittime.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&inittime) == FALSE )
	{
		MessageBox(aqapp.ini.T(274),     // FailInitTime
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		newfit->InitTime(inittime);
	}

	if ( m_cb_status.GetCheck() == TRUE )
	{
		newfit->FitActive(TRUE);
	}
	else
	{
		newfit->FitActive(FALSE);
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddFit(newfit) == FALSE )
		{
			MessageBox(aqapp.ini.T(275),
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceFit(oldfit,newfit) == FALSE )
		{
			MessageBox(aqapp.ini.T(276),
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CCaParEstTarget dialog


CCaParEstTarget::CCaParEstTarget(CWnd* pParent /*=NULL*/)
	: CDialog(CCaParEstTarget::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaParEstTarget)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCaParEstTarget::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaParEstTarget)
	DDX_Control(pDX, IDC_CB_SPACEREL, m_cb_spacerel);
	DDX_Control(pDX, IDC_TX_VAR, m_tx_var);
	DDX_Control(pDX, IDC_TX_DATA, m_tx_data);
	DDX_Control(pDX, IDC_TX_COMPZONE, m_tx_zone);
	DDX_Control(pDX, IDC_TX_COMP, m_tx_comp);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_LE_DATA, m_le_data);
	DDX_Control(pDX, IDC_LE_COMPZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_COMP, m_le_comp);
	DDX_Control(pDX, IDC_ED_TORX, m_ed_torx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaParEstTarget, CDialog)
	//{{AFX_MSG_MAP(CCaParEstTarget)
	ON_CBN_CLOSEUP(IDC_LE_COMP, OnCloseupLeComp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CCaParEstTarget::DoModal(FIT* f, CARDINAL p, FITDATA d, BOOL a)
{
	fit  = f;
	pos  = p;
	data = d;
	add  = a;
	return CDialog::DoModal();
}


void CCaParEstTarget::UpdateZoneList()
{
	int i = m_le_comp.GetCurSel();
	if ( i == CB_ERR )
	{
		m_le_zone.ResetContent();
		m_le_zone.EnableWindow(FALSE);
		m_tx_zone.EnableWindow(FALSE);
	}
	else
	{
		AQCOMP* com = aqsys.Complist()->Get(i+1);
		if ( com == 0 )
		{
			m_le_zone.ResetContent();
			m_le_zone.EnableWindow(FALSE);
			m_tx_zone.EnableWindow(FALSE);
		}
		else
		{
			m_le_zone.ResetContent();
			for ( CARDINAL j=0; j<com->NumZones(); j++ )
			{
				m_le_zone.AddString(com->ZoneName(j));
			}
			if ( data.zone >= com->NumZones() ) data.zone = 0;
			m_le_zone.SetCurSel(data.zone);
			if ( com->NumZones() > 1 )
			{
				m_le_zone.EnableWindow(TRUE);
				m_tx_zone.EnableWindow(TRUE);
			}
			else
			{
				m_le_zone.EnableWindow(FALSE);
				m_tx_zone.EnableWindow(FALSE);
			}
		}
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CCaParEstTarget message handlers

BOOL CCaParEstTarget::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int cursel = 0;
	CARDINAL i = 0;
	m_le_data.ResetContent();
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( var->Type() == RealListVar )
		{
			if ( data.data == var ) cursel = i;
			m_le_data.InsertString(-1,var->Symbol());
			i++;
		}
		var = var->Next();
	}
	if ( m_le_data.GetCount() < 2 )
	{
		m_tx_data.EnableWindow(FALSE);
		m_le_data.EnableWindow(FALSE);
	}
	m_le_data.SetCurSel(cursel);
	
	cursel = 0;
	i = 0;
	m_le_var.ResetContent();
	var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		if ( data.var == var ) cursel = i;
		m_le_var.InsertString(-1,var->Symbol());
		i++;
		var = var->Next();
	}
	if ( m_le_var.GetCount() < 2 )
	{
		m_tx_var.EnableWindow(FALSE);
		m_le_var.EnableWindow(FALSE);
	}
	m_le_var.SetCurSel(cursel);
	
	cursel = 0;
	i = 0;
	m_le_comp.ResetContent();
	AQCOMP* com = aqsys.Complist()->First();
	while ( com != 0 )
	{
		if ( data.com == com ) cursel = i;
		m_le_comp.InsertString(-1,com->Symbol());
		i++;
		com = com->Next();
	}
	if ( m_le_comp.GetCount() < 2 )
	{
		m_tx_comp.EnableWindow(FALSE);
		m_le_comp.EnableWindow(FALSE);
	}
	m_le_comp.SetCurSel(cursel);
	
	UpdateZoneList();

	char buffer[1024];
	NumToString(data.spacetime,buffer,sizeof(buffer));
	m_ed_torx.SetWindowText(buffer);

    if ( data.spacerel == TRUE ) m_cb_spacerel.SetCheck(1);
	else                         m_cb_spacerel.SetCheck(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCaParEstTarget::OnCancel() 
{
	CDialog::OnCancel();
}


void CCaParEstTarget::OnOK() 
{
	char buffer[1024];

	int index = m_le_data.GetCurSel();
	m_le_data.GetLBText(index,buffer);
	AQVAR* var = aqsys.GetVar(buffer);
	if ( var == 0 )
	{
		MessageBox(aqapp.ini.T(292),    // IllegalData
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	data.data = var;
	
	index = m_le_var.GetCurSel();
	m_le_var.GetLBText(index,buffer);
	var = aqsys.GetVar(buffer);
	if ( var == 0 )
	{
		MessageBox(aqapp.ini.T(293),    // IllegalVar
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	data.var = var;
	
	index = m_le_comp.GetCurSel();
	AQCOMP* com = aqsys.Complist()->Get(index+1);
	if ( com == 0 )
	{
		MessageBox(aqapp.ini.T(294),    // IllegalComp
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	data.com = com;
	
	index = m_le_zone.GetCurSel();
	data.zone = index;

	REAL torx;
	m_ed_torx.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&torx) == FALSE )
	{
		MessageBox(aqapp.ini.T(295),    // UnableReadTorX
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	data.spacetime = torx;

	if ( m_cb_spacerel.GetCheck() ) data.spacerel = TRUE;
	else                            data.spacerel = FALSE;
	
	if ( add == TRUE )
	{
		if ( fit->Add(data,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(296),   // UnableAddTarget
					   aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( fit->Replace(data,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(297),   // IllegalTarget
					   aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CCaParEstTarget::OnCloseupLeComp() 
{
	UpdateZoneList();
	return;
}


