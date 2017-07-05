// edadv_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edadv_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static int inputtype = 0;

static int EditInputs(ADVCOMP* com);

/////////////////////////////////////////////////////////////////////////////
// CEdAdv dialog


CEdAdv::CEdAdv(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdv::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdv)
	//}}AFX_DATA_INIT
}


void CEdAdv::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdv)
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_RB_WITHOUTDIFF, m_rb_withoutdiff);
	DDX_Control(pDX, IDC_RB_WITHDIFF, m_rb_withdiff);
	DDX_Control(pDX, IDC_RB_HIGHRES, m_rb_highres);
	DDX_Control(pDX, IDC_RB_LOWRES, m_rb_lowres);
	DDX_Control(pDX, IDC_ED_START, m_ed_start);
	DDX_Control(pDX, IDC_ED_NUMGRID, m_ed_numgrid);
	DDX_Control(pDX, IDC_ED_CROSSSECT, m_ed_crosssect);
	DDX_Control(pDX, IDC_ED_END, m_ed_end);
	DDX_Control(pDX, IDC_ED_DIFF, m_ed_diff);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	DDX_Control(pDX, IDC_BU_VARS, m_bu_vars);
	DDX_Control(pDX, IDC_BU_PROCS, m_bu_procs);
	DDX_Control(pDX, IDC_BU_INP, m_bu_inp);
	DDX_Control(pDX, IDC_BU_INITS, m_bu_inits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdv, CDialog)
	//{{AFX_MSG_MAP(CEdAdv)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_RB_WITHOUTDIFF, OnRbWithoutdiff)
	ON_BN_CLICKED(IDC_RB_WITHDIFF, OnRbWithdiff)
	ON_BN_CLICKED(IDC_RB_LOWRES, OnRbLowres)
	ON_BN_CLICKED(IDC_RB_HIGHRES, OnRbHighres)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdv::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	acomp = new ADVCOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdv message handlers

BOOL CEdAdv::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( acomp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(acomp->Symbol());
	}

	if ( acomp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(acomp->Description());
	}

	NumToString(acomp->CmpIndex(),buffer,sizeof(buffer));
	m_ed_compind.SetWindowText(buffer);

	NumToString(acomp->XStart(),buffer,sizeof(buffer));
	m_ed_start.SetWindowText(buffer);

	NumToString(acomp->XEnd(),buffer,sizeof(buffer));
	m_ed_end.SetWindowText(buffer);
	
	if ( acomp->CrossSect() != 0 )
	{
		if ( acomp->CrossSect()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_crosssect.SetWindowText(buffer);
		}
	}

	if ( acomp->GlobDiff() != 0 )
	{
		if ( acomp->GlobDiff()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_diff.SetWindowText(buffer);
		}
	}
	withdiffusion = acomp->WithDiffusion();
	if ( withdiffusion == FALSE )
	{
		m_rb_withoutdiff.SetCheck(1);
		m_ed_diff.EnableWindow(FALSE);
	}
	else
	{
		m_rb_withdiff.SetCheck(1);
		m_ed_diff.EnableWindow(TRUE);
	}

	NumToString(acomp->NumGridPts(),buffer,sizeof(buffer));
	m_ed_numgrid.SetWindowText(buffer);

	if ( acomp->HighRes() == TRUE )
	{
		m_rb_highres.SetCheck(1);
	}
	else
	{
		m_rb_lowres.SetCheck(1);
	}

	if ( acomp->Active() == TRUE )
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


void CEdAdv::OnBuVars() 
{
	ADVCOMP* tempcomp = new ADVCOMP(acomp);
	CEdCompActVar EdCompActVarDlg;
	if ( EdCompActVarDlg.DoModal(tempcomp) == IDOK )
	{
		delete acomp;
		acomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdAdv::OnBuProcs() 
{
	ADVCOMP* tempcomp = new ADVCOMP(acomp);
	CEdCompActProc EdCompActProcDlg;
	if ( EdCompActProcDlg.DoModal(tempcomp) == IDOK )
	{
		delete acomp;
		acomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdAdv::OnBuInits() 
{
	ADVCOMP* tempcomp = new ADVCOMP(acomp);
	CEdAdvInits EdAdvInitsDlg;
	if ( EdAdvInitsDlg.DoModal(tempcomp) == IDOK )
	{
		delete acomp;
		acomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdAdv::OnBuInp() 
{
	ADVCOMP* tempcomp = new ADVCOMP(acomp);
	if ( EditInputs(tempcomp) == IDOK )
	{
		delete acomp;
		acomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdAdv::OnCancel() 
{
	delete acomp;
	
	CDialog::OnCancel();
}


void CEdAdv::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r1, r2; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( acomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(404),  // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	acomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(405), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	acomp->CmpIndex(c);

	m_ed_start.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(406), // FailStart
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_end.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(407), // FailEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( acomp->XStartEnd(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(408), // IllegalStartEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_crosssect.GetWindowText(buffer,sizeof(buffer));
	if ( acomp->CrossSect(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(409), // ParseErrCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(410), // IllegalCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_diff.GetWindowText(buffer,sizeof(buffer));
	if ( acomp->GlobDiff(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( withdiffusion == TRUE )
		{
			if ( numerr > 0 )
			{
				MessageBox(aqapp.ini.T(411), // ParseErrDiff
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			else
			{
				MessageBox(aqapp.ini.T(412), // IllegalDiff
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( acomp->WithDiffusion(withdiffusion) == FALSE )
	{
		aqapp.ProgErr("Unable to set diffusion type");
	}

	m_ed_numgrid.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(413), // FailNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( acomp->NumGridPts(c) == FALSE )
	{
		MessageBox(aqapp.ini.T(414), // IllegalNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_active.GetCheck() ) acomp->Active(TRUE);
    else                          acomp->Active(FALSE);

	if ( add == TRUE )
	{
		if ( aqsys.AddComp(acomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(415),  // FailAddComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,acomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(416),  // IllegalComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


void CEdAdv::OnRbWithoutdiff() 
{
	withdiffusion = FALSE;
	m_ed_diff.EnableWindow(FALSE);
	return;
}


void CEdAdv::OnRbWithdiff() 
{
	withdiffusion = TRUE;
	m_ed_diff.EnableWindow(TRUE);
	return;
}


void CEdAdv::OnRbLowres() 
{
	acomp->HighRes(FALSE);
	return;
}


void CEdAdv::OnRbHighres() 
{
	acomp->HighRes(TRUE);
	return;
}


void CEdAdv::OnBuAcc() 
{
	ADVCOMP* tempcomp = new ADVCOMP(acomp);
	CEdAdvAcc EdAdvAccDlg;
	if ( EdAdvAccDlg.DoModal(tempcomp) == IDOK )
	{
		delete acomp;
		acomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInits dialog


CEdAdvInits::CEdAdvInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvInits, CDialog)
	//{{AFX_MSG_MAP(CEdAdvInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvInits::DoModal(ADVCOMP* com)
{
	acomp = com;
	return CDialog::DoModal();
}


void CEdAdvInits::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inits.GetTopIndex();
	m_lb_inits.ResetContent();
	for ( CARDINAL i=0; i<acomp->NumInitCond(); i++ )
	{
		strcpy(buffer,acomp->InitVar(i)->Symbol());
		strcat(buffer,"(");
		strcat(buffer,acomp->ZoneName(acomp->InitZone(i)));
		strcat(buffer,") : ");
		acomp->InitVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inits.InsertString(-1,buffer);
	}
	m_lb_inits.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdAdvInits::UpdateActivation()
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
// CEdAdvInits message handlers


BOOL CEdAdvInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvInits::OnBuAdd() 
{
	int max = acomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdAdvInit EdAdvInitDlg;
	EdAdvInitDlg.DoModal(acomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdAdvInits::OnBuEdit() 
{
	int max = acomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdAdvInit EdAdvInitDlg;
	EdAdvInitDlg.DoModal(acomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdAdvInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(417),            // DelInits?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = acomp->NumInitCond();
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
			if ( acomp->DeleteInitCond(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(418),  // UnableDelIni
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


void CEdAdvInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;	
}


void CEdAdvInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInit dialog


CEdAdvInit::CEdAdvInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvInit, CDialog)
	//{{AFX_MSG_MAP(CEdAdvInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvInit::DoModal(ADVCOMP* com, BOOL a, int i)
{
	acomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInit message handlers

BOOL CEdAdvInit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=acomp->NumInitCond())
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
	for ( CARDINAL i=0; i<acomp->NumZones(); i++ )
	{
		m_le_zone.AddString(acomp->ZoneName(i));
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
		m_le_zone.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(acomp->InitVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		m_le_zone.SetCurSel(acomp->InitZone(pos));
		if ( acomp->InitVal(pos) != 0 )
		{
			acomp->InitVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_initcond.SetWindowText(buffer);
		}
	}
	
	if ( acomp->NumZones() == 1 )
	{
		m_tx_zone.EnableWindow(FALSE);
		m_le_zone.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvInit::OnOK() 
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
		if ( acomp->AddInitCond(aqsys.Varlist(),i,var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(419),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(420),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( acomp->ReplaceInitCond(aqsys.Varlist(),i,var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(419),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(421),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////


static int EditInputs(ADVCOMP* com)
{
	CEdAdvSelInp EdAdvSelInpDlg;
	if ( EdAdvSelInpDlg.DoModal(com) == IDOK )
	{
		switch ( inputtype )
		{
		case 0:
			{
				CEdAdvInInps EdAdvInInpsDlg;
				return EdAdvInInpsDlg.DoModal(com);
			}
		case 1:
			{
				CEdAdvLatInps EdAdvLatInpsDlg;
				return EdAdvLatInpsDlg.DoModal(com);
			}
		default:
			{
				CEdAdvInInps EdAdvInInpsDlg;
				return EdAdvInInpsDlg.DoModal(com);
			}
		}
	}
	return IDCANCEL;
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvSelInp dialog


CEdAdvSelInp::CEdAdvSelInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvSelInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvSelInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvSelInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvSelInp)
	DDX_Control(pDX, IDC_RB_LATINP, m_rb_latinp);
	DDX_Control(pDX, IDC_RB_INLETINP, m_rb_inletinp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvSelInp, CDialog)
	//{{AFX_MSG_MAP(CEdAdvSelInp)
	ON_BN_CLICKED(IDC_RB_INLETINP, OnRbInletinp)
	ON_BN_CLICKED(IDC_RB_LATINP, OnRbLatinp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvSelInp::DoModal(ADVCOMP* com)
{
	acomp = com;

	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvSelInp message handlers


BOOL CEdAdvSelInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	switch ( inputtype )
	{
	case 0:
		m_rb_inletinp.SetCheck(1);
		break;
	case 1:
		m_rb_latinp.SetCheck(1);
		break;
	default:
		inputtype = 0;
		m_rb_inletinp.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvSelInp::OnRbInletinp() 
{
	inputtype = 0;
	return;
}


void CEdAdvSelInp::OnRbLatinp() 
{
	inputtype = 1;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInInps dialog


CEdAdvInInps::CEdAdvInInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvInInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvInInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvInInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvInInps)
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvInInps, CDialog)
	//{{AFX_MSG_MAP(CEdAdvInInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvInInps::DoModal(ADVCOMP* com)
{
	acomp = com;
	return CDialog::DoModal();
}


void CEdAdvInInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<acomp->NumInput(); i++ )
	{
		strcpy(buffer,acomp->InputVar(i)->Symbol());
		strcat(buffer," : ");
		acomp->InputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdAdvInInps::UpdateActivation()
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
// CEdAdvInInps message handlers


BOOL CEdAdvInInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( acomp->Inflow() != 0 )
	{
		if ( acomp->Inflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvInInps::OnBuAdd() 
{
	int max = acomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdAdvInInp EdAdvInInpDlg;
	EdAdvInInpDlg.DoModal(acomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdAdvInInps::OnBuEdit() 
{
	int max = acomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdAdvInInp EdAdvInInpDlg;
	EdAdvInInpDlg.DoModal(acomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdAdvInInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(422),            // DelUpInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = acomp->NumInput();
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
			if ( acomp->DeleteInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(423),  // FailDelInp
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


void CEdAdvInInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdAdvInInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdAdvInInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( acomp->Inflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(424), // IllegalInInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInInp dialog


CEdAdvInInp::CEdAdvInInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvInInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvInInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvInInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvInInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvInInp, CDialog)
	//{{AFX_MSG_MAP(CEdAdvInInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvInInp::DoModal(ADVCOMP* com, BOOL a, int i)
{
	acomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInInp message handlers


BOOL CEdAdvInInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=acomp->NumInput())
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
		int varpos = aqsys.Varlist()->Position(acomp->InputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( acomp->InputFlux(pos) != 0 )
		{
			acomp->InputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvInInp::OnOK() 
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
		if ( acomp->AddInput(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(425),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(426),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( acomp->ReplaceInput(aqsys.Varlist(),var,
						 buffer1,buffer2,parseerrors,
								 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(425),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(427),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvLatInps dialog


CEdAdvLatInps::CEdAdvLatInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvLatInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvLatInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvLatInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvLatInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvLatInps, CDialog)
	//{{AFX_MSG_MAP(CEdAdvLatInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvLatInps::DoModal(ADVCOMP* com)
{
	acomp = com;
	return CDialog::DoModal();
}


void CEdAdvLatInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<acomp->NumLatInput(); i++ )
	{
		strcpy(buffer,acomp->LatInputVar(i)->Symbol());
		strcat(buffer," : ");
		acomp->LatInputConc(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdAdvLatInps::UpdateActivation()
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
// CEdAdvLatInps message handlers


BOOL CEdAdvLatInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( acomp->LatInflow() != 0 )
	{
		if ( acomp->LatInflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvLatInps::OnBuAdd() 
{
	int max = acomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdAdvLatInp EdAdvLatInpDlg;
	EdAdvLatInpDlg.DoModal(acomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdAdvLatInps::OnBuEdit() 
{
	int max = acomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdAdvLatInp EdAdvLatInpDlg;
	EdAdvLatInpDlg.DoModal(acomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdAdvLatInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(428),            // DelLatInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = acomp->NumLatInput();
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
			if ( acomp->DeleteLatInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(429),  // FailDelInp
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


void CEdAdvLatInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdAdvLatInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdAdvLatInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( acomp->LatInflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(430), // IllegalLatInf
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvLatInp dialog


CEdAdvLatInp::CEdAdvLatInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvLatInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvLatInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvLatInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvLatInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvLatInp, CDialog)
	//{{AFX_MSG_MAP(CEdAdvLatInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvLatInp::DoModal(ADVCOMP* com, BOOL a, int i)
{
	acomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvLatInp message handlers

BOOL CEdAdvLatInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=acomp->NumLatInput())
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
		int varpos = aqsys.Varlist()->Position(acomp->LatInputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( acomp->LatInputConc(pos) != 0 )
		{
			acomp->LatInputConc(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdAdvLatInp::OnOK() 
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
		if ( acomp->AddLatInput(aqsys.Varlist(),var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(431),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(432),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( acomp->ReplaceLatInput(aqsys.Varlist(),var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(431),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(433),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CEdAdvAcc dialog


CEdAdvAcc::CEdAdvAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdAdvAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdAdvAcc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdAdvAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdAdvAcc)
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_RELACCD, m_ed_relaccd);
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	DDX_Control(pDX, IDC_ED_ABSACCD, m_ed_absaccd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdAdvAcc, CDialog)
	//{{AFX_MSG_MAP(CEdAdvAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdAdvAcc::DoModal(ADVCOMP* com)
{
	acomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdAdvAcc message handlers

BOOL CEdAdvAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	NumToString(acomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(acomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(acomp->RelAccD(),buffer,sizeof(buffer));
	m_ed_relaccd.SetWindowText(buffer);
	NumToString(acomp->AbsAccD(),buffer,sizeof(buffer));
	m_ed_absaccd.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdAdvAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(571),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(572),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( acomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(573),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccd.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(574),  // FailRelAccD
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccd.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(575),  // FailRelAccD
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( acomp->AccD(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(576),  // FailAccD
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
		
	CDialog::OnOK();
}

