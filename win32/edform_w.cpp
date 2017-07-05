// edform_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edform_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarForm dialog


CEdVarForm::CEdVarForm(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdVarForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarForm)
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_EXPR, m_ed_expr);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarForm, CDialog)
	//{{AFX_MSG_MAP(CEdVarForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarForm::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	fvar = new FORMVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdVarForm message handlers

BOOL CEdVarForm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( fvar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(fvar->Symbol());
	}

	if ( fvar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(fvar->Description());
	}

	if ( fvar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(fvar->Unit());
	}

	if ( fvar->UnParse(buffer,sizeof(buffer)) == 0 )
	{
		m_ed_expr.SetWindowText(buffer);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarForm::OnCancel() 
{
	delete fvar;
	
	CDialog::OnCancel();
}


void CEdVarForm::OnOK() 
{
	char buffer[1024]; char buffer1[1024];

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( fvar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(239),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	fvar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	fvar->Unit(buffer);

	m_ed_expr.GetWindowText(buffer,sizeof(buffer));
	if ( fvar->Parse(buffer,aqsys.Varlist(),buffer1) != 0 )
	{
		MessageBox(aqapp.ini.T(240),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddVar(fvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(241),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,fvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(242),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}

