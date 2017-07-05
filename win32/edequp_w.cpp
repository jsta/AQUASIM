// edequp_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edequp_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdProcEqu dialog


CEdProcEqu::CEdProcEqu(CWnd* pParent /*=NULL*/)
	: CDialog(CEdProcEqu::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdProcEqu)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdProcEqu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdProcEqu)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_EQUATION, m_ed_equation);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdProcEqu, CDialog)
	//{{AFX_MSG_MAP(CEdProcEqu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdProcEqu::DoModal(AQPROC* proc, BOOL a)
{
	oldp = proc;
	equp = new EQUPROC(oldp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdProcEqu message handlers

BOOL CEdProcEqu::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( equp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(equp->Symbol());
	}

	if ( equp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(equp->Description());
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
	if ( equp->Variable() != 0 )
	{
		int varpos = aqsys.Varlist()->Position(equp->Variable());
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
	}
	else
	{
		m_le_var.SetCurSel(0);
	}

	if ( equp->Equation() != 0 )
	{
		if ( equp->Equation()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_equation.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdProcEqu::OnCancel() 
{
	delete equp;
	
	CDialog::OnCancel();
}


void CEdProcEqu::OnOK() 
{
	char buffer[1024], buffer1[1024];
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( equp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(300),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	equp->Description(buffer);

	int index = m_le_var.GetCurSel();
	AQVAR* var = aqsys.Varlist()->Get(index+1);
	if ( var == 0 )
	{
		MessageBox(aqapp.ini.T(312),    // IllegalVar
				   aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	CARDINAL numerr;
	m_ed_equation.GetWindowText(buffer,sizeof(buffer));
	if ( equp->Equation(var,aqsys.Varlist(),buffer,buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(308), // ParseErrEq
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(309), // IllegalEq
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddProc(equp) == FALSE )
		{
			MessageBox(aqapp.ini.T(310),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceProc(oldp,equp) == FALSE )
		{
			MessageBox(aqapp.ini.T(311),  // IllegalProc
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}
