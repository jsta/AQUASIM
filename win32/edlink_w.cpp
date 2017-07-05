// edlink_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "edlink_w.h"
#include "edadvl_w.h"
#include "eddiff_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static AQLINKTYPE linktype = AdvLink;

static void EditLink(AQLINK* link, BOOL add);

static void EditLink(AQLINK* link, BOOL add)
{
	if ( link != 0 ) linktype = link->Type();
	switch ( linktype )
	{
	case AdvLink:
		{
			CEdLinkAdv EdLinkAdvDlg;
			EdLinkAdvDlg.DoModal(link,add);
		}
		break;
	case DiffLink:
		{
			CEdLinkDiff EdLinkDiffDlg;
			EdLinkDiffDlg.DoModal(link,add);
		}
		break;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinks dialog


CEdLinks::CEdLinks(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLinks::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLinks)
	//}}AFX_DATA_INIT

	created = 0;
}


void CEdLinks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLinks)
	DDX_Control(pDX, IDC_LB_LINKLIST, m_linklist);
	DDX_Control(pDX, IDC_BU_NEW, m_new);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	DDX_Control(pDX, IDC_TX_TYPE, m_tx_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLinks, CDialog)
	//{{AFX_MSG_MAP(CEdLinks)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_LBN_DBLCLK(IDC_LB_LINKLIST, OnDblclkLbLinklist)
	ON_LBN_SELCHANGE(IDC_LB_LINKLIST, OnSelchangeLbLinklist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEdLinks::UpdateActivation()
{
	if ( aqsys.Complist()->Size() > 0 )
	{
		m_new.EnableWindow(TRUE);
	}
	else
	{
		m_new.EnableWindow(FALSE);
	}
	switch ( m_linklist.GetSelCount() )
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
			int max = aqsys.Linklist()->Size();
			int* indices = new int[max];
			int num = m_linklist.GetSelItems(max,indices);
			int linknum = indices[0]+1;
			delete [] indices;
			if ( num != 1 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			AQLINK* link = aqsys.Linklist()->Get(linknum);
			if ( link == 0 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			m_tx_type.SetWindowText(link->TypeName());
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


void CEdLinks::UpdateDialog()
{
	if ( created == 0 ) return;

	int topindex = m_linklist.GetTopIndex();
	m_linklist.ResetContent();
	AQLINK* link = aqsys.Linklist()->First();
	while ( link != 0 )
	{
	   m_linklist.InsertString(-1,link->Symbol());
	   link = link->Next();
	}
	m_linklist.SetTopIndex(topindex);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLinks message handlers

BOOL CEdLinks::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLinks::OnOK() // Close button
{
	DestroyWindow();
	return;
}


void CEdLinks::OnCancel() // Close button 
{
	DestroyWindow();
	return;
}


void CEdLinks::OnClose() // Close box
{
	DestroyWindow();

	CDialog::OnClose();
}


void CEdLinks::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CEdLinks::OnBuNew() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CSelLinkType SelLinkDlg;
	SelLinkDlg.DoModal();
	UpdateDialog();
	return; 
}


void CEdLinks::OnBuEdit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Linklist()->Size();
	int* indices = new int[max];
	int num = m_linklist.GetSelItems(max,indices);
	int linknum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQLINK* link = aqsys.Linklist()->Get(linknum);
	if ( link == 0 )
	{
		aqapp.ProgErr("Link not found");
		return;
	}
	EditLink(link,FALSE);

	UpdateDialog();
	return; 
}


void CEdLinks::OnBuDuplicate() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Linklist()->Size();
	int* indices = new int[max];
	int num = m_linklist.GetSelItems(max,indices);
	int linknum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQLINK* link = aqsys.Linklist()->Get(linknum);
	if ( link == 0 )
	{
		aqapp.ProgErr("Link not found");
		return;
	}
	EditLink(link,TRUE);

	UpdateDialog();
	return; 
}


void CEdLinks::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	switch ( MessageBox(aqapp.ini.T(11),              // DelLinks?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = aqsys.Linklist()->Size();
	int* indices = new int[max];
	int num = m_linklist.GetSelItems(max,indices);
	int j,k;
	AQLINK* link;
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
			link = aqsys.Linklist()->Get(indices[j]-k+1);
			if ( aqsys.DeleteLink(link) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(21)); // FailDelLink
				strcat(buffer," ");
				strcat(buffer,link->Symbol());
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


void CEdLinks::OnDblclkLbLinklist() 
{
	OnBuEdit();
	return;
}


void CEdLinks::OnSelchangeLbLinklist() 
{
	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSelLinkType dialog


CSelLinkType::CSelLinkType(CWnd* pParent /*=NULL*/)
	: CDialog(CSelLinkType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelLinkType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelLinkType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelLinkType)
	DDX_Control(pDX, IDC_RB_DIFFLINK, m_rb_difflink);
	DDX_Control(pDX, IDC_RB_ADVLINK, m_rb_advlink);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelLinkType, CDialog)
	//{{AFX_MSG_MAP(CSelLinkType)
	ON_BN_CLICKED(IDC_RB_ADVLINK, OnRbAdvlink)
	ON_BN_CLICKED(IDC_RB_DIFFLINK, OnRbDifflink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSelLinkType message handlers

BOOL CSelLinkType::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( aqsys.Complist()->Size() < 2 )
	{
		m_rb_difflink.EnableWindow(FALSE);
		if ( linktype == DiffLink )
		{
			linktype = AdvLink;
		}
	}
	switch ( linktype )
	{
	case AdvLink:
		m_rb_advlink.SetCheck(1);
		break;
	case DiffLink:
		m_rb_difflink.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CSelLinkType::OnOK() 
{
	EditLink(0,TRUE);
	
	CDialog::OnOK();
}


void CSelLinkType::OnRbAdvlink() 
{
	linktype = AdvLink;
	return;
}


void CSelLinkType::OnRbDifflink() 
{
	linktype = DiffLink;
	return;
}

