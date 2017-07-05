// edvar_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "edvar_w.h"
#include "edstat_w.h"
#include "edprog_w.h"
#include "edpar_w.h"
#include "edrlis_w.h"
#include "edvlis_w.h"
#include "edform_w.h"
#include "edprob_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static AQVARTYPE vartype = StateVar;

static void EditVar(AQVAR* var, BOOL add, BOOLEAN changetype=FALSE);

static void EditVar(AQVAR* var, BOOL add, BOOLEAN changetype)
{
	if ( var!=0 && changetype==FALSE ) vartype = var->Type();
	switch ( vartype )
	{
	case StateVar:
		{
			CEdVarState EdVarStateDlg;
			EdVarStateDlg.DoModal(var,add);
		}
		break;
	case ProgVar:
		{
			CEdVarProg EdVarProgDlg;
			EdVarProgDlg.DoModal(var,add);
		}
		break;
	case ConstVar:
		{
			CEdVarPar EdVarParDlg;
			EdVarParDlg.DoModal(var,add);
		}
		break;
	case RealListVar:
		{
			CEdVarRealList EdVarRealListDlg;
			EdVarRealListDlg.DoModal(var,add);
		}
		break;
	case VarListVar:
		{
			CEdVarVarList EdVarVarListDlg;
			EdVarVarListDlg.DoModal(var,add);
		}
		break;
	case FormVar:
		{
			CEdVarForm EdVarFormDlg;
			EdVarFormDlg.DoModal(var,add);
		}
		break;
	case ProbeVar:
		{
			CEdVarProbe EdVarProbeDlg;
			EdVarProbeDlg.DoModal(var,add);
		}
		break;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdVars dialog


CEdVars::CEdVars(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVars::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVars)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CEdVars::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVars)
	DDX_Control(pDX, IDC_BU_NEW, m_new);
	DDX_Control(pDX, IDC_BU_EXCHANGE, m_exchange);
	DDX_Control(pDX, IDC_BU_EDITTYPE, m_edittype);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_LB_VARLIST, m_varlist);
	DDX_Control(pDX, IDC_TX_TYPE, m_tx_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVars, CDialog)
	//{{AFX_MSG_MAP(CEdVars)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_LBN_SELCHANGE(IDC_LB_VARLIST, OnSelchangeLbVarlist)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_EDITTYPE, OnBuEdittype)
	ON_BN_CLICKED(IDC_BU_EXCHANGE, OnBuExchange)
	ON_LBN_DBLCLK(IDC_LB_VARLIST, OnDblclkLbVarlist)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEdVars::UpdateActivation()
{
	switch ( m_varlist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_edittype.EnableWindow(FALSE);
		m_exchange.EnableWindow(FALSE);
		m_delete.EnableWindow(FALSE);
		m_tx_type.SetWindowText("");
		break;
	case 1:
		m_duplicate.EnableWindow(TRUE);
		m_edit.EnableWindow(TRUE);
		m_edittype.EnableWindow(TRUE);
		m_exchange.EnableWindow(FALSE);
		m_delete.EnableWindow(TRUE);
		{
			int max = aqsys.Varlist()->Size();
			int* indices = new int[max];
			int num = m_varlist.GetSelItems(max,indices);
			int varnum = indices[0]+1;
			delete [] indices;
			if ( num != 1 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			AQVAR* var = aqsys.Varlist()->Get(varnum);
			if ( var == 0 )
			{
				m_tx_type.SetWindowText("");
				break;
			}
			m_tx_type.SetWindowText(var->TypeName());
		}
		break;
	case 2:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_edittype.EnableWindow(FALSE);
		m_exchange.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		m_tx_type.SetWindowText("");
		break;
	default:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_edittype.EnableWindow(FALSE);
		m_exchange.EnableWindow(FALSE);
		m_delete.EnableWindow(TRUE);
		m_tx_type.SetWindowText("");
	   break;
	}
	return;
}


void CEdVars::UpdateDialog()
{
	if ( created == 0 ) return;

	int topindex = m_varlist.GetTopIndex();
	m_varlist.ResetContent();
	AQVAR* var = aqsys.Varlist()->First();
	while ( var != 0 )
	{
	   m_varlist.InsertString(-1,var->Symbol());
	   var = var->Next();
	}
	m_varlist.SetTopIndex(topindex);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdVars message handlers

BOOL CEdVars::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVars::OnOK() // Close button
{
	DestroyWindow();
	return;
}


void CEdVars::OnCancel() // Close button 
{
	DestroyWindow();
	return;
}


void CEdVars::OnClose() // Close box
{
	DestroyWindow();

	CDialog::OnClose();
}


void CEdVars::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CEdVars::OnBuNew() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CSelVarDlg SelVarDlg;
	SelVarDlg.DoModal(0);
	aqapp.UpdateDialogs();
	return; 
}


void CEdVars::OnBuEdit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_varlist.GetSelItems(max,indices);
	int varnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQVAR* var = aqsys.Varlist()->Get(varnum);
	if ( var == 0 )
	{
		aqapp.ProgErr("Variable not found");
		return;
	}
	EditVar(var,FALSE);

	aqapp.UpdateDialogs();
	return; 
}


void CEdVars::OnBuDuplicate() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_varlist.GetSelItems(max,indices);
	int varnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQVAR* var = aqsys.Varlist()->Get(varnum);
	if ( var == 0 )
	{
		aqapp.ProgErr("Variable not found");
		return;
	}
	EditVar(var,TRUE);

	aqapp.UpdateDialogs();
	return; 
}


void CEdVars::OnBuEdittype() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_varlist.GetSelItems(max,indices);
	int varnum = indices[0]+1;
	delete [] indices;
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	AQVAR* var = aqsys.Varlist()->Get(varnum);
	if ( var == 0 )
	{
		aqapp.ProgErr("Variable not found");
		return;
	}
	vartype = var->Type();
	CSelVarDlg SelVarDlg;
	SelVarDlg.DoModal(var);

	aqapp.UpdateDialogs();
	return; 
}


void CEdVars::OnBuExchange() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_varlist.GetSelItems(max,indices);
	AQVAR* var1; AQVAR* var2;
	switch ( num )
	{
	case 2:
		var1 = aqsys.Varlist()->Get(indices[0]+1);
		var2 = aqsys.Varlist()->Get(indices[1]+1);
		if ( aqsys.ExchangeVar(var1,var2) == TRUE )
		{
			MessageBox(aqapp.ini.T(357),
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONEXCLAMATION);
		}
		else
		{
			MessageBox(aqapp.ini.T(358),
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONEXCLAMATION);
		}
		break;
	default:
		aqapp.ProgErr("Illegal list box selection");
		break;
	}
	delete [] indices;

	aqapp.UpdateDialogs();
	return; 
}


void CEdVars::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	switch ( MessageBox(aqapp.ini.T(8),              // DelVars?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = aqsys.Varlist()->Size();
	int* indices = new int[max];
	int num = m_varlist.GetSelItems(max,indices);
	int j,k;
	AQVAR* var;
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
			var = aqsys.Varlist()->Get(indices[j]-k+1);
			if ( aqsys.DeleteVar(var) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(18)); // FailDelVar
				strcat(buffer," ");
				strcat(buffer,var->Symbol());
				MessageBox(buffer,
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	aqapp.UpdateDialogs();
	return; 
}


void CEdVars::OnSelchangeLbVarlist() 
{
	UpdateActivation();
	return; 
}


void CEdVars::OnDblclkLbVarlist() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSelVarDlg dialog


CSelVarDlg::CSelVarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelVarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelVarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelVarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelVarDlg)
	DDX_Control(pDX, IDC_RB_STATEVAR, m_statevar);
	DDX_Control(pDX, IDC_RB_PROGVAR,  m_progvar);
	DDX_Control(pDX, IDC_RB_PARVAR,   m_parvar);
	DDX_Control(pDX, IDC_RB_REALLISTVAR,  m_reallistvar);
	DDX_Control(pDX, IDC_RB_VARLISTVAR,   m_varlistvar);
	DDX_Control(pDX, IDC_RB_FORMVAR,  m_formvar);
	DDX_Control(pDX, IDC_RB_PROBEVAR,  m_probevar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelVarDlg, CDialog)
	//{{AFX_MSG_MAP(CSelVarDlg)
	ON_BN_CLICKED(IDC_RB_FORMVAR, OnRbFormvar)
	ON_BN_CLICKED(IDC_RB_PARVAR, OnRbParvar)
	ON_BN_CLICKED(IDC_RB_PROBEVAR, OnRbProbevar)
	ON_BN_CLICKED(IDC_RB_PROGVAR, OnRbProgvar)
	ON_BN_CLICKED(IDC_RB_REALLISTVAR, OnRbReallistvar)
	ON_BN_CLICKED(IDC_RB_STATEVAR, OnRbStatevar)
	ON_BN_CLICKED(IDC_RB_VARLISTVAR, OnRbVarlistvar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CSelVarDlg::DoModal(AQVAR* v)
{
	var = v;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CSelVarDlg message handlers

BOOL CSelVarDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CARDINAL num = aqsys.Varlist()->Size();
	if ( ( var==0 && num==0 ) || ( var!=0 && num==1 ) )
	{
		m_reallistvar.EnableWindow(FALSE);
		m_varlistvar.EnableWindow(FALSE);
		m_probevar.EnableWindow(FALSE);
		if ( vartype==RealListVar || vartype==VarListVar )
		{
			vartype = StateVar;
		}
	}
	if ( aqsys.Complist()->Size() == 0 )
	{
		m_probevar.EnableWindow(FALSE);
		if ( vartype == ProbeVar )
		{
			vartype = StateVar;
		}
	}
	switch ( vartype )
	{
	case StateVar:
		m_statevar.SetCheck(1);
		break;
	case ProgVar:
		m_progvar.SetCheck(1);
		break;
	case ConstVar:
		m_parvar.SetCheck(1);
		break;
	case RealListVar:
		m_reallistvar.SetCheck(1);
		break;
	case VarListVar:
		m_varlistvar.SetCheck(1);
		break;
	case FormVar:
		m_formvar.SetCheck(1);
		break;
	case ProbeVar:
		m_probevar.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CSelVarDlg::OnOK() 
{
	if ( var == 0 )  // new
	{
		EditVar(var,TRUE);
	}
	else             // edit type
	{
		EditVar(var,FALSE,TRUE);
	}
	
	CDialog::OnOK();
}


void CSelVarDlg::OnRbStatevar() 
{
	vartype = StateVar;
	return;
}


void CSelVarDlg::OnRbProgvar() 
{
	vartype = ProgVar;
	return;
}


void CSelVarDlg::OnRbParvar() 
{
	vartype = ConstVar;
	return;
}


void CSelVarDlg::OnRbReallistvar() 
{
	vartype = RealListVar;
	return;
}


void CSelVarDlg::OnRbVarlistvar() 
{
	vartype = VarListVar;
	return;
}


void CSelVarDlg::OnRbFormvar() 
{
	vartype = FormVar;
	return;
}


void CSelVarDlg::OnRbProbevar() 
{
	vartype = ProbeVar;
	return;
}




