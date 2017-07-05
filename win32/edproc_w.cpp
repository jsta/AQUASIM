// edproc_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "edproc_w.h"
#include "eddynp_w.h"
#include "edequp_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static AQPROCTYPE proctype = DynProc;

static void EditProc(AQPROC* proc, BOOL add);

static void EditProc(AQPROC* proc, BOOL add)
{
	if ( proc != 0 ) proctype = proc->Type();
	switch ( proctype )
	{
	case DynProc:
		{
			CEdProcDyn EdProcDynDlg;
			EdProcDynDlg.DoModal(proc,add);
		}
		break;
	case EquProc:
		{
			CEdProcEqu EdProcEquDlg;
			EdProcEquDlg.DoModal(proc,add);
		}
		break;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdProcs dialog


CEdProcs::CEdProcs(CWnd* pParent /*=NULL*/)
	: CDialog(CEdProcs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdProcs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CEdProcs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdProcs)
	DDX_Control(pDX, IDC_LB_PROCLIST, m_proclist);
	DDX_Control(pDX, IDC_BU_NEW, m_new);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	DDX_Control(pDX, IDC_TX_TYPE, m_tx_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdProcs, CDialog)
	//{{AFX_MSG_MAP(CEdProcs)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_LBN_DBLCLK(IDC_LB_PROCLIST, OnDblclkLbProclist)
	ON_LBN_SELCHANGE(IDC_LB_PROCLIST, OnSelchangeLbProclist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEdProcs::UpdateActivation()
{
	if ( aqsys.Varlist()->Size() > 0 )
	{
		m_new.EnableWindow(TRUE);
	}
	else
	{
		m_new.EnableWindow(FALSE);
	}
	switch ( m_proclist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(FALSE);
		m_tx_type.SetWindowText("");
		break;
	case 1:
		m_duplicate.EnableWindow(TRUE);
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		{
			int max = aqsys.Proclist()->Size();
			int* indices = new int[max];
			int num = m_proclist.GetSelItems(max,indices);
			int procnum = indices[0]+1;
			delete [] indices;
			if ( num != 1 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			AQPROC* proc = aqsys.Proclist()->Get(procnum);
			if ( proc == 0 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			m_tx_type.SetWindowText(proc->TypeName());
		}
		break;
	default:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(TRUE);
		m_tx_type.SetWindowText("");
	   break;
	}
	return;
}


void CEdProcs::UpdateDialog()
{
	if ( created == 0 ) return;

	int topindex = m_proclist.GetTopIndex();
	m_proclist.ResetContent();
	AQPROC* proc = aqsys.Proclist()->First();
	while ( proc != 0 )
	{
	   m_proclist.InsertString(-1,proc->Symbol());
	   proc = proc->Next();
	}
	m_proclist.SetTopIndex(topindex);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdProcs message handlers

BOOL CEdProcs::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdProcs::OnOK() // Close button
{
	DestroyWindow();
	return;
}


void CEdProcs::OnCancel() // Close button 
{
	DestroyWindow();
	return;
}


void CEdProcs::OnClose() // Close box
{
	DestroyWindow();

	CDialog::OnClose();
}

void CEdProcs::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CEdProcs::OnBuNew() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CSelProcDlg SelProcDlg;
	SelProcDlg.DoModal();
	UpdateDialog();
	return; 
}


void CEdProcs::OnBuEdit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Proclist()->Size();
	int* indices = new int[max];
	int num = m_proclist.GetSelItems(max,indices);
	int procnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQPROC* proc = aqsys.Proclist()->Get(procnum);
	if ( proc == 0 )
	{
		aqapp.ProgErr("Process not found");
		return;
	}
	EditProc(proc,FALSE);

	UpdateDialog();
	return; 
}


void CEdProcs::OnBuDuplicate() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Proclist()->Size();
	int* indices = new int[max];
	int num = m_proclist.GetSelItems(max,indices);
	int procnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQPROC* proc = aqsys.Proclist()->Get(procnum);
	if ( proc == 0 )
	{
		aqapp.ProgErr("Process not found");
		return;
	}
	EditProc(proc,TRUE);

	UpdateDialog();
	return; 
}


void CEdProcs::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	switch ( MessageBox(aqapp.ini.T(9),              // DelProcs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = aqsys.Proclist()->Size();
	int* indices = new int[max];
	int num = m_proclist.GetSelItems(max,indices);
	int j,k;
	AQPROC* proc;
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
			proc = aqsys.Proclist()->Get(indices[j]-k+1);
			if ( aqsys.DeleteProc(proc) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(19)); // FailDelProc
				strcat(buffer," ");
				strcat(buffer,proc->Symbol());
				MessageBox(buffer,
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	UpdateDialog();
}


void CEdProcs::OnDblclkLbProclist() 
{
	OnBuEdit();
	return;
}


void CEdProcs::OnSelchangeLbProclist() 
{
	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSelProcDlg dialog


CSelProcDlg::CSelProcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelProcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelProcDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelProcDlg)
	DDX_Control(pDX, IDC_RB_EQPROC, m_rb_eqproc);
	DDX_Control(pDX, IDC_RB_DYNPROC, m_rb_dynproc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelProcDlg, CDialog)
	//{{AFX_MSG_MAP(CSelProcDlg)
	ON_BN_CLICKED(IDC_RB_DYNPROC, OnRbDynproc)
	ON_BN_CLICKED(IDC_RB_EQPROC, OnRbEqproc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSelProcDlg message handlers

BOOL CSelProcDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	switch ( proctype )
	{
	case DynProc:
		m_rb_dynproc.SetCheck(1);
		break;
	case EquProc:
		m_rb_eqproc.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CSelProcDlg::OnOK() 
{
	EditProc(0,TRUE);
	
	CDialog::OnOK();
}


void CSelProcDlg::OnRbDynproc() 
{
	proctype = DynProc;
	return;
}


void CSelProcDlg::OnRbEqproc() 
{
	proctype = EquProc;
	return;
}



