// edfilm_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edfilm_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdFilm dialog


CEdFilm::CEdFilm(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilm)
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_RB_HIGHRES, m_rb_highres);
	DDX_Control(pDX, IDC_TX_VOLREACT, m_tx_volreact);
	DDX_Control(pDX, IDC_TX_VOLBULK, m_tx_volbulk);
	DDX_Control(pDX, IDC_RB_UNCONFINED, m_rb_unconfined);
	DDX_Control(pDX, IDC_RB_POREPART, m_rb_porepart);
	DDX_Control(pDX, IDC_RB_PORELIQ, m_rb_poreliq);
	DDX_Control(pDX, IDC_RB_MATRIXRIGID, m_rb_matrixrigid);
	DDX_Control(pDX, IDC_RB_MATRIXDIFF, m_rb_matrixdiff);
	DDX_Control(pDX, IDC_RB_LOWRES, m_rb_lowres);
	DDX_Control(pDX, IDC_RB_DETACHINDI, m_rb_detachindi);
	DDX_Control(pDX, IDC_RB_DETACHGLOB, m_rb_detachglob);
	DDX_Control(pDX, IDC_RB_CONFINED, m_rb_confined);
	DDX_Control(pDX, IDC_ED_VOLREACT, m_ed_volreact);
	DDX_Control(pDX, IDC_ED_VOLBULK, m_ed_volbulk);
	DDX_Control(pDX, IDC_ED_RATEPOR, m_ed_ratepor);
	DDX_Control(pDX, IDC_ED_NUMGRID, m_ed_numgrid);
	DDX_Control(pDX, IDC_ED_DETACH, m_ed_detach);
	DDX_Control(pDX, IDC_BU_DISSVARS, m_bu_dissvars);
	DDX_Control(pDX, IDC_ED_AREA, m_ed_area);
	DDX_Control(pDX, IDC_BU_PARTVARS, m_bu_partvars);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	DDX_Control(pDX, IDC_BU_VARS, m_bu_vars);
	DDX_Control(pDX, IDC_BU_PROCS, m_bu_procs);
	DDX_Control(pDX, IDC_BU_INP, m_bu_inp);
	DDX_Control(pDX, IDC_BU_INITS, m_bu_inits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilm, CDialog)
	//{{AFX_MSG_MAP(CEdFilm)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_BU_PARTVARS, OnBuPartvars)
	ON_BN_CLICKED(IDC_BU_DISSVARS, OnBuDissvars)
	ON_BN_CLICKED(IDC_RB_CONFINED, OnRbConfined)
	ON_BN_CLICKED(IDC_RB_UNCONFINED, OnRbUnconfined)
	ON_BN_CLICKED(IDC_RB_PORELIQ, OnRbPoreliq)
	ON_BN_CLICKED(IDC_RB_POREPART, OnRbPorepart)
	ON_BN_CLICKED(IDC_RB_MATRIXRIGID, OnRbMatrixrigid)
	ON_BN_CLICKED(IDC_RB_MATRIXDIFF, OnRbMatrixdiff)
	ON_BN_CLICKED(IDC_RB_DETACHINDI, OnRbDetachindi)
	ON_BN_CLICKED(IDC_RB_DETACHGLOB, OnRbDetachglob)
	ON_BN_CLICKED(IDC_RB_LOWRES, OnRbLowres)
	ON_BN_CLICKED(IDC_RB_HIGHRES, OnRbHighres)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilm::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	fcomp = new FILMCOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilm message handlers

BOOL CEdFilm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( fcomp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(fcomp->Symbol());
	}

	if ( fcomp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(fcomp->Description());
	}

	NumToString(fcomp->CmpIndex(),buffer,sizeof(buffer));
	m_ed_compind.SetWindowText(buffer);

	NumToString(fcomp->ReactVol(),buffer,sizeof(buffer));
	m_ed_volreact.SetWindowText(buffer);
	NumToString(fcomp->BulkVol(),buffer,sizeof(buffer));
	m_ed_volbulk.SetWindowText(buffer);
	conf = fcomp->Confined();
	if ( conf == TRUE )
	{
		m_rb_confined.SetCheck(1);
		m_tx_volbulk.EnableWindow(FALSE);
		m_ed_volbulk.EnableWindow(FALSE);
	}
	else
	{
		m_rb_unconfined.SetCheck(1);
		m_tx_volreact.EnableWindow(FALSE);
		m_ed_volreact.EnableWindow(FALSE);
	}

	if ( fcomp->PoreParticles() == TRUE )
	{
		m_rb_porepart.SetCheck(1);
	}
	else
	{
		m_rb_poreliq.SetCheck(1);
	}

	matrixdiff = fcomp->MatrixDiff();
	if ( matrixdiff == TRUE )
	{
		m_rb_matrixdiff.SetCheck(1);
	}
	else
	{
		m_rb_matrixrigid.SetCheck(1);
		m_rb_detachindi.EnableWindow(FALSE);
	}

	if ( fcomp->Udet() != 0 )
	{
		if ( fcomp->Udet()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_detach.SetWindowText(buffer);
		}
	}
	detachglobal = fcomp->DetachGlobal();
	if ( detachglobal == TRUE )
	{
		m_rb_detachglob.SetCheck(1);
	}
	else
	{
		m_rb_detachindi.SetCheck(1);
		m_ed_detach.EnableWindow(FALSE);
		m_rb_matrixrigid.EnableWindow(FALSE);
	}

	if ( fcomp->Surface() != 0 )
	{
		if ( fcomp->Surface()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_area.SetWindowText(buffer);
		}
	}

		if ( fcomp->RateEps() != 0 )
	{
		if ( fcomp->RateEps()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_ratepor.SetWindowText(buffer);
		}
	}

	NumToString(fcomp->NumGridPts(),buffer,sizeof(buffer));
	m_ed_numgrid.SetWindowText(buffer);

	if ( fcomp->HighRes() == TRUE )
	{
		m_rb_highres.SetCheck(1);
	}
	else
	{
		m_rb_lowres.SetCheck(1);
	}

	if ( fcomp->Active() == TRUE )
	{
		m_cb_active.SetCheck(1);
	}
	else
	{
		m_cb_active.SetCheck(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilm::OnBuVars() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdCompActVar EdCompActVarDlg;
	if ( EdCompActVarDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdFilm::OnBuProcs() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdCompActProc EdCompActProcDlg;
	if ( EdCompActProcDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdFilm::OnBuInits() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdFilmInits EdFilmInitsDlg;
	if ( EdFilmInitsDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdFilm::OnBuInp() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdFilmInps EdFilmInpsDlg;
	if ( EdFilmInpsDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdFilm::OnBuPartvars() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdFilmParts EdFilmPartsDlg;
	if ( EdFilmPartsDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdFilm::OnBuDissvars() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdFilmDisss EdFilmDisssDlg;
	if ( EdFilmDisssDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdFilm::OnRbConfined() 
{
	conf = TRUE;
	m_tx_volreact.EnableWindow(TRUE);
	m_ed_volreact.EnableWindow(TRUE);
	m_tx_volbulk.EnableWindow(FALSE);
	m_ed_volbulk.EnableWindow(FALSE);
	return;
}


void CEdFilm::OnRbUnconfined() 
{
	conf = FALSE;
	m_tx_volbulk.EnableWindow(TRUE);
	m_ed_volbulk.EnableWindow(TRUE);
	m_tx_volreact.EnableWindow(FALSE);
	m_ed_volreact.EnableWindow(FALSE);
	return;
}


void CEdFilm::OnRbPoreliq() 
{
	fcomp->PoreParticles(FALSE);
	return;
}


void CEdFilm::OnRbPorepart() 
{
	fcomp->PoreParticles(TRUE);
	return;
}


void CEdFilm::OnRbMatrixrigid() 
{
	if ( detachglobal == FALSE )
	{
		m_rb_matrixdiff.SetCheck(1);
		return;
	}
	matrixdiff = FALSE;
	m_rb_detachindi.EnableWindow(FALSE);
	return;
}


void CEdFilm::OnRbMatrixdiff() 
{
	matrixdiff = TRUE;
	m_rb_detachindi.EnableWindow(TRUE);
	return;
}


void CEdFilm::OnRbDetachindi() 
{
	if ( matrixdiff == FALSE )
	{
		m_rb_detachglob.SetCheck(1);
		return;
	}
	detachglobal = FALSE;
	m_rb_matrixrigid.EnableWindow(FALSE);
	m_ed_detach.EnableWindow(FALSE);
	return;
}


void CEdFilm::OnRbDetachglob() 
{
	detachglobal = TRUE;
	m_rb_matrixrigid.EnableWindow(TRUE);
	m_ed_detach.EnableWindow(TRUE);
	return;
}


void CEdFilm::OnRbLowres() 
{
	fcomp->HighRes(FALSE);
	return;
}


void CEdFilm::OnRbHighres() 
{
	fcomp->HighRes(TRUE);
	return;
}


void CEdFilm::OnCancel() 
{
	delete fcomp;
	
	CDialog::OnCancel();
}


void CEdFilm::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( fcomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(361),   // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	fcomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(362), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	fcomp->CmpIndex(c);

	if ( conf == TRUE )
	{
		fcomp->Confined(TRUE);
		m_ed_volreact.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == FALSE )
		{
			MessageBox(aqapp.ini.T(363), // IllegalReactVol
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( fcomp->ReactVol(r) == FALSE )
		{
			MessageBox(aqapp.ini.T(364), // FailReactVol
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		m_ed_volbulk.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			fcomp->BulkVol(r);
		}
	}
	else
	{
		fcomp->Confined(FALSE);
		m_ed_volbulk.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == FALSE )
		{
			MessageBox(aqapp.ini.T(365), // IllegalBulkVol
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( fcomp->BulkVol(r) == FALSE )
		{
			MessageBox(aqapp.ini.T(366), // FailBulkVol
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		m_ed_volreact.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			fcomp->ReactVol(r);
		}
	}

	m_ed_detach.GetWindowText(buffer,sizeof(buffer));
	if ( fcomp->Udet(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( detachglobal == TRUE )
		{
			if ( numerr > 0 )
			{
				MessageBox(aqapp.ini.T(367), // ParseErrUdet
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			else
			{
				MessageBox(aqapp.ini.T(368), // IllegalUdet
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( detachglobal == TRUE )
	{
		if ( fcomp->DetachGlobal(TRUE) == FALSE )
		{
			aqapp.ProgErr("Unable to set detachment type");
		}
		if ( fcomp->MatrixDiff(matrixdiff) == FALSE )
		{
			aqapp.ProgErr("Unable to set matrix type");
		}
	}
	else
	{
		if ( fcomp->MatrixDiff(matrixdiff) == FALSE )
		{
			aqapp.ProgErr("Unable to set matrix type");
		}
		if ( fcomp->DetachGlobal(FALSE) == FALSE )
		{
			aqapp.ProgErr("Unable to set detachment type");
		}
	}

	m_ed_area.GetWindowText(buffer,sizeof(buffer));
	if ( fcomp->Surface(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(369), // ParseErrArea
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(370), // IllegalArea
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_ratepor.GetWindowText(buffer,sizeof(buffer));
	if ( fcomp->RateEps(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(371), // ParseErrRatePor
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(372), // IllegalRatePor
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_numgrid.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(373), // FailNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( fcomp->NumGridPts(c) == FALSE )
	{
		MessageBox(aqapp.ini.T(374), // IllegalNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_active.GetCheck() ) fcomp->Active(TRUE);
    else                          fcomp->Active(FALSE);

	if ( add == TRUE )
	{
		if ( aqsys.AddComp(fcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(375), // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,fcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(376),  // IllegalComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdFilm::OnBuAcc() 
{
	FILMCOMP* tempcomp = new FILMCOMP(fcomp);
	CEdFilmAcc EdFilmAccDlg;
	if ( EdFilmAccDlg.DoModal(tempcomp) == IDOK )
	{
		delete fcomp;
		fcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInits dialog


CEdFilmInits::CEdFilmInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilmInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmInits, CDialog)
	//{{AFX_MSG_MAP(CEdFilmInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmInits::DoModal(FILMCOMP* com)
{
	fcomp = com;
	return CDialog::DoModal();
}


void CEdFilmInits::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inits.GetTopIndex();
	m_lb_inits.ResetContent();
	for ( CARDINAL i=0; i<fcomp->NumInitCond(); i++ )
	{
		strcpy(buffer,fcomp->InitVar(i)->Symbol());
		strcat(buffer,"(");
		strcat(buffer,fcomp->ZoneName(fcomp->InitZone(i)));
		strcat(buffer,") : ");
		fcomp->InitVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inits.InsertString(-1,buffer);
	}
	m_lb_inits.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdFilmInits::UpdateActivation()
{
	switch ( m_lb_inits.GetSelCount() )
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


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInits message handlers

BOOL CEdFilmInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilmInits::OnBuAdd() 
{
	int max = fcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdFilmInit EdFilmInitDlg;
	EdFilmInitDlg.DoModal(fcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdFilmInits::OnBuEdit() 
{
	int max = fcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdFilmInit EdFilmInitDlg;
	EdFilmInitDlg.DoModal(fcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdFilmInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(377),   // DelInits?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = fcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
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
			if ( fcomp->DeleteInitCond(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(378),   // FailDelIni
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


void CEdFilmInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;
}


void CEdFilmInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInit dialog


CEdFilmInit::CEdFilmInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilmInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmInit, CDialog)
	//{{AFX_MSG_MAP(CEdFilmInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmInit::DoModal(FILMCOMP* com, BOOL a, int i)
{
	fcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInit message handlers

BOOL CEdFilmInit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=fcomp->NumInitCond())
	{
		aqapp.ProgErr("Illegal Initial Condition");
	}

	char buffer[1024];
	m_le_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_var.AddString(var->Symbol());
			var = var->Next();
		}
	}
	m_le_zone.ResetContent();
	for ( CARDINAL i=0; i<fcomp->NumZones(); i++ )
	{
		m_le_zone.AddString(fcomp->ZoneName(i));
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
		m_le_zone.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(fcomp->InitVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		m_le_zone.SetCurSel(fcomp->InitZone(pos));
		if ( fcomp->InitVal(pos) != 0 )
		{
			fcomp->InitVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_initcond.SetWindowText(buffer);
		}
	}
	
	if ( fcomp->NumZones() == 1 )
	{
		m_tx_zone.EnableWindow(FALSE);
		m_le_zone.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilmInit::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	AQVAR* var;
	int  i;
	CARDINAL parseerrors;

	i = m_le_var.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(72), // IllegalVar
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		var = aqsys.Varlist()->Get(i+1);
		if ( var == 0 )
		{
			aqapp.ProgErr("Variable not found");
			return;
		}
	}
	i = m_le_zone.GetCurSel();
	m_ed_initcond.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( fcomp->AddInitCond(aqsys.Varlist(),i,var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(379),  // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(380),  // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( fcomp->ReplaceInitCond(aqsys.Varlist(),i,var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(379),   // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(382),   // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInps dialog


CEdFilmInps::CEdFilmInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilmInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmInps, CDialog)
	//{{AFX_MSG_MAP(CEdFilmInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmInps::DoModal(FILMCOMP* com)
{
	fcomp = com;
	return CDialog::DoModal();
}


void CEdFilmInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<fcomp->NumInput(); i++ )
	{
		strcpy(buffer,fcomp->InputVar(i)->Symbol());
		strcat(buffer," : ");
		fcomp->InputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdFilmInps::UpdateActivation()
{
	switch ( m_lb_inputs.GetSelCount() )
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


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInps message handlers

BOOL CEdFilmInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( fcomp->Inflow() != 0 )
	{
		if ( fcomp->Inflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilmInps::OnBuAdd() 
{
	int max = fcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdFilmInp EdFilmInpDlg;
	EdFilmInpDlg.DoModal(fcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdFilmInps::OnBuEdit() 
{
	int max = fcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdFilmInp EdFilmInpDlg;
	EdFilmInpDlg.DoModal(fcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdFilmInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(383),    // DelInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = fcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
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
			if ( fcomp->DeleteInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(384),  // FailDelInp
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


void CEdFilmInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdFilmInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdFilmInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( fcomp->Inflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(385), // IllegalInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInp dialog


CEdFilmInp::CEdFilmInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilmInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmInp, CDialog)
	//{{AFX_MSG_MAP(CEdFilmInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmInp::DoModal(FILMCOMP* com, BOOL a, int i)
{
	fcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInp message handlers

BOOL CEdFilmInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=fcomp->NumInput())
	{
		aqapp.ProgErr("Illegal Input");
	}

	char buffer[1024];
	m_le_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_var.AddString(var->Symbol());
			var = var->Next();
		}
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(fcomp->InputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( fcomp->InputFlux(pos) != 0 )
		{
			fcomp->InputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdFilmInp::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	AQVAR* var;
	int  i;
	CARDINAL parseerrors;

	i = m_le_var.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(72), // IllegalVar
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		var = aqsys.Varlist()->Get(i+1);
		if ( var == 0 )
		{
			aqapp.ProgErr("Variable not found");
			return;
		}
	}
	m_ed_inpflux.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( fcomp->AddInput(aqsys.Varlist(),var,
					 buffer1,buffer2,parseerrors,
							 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(386),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(387),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( fcomp->ReplaceInput(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(386),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(388),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmParts dialog


CEdFilmParts::CEdFilmParts(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmParts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmParts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilmParts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmParts)
	DDX_Control(pDX, IDC_LB_VARS, m_lb_vars);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmParts, CDialog)
	//{{AFX_MSG_MAP(CEdFilmParts)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_VARS, OnSelchangeLbVars)
	ON_LBN_DBLCLK(IDC_LB_VARS, OnDblclkLbVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmParts::DoModal(FILMCOMP* com)
{
	fcomp = com;
	return CDialog::DoModal();
}


void CEdFilmParts::UpdateDialog()
{
	int topindex = m_lb_vars.GetTopIndex();
	m_lb_vars.ResetContent();
	for ( CARDINAL i=0; i<fcomp->NX(); i++ )
	{
		m_lb_vars.InsertString(-1,fcomp->Part(i)->Symbol());
	}
	m_lb_vars.SetTopIndex(topindex);
	UpdateActivation();
	return;
}
	
	
void CEdFilmParts::UpdateActivation()
{
	switch ( m_lb_vars.GetSelCount() )
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


/////////////////////////////////////////////////////////////////////////////
// CEdFilmParts message handlers

BOOL CEdFilmParts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilmParts::OnBuAdd() 
{
	int max = fcomp->NX();
	int* indices = new int[max];
	int num = m_lb_vars.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdFilmPart EdFilmPartDlg;
	EdFilmPartDlg.DoModal(fcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdFilmParts::OnBuEdit() 
{
	int max = fcomp->NX();
	int* indices = new int[max];
	int num = m_lb_vars.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdFilmPart EdFilmPartDlg;
	EdFilmPartDlg.DoModal(fcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdFilmParts::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(381),   // DelParts?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = fcomp->NX();
	int* indices = new int[max];
	int num = m_lb_vars.GetSelItems(max,indices);
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
			if ( fcomp->DelX(fcomp->Part(indices[j]-k)) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(389),  // FailDelPart
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


void CEdFilmParts::OnSelchangeLbVars() 
{
	UpdateActivation();
	return;
}


void CEdFilmParts::OnDblclkLbVars() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmPart dialog


CEdFilmPart::CEdFilmPart(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmPart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmPart)
	//}}AFX_DATA_INIT
}


void CEdFilmPart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmPart)
	DDX_Control(pDX, IDC_TX_VAR, m_tx_var);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_VOLDET, m_ed_voldet);
	DDX_Control(pDX, IDC_ED_VOLATT, m_ed_volatt);
	DDX_Control(pDX, IDC_ED_SURFDET, m_ed_surfdet);
	DDX_Control(pDX, IDC_ED_SURFATT, m_ed_surfatt);
	DDX_Control(pDX, IDC_ED_POREDIFF, m_ed_porediff);
	DDX_Control(pDX, IDC_ED_MATRIXDIFF, m_ed_matrixdiff);
	DDX_Control(pDX, IDC_ED_DENSITY, m_ed_density);
	DDX_Control(pDX, IDC_ED_BOUNDRES, m_ed_boundres);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmPart, CDialog)
	//{{AFX_MSG_MAP(CEdFilmPart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmPart::DoModal(FILMCOMP* com, BOOL a, int i)
{
	fcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmPart message handlers

BOOL CEdFilmPart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	m_le_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_var.AddString(var->Symbol());
			var = var->Next();
		}
	}
	if ( add==FALSE && pos>=fcomp->NX())
	{
		aqapp.ProgErr("Illegal ");
	}
	else
	{
		if ( add == FALSE )
		{
			int varpos = aqsys.Varlist()->Position(fcomp->Part(pos));
			if ( varpos <= 0 )
			{
				aqapp.ProgErr("Variable not found");
			}
			else
			{
				m_le_var.SetCurSel(varpos-1);
			}
			m_le_var.EnableWindow(FALSE);
			m_tx_var.EnableWindow(FALSE);
			if ( fcomp->Rho(pos) != 0 )
			{
				fcomp->Rho(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_density.SetWindowText(buffer);
			}
			if ( fcomp->SurfKat(pos) != 0 )
			{
				fcomp->SurfKat(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_surfatt.SetWindowText(buffer);
			}
			if ( fcomp->SurfKde(pos) != 0 )
			{
				fcomp->SurfKde(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_surfdet.SetWindowText(buffer);
			}
			if ( fcomp->VolKat(pos) != 0 )
			{
				fcomp->VolKat(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_volatt.SetWindowText(buffer);
			}
			if ( fcomp->VolKde(pos) != 0 )
			{
				fcomp->VolKde(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_voldet.SetWindowText(buffer);
			}
			if ( fcomp->ResX(pos) != 0 )
			{
				fcomp->ResX(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_boundres.SetWindowText(buffer);
			}
			if ( fcomp->PoreDiffX(pos) != 0 )
			{
				fcomp->PoreDiffX(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_porediff.SetWindowText(buffer);
			}
			if ( fcomp->MatrixDiffX(pos) != 0 )
			{
				fcomp->MatrixDiffX(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_matrixdiff.SetWindowText(buffer);
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilmPart::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	AQVAR* var;
	int  i;
	CARDINAL parseerrors;

	if ( add == FALSE )
	{
		var = fcomp->Part(pos);
	}
	else
	{
		i = m_le_var.GetCurSel();
		if ( i == CB_ERR )
		{
			MessageBox(aqapp.ini.T(72), // IllegalVar
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			var = aqsys.Varlist()->Get(i+1);
			if ( var == 0 )
			{
				aqapp.ProgErr("Variable not found");
				return;
			}
			if ( fcomp->AddX(var) == FALSE )
			{
				MessageBox(aqapp.ini.T(390),  // UnableAddPart
				           aqapp.ini.T(111),
					       MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	m_ed_density.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->Rho(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(391),  // IllegalDens
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_surfatt.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->SurfKat(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(392),  // IllegalSurfKat
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_surfdet.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->SurfKde(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(393),  // IllegalSurfKde
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_volatt.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->VolKat(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(394),  // IllegalVolKat
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_voldet.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->VolKde(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(395),  // IllegalVolKde
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_boundres.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->ResX(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(396),  // IllegalResX
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_porediff.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->PoreDiffX(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(397),  // IllegalPoreDiffX
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_matrixdiff.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->MatrixDiffX(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(398),  // IllegalMatDiff
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmDisss dialog


CEdFilmDisss::CEdFilmDisss(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmDisss::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmDisss)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdFilmDisss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmDisss)
	DDX_Control(pDX, IDC_LB_VARS, m_lb_vars);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmDisss, CDialog)
	//{{AFX_MSG_MAP(CEdFilmDisss)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_VARS, OnSelchangeLbVars)
	ON_LBN_DBLCLK(IDC_LB_VARS, OnDblclkLbVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmDisss::DoModal(FILMCOMP* com)
{
	fcomp = com;
	return CDialog::DoModal();
}


void CEdFilmDisss::UpdateDialog()
{
	int topindex = m_lb_vars.GetTopIndex();
	m_lb_vars.ResetContent();
	for ( CARDINAL i=0; i<fcomp->NS(); i++ )
	{
		m_lb_vars.InsertString(-1,fcomp->Diss(i)->Symbol());
	}
	m_lb_vars.SetTopIndex(topindex);
	UpdateActivation();
	return;
}
	
	
void CEdFilmDisss::UpdateActivation()
{
	switch ( m_lb_vars.GetSelCount() )
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


/////////////////////////////////////////////////////////////////////////////
// CEdFilmDisss message handlers

BOOL CEdFilmDisss::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdFilmDisss::OnBuAdd() 
{
	int max = fcomp->NS();
	int* indices = new int[max];
	int num = m_lb_vars.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CFilmDiss EdFilmDissDlg;
	EdFilmDissDlg.DoModal(fcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdFilmDisss::OnBuEdit() 
{
	int max = fcomp->NS();
	int* indices = new int[max];
	int num = m_lb_vars.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CFilmDiss EdFilmDissDlg;
	EdFilmDissDlg.DoModal(fcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdFilmDisss::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(399),   // DelDisss?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = fcomp->NS();
	int* indices = new int[max];
	int num = m_lb_vars.GetSelItems(max,indices);
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
			if ( fcomp->DelS(fcomp->Diss(indices[j]-k)) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(400),  // FailDelDiss
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


void CEdFilmDisss::OnSelchangeLbVars() 
{
	UpdateActivation();
	return;
}


void CEdFilmDisss::OnDblclkLbVars() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CFilmDiss dialog


CFilmDiss::CFilmDiss(CWnd* pParent /*=NULL*/)
	: CDialog(CFilmDiss::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilmDiss)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFilmDiss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilmDiss)
	DDX_Control(pDX, IDC_TX_VAR, m_tx_var);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_POREDIFF, m_ed_porediff);
	DDX_Control(pDX, IDC_ED_BOUNDRES, m_ed_boundres);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilmDiss, CDialog)
	//{{AFX_MSG_MAP(CFilmDiss)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CFilmDiss::DoModal(FILMCOMP* com, BOOL a, int i)
{
	fcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CFilmDiss message handlers

BOOL CFilmDiss::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	m_le_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_var.AddString(var->Symbol());
			var = var->Next();
		}
	}
	if ( add==FALSE && pos>=fcomp->NS())
	{
		aqapp.ProgErr("Illegal ");
	}
	else
	{
		if ( add == FALSE )
		{
			int varpos = aqsys.Varlist()->Position(fcomp->Diss(pos));
			if ( varpos <= 0 )
			{
				aqapp.ProgErr("Variable not found");
			}
			else
			{
				m_le_var.SetCurSel(varpos-1);
			}
			m_le_var.EnableWindow(FALSE);
			m_tx_var.EnableWindow(FALSE);
			if ( fcomp->ResS(pos) != 0 )
			{
				fcomp->ResS(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_boundres.SetWindowText(buffer);
			}
			if ( fcomp->PoreDiffS(pos) != 0 )
			{
				fcomp->PoreDiffS(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_porediff.SetWindowText(buffer);
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFilmDiss::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	AQVAR* var;
	int  i;
	CARDINAL parseerrors;

	if ( add == FALSE )
	{
		var = fcomp->Diss(pos);
	}
	else
	{
		i = m_le_var.GetCurSel();
		if ( i == CB_ERR )
		{
			MessageBox(aqapp.ini.T(72), // IllegalVar
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			var = aqsys.Varlist()->Get(i+1);
			if ( var == 0 )
			{
				aqapp.ProgErr("Variable not found");
				return;
			}
			if ( fcomp->AddS(var) == FALSE )
			{
				MessageBox(aqapp.ini.T(401),  // FailAddDiss
				           aqapp.ini.T(111),
					       MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	m_ed_boundres.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->ResS(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(402),  // IllegalResS
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}
	m_ed_porediff.GetWindowText(buffer1,sizeof(buffer1));
	if ( fcomp->PoreDiffS(var,buffer1,aqsys.Varlist(),buffer2,parseerrors) == FALSE )
	{
		MessageBox(aqapp.ini.T(403),  // IllegalPoreDiff
		           aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		if ( add == TRUE ) fcomp->DelX(var);
		return;
	}

	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CEdFilmAcc dialog


CEdFilmAcc::CEdFilmAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdFilmAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdFilmAcc)
	//}}AFX_DATA_INIT
}


void CEdFilmAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdFilmAcc)
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	DDX_Control(pDX, IDC_ED_RELACCZ, m_ed_relaccz);
	DDX_Control(pDX, IDC_ED_RELACCV, m_ed_relaccv);
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_RELACCEPS, m_ed_relacceps);
	DDX_Control(pDX, IDC_ED_ABSACCZ, m_ed_absaccz);
	DDX_Control(pDX, IDC_ED_ABSACCV, m_ed_absaccv);
	DDX_Control(pDX, IDC_ED_ABSACCEPS, m_ed_absacceps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdFilmAcc, CDialog)
	//{{AFX_MSG_MAP(CEdFilmAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdFilmAcc::DoModal(FILMCOMP* com)
{
	fcomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdFilmAcc message handlers

BOOL CEdFilmAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	NumToString(fcomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(fcomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(fcomp->RelAccV(),buffer,sizeof(buffer));
	m_ed_relaccv.SetWindowText(buffer);
	NumToString(fcomp->AbsAccV(),buffer,sizeof(buffer));
	m_ed_absaccv.SetWindowText(buffer);
	
	NumToString(fcomp->RelAccZ(),buffer,sizeof(buffer));
	m_ed_relaccz.SetWindowText(buffer);
	NumToString(fcomp->AbsAccZ(),buffer,sizeof(buffer));
	m_ed_absaccz.SetWindowText(buffer);
	
	NumToString(fcomp->RelAccEPS(),buffer,sizeof(buffer));
	m_ed_relacceps.SetWindowText(buffer);
	NumToString(fcomp->AbsAccEPS(),buffer,sizeof(buffer));
	m_ed_absacceps.SetWindowText(buffer);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdFilmAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(577),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(578),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( fcomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(579),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccv.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(580),  // FailRelAccV
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccv.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(581),  // FailRelAccV
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( fcomp->AccV(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(582),  // FailAccV
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccz.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(583),  // FailRelAccZ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccz.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(584),  // FailRelAccZ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( fcomp->AccZ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(585),  // FailAccZ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relacceps.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(586),  // FailRelAccEPS
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacceps.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(587),  // FailAbsAccEPS
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( fcomp->AccEPS(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(588),  // FailAccEPS
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}

