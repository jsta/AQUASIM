// edcomp_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "edcomp_w.h"
#include "edmix_w.h"
#include "edfilm_w.h"
#include "edadv_w.h"
#include "edsoil_w.h"
#include "edriv_w.h"
#include "edlake_w.h"
#include "edsed_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static AQCOMPTYPE comptype = MixComp;

static void EditComp(AQCOMP* com, BOOL add);

static void EditComp(AQCOMP* com, BOOL add)
{
	if ( com != 0 ) comptype = com->Type();
	switch ( comptype )
	{
	case MixComp:
		{
			CEdMix EdMixDlg;
			EdMixDlg.DoModal(com,add);
		}
		break;
	case FilmComp:
		{
			CEdFilm EdFilmDlg;
			EdFilmDlg.DoModal(com,add);
		}
		break;
	case AdvComp:
		{
			CEdAdv EdAdvDlg;
			EdAdvDlg.DoModal(com,add);
		}
		break;
	case SoilComp:
		{
			CEdSoil EdSoilDlg;
			EdSoilDlg.DoModal(com,add);
		}
		break;
	case RivComp:
		{
			CEdRiv EdRivDlg;
			EdRivDlg.DoModal(com,add);
		}
		break;
	case LakeComp:
		{
			CEdLake EdLakeDlg;
			EdLakeDlg.DoModal(com,add);
		}
		break;
	case SedComp:
		{
			CEdSed EdSedDlg;
			EdSedDlg.DoModal(com,add);
		}
		break;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdComps dialog


CEdComps::CEdComps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdComps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdComps)
	//}}AFX_DATA_INIT

	created = 0;
}


void CEdComps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdComps)
	DDX_Control(pDX, IDC_BU_NEW, m_new);
	DDX_Control(pDX, IDC_LB_COMPLIST, m_complist);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	DDX_Control(pDX, IDC_BU_ACT, m_bu_act);
	DDX_Control(pDX, IDC_BU_INACT, m_bu_inact);
	DDX_Control(pDX, IDC_TX_TYPE, m_tx_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdComps, CDialog)
	//{{AFX_MSG_MAP(CEdComps)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_BN_CLICKED(IDC_BU_ACT, OnBuAct)
	ON_BN_CLICKED(IDC_BU_INACT, OnBuInact)
	ON_LBN_DBLCLK(IDC_LB_COMPLIST, OnDblclkLbComplist)
	ON_LBN_SELCHANGE(IDC_LB_COMPLIST, OnSelchangeLbComplist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEdComps::UpdateActivation()
{
/*
	if ( aqsys.Varlist()->Size() > 0 )
	{
		m_new.EnableWindow(TRUE);
	}
	else
	{
		m_new.EnableWindow(FALSE);
	}
*/
	switch ( m_complist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(FALSE);
		m_bu_act.EnableWindow(FALSE);
		m_bu_inact.EnableWindow(FALSE);
		m_tx_type.SetWindowText("");
		break;
	case 1:
		m_duplicate.EnableWindow(TRUE);
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		{
			int max = aqsys.Complist()->Size();
			int* indices = new int[max];
			int num = m_complist.GetSelItems(max,indices);
			int compnum = indices[0]+1;
			delete [] indices;
			if ( num != 1 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			AQCOMP* com = aqsys.Complist()->Get(compnum);
			if ( com == 0 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			m_tx_type.SetWindowText(com->TypeName());
            if ( com->Active() == TRUE )
            {
               m_bu_act.EnableWindow(FALSE);
               m_bu_inact.EnableWindow(TRUE);
            }
            else
            {
               m_bu_act.EnableWindow(TRUE);
               m_bu_inact.EnableWindow(FALSE);
            }
		}
		break;
	default:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(TRUE);
		m_tx_type.SetWindowText("");
		{
			int max = aqsys.Complist()->Size();
			int* indices = new int[max];
			int num = m_complist.GetSelItems(max,indices);
            m_bu_act.EnableWindow(FALSE);
            m_bu_inact.EnableWindow(FALSE);
            for ( CARDINAL i=0; i<num; i++ )
			{
				AQCOMP* com = aqsys.Complist()->Get(indices[i]+1);
				if ( com->Active() == TRUE )
				{
				   m_bu_inact.EnableWindow(TRUE);
				}
				else
				{
				   m_bu_act.EnableWindow(TRUE);
				}
			}
			delete [] indices;
		}
        break;
   }
   return;
}


void CEdComps::UpdateDialog()
{
	if ( created == 0 ) return;
    char buffer[1024];

	int topindex = m_complist.GetTopIndex();
	m_complist.ResetContent();
	AQCOMP* com = aqsys.Complist()->First();
	while ( com != 0 )
	{
       buffer[0] = '\0';
       if ( com->Active() != TRUE ) strcpy(buffer,"   ");
       strcat(buffer,com->Symbol());
	   m_complist.InsertString(-1,buffer);
	   com = com->Next();
	}
	m_complist.SetTopIndex(topindex);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdComps message handlers

BOOL CEdComps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdComps::OnOK() // Close button
{
	DestroyWindow();
	return;
}


void CEdComps::OnCancel() // Close button 
{
	DestroyWindow();
	return;
}


void CEdComps::OnClose() // Close box
{
	DestroyWindow();

	CDialog::OnClose();
}


void CEdComps::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CEdComps::OnBuNew() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CSelCompDlg SelCompDlg;
	SelCompDlg.DoModal();
	aqapp.UpdateDialogs();
}


void CEdComps::OnBuEdit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Complist()->Size();
	int* indices = new int[max];
	int num = m_complist.GetSelItems(max,indices);
	int compnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQCOMP* com = aqsys.Complist()->Get(compnum);
	if ( com == 0 )
	{
		aqapp.ProgErr("Compartment not found");
		return;
	}
	EditComp(com,FALSE);

	aqapp.UpdateDialogs();
	return;
}


void CEdComps::OnBuDuplicate() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Complist()->Size();
	int* indices = new int[max];
	int num = m_complist.GetSelItems(max,indices);
	int compnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQCOMP* com = aqsys.Complist()->Get(compnum);
	if ( com == 0 )
	{
		aqapp.ProgErr("Compartment not found");
		return;
	}
	EditComp(com,TRUE);

	aqapp.UpdateDialogs();
	return;
}


void CEdComps::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	switch ( MessageBox(aqapp.ini.T(10),              // DelComps?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = aqsys.Complist()->Size();
	int* indices = new int[max];
	int num = m_complist.GetSelItems(max,indices);
	int j,k;
	AQCOMP* com;
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
			com = aqsys.Complist()->Get(indices[j]-k+1);
			if ( aqsys.DeleteComp(com) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(20)); // FailDelComp
				strcat(buffer," ");
				strcat(buffer,com->Symbol());
				MessageBox(buffer,
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	aqapp.UpdateDialogs();
}


void CEdComps::OnBuAct() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Complist()->Size();
	int* indices = new int[max];
	int num = m_complist.GetSelItems(max,indices);
	int j;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("Illegal list box selection");
		break;
	default:
		for ( j=0; j<num; j++ )
		{
            aqsys.Activate(aqsys.Complist()->Get(indices[j]+1));
		}
		break;
	}
	delete [] indices;

	aqapp.UpdateDialogs();
}


void CEdComps::OnBuInact() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Complist()->Size();
	int* indices = new int[max];
	int num = m_complist.GetSelItems(max,indices);
	int j;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("Illegal list box selection");
		break;
	default:
		for ( j=0; j<num; j++ )
		{
            aqsys.Inactivate(aqsys.Complist()->Get(indices[j]+1));
		}
		break;
	}
	delete [] indices;

	aqapp.UpdateDialogs();
}


void CEdComps::OnDblclkLbComplist() 
{
	OnBuEdit();
}


void CEdComps::OnSelchangeLbComplist() 
{
	UpdateActivation();
}


/////////////////////////////////////////////////////////////////////////////
// CSelCompDlg dialog


CSelCompDlg::CSelCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelCompDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelCompDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelCompDlg)
	DDX_Control(pDX, IDC_RB_MIXEDCOMP, m_mixcomp);
	DDX_Control(pDX, IDC_RB_FILMCOMP,  m_filmcomp);
	DDX_Control(pDX, IDC_RB_ADVCOMP,   m_advcomp);
	DDX_Control(pDX, IDC_RB_SOILCOMP,  m_soilcomp);
	DDX_Control(pDX, IDC_RB_RIVCOMP,   m_rivcomp);
	DDX_Control(pDX, IDC_RB_LAKECOMP,  m_lakecomp);
	DDX_Control(pDX, IDC_RB_SEDCOMP,   m_sedcomp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelCompDlg, CDialog)
	//{{AFX_MSG_MAP(CSelCompDlg)
	ON_BN_CLICKED(IDC_RB_LAKECOMP, OnLakecomp)
	ON_BN_CLICKED(IDC_RB_MIXEDCOMP, OnMixedcomp)
	ON_BN_CLICKED(IDC_RB_FILMCOMP, OnFilmcomp)
	ON_BN_CLICKED(IDC_RB_ADVCOMP, OnAdvcomp)
	ON_BN_CLICKED(IDC_RB_RIVCOMP, OnRivcomp)
	ON_BN_CLICKED(IDC_RB_SOILCOMP, OnSoilcomp)
	ON_BN_CLICKED(IDC_RB_SEDCOMP,  OnSedcomp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSelCompDlg message handlers

BOOL CSelCompDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	switch ( comptype )
	{
	case MixComp:
		m_mixcomp.SetCheck(1);
		break;
	case FilmComp:
		m_filmcomp.SetCheck(1);
		break;
	case AdvComp:
		m_advcomp.SetCheck(1);
		break;
	case SoilComp:
		m_soilcomp.SetCheck(1);
		break;
	case RivComp:
		m_rivcomp.SetCheck(1);
		break;
	case LakeComp:
		m_lakecomp.SetCheck(1);
		break;		
	case SedComp:
		m_sedcomp.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CSelCompDlg::OnOK() 
{
	EditComp(0,TRUE);

	CDialog::OnOK();
}


void CSelCompDlg::OnMixedcomp() 
{
	comptype = MixComp;
	return;
}


void CSelCompDlg::OnFilmcomp() 
{
	comptype = FilmComp;
	return;
}


void CSelCompDlg::OnAdvcomp() 
{
	comptype = AdvComp;
	return;
}


void CSelCompDlg::OnSoilcomp() 
{
	comptype = SoilComp;
	return;
}


void CSelCompDlg::OnRivcomp() 
{
	comptype = RivComp;
	return;
}


void CSelCompDlg::OnLakecomp() 
{
	comptype = LakeComp;
	return;
}


void CSelCompDlg::OnSedcomp() 
{
	comptype = SedComp;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdCompActVar dialog


CEdCompActVar::CEdCompActVar(CWnd* pParent /*=NULL*/)
	: CDialog(CEdCompActVar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdCompActVar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdCompActVar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdCompActVar)
	DDX_Control(pDX, IDC_LB_AVAIL, m_lb_avail);
	DDX_Control(pDX, IDC_LB_ACT, m_lb_act);
	DDX_Control(pDX, IDC_BU_INACT, m_bu_inact);
	DDX_Control(pDX, IDC_BU_ACT, m_bu_act);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdCompActVar, CDialog)
	//{{AFX_MSG_MAP(CEdCompActVar)
	ON_LBN_SELCHANGE(IDC_LB_ACT, OnSelchangeLbAct)
	ON_LBN_DBLCLK(IDC_LB_ACT, OnDblclkLbAct)
	ON_LBN_SELCHANGE(IDC_LB_AVAIL, OnSelchangeLbAvail)
	ON_LBN_DBLCLK(IDC_LB_AVAIL, OnDblclkLbAvail)
	ON_BN_CLICKED(IDC_BU_ACT, OnBuAct)
	ON_BN_CLICKED(IDC_BU_INACT, OnBuInact)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdCompActVar::DoModal(AQCOMP* com)
{
	xcomp = com;
	return CDialog::DoModal();
}


void CEdCompActVar::UpdateDialog()
{
	int topindex = m_lb_act.GetTopIndex();
	m_lb_act.ResetContent();
	for ( CARDINAL i=0; i<xcomp->NumActVar(); i++ )
	{
		m_lb_act.InsertString(-1,xcomp->ActVar(i)->Symbol());
	}
	m_lb_act.SetTopIndex(topindex);

	UpdateActivation();
}


void CEdCompActVar::UpdateActivation()
{
	switch ( m_lb_act.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_inact.EnableWindow(FALSE);
		break;
	default:
		m_bu_inact.EnableWindow(TRUE);
		break;
	}

	CARDINAL i = 0;
	BOOLEAN found = FALSE; BOOLEAN active;
	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_lb_avail.GetSelItems(max,indices);
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_act.EnableWindow(FALSE);
		break;
	default:
		for ( i=0; i<num; i++ )
		{
			active = FALSE;
			for ( CARDINAL j=0; j<xcomp->NumActVar(); j++ )
			{
				if ( aqsys.Varlist()->Get(indices[i]+1) == xcomp->ActVar(j) )
				{
					active = TRUE;
					break;
				}
			}
			if ( active == FALSE )
			{
				found = TRUE;
				break;
			}
		}
		if ( found == TRUE )
		{
			m_bu_act.EnableWindow(TRUE);
		}
		else
		{
			m_bu_act.EnableWindow(FALSE);
		}
		break;
	}
	delete [] indices;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdCompActVar message handlers

BOOL CEdCompActVar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int topindex = m_lb_avail.GetTopIndex();
	m_lb_avail.ResetContent();
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
		m_lb_avail.InsertString(-1,var->Symbol());
		var = var->Next();
	}
	m_lb_avail.SetTopIndex(topindex);
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdCompActVar::OnSelchangeLbAct() 
{
	UpdateActivation();
	return;
}


void CEdCompActVar::OnDblclkLbAct() 
{
	OnBuInact();
	return;
}


void CEdCompActVar::OnSelchangeLbAvail() 
{
	UpdateActivation();
	return;
}


void CEdCompActVar::OnDblclkLbAvail() 
{
	OnBuAct();
	return;
}


void CEdCompActVar::OnBuAct() 
{
	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_lb_avail.GetSelItems(max,indices);
	int pos;
	int n = m_lb_act.GetSelItems(1,&pos);
	for ( CARDINAL i=0; i<num; i++ )
	{
		if ( xcomp->AddActVar(aqsys.Varlist()->Get(indices[i]+1),pos) == TRUE )
		{
			pos++;
		}
	}
	delete indices;
	UpdateDialog();
	return;
}


void CEdCompActVar::OnBuInact() 
{
	int max = xcomp->NumActVar();
	int* indices = new int[max];
	int num = m_lb_act.GetSelItems(max,indices);
	int k = 0;
	for ( CARDINAL i=0; i<num; i++ )
	{
		if ( xcomp->RemoveActVar(indices[i]-k) == TRUE )
		{
			k++;
		}
	}
	delete indices;
	UpdateDialog();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdCompActProc dialog


CEdCompActProc::CEdCompActProc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdCompActProc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdCompActProc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdCompActProc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdCompActProc)
	DDX_Control(pDX, IDC_LB_AVAIL, m_lb_avail);
	DDX_Control(pDX, IDC_LB_ACT, m_lb_act);
	DDX_Control(pDX, IDC_BU_INACT, m_bu_inact);
	DDX_Control(pDX, IDC_BU_ACT, m_bu_act);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdCompActProc, CDialog)
	//{{AFX_MSG_MAP(CEdCompActProc)
	ON_BN_CLICKED(IDC_BU_ACT, OnBuAct)
	ON_BN_CLICKED(IDC_BU_INACT, OnBuInact)
	ON_LBN_SELCHANGE(IDC_LB_ACT, OnSelchangeLbAct)
	ON_LBN_DBLCLK(IDC_LB_ACT, OnDblclkLbAct)
	ON_LBN_SELCHANGE(IDC_LB_AVAIL, OnSelchangeLbAvail)
	ON_LBN_DBLCLK(IDC_LB_AVAIL, OnDblclkLbAvail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdCompActProc::DoModal(AQCOMP* com)
{
	xcomp = com;
	return CDialog::DoModal();
}


void CEdCompActProc::UpdateDialog()
{
	int topindex = m_lb_act.GetTopIndex();
	m_lb_act.ResetContent();
	for ( CARDINAL i=0; i<xcomp->NumActProc(); i++ )
	{
		m_lb_act.InsertString(-1,xcomp->ActProc(i)->Symbol());
	}
	m_lb_act.SetTopIndex(topindex);

	UpdateActivation();
}


void CEdCompActProc::UpdateActivation()
{
	switch ( m_lb_act.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_inact.EnableWindow(FALSE);
		break;
	default:
		m_bu_inact.EnableWindow(TRUE);
		break;
	}

	CARDINAL i = 0;
	BOOLEAN found = FALSE; BOOLEAN active;
	int max = aqsys.Proclist()->Size();
	int* indices = new int[max];
	int num = m_lb_avail.GetSelItems(max,indices);
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_act.EnableWindow(FALSE);
		break;
	default:
		for ( i=0; i<num; i++ )
		{
			active = FALSE;
			for ( CARDINAL j=0; j<xcomp->NumActProc(); j++ )
			{
				if ( aqsys.Proclist()->Get(indices[i]+1) == xcomp->ActProc(j) )
				{
					active = TRUE;
					break;
				}
			}
			if ( active == FALSE )
			{
				found = TRUE;
				break;
			}
		}
		if ( found == TRUE )
		{
			m_bu_act.EnableWindow(TRUE);
		}
		else
		{
			m_bu_act.EnableWindow(FALSE);
		}
		break;
	}
	delete [] indices;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdCompActProc message handlers

BOOL CEdCompActProc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int topindex = m_lb_avail.GetTopIndex();
	m_lb_avail.ResetContent();
	AQPROC* proc = aqsys.Proclist()->First();
	while ( proc != 0 )
	{
		m_lb_avail.InsertString(-1,proc->Symbol());
		proc = proc->Next();
	}
	m_lb_avail.SetTopIndex(topindex);
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdCompActProc::OnSelchangeLbAct() 
{
	UpdateActivation();
	return;
}


void CEdCompActProc::OnDblclkLbAct() 
{
	OnBuInact();
	return;
}


void CEdCompActProc::OnSelchangeLbAvail() 
{
	UpdateActivation();
	return;
}


void CEdCompActProc::OnDblclkLbAvail() 
{
	OnBuAct();
	return;
}


void CEdCompActProc::OnBuAct() 
{
	int max = aqsys.Proclist()->Size();
	int* indices = new int[max];
	int num = m_lb_avail.GetSelItems(max,indices);
	int pos;
	int n = m_lb_act.GetSelItems(1,&pos);
	for ( CARDINAL i=0; i<num; i++ )
	{
		if ( xcomp->AddActProc(aqsys.Proclist()->Get(indices[i]+1),pos) == TRUE )
		{
			pos++;
		}
	}
	delete indices;
	UpdateDialog();
	return;
}


void CEdCompActProc::OnBuInact() 
{
	int max = xcomp->NumActProc();
	int* indices = new int[max];
	int num = m_lb_act.GetSelItems(max,indices);
	int k = 0;
	for ( CARDINAL i=0; i<num; i++ )
	{
		if ( xcomp->RemoveActProc(indices[i]-k) == TRUE )
		{
			k++;
		}
	}
	delete indices;
	UpdateDialog();
	return;
}

