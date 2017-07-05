// edsed_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edsed_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdSedDlg dialog


CEdSed::CEdSed(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSed)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSed)
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_RB_LOWRES, m_rb_lowres);
	DDX_Control(pDX, IDC_RB_HIGHRES, m_rb_highres);
	DDX_Control(pDX, IDC_ED_TOP, m_ed_top);
	DDX_Control(pDX, IDC_ED_NUMGRID, m_ed_numgrid);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_RATEPOR, m_ed_ratepor);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	DDX_Control(pDX, IDC_ED_BOTTOM, m_ed_bottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSed, CDialog)
	//{{AFX_MSG_MAP(CEdSed)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_RB_LOWRES, OnRbLowres)
	ON_BN_CLICKED(IDC_RB_HIGHRES, OnRbHighres)
	ON_BN_CLICKED(IDC_BU_PARTVARS, OnBuPartvars)
	ON_BN_CLICKED(IDC_BU_DISSVARS, OnBuDissvars)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSed::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	scomp = new SEDCOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSed message handlers

BOOL CEdSed::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( scomp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(scomp->Symbol());
	}

	if ( scomp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(scomp->Description());
	}

	NumToString(scomp->CmpIndex(),buffer,sizeof(buffer));
	m_ed_compind.SetWindowText(buffer);

	NumToString(scomp->ZTop(),buffer,sizeof(buffer));
	m_ed_top.SetWindowText(buffer);

	NumToString(scomp->ZBottom(),buffer,sizeof(buffer));
	m_ed_bottom.SetWindowText(buffer);

	if ( scomp->RatePorosity() != 0 )
	{
		if ( scomp->RatePorosity()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_ratepor.SetWindowText(buffer);
		}
	}

	NumToString(scomp->NumGridPts(),buffer,sizeof(buffer));
	m_ed_numgrid.SetWindowText(buffer);

	if ( scomp->HighRes() == TRUE )
	{
		m_rb_highres.SetCheck(1);
	}
	else
	{
		m_rb_lowres.SetCheck(1);
	}

	if ( scomp->Active() == TRUE )
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


void CEdSed::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r1,r2; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(650),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	scomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(651), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	scomp->CmpIndex(c);

	m_ed_top.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(652), // FailTop
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_bottom.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(653), // FailTop
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->ZTopBottom(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(654), // IllegalTopBottom
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_ratepor.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->RatePorosity(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(655), // ParseErrRatePor
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(656), // IllegalRatePor
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_numgrid.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(657), // FailNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->NumGridPts(c) == FALSE )
	{
		MessageBox(aqapp.ini.T(658), // IllegalNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_active.GetCheck() ) scomp->Active(TRUE);
    else                          scomp->Active(FALSE);

	if ( add == TRUE )
	{
		if ( aqsys.AddComp(scomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(659),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,scomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(660),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


void CEdSed::OnCancel() 
{
	delete scomp;
	
	CDialog::OnCancel();
}


void CEdSed::OnBuVars() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdCompActVar EdCompActVarDlg;
	if ( EdCompActVarDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdSed::OnBuProcs() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdCompActProc EdCompActProcDlg;
	if ( EdCompActProcDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdSed::OnBuInits() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdSedInits EdSedInitsDlg;
	if ( EdSedInitsDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdSed::OnBuInp() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdSedInps EdSedInpsDlg;
	if ( EdSedInpsDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdSed::OnBuPartvars() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdSedParts EdSedPartsDlg;
	if ( EdSedPartsDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdSed::OnBuDissvars() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdSedDisss EdSedDisssDlg;
	if ( EdSedDisssDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdSed::OnRbLowres() 
{
	scomp->HighRes(FALSE);
	return;
}


void CEdSed::OnRbHighres() 
{
	scomp->HighRes(TRUE);
	return;
}


void CEdSed::OnBuAcc() 
{
	SEDCOMP* tempcomp = new SEDCOMP(scomp);
	CEdSedAcc EdSedAccDlg;
	if ( EdSedAccDlg.DoModal(tempcomp) == IDOK )
	{
		delete scomp;
		scomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedInits dialog


CEdSedInits::CEdSedInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedInits, CDialog)
	//{{AFX_MSG_MAP(CEdSedInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedInits::DoModal(SEDCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSedInits::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inits.GetTopIndex();
	m_lb_inits.ResetContent();
	for ( CARDINAL i=0; i<scomp->NumInitCond(); i++ )
	{
		strcpy(buffer,scomp->InitVar(i)->Symbol());
		strcat(buffer,"(");
		strcat(buffer,scomp->ZoneName(scomp->InitZone(i)));
		strcat(buffer,") : ");
		scomp->InitVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inits.InsertString(-1,buffer);
	}
	m_lb_inits.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdSedInits::UpdateActivation()
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
// CEdSedInits message handlers

BOOL CEdSedInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedInits::OnBuAdd() 
{
	int max = scomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSedInit EdSedInitDlg;
	EdSedInitDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSedInits::OnBuEdit() 
{
	int max = scomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSedInit EdSedInitDlg;
	EdSedInitDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSedInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(661),            // DelInits?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumInitCond();
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
			if ( scomp->DeleteInitCond(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(662),
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


void CEdSedInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;
}


void CEdSedInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedInit dialog


CEdSedInit::CEdSedInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedInit, CDialog)
	//{{AFX_MSG_MAP(CEdSedInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedInit::DoModal(SEDCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedInit message handlers

BOOL CEdSedInit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=scomp->NumInitCond())
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
	for ( CARDINAL i=0; i<scomp->NumZones(); i++ )
	{
		m_le_zone.AddString(scomp->ZoneName(i));
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
		m_le_zone.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(scomp->InitVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		m_le_zone.SetCurSel(scomp->InitZone(pos));
		if ( scomp->InitVal(pos) != 0 )
		{
			scomp->InitVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_initcond.SetWindowText(buffer);
		}
	}
	
	if ( scomp->NumZones() == 1 )
	{
		m_tx_zone.EnableWindow(FALSE);
		m_le_zone.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdSedInit::OnOK() 
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
		if ( scomp->AddInitCond(aqsys.Varlist(),i,var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(663),
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(664),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceInitCond(aqsys.Varlist(),i,var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(663),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(665),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedInps dialog


CEdSedInps::CEdSedInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedInps)
	DDX_Control(pDX, IDC_ED_SURFPOR, m_ed_surfpor);
	DDX_Control(pDX, IDC_LB_TOPBCS, m_lb_topbcs);
	DDX_Control(pDX, IDC_BU_EDITTOPBC, m_bu_edittopbc);
	DDX_Control(pDX, IDC_BU_DELETETOPBC, m_bu_deletetopbc);
	DDX_Control(pDX, IDC_BU_ADDTOPBC, m_bu_addtopbc);
	DDX_Control(pDX, IDC_LB_BOTTOMBCS, m_lb_bottombcs);
	DDX_Control(pDX, IDC_BU_EDITBOTTOMBC, m_bu_editbottombc);
	DDX_Control(pDX, IDC_BU_DELETEBOTTOMBC, m_bu_deletebottombc);
	DDX_Control(pDX, IDC_BU_ADDBOTTOMBC, m_bu_addbottombc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedInps, CDialog)
	//{{AFX_MSG_MAP(CEdSedInps)
	ON_BN_CLICKED(IDC_BU_ADDTOPBC, OnBuAddTopBC)
	ON_BN_CLICKED(IDC_BU_DELETETOPBC, OnBuDeleteTopBC)
	ON_BN_CLICKED(IDC_BU_EDITTOPBC, OnBuEditTopBC)
	ON_LBN_SELCHANGE(IDC_LB_TOPBCS, OnSelchangeLbTopBCs)
	ON_LBN_DBLCLK(IDC_LB_TOPBCS, OnDblclkLbTopBCs)
	ON_BN_CLICKED(IDC_BU_ADDBOTTOMBC, OnBuAddBottomBC)
	ON_BN_CLICKED(IDC_BU_DELETEBOTTOMBC, OnBuDeleteBottomBC)
	ON_BN_CLICKED(IDC_BU_EDITBOTTOMBC, OnBuEditBottomBC)
	ON_LBN_SELCHANGE(IDC_LB_BOTTOMBCS, OnSelchangeLbBottomBCs)
	ON_LBN_DBLCLK(IDC_LB_BOTTOMBCS, OnDblclkLbBottomBCs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedInps::DoModal(SEDCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSedInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	CARDINAL i;
	int topindex = m_lb_topbcs.GetTopIndex();
	m_lb_topbcs.ResetContent();
	for ( i=0; i<scomp->NumTopBC(); i++ )
	{
		strcpy(buffer,scomp->TopBCVar(i)->Symbol());
		strcat(buffer," : ");
		scomp->TopBCCorF(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_topbcs.InsertString(-1,buffer);
	}
	m_lb_topbcs.SetTopIndex(topindex);
	topindex = m_lb_bottombcs.GetTopIndex();
	m_lb_bottombcs.ResetContent();
	for ( i=0; i<scomp->NumBottomBC(); i++ )
	{
		strcpy(buffer,scomp->BottomBCVar(i)->Symbol());
		strcat(buffer," : ");
		scomp->BottomBCConc(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_bottombcs.InsertString(-1,buffer);
	}
	m_lb_bottombcs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdSedInps::UpdateActivation()
{
	switch ( m_lb_topbcs.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_edittopbc.EnableWindow(FALSE);
		m_bu_deletetopbc.EnableWindow(FALSE);
		break;
	case 1:
		m_bu_edittopbc.EnableWindow(TRUE);
		m_bu_deletetopbc.EnableWindow(TRUE);
		break;
	default:
		m_bu_edittopbc.EnableWindow(FALSE);
		m_bu_deletetopbc.EnableWindow(TRUE);
	   break;
	}
	switch ( m_lb_bottombcs.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_editbottombc.EnableWindow(FALSE);
		m_bu_deletebottombc.EnableWindow(FALSE);
		break;
	case 1:
		m_bu_editbottombc.EnableWindow(TRUE);
		m_bu_deletebottombc.EnableWindow(TRUE);
		break;
	default:
		m_bu_editbottombc.EnableWindow(FALSE);
		m_bu_deletebottombc.EnableWindow(TRUE);
	   break;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedInps message handlers

BOOL CEdSedInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( scomp->SurfPorosity() != 0 )
	{
    	scomp->SurfPorosity()->UnParse(buffer,sizeof(buffer));
		m_ed_surfpor.SetWindowText(buffer);
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedInps::OnBuAddTopBC() 
{
	int max = scomp->NumTopBC();
	int* indices = new int[max];
	int num = m_lb_topbcs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSedTopBC EdSedTopBCDlg;
	EdSedTopBCDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSedInps::OnBuEditTopBC() 
{
	int max = scomp->NumTopBC();
	int* indices = new int[max];
	int num = m_lb_topbcs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSedTopBC EdSedTopBCDlg;
	EdSedTopBCDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSedInps::OnBuDeleteTopBC() 
{
	switch ( MessageBox(aqapp.ini.T(666),            // DelTopBCs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumTopBC();
	int* indices = new int[max];
	int num = m_lb_topbcs.GetSelItems(max,indices);
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
			if ( scomp->DeleteTopBC(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(667),
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


void CEdSedInps::OnSelchangeLbTopBCs() 
{
	UpdateActivation();
	return;
}


void CEdSedInps::OnDblclkLbTopBCs() 
{
	OnBuEditTopBC();
	return;
}


void CEdSedInps::OnBuAddBottomBC() 
{
	int max = scomp->NumBottomBC();
	int* indices = new int[max];
	int num = m_lb_bottombcs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSedBottomBC EdSedBottomBCDlg;
	EdSedBottomBCDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSedInps::OnBuEditBottomBC() 
{
	int max = scomp->NumBottomBC();
	int* indices = new int[max];
	int num = m_lb_bottombcs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSedBottomBC EdSedBottomBCDlg;
	EdSedBottomBCDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSedInps::OnBuDeleteBottomBC() 
{
	switch ( MessageBox(aqapp.ini.T(666),            // DelBottomBCs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumBottomBC();
	int* indices = new int[max];
	int num = m_lb_bottombcs.GetSelItems(max,indices);
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
			if ( scomp->DeleteBottomBC(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(667),
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


void CEdSedInps::OnSelchangeLbBottomBCs() 
{
	UpdateActivation();
	return;
}


void CEdSedInps::OnDblclkLbBottomBCs() 
{
	OnBuEditBottomBC();
	return;
}


void CEdSedInps::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	CARDINAL parseerrors;
	m_ed_surfpor.GetWindowText(buffer1,sizeof(buffer1));
    if ( scomp->SurfPorosity(buffer1,aqsys.Varlist(),buffer2,
		                     parseerrors) == FALSE )
	{
	   MessageBox(aqapp.ini.T(674),     // ParseErrSurfPor
	     		  aqapp.ini.T(111),
				  MB_OK+MB_ICONERROR);
	   return;
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedTopBC dialog


CEdSedTopBC::CEdSedTopBC(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedTopBC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedTopBC)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedTopBC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_CONC, m_ed_conc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedTopBC, CDialog)
	//{{AFX_MSG_MAP(CEdSedTopBC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedTopBC::DoModal(SEDCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedTopBC message handlers

BOOL CEdSedTopBC::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=scomp->NumTopBC())
	{
		aqapp.ProgErr("Illegal TopBC");
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
		int varpos = aqsys.Varlist()->Position(scomp->TopBCVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( scomp->TopBCCorF(pos) != 0 )
		{
			scomp->TopBCCorF(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_conc.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdSedTopBC::OnOK() 
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
	m_ed_conc.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( scomp->AddTopBC(aqsys.Varlist(),var,
					 buffer1,buffer2,parseerrors,
							 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(668),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(669),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceTopBC(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(668),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(670),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedBottomBC dialog


CEdSedBottomBC::CEdSedBottomBC(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedBottomBC::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedBottomBC)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedBottomBC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_CONC, m_ed_conc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedBottomBC, CDialog)
	//{{AFX_MSG_MAP(CEdSedBottomBC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedBottomBC::DoModal(SEDCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedBottomBC message handlers

BOOL CEdSedBottomBC::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=scomp->NumBottomBC())
	{
		aqapp.ProgErr("Illegal BottomBC");
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
		int varpos = aqsys.Varlist()->Position(scomp->BottomBCVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( scomp->BottomBCConc(pos) != 0 )
		{
			scomp->BottomBCConc(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_conc.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdSedBottomBC::OnOK() 
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
	m_ed_conc.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( scomp->AddBottomBC(aqsys.Varlist(),var,
					 buffer1,buffer2,parseerrors,
							 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(668),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(669),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceBottomBC(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(668),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(670),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedParts dialog


CEdSedParts::CEdSedParts(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedParts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedParts)
	//}}AFX_DATA_INIT
}


void CEdSedParts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedParts)
	DDX_Control(pDX, IDC_LB_VARS, m_lb_partlist);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedParts, CDialog)
	//{{AFX_MSG_MAP(CEdSedParts)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_VARS, OnSelchangeLbVars)
	ON_LBN_DBLCLK(IDC_LB_VARS, OnDblclkLbVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedParts::DoModal(SEDCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSedParts::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_partlist.GetTopIndex();
	m_lb_partlist.ResetContent();
	for ( CARDINAL i=0; i<scomp->NumPartVar(); i++ )
	{
		strcpy(buffer,scomp->PartVar(i)->Symbol());
		strcat(buffer," : ");
		scomp->PartRho(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		scomp->PartBioturb(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_partlist.InsertString(-1,buffer);
	}
	m_lb_partlist.SetTopIndex(topindex);
	UpdateActivation();
	return;
}
	
	
void CEdSedParts::UpdateActivation()
{
	switch ( m_lb_partlist.GetSelCount() )
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
// CEdSedParts message handlers


BOOL CEdSedParts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedParts::OnBuAdd() 
{
	int max = scomp->NumPartVar();
	int* indices = new int[max];
	int num = m_lb_partlist.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSedPart EdSedPartDlg;
	EdSedPartDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSedParts::OnBuEdit() 
{
	int max = scomp->NumPartVar();
	int* indices = new int[max];
	int num = m_lb_partlist.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSedPart EdSedPartDlg;
	EdSedPartDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSedParts::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(165),            // DelParts?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumPartVar();
	int* indices = new int[max];
	int num = m_lb_partlist.GetSelItems(max,indices);
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
			if ( scomp->DeletePartVar(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(672),
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


void CEdSedParts::OnSelchangeLbVars() 
{
	UpdateActivation();
	return;
}


void CEdSedParts::OnDblclkLbVars() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedDisss dialog


CEdSedDisss::CEdSedDisss(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedDisss::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedDisss)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedDisss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedDisss)
	DDX_Control(pDX, IDC_LB_VARS, m_lb_disslist);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedDisss, CDialog)
	//{{AFX_MSG_MAP(CEdSedDisss)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_DBLCLK(IDC_LB_VARS, OnDblclkLbVars)
	ON_LBN_SELCHANGE(IDC_LB_VARS, OnSelchangeLbVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedDisss::DoModal(SEDCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSedDisss::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_disslist.GetTopIndex();
	m_lb_disslist.ResetContent();
	for ( CARDINAL i=0; i<scomp->NumDissVar(); i++ )
	{
		strcpy(buffer,scomp->DissVar(i)->Symbol());
		strcat(buffer," : ");
		scomp->Dissdiff(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_disslist.InsertString(-1,buffer);
	}
	m_lb_disslist.SetTopIndex(topindex);
	UpdateActivation();
}


void CEdSedDisss::UpdateActivation()
{
	switch ( m_lb_disslist.GetSelCount() )
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
// CEdSedDisss message handlers

BOOL CEdSedDisss::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedDisss::OnBuAdd() 
{
	int max = scomp->NumDissVar();
	int* indices = new int[max];
	int num = m_lb_disslist.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSedDiss EdSedDissDlg;
	EdSedDissDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
}


void CEdSedDisss::OnBuEdit() 
{
	int max = scomp->NumDissVar();
	int* indices = new int[max];
	int num = m_lb_disslist.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSedDiss EdSedDissDlg;
	EdSedDissDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
}


void CEdSedDisss::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(677),            // DelDisss?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumDissVar();
	int* indices = new int[max];
	int num = m_lb_disslist.GetSelItems(max,indices);
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
			if ( scomp->DeleteDissVar(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(678),
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;
	UpdateDialog();
}


void CEdSedDisss::OnSelchangeLbVars() 
{
	UpdateActivation();
}


void CEdSedDisss::OnDblclkLbVars() 
{
	OnBuEdit();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedPart dialog


CEdSedPart::CEdSedPart(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedPart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedPart)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedPart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedPart)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_BIOTURB, m_ed_bioturb);
	DDX_Control(pDX, IDC_ED_DENSITY, m_ed_density);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedPart, CDialog)
	//{{AFX_MSG_MAP(CEdSedPart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedPart::DoModal(SEDCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEdSedPart message handlers

BOOL CEdSedPart::OnInitDialog() 
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
	if ( add==FALSE && pos>=scomp->NumPartVar())
	{
		aqapp.ProgErr("Illegal ");
	}
	else
	{
		if ( add == FALSE )
		{
			int varpos = aqsys.Varlist()->Position(scomp->PartVar(pos));
			if ( varpos <= 0 )
			{
				aqapp.ProgErr("Variable not found");
			}
			else
			{
				m_le_var.SetCurSel(varpos-1);
			}
			if ( scomp->PartRho(pos) != 0 )
			{
				scomp->PartRho(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_density.SetWindowText(buffer);
			}
			if ( scomp->PartBioturb(pos) != 0 )
			{
				scomp->PartBioturb(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_bioturb.SetWindowText(buffer);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedPart::OnOK() 
{
	char buffer1[1024], buffer2[1024], 
         buffer3[1024], buffer4[1024];
	AQVAR* var;
	int  i;
	CARDINAL errorsrho, errorsbioturb;

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
	m_ed_density.GetWindowText(buffer1,sizeof(buffer1));
	m_ed_bioturb.GetWindowText(buffer3,sizeof(buffer3));
	if ( add == TRUE )
	{
		if ( scomp->AddPartVar(aqsys.Varlist(),var,
                               buffer1,buffer2,errorsrho,
                               buffer3,buffer4,errorsbioturb,
							   pos) == FALSE )
		{
			if ( errorsrho > 0 )
			{
			   MessageBox(aqapp.ini.T(673),       // ParseErrRho
						  aqapp.ini.T(111),
						  MB_OK+MB_ICONERROR);
			}
			else
			{
     		   if ( errorsbioturb > 0 )
			   {
                  MessageBox(aqapp.ini.T(688),    // ParseErrBioturb
                             aqapp.ini.T(111),
                             MB_OK+MB_ICONERROR);
			   }
			   else
               {
				  MessageBox(aqapp.ini.T(675),    // UnableAddPart
                             aqapp.ini.T(111),
                              MB_OK+MB_ICONERROR);
			   }
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplacePartVar(aqsys.Varlist(),var,
					       buffer1,buffer2,errorsrho,
							       buffer3,buffer4,errorsbioturb,
							       pos) == FALSE )
		{
			if ( errorsrho > 0 )
			{
				MessageBox(aqapp.ini.T(673),     // ParseErrRho
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
			   if ( errorsbioturb > 0 )
			   {
                  MessageBox(aqapp.ini.T(688), // ParseErrBioturb
                             aqapp.ini.T(111),
                             MB_OK+MB_ICONERROR);
			   }
			   else
               {
                  MessageBox(aqapp.ini.T(676), // IllegalPart
                             aqapp.ini.T(111),
                             MB_OK+MB_ICONERROR);
			   }
			}
			return;
		}
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedDiss dialog


CEdSedDiss::CEdSedDiss(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedDiss::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedDiss)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSedDiss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedDiss)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_SEDEXCH, m_ed_sedexch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedDiss, CDialog)
	//{{AFX_MSG_MAP(CEdSedDiss)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedDiss::DoModal(SEDCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedDiss message handlers

BOOL CEdSedDiss::OnInitDialog() 
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
	if ( add==FALSE && pos>=scomp->NumDissVar())
	{
		aqapp.ProgErr("Illegal Variable");
	}
	else
	{
		if ( add == FALSE )
		{
			int varpos = aqsys.Varlist()->Position(scomp->DissVar(pos));
			if ( varpos <= 0 )
			{
				aqapp.ProgErr("Variable not found");
			}
			else
			{
				m_le_var.SetCurSel(varpos-1);
			}
			if ( scomp->Dissdiff(pos) != 0 )
			{
				scomp->Dissdiff(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_sedexch.SetWindowText(buffer);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedDiss::OnOK() 
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
	m_ed_sedexch.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( scomp->AddDissVar(aqsys.Varlist(),var,
					   buffer1,buffer2,parseerrors,
							   pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(679),     // ParseErrDiff
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(680),     // UnableAddDiss
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceDissVar(aqsys.Varlist(),var,
					       buffer1,buffer2,parseerrors,
							       pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(679),     // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(681),     // IllegalDiss
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedAcc dialog


CEdSedAcc::CEdSedAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSedAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSedAcc)
	//}}AFX_DATA_INIT
}


void CEdSedAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSedAcc)
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	DDX_Control(pDX, IDC_ED_RELACCPOR, m_ed_relaccpor);
	DDX_Control(pDX, IDC_ED_ABSACCPOR, m_ed_absaccpor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSedAcc, CDialog)
	//{{AFX_MSG_MAP(CEdSedAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSedAcc::DoModal(SEDCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSedAcc message handlers


BOOL CEdSedAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	NumToString(scomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(scomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(scomp->RelAccPor(),buffer,sizeof(buffer));
	m_ed_relaccpor.SetWindowText(buffer);
	NumToString(scomp->AbsAccPor(),buffer,sizeof(buffer));
	m_ed_absaccpor.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSedAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(682),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(683),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(684),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccpor.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(685),  // FailRelAccPor
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccpor.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(686),  // FailAbsAccPor
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->AccPor(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(687),  // FailAccPor
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	

	
	CDialog::OnOK();
}




