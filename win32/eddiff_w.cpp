// eddiff_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "eddiff_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdLinkDiff dialog


CEdLinkDiff::CEdLinkDiff(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLinkDiff::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLinkDiff)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLinkDiff::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLinkDiff)
	DDX_Control(pDX, IDC_TX_CONN2, m_tx_conn2);
	DDX_Control(pDX, IDC_TX_CONN1, m_tx_conn1);
	DDX_Control(pDX, IDC_TX_COMP2, m_tx_comp2);
	DDX_Control(pDX, IDC_TX_COMP1, m_tx_comp1);
	DDX_Control(pDX, IDC_LE_CONN2, m_le_conn2);
	DDX_Control(pDX, IDC_LE_CONN1, m_le_conn1);
	DDX_Control(pDX, IDC_LE_COMP2, m_le_comp2);
	DDX_Control(pDX, IDC_LE_COMP1, m_le_comp1);
	DDX_Control(pDX, IDC_LB_EXCOEFF, m_lb_excoeff);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_LINKIND, m_ed_linkind);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_description);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLinkDiff, CDialog)
	//{{AFX_MSG_MAP(CEdLinkDiff)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_DBLCLK(IDC_LB_EXCOEFF, OnDblclkLbExcoeff)
	ON_LBN_SELCHANGE(IDC_LB_EXCOEFF, OnSelchangeLbExcoeff)
	ON_CBN_CLOSEUP(IDC_LE_COMP1, OnCloseupLeComp1)
	ON_CBN_CLOSEUP(IDC_LE_COMP2, OnCloseupLeComp2)
	ON_CBN_CLOSEUP(IDC_LE_CONN1, OnCloseupLeConn1)
	ON_CBN_CLOSEUP(IDC_LE_CONN2, OnCloseupLeConn2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLinkDiff::DoModal(AQLINK* link, BOOL a)
{
	oldlink = link;
	dlink = new DIFFLINK(oldlink);
	add = a;
	conn1 = 0;
	conn2 = 0;
	return CDialog::DoModal();
}


void CEdLinkDiff::UpdateConnectList1()
{
	int pos = m_le_comp1.GetCurSel() + 1;
	m_le_conn1.ResetContent();
	AQCOMP* com = aqsys.Complist()->Get(pos);
	if ( com == 0 )
	{
		aqapp.ProgErr("Illegal compartment");
		return;
	}
	for ( CARDINAL i=0; i<com->NumDiffConn(); i++ )
	{
		m_le_conn1.AddString(com->DiffName(i));
	}
	if ( conn1 >= com->NumDiffConn() )
	{
		conn1 = 0;
	}
	m_le_conn1.SetCurSel(conn1);
	if ( com->NumDiffConn() < 2 )
	{
		m_tx_conn1.EnableWindow(FALSE);
		m_le_conn1.EnableWindow(FALSE);
	}
	else
	{
		m_tx_conn1.EnableWindow(TRUE);
		m_le_conn1.EnableWindow(TRUE);
	}

	return;
}


void CEdLinkDiff::UpdateConnectList2()
{
	int pos = m_le_comp2.GetCurSel()+1;
	m_le_conn2.ResetContent();
	AQCOMP* com = aqsys.Complist()->Get(pos);
	if ( com == 0 )
	{
		aqapp.ProgErr("Illegal compartment");
		return;
	}
	for ( CARDINAL i=0; i<com->NumDiffConn(); i++ )
	{
		m_le_conn2.AddString(com->DiffName(i));
	}
	if ( conn2 >= com->NumDiffConn() )
	{
		conn2 = 0;
	}
	m_le_conn2.SetCurSel(conn2);
	if ( com->NumDiffConn() < 2 )
	{
		m_tx_conn2.EnableWindow(FALSE);
		m_le_conn2.EnableWindow(FALSE);
	}
	else
	{
		m_tx_conn2.EnableWindow(TRUE);
		m_le_conn2.EnableWindow(TRUE);
	}

	return;
}


void CEdLinkDiff::UpdateActivation()
{
	switch ( m_lb_excoeff.GetSelCount() )
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


void CEdLinkDiff::UpdateExchList()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_excoeff.GetTopIndex();
	m_lb_excoeff.ResetContent();
	for ( CARDINAL i=0; i<dlink->NumExch(); i++ )
	{
		strcpy(buffer,dlink->ExchVar(i)->Symbol());
		strcat(buffer,"  :  ");
		dlink->ExchVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,",  ");
		dlink->ExchFac(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_excoeff.InsertString(-1,buffer);
	}
	m_lb_excoeff.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinkDiff message handlers

BOOL CEdLinkDiff::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	if ( dlink->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(dlink->Symbol());
	}

	NumToString(dlink->LnkIndex(),buffer,sizeof(buffer));
	m_ed_linkind.SetWindowText(buffer);

	if ( dlink->Description() != 0 )
	{
		m_ed_description.SetWindowText(dlink->Description());
	}

	m_le_comp1.ResetContent();
	{
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_le_comp1.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( dlink->GetComp1() != 0 )
	{
		int pos = aqsys.Complist()->Position(dlink->GetComp1());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_le_comp1.SetCurSel(pos-1);
			conn1 = dlink->GetConn1();
		}
	}
	else
	{
		m_le_comp1.SetCurSel(0);
	}

	UpdateConnectList1();

	m_le_comp2.ResetContent();
	{
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_le_comp2.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( dlink->GetComp2() != 0 )
	{
		int pos = aqsys.Complist()->Position(dlink->GetComp2());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_le_comp2.SetCurSel(pos-1);
			conn2 = dlink->GetConn2();
		}
	}
	else
	{
		m_le_comp2.SetCurSel(0);
	}

	UpdateConnectList2();

	UpdateExchList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLinkDiff::OnCloseupLeComp1() 
{
	UpdateConnectList1();
	return;
}


void CEdLinkDiff::OnCloseupLeComp2() 
{
	UpdateConnectList2();	
	return;
}


void CEdLinkDiff::OnCloseupLeConn1() 
{
	conn1 = m_le_conn1.GetCurSel();
	return;	
}


void CEdLinkDiff::OnCloseupLeConn2() 
{
	conn2 = m_le_conn2.GetCurSel();
	return;	
}


void CEdLinkDiff::OnBuAdd() 
{
	int max = dlink->NumExch();
	int* indices = new int[max];
	int num = m_lb_excoeff.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLinkDiffExch EdLinkDiffExchDlg;
	EdLinkDiffExchDlg.DoModal(dlink,TRUE,pos);

	UpdateExchList();
	return;
}


void CEdLinkDiff::OnBuEdit() 
{
	int max = dlink->NumExch();
	int* indices = new int[max];
	int num = m_lb_excoeff.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLinkDiffExch EdLinkDiffExchDlg;
	EdLinkDiffExchDlg.DoModal(dlink,FALSE,pos);

	UpdateExchList();
	return;
}


void CEdLinkDiff::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(542), // DelExchs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = dlink->NumExch();
	int* indices = new int[max];
	int num = m_lb_excoeff.GetSelItems(max,indices);
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
			if ( dlink->DeleteExchCoeff(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(543),  // FailDelExch
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	UpdateExchList();
	return;
}


void CEdLinkDiff::OnDblclkLbExcoeff() 
{
	OnBuEdit();
	return;
}


void CEdLinkDiff::OnSelchangeLbExcoeff() 
{
	UpdateActivation();
	return;
}


void CEdLinkDiff::OnCancel() 
{
	delete dlink;
	
	CDialog::OnCancel();
}


void CEdLinkDiff::OnOK() 
{
	char buffer[1024]; int pos; int i; CARDINAL c; AQCOMP* com;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( dlink->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(544),  // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_linkind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(545),  // FailLnkInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	dlink->LnkIndex(c);

	m_ed_description.GetWindowText(buffer,sizeof(buffer));
	dlink->Description(buffer);

	pos = m_le_comp1.GetCurSel()+1;
	com = aqsys.Complist()->Get(pos);
	if ( com == 0 )
	{
		MessageBox(aqapp.ini.T(546),  // IllegalComp1
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	i = m_le_conn1.GetCurSel();
	if ( dlink->SetComp1(com,i) == FALSE )
	{
		MessageBox(aqapp.ini.T(547),  // IllegalConn1
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}

	pos = m_le_comp2.GetCurSel()+1;
	com = aqsys.Complist()->Get(pos);
	if ( com == 0 )
	{
		MessageBox(aqapp.ini.T(548),  // IllegalComp2
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	i = m_le_conn2.GetCurSel();
	if ( dlink->SetComp2(com,i) == FALSE )
	{
		MessageBox(aqapp.ini.T(549),  // IllegalConn2
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddLink(dlink) == FALSE )
		{
			MessageBox(aqapp.ini.T(550),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceLink(oldlink,dlink) == FALSE )
		{
			MessageBox(aqapp.ini.T(551),  // IllegalLink
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CEdLinkDiffExch dialog


CEdLinkDiffExch::CEdLinkDiffExch(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLinkDiffExch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLinkDiffExch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLinkDiffExch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLinkDiffExch)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_EXCH, m_ed_exch);
	DDX_Control(pDX, IDC_ED_CONV1, m_ed_conv1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLinkDiffExch, CDialog)
	//{{AFX_MSG_MAP(CEdLinkDiffExch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLinkDiffExch::DoModal(DIFFLINK* l, BOOL a, int i)
{
	dlink = l;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinkDiffExch message handlers


BOOL CEdLinkDiffExch::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=dlink->NumExch())
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
		int varpos = aqsys.Varlist()->Position(dlink->ExchVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( dlink->ExchVal(pos) != 0 )
		{
			dlink->ExchVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_exch.SetWindowText(buffer);
		}
		if ( dlink->ExchFac(pos) != 0 )
		{
			dlink->ExchFac(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_conv1.SetWindowText(buffer);
		}
	}
	else
	{
		m_le_var.SetCurSel(0);
		m_ed_conv1.SetWindowText("1");
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLinkDiffExch::OnCancel() 
{
	CDialog::OnCancel();
}


void CEdLinkDiffExch::OnOK() 
{
	char buffer1[1024], buffer2[1024];
	char buffer3[1024], buffer4[1024];
	AQVAR* var;
	int  i;
	CARDINAL errorsexch, errorsfact;

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
	m_ed_exch.GetWindowText(buffer1,sizeof(buffer1));
	m_ed_conv1.GetWindowText(buffer3,sizeof(buffer3));
	if ( add == TRUE )
	{
		if ( dlink->AddExchCoeff(aqsys.Varlist(),var,
					     buffer1,buffer2,errorsexch,
					     buffer3,buffer4,errorsfact,
							     pos) == FALSE )
		{
			if ( errorsexch > 0 )
			{
				MessageBox(aqapp.ini.T(552), // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				if ( errorsfact > 0 )
				{
					MessageBox(aqapp.ini.T(553), // ParseErrConv
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
				else
				{
					MessageBox(aqapp.ini.T(554),     // UnableAddExch
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
			}
			return;
		}
	}
	else
	{
		if ( dlink->ReplaceExchCoeff(aqsys.Varlist(),var,
					     buffer1,buffer2,errorsexch,
					     buffer3,buffer4,errorsfact,
								 pos) == FALSE )
		{
			if ( errorsexch > 0 )
			{
				MessageBox(aqapp.ini.T(552), // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				if ( errorsfact > 0 )
				{
					MessageBox(aqapp.ini.T(553), // ParseErrConv
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
				else
				{
					MessageBox(aqapp.ini.T(555),     // IllegalExch
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
			}
			return;
		}
	}
	
	CDialog::OnOK();
}
