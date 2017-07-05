// edprog_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edprog_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarProg dialog


CEdVarProg::CEdVarProg(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarProg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarProg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdVarProg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarProg)
	DDX_Control(pDX, IDC_LE_REFTO, m_le_refto);
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarProg, CDialog)
	//{{AFX_MSG_MAP(CEdVarProg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarProg::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	pvar = new PROGVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdVarProg message handlers

BOOL CEdVarProg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( pvar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(pvar->Symbol());
	}

	if ( pvar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(pvar->Description());
	}

	if ( pvar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(pvar->Unit());
	}

	m_le_refto.ResetContent();
	for ( CARDINAL i=0; i<PROGVAR::NumProgVarTypes(); i++ )
	{
		m_le_refto.AddString(PROGVAR::ProgVarTypeName((PROGVARTYPE)i));
	}
	m_le_refto.SetCurSel((int)pvar->ProgVarType());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarProg::OnCancel() 
{
	delete pvar;
	
	CDialog::OnCancel();
}


void CEdVarProg::OnOK() 
{
	char buffer[1024];
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( pvar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(251),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	pvar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	pvar->Unit(buffer);

	int i = m_le_refto.GetCurSel();
	if ( i == CB_ERR )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	else
	{
		pvar->ProgVarType((PROGVARTYPE)i);
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddVar(pvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(252),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,pvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(253),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


