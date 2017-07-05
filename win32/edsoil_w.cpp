// edsoil_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edsoil_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static int inputtype = 0;

static int EditInputs(SOILCOMP* com);

/////////////////////////////////////////////////////////////////////////////
// CEdSoil dialog


CEdSoil::CEdSoil(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoil::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoil)
	//}}AFX_DATA_INIT
}


void CEdSoil::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoil)
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_BU_ACC, m_bu_acc);
	DDX_Control(pDX, IDC_RB_WITHDIFF, m_rb_withdisp);
	DDX_Control(pDX, IDC_RB_WITHOUTDIFF, m_rb_withoutdisp);
	DDX_Control(pDX, IDC_ED_DIFF, m_ed_disp);
	DDX_Control(pDX, IDC_ED_CROSSSECT, m_ed_crosssect);
	DDX_Control(pDX, IDC_ED_START, m_ed_start);
	DDX_Control(pDX, IDC_ED_END, m_ed_end);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD2, m_bu_add);
	DDX_Control(pDX, IDC_ED_NUMGRID, m_ed_numgrid);
	DDX_Control(pDX, IDC_LB_PARZONES, m_lb_parzones);
	DDX_Control(pDX, IDC_RB_LOWRES, m_rb_lowres);
	DDX_Control(pDX, IDC_RB_HIGHRES, m_rb_highres);
	DDX_Control(pDX, IDC_ED_ADVVOLFRACT, m_ed_advvolfract);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	DDX_Control(pDX, IDC_BU_VARS, m_bu_vars);
	DDX_Control(pDX, IDC_BU_PROCS, m_bu_procs);
	DDX_Control(pDX, IDC_BU_INP, m_bu_inp);
	DDX_Control(pDX, IDC_BU_INITS, m_bu_inits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoil, CDialog)
	//{{AFX_MSG_MAP(CEdSoil)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_RB_WITHDIFF, OnRbWithdiff)
	ON_BN_CLICKED(IDC_RB_WITHOUTDIFF, OnRbWithoutdiff)
	ON_BN_CLICKED(IDC_BU_ADD2, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_PARZONES, OnSelchangeLbParzones)
	ON_LBN_DBLCLK(IDC_LB_PARZONES, OnDblclkLbParzones)
	ON_BN_CLICKED(IDC_RB_LOWRES, OnRbLowres)
	ON_BN_CLICKED(IDC_RB_HIGHRES, OnRbHighres)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoil::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	scomp = new SOILCOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


void CEdSoil::UpdateActivation()
{
	switch ( m_lb_parzones.GetSelCount() )
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


void CEdSoil::UpdateDialog()
{
	int topindex = m_lb_parzones.GetTopIndex();
	m_lb_parzones.ResetContent();
	PARZONE* zone = scomp->ParZoneList()->First();
	while ( zone != 0 )
	{
	   m_lb_parzones.InsertString(-1,zone->Symbol());
	   zone = zone->Next();
	}
	m_lb_parzones.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoil message handlers

BOOL CEdSoil::OnInitDialog() 
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

	NumToString(scomp->XStart(),buffer,sizeof(buffer));
	m_ed_start.SetWindowText(buffer);

	NumToString(scomp->XEnd(),buffer,sizeof(buffer));
	m_ed_end.SetWindowText(buffer);
	
	if ( scomp->CrossSect() != 0 )
	{
		if ( scomp->CrossSect()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_crosssect.SetWindowText(buffer);
		}
	}
	
	if ( scomp->AdvVolFract() != 0 )
	{
		if ( scomp->AdvVolFract()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_advvolfract.SetWindowText(buffer);
		}
	}
	
	withdispersion = scomp->WithDispersion();
	if ( scomp->Dispersion() != 0 )
	{
		if ( scomp->Dispersion()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_disp.SetWindowText(buffer);
		}
	}
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

	UpdateDialog();

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


void CEdSoil::OnBuVars() 
{
	SOILCOMP* tempcomp = new SOILCOMP(scomp);
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


void CEdSoil::OnBuProcs() 
{
	SOILCOMP* tempcomp = new SOILCOMP(scomp);
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


void CEdSoil::OnBuInits() 
{
	SOILCOMP* tempcomp = new SOILCOMP(scomp);
	CEdSoilInits EdSoilInitsDlg;
	if ( EdSoilInitsDlg.DoModal(tempcomp) == IDOK )
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


void CEdSoil::OnBuInp() 
{
	SOILCOMP* tempcomp = new SOILCOMP(scomp);
	if ( EditInputs(tempcomp) == IDOK )
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


void CEdSoil::OnCancel() 
{
	delete scomp;
	
	CDialog::OnCancel();
}


void CEdSoil::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r1, r2; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(434),  // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	scomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(435), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	scomp->CmpIndex(c);

	m_ed_start.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(436), // FailStart
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_end.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(437), // FailEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->XStartEnd(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(438), // IllegalStartEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_crosssect.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->CrossSect(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(439), // ParseErrCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(440), // IllegalCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_advvolfract.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->AdvVolFract(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(441), // ParseErrCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(442), // IllegalCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_disp.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->Dispersion(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( withdispersion == TRUE )
		{
			if ( numerr > 0 )
			{
				MessageBox(aqapp.ini.T(443), // ParseErrDisp
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			else
			{
				MessageBox(aqapp.ini.T(444), // IllegalDisp
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( scomp->WithDispersion(withdispersion) == FALSE )
	{
		aqapp.ProgErr("Unable to set dispersion type");
	}

	m_ed_numgrid.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(445), // FailNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->NumGridPts(c) == FALSE )
	{
		MessageBox(aqapp.ini.T(446), // IllegalNumGrid
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
			MessageBox(aqapp.ini.T(447),  // UnableAddComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,scomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(448),   // IllegalComp
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdSoil::OnRbWithoutdiff() 
{
	withdispersion = FALSE;
	m_ed_disp.EnableWindow(FALSE);
	return;
}


void CEdSoil::OnRbWithdiff() 
{
	withdispersion = TRUE;
	m_ed_disp.EnableWindow(TRUE);
	return;
}


void CEdSoil::OnBuAdd() 
{
	CEdSoilParZone ParZoneDlg;
	ParZoneDlg.DoModal(scomp,0);
	UpdateDialog();
	return; 
}


void CEdSoil::OnBuEdit() 
{
	int max = scomp->ParZoneList()->Size();
	int* indices = new int[max];
	int num = m_lb_parzones.GetSelItems(max,indices);
	int zonenum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	PARZONE* parzone = scomp->ParZoneList()->Get(zonenum);
	if ( parzone == 0 )
	{
		aqapp.ProgErr("Parallel zone not found");
		return;
	}
	CEdSoilParZone ParZoneDlg;
	ParZoneDlg.DoModal(scomp,parzone);

	UpdateDialog();
	return; 
}


void CEdSoil::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(449),  // DelZones?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->ParZoneList()->Size();
	int* indices = new int[max];
	int num = m_lb_parzones.GetSelItems(max,indices);
	int j,k;
	PARZONE* parzone;
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
			parzone = scomp->ParZoneList()->Get(indices[j]-k+1);
			if ( scomp->DeleteParZone(parzone) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(450)); // FailDelZone
				strcat(buffer," ");
				strcat(buffer,parzone->Symbol());
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


void CEdSoil::OnSelchangeLbParzones() 
{
	UpdateActivation();
	return;
}


void CEdSoil::OnDblclkLbParzones() 
{
	OnBuEdit();
	return;
}


void CEdSoil::OnRbLowres() 
{
	scomp->HighRes(FALSE);
	return;
}


void CEdSoil::OnRbHighres() 
{
	scomp->HighRes(TRUE);
	return;
}


void CEdSoil::OnBuAcc() 
{
	SOILCOMP* tempcomp = new SOILCOMP(scomp);
	CEdSoilAcc EdSoilAccDlg;
	if ( EdSoilAccDlg.DoModal(tempcomp) == IDOK )
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
// CEdSoilInits dialog


CEdSoilInits::CEdSoilInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilInits, CDialog)
	//{{AFX_MSG_MAP(CEdSoilInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilInits::DoModal(SOILCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSoilInits::UpdateDialog()
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


void CEdSoilInits::UpdateActivation()
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
// CEdSoilInits message handlers


BOOL CEdSoilInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilInits::OnBuAdd() 
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
	CEdSoilInit EdSoilInitDlg;
	EdSoilInitDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSoilInits::OnBuEdit() 
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
	CEdSoilInit EdSoilInitDlg;
	EdSoilInitDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSoilInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(451),       // DelInits?
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
				MessageBox(aqapp.ini.T(452),  // FailDelIni
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


void CEdSoilInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;	
}


void CEdSoilInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInit dialog


CEdSoilInit::CEdSoilInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilInit, CDialog)
	//{{AFX_MSG_MAP(CEdSoilInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilInit::DoModal(SOILCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInit message handlers

BOOL CEdSoilInit::OnInitDialog() 
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


void CEdSoilInit::OnOK() 
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
				MessageBox(aqapp.ini.T(453),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(454),     // UnableAddInit
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
				MessageBox(aqapp.ini.T(453),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(455),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////


static int EditInputs(SOILCOMP* com)
{
	CEdSoilSelInp EdSoilSelInpDlg;
	if ( EdSoilSelInpDlg.DoModal(com) == IDOK )
	{
		switch ( inputtype )
		{
		case 0:
			{
				CEdSoilInInps EdSoilInInpsDlg;
				return EdSoilInInpsDlg.DoModal(com);
			}
		case 1:
			{
				CEdSoilLatInps EdSoilLatInpsDlg;
				return EdSoilLatInpsDlg.DoModal(com);
			}
		default:
			{
				CEdSoilInInps EdSoilInInpsDlg;
				return EdSoilInInpsDlg.DoModal(com);
			}
		}
	}
	return IDCANCEL;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilSelInp dialog


CEdSoilSelInp::CEdSoilSelInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilSelInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilSelInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilSelInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilSelInp)
	DDX_Control(pDX, IDC_RB_LATINP, m_rb_latinp);
	DDX_Control(pDX, IDC_RB_INLETINP, m_rb_inletinp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilSelInp, CDialog)
	//{{AFX_MSG_MAP(CEdSoilSelInp)
	ON_BN_CLICKED(IDC_RB_INLETINP, OnRbInletinp)
	ON_BN_CLICKED(IDC_RB_LATINP, OnRbLatinp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilSelInp::DoModal(SOILCOMP* com)
{
	scomp = com;

	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilSelInp message handlers

BOOL CEdSoilSelInp::OnInitDialog() 
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


void CEdSoilSelInp::OnRbInletinp() 
{
	inputtype = 0;
	return;
}


void CEdSoilSelInp::OnRbLatinp() 
{
	inputtype = 1;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInInps dialog


CEdSoilInInps::CEdSoilInInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilInInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilInInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilInInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilInInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilInInps, CDialog)
	//{{AFX_MSG_MAP(CEdSoilInInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilInInps::DoModal(SOILCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSoilInInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<scomp->NumInput(); i++ )
	{
		strcpy(buffer,scomp->InputVar(i)->Symbol());
		strcat(buffer," : ");
		scomp->InputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdSoilInInps::UpdateActivation()
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
// CEdSoilInInps message handlers

BOOL CEdSoilInInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( scomp->Inflow() != 0 )
	{
		if ( scomp->Inflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilInInps::OnBuAdd() 
{
	int max = scomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSoilInInp EdSoilInInpDlg;
	EdSoilInInpDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSoilInInps::OnBuEdit() 
{
	int max = scomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSoilInInp EdSoilInInpDlg;
	EdSoilInInpDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSoilInInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(456),            // DelInInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumInput();
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
			if ( scomp->DeleteInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(457),  // FailDelInp
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


void CEdSoilInInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdSoilInInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdSoilInInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->Inflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(458), // IllegalInInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInInp dialog


CEdSoilInInp::CEdSoilInInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilInInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilInInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilInInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilInInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilInInp, CDialog)
	//{{AFX_MSG_MAP(CEdSoilInInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilInInp::DoModal(SOILCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInInp message handlers


BOOL CEdSoilInInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=scomp->NumInput())
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
		int varpos = aqsys.Varlist()->Position(scomp->InputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( scomp->InputFlux(pos) != 0 )
		{
			scomp->InputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilInInp::OnOK() 
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
		if ( scomp->AddInput(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(459),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(460),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceInput(aqsys.Varlist(),var,
						 buffer1,buffer2,parseerrors,
								 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(459),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(461),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilLatInps dialog


CEdSoilLatInps::CEdSoilLatInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilLatInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilLatInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilLatInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilLatInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilLatInps, CDialog)
	//{{AFX_MSG_MAP(CEdSoilLatInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilLatInps::DoModal(SOILCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


void CEdSoilLatInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<scomp->NumLatInput(); i++ )
	{
		strcpy(buffer,scomp->LatInputVar(i)->Symbol());
		strcat(buffer," : ");
		scomp->LatInputConc(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdSoilLatInps::UpdateActivation()
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
// CEdSoilLatInps message handlers


BOOL CEdSoilLatInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( scomp->LatInflow() != 0 )
	{
		if ( scomp->LatInflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilLatInps::OnBuAdd() 
{
	int max = scomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSoilLatInp EdSoilLatInpDlg;
	EdSoilLatInpDlg.DoModal(scomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSoilLatInps::OnBuEdit() 
{
	int max = scomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSoilLatInp EdSoilLatInpDlg;
	EdSoilLatInpDlg.DoModal(scomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSoilLatInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(462),       // DelLatInputs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = scomp->NumLatInput();
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
			if ( scomp->DeleteLatInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(463),  // FailDelInp
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


void CEdSoilLatInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdSoilLatInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdSoilLatInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( scomp->LatInflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(464), // IllegalLatInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilLatInp dialog


CEdSoilLatInp::CEdSoilLatInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilLatInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilLatInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilLatInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilLatInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilLatInp, CDialog)
	//{{AFX_MSG_MAP(CEdSoilLatInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilLatInp::DoModal(SOILCOMP* com, BOOL a, int i)
{
	scomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilLatInp message handlers

BOOL CEdSoilLatInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=scomp->NumLatInput())
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
		int varpos = aqsys.Varlist()->Position(scomp->LatInputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( scomp->LatInputConc(pos) != 0 )
		{
			scomp->LatInputConc(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilLatInp::OnOK() 
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
		if ( scomp->AddLatInput(aqsys.Varlist(),var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(465),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(466),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceLatInput(aqsys.Varlist(),var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(465),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(465),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilParZone dialog


CEdSoilParZone::CEdSoilParZone(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilParZone::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilParZone)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilParZone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilParZone)
	DDX_Control(pDX, IDC_LB_SERZONES, m_lb_serzones);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilParZone, CDialog)
	//{{AFX_MSG_MAP(CEdSoilParZone)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_SERZONES, OnSelchangeLbSerzones)
	ON_LBN_DBLCLK(IDC_LB_SERZONES, OnDblclkLbSerzones)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilParZone::DoModal(SOILCOMP* com, PARZONE* zone)
{
	scomp = com;
	oldzone = zone;
	parzone = new PARZONE(oldzone);
	return CDialog::DoModal();
}


void CEdSoilParZone::UpdateActivation()
{
	switch ( m_lb_serzones.GetSelCount() )
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


void CEdSoilParZone::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];

	int topindex = m_lb_serzones.GetTopIndex();
	m_lb_serzones.ResetContent();
	for ( CARDINAL i=0; i<parzone->NumSerialZones(); i++ )
	{
		const SERZONE* serzone = parzone->SerialZone(i);
		NumToString(serzone->ZoneIndex(),buffer,sizeof(buffer));
		(serzone->VolumeFract())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,":  ");
		strcat(buffer,buffer1);
		m_lb_serzones.InsertString(-1,buffer);
	}
	m_lb_serzones.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilParZone message handlers

BOOL CEdSoilParZone::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( parzone->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(parzone->Symbol());
	}

	if ( parzone->Description() != 0 )
	{
		m_ed_descript.SetWindowText(parzone->Description());
	}

	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilParZone::OnBuAdd() 
{
	int max = parzone->NumSerialZones();
	int* indices = new int[max];
	int num = m_lb_serzones.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSoilSerZone EdSerZoneDlg;
	EdSerZoneDlg.DoModal(parzone,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSoilParZone::OnBuEdit() 
{
	int max = parzone->NumSerialZones();
	int* indices = new int[max];
	int num = m_lb_serzones.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSoilSerZone EdSerZoneDlg;
	EdSerZoneDlg.DoModal(parzone,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSoilParZone::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(468),  // DelZones?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = parzone->NumSerialZones();
	int* indices = new int[max];
	int num = m_lb_serzones.GetSelItems(max,indices);
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
			if ( parzone->DeleteSerial(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(469),   // FailDelZone
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


void CEdSoilParZone::OnSelchangeLbSerzones() 
{
	UpdateActivation();
	return;
}


void CEdSoilParZone::OnDblclkLbSerzones() 
{
	OnBuEdit();
	return;
}


void CEdSoilParZone::OnCancel() 
{
	delete parzone;
	
	CDialog::OnCancel();
}


void CEdSoilParZone::OnOK() 
{
	char buffer[024];
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( parzone->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(470),  // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	parzone->Description(buffer);

	if ( oldzone == 0 )
	{
		if ( scomp->AddParZone(parzone) == FALSE )
		{
			MessageBox(aqapp.ini.T(471),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( scomp->ReplaceParZone(oldzone,parzone) == FALSE )
		{
			MessageBox(aqapp.ini.T(472),  // IllegalZone
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilSerZone dialog


CEdSoilSerZone::CEdSoilSerZone(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilSerZone::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilSerZone)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilSerZone::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilSerZone)
	DDX_Control(pDX, IDC_LB_EXCHPARS, m_lb_exchpars);
	DDX_Control(pDX, IDC_ED_ZONEIND, m_ed_zoneind);
	DDX_Control(pDX, IDC_ED_VOLFRACT, m_ed_volfract);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilSerZone, CDialog)
	//{{AFX_MSG_MAP(CEdSoilSerZone)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_EXCHPARS, OnSelchangeLbExchpars)
	ON_LBN_DBLCLK(IDC_LB_EXCHPARS, OnDblclkLbExchpars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilSerZone::DoModal(PARZONE* zone, BOOL a, int i)
{
	parzone = zone;
	add     = a;
	pos     = i;
	if ( add == TRUE )
	{
		serzone = new SERZONE;
	}
	else
	{
		serzone = new SERZONE(parzone->SerialZone(pos));
	}
	return CDialog::DoModal();
}


void CEdSoilSerZone::UpdateActivation()
{
	switch ( m_lb_exchpars.GetSelCount() )
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


void CEdSoilSerZone::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];

	int topindex = m_lb_exchpars.GetTopIndex();
	m_lb_exchpars.ResetContent();
	for ( CARDINAL i=0; i<serzone->NumExch(); i++ )
	{
		strcpy(buffer,(serzone->ExchVar(i))->Symbol());
		strcat(buffer,":  ");
		(serzone->ExchVal(i))->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		(serzone->ExchFac(i))->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_exchpars.InsertString(-1,buffer);
	}
	m_lb_exchpars.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilSerZone message handlers

BOOL CEdSoilSerZone::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	NumToString(serzone->ZoneIndex(),buffer,sizeof(buffer));
	m_ed_zoneind.SetWindowText(buffer);

	if ( serzone->VolumeFract() != 0 )
	{
		if ( serzone->VolumeFract()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_volfract.SetWindowText(buffer);
		}
	}

	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilSerZone::OnBuAdd() 
{
	int max = serzone->NumExch();
	int* indices = new int[max];
	int num = m_lb_exchpars.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdSoilExchCoeff EdSoilExchCoeffDlg;
	EdSoilExchCoeffDlg.DoModal(serzone,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdSoilSerZone::OnBuEdit() 
{
	int max = serzone->NumExch();
	int* indices = new int[max];
	int num = m_lb_exchpars.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdSoilExchCoeff EdSoilExchCoeffDlg;
	EdSoilExchCoeffDlg.DoModal(serzone,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdSoilSerZone::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(473),  // DelExch?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = serzone->NumExch();
	int* indices = new int[max];
	int num = m_lb_exchpars.GetSelItems(max,indices);
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
			if ( serzone->DeleteExchCoeff(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,"Unable to delete Exchange Coefficient"); // FailDelExch
			}
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
	return; 
}


void CEdSoilSerZone::OnSelchangeLbExchpars() 
{
	UpdateActivation();
	return;
}


void CEdSoilSerZone::OnDblclkLbExchpars() 
{
	OnBuEdit();
	return;
}


void CEdSoilSerZone::OnCancel() 
{
	delete serzone;
	
	CDialog::OnCancel();
}


void CEdSoilSerZone::OnOK() 
{
	char buffer[1024]; char buffer1[1024]; CARDINAL c,numerr;

	m_ed_zoneind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(474), // FailZoneInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	serzone->ZoneIndex(c);

	m_ed_volfract.GetWindowText(buffer,sizeof(buffer));
	if ( serzone->VolumeFract(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(475), // ParseErrVolFract
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(476), // IllegalVolFract
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( add == TRUE )
	{
		if ( parzone->AddSerial(serzone,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(477), // FailAddZone
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( parzone->ReplaceSerial(serzone,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(478), // IllegalZone
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilExchCoeff dialog


CEdSoilExchCoeff::CEdSoilExchCoeff(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilExchCoeff::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilExchCoeff)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilExchCoeff::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilExchCoeff)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_EXCHCOEFF, m_ed_exchcoeff);
	DDX_Control(pDX, IDC_ED_CONVFACT, m_ed_convfact);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilExchCoeff, CDialog)
	//{{AFX_MSG_MAP(CEdSoilExchCoeff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilExchCoeff::DoModal(SERZONE* zone, BOOL a, int i)
{
	serzone = zone;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilExchCoeff message handlers

BOOL CEdSoilExchCoeff::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=serzone->NumExch())
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
		m_ed_convfact.SetWindowText("1");
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(serzone->ExchVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( serzone->ExchVal(pos) != 0 )
		{
			serzone->ExchVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_exchcoeff.SetWindowText(buffer);
		}
		if ( serzone->ExchFac(pos) != 0 )
		{
			serzone->ExchFac(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_convfact.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdSoilExchCoeff::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	char buffer3[1024], buffer4[1024];
	AQVAR* var;
	int  i;
	CARDINAL errorsexch, errorsconv;

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

	m_ed_exchcoeff.GetWindowText(buffer1,sizeof(buffer1));
	m_ed_convfact.GetWindowText(buffer3,sizeof(buffer3));
	if ( add == TRUE )
	{
		if ( serzone->AddExchCoeff(aqsys.Varlist(),var,
					    buffer1,buffer2,errorsexch,
					    buffer3,buffer4,errorsconv,
							    pos) == FALSE )
		{
			if ( errorsexch > 0 )
			{
				MessageBox(aqapp.ini.T(479),     // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				if ( errorsconv > 0 )
				{
					MessageBox(aqapp.ini.T(480),     // ParseErrConv
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
				else
				{
					MessageBox(aqapp.ini.T(481),     // UnableAddExch
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
			}
			return;
		}
	}
	else
	{
		if ( serzone->ReplaceExchCoeff(aqsys.Varlist(),var,
					    buffer1,buffer2,errorsexch,
					    buffer3,buffer4,errorsconv,
								pos) == FALSE )
		{
			if ( errorsexch > 0 )
			{
				MessageBox(aqapp.ini.T(479),     // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				if ( errorsconv > 0 )
				{
					MessageBox(aqapp.ini.T(480),     // ParseErrConv
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
				else
				{
					MessageBox(aqapp.ini.T(482),     // IllegalExch
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
// CEdSoilAcc dialog


CEdSoilAcc::CEdSoilAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdSoilAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdSoilAcc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdSoilAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdSoilAcc)
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_RELACCD, m_ed_relaccd);
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	DDX_Control(pDX, IDC_ED_ABSACCD, m_ed_absaccd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdSoilAcc, CDialog)
	//{{AFX_MSG_MAP(CEdSoilAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdSoilAcc::DoModal(SOILCOMP* com)
{
	scomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdSoilAcc message handlers

BOOL CEdSoilAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	NumToString(scomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(scomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(scomp->RelAccD(),buffer,sizeof(buffer));
	m_ed_relaccd.SetWindowText(buffer);
	NumToString(scomp->AbsAccD(),buffer,sizeof(buffer));
	m_ed_absaccd.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdSoilAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(607),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(608),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(609),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccd.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(610),  // FailRelAccD
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccd.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(611),  // FailRelAccD
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scomp->AccD(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(612),  // FailAccD
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}

