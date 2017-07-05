// edpar_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edpar_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarPar dialog


CEdVarPar::CEdVarPar(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarPar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarPar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdVarPar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarPar)
	DDX_Control(pDX, IDC_ED_VALUE, m_ed_value);
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_STDEV, m_ed_stdev);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_MIN, m_ed_min);
	DDX_Control(pDX, IDC_ED_MAX, m_ed_max);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_CB_SENSACT, m_cb_sensact);
	DDX_Control(pDX, IDC_CB_FITACT, m_cb_fitact);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarPar, CDialog)
	//{{AFX_MSG_MAP(CEdVarPar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarPar::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	cvar = new CONSTVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdVarPar message handlers

BOOL CEdVarPar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( cvar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(cvar->Symbol());
	}

	if ( cvar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(cvar->Description());
	}

	if ( cvar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(cvar->Unit());
	}

	NumToString(cvar->Evaluate(),buffer,sizeof(buffer));
	m_ed_value.SetWindowText(buffer);

	NumToString(cvar->StDev(),buffer,sizeof(buffer));
	m_ed_stdev.SetWindowText(buffer);

	NumToString(cvar->Minimum(),buffer,sizeof(buffer));
	m_ed_min.SetWindowText(buffer);

	NumToString(cvar->Maximum(),buffer,sizeof(buffer));
	m_ed_max.SetWindowText(buffer);

	if ( cvar->SensActive() == TRUE )
	{
		m_cb_sensact.SetCheck(1);
	}
	else
	{
		m_cb_sensact.SetCheck(0);
	}

	if ( cvar->FitActive() == TRUE )
	{
		m_cb_fitact.SetCheck(1);
	}
	else
	{
		m_cb_fitact.SetCheck(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarPar::OnCancel() 
{
	delete cvar;
	
	CDialog::OnCancel();
}


void CEdVarPar::OnOK() 
{
	REAL val,s,mini,maxi;
	BOOLEAN sensact=FALSE; BOOLEAN fitact=FALSE;
	char buffer[1024];
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( cvar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(243),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	cvar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	cvar->Unit(buffer);

	m_ed_value.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&val) == FALSE )
	{
		MessageBox(aqapp.ini.T(244), // FailValue
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_stdev.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&s) == FALSE )
	{
		MessageBox(aqapp.ini.T(245), // FailStDev
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_min.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&mini) == FALSE )
	{
		MessageBox(aqapp.ini.T(246), // FailMin
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_max.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&maxi) == FALSE )
	{
		MessageBox(aqapp.ini.T(247), // FailValue
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_sensact.GetCheck() ) sensact = TRUE;

	if ( m_cb_fitact.GetCheck() ) fitact = TRUE;

	if ( cvar->Set(val,s,mini,maxi,sensact,fitact) == FALSE )
	{
		MessageBox(aqapp.ini.T(248), // FailValue
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddVar(cvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(249),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,cvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(250),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


