// eddynp_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "eddynp_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdProcDyn dialog


CEdProcDyn::CEdProcDyn(CWnd* pParent /*=NULL*/)
	: CDialog(CEdProcDyn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdProcDyn)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdProcDyn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdProcDyn)
	DDX_Control(pDX, IDC_LB_STOICH, m_lb_stoich);
	DDX_Control(pDX, IDC_ED_RATE, m_ed_rate);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdProcDyn, CDialog)
	//{{AFX_MSG_MAP(CEdProcDyn)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_DBLCLK(IDC_LB_STOICH, OnDblclkLbStoich)
	ON_LBN_SELCHANGE(IDC_LB_STOICH, OnSelchangeLbStoich)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdProcDyn::DoModal(AQPROC* proc, BOOL a)
{
	oldp = proc;
	dynp = new DYNPROC(oldp);
	add = a;
	return CDialog::DoModal();
}


void CEdProcDyn::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_stoich.GetTopIndex();
	m_lb_stoich.ResetContent();
	for ( CARDINAL i=0; i<dynp->NumVar(); i++ )
	{
		strcpy(buffer,dynp->Variable(i)->Symbol());
		strcat(buffer," : ");
		dynp->StoichCoeff(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_stoich.InsertString(-1,buffer);
	}
	m_lb_stoich.SetTopIndex(topindex);
	UpdateActivation();
}


void CEdProcDyn::UpdateActivation()
{
	switch ( m_lb_stoich.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_edit.EnableWindow(FALSE);
		m_bu_delete.EnableWindow(FALSE);
		break;
	case 1:
		m_bu_edit.EnableWindow(TRUE);
		m_bu_delete.EnableWindow(TRUE);
		break;
	default:
		m_bu_edit.EnableWindow(FALSE);
		m_bu_delete.EnableWindow(TRUE);
	   break;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdProcDyn message handlers

BOOL CEdProcDyn::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( dynp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(dynp->Symbol());
	}

	if ( dynp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(dynp->Description());
	}

	if ( dynp->RateLaw() != 0 )
	{
		if ( dynp->RateLaw()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_rate.SetWindowText(buffer);
		}
	}

	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdProcDyn::OnCancel() 
{
	delete dynp;
	
	CDialog::OnCancel();
}


void CEdProcDyn::OnOK() 
{
	char buffer[1024], buffer1[1024];
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( dynp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(300),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	dynp->Description(buffer);

	CARDINAL numerr;
	m_ed_rate.GetWindowText(buffer,sizeof(buffer));
	if ( dynp->RateLaw(aqsys.Varlist(),buffer,buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(301), // ParseErrRate
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(302), // IllegalRate
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddProc(dynp) == FALSE )
		{
			MessageBox(aqapp.ini.T(303),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceProc(oldp,dynp) == FALSE )
		{
			MessageBox(aqapp.ini.T(304),  // IllegalProc
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdProcDyn::OnBuAdd() 
{
	int max = dynp->NumVar();
	int* indices = new int[max];
	int num = m_lb_stoich.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdProcDynStoich EdProcDynStoichDlg;
	EdProcDynStoichDlg.DoModal(dynp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdProcDyn::OnBuEdit() 
{
	int max = dynp->NumVar();
	int* indices = new int[max];
	int num = m_lb_stoich.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdProcDynStoich EdProcDynStoichDlg;
	EdProcDynStoichDlg.DoModal(dynp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdProcDyn::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(298),            // DelStoichs?
		     aqapp.ini.T(111),
			 MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = dynp->NumVar();
	int* indices = new int[max];
	int num = m_lb_stoich.GetSelItems(max,indices);
	int j,k;
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
			if ( dynp->DeleteStoichCoeff(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(299),
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;
	UpdateDialog();
	return;
}


void CEdProcDyn::OnDblclkLbStoich() 
{
	OnBuEdit();
	return;
}


void CEdProcDyn::OnSelchangeLbStoich() 
{
	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdProcDynStoich dialog


CEdProcDynStoich::CEdProcDynStoich(CWnd* pParent /*=NULL*/)
	: CDialog(CEdProcDynStoich::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdProcDynStoich)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdProcDynStoich::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdProcDynStoich)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_STOICH, m_ed_stoich);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdProcDynStoich, CDialog)
	//{{AFX_MSG_MAP(CEdProcDynStoich)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdProcDynStoich::DoModal(DYNPROC* p, BOOL a, int i)
{
	dynp = p;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdProcDynStoich message handlers

BOOL CEdProcDynStoich::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=dynp->NumVar())
	{
		aqapp.ProgErr("Illegal Input");
	}

	char buffer[1024];
	m_le_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_var.AddString(var->Symbol());
			var = var->Next();
		}
	}

	if ( add == FALSE )
	{
		int varpos = aqsys.Varlist()->Position(dynp->Variable(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( dynp->StoichCoeff(pos) != 0 )
		{
			dynp->StoichCoeff(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_stoich.SetWindowText(buffer);
		}
	}
	else
	{
		m_le_var.SetCurSel(0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdProcDynStoich::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	AQVAR* var;
	int  i;
	CARDINAL parseerrors;

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
		var = aqsys.Varlist()->Get(i+1);
		if ( var == 0 )
		{
			aqapp.ProgErr("Variable not found");
			return;
		}
	}
	m_ed_stoich.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( dynp->AddStoichCoeff(aqsys.Varlist(),var,
					              buffer1,buffer2,parseerrors,
						          pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(305),     // ParseErrStoich
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(306),     // UnableAddStoich
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( dynp->ReplaceStoichCoeff(aqsys.Varlist(),var,
						              buffer1,buffer2,parseerrors,
								      pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(305),     // ParseErrStoich
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(307),     // IllegalStoich
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}

