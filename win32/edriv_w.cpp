// edriv_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edriv_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static int inputtype = 0;

static int EditInputs(RIVCOMP* com);

/////////////////////////////////////////////////////////////////////////////
// CEdRiv dialog


CEdRiv::CEdRiv(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRiv::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRiv)
	//}}AFX_DATA_INIT
}


void CEdRiv::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRiv)
	DDX_Control(pDX, IDC_RB_SEDYES, m_rb_sedyes);
	DDX_Control(pDX, IDC_RB_SEDNO, m_rb_sedno);
	DDX_Control(pDX, IDC_BU_SED, m_bu_sed);
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_BU_ACC, m_bu_acc);
	DDX_Control(pDX, IDC_RB_HIGHRES, m_rb_highres);
	DDX_Control(pDX, IDC_RB_WITHOUTDISP, m_rb_withoutdisp);
	DDX_Control(pDX, IDC_TX_ENDLEVEL, m_tx_endlevel);
	DDX_Control(pDX, IDC_RB_WITHDISP, m_rb_withdisp);
	DDX_Control(pDX, IDC_RB_METHKIN, m_rb_methkin);
	DDX_Control(pDX, IDC_RB_METHDIFF, m_rb_methdiff);
	DDX_Control(pDX, IDC_RB_LEVNORM, m_rb_levnorm);
	DDX_Control(pDX, IDC_RB_LEVGIVEN, m_rb_levgiven);
	DDX_Control(pDX, IDC_RB_LOWRES, m_rb_lowres);
	DDX_Control(pDX, IDC_RB_LEVCRIT, m_rb_levcrit);
	DDX_Control(pDX, IDC_ED_WIDTH, m_ed_width);
	DDX_Control(pDX, IDC_ED_START, m_ed_start);
	DDX_Control(pDX, IDC_ED_PERIMETER, m_ed_perimeter);
	DDX_Control(pDX, IDC_ED_NUMGRID, m_ed_numgrid);
	DDX_Control(pDX, IDC_ED_GRAVIT, m_ed_gravit);
	DDX_Control(pDX, IDC_ED_FRICT, m_ed_frict);
	DDX_Control(pDX, IDC_ED_END, m_ed_end);
	DDX_Control(pDX, IDC_ED_ENDLEVEL, m_ed_endlevel);
	DDX_Control(pDX, IDC_ED_DISP, m_ed_disp);
	DDX_Control(pDX, IDC_ED_CROSSSECT, m_ed_crosssect);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	DDX_Control(pDX, IDC_BU_VARS, m_bu_vars);
	DDX_Control(pDX, IDC_BU_PROCS, m_bu_procs);
	DDX_Control(pDX, IDC_BU_INP, m_bu_inp);
	DDX_Control(pDX, IDC_BU_INITS, m_bu_inits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRiv, CDialog)
	//{{AFX_MSG_MAP(CEdRiv)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_RB_HIGHRES, OnRbHighres)
	ON_BN_CLICKED(IDC_RB_LEVCRIT, OnRbLevcrit)
	ON_BN_CLICKED(IDC_RB_LEVGIVEN, OnRbLevgiven)
	ON_BN_CLICKED(IDC_RB_LEVNORM, OnRbLevnorm)
	ON_BN_CLICKED(IDC_RB_LOWRES, OnRbLowres)
	ON_BN_CLICKED(IDC_RB_METHDIFF, OnRbMethdiff)
	ON_BN_CLICKED(IDC_RB_METHKIN, OnRbMethkin)
	ON_BN_CLICKED(IDC_RB_WITHDISP, OnRbWithdisp)
	ON_BN_CLICKED(IDC_RB_WITHOUTDISP, OnRbWithoutdisp)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	ON_BN_CLICKED(IDC_RB_SEDNO, OnRbSedno)
	ON_BN_CLICKED(IDC_RB_SEDYES, OnRbSedyes)
	ON_BN_CLICKED(IDC_BU_SED, OnBuSed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRiv::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	rcomp = new RIVCOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRiv message handlers

BOOL CEdRiv::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( rcomp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(rcomp->Symbol());
	}

	if ( rcomp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(rcomp->Description());
	}

	NumToString(rcomp->CmpIndex(),buffer,sizeof(buffer));
	m_ed_compind.SetWindowText(buffer);

	NumToString(rcomp->Gravit(),buffer,sizeof(buffer));
	m_ed_gravit.SetWindowText(buffer);

	NumToString(rcomp->XStart(),buffer,sizeof(buffer));
	m_ed_start.SetWindowText(buffer);

	NumToString(rcomp->XEnd(),buffer,sizeof(buffer));
	m_ed_end.SetWindowText(buffer);
	
	if ( rcomp->CrossSect() != 0 )
	{
		if ( rcomp->CrossSect()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_crosssect.SetWindowText(buffer);
		}
	}

	if ( rcomp->Peri() != 0 )
	{
		if ( rcomp->Peri()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_perimeter.SetWindowText(buffer);
		}
	}

	if ( rcomp->Wi() != 0 )
	{
		if ( rcomp->Wi()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_width.SetWindowText(buffer);
		}
	}

	if ( rcomp->FrictionSlope() != 0 )
	{
		if ( rcomp->FrictionSlope()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_frict.SetWindowText(buffer);
		}
	}

	if ( rcomp->Dispersion() != 0 )
	{
		if ( rcomp->Dispersion()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_disp.SetWindowText(buffer);
		}
	}
	withdispersion = rcomp->WithDispersion();
	if ( withdispersion == FALSE )
	{
		m_rb_withoutdisp.SetCheck(1);
		m_ed_disp.EnableWindow(FALSE);
	}
	else
	{
		m_rb_withdisp.SetCheck(1);
		m_ed_disp.EnableWindow(TRUE);
	}

	if ( rcomp->EndLevel() != 0 )
	{
		if ( rcomp->EndLevel()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_endlevel.SetWindowText(buffer);
		}
	}
	leveltype = rcomp->EndLevelType();
	switch ( leveltype )
	{
	case NormalLevel:
		m_rb_levnorm.SetCheck(1);
		m_ed_endlevel.EnableWindow(FALSE);
		break;
	case CriticalLevel:
		m_rb_levcrit.SetCheck(1);
		m_ed_endlevel.EnableWindow(FALSE);
		break;
	case GivenLevel:
		m_rb_levgiven.SetCheck(1);
		m_ed_endlevel.EnableWindow(TRUE);
		break;
	}
	switch ( rcomp->Method() )
	{
	case KinWave:
		m_rb_methkin.SetCheck(1);
		m_tx_endlevel.EnableWindow(FALSE);
		m_rb_levnorm.EnableWindow(FALSE);
		m_rb_levcrit.EnableWindow(FALSE);
		m_rb_levgiven.EnableWindow(FALSE);
		m_ed_endlevel.EnableWindow(FALSE);
		break;
	case DiffWave:
		m_rb_methdiff.SetCheck(1);
		m_tx_endlevel.EnableWindow(TRUE);
		m_rb_levnorm.EnableWindow(TRUE);
		m_rb_levcrit.EnableWindow(TRUE);
		m_rb_levgiven.EnableWindow(TRUE);
//		m_ed_endlevel.EnableWindow(TRUE);
		break;
	}

	if ( rcomp->CalcSed() == TRUE )
	{
		m_rb_sedyes.SetCheck(1);
		m_bu_sed.EnableWindow(TRUE);
	}
	else
	{
		m_rb_sedno.SetCheck(1);
		m_bu_sed.EnableWindow(FALSE);
	}

	NumToString(rcomp->NumGridPts(),buffer,sizeof(buffer));
	m_ed_numgrid.SetWindowText(buffer);

	if ( rcomp->HighRes() == TRUE )
	{
		m_rb_highres.SetCheck(1);
	}
	else
	{
		m_rb_lowres.SetCheck(1);
	}

	if ( rcomp->Active() == TRUE )
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


void CEdRiv::OnBuVars() 
{
	RIVCOMP* tempcomp = new RIVCOMP(rcomp);
	CEdCompActVar EdCompActVarDlg;
	if ( EdCompActVarDlg.DoModal(tempcomp) == IDOK )
	{
		delete rcomp;
		rcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdRiv::OnBuProcs() 
{
	RIVCOMP* tempcomp = new RIVCOMP(rcomp);
	CEdCompActProc EdCompActProcDlg;
	if ( EdCompActProcDlg.DoModal(tempcomp) == IDOK )
	{
		delete rcomp;
		rcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdRiv::OnBuInits() 
{
	RIVCOMP* tempcomp = new RIVCOMP(rcomp);
	CEdRivInits EdRivInitsDlg;
	if ( EdRivInitsDlg.DoModal(tempcomp) == IDOK )
	{
		delete rcomp;
		rcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdRiv::OnBuInp() 
{
	RIVCOMP* tempcomp = new RIVCOMP(rcomp);
	if ( EditInputs(tempcomp) == IDOK )
	{
		delete rcomp;
		rcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdRiv::OnCancel() 
{
	delete rcomp;
	
	CDialog::OnCancel();
}


void CEdRiv::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r1,r2; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(483),  // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	rcomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(484), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	rcomp->CmpIndex(c);

	m_ed_gravit.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(485), // FailGravit
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->Gravit(r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(486), // IllegalGravit
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_start.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(487), // FailStart
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_end.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(488), // FailEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->XStartEnd(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(489), // IllegalStartEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_crosssect.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->CrossSect(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(490), // ParseErrCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(491), // IllegalCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_perimeter.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->Peri(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(492), // ParseErrPeri
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(493), // IllegalPeri
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_width.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->Wi(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(494), // ParseErrWidth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(495), // IllegalWidth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_frict.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->FrictionSlope(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(496), // ParseErrFrict
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(497), // IllegalFrict
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_disp.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->Dispersion(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( withdispersion == TRUE )
		{
			if ( numerr > 0 )
			{
				MessageBox(aqapp.ini.T(498), // ParseErrDisp
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			else
			{
				MessageBox(aqapp.ini.T(499), // IllegalDisp
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( rcomp->WithDispersion(withdispersion) == FALSE )
	{
		aqapp.ProgErr("Unable to set dispersion type");
	}

	m_ed_endlevel.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->EndLevel(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( leveltype == GivenLevel )
		{
			if ( numerr > 0 )
			{
				MessageBox(aqapp.ini.T(500), // ParseErrLevel
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			else
			{
				MessageBox(aqapp.ini.T(501), // IllegalLevel
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( rcomp->EndLevelType(leveltype) == FALSE )
	{
		if ( rcomp->Method() != KinWave )
		{
			aqapp.ProgErr("Unable to set level type");
		}
	}

	m_ed_numgrid.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(502), // FailNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->NumGridPts(c) == FALSE )
	{
		MessageBox(aqapp.ini.T(503), // IllegalNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_active.GetCheck() ) rcomp->Active(TRUE);
    else                          rcomp->Active(FALSE);

	if ( add == TRUE )
	{
		if ( aqsys.AddComp(rcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(504),  // FailAddComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,rcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(505),  // IllegalComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdRiv::OnRbWithoutdisp() 
{
	withdispersion = FALSE;
	m_ed_disp.EnableWindow(FALSE);
	return;
}


void CEdRiv::OnRbWithdisp() 
{
	withdispersion = TRUE;
	m_ed_disp.EnableWindow(TRUE);
	return;
}


void CEdRiv::OnRbMethkin() 
{
	rcomp->Method(KinWave);
	m_tx_endlevel.EnableWindow(FALSE);
	m_rb_levnorm.EnableWindow(FALSE);
	m_rb_levcrit.EnableWindow(FALSE);
	m_rb_levgiven.EnableWindow(FALSE);
	m_ed_endlevel.EnableWindow(FALSE);
	return;
}


void CEdRiv::OnRbMethdiff() 
{
	rcomp->Method(DiffWave);
	m_tx_endlevel.EnableWindow(TRUE);
	m_rb_levnorm.EnableWindow(TRUE);
	m_rb_levcrit.EnableWindow(TRUE);
	m_rb_levgiven.EnableWindow(TRUE);
	if ( leveltype == GivenLevel )
	{
		m_ed_endlevel.EnableWindow(TRUE);
	}
	return;
}


void CEdRiv::OnRbLevnorm() 
{
	leveltype = NormalLevel;
	m_ed_endlevel.EnableWindow(FALSE);
	return;
}


void CEdRiv::OnRbLevcrit() 
{
	leveltype = CriticalLevel;
	m_ed_endlevel.EnableWindow(FALSE);
	return;
}


void CEdRiv::OnRbLevgiven() 
{
	leveltype = GivenLevel;
	m_ed_endlevel.EnableWindow(TRUE);
	return;
}



void CEdRiv::OnRbSedno() 
{
	rcomp->CalcSed(FALSE);
	m_bu_sed.EnableWindow(FALSE);
	return;
}


void CEdRiv::OnRbSedyes() 
{
	rcomp->CalcSed(TRUE);
	m_bu_sed.EnableWindow(TRUE);
	return;
}


void CEdRiv::OnBuSed() 
{
	RIVCOMP* tempcomp = new RIVCOMP(rcomp);
	CEdRivSed EdRivSedDlg;
	if ( EdRivSedDlg.DoModal(tempcomp) == IDOK )
	{
		delete rcomp;
		rcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdRiv::OnRbLowres() 
{
	rcomp->HighRes(FALSE);
	return;
}


void CEdRiv::OnRbHighres() 
{
	rcomp->HighRes(TRUE);
	return;
}


void CEdRiv::OnBuAcc() 
{
	RIVCOMP* tempcomp = new RIVCOMP(rcomp);
	CEdRivAcc EdRivAccDlg;
	if ( EdRivAccDlg.DoModal(tempcomp) == IDOK )
	{
		delete rcomp;
		rcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivInits dialog


CEdRivInits::CEdRivInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivInits, CDialog)
	//{{AFX_MSG_MAP(CEdRivInits)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivInits::DoModal(RIVCOMP* com)
{
	rcomp = com;
	return CDialog::DoModal();
}


void CEdRivInits::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inits.GetTopIndex();
	m_lb_inits.ResetContent();
	for ( CARDINAL i=0; i<rcomp->NumInitCond(); i++ )
	{
		strcpy(buffer,rcomp->InitVar(i)->Symbol());
		strcat(buffer,"(");
		strcat(buffer,rcomp->ZoneName(rcomp->InitZone(i)));
		strcat(buffer,") : ");
		rcomp->InitVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inits.InsertString(-1,buffer);
	}
	m_lb_inits.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdRivInits::UpdateActivation()
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
// CEdRivInits message handlers


BOOL CEdRivInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivInits::OnBuAdd() 
{
	int max = rcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdRivInit EdRivInitDlg;
	EdRivInitDlg.DoModal(rcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdRivInits::OnBuEdit() 
{
	int max = rcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdRivInit EdRivInitDlg;
	EdRivInitDlg.DoModal(rcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdRivInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(506),   // DelInits?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = rcomp->NumInitCond();
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
			if ( rcomp->DeleteInitCond(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(507),  // FailDelInit
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


void CEdRivInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;	
}


void CEdRivInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivInit dialog


CEdRivInit::CEdRivInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivInit, CDialog)
	//{{AFX_MSG_MAP(CEdRivInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivInit::DoModal(RIVCOMP* com, BOOL a, int i)
{
	rcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivInit message handlers


BOOL CEdRivInit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=rcomp->NumInitCond())
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
	for ( CARDINAL i=0; i<rcomp->NumZones(); i++ )
	{
		m_le_zone.AddString(rcomp->ZoneName(i));
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
		m_le_zone.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(rcomp->InitVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		m_le_zone.SetCurSel(rcomp->InitZone(pos));
		if ( rcomp->InitVal(pos) != 0 )
		{
			rcomp->InitVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_initcond.SetWindowText(buffer);
		}
	}
	
	if ( rcomp->NumZones() == 1 )
	{
		m_tx_zone.EnableWindow(FALSE);
		m_le_zone.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivInit::OnOK() 
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
		if ( rcomp->AddInitCond(aqsys.Varlist(),i,var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(508),  // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(509),  // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( rcomp->ReplaceInitCond(aqsys.Varlist(),i,var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(508),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(510),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////


static int EditInputs(RIVCOMP* com)
{
	CEdRivSelInp EdRivSelInpDlg;
	if ( EdRivSelInpDlg.DoModal(com) == IDOK )
	{
		switch ( inputtype )
		{
		case 0:
			{
				CEdRivUpInps EdRivUpInpsDlg;
				return EdRivUpInpsDlg.DoModal(com);
			}
		case 1:
			{
				CEdRivLatInps EdRivLatInpsDlg;
				return EdRivLatInpsDlg.DoModal(com);
			}
		default:
			{
				CEdRivUpInps EdRivUpInpsDlg;
				return EdRivUpInpsDlg.DoModal(com);
			}
		}
	}
	return IDCANCEL;
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivSelInp dialog


CEdRivSelInp::CEdRivSelInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivSelInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivSelInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivSelInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivSelInp)
	DDX_Control(pDX, IDC_RB_UPSTREAMINP, m_rb_upstreaminp);
	DDX_Control(pDX, IDC_RB_LATINP, m_rb_latinp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivSelInp, CDialog)
	//{{AFX_MSG_MAP(CEdRivSelInp)
	ON_BN_CLICKED(IDC_RB_LATINP, OnRbLatinp)
	ON_BN_CLICKED(IDC_RB_UPSTREAMINP, OnRbUpstreaminp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivSelInp::DoModal(RIVCOMP* com)
{
	rcomp = com;

	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivSelInp message handlers


BOOL CEdRivSelInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	switch ( inputtype )
	{
	case 0:
		m_rb_upstreaminp.SetCheck(1);
		break;
	case 1:
		m_rb_latinp.SetCheck(1);
		break;
	default:
		inputtype = 0;
		m_rb_upstreaminp.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivSelInp::OnRbUpstreaminp() 
{
	inputtype = 0;
	return;
}


void CEdRivSelInp::OnRbLatinp() 
{
	inputtype = 1;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivUpInps dialog


CEdRivUpInps::CEdRivUpInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivUpInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivUpInps)
	//}}AFX_DATA_INIT
}


void CEdRivUpInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivUpInps)
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivUpInps, CDialog)
	//{{AFX_MSG_MAP(CEdRivUpInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivUpInps::DoModal(RIVCOMP* com)
{
	rcomp = com;
	return CDialog::DoModal();
}


void CEdRivUpInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<rcomp->NumInput(); i++ )
	{
		strcpy(buffer,rcomp->InputVar(i)->Symbol());
		strcat(buffer," : ");
		rcomp->InputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdRivUpInps::UpdateActivation()
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
// CEdRivUpInps message handlers


BOOL CEdRivUpInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( rcomp->Inflow() != 0 )
	{
		if ( rcomp->Inflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivUpInps::OnBuAdd() 
{
	int max = rcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdRivUpInp EdRivUpInpDlg;
	EdRivUpInpDlg.DoModal(rcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdRivUpInps::OnBuEdit() 
{
	int max = rcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdRivUpInp EdRivUpInpDlg;
	EdRivUpInpDlg.DoModal(rcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdRivUpInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(511),    // DelUpInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = rcomp->NumInput();
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
			if ( rcomp->DeleteInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(512),  // FailDelInp
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


void CEdRivUpInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdRivUpInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdRivUpInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->Inflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(513), // IllegalInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivUpInp dialog


CEdRivUpInp::CEdRivUpInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivUpInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivUpInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivUpInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivUpInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivUpInp, CDialog)
	//{{AFX_MSG_MAP(CEdRivUpInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivUpInp::DoModal(RIVCOMP* com, BOOL a, int i)
{
	rcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivUpInp message handlers


BOOL CEdRivUpInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=rcomp->NumInput())
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
		int varpos = aqsys.Varlist()->Position(rcomp->InputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( rcomp->InputFlux(pos) != 0 )
		{
			rcomp->InputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivUpInp::OnOK() 
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
		if ( rcomp->AddInput(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(514),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(515),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( rcomp->ReplaceInput(aqsys.Varlist(),var,
						 buffer1,buffer2,parseerrors,
								 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(514),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(516),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivLatInps dialog


CEdRivLatInps::CEdRivLatInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivLatInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivLatInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivLatInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivLatInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivLatInps, CDialog)
	//{{AFX_MSG_MAP(CEdRivLatInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivLatInps::DoModal(RIVCOMP* com)
{
	rcomp = com;
	return CDialog::DoModal();
}


void CEdRivLatInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<rcomp->NumLatInput(); i++ )
	{
		strcpy(buffer,rcomp->LatInputVar(i)->Symbol());
		strcat(buffer," : ");
		rcomp->LatInputConc(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdRivLatInps::UpdateActivation()
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
// CEdRivLatInps message handlers


BOOL CEdRivLatInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( rcomp->LatInflow() != 0 )
	{
		if ( rcomp->LatInflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivLatInps::OnBuAdd() 
{
	int max = rcomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdRivLatInp EdRivLatInpDlg;
	EdRivLatInpDlg.DoModal(rcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdRivLatInps::OnBuEdit() 
{
	int max = rcomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdRivLatInp EdRivLatInpDlg;
	EdRivLatInpDlg.DoModal(rcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdRivLatInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(517),            // DelLatInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = rcomp->NumLatInput();
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
			if ( rcomp->DeleteLatInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(518),  // FailDelInp
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


void CEdRivLatInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdRivLatInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdRivLatInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( rcomp->LatInflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(519), // IllegalLatInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivLatInp dialog


CEdRivLatInp::CEdRivLatInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivLatInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivLatInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivLatInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivLatInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivLatInp, CDialog)
	//{{AFX_MSG_MAP(CEdRivLatInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivLatInp::DoModal(RIVCOMP* com, BOOL a, int i)
{
	rcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivLatInp message handlers


BOOL CEdRivLatInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=rcomp->NumLatInput())
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
		int varpos = aqsys.Varlist()->Position(rcomp->LatInputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( rcomp->LatInputConc(pos) != 0 )
		{
			rcomp->LatInputConc(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivLatInp::OnOK() 
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
		if ( rcomp->AddLatInput(aqsys.Varlist(),var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(520),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(521),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( rcomp->ReplaceLatInput(aqsys.Varlist(),var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(520),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(522),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CEdRivAcc dialog


CEdRivAcc::CEdRivAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivAcc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivAcc)
	DDX_Control(pDX, IDC_ED_RELACCZ, m_ed_relaccz);
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_RELACCE, m_ed_relacce);
	DDX_Control(pDX, IDC_ED_RELACCA, m_ed_relacca);
	DDX_Control(pDX, IDC_ED_ABSACCZ, m_ed_absaccz);
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	DDX_Control(pDX, IDC_ED_ABSACCE, m_ed_absacce);
	DDX_Control(pDX, IDC_ED_ABSACCA, m_ed_absacca);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivAcc, CDialog)
	//{{AFX_MSG_MAP(CEdRivAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivAcc::DoModal(RIVCOMP* com)
{
	rcomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivAcc message handlers

BOOL CEdRivAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	NumToString(rcomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(rcomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(rcomp->RelAccA(),buffer,sizeof(buffer));
	m_ed_relacca.SetWindowText(buffer);
	NumToString(rcomp->AbsAccA(),buffer,sizeof(buffer));
	m_ed_absacca.SetWindowText(buffer);
	
	NumToString(rcomp->RelAccZ(),buffer,sizeof(buffer));
	m_ed_relaccz.SetWindowText(buffer);
	NumToString(rcomp->AbsAccZ(),buffer,sizeof(buffer));
	m_ed_absaccz.SetWindowText(buffer);
	
	NumToString(rcomp->RelAccE(),buffer,sizeof(buffer));
	m_ed_relacce.SetWindowText(buffer);
	NumToString(rcomp->AbsAccE(),buffer,sizeof(buffer));
	m_ed_absacce.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdRivAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(595),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(596),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(597),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relacca.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(598),  // FailRelAccA
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacca.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(599),  // FailRelAccA
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->AccA(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(600),  // FailAccA
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccz.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(601),  // FailRelAccZ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccz.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(602),  // FailRelAccZ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->AccZ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(603),  // FailAccZ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relacce.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(604),  // FailRelAccE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacce.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(605),  // FailAbsAccE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( rcomp->AccE(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(606),  // FailAccE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CEdRivSedLayer dialog


CEdRivSedLayer::CEdRivSedLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivSedLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivSedLayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdRivSedLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivSedLayer)
	DDX_Control(pDX, IDC_ED_DISPERSION, m_ed_dispersion);
	DDX_Control(pDX, IDC_ED_ZONEIND, m_ed_zoneind);
	DDX_Control(pDX, IDC_ED_VELOCITY, m_ed_velocity);
	DDX_Control(pDX, IDC_ED_POROSITY, m_ed_porosity);
	DDX_Control(pDX, IDC_ED_EXCHCOEFF, m_ed_exchcoeff);
	DDX_Control(pDX, IDC_ED_AREA, m_ed_area);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivSedLayer, CDialog)
	//{{AFX_MSG_MAP(CEdRivSedLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivSedLayer::DoModal(RIVCOMP* com, BOOL a, int i)
{
	rcomp = com;
	add   = a;
	pos   = i;
	if ( add == TRUE )
	{
		sedlayer = new RIVSEDLAYER;
	}
	else
	{
		sedlayer = new RIVSEDLAYER(rcomp->SedLayer(pos));
	}
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivSedLayer message handlers


BOOL CEdRivSedLayer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	NumToString(sedlayer->ZoneIndex(),buffer,sizeof(buffer));
	m_ed_zoneind.SetWindowText(buffer);

	if ( sedlayer->Area() != 0 )
	{
		if ( sedlayer->Area()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_area.SetWindowText(buffer);
		}
	}
	
	if ( sedlayer->Porosity() != 0 )
	{
		if ( sedlayer->Porosity()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_porosity.SetWindowText(buffer);
		}
	}
	
	if ( sedlayer->Discharge() != 0 )
	{
		if ( sedlayer->Discharge()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_velocity.SetWindowText(buffer);
		}
	}
	
	if ( sedlayer->Dispersion() != 0 )
	{
		if ( sedlayer->Dispersion()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_dispersion.SetWindowText(buffer);
		}
	}
	
	if ( sedlayer->ExchCoeff() != 0 )
	{
		if ( sedlayer->ExchCoeff()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_exchcoeff.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivSedLayer::OnOK() 
{
	char buffer[1024]; char buffer1[1024]; CARDINAL c,numerr;

	m_ed_zoneind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(628), // FailZoneInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	sedlayer->ZoneIndex(c);

	m_ed_area.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->Area(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(629), // ParseErrArea
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(630), // IllegalArea
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_porosity.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->Porosity(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(631), // ParseErrPorosity
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(632), // IllegalPorosity
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_velocity.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->Discharge(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(633), // ParseErrDischarge
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(634), // IllegalDischarge
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_dispersion.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->Dispersion(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(641), // ParseErrDispersion
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(642), // IllegalDispersion
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_exchcoeff.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->ExchCoeff(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(635), // ParseErrExchCoeff
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(636), // IllegalExchCoeff
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( add == TRUE )
	{
		if ( rcomp->AddSedLayer(sedlayer,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(637), // FailAddSedLayer
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( rcomp->ReplaceSedLayer(sedlayer,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(638), // IllegalSedlayer
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}



void CEdRivSedLayer::OnCancel() 
{
    delete sedlayer;
	
	CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivSed dialog


CEdRivSed::CEdRivSed(CWnd* pParent /*=NULL*/)
	: CDialog(CEdRivSed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdRivSed)
	//}}AFX_DATA_INIT
}


void CEdRivSed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdRivSed)
	DDX_Control(pDX, IDC_LB_SEDLAYERS, m_lb_sedlayers);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdRivSed, CDialog)
	//{{AFX_MSG_MAP(CEdRivSed)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_SEDLAYERS, OnSelchangeLbSedlayers)
	ON_LBN_DBLCLK(IDC_LB_SEDLAYERS, OnDblclkLbSedlayers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdRivSed::DoModal(RIVCOMP* com)
{
    rcomp = com;
	return CDialog::DoModal();
}


void CEdRivSed::UpdateActivation()
{
	switch ( m_lb_sedlayers.GetSelCount() )
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


void CEdRivSed::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];

	int topindex = m_lb_sedlayers.GetTopIndex();
	m_lb_sedlayers.ResetContent();
	for ( CARDINAL i=0; i<rcomp->NumSedLayers(); i++ )
	{
		const RIVSEDLAYER* sedlayer = rcomp->SedLayer(i);
		NumToString(sedlayer->ZoneIndex(),buffer,sizeof(buffer));
		strcat(buffer,":  ");
		(sedlayer->Area())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		(sedlayer->Porosity())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		(sedlayer->Discharge())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		(sedlayer->Dispersion())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		(sedlayer->ExchCoeff())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_sedlayers.InsertString(-1,buffer);
	}
	m_lb_sedlayers.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdRivSed message handlers

BOOL CEdRivSed::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdRivSed::OnBuAdd() 
{
	int max = rcomp->NumSedLayers();
	int* indices = new int[max];
	int num = m_lb_sedlayers.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdRivSedLayer EdRivSedLayerDlg;
	EdRivSedLayerDlg.DoModal(rcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdRivSed::OnBuEdit() 
{
	int max = rcomp->NumSedLayers();
	int* indices = new int[max];
	int num = m_lb_sedlayers.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdRivSedLayer EdRivSedLayerDlg;
	EdRivSedLayerDlg.DoModal(rcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdRivSed::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(639),  // DelLayers?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = rcomp->NumSedLayers();
	int* indices = new int[max];
	int num = m_lb_sedlayers.GetSelItems(max,indices);
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
			if ( rcomp->DeleteSedLayer(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(640),   // FailDelLayer
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


void CEdRivSed::OnSelchangeLbSedlayers() 
{
	UpdateActivation();
	return;
}


void CEdRivSed::OnDblclkLbSedlayers() 
{
	OnBuEdit();
	return;
}


void CEdRivSed::OnOK() 
{
	CDialog::OnOK();
}


