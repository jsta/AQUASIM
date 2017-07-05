// edprob_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edprob_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarProbe dialog


CEdVarProbe::CEdVarProbe(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarProbe::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarProbe)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdVarProbe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarProbe)
	DDX_Control(pDX, IDC_CB_SPACEREL, m_cb_spacerel);
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_LE_COMP, m_le_comp);
	DDX_Control(pDX, IDC_TX_COMP, m_tx_comp);
	DDX_Control(pDX, IDC_LE_COMPZONE, m_le_zone);
	DDX_Control(pDX, IDC_TX_COMPZONE, m_tx_zone);
	DDX_Control(pDX, IDC_ED_SPACE, m_ed_space);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarProbe, CDialog)
	//{{AFX_MSG_MAP(CEdVarProbe)
	ON_CBN_CLOSEUP(IDC_LE_COMP, OnCloseupLeComp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarProbe::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	probevar = new PROBEVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


void CEdVarProbe::UpdateZoneList()
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
			if ( probevar->Zone() >= com->NumZones() ) probevar->Zone(0);
			m_le_zone.SetCurSel(probevar->Zone());
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
// CEdVarProbe message handlers

BOOL CEdVarProbe::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];

	if ( probevar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(probevar->Symbol());
	}

	if ( probevar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(probevar->Description());
	}

	if ( probevar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(probevar->Unit());
	}

	m_le_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_var.AddString(var->Symbol());
			var = var->Next();
		}
	}
	if ( probevar->Variable() != 0 )
	{
		int pos = aqsys.Varlist()->Position(probevar->Variable());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(pos-1);
		}
	}
	else
	{
		m_le_var.SetCurSel(0);
	}

	m_le_comp.ResetContent();
	{
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_le_comp.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( probevar->Compartment() != 0 )
	{
		int pos = aqsys.Complist()->Position(probevar->Compartment());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_le_comp.SetCurSel(pos-1);
		}
	}
	else
	{
		m_le_comp.SetCurSel(0);
	}
	if ( aqsys.Complist()->Size() == 1 )
	{
		m_le_comp.EnableWindow(FALSE);
		m_tx_comp.EnableWindow(FALSE);
	}

	UpdateZoneList();

	NumToString(probevar->Space(),buffer,sizeof(buffer));
	m_ed_space.SetWindowText(buffer);

	if ( probevar->SpaceRel() ) m_cb_spacerel.SetCheck(1);
	else                        m_cb_spacerel.SetCheck(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarProbe::OnCloseupLeComp() 
{
   UpdateZoneList();
   return;	
}


void CEdVarProbe::OnCancel() 
{
	delete probevar;
	
	CDialog::OnCancel();
}


void CEdVarProbe::OnOK() 
{
	char buffer[1024];
	int i;
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( probevar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(260),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	probevar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	probevar->Unit(buffer);

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
		AQVAR* var = aqsys.Varlist()->Get(i+1);
		if ( var == 0 )
		{
			aqapp.ProgErr("Variable not found");
			return;
		}
		if ( probevar->Variable(var) == FALSE )
		{
			aqapp.ProgErr("Unable to set Variable");
			return;
		}
	}

	i = m_le_comp.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(75), // IllegalComp
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		AQCOMP* com = aqsys.Complist()->Get(i+1);
		if ( com == 0 )
		{
			aqapp.ProgErr("Compartment not found");
			return;
		}
		if ( probevar->Compartment(com) == FALSE )
		{
			aqapp.ProgErr("Unable to set Compartment");
			return;
		}
	}

	i = m_le_zone.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(76), // IllegalZone
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		if ( probevar->Zone(i) == FALSE )
		{
			aqapp.ProgErr("Unable to set Zone");
			return;
		}
	}

	REAL space;
	m_ed_space.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&space) == FALSE )
	{
		MessageBox(aqapp.ini.T(77), // IllegalLocation
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		probevar->Space(space);
	}

	BOOLEAN relative = FALSE;
    if ( m_cb_spacerel.GetCheck() ) relative = TRUE;
    probevar->SpaceRel(relative);

	if ( add == TRUE )
	{
		if ( aqsys.AddVar(probevar) == FALSE )
		{
			MessageBox(aqapp.ini.T(261),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,probevar) == FALSE )
		{
			MessageBox(aqapp.ini.T(262),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}

