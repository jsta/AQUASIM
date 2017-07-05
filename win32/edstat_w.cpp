// edstat_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edstat_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarState dialog


CEdVarState::CEdVarState(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarState::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarState)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdVarState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarState)
	DDX_Control(pDX, IDC_RB_EQ, m_rb_eq);
	DDX_Control(pDX, IDC_RB_DYN, m_rb_dyn);
	DDX_Control(pDX, IDC_RB_VOL, m_rb_vol);
	DDX_Control(pDX, IDC_RB_SURF, m_rb_surf);
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_RELACC, m_ed_relacc);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_ABSACC, m_ed_absacc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarState, CDialog)
	//{{AFX_MSG_MAP(CEdVarState)
	ON_BN_CLICKED(IDC_RB_DYN, OnRbDyn)
	ON_BN_CLICKED(IDC_RB_EQ, OnRbEq)
	ON_BN_CLICKED(IDC_RB_VOL, OnRbVol)
	ON_BN_CLICKED(IDC_RB_SURF, OnRbSurf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarState::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	svar = new STATEVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdVarState message handlers

BOOL CEdVarState::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];

	if ( svar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(svar->Symbol());
	}

	if ( svar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(svar->Description());
	}

	if ( svar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(svar->Unit());
	}

	switch ( svar->StateVarType() )
	{
	case VolStateVar:
		m_rb_dyn.SetCheck(1);
		m_rb_vol.SetCheck(1);
		volifdyn = TRUE;
		break;
	case SurfStateVar:
		m_rb_dyn.SetCheck(1);
		m_rb_surf.SetCheck(1);
		volifdyn = FALSE;
		break;
	case EquStateVar:
		m_rb_eq.SetCheck(1);
		m_rb_vol.SetCheck(1);
		volifdyn = TRUE;
		m_rb_vol.EnableWindow(FALSE);
		m_rb_surf.EnableWindow(FALSE);
		break;
	}

	NumToString(svar->RelAccuracy(),buffer,sizeof(buffer));
	m_ed_relacc.SetWindowText(buffer);

	NumToString(svar->AbsAccuracy(),buffer,sizeof(buffer));
	m_ed_absacc.SetWindowText(buffer);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarState::OnRbDyn() 
{
	if ( volifdyn == TRUE )
	{
		if ( svar->StateVarType(VolStateVar) == TRUE )
		{
			m_rb_vol.EnableWindow(TRUE);
			m_rb_surf.EnableWindow(TRUE);
		}
	}
	else
	{
		if ( svar->StateVarType(SurfStateVar) == TRUE )
		{
			m_rb_vol.EnableWindow(TRUE);
			m_rb_surf.EnableWindow(TRUE);
		}
	}
	return;
}


void CEdVarState::OnRbEq() 
{
	if ( svar->StateVarType(EquStateVar) == TRUE )
	{
		m_rb_vol.EnableWindow(FALSE);
		m_rb_surf.EnableWindow(FALSE);
	}
	return;
}


void CEdVarState::OnRbVol() 
{
	svar->StateVarType(VolStateVar);
	return;
}


void CEdVarState::OnRbSurf() 
{
	svar->StateVarType(SurfStateVar);
	return;
}


void CEdVarState::OnCancel() 
{
	delete svar;
	
	CDialog::OnCancel();
}


void CEdVarState::OnOK() 
{
	char buffer[1024];

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( svar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(233),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	svar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	svar->Unit(buffer);

	REAL relacc, absacc;
	m_ed_relacc.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(234), // FailRelAcc
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacc.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(235), // FailAbsAcc
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( svar->Accuracy(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(236), // IllegalAcc
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddVar(svar) == FALSE )
		{
			MessageBox(aqapp.ini.T(237),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,svar) == FALSE )
		{
			MessageBox(aqapp.ini.T(238),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


