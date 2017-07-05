// ednum_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "ednum_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdNumPar dialog


CEdNumPar::CEdNumPar(CWnd* pParent /*=NULL*/)
	: CDialog(CEdNumPar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdNumPar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdNumPar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdNumPar)
	DDX_Control(pDX, IDC_ED_FITTOL, m_ed_fittol);
	DDX_Control(pDX, IDC_ED_STEPSIZE, m_ed_stepsize);
	DDX_Control(pDX, IDC_ED_ORDER, m_ed_order);
	DDX_Control(pDX, IDC_ED_MAXSTEP, m_ed_maxstep);
	DDX_Control(pDX, IDC_ED_CODIAG, m_ed_codiag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdNumPar, CDialog)
	//{{AFX_MSG_MAP(CEdNumPar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEdNumPar message handlers

BOOL CEdNumPar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	NumToString(aqsys.DtMax(),buffer,sizeof(buffer));
	m_ed_stepsize.SetWindowText(buffer);

	NumToString(aqsys.MaxOrd(),buffer,sizeof(buffer));
	m_ed_order.SetWindowText(buffer);

	NumToString(aqsys.NumCoDiag(),buffer,sizeof(buffer));
	m_ed_codiag.SetWindowText(buffer);

	NumToString(aqsys.MaxNumSteps(),buffer,sizeof(buffer));
	m_ed_maxstep.SetWindowText(buffer);

	NumToString(aqsys.FitTol(),buffer,sizeof(buffer));
	m_ed_fittol.SetWindowText(buffer);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdNumPar::OnCancel() 
{
	CDialog::OnCancel();
}


void CEdNumPar::OnOK() 
{
	char buffer[1024]; REAL r; CARDINAL c;

	m_ed_stepsize.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r) == FALSE )
	{
		MessageBox("Unable to read Maximum Internal Step Size", // FailReadSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( aqsys.DtMax(r) == FALSE )
	{
		MessageBox("Illegal value of Maximum Internal Step Size", // FailReadSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_order.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox("Unable to read Maximum Integration Order", // FailReadOrder
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( aqsys.MaxOrd(c) == FALSE )
	{
		MessageBox("Illegal value of Maximum Integration Order", // FailReadOrder
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_codiag.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox("Unable to read Number of Codiagonals of the Jacobian Matrix", // FailReadDiag
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	aqsys.NumCoDiag(c);

	m_ed_maxstep.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox("Unable to read Maximum Number of Time Steps", // FailReadSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( aqsys.MaxNumSteps(c) == FALSE )
	{
		MessageBox("Illegal value of Maximum Number of Internal Time Steps", // FailReadSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_fittol.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r) == FALSE )
	{
		MessageBox("Unable to read Tolerance for Convergence of Parameter Estimation", // FailReadSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( aqsys.FitTol(r) == FALSE )
	{
		MessageBox("Illegal value of Tolerance for Convergence of Parameter Estimation", // FailReadSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	CDialog::OnOK();
}
