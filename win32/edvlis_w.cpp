// edvlis_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edvlis_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarVarList dialog


CEdVarVarList::CEdVarVarList(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarVarList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarVarList)
	//}}AFX_DATA_INIT
}


void CEdVarVarList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarVarList)
	DDX_Control(pDX, IDC_BU_REPLVAL, m_bu_replace);
	DDX_Control(pDX, IDC_LE_VALDATA, m_le_valdata);
	DDX_Control(pDX, IDC_TX_WIDTH, m_tx_width);
	DDX_Control(pDX, IDC_TX_NUMPAIRS, m_tx_numpairs);
	DDX_Control(pDX, IDC_RB_SPLINE, m_rb_spline);
	DDX_Control(pDX, IDC_RB_SMOOTH, m_rb_smooth);
	DDX_Control(pDX, IDC_RB_LINEAR, m_rb_linear);
	DDX_Control(pDX, IDC_LE_ARG, m_le_arg);
	DDX_Control(pDX, IDC_LB_LIST, m_lb_list);
	DDX_Control(pDX, IDC_ED_WIDTH, m_ed_width);
	DDX_Control(pDX, IDC_ED_ARGDATA, m_ed_argdata);
	DDX_Control(pDX, IDC_BU_ADDVAL, m_bu_add);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarVarList, CDialog)
	//{{AFX_MSG_MAP(CEdVarVarList)
	ON_BN_CLICKED(IDC_BU_ADDVAL, OnBuAddval)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_DBLCLK(IDC_LB_LIST, OnDblclkLbList)
	ON_LBN_SELCHANGE(IDC_LB_LIST, OnSelchangeLbList)
	ON_BN_CLICKED(IDC_RB_LINEAR, OnRbLinear)
	ON_BN_CLICKED(IDC_RB_SMOOTH, OnRbSmooth)
	ON_BN_CLICKED(IDC_RB_SPLINE, OnRbSpline)
	ON_BN_CLICKED(IDC_BU_REPLVAL, OnBuReplval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarVarList::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	vlvar = new VARLISTVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


void CEdVarVarList::UpdateDialog()
{
	char buffer[1024]; char* dist = "       ";
	int topindex = m_lb_list.GetTopIndex();
	m_lb_list.ResetContent();
	for ( CARDINAL i=0; i<vlvar->NumPairs(); i++ )
	{
		NumToStr(vlvar->X()[i],buffer,sizeof(buffer),TRUE,30);
		strcat(buffer,dist);
		strcat(buffer,vlvar->Yvar()[i]->Symbol());
		m_lb_list.InsertString(-1,buffer);
	}
	m_lb_list.SetTopIndex(topindex);
	NumToString(vlvar->NumPairs(),buffer,sizeof(buffer));
	m_tx_numpairs.SetWindowText(buffer);

	UpdateActivation();
	return;
}


void CEdVarVarList::UpdateActivation()
{
	switch ( m_lb_list.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_bu_delete.EnableWindow(FALSE);
		m_bu_replace.EnableWindow(FALSE);
		break;
	case 1:
		m_bu_delete.EnableWindow(TRUE);
		m_bu_replace.EnableWindow(TRUE);
		break;
	default:
		m_bu_delete.EnableWindow(TRUE);
		m_bu_replace.EnableWindow(FALSE);
		break;
	}

	if ( vlvar->Method() == smooth )
	{
		m_tx_width.EnableWindow(TRUE);
		m_ed_width.EnableWindow(TRUE);
	}
	else
	{
		m_tx_width.EnableWindow(FALSE);
		m_ed_width.EnableWindow(FALSE);
	}

	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdVarVarList message handlers

BOOL CEdVarVarList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	if ( vlvar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(vlvar->Symbol());
	}

	if ( vlvar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(vlvar->Description());
	}

	if ( vlvar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(vlvar->Unit());
	}

	m_le_arg.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_arg.AddString(var->Symbol());
			var = var->Next();
		}
	}
	if ( vlvar->GetArg() != 0 )
	{
		int pos = aqsys.Varlist()->Position(vlvar->GetArg());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Argument not found");
		}
		else
		{
			m_le_arg.SetCurSel(pos-1);
		}
	}
	else
	{
		m_le_arg.SetCurSel(0);
	}

	m_le_valdata.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_le_valdata.AddString(var->Symbol());
			var = var->Next();
		}
	}
	m_le_valdata.SetCurSel(0);

	switch ( vlvar->Method() )
	{
	case linear:
		m_rb_linear.SetCheck(1);
		break;
	case spline:
		m_rb_spline.SetCheck(1);
		break;
	case smooth:
		m_rb_smooth.SetCheck(1);
		break;
	}

	NumToString(vlvar->SmoothWidth(),buffer,sizeof(buffer));
	m_ed_width.SetWindowText(buffer);

	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarVarList::OnCancel() 
{
	delete vlvar;
	
	CDialog::OnCancel();
}


void CEdVarVarList::OnOK() 
{
	char buffer[1024];
	int i;
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( vlvar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(257),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	vlvar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	vlvar->Unit(buffer);

	i = m_le_arg.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(349), // IllegalArg
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		AQVAR* var = aqsys.Varlist()->Get(i+1);
		if ( var == 0 )
		{
			aqapp.ProgErr("Variable not found");
			return;
		}
		vlvar->SetArg(var);
	}

	if ( vlvar->NumPairs() == 0 )
	{
		MessageBox(aqapp.ini.T(360), // NoDataPairs
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( vlvar->Method() == smooth )
	{
		REAL w;
		m_ed_width.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&w) == FALSE )
		{
			MessageBox(aqapp.ini.T(350), // FailReadWidth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( vlvar->SmoothWidth(w) == FALSE )
		{
			MessageBox(aqapp.ini.T(351), // IllegalWidth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		REAL w;
		m_ed_width.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&w) == TRUE )
		{
			vlvar->SmoothWidth(w);
		}
	}

	if ( add == TRUE )
	{
		if ( aqsys.AddVar(vlvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(258),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,vlvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(259),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdVarVarList::OnBuAddval() 
{
	char buffer[1024];
	int i;
	
	REAL x; AQVAR* y;
	m_ed_argdata.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&x) == FALSE )
	{
		MessageBox(aqapp.ini.T(352),
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	i = m_le_valdata.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(353), // IllegalVal
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		y = aqsys.Varlist()->Get(i+1);
		if ( y == 0 )
		{
			aqapp.ProgErr("Value not found");
			return;
		}
	}
	if ( vlvar->Add(x,y) == TRUE )
	{
		m_ed_argdata.SetWindowText("");
		UpdateDialog();
		int topindex = vlvar->ArgIndex(x)-3;
		if ( topindex < 0 ) topindex = 0;
		m_lb_list.SetTopIndex(topindex);
	}
	else
	{
		MessageBox(aqapp.ini.T(354),  // FailAddData
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
	}
	return;
}


void CEdVarVarList::OnBuReplval() 
{
	char buffer[1024];
	int i;

	int max = vlvar->NumPairs();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);
	int pos = indices[0];
	delete indices;

	if ( num == 1 )
	{
		REAL x; AQVAR* y;
		m_ed_argdata.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&x) == FALSE )
		{
			MessageBox(aqapp.ini.T(352),
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		i = m_le_valdata.GetCurSel();
		if ( i == CB_ERR )
		{
			MessageBox(aqapp.ini.T(353), // IllegalVal
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			y = aqsys.Varlist()->Get(i+1);
			if ( y == 0 )
			{
				aqapp.ProgErr("Value not found");
				return;
			}
		}
		if ( vlvar->Replace(pos,x,y) == TRUE )
		{
			m_ed_argdata.SetWindowText("");
			UpdateDialog();
			if ( vlvar->ArgIndex(x) != pos )
			{
				int topindex = vlvar->ArgIndex(x)-3;
				if ( topindex < 0 ) topindex = 0;
				m_lb_list.SetTopIndex(topindex);
			}
		}
		else
		{
			MessageBox(aqapp.ini.T(355),  // FailReplaceData
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
		}
	}
	return;
}


void CEdVarVarList::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(356),  // DelSelData?
			 aqapp.ini.T(111),
			 MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = vlvar->NumPairs();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);
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
			if ( vlvar->Delete(indices[j]-k) == TRUE )
			{
				k++;
			}
		}
		break;
	}
	delete [] indices;
	UpdateDialog();
	return;
}


void CEdVarVarList::OnDblclkLbList() 
{
	return;	
}


void CEdVarVarList::OnSelchangeLbList() 
{
	char buffer[1024];

	int max = vlvar->NumPairs();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);

	if ( num == 1 )
	{
		int i = indices[0];
		if ( i>=0 && i<vlvar->NumPairs() )
		{
			NumToString(vlvar->X()[i],buffer,sizeof(buffer));
			m_ed_argdata.SetWindowText(buffer);
			int pos = aqsys.Varlist()->Position(vlvar->Yvar()[i]);
			m_le_valdata.SetCurSel(pos-1);
		}
	}
	delete [] indices;
	UpdateActivation();
	return;
}


void CEdVarVarList::OnRbLinear() 
{
	vlvar->Method(linear);
	UpdateActivation();
	return;
}


void CEdVarVarList::OnRbSmooth() 
{
	vlvar->Method(smooth);
	UpdateActivation();
	return;
}


void CEdVarVarList::OnRbSpline() 
{
	vlvar->Method(spline);
	UpdateActivation();
	return;
}



