// edrlis_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edrlis_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdVarRealList dialog


CEdVarRealList::CEdVarRealList(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarRealList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarRealList)
	//}}AFX_DATA_INIT
}


void CEdVarRealList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarRealList)
	DDX_Control(pDX, IDC_BU_REPLVAL, m_bu_replace);
	DDX_Control(pDX, IDC_TX_STDDATA, m_tx_stddata);
	DDX_Control(pDX, IDC_RB_INDIVIDUAL, m_rb_individual);
	DDX_Control(pDX, IDC_RB_GLOBAL, m_rb_global);
	DDX_Control(pDX, IDC_TX_WIDTH, m_tx_width);
	DDX_Control(pDX, IDC_TX_RELSTD, m_tx_relstd);
	DDX_Control(pDX, IDC_TX_NUMPAIRS, m_tx_numpairs);
	DDX_Control(pDX, IDC_TX_ABSSTD, m_tx_absstd);
	DDX_Control(pDX, IDC_RB_SPLINE, m_rb_spline);
	DDX_Control(pDX, IDC_RB_SMOOTH, m_rb_smooth);
	DDX_Control(pDX, IDC_RB_LINEAR, m_rb_linear);
	DDX_Control(pDX, IDC_LE_ARG, m_le_arg);
	DDX_Control(pDX, IDC_LB_LIST, m_lb_list);
	DDX_Control(pDX, IDC_ED_WIDTH, m_ed_width);
	DDX_Control(pDX, IDC_ED_VALDATA, m_ed_valdata);
	DDX_Control(pDX, IDC_ED_STDDATA, m_ed_stddata);
	DDX_Control(pDX, IDC_ED_RELSTD, m_ed_relstd);
	DDX_Control(pDX, IDC_ED_MIN, m_ed_min);
	DDX_Control(pDX, IDC_ED_MAX, m_ed_max);
	DDX_Control(pDX, IDC_ED_ARGDATA, m_ed_argdata);
	DDX_Control(pDX, IDC_ED_ABSSTD, m_ed_absstd);
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_BU_WRITE, m_bu_write);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADDVAL, m_bu_add);
	DDX_Control(pDX, IDC_BU_READ, m_bu_read);
	DDX_Control(pDX, IDC_ED_UNIT, m_ed_unit);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarRealList, CDialog)
	//{{AFX_MSG_MAP(CEdVarRealList)
	ON_BN_CLICKED(IDC_BU_ADDVAL, OnBuAddval)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_READ, OnBuRead)
	ON_BN_CLICKED(IDC_BU_WRITE, OnBuWrite)
	ON_LBN_SELCHANGE(IDC_LB_LIST, OnSelchangeLbList)
	ON_BN_CLICKED(IDC_RB_GLOBAL, OnRbGlobal)
	ON_BN_CLICKED(IDC_RB_INDIVIDUAL, OnRbIndividual)
	ON_BN_CLICKED(IDC_RB_LINEAR, OnRbLinear)
	ON_BN_CLICKED(IDC_RB_SMOOTH, OnRbSmooth)
	ON_BN_CLICKED(IDC_RB_SPLINE, OnRbSpline)
	ON_BN_CLICKED(IDC_BU_REPLVAL, OnBuReplval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarRealList::DoModal(AQVAR* var, BOOL a)
{
	oldvar = var;
	rlvar = new REALLISTVAR(oldvar);
	add = a;
	return CDialog::DoModal();
}


void CEdVarRealList::UpdateDialog()
{
	char buffer[1024], buffer1[1024];
	char dist[2]; dist[0] = ' '; dist[1] = '\0';
	int topindex = m_lb_list.GetTopIndex();
	m_lb_list.ResetContent();
	for ( CARDINAL i=0; i<rlvar->NumPairs(); i++ )
	{
		NumToStr(rlvar->X()[i],buffer,sizeof(buffer),TRUE,30);
		strcat(buffer,dist);
		NumToStr(rlvar->Y()[i],buffer1,sizeof(buffer1),TRUE,30);
		strcat(buffer,buffer1);
		if ( rlvar->StDevGlobal() == FALSE )
		{
			strcat(buffer,dist);
			NumToStr(rlvar->GetStDev(i),buffer1,sizeof(buffer1),TRUE,30);
			strcat(buffer,buffer1);
		}
		m_lb_list.InsertString(-1,buffer);
	}
	m_lb_list.SetTopIndex(topindex);
	NumToString(rlvar->NumPairs(),buffer,sizeof(buffer));
	m_tx_numpairs.SetWindowText(buffer);
	
	UpdateActivation();
	return;
}


void CEdVarRealList::UpdateActivation()
{
	if ( rlvar->NumPairs() > 0 )
	{
		m_bu_write.EnableWindow(TRUE);
	}
	else
	{
		m_bu_write.EnableWindow(FALSE);
	}

	if ( rlvar->StDevGlobal() == TRUE )
	{
		m_tx_relstd.EnableWindow(TRUE);
		m_ed_relstd.EnableWindow(TRUE);
		m_tx_absstd.EnableWindow(TRUE);
		m_ed_absstd.EnableWindow(TRUE);
		m_ed_stddata.SetWindowText("");
		m_tx_stddata.EnableWindow(FALSE);
		m_ed_stddata.EnableWindow(FALSE);
	}
	else
	{
		m_tx_relstd.EnableWindow(FALSE);
		m_ed_relstd.EnableWindow(FALSE);
		m_tx_absstd.EnableWindow(FALSE);
		m_ed_absstd.EnableWindow(FALSE);
		m_tx_stddata.EnableWindow(TRUE);
		m_ed_stddata.EnableWindow(TRUE);
	}

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

	if ( rlvar->Method() == smooth )
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
// CEdVarRealList message handlers

BOOL CEdVarRealList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	if ( rlvar->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(rlvar->Symbol());
	}

	if ( rlvar->Description() != 0 )
	{
		m_ed_descript.SetWindowText(rlvar->Description());
	}

	if ( rlvar->Unit() != 0 )
	{
		m_ed_unit.SetWindowText(rlvar->Unit());
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
	if ( rlvar->GetArg() != 0 )
	{
		int pos = aqsys.Varlist()->Position(rlvar->GetArg());
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

	if ( rlvar->StDevGlobal() == TRUE )
	{
		m_rb_global.SetCheck(1);
	}
	else
	{
		m_rb_individual.SetCheck(1);
	}

	NumToString(rlvar->RelStDev(),buffer,sizeof(buffer));
	m_ed_relstd.SetWindowText(buffer);

	NumToString(rlvar->AbsStDev(),buffer,sizeof(buffer));
	m_ed_absstd.SetWindowText(buffer);

	NumToString(rlvar->Minimum(),buffer,sizeof(buffer));
	m_ed_min.SetWindowText(buffer);

	NumToString(rlvar->Maximum(),buffer,sizeof(buffer));
	m_ed_max.SetWindowText(buffer);

	switch ( rlvar->Method() )
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

	NumToString(rlvar->SmoothWidth(),buffer,sizeof(buffer));
	m_ed_width.SetWindowText(buffer);

	if ( rlvar->SensActive() )
	{
		m_cb_active.SetCheck(1);
	}

	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarRealList::OnCancel() 
{
	delete rlvar;
	
	CDialog::OnCancel();
}


void CEdVarRealList::OnOK() 
{
	char buffer[1024];
	int i;
	
	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( rlvar->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(254),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	rlvar->Description(buffer);

	m_ed_unit.GetWindowText(buffer,sizeof(buffer));
	rlvar->Unit(buffer);

	i = m_le_arg.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(310), // IllegalArg
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
		rlvar->SetArg(var);
	}

	if ( rlvar->StDevGlobal() == TRUE )
	{
		REAL rstd, astd;
		m_ed_relstd.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&rstd) == FALSE )
		{
			MessageBox(aqapp.ini.T(311), // UnableReadRelStd
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		m_ed_absstd.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&astd) == FALSE )
		{
			MessageBox(aqapp.ini.T(311), // UnableReadAbsStd
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->SetStDev(rstd,astd) == FALSE )
		{
			MessageBox(aqapp.ini.T(313), // InconsistStd
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		REAL rstd, astd;
		m_ed_relstd.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&rstd) == FALSE )
		{
			rstd = rlvar->RelStDev();
		}
		m_ed_absstd.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&astd) == FALSE )
		{
			astd = rlvar->AbsStDev();
		}
		rlvar->SetStDev(rstd,astd);
	}

	REAL mini,maxi;
	m_ed_min.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&mini) == FALSE )
	{
		MessageBox(aqapp.ini.T(314), // UnableReadMin
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_max.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&maxi) == FALSE )
	{
		MessageBox(aqapp.ini.T(315), // UnableReadMax
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( mini < rlvar->Maximum() )
	{
		if ( rlvar->Minimum(mini) == FALSE )
		{
			MessageBox(aqapp.ini.T(316), // IllegalMin
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->Maximum(maxi) == FALSE )
		{
			MessageBox(aqapp.ini.T(317), // IllegalMax
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( rlvar->Maximum(maxi) == FALSE )
		{
			MessageBox(aqapp.ini.T(317), // IllegalMax
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->Minimum(mini) == FALSE )
		{
			MessageBox(aqapp.ini.T(316), // IllegalMin
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( rlvar->NumPairs() == 0 )
	{
		MessageBox(aqapp.ini.T(359), // NoDataPairs
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( rlvar->Method() == smooth )
	{
		REAL w;
		m_ed_width.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&w) == FALSE )
		{
			MessageBox(aqapp.ini.T(318), // UnableReadWidth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->SmoothWidth(w) == FALSE )
		{
			MessageBox(aqapp.ini.T(341), // IllegalWidth
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
			rlvar->SmoothWidth(w);
		}
	}

	if ( m_cb_active.GetCheck() ) 
	{
		rlvar->SensActive(TRUE);
	}
	else
	{
		rlvar->SensActive(FALSE);
	}

	REAL argval;
	m_ed_argdata.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&argval) == TRUE )
	{
		switch ( MessageBox(aqapp.ini.T(618),  // AddDataPair?
					        aqapp.ini.T(111),
							MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			return;
		default:
			break;
		}
	}
	if ( add == TRUE )
	{
		if ( aqsys.AddVar(rlvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(255),  // UnableAdd
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceVar(oldvar,rlvar) == FALSE )
		{
			MessageBox(aqapp.ini.T(256),  // IllegalVar
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


void CEdVarRealList::OnBuAddval() 
{
	char buffer[1024];

	REAL mini,maxi;
	m_ed_min.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&mini) == FALSE )
	{
		mini = rlvar->Minimum();
		NumToString(mini,buffer,sizeof(buffer));
		m_ed_min.SetWindowText(buffer);
	}
	m_ed_max.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&maxi) == FALSE )
	{
		maxi = rlvar->Maximum();
		NumToString(maxi,buffer,sizeof(buffer));
		m_ed_max.SetWindowText(buffer);
	}
	if ( mini < rlvar->Maximum() )
	{
		if ( rlvar->Minimum(mini) == FALSE )
		{
			MessageBox(aqapp.ini.T(316), // IllegalMin
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->Maximum(maxi) == FALSE )
		{
			MessageBox(aqapp.ini.T(317), // IllegalMax
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( rlvar->Maximum(maxi) == FALSE )
		{
			MessageBox(aqapp.ini.T(317), // IllegalMax
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->Minimum(mini) == FALSE )
		{
			MessageBox(aqapp.ini.T(316), // IllegalMin
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	REAL x,y;
	m_ed_argdata.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&x) == FALSE )
	{
		MessageBox(aqapp.ini.T(319),  // SpecifyArg
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_valdata.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&y) == FALSE )
	{
		MessageBox(aqapp.ini.T(320),  // SpecifyVal
			       aqapp.ini.T(111),
			       MB_OK+MB_ICONERROR);
		return;
	}
	if ( rlvar->StDevGlobal() == TRUE )
	{
		if ( rlvar->Add(x,y) == TRUE )
		{
			m_ed_argdata.SetWindowText("");
			m_ed_valdata.SetWindowText("");
			m_ed_stddata.SetWindowText("");
			UpdateDialog();
			int topindex = rlvar->ArgIndex(x)-3;
			if ( topindex < 0 ) topindex = 0;
			m_lb_list.SetTopIndex(topindex);
		}
		else
		{
			MessageBox(aqapp.ini.T(321),  // UnableAddData
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
		}
	}
	else
	{
		REAL s;
		m_ed_stddata.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&s) == FALSE )
		{
			MessageBox(aqapp.ini.T(322),
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( s <= 0 )
		{
			MessageBox(aqapp.ini.T(323),  // IllegalStd
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->Add(x,y,s) == TRUE )
		{
			m_ed_argdata.SetWindowText("");
			m_ed_valdata.SetWindowText("");
			m_ed_stddata.SetWindowText("");
			UpdateDialog();
			int topindex = rlvar->ArgIndex(x)-3;
			if ( topindex < 0 ) topindex = 0;
			m_lb_list.SetTopIndex(topindex);
		}
		else
		{
			MessageBox(aqapp.ini.T(321),
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
		}
	}

	return;
}


void CEdVarRealList::OnBuReplval() 
{
	char buffer[1024];

	int max = rlvar->NumPairs();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);
	int pos = indices[0];
	delete indices;

	if ( num == 1 )
	{
		REAL mini,maxi;
		m_ed_min.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&mini) == FALSE )
		{
			mini = rlvar->Minimum();
			NumToString(mini,buffer,sizeof(buffer));
			m_ed_min.SetWindowText(buffer);
		}
		m_ed_max.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&maxi) == FALSE )
		{
			maxi = rlvar->Maximum();
			NumToString(maxi,buffer,sizeof(buffer));
			m_ed_max.SetWindowText(buffer);
		}
		if ( mini < rlvar->Maximum() )
		{
			if ( rlvar->Minimum(mini) == FALSE )
			{
				MessageBox(aqapp.ini.T(316), // IllegalMin
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			if ( rlvar->Maximum(maxi) == FALSE )
			{
				MessageBox(aqapp.ini.T(317), // IllegalMax
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}
		else
		{
			if ( rlvar->Maximum(maxi) == FALSE )
			{
				MessageBox(aqapp.ini.T(317), // IllegalMax
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			if ( rlvar->Minimum(mini) == FALSE )
			{
				MessageBox(aqapp.ini.T(316), // IllegalMin
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
		}

		REAL x,y;
		m_ed_argdata.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&x) == FALSE )
		{
			MessageBox(aqapp.ini.T(319),  // SpecifyArg
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
		m_ed_valdata.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&y) == FALSE )
		{
			MessageBox(aqapp.ini.T(320),  // SpecifyVal
				       aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
		if ( rlvar->StDevGlobal() == TRUE )
		{
			if ( rlvar->Replace(pos,x,y) == TRUE )
			{
				m_ed_argdata.SetWindowText("");
				m_ed_valdata.SetWindowText("");
				m_ed_stddata.SetWindowText("");
				UpdateDialog();
				if ( rlvar->ArgIndex(x) != pos )
				{
					int topindex = rlvar->ArgIndex(x)-3;
					if ( topindex < 0 ) topindex = 0;
					m_lb_list.SetTopIndex(topindex);
				}
			}
			else
			{
				MessageBox(aqapp.ini.T(324),
					       aqapp.ini.T(111),
					       MB_OK+MB_ICONERROR);
			}
		}
		else
		{
			REAL s;
			m_ed_stddata.GetWindowText(buffer,sizeof(buffer));
			if ( StringToNum(buffer,&s) == FALSE )
			{
				MessageBox(aqapp.ini.T(322),  // SpecifyStd
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			if ( s <= 0 )
			{
				MessageBox(aqapp.ini.T(323),  // IllegalStd
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
				return;
			}
			if ( rlvar->Replace(pos,x,y,s) == TRUE )
			{
				m_ed_argdata.SetWindowText("");
				m_ed_valdata.SetWindowText("");
				m_ed_stddata.SetWindowText("");
				UpdateDialog();
				if ( rlvar->ArgIndex(x) != pos )
				{
					int topindex = rlvar->ArgIndex(x)-3;
					if ( topindex < 0 ) topindex = 0;
					m_lb_list.SetTopIndex(topindex);
				}
			}
			else
			{
				MessageBox(aqapp.ini.T(324),  // UnableReplace
					       aqapp.ini.T(111),
					       MB_OK+MB_ICONERROR);
			}
		}
	}
	return;
}


void CEdVarRealList::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(325),  // DelSelData?
			 aqapp.ini.T(111),
			 MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = rlvar->NumPairs();
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
			if ( rlvar->Delete(indices[j]-k) == TRUE )
			{
				k++;
			}
		}
		m_ed_argdata.SetWindowText(" ");
		m_ed_valdata.SetWindowText(" ");
		m_ed_stddata.SetWindowText(" ");
		break;
	}
	delete [] indices;
	UpdateDialog();
	return;
}


void CEdVarRealList::OnBuRead() 
{
	char filter[260];
	char filename[260];
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

	strcpy(filter,"Text Files (*.txt)0*.txt0Data Files (*.dat)0*.dat0All Files0*.*0");
	filter[18] = '\0'; filter[24] = '\0';
	filter[43] = '\0'; filter[49] = '\0';
	filter[59] = '\0'; filter[63] = '\0';
	strcpy(filename,aqapp.readdatafilename);
	filetitle[0] = '\0';
	strcpy(title,aqapp.ini.T(342));  // SelOpenFile
	strcat(title,":");
	dirname[0] = '\0';
	strcpy(fileextension,"txt");
 
	OPENFILENAME filenamestruct;
    filenamestruct.lStructSize       = sizeof(OPENFILENAME); 
    filenamestruct.hwndOwner         = NULL; 
    filenamestruct.hInstance         = NULL; 
    filenamestruct.lpstrFilter       = filter; 
    filenamestruct.lpstrCustomFilter = NULL; 
    filenamestruct.nMaxCustFilter    = 0; 
    filenamestruct.nFilterIndex      = 0; 
    filenamestruct.lpstrFile         = filename; 
    filenamestruct.nMaxFile          = sizeof(filename); 
    filenamestruct.lpstrFileTitle    = filetitle; 
    filenamestruct.nMaxFileTitle     = sizeof(filetitle); 
    filenamestruct.lpstrInitialDir   = dirname; 
    filenamestruct.lpstrTitle        = title; 
    filenamestruct.Flags             = OFN_EXPLORER |
		                               OFN_FILEMUSTEXIST |
									   OFN_HIDEREADONLY |
									   OFN_LONGNAMES |
									   OFN_PATHMUSTEXIST; 
    filenamestruct.lpstrDefExt       = fileextension; 
    filenamestruct.lCustData         = NULL; 
    filenamestruct.lpfnHook          = NULL; 
    filenamestruct.lpTemplateName    = NULL; 

	if ( GetOpenFileName(&filenamestruct) )
	{
		std::ifstream readfile;
		readfile.open(filename,std::ios::in);
		CEdVarRealListData EdVarRealListDataDlg;
		if ( EdVarRealListDataDlg.DoModal(rlvar,&readfile) == IDOK )
		{
			strcpy(aqapp.readdatafilename,filename);
			UpdateDialog();
		}
	}
	else
	{
		// no file selected
	}
	return;
}


void CEdVarRealList::OnBuWrite() 
{
	char filter[260];
	char filename[260];
	char filetitle[260]; 
 	char title[260];
	char dirname[MAX_PATH];
	char fileextension[4];

	strcpy(filter,"Text Files0*.txt0");
	filter[10] = '\0'; filter[16] = '\0';
	strcpy(filename,aqapp.writedatafilename);
	filetitle[0] = '\0';
	strcpy(title,aqapp.ini.T(343));  // SelDataFile
	strcat(title,":");
	dirname[0] = '\0';
	strcpy(fileextension,"txt");
 
	OPENFILENAME filenamestruct;
    filenamestruct.lStructSize       = sizeof(OPENFILENAME); 
    filenamestruct.hwndOwner         = NULL; 
    filenamestruct.hInstance         = NULL; 
    filenamestruct.lpstrFilter       = filter; 
    filenamestruct.lpstrCustomFilter = NULL; 
    filenamestruct.nMaxCustFilter    = 0; 
    filenamestruct.nFilterIndex      = 0; 
    filenamestruct.lpstrFile         = filename; 
    filenamestruct.nMaxFile          = sizeof(filename); 
    filenamestruct.lpstrFileTitle    = filetitle; 
    filenamestruct.nMaxFileTitle     = sizeof(filetitle); 
    filenamestruct.lpstrInitialDir   = dirname; 
    filenamestruct.lpstrTitle        = title; 
    filenamestruct.Flags             = OFN_EXPLORER |
									   OFN_HIDEREADONLY |
									   OFN_OVERWRITEPROMPT |
									   OFN_LONGNAMES;
    filenamestruct.lpstrDefExt       = fileextension; 
    filenamestruct.lCustData         = NULL; 
    filenamestruct.lpfnHook          = NULL; 
    filenamestruct.lpTemplateName    = NULL; 

	if ( GetSaveFileName(&filenamestruct) )
	{
		aqapp.CancelDlg.Display(aqapp.ini.T(344)); // Writing
		std::ofstream writefile;
		writefile.open(filename,std::ios::out);
        SetFileType(filename,FTdatW,FCdat);
		if ( rlvar->WriteData(writefile) == TRUE )
		{
			writefile.close();
			aqapp.CancelDlg.Remove();
			strcpy(aqapp.writedatafilename,filename);
	    	AfxMessageBox(aqapp.ini.T(326),
				          MB_OK+MB_ICONINFORMATION); // WriteSuccess
		}
		else
		{
			writefile.close();
			aqapp.CancelDlg.Remove();
	    	AfxMessageBox(aqapp.ini.T(326),
				          MB_OK+MB_ICONINFORMATION); // WriteFailure
		}
	}
	else
	{
		// no file selected
	}
	return;
}


void CEdVarRealList::OnSelchangeLbList() 
{
	char buffer[1024];

	int max = rlvar->NumPairs();
	int* indices = new int[max];
	int num = m_lb_list.GetSelItems(max,indices);

	if ( num == 1 )
	{
		int i = indices[0];
		if ( i>=0 && i<rlvar->NumPairs() )
		{
			NumToString(rlvar->X()[i],buffer,sizeof(buffer));
			m_ed_argdata.SetWindowText(buffer);
			NumToString(rlvar->Y()[i],buffer,sizeof(buffer));
			m_ed_valdata.SetWindowText(buffer);
			if ( rlvar->StDevGlobal() == FALSE )
			{
				NumToString(rlvar->GetStDev(i),buffer,sizeof(buffer));
				m_ed_stddata.SetWindowText(buffer);
			}
		}
	}
	delete [] indices;
	UpdateActivation();
	return;
}


void CEdVarRealList::OnRbGlobal() 
{
	if ( rlvar->StDevGlobal() == FALSE )
	{
		rlvar->StDevGlobal(TRUE);
		switch ( MessageBox(aqapp.ini.T(328),              // DelIndStd?
					        aqapp.ini.T(111),
						    MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
		{
		case IDYES:
			rlvar->DeleteIndividualStDev();
			break;
		default:
			break;
		}
	}
	UpdateDialog();
	return;
}


void CEdVarRealList::OnRbIndividual() 
{
	char buffer[1024];
	REAL rstd, astd;

	m_ed_relstd.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&rstd) == FALSE )
	{
		rstd = rlvar->RelStDev();
		NumToString(rstd,buffer,sizeof(buffer));
		m_ed_relstd.SetWindowText(buffer);
	}
	m_ed_absstd.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&astd) == FALSE )
	{
		astd = rlvar->AbsStDev();
		NumToString(astd,buffer,sizeof(buffer));
		m_ed_absstd.SetWindowText(buffer);
	}
	if ( rlvar->SetStDev(rstd,astd) == FALSE )
	{
		NumToString(rlvar->RelStDev(),buffer,sizeof(buffer));
		m_ed_relstd.SetWindowText(buffer);
		NumToString(rlvar->AbsStDev(),buffer,sizeof(buffer));
		m_ed_absstd.SetWindowText(buffer);
	}
	rlvar->StDevGlobal(FALSE);
	UpdateDialog();
	return;
}


void CEdVarRealList::OnRbLinear() 
{
	rlvar->Method(linear);
	UpdateActivation();
	return;
}


void CEdVarRealList::OnRbSmooth() 
{
	rlvar->Method(smooth);
	UpdateActivation();
	return;
}


void CEdVarRealList::OnRbSpline() 
{
	rlvar->Method(spline);
	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdVarRealListData dialog


CEdVarRealListData::CEdVarRealListData(CWnd* pParent /*=NULL*/)
	: CDialog(CEdVarRealListData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdVarRealListData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdVarRealListData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdVarRealListData)
	DDX_Control(pDX, IDC_TX_STD, m_tx_stdev);
	DDX_Control(pDX, IDC_ED_VAL, m_ed_value);
	DDX_Control(pDX, IDC_ED_STD, m_ed_stdev);
	DDX_Control(pDX, IDC_ED_START, m_ed_startrow);
	DDX_Control(pDX, IDC_ED_END, m_ed_endrow);
	DDX_Control(pDX, IDC_ED_ARG, m_ed_argument);
	DDX_Control(pDX, IDC_CB_DEL, m_cb_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdVarRealListData, CDialog)
	//{{AFX_MSG_MAP(CEdVarRealListData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdVarRealListData::DoModal(REALLISTVAR* var, std::ifstream* file)
{
	rlvar = var;
	readfile = file;
	return CDialog::DoModal();
}


INTEGER CEdVarRealListData::firstrow = 4;
INTEGER CEdVarRealListData::lastrow  = 0;
INTEGER CEdVarRealListData::colarg   = 1;
INTEGER CEdVarRealListData::colval   = 2;
INTEGER CEdVarRealListData::colstd   = 3;
BOOLEAN CEdVarRealListData::delpairs = TRUE;


/////////////////////////////////////////////////////////////////////////////
// CEdVarRealListData message handlers

BOOL CEdVarRealListData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	NumToString(firstrow,buffer,sizeof(buffer));
	m_ed_startrow.SetWindowText(buffer);
	NumToString(lastrow,buffer,sizeof(buffer));
	m_ed_endrow.SetWindowText(buffer);
	NumToString(colarg,buffer,sizeof(buffer));
	m_ed_argument.SetWindowText(buffer);
	NumToString(colval,buffer,sizeof(buffer));
	m_ed_value.SetWindowText(buffer);
	if ( rlvar->StDevGlobal() == TRUE )
	{
		m_tx_stdev.EnableWindow(FALSE);
		m_ed_stdev.EnableWindow(FALSE);
	}
	else
	{
		NumToString(colstd,buffer,sizeof(buffer));
		m_ed_stdev.SetWindowText(buffer);
	}
	if ( delpairs == TRUE )
	{
		m_cb_delete.SetCheck(1);
	}
	else
	{
		m_cb_delete.SetCheck(0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdVarRealListData::OnCancel() 
{
	readfile->close();
	CDialog::OnCancel();
}


void CEdVarRealListData::OnOK() 
{
	char buffer[1024];

	m_ed_startrow.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&firstrow) == FALSE )
	{
		MessageBox(aqapp.ini.T(329), // FailReadStart
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( firstrow <= 0 )
	{
		MessageBox(aqapp.ini.T(330), // IllegalStart
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_endrow.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&lastrow) == FALSE )
	{
		MessageBox(aqapp.ini.T(331), // FailReadEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lastrow < 0 )
	{
		MessageBox(aqapp.ini.T(332), // IllegalEnd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lastrow != 0 && lastrow < firstrow )
	{
		MessageBox(aqapp.ini.T(333), // IllegalEndStart
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_argument.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&colarg) == FALSE )
	{
		MessageBox(aqapp.ini.T(334), // FailReadArg
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( colarg <= 0 )
	{
		MessageBox(aqapp.ini.T(335), // IllegalArg
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_value.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&colval) == FALSE )
	{
		MessageBox(aqapp.ini.T(336), // FailReadVal
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( colval <= 0 )
	{
		MessageBox(aqapp.ini.T(337), // IllegalArg
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	CARDINAL COLSTD = 0;
	if ( rlvar->StDevGlobal() == FALSE )
	{
		m_ed_stdev.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&colstd) == FALSE )
		{
			MessageBox(aqapp.ini.T(338), // FailReadStD
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		if ( colstd <= 0 )
		{
			MessageBox(aqapp.ini.T(339), // IllegalStD
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		COLSTD = colstd;
	}

	if ( m_cb_delete.GetCheck() )
	{
		delpairs = TRUE;
		rlvar->Delete();
	}
	else
	{
		delpairs = FALSE;
	}

	CARDLONG numread;
	aqapp.CancelDlg.Display(aqapp.ini.T(345)); // Reading
	if ( rlvar->ReadData(*readfile,&numread,colarg,colval,COLSTD,
		                 firstrow,lastrow) == TRUE )
	{
		aqapp.CancelDlg.Remove();
		readfile->close();
		NumToString(numread,buffer,sizeof(buffer));
		strcat(buffer," ");
		strcat(buffer,aqapp.ini.T(346));
		MessageBox(buffer,aqapp.ini.T(111),MB_OK+MB_ICONINFORMATION);
	}
	else
	{
		aqapp.CancelDlg.Remove();
		readfile->close();
		if ( numread == 0 )
		{
			MessageBox(aqapp.ini.T(340), // UnableRead
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
		}
		else
		{
			char bufnum[1024];
			NumToString(numread,bufnum,sizeof(bufnum));
			strcpy(buffer,aqapp.ini.T(347));
			strcat(buffer," ");
			strcat(buffer,bufnum);
			strcat(buffer," ");
			strcat(buffer,aqapp.ini.T(348));
			MessageBox(buffer,aqapp.ini.T(111),MB_OK+MB_ICONERROR);
		}
	}

	CDialog::OnOK();
}


