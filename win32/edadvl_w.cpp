// edadvl_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edadvl_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdv dialog


CEdLinkAdv::CEdLinkAdv(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLinkAdv::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLinkAdv)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLinkAdv::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLinkAdv)
	DDX_Control(pDX, IDC_TX_CONNOUT, m_tx_connout);
	DDX_Control(pDX, IDC_TX_CONNIN, m_tx_connin);
	DDX_Control(pDX, IDC_TX_COMPOUT, m_tx_compout);
	DDX_Control(pDX, IDC_TX_COMPIN, m_tx_compin);
	DDX_Control(pDX, IDC_LE_CONNOUT, m_le_connout);
	DDX_Control(pDX, IDC_LE_CONNIN, m_le_connin);
	DDX_Control(pDX, IDC_LE_COMPOUT, m_le_compout);
	DDX_Control(pDX, IDC_LE_COMPIN, m_le_compin);
	DDX_Control(pDX, IDC_LB_BIFS, m_lb_bifs);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_LINKIND, m_ed_linkind);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_description);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLinkAdv, CDialog)
	//{{AFX_MSG_MAP(CEdLinkAdv)
	ON_LBN_DBLCLK(IDC_LB_BIFS, OnDblclkLbBifs)
	ON_LBN_SELCHANGE(IDC_LB_BIFS, OnSelchangeLbBifs)
	ON_CBN_CLOSEUP(IDC_LE_COMPIN, OnCloseupLeCompin)
	ON_CBN_CLOSEUP(IDC_LE_COMPOUT, OnCloseupLeCompout)
	ON_CBN_CLOSEUP(IDC_LE_CONNIN, OnCloseupLeConnin)
	ON_CBN_CLOSEUP(IDC_LE_CONNOUT, OnCloseupLeConnout)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLinkAdv::DoModal(AQLINK* link, BOOL a)
{
	oldlink = link;
	alink = new ADVLINK(oldlink);
	add = a;
	connin  = 0;
	connout = 0;
	return CDialog::DoModal();
}


void CEdLinkAdv::UpdateConnectListIn()
{
	int pos = m_le_compin.GetCurSel() + 1;
	m_le_connin.ResetContent();
	AQCOMP* com = aqsys.Complist()->Get(pos);
	if ( com == 0 )
	{
		aqapp.ProgErr("Illegal compartment");
		return;
	}
	for ( CARDINAL i=0; i<com->NumAdvOutConn(); i++ )
	{
		m_le_connin.AddString(com->AdvOutName(i));
	}
	if ( connin >= com->NumAdvOutConn() )
	{
		connin = 0;
	}
	m_le_connin.SetCurSel(connin);
	if ( com->NumAdvOutConn() < 2 )
	{
		m_tx_connin.EnableWindow(FALSE);
		m_le_connin.EnableWindow(FALSE);
	}
	else
	{
		m_tx_connin.EnableWindow(TRUE);
		m_le_connin.EnableWindow(TRUE);
	}

	return;
}


void CEdLinkAdv::UpdateConnectListOut()
{
	int pos = m_le_compout.GetCurSel();
	m_le_connout.ResetContent();
	if ( pos == 0 )
	{
		m_tx_connout.EnableWindow(FALSE);
		m_le_connout.EnableWindow(FALSE);
	}
	else
	{
		AQCOMP* com = aqsys.Complist()->Get(pos);
		if ( com == 0 )
		{
			aqapp.ProgErr("Illegal compartment");
			return;
		}
		for ( CARDINAL i=0; i<com->NumAdvInConn(); i++ )
		{
			m_le_connout.AddString(com->AdvInName(i));
		}
		if ( connout >= com->NumAdvInConn() )
		{
			connout = 0;
		}
		m_le_connout.SetCurSel(connout);
		if ( com->NumAdvInConn() < 2 )
		{
			m_tx_connout.EnableWindow(FALSE);
			m_le_connout.EnableWindow(FALSE);
		}
		else
		{
			m_tx_connout.EnableWindow(TRUE);
			m_le_connout.EnableWindow(TRUE);
		}
	}

	return;
}


void CEdLinkAdv::UpdateActivation()
{
	switch ( m_lb_bifs.GetSelCount() )
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


void CEdLinkAdv::UpdateBifList()
{
	int topindex = m_lb_bifs.GetTopIndex();
	m_lb_bifs.ResetContent();
	BIF* bif = alink->Biflist()->First();
	while ( bif != 0 )
	{
	   m_lb_bifs.InsertString(-1,bif->Symbol());
	   bif = bif->Next();
	}
	m_lb_bifs.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdv message handlers

BOOL CEdLinkAdv::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	if ( alink->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(alink->Symbol());
	}

	NumToString(alink->LnkIndex(),buffer,sizeof(buffer));
	m_ed_linkind.SetWindowText(buffer);

	if ( alink->Description() != 0 )
	{
		m_ed_description.SetWindowText(alink->Description());
	}

	m_le_compin.ResetContent();
	{
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_le_compin.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( alink->GetCompIn() != 0 )
	{
		int pos = aqsys.Complist()->Position(alink->GetCompIn());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_le_compin.SetCurSel(pos-1);
			connin = alink->GetConnIn();
		}
	}
	else
	{
		m_le_compin.SetCurSel(0);
	}

	UpdateConnectListIn();

	m_le_compout.ResetContent();
	{
		m_le_compout.AddString("");
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_le_compout.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( alink->GetCompOut() != 0 )
	{
		int pos = aqsys.Complist()->Position(alink->GetCompOut());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_le_compout.SetCurSel(pos);
			connout = alink->GetConnOut();
		}
	}
	else
	{
		m_le_compout.SetCurSel(0);
	}

	UpdateConnectListOut();

	UpdateBifList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLinkAdv::OnCloseupLeCompin() 
{
	UpdateConnectListIn();
	return;
}


void CEdLinkAdv::OnCloseupLeCompout() 
{
	UpdateConnectListOut();
	return;
}


void CEdLinkAdv::OnCloseupLeConnin() 
{
	connin = m_le_connin.GetCurSel();
	return;	
}


void CEdLinkAdv::OnCloseupLeConnout() 
{
	connout = m_le_connout.GetCurSel();
	return;	
}


void CEdLinkAdv::OnBuAdd() 
{
	CEdLinkAdvBif EdLinkAdvBifDlg;
	EdLinkAdvBifDlg.DoModal(alink,0,TRUE);

	UpdateBifList();
	return;
}


void CEdLinkAdv::OnBuEdit() 
{
	int max = alink->Biflist()->Size();
	int* indices = new int[max];
	int num = m_lb_bifs.GetSelItems(max,indices);
	int bifnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	BIF* bif = alink->Biflist()->Get(bifnum);
	if ( bif == 0 )
	{
		aqapp.ProgErr("Bifurcation not found");
		return;
	}
	CEdLinkAdvBif EdLinkAdvBifDlg;
	EdLinkAdvBifDlg.DoModal(alink,bif,FALSE);

	UpdateBifList();
	return;
}


void CEdLinkAdv::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(523),    // DelBifs?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = alink->Biflist()->Size();
	int* indices = new int[max];
	int num = m_lb_bifs.GetSelItems(max,indices);
	int j,k;
	BIF* b;
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
			b = alink->Biflist()->Get(indices[j]-k+1);
			if ( alink->DeleteBif(b) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(524)); // FailDelBif
				strcat(buffer," ");
				strcat(buffer,b->Symbol());
				MessageBox(buffer,
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	UpdateBifList();
	return;
}


void CEdLinkAdv::OnDblclkLbBifs() 
{
	OnBuEdit();
	return;
}


void CEdLinkAdv::OnSelchangeLbBifs() 
{
	UpdateActivation();
	return;
}


void CEdLinkAdv::OnCancel() 
{
	delete alink;
	
	CDialog::OnCancel();
}


void CEdLinkAdv::OnOK() 
{
	char buffer[1024]; int pos; int i; CARDINAL c; AQCOMP* com;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( alink->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(525),  // IllegalName
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_linkind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(526),  // FailLnkInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	alink->LnkIndex(c);

	m_ed_description.GetWindowText(buffer,sizeof(buffer));
	alink->Description(buffer);

	pos = m_le_compin.GetCurSel()+1;
	com = aqsys.Complist()->Get(pos);
	if ( com == 0 )
	{
		MessageBox(aqapp.ini.T(527),  // IllegalFromComp
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	i = m_le_connin.GetCurSel();
	if ( alink->SetCompIn(com,i) == FALSE )
	{
		MessageBox(aqapp.ini.T(528),  // IllegalConn
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}

	pos = m_le_compout.GetCurSel();
	com = aqsys.Complist()->Get(pos);
	i = m_le_connout.GetCurSel();
	if ( alink->SetCompOut(com,i) == FALSE )
	{
		MessageBox(aqapp.ini.T(528),  // IllegalConn
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddLink(alink) == FALSE )
		{
			MessageBox(aqapp.ini.T(529),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceLink(oldlink,alink) == FALSE )
		{
			MessageBox(aqapp.ini.T(530),  // IllegalLink
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdvBif dialog


CEdLinkAdvBif::CEdLinkAdvBif(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLinkAdvBif::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLinkAdvBif)
	//}}AFX_DATA_INIT
}


void CEdLinkAdvBif::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLinkAdvBif)
	DDX_Control(pDX, IDC_TX_CONNECT, m_tx_connect);
	DDX_Control(pDX, IDC_LE_COMPART, m_le_compart);
	DDX_Control(pDX, IDC_TX_FLUXLIST, m_tx_fluxlist);
	DDX_Control(pDX, IDC_TX_COMPART, m_tx_compart);
	DDX_Control(pDX, IDC_RB_WITHWATER, m_rb_withwater);
	DDX_Control(pDX, IDC_RB_ASGIVEN, m_rb_asgiven);
	DDX_Control(pDX, IDC_LE_CONNECT, m_le_connect);
	DDX_Control(pDX, IDC_LB_FLUXES, m_lb_fluxes);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	DDX_Control(pDX, IDC_ED_WATERFLOW, m_ed_waterflow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLinkAdvBif, CDialog)
	//{{AFX_MSG_MAP(CEdLinkAdvBif)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_DBLCLK(IDC_LB_FLUXES, OnDblclkLbFluxes)
	ON_CBN_CLOSEUP(IDC_LE_CONNECT, OnCloseupLeConnect)
	ON_CBN_CLOSEUP(IDC_LE_COMPART, OnCloseupLeCompart)
	ON_BN_CLICKED(IDC_RB_ASGIVEN, OnRbAsgiven)
	ON_BN_CLICKED(IDC_RB_WITHWATER, OnRbWithwater)
	ON_LBN_SELCHANGE(IDC_LB_FLUXES, OnSelchangeLbFluxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLinkAdvBif::DoModal(ADVLINK* l, BIF* b, BOOL a)
{
	alink = l;
	oldbif = b;
	bif = new BIF(oldbif);
	add = a;
	connect  = 0;
	return CDialog::DoModal();
}


void CEdLinkAdvBif::UpdateConnectList()
{
	int pos = m_le_compart.GetCurSel();
	m_le_connect.ResetContent();
	if ( pos == 0 )
	{
		m_tx_connect.EnableWindow(FALSE);
		m_le_connect.EnableWindow(FALSE);
	}
	else
	{
		AQCOMP* com = aqsys.Complist()->Get(pos);
		if ( com == 0 )
		{
			aqapp.ProgErr("Illegal compartment");
			return;
		}
		for ( CARDINAL i=0; i<com->NumAdvInConn(); i++ )
		{
			m_le_connect.AddString(com->AdvInName(i));
		}
		if ( connect >= com->NumAdvInConn() )
		{
			connect = 0;
		}
		m_le_connect.SetCurSel(connect);
		if ( com->NumAdvInConn() < 2 )
		{
			m_tx_connect.EnableWindow(FALSE);
			m_le_connect.EnableWindow(FALSE);
		}
		else
		{
			m_tx_connect.EnableWindow(TRUE);
			m_le_connect.EnableWindow(TRUE);
		}
	}

	return;
}


void CEdLinkAdvBif::UpdateActivation()
{
	if ( bif->BifType() == AsGiven )
	{
		m_bu_add.EnableWindow(TRUE);
		switch ( m_lb_fluxes.GetSelCount() )
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
	}
	else
	{
		m_bu_add.EnableWindow(FALSE);
		m_bu_edit.EnableWindow(FALSE);
		m_bu_delete.EnableWindow(FALSE);
	}
	return;
}


void CEdLinkAdvBif::UpdateFluxList()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_fluxes.GetTopIndex();
	m_lb_fluxes.ResetContent();
	for ( CARDINAL i=0; i<bif->NumFlux(); i++ )
	{
		strcpy(buffer,bif->FluxVar(i)->Symbol());
		strcat(buffer,"  :  ");
		bif->FluxVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_fluxes.InsertString(-1,buffer);
	}
	m_lb_fluxes.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdvBif message handlers

BOOL CEdLinkAdvBif::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	if ( bif->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(bif->Symbol());
	}

	if ( bif->Description() != 0 )
	{
		m_ed_descript.SetWindowText(bif->Description());
	}

	m_le_compart.ResetContent();
	{
		m_le_compart.AddString("");
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_le_compart.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( bif->GetBifComp() != 0 )
	{
		int pos = aqsys.Complist()->Position(bif->GetBifComp());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_le_compart.SetCurSel(pos);
			connect = bif->GetBifConn();
		}
	}
	else
	{
		m_le_compart.SetCurSel(0);
	}

	UpdateConnectList();

	if ( bif->WaterFlow() != 0 )
	{
		if ( bif->WaterFlow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_waterflow.SetWindowText(buffer);
		}
	}
	else
	{
		m_ed_waterflow.SetWindowText("0");
	}

	if ( bif->BifType() == WithWater )
	{
		m_rb_withwater.SetCheck(1);
		m_tx_fluxlist.EnableWindow(FALSE);
		m_lb_fluxes.EnableWindow(FALSE);
	}
	else
	{
		m_rb_asgiven.SetCheck(1);
		m_tx_fluxlist.EnableWindow(TRUE);
		m_lb_fluxes.EnableWindow(TRUE);
	}

	UpdateFluxList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLinkAdvBif::OnBuAdd() 
{
	int max = bif->NumFlux();
	int* indices = new int[max];
	int num = m_lb_fluxes.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLinkAdvBifFlux EdLinkAdvBifFluxDlg;
	EdLinkAdvBifFluxDlg.DoModal(bif,TRUE,pos);

	UpdateFluxList();
	return;
}


void CEdLinkAdvBif::OnBuEdit() 
{
	int max = bif->NumFlux();
	int* indices = new int[max];
	int num = m_lb_fluxes.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLinkAdvBifFlux EdLinkAdvBifFluxDlg;
	EdLinkAdvBifFluxDlg.DoModal(bif,FALSE,pos);

	UpdateFluxList();
	return;
}


void CEdLinkAdvBif::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(531), // DelFluxes?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = bif->NumFlux();
	int* indices = new int[max];
	int num = m_lb_fluxes.GetSelItems(max,indices);
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
			if ( bif->DeleteFlux(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(532),  // FailDelFlux
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	UpdateFluxList();
	return;
}


void CEdLinkAdvBif::OnDblclkLbFluxes() 
{
	OnBuEdit();
	return;
}


void CEdLinkAdvBif::OnSelchangeLbFluxes() 
{
	UpdateActivation();
	return;
}


void CEdLinkAdvBif::OnCloseupLeCompart() 
{
	UpdateConnectList();
	return;
}


void CEdLinkAdvBif::OnCloseupLeConnect() 
{
	connect = m_le_connect.GetCurSel();
	return;	
}


void CEdLinkAdvBif::OnRbAsgiven() 
{
	bif->BifType(AsGiven);
	m_tx_fluxlist.EnableWindow(TRUE);
	m_lb_fluxes.EnableWindow(TRUE);
	UpdateActivation();
	return;
}


void CEdLinkAdvBif::OnRbWithwater() 
{
	bif->BifType(WithWater);
	m_tx_fluxlist.EnableWindow(FALSE);
	m_lb_fluxes.EnableWindow(FALSE);
	UpdateActivation();
	return;
}


void CEdLinkAdvBif::OnCancel() 
{
	delete bif;
	
	CDialog::OnCancel();
}


void CEdLinkAdvBif::OnOK() 
{
	char buffer[1024]; char buffer1[1024];
	CARDINAL numerr; int pos; int i; AQCOMP* com;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( bif->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(533),  // IllegalName
			       aqapp.ini.T(111), 
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	bif->Description(buffer);

	pos = m_le_compart.GetCurSel();
	com = aqsys.Complist()->Get(pos);
	i = m_le_connect.GetCurSel();
	if ( bif->SetBifComp(com,i) == FALSE )
	{
		MessageBox(aqapp.ini.T(534),  // IllegalConn
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_waterflow.GetWindowText(buffer,sizeof(buffer));
	if ( bif->WaterFlow(buffer,aqsys.Varlist(),buffer1,numerr)
                                                        == FALSE)
	{
		if ( numerr != 0 )
		{
			MessageBox(aqapp.ini.T(535),  // ParseError
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
		}
		else
		{
			MessageBox(aqapp.ini.T(536),  // IllegalDep
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
		}
		return;
	}

	if ( add == TRUE )
	{
		if ( alink->AddBif(bif) == FALSE )
		{
			MessageBox(aqapp.ini.T(537),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( alink->ReplaceBif(oldbif,bif) == FALSE )
		{
			MessageBox(aqapp.ini.T(538),  // IllegalBif
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdvBifFlux dialog


CEdLinkAdvBifFlux::CEdLinkAdvBifFlux(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLinkAdvBifFlux::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLinkAdvBifFlux)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLinkAdvBifFlux::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLinkAdvBifFlux)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_FLUX, m_ed_flux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLinkAdvBifFlux, CDialog)
	//{{AFX_MSG_MAP(CEdLinkAdvBifFlux)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLinkAdvBifFlux::DoModal(BIF* b, BOOL a, int i)
{
	bif = b;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdvBifFlux message handlers


BOOL CEdLinkAdvBifFlux::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=bif->NumFlux())
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
		int varpos = aqsys.Varlist()->Position(bif->FluxVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( bif->FluxVal(pos) != 0 )
		{
			bif->FluxVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_flux.SetWindowText(buffer);
		}
	}
	else
	{
		m_le_var.SetCurSel(0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLinkAdvBifFlux::OnCancel() 
{
	CDialog::OnCancel();
}


void CEdLinkAdvBifFlux::OnOK() 
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
	m_ed_flux.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( bif->AddFlux(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(539),     // ParseErrFlux
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(540),     // FailAddFlux
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( bif->ReplaceFlux(aqsys.Varlist(),var,
						 buffer1,buffer2,parseerrors,
								 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(539),     // ParseErrFlux
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(541),     // IllegalFlux
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}
