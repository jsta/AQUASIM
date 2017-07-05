// file_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "file_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilePrintOpt dialog


CFilePrintOpt::CFilePrintOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CFilePrintOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilePrintOpt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFilePrintOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilePrintOpt)
	DDX_Control(pDX, IDC_RB_SHORT, m_rb_short);
	DDX_Control(pDX, IDC_RB_LONG, m_rb_long);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilePrintOpt, CDialog)
	//{{AFX_MSG_MAP(CFilePrintOpt)
	ON_BN_CLICKED(IDC_RB_LONG, OnRbLong)
	ON_BN_CLICKED(IDC_RB_SHORT, OnRbShort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePrintOpt message handlers


BOOL CFilePrintOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	printshort = aqsys.PrintShort();

	if ( printshort == TRUE )
	{
		m_rb_short.SetCheck(1);
	}
	else
	{
		m_rb_long.SetCheck(1);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFilePrintOpt::OnRbLong() 
{
	printshort = FALSE;
	return;
}


void CFilePrintOpt::OnRbShort() 
{
	printshort = TRUE;
	return;
}


void CFilePrintOpt::OnOK() 
{
	aqsys.PrintShort(printshort);
	
	CDialog::OnOK();
}
