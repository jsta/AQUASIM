// edmix_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edmix_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdMix dialog


CEdMix::CEdMix(CWnd* pParent /*=NULL*/)
	: CDialog(CEdMix::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdMix)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdMix::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdMix)
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_TX_VOL, m_tx_vol);
	DDX_Control(pDX, IDC_TX_OUTFLOW, m_tx_outflow);
	DDX_Control(pDX, IDC_RB_CONSTVOL, m_rb_constvol);
	DDX_Control(pDX, IDC_RB_VARVOL, m_rb_varvol);
	DDX_Control(pDX, IDC_ED_VOL, m_ed_vol);
	DDX_Control(pDX, IDC_ED_OUTFLOW, m_ed_outflow);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdMix, CDialog)
	//{{AFX_MSG_MAP(CEdMix)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_RB_CONSTVOL, OnRbConstvol)
	ON_BN_CLICKED(IDC_RB_VARVOL, OnRbVarvol)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdMix::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	mcomp = new MIXCOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdMix message handlers

BOOL CEdMix::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( mcomp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(mcomp->Symbol());
	}

	if ( mcomp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(mcomp->Description());
	}

	NumToString(mcomp->CmpIndex(),buffer,sizeof(buffer));
	m_ed_compind.SetWindowText(buffer);
	
	NumToString(mcomp->Vol(),buffer,sizeof(buffer));
	m_ed_vol.SetWindowText(buffer);

	if ( mcomp->Outflow() != 0 )
	{
		if ( mcomp->Outflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_outflow.SetWindowText(buffer);
		}
	}

	if ( mcomp->FixedVol() == TRUE )
	{
		m_rb_constvol.SetCheck(1);
		m_tx_outflow.EnableWindow(FALSE);
		m_ed_outflow.EnableWindow(FALSE);
	}
	else
	{
		m_rb_varvol.SetCheck(1);
		m_tx_vol.EnableWindow(FALSE);
		m_ed_vol.EnableWindow(FALSE);
	}

	if ( mcomp->Active() == TRUE )
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


void CEdMix::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( mcomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(176),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	mcomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(177), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	mcomp->CmpIndex(c);

	m_ed_vol.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r) == FALSE )
	{
		if ( m_rb_constvol.GetCheck() == 1 )
		{
			MessageBox(aqapp.ini.T(178), // FailVol
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( mcomp->Vol(r) == FALSE )
		{
			if ( m_rb_constvol.GetCheck() == 1 )
			{
				MessageBox(aqapp.ini.T(179), // IllegalVol
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( m_rb_constvol.GetCheck() == 1 ) mcomp->FixedVol(TRUE);

	m_ed_outflow.GetWindowText(buffer,sizeof(buffer));
	if ( mcomp->Outflow(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( m_rb_varvol.GetCheck() == 1 )
		{
			if ( numerr > 0 )
			{
				MessageBox(aqapp.ini.T(180), // ParseErrOutflow
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			else
			{
				MessageBox(aqapp.ini.T(181), // IllegalOutflow
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
	}
	if ( m_rb_varvol.GetCheck() == 1 ) mcomp->FixedVol(FALSE);

	if ( m_cb_active.GetCheck() ) mcomp->Active(TRUE);
    else                          mcomp->Active(FALSE);

	if ( add == TRUE )
	{
		if ( aqsys.AddComp(mcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(182),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,mcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(183),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdMix::OnCancel() 
{
	delete mcomp;
	
	CDialog::OnCancel();
}


void CEdMix::OnBuVars() 
{
	MIXCOMP* tempcomp = new MIXCOMP(mcomp);
	CEdCompActVar EdCompActVarDlg;
	if ( EdCompActVarDlg.DoModal(tempcomp) == IDOK )
	{
		delete mcomp;
		mcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdMix::OnBuProcs() 
{
	MIXCOMP* tempcomp = new MIXCOMP(mcomp);
	CEdCompActProc EdCompActProcDlg;
	if ( EdCompActProcDlg.DoModal(tempcomp) == IDOK )
	{
		delete mcomp;
		mcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdMix::OnBuInits() 
{
	MIXCOMP* tempcomp = new MIXCOMP(mcomp);
	CEdMixInits EdMixInitsDlg;
	if ( EdMixInitsDlg.DoModal(tempcomp) == IDOK )
	{
		delete mcomp;
		mcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdMix::OnBuInp() 
{
	MIXCOMP* tempcomp = new MIXCOMP(mcomp);
	CEdMixInps EdMixInpsDlg;
	if ( EdMixInpsDlg.DoModal(tempcomp) == IDOK )
	{
		delete mcomp;
		mcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdMix::OnRbConstvol() 
{
	m_tx_vol.EnableWindow(TRUE);
	m_ed_vol.EnableWindow(TRUE);
	m_tx_outflow.EnableWindow(FALSE);
	m_ed_outflow.EnableWindow(FALSE);
}


void CEdMix::OnRbVarvol() 
{
	m_tx_vol.EnableWindow(FALSE);
	m_ed_vol.EnableWindow(FALSE);
	m_tx_outflow.EnableWindow(TRUE);
	m_ed_outflow.EnableWindow(TRUE);
}


void CEdMix::OnBuAcc() 
{
	MIXCOMP* tempcomp = new MIXCOMP(mcomp);
	CEdMixAcc EdMixAccDlg;
	if ( EdMixAccDlg.DoModal(tempcomp) == IDOK )
	{
		delete mcomp;
		mcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixInits dialog


CEdMixInits::CEdMixInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdMixInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdMixInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdMixInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdMixInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdMixInits, CDialog)
	//{{AFX_MSG_MAP(CEdMixInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdMixInits::DoModal(MIXCOMP* com)
{
	mcomp = com;
	return CDialog::DoModal();
}


void CEdMixInits::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inits.GetTopIndex();
	m_lb_inits.ResetContent();
	for ( CARDINAL i=0; i<mcomp->NumInitCond(); i++ )
	{
		strcpy(buffer,mcomp->InitVar(i)->Symbol());
		strcat(buffer,"(");
		strcat(buffer,mcomp->ZoneName(mcomp->InitZone(i)));
		strcat(buffer,") : ");
		mcomp->InitVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inits.InsertString(-1,buffer);
	}
	m_lb_inits.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdMixInits::UpdateActivation()
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
// CEdMixInits message handlers

BOOL CEdMixInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdMixInits::OnBuAdd() 
{
	int max = mcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdMixInit EdMixInitDlg;
	EdMixInitDlg.DoModal(mcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdMixInits::OnBuEdit() 
{
	int max = mcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdMixInit EdMixInitDlg;
	EdMixInitDlg.DoModal(mcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdMixInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(184),            // DelInits?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = mcomp->NumInitCond();
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
			if ( mcomp->DeleteInitCond(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(185),
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


void CEdMixInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;
}


void CEdMixInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixInit dialog


CEdMixInit::CEdMixInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdMixInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdMixInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdMixInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdMixInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdMixInit, CDialog)
	//{{AFX_MSG_MAP(CEdMixInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdMixInit::DoModal(MIXCOMP* com, BOOL a, int i)
{
	mcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixInit message handlers

BOOL CEdMixInit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=mcomp->NumInitCond())
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
	for ( CARDINAL i=0; i<mcomp->NumZones(); i++ )
	{
		m_le_zone.AddString(mcomp->ZoneName(i));
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
		m_le_zone.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(mcomp->InitVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		m_le_zone.SetCurSel(mcomp->InitZone(pos));
		if ( mcomp->InitVal(pos) != 0 )
		{
			mcomp->InitVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_initcond.SetWindowText(buffer);
		}
	}
	
	if ( mcomp->NumZones() == 1 )
	{
		m_tx_zone.EnableWindow(FALSE);
		m_le_zone.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdMixInit::OnOK() 
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
		if ( mcomp->AddInitCond(aqsys.Varlist(),i,var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(186),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(187),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( mcomp->ReplaceInitCond(aqsys.Varlist(),i,var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(186),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(188),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixInps dialog


CEdMixInps::CEdMixInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdMixInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdMixInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdMixInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdMixInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdMixInps, CDialog)
	//{{AFX_MSG_MAP(CEdMixInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdMixInps::DoModal(MIXCOMP* com)
{
	mcomp = com;
	return CDialog::DoModal();
}


void CEdMixInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<mcomp->NumInput(); i++ )
	{
		strcpy(buffer,mcomp->InputVar(i)->Symbol());
		strcat(buffer," : ");
		mcomp->InputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdMixInps::UpdateActivation()
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
// CEdMixInps message handlers

BOOL CEdMixInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( mcomp->Inflow() != 0 )
	{
		if ( mcomp->Inflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdMixInps::OnBuAdd() 
{
	int max = mcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdMixInp EdMixInpDlg;
	EdMixInpDlg.DoModal(mcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdMixInps::OnBuEdit() 
{
	int max = mcomp->NumInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdMixInp EdMixInpDlg;
	EdMixInpDlg.DoModal(mcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdMixInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(189),            // DelInputs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = mcomp->NumInput();
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
			if ( mcomp->DeleteInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(190),
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


void CEdMixInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdMixInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdMixInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( mcomp->Inflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(194), // IllegalInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixInp dialog


CEdMixInp::CEdMixInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdMixInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdMixInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdMixInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdMixInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdMixInp, CDialog)
	//{{AFX_MSG_MAP(CEdMixInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdMixInp::DoModal(MIXCOMP* com, BOOL a, int i)
{
	mcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixInp message handlers

BOOL CEdMixInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=mcomp->NumInput())
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
		int varpos = aqsys.Varlist()->Position(mcomp->InputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( mcomp->InputFlux(pos) != 0 )
		{
			mcomp->InputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdMixInp::OnOK() 
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
		if ( mcomp->AddInput(aqsys.Varlist(),var,
					 buffer1,buffer2,parseerrors,
							 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(191),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(192),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( mcomp->ReplaceInput(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(191),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(193),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CEdMixAcc dialog


CEdMixAcc::CEdMixAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdMixAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdMixAcc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdMixAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdMixAcc)
	DDX_Control(pDX, IDC_ED_RELACCV, m_ed_relaccv);
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_ABSACCV, m_ed_absaccv);
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdMixAcc, CDialog)
	//{{AFX_MSG_MAP(CEdMixAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdMixAcc::DoModal(MIXCOMP* com)
{
	mcomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdMixAcc message handlers

BOOL CEdMixAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	NumToString(mcomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(mcomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(mcomp->RelAccV(),buffer,sizeof(buffer));
	m_ed_relaccv.SetWindowText(buffer);
	NumToString(mcomp->AbsAccV(),buffer,sizeof(buffer));
	m_ed_absaccv.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdMixAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(589),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(590),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( mcomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(591),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccv.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(592),  // FailRelAccV
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccv.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(593),  // FailRelAccV
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( mcomp->AccV(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(594),  // FailAccV
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}

