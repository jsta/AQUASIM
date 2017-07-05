// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "waidlg_w.h"
#include "./aqcore/all.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog


CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDlg)
	DDX_Control(pDX, IDC_WAIT_TEXT, m_wait_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWaitDlg::Display(const char* txt)
{
	if ( created )
	{
	   BringWindowToTop();
	}
	else
	{
	   Create(IDD_WAIT_CANCEL);
	}
	m_wait_text.SetWindowText(txt);
	return;
}


void CWaitDlg::Remove()
{
	DestroyWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CWaitDlg message handlers

BOOL CWaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	aqapp.ClearUserInterrupt();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CWaitDlg::OnOK() 
{
	aqapp.SetUserInterrupt();
	return;
}


void CWaitDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CWaitSimDlg dialog


CWaitSimDlg::CWaitSimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitSimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitSimDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CWaitSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitSimDlg)
	DDX_Control(pDX, IDC_WAIT_CALC, m_wait_calc);
	DDX_Control(pDX, IDC_WAIT_TIME, m_wait_time);
	DDX_Control(pDX, IDC_WAIT_TEXT, m_wait_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitSimDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitSimDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWaitSimDlg::Display(const char* txt)
{
	if ( created )
	{
	   BringWindowToTop();
	}
	else
	{
	   Create(IDD_WAIT_SIM);
	}
	m_wait_text.SetWindowText(txt);
	return;
}


void CWaitSimDlg::SetCalcname(char* name)
{
	if ( created == 0 ) return;
	m_wait_calc.SetWindowText(name);
	return;
}


void CWaitSimDlg::SetTime(REAL t)
{
	if ( created == 0 ) return;
	char buf[128];
	NumToString(t,buf,128);
	m_wait_time.SetWindowText((const char*)buf);
	return;
}


void CWaitSimDlg::Remove()
{
	DestroyWindow();
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CWaitSimDlg message handlers

BOOL CWaitSimDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	aqapp.ClearUserInterrupt();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CWaitSimDlg::OnOK() 
{
	aqapp.SetUserInterrupt();
	return;
}


void CWaitSimDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CWaitSensDlg dialog


CWaitSensDlg::CWaitSensDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitSensDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitSensDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CWaitSensDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitSensDlg)
	DDX_Control(pDX, IDC_WAIT_TEXT, m_wait_text);
	DDX_Control(pDX, IDC_WAIT_NUMSIM, m_wait_numsim);
	DDX_Control(pDX, IDC_WAIT_CURSIM, m_wait_cursim);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitSensDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitSensDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWaitSensDlg::Display(const char* txt)
{
	if ( created )
	{
	   BringWindowToTop();
	}
	else
	{
	   Create(IDD_WAIT_SENS);
	}
	m_wait_text.SetWindowText(txt);
	return;
}


void CWaitSensDlg::SetSim(CARDINAL num, CARDINAL cur)
{
	if ( created == 0 ) return;
	char buf[128];
	NumToString(num,buf,128);
	m_wait_numsim.SetWindowText((const char*)buf);
	if ( cur <= num )
	{
		NumToString(cur,buf,128);
		m_wait_cursim.SetWindowText((const char*)buf);
	}
	else
	{
		m_wait_cursim.SetWindowText("Sens. Ranking");
	}
	return;
}


void CWaitSensDlg::Remove()
{
	DestroyWindow();
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CWaitSensDlg message handlers

BOOL CWaitSensDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	aqapp.ClearUserInterrupt();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWaitSensDlg::OnOK() 
{
	aqapp.SetUserInterrupt();
	return;
}


void CWaitSensDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CWaitFitDlg dialog


CWaitFitDlg::CWaitFitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitFitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitFitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CWaitFitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitFitDlg)
	DDX_Control(pDX, IDC_WAIT_TEXT, m_wait_text);
	DDX_Control(pDX, IDC_WAIT_INICHI, m_wait_inichi);
	DDX_Control(pDX, IDC_WAIT_CURCHI, m_wait_curchi);
	DDX_Control(pDX, IDC_WAIT_BESTCHI, m_wait_bestchi);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitFitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitFitDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWaitFitDlg::Display(const char* txt)
{
	if ( created )
	{
	   BringWindowToTop();
	}
	else
	{
	   Create(IDD_WAIT_FIT);
	}
	m_wait_text.SetWindowText(txt);
	return;
}


void CWaitFitDlg::SetChi(REAL inichi, REAL bestchi, REAL curchi)
{
	if ( created == 0 ) return;
	char buf[128];
	NumToString(inichi,buf,128);
	m_wait_inichi.SetWindowText((const char*)buf);
	NumToString(bestchi,buf,128);
	m_wait_bestchi.SetWindowText((const char*)buf);
	NumToString(curchi,buf,128);
	m_wait_curchi.SetWindowText((const char*)buf);
	return;
}


void CWaitFitDlg::Remove()
{
	DestroyWindow();
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CWaitFitDlg message handlers

BOOL CWaitFitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	aqapp.ClearUserInterrupt();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWaitFitDlg::OnOK() 
{
	aqapp.SetUserInterrupt();
	return;
}


void CWaitFitDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
	return;
}


