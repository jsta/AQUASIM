// edlake_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "edlake_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static int inputtype = 0;

static int EditInputs(LAKECOMP* com);

/////////////////////////////////////////////////////////////////////////////
// CEdLake dialog


CEdLake::CEdLake(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLake::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLake)
	//}}AFX_DATA_INIT
}


void CEdLake::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLake)
	DDX_Control(pDX, IDC_CB_ACTIVE, m_cb_active);
	DDX_Control(pDX, IDC_RB_TKENO, m_rb_tkeno);
	DDX_Control(pDX, IDC_RB_TKEYES, m_rb_tkeyes);
	DDX_Control(pDX, IDC_RB_SEDNO, m_rb_sedno);
	DDX_Control(pDX, IDC_RB_SEDYES, m_rb_sedyes);
	DDX_Control(pDX, IDC_RB_LOWRES, m_rb_lowres);
	DDX_Control(pDX, IDC_RB_HIGHRES, m_rb_highres);
	DDX_Control(pDX, IDC_ED_TOP, m_ed_top);
	DDX_Control(pDX, IDC_ED_NUMGRID, m_ed_numgrid);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_GRAVIT, m_ed_gravit);
	DDX_Control(pDX, IDC_ED_DIFFUSION, m_ed_diffusion);
	DDX_Control(pDX, IDC_ED_ISURF, m_ed_isurf);
	DDX_Control(pDX, IDC_ED_ETA, m_ed_eta);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_DENSITY, m_ed_density);
	DDX_Control(pDX, IDC_ED_CROSSSECT, m_ed_crosssect);
	DDX_Control(pDX, IDC_ED_COMPIND, m_ed_compind);
	DDX_Control(pDX, IDC_ED_BOTTOM, m_ed_bottom);
	DDX_Control(pDX, IDC_BU_TKE, m_bu_tke);
	DDX_Control(pDX, IDC_BU_SED, m_bu_sed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLake, CDialog)
	//{{AFX_MSG_MAP(CEdLake)
	ON_BN_CLICKED(IDC_BU_VARS, OnBuVars)
	ON_BN_CLICKED(IDC_BU_PROCS, OnBuProcs)
	ON_BN_CLICKED(IDC_BU_INITS, OnBuInits)
	ON_BN_CLICKED(IDC_BU_INP, OnBuInp)
	ON_BN_CLICKED(IDC_BU_SED, OnBuSed)
	ON_BN_CLICKED(IDC_BU_TKE, OnBuTke)
	ON_BN_CLICKED(IDC_RB_SEDNO, OnRbSedno)
	ON_BN_CLICKED(IDC_RB_SEDYES, OnRbSedyes)
	ON_BN_CLICKED(IDC_RB_TKENO, OnRbTkeno)
	ON_BN_CLICKED(IDC_RB_TKEYES, OnRbTkeyes)
	ON_BN_CLICKED(IDC_RB_LOWRES, OnRbLowres)
	ON_BN_CLICKED(IDC_RB_HIGHRES, OnRbHighres)
	ON_BN_CLICKED(IDC_BU_PARTVARS, OnBuPartvars)
	ON_BN_CLICKED(IDC_BU_DISSVARS, OnBuDissvars)
	ON_BN_CLICKED(IDC_BU_ACC, OnBuAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLake::DoModal(AQCOMP* com, BOOL a)
{
	oldcomp = com;
	lcomp = new LAKECOMP(oldcomp);
	add = a;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLake message handlers

BOOL CEdLake::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( lcomp->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(lcomp->Symbol());
	}

	if ( lcomp->Description() != 0 )
	{
		m_ed_descript.SetWindowText(lcomp->Description());
	}

	NumToString(lcomp->CmpIndex(),buffer,sizeof(buffer));
	m_ed_compind.SetWindowText(buffer);

	NumToString(lcomp->Gravit(),buffer,sizeof(buffer));
	m_ed_gravit.SetWindowText(buffer);

	NumToString(lcomp->ZTop(),buffer,sizeof(buffer));
	m_ed_top.SetWindowText(buffer);

	NumToString(lcomp->ZBottom(),buffer,sizeof(buffer));
	m_ed_bottom.SetWindowText(buffer);

	if ( lcomp->CrossSect() != 0 )
	{
		if ( lcomp->CrossSect()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_crosssect.SetWindowText(buffer);
		}
	}

	if ( lcomp->Density() != 0 )
	{
		if ( lcomp->Density()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_density.SetWindowText(buffer);
		}
	}

	if ( lcomp->Diffusion() != 0 )
	{
		if ( lcomp->Diffusion()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_diffusion.SetWindowText(buffer);
		}
	}

	if ( lcomp->LightSurface() != 0 )
	{
		if ( lcomp->LightSurface()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_isurf.SetWindowText(buffer);
		}
	}

	if ( lcomp->LightExtinct() != 0 )
	{
		if ( lcomp->LightExtinct()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_eta.SetWindowText(buffer);
		}
	}

	if ( lcomp->CalcSed() == TRUE )
	{
		m_rb_sedyes.SetCheck(1);
		m_bu_sed.EnableWindow(TRUE);
	}
	else
	{
		m_rb_sedno.SetCheck(1);
		m_bu_sed.EnableWindow(FALSE);
	}

	if ( lcomp->CalcTKE() == TRUE )
	{
		m_rb_tkeyes.SetCheck(1);
		m_bu_tke.EnableWindow(TRUE);
	}
	else
	{
		m_rb_tkeno.SetCheck(1);
		m_bu_tke.EnableWindow(FALSE);
	}

	NumToString(lcomp->NumGridPts(),buffer,sizeof(buffer));
	m_ed_numgrid.SetWindowText(buffer);

	if ( lcomp->HighRes() == TRUE )
	{
		m_rb_highres.SetCheck(1);
	}
	else
	{
		m_rb_lowres.SetCheck(1);
	}

	if ( lcomp->Active() == TRUE )
	{
		m_cb_active.SetCheck(1);
	}
	else
	{
		m_cb_active.SetCheck(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLake::OnOK() 
{
	char buffer[1024], buffer1[1024]; REAL r1,r2; CARDINAL c, numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(130),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	lcomp->Description(buffer);

	m_ed_compind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(131), // FailCompInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	lcomp->CmpIndex(c);

	m_ed_gravit.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(132), // FailGravit
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->Gravit(r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(133), // IllegalGravit
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_top.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(134), // FailTop
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_bottom.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(135), // FailTop
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->ZTopBottom(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(136), // IllegalTopBottom
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_crosssect.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->CrossSect(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(137), // ParseErrCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(138), // IllegalCross
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_density.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->Density(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(139), // ParseErrDens
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(140), // IllegalDens
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_diffusion.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->Diffusion(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(141), // ParseErrDiff
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(142), // IllegalDiff
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_isurf.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->LightSurface(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(643), // ParseErrIsurf
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(644), // IllegalIsurf
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_eta.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->LightExtinct(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(645), // ParseErrEta
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(646), // IllegalEta
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_numgrid.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(143), // FailNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->NumGridPts(c) == FALSE )
	{
		MessageBox(aqapp.ini.T(144), // IllegalNumGrid
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_active.GetCheck() ) lcomp->Active(TRUE);
    else                          lcomp->Active(FALSE);

	if ( add == TRUE )
	{
		if ( aqsys.AddComp(lcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(128),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplaceComp(oldcomp,lcomp) == FALSE )
		{
			MessageBox(aqapp.ini.T(129),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


void CEdLake::OnCancel() 
{
	delete lcomp;
	
	CDialog::OnCancel();
}


void CEdLake::OnBuVars() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdCompActVar EdCompActVarDlg;
	if ( EdCompActVarDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnBuProcs() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdCompActProc EdCompActProcDlg;
	if ( EdCompActProcDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnBuInits() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdLakeInits EdLakeInitsDlg;
	if ( EdLakeInitsDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnBuInp() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	if ( EditInputs(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnBuPartvars() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdLakeParts EdLakePartsDlg;
	if ( EdLakePartsDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnBuDissvars() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdLakeDisss EdLakeDisssDlg;
	if ( EdLakeDisssDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnRbSedno() 
{
	lcomp->CalcSed(FALSE);
	m_bu_sed.EnableWindow(FALSE);
	return;
}


void CEdLake::OnRbSedyes() 
{
	lcomp->CalcSed(TRUE);
	m_bu_sed.EnableWindow(TRUE);
	return;
}


void CEdLake::OnBuSed() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdLakeSed EdLakeSedDlg;
	if ( EdLakeSedDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnRbTkeno() 
{
	lcomp->CalcTKE(FALSE);
	m_bu_tke.EnableWindow(FALSE);
	return;
}


void CEdLake::OnRbTkeyes() 
{
	lcomp->CalcTKE(TRUE);
	m_bu_tke.EnableWindow(TRUE);
	return;
}


void CEdLake::OnBuTke() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdLakeTKE EdLakeTKEDlg;
	if ( EdLakeTKEDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


void CEdLake::OnRbLowres() 
{
	lcomp->HighRes(FALSE);
	return;
}


void CEdLake::OnRbHighres() 
{
	lcomp->HighRes(TRUE);
	return;
}


void CEdLake::OnBuAcc() 
{
	LAKECOMP* tempcomp = new LAKECOMP(lcomp);
	CEdLakeAcc EdLakeAccDlg;
	if ( EdLakeAccDlg.DoModal(tempcomp) == IDOK )
	{
		delete lcomp;
		lcomp = tempcomp;
	}
	else
	{
		delete tempcomp;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSed dialog


CEdLakeSed::CEdLakeSed(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSed)
	//}}AFX_DATA_INIT
}


void CEdLakeSed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSed)
	DDX_Control(pDX, IDC_ED_VOLFLUX, m_ed_volflux);
	DDX_Control(pDX, IDC_ED_RESUSP, m_ed_resusp);
	DDX_Control(pDX, IDC_LB_SEDLAYERS, m_lb_sedlayers);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSed, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSed)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_SEDLAYERS, OnSelchangeLbSedlayers)
	ON_LBN_DBLCLK(IDC_LB_SEDLAYERS, OnDblclkLbSedlayers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSed::DoModal(LAKECOMP* com)
{
    lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeSed::UpdateActivation()
{
	switch ( m_lb_sedlayers.GetSelCount() )
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


void CEdLakeSed::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];

	int topindex = m_lb_sedlayers.GetTopIndex();
	m_lb_sedlayers.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumSedLayers(); i++ )
	{
		const LAKESEDLAYER* sedlayer = lcomp->SedLayer(i);
		NumToString(sedlayer->ZoneIndex(),buffer,sizeof(buffer));
		strcat(buffer,":  ");
		(sedlayer->Thickness())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		(sedlayer->Porosity())->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_sedlayers.InsertString(-1,buffer);
	}
	m_lb_sedlayers.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSed message handlers

BOOL CEdLakeSed::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( lcomp->SedFluxVol() != 0 )
	{
		if ( lcomp->SedFluxVol()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_volflux.SetWindowText(buffer);
		}
	}

	if ( lcomp->SedResusp() != 0 )
	{
		if ( lcomp->SedResusp()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_resusp.SetWindowText(buffer);
		}
	}

    UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeSed::OnBuAdd() 
{
	int max = lcomp->NumSedLayers();
	int* indices = new int[max];
	int num = m_lb_sedlayers.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakeSedLayer EdSedLayerDlg;
	EdSedLayerDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdLakeSed::OnBuEdit() 
{
	int max = lcomp->NumSedLayers();
	int* indices = new int[max];
	int num = m_lb_sedlayers.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakeSedLayer EdSedLayerDlg;
	EdSedLayerDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdLakeSed::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(619),  // DelLayers?
				        aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumSedLayers();
	int* indices = new int[max];
	int num = m_lb_sedlayers.GetSelItems(max,indices);
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
			if ( lcomp->DeleteSedLayer(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(620),   // FailDelLayer
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


void CEdLakeSed::OnSelchangeLbSedlayers() 
{
	UpdateActivation();
	return;
}


void CEdLakeSed::OnDblclkLbSedlayers() 
{
	OnBuEdit();
	return;
}


void CEdLakeSed::OnOK() 
{
	char buffer[1024], buffer1[1024]; CARDINAL numerr;

	m_ed_volflux.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->SedFluxVol(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(149), // ParseErrVolFlux
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(150), // IllegalVolFlux
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_resusp.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->SedResusp(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(151), // ParseErrResusp
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(152), // IllegalResusp
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeTKE dialog


CEdLakeTKE::CEdLakeTKE(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeTKE::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeTKE)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeTKE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeTKE)
	DDX_Control(pDX, IDC_ED_SURFSHEAR, m_ed_surfshear);
	DDX_Control(pDX, IDC_ED_PRESS, m_ed_pressgrad);
	DDX_Control(pDX, IDC_ED_PWIND, m_ed_pwind);
	DDX_Control(pDX, IDC_ED_PRANTL, m_ed_prantl);
	DDX_Control(pDX, IDC_ED_SIGTKE, m_ed_sigtke);
	DDX_Control(pDX, IDC_ED_SIGEPS, m_ed_sigeps);
	DDX_Control(pDX, IDC_ED_KZMIN, m_ed_kzmin);
	DDX_Control(pDX, IDC_ED_PINT, m_ed_pint);
	DDX_Control(pDX, IDC_ED_PBOTTOM, m_ed_pbottom);
	DDX_Control(pDX, IDC_ED_DISSIP, m_ed_dissip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeTKE, CDialog)
	//{{AFX_MSG_MAP(CEdLakeTKE)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeTKE::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEdLakeTKE message handlers

BOOL CEdLakeTKE::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( lcomp->Prandtl() != 0 )
	{
		if ( lcomp->Prandtl()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_prantl.SetWindowText(buffer);
		}
	}

	NumToString(lcomp->SigmaTKE(),buffer,sizeof(buffer));
    m_ed_sigtke.SetWindowText(buffer);

	NumToString(lcomp->SigmaEps(),buffer,sizeof(buffer));
    m_ed_sigeps.SetWindowText(buffer);

	NumToString(lcomp->KzMin(),buffer,sizeof(buffer));
    m_ed_kzmin.SetWindowText(buffer);

	if ( lcomp->SurfaceShear() != 0 )
	{
		if ( lcomp->SurfaceShear()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_surfshear.SetWindowText(buffer);
		}
	}

	if ( lcomp->PressureGradient() != 0 )
	{
		if ( lcomp->PressureGradient()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_pressgrad.SetWindowText(buffer);
		}
	}

	if ( lcomp->ProdDiss() != 0 )
	{
		if ( lcomp->ProdDiss()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_dissip.SetWindowText(buffer);
		}
	}
	
	if ( lcomp->SeichePwind() != 0 )
	{
		if ( lcomp->SeichePwind()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_pwind.SetWindowText(buffer);
		}
	}

	if ( lcomp->SeichePbottom() != 0 )
	{
		if ( lcomp->SeichePbottom()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_pbottom.SetWindowText(buffer);
		}
	}

	if ( lcomp->SeichePint() != 0 )
	{
		if ( lcomp->SeichePint()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_pint.SetWindowText(buffer);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeTKE::OnOK() 
{
	char buffer[1024], buffer1[1024]; CARDINAL numerr; REAL r;
	
	m_ed_prantl.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->Prandtl(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(153), // ParseErrPrantl
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(154), // IllegalPrantl
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

    m_ed_sigtke.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r) == FALSE )
	{
		MessageBox(aqapp.ini.T(613), // FailSigTKE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->SigmaTKE(r) == FALSE )
	{
		MessageBox(aqapp.ini.T(614), // IllegalSigTKE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

    m_ed_sigeps.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r) == FALSE )
	{
		MessageBox(aqapp.ini.T(615), // FailSigEps
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->SigmaEps(r) == FALSE )
	{
		MessageBox(aqapp.ini.T(616), // IllegalSigEps
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

    m_ed_kzmin.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r) == FALSE )
	{
		MessageBox(aqapp.ini.T(624), // FailKzMin
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->KzMin(r) == FALSE )
	{
		MessageBox(aqapp.ini.T(625), // IllegalKzMin
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_surfshear.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->SurfaceShear(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(155), // ParseErrShear
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(156), // IllegalShear
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_pressgrad.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->PressureGradient(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(626), // ParseErrPress
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(627), // IllegalPress
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_dissip.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->ProdDiss(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(157), // ParseErrDissip
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(158), // IllegalProdDiss
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_pwind.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->SeichePwind(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(159), // ParseErrPwind
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(160), // IllegalPwind
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_pbottom.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->SeichePbottom(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(161), // ParseErrPbottom
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(162), // IllegalPbottom
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_pint.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->SeichePint(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(163), // ParseErrPint
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(164), // IllegalPint
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeParts dialog


CEdLakeParts::CEdLakeParts(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeParts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeParts)
	//}}AFX_DATA_INIT
}


void CEdLakeParts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeParts)
	DDX_Control(pDX, IDC_LB_VARS, m_lb_partlist);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeParts, CDialog)
	//{{AFX_MSG_MAP(CEdLakeParts)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_VARS, OnSelchangeLbVars)
	ON_LBN_DBLCLK(IDC_LB_VARS, OnDblclkLbVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeParts::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeParts::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_partlist.GetTopIndex();
	m_lb_partlist.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumPartVar(); i++ )
	{
		strcpy(buffer,lcomp->PartVar(i)->Symbol());
		strcat(buffer," : ");
		lcomp->PartRho(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		lcomp->PartSedVeloc(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,", ");
		lcomp->PartMobility(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_partlist.InsertString(-1,buffer);
	}
	m_lb_partlist.SetTopIndex(topindex);
	UpdateActivation();
	return;
}
	
	
void CEdLakeParts::UpdateActivation()
{
	switch ( m_lb_partlist.GetSelCount() )
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
// CEdLakeParts message handlers


BOOL CEdLakeParts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeParts::OnBuAdd() 
{
	int max = lcomp->NumPartVar();
	int* indices = new int[max];
	int num = m_lb_partlist.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakePart EdLakePartDlg;
	EdLakePartDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdLakeParts::OnBuEdit() 
{
	int max = lcomp->NumPartVar();
	int* indices = new int[max];
	int num = m_lb_partlist.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakePart EdLakePartDlg;
	EdLakePartDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdLakeParts::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(165),            // DelParts?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumPartVar();
	int* indices = new int[max];
	int num = m_lb_partlist.GetSelItems(max,indices);
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
			if ( lcomp->DeletePartVar(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(167),
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


void CEdLakeParts::OnSelchangeLbVars() 
{
	UpdateActivation();
	return;
}


void CEdLakeParts::OnDblclkLbVars() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeDisss dialog


CEdLakeDisss::CEdLakeDisss(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeDisss::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeDisss)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeDisss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeDisss)
	DDX_Control(pDX, IDC_LB_VARS, m_lb_disslist);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeDisss, CDialog)
	//{{AFX_MSG_MAP(CEdLakeDisss)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_DBLCLK(IDC_LB_VARS, OnDblclkLbVars)
	ON_LBN_SELCHANGE(IDC_LB_VARS, OnSelchangeLbVars)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeDisss::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeDisss::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_disslist.GetTopIndex();
	m_lb_disslist.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumDissVar(); i++ )
	{
		strcpy(buffer,lcomp->DissVar(i)->Symbol());
		strcat(buffer," : ");
		lcomp->Disskdiff(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_disslist.InsertString(-1,buffer);
	}
	m_lb_disslist.SetTopIndex(topindex);
	UpdateActivation();
}


void CEdLakeDisss::UpdateActivation()
{
	switch ( m_lb_disslist.GetSelCount() )
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
// CEdLakeDisss message handlers

BOOL CEdLakeDisss::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeDisss::OnBuAdd() 
{
	int max = lcomp->NumDissVar();
	int* indices = new int[max];
	int num = m_lb_disslist.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakeDiss EdLakeDissDlg;
	EdLakeDissDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
}


void CEdLakeDisss::OnBuEdit() 
{
	int max = lcomp->NumDissVar();
	int* indices = new int[max];
	int num = m_lb_disslist.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakeDiss EdLakeDissDlg;
	EdLakeDissDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
}


void CEdLakeDisss::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(166),            // DelDisss?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumDissVar();
	int* indices = new int[max];
	int num = m_lb_disslist.GetSelItems(max,indices);
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
			if ( lcomp->DeleteDissVar(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(168),
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;
	UpdateDialog();
}


void CEdLakeDisss::OnSelchangeLbVars() 
{
	UpdateActivation();
}


void CEdLakeDisss::OnDblclkLbVars() 
{
	OnBuEdit();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakePart dialog


CEdLakePart::CEdLakePart(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakePart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakePart)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakePart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakePart)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_SEDVELOC, m_ed_sedveloc);
	DDX_Control(pDX, IDC_ED_DENSITY, m_ed_density);
	DDX_Control(pDX, IDC_ED_MOBILITY, m_ed_mobility);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakePart, CDialog)
	//{{AFX_MSG_MAP(CEdLakePart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakePart::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEdLakePart message handlers

BOOL CEdLakePart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
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
	if ( add==FALSE && pos>=lcomp->NumPartVar())
	{
		aqapp.ProgErr("Illegal ");
	}
	else
	{
		if ( add == FALSE )
		{
			int varpos = aqsys.Varlist()->Position(lcomp->PartVar(pos));
			if ( varpos <= 0 )
			{
				aqapp.ProgErr("Variable not found");
			}
			else
			{
				m_le_var.SetCurSel(varpos-1);
			}
			if ( lcomp->PartRho(pos) != 0 )
			{
				lcomp->PartRho(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_density.SetWindowText(buffer);
			}
			if ( lcomp->PartSedVeloc(pos) != 0 )
			{
				lcomp->PartSedVeloc(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_sedveloc.SetWindowText(buffer);
			}
			if ( lcomp->PartMobility(pos) != 0 )
			{
				lcomp->PartMobility(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_mobility.SetWindowText(buffer);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakePart::OnOK() 
{
	char buffer1[1024], buffer2[1024], buffer3[1024], buffer4[1024],
		 buffer5[1024], buffer6[1024];
	AQVAR* var;
	int  i;
	CARDINAL errorsrho, errorssedvel, errorsmobility;

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
	m_ed_density.GetWindowText(buffer1,sizeof(buffer1));
	m_ed_sedveloc.GetWindowText(buffer3,sizeof(buffer3));
	m_ed_mobility.GetWindowText(buffer5,sizeof(buffer5));
	if ( add == TRUE )
	{
		if ( lcomp->AddPartVar(aqsys.Varlist(),var,
					   buffer1,buffer2,errorsrho,
							   buffer3,buffer4,errorssedvel,
							   buffer5,buffer6,errorsmobility,
							   pos) == FALSE )
		{
			if ( errorsrho > 0 )
			{
				MessageBox(aqapp.ini.T(169),     // ParseErrRho
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				if ( errorssedvel > 0 )
				{
					MessageBox(aqapp.ini.T(170), // ParseErrSedVel
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
	 		   else
			   {
			   	   if ( errorsmobility > 0 )
				   {
					  MessageBox(aqapp.ini.T(689), // ParseErrMobility
					 		     aqapp.ini.T(111),
							     MB_OK+MB_ICONERROR);
				   }
				   else
				   {
					  MessageBox(aqapp.ini.T(171), // UnableAddPart
							     aqapp.ini.T(111),
							     MB_OK+MB_ICONERROR);
				   }
			   }
			}
			return;
		}
	}
	else
	{
		if ( lcomp->ReplacePartVar(aqsys.Varlist(),var,
					       buffer1,buffer2,errorsrho,
							       buffer3,buffer4,errorssedvel,
							       buffer5,buffer6,errorsmobility,
							       pos) == FALSE )
		{
			if ( errorsrho > 0 )
			{
				MessageBox(aqapp.ini.T(169),     // ParseErrRho
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				if ( errorssedvel > 0 )
				{
					MessageBox(aqapp.ini.T(170), // ParseErrSedVel
							   aqapp.ini.T(111),
							   MB_OK+MB_ICONERROR);
				}
			    else
				{
				   if ( errorsmobility > 0 )
				   {
					  MessageBox(aqapp.ini.T(689), // ParseErrMobility
							     aqapp.ini.T(111),
							     MB_OK+MB_ICONERROR);
				   }
				   else
				   {
					  MessageBox(aqapp.ini.T(172), // IllegalPart
							     aqapp.ini.T(111),
							     MB_OK+MB_ICONERROR);
				   }
				}
			}
			return;
		}
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeDiss dialog


CEdLakeDiss::CEdLakeDiss(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeDiss::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeDiss)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeDiss::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeDiss)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_SEDEXCH, m_ed_sedexch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeDiss, CDialog)
	//{{AFX_MSG_MAP(CEdLakeDiss)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeDiss::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeDiss message handlers

BOOL CEdLakeDiss::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
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
	if ( add==FALSE && pos>=lcomp->NumDissVar())
	{
		aqapp.ProgErr("Illegal Variable");
	}
	else
	{
		if ( add == FALSE )
		{
			int varpos = aqsys.Varlist()->Position(lcomp->DissVar(pos));
			if ( varpos <= 0 )
			{
				aqapp.ProgErr("Variable not found");
			}
			else
			{
				m_le_var.SetCurSel(varpos-1);
			}
			if ( lcomp->Disskdiff(pos) != 0 )
			{
				lcomp->Disskdiff(pos)->UnParse(buffer,sizeof(buffer));
				m_ed_sedexch.SetWindowText(buffer);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeDiss::OnOK() 
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
	m_ed_sedexch.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( lcomp->AddDissVar(aqsys.Varlist(),var,
					   buffer1,buffer2,parseerrors,
							   pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(173),     // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(174),     // UnableAddDiss
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( lcomp->ReplaceDissVar(aqsys.Varlist(),var,
					       buffer1,buffer2,parseerrors,
							       pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(173),     // ParseErrExch
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(175),     // IllegalDiss
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}

	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CEdLakeInits dialog


CEdLakeInits::CEdLakeInits(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeInits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeInits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeInits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeInits)
	DDX_Control(pDX, IDC_LB_INITS, m_lb_inits);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeInits, CDialog)
	//{{AFX_MSG_MAP(CEdLakeInits)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INITS, OnSelchangeLbInits)
	ON_LBN_DBLCLK(IDC_LB_INITS, OnDblclkLbInits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeInits::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeInits::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inits.GetTopIndex();
	m_lb_inits.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumInitCond(); i++ )
	{
		strcpy(buffer,lcomp->InitVar(i)->Symbol());
		strcat(buffer,"(");
		strcat(buffer,lcomp->ZoneName(lcomp->InitZone(i)));
		strcat(buffer,") : ");
		lcomp->InitVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inits.InsertString(-1,buffer);
	}
	m_lb_inits.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdLakeInits::UpdateActivation()
{
	switch ( m_lb_inits.GetSelCount() )
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
// CEdLakeInits message handlers


BOOL CEdLakeInits::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeInits::OnBuAdd() 
{
	int max = lcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakeInit EdLakeInitDlg;
	EdLakeInitDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdLakeInits::OnBuEdit() 
{
	int max = lcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakeInit EdLakeInitDlg;
	EdLakeInitDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdLakeInits::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(195),            // DelInits?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumInitCond();
	int* indices = new int[max];
	int num = m_lb_inits.GetSelItems(max,indices);
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
			if ( lcomp->DeleteInitCond(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(196),
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


void CEdLakeInits::OnSelchangeLbInits() 
{
	UpdateActivation();
	return;
}


void CEdLakeInits::OnDblclkLbInits() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeInit dialog


CEdLakeInit::CEdLakeInit(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeInit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeInit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeInit)
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_LE_ZONE, m_le_zone);
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INITCOND, m_ed_initcond);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeInit, CDialog)
	//{{AFX_MSG_MAP(CEdLakeInit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeInit::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeInit message handlers


BOOL CEdLakeInit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=lcomp->NumInitCond())
	{
		aqapp.ProgErr("Illegal Initial Condition");
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
	m_le_zone.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumZones(); i++ )
	{
		m_le_zone.AddString(lcomp->ZoneName(i));
	}

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
		m_le_zone.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(lcomp->InitVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		m_le_zone.SetCurSel(lcomp->InitZone(pos));
		if ( lcomp->InitVal(pos) != 0 )
		{
			lcomp->InitVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_initcond.SetWindowText(buffer);
		}
	}
	
	if ( lcomp->NumZones() == 1 )
	{
		m_tx_zone.EnableWindow(FALSE);
		m_le_zone.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdLakeInit::OnOK() 
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
	i = m_le_zone.GetCurSel();
	m_ed_initcond.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( lcomp->AddInitCond(aqsys.Varlist(),i,var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(197),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(198),     // UnableAddInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( lcomp->ReplaceInitCond(aqsys.Varlist(),i,var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(197),     // ParseErrInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(199),     // IllegalInit
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////


static int EditInputs(LAKECOMP* com)
{
	CEdLakeSelInp EdLakeSelInpDlg;
	if ( EdLakeSelInpDlg.DoModal(com) == IDOK )
	{
		switch ( inputtype )
		{
		case 0:
			{
				CEdLakeSurfInps EdLakeSurfInpsDlg;
				return EdLakeSurfInpsDlg.DoModal(com);
			}
		case 1:
			{
				CEdLakeLatInps EdLakeLatInpsDlg;
				return EdLakeLatInpsDlg.DoModal(com);
			}
		case 2:
			{
				CEdLakePointInps EdLakePointInpsDlg;
				return EdLakePointInpsDlg.DoModal(com);
			}
		case 3:
			{
				CEdLakeSedInps EdLakeSedInpsDlg;
				return EdLakeSedInpsDlg.DoModal(com);
			}
		default:
			{
				CEdLakeSurfInps EdLakeSurfInpsDlg;
				return EdLakeSurfInpsDlg.DoModal(com);
			}
		}
	}
	return IDCANCEL;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSelInp dialog


CEdLakeSelInp::CEdLakeSelInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSelInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSelInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeSelInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSelInp)
	DDX_Control(pDX, IDC_RB_SURFINP, m_rb_surfinp);
	DDX_Control(pDX, IDC_RB_SEDINP, m_rb_sedinp);
	DDX_Control(pDX, IDC_RB_POINTINPS, m_rb_pointinps);
	DDX_Control(pDX, IDC_RB_LATINP, m_rb_latinp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSelInp, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSelInp)
	ON_BN_CLICKED(IDC_RB_LATINP, OnRbLatinp)
	ON_BN_CLICKED(IDC_RB_POINTINPS, OnRbPointinps)
	ON_BN_CLICKED(IDC_RB_SEDINP, OnRbSedinp)
	ON_BN_CLICKED(IDC_RB_SURFINP, OnRbSurfinp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSelInp::DoModal(LAKECOMP* com)
{
	lcomp = com;

	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSelInp message handlers

BOOL CEdLakeSelInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	switch ( inputtype )
	{
	case 0:
		m_rb_surfinp.SetCheck(1);
		break;
	case 1:
		m_rb_latinp.SetCheck(1);
		break;
	case 2:
		m_rb_pointinps.SetCheck(1);
		break;
	case 3:
		m_rb_sedinp.SetCheck(1);
		break;
	default:
		inputtype = 0;
		m_rb_surfinp.SetCheck(1);
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeSelInp::OnRbSurfinp() 
{
	inputtype = 0;
	return;
}


void CEdLakeSelInp::OnRbLatinp() 
{
	inputtype = 1;
	return;
}


void CEdLakeSelInp::OnRbPointinps() 
{
	inputtype = 2;
	return;
}


void CEdLakeSelInp::OnRbSedinp() 
{
	inputtype = 3;
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSurfInps dialog


CEdLakeSurfInps::CEdLakeSurfInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSurfInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSurfInps)
	//}}AFX_DATA_INIT
}


void CEdLakeSurfInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSurfInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSurfInps, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSurfInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSurfInps::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeSurfInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumSurfInput(); i++ )
	{
		strcpy(buffer,lcomp->SurfInputVar(i)->Symbol());
		strcat(buffer," : ");
		lcomp->SurfInputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdLakeSurfInps::UpdateActivation()
{
	switch ( m_lb_inputs.GetSelCount() )
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
// CEdLakeSurfInps message handlers


BOOL CEdLakeSurfInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeSurfInps::OnBuAdd() 
{
	int max = lcomp->NumSurfInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakeSurfInp EdLakeSurfInpDlg;
	EdLakeSurfInpDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdLakeSurfInps::OnBuEdit() 
{
	int max = lcomp->NumSurfInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakeSurfInp EdLakeSurfInpDlg;
	EdLakeSurfInpDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdLakeSurfInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(200),            // DelSurfInputs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumSurfInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
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
			if ( lcomp->DeleteSurfInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(201),
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


void CEdLakeSurfInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdLakeSurfInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSurfInp dialog


CEdLakeSurfInp::CEdLakeSurfInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSurfInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSurfInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeSurfInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSurfInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSurfInp, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSurfInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSurfInp::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSurfInp message handlers


BOOL CEdLakeSurfInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=lcomp->NumSurfInput())
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

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(lcomp->SurfInputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( lcomp->SurfInputFlux(pos) != 0 )
		{
			lcomp->SurfInputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeSurfInp::OnOK() 
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
	m_ed_inpflux.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( lcomp->AddSurfInput(aqsys.Varlist(),var,
					     buffer1,buffer2,parseerrors,
							     pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(203),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(204),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( lcomp->ReplaceSurfInput(aqsys.Varlist(),var,
						 buffer1,buffer2,parseerrors,
								 pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(203),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(205),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeLatInps dialog


CEdLakeLatInps::CEdLakeLatInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeLatInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeLatInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeLatInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeLatInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeLatInps, CDialog)
	//{{AFX_MSG_MAP(CEdLakeLatInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeLatInps::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeLatInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumLatInput(); i++ )
	{
		strcpy(buffer,lcomp->LatInputVar(i)->Symbol());
		strcat(buffer," : ");
		lcomp->LatInputConc(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
	return;
}


void CEdLakeLatInps::UpdateActivation()
{
	switch ( m_lb_inputs.GetSelCount() )
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
// CEdLakeLatInps message handlers


BOOL CEdLakeLatInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	if ( lcomp->LatInflow() != 0 )
	{
		if ( lcomp->LatInflow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeLatInps::OnBuAdd() 
{
	int max = lcomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakeLatInp EdLakeLatInpDlg;
	EdLakeLatInpDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
	return;
}


void CEdLakeLatInps::OnBuEdit() 
{
	int max = lcomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakeLatInp EdLakeLatInpDlg;
	EdLakeLatInpDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
	return;
}


void CEdLakeLatInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(206),            // DelLatInputs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumLatInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
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
			if ( lcomp->DeleteLatInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(207),
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


void CEdLakeLatInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
	return;
}


void CEdLakeLatInps::OnDblclkLbInputs() 
{
	OnBuEdit();
	return;
}


void CEdLakeLatInps::OnOK() 
{
	char buffer[1024], buffer1[1024];
	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( lcomp->LatInflow(aqsys.Varlist(),buffer,buffer1) == FALSE )
	{
		MessageBox(aqapp.ini.T(208), // IllegalLatInflow
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeLatInp dialog


CEdLakeLatInp::CEdLakeLatInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeLatInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeLatInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeLatInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeLatInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeLatInp, CDialog)
	//{{AFX_MSG_MAP(CEdLakeLatInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeLatInp::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeLatInp message handlers

BOOL CEdLakeLatInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=lcomp->NumLatInput())
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

	if ( add == TRUE )
	{
		m_le_var.SetCurSel(0);
	}
	else
	{
		int varpos = aqsys.Varlist()->Position(lcomp->LatInputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( lcomp->LatInputConc(pos) != 0 )
		{
			lcomp->LatInputConc(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeLatInp::OnOK() 
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
	m_ed_inpflux.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( lcomp->AddLatInput(aqsys.Varlist(),var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(209),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(210),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( lcomp->ReplaceLatInput(aqsys.Varlist(),var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(209),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(211),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedInps dialog


CEdLakeSedInps::CEdLakeSedInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSedInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSedInps)
	//}}AFX_DATA_INIT
}


void CEdLakeSedInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSedInps)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	DDX_Control(pDX, IDC_BU_ADD, m_bu_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSedInps, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSedInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSedInps::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakeSedInps::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<lcomp->NumSedInput(); i++ )
	{
		strcpy(buffer,lcomp->SedInputVar(i)->Symbol());
		strcat(buffer," : ");
		lcomp->SedInputFlux(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
}


void CEdLakeSedInps::UpdateActivation()
{
	switch ( m_lb_inputs.GetSelCount() )
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
// CEdLakeSedInps message handlers

BOOL CEdLakeSedInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdLakeSedInps::OnBuAdd() 
{
	int max = lcomp->NumSedInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakeSedInp EdLakeSedInpDlg;
	EdLakeSedInpDlg.DoModal(lcomp,TRUE,pos);
	UpdateDialog();
}

void CEdLakeSedInps::OnBuEdit() 
{
	int max = lcomp->NumSedInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakeSedInp EdLakeSedInpDlg;
	EdLakeSedInpDlg.DoModal(lcomp,FALSE,pos);
	UpdateDialog();
}

void CEdLakeSedInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(212),            // DelSedInputs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->NumSedInput();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
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
			if ( lcomp->DeleteSedInput(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(213),
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;
	UpdateDialog();
}


void CEdLakeSedInps::OnSelchangeLbInputs() 
{
	UpdateActivation();
}


void CEdLakeSedInps::OnDblclkLbInputs() 
{
	OnBuEdit();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedInp dialog


CEdLakeSedInp::CEdLakeSedInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSedInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSedInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeSedInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSedInp)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSedInp, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSedInp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSedInp::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedInp message handlers

BOOL CEdLakeSedInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=lcomp->NumSedInput())
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
		int varpos = aqsys.Varlist()->Position(lcomp->SedInputVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( lcomp->SedInputFlux(pos) != 0 )
		{
			lcomp->SedInputFlux(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdLakeSedInp::OnOK() 
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
	m_ed_inpflux.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( lcomp->AddSedInput(aqsys.Varlist(),var,
					    buffer1,buffer2,parseerrors,
							    pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(214),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(215),     // UnableAddInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( lcomp->ReplaceSedInput(aqsys.Varlist(),var,
						buffer1,buffer2,parseerrors,
								pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(214),     // ParseErrInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(216),     // IllegalInp
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInps dialog


CEdLakePointInps::CEdLakePointInps(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakePointInps::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakePointInps)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakePointInps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakePointInps)
	DDX_Control(pDX, IDC_LB_PINPS, m_lb_pinps);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakePointInps, CDialog)
	//{{AFX_MSG_MAP(CEdLakePointInps)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_LBN_SELCHANGE(IDC_LB_PINPS, OnSelchangeLbPinps)
	ON_LBN_DBLCLK(IDC_LB_PINPS, OnDblclkLbPinps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakePointInps::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


void CEdLakePointInps::UpdateDialog()
{
	int topindex = m_lb_pinps.GetTopIndex();
	m_lb_pinps.ResetContent();
	PINF* pinf = lcomp->PointInflist()->First();
	while ( pinf != 0 )
	{
		m_lb_pinps.InsertString(-1,pinf->Symbol());
		pinf = pinf->Next();
	}
	m_lb_pinps.SetTopIndex(topindex);
	UpdateActivation();
}


void CEdLakePointInps::UpdateActivation()
{
	switch ( m_lb_pinps.GetSelCount() )
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
// CEdLakePointInps message handlers

BOOL CEdLakePointInps::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakePointInps::OnBuAdd() 
{
	CEdLakePointInp EdLakePointInpDlg;
	EdLakePointInpDlg.DoModal(lcomp,0);
	UpdateDialog();
	return;
}


void CEdLakePointInps::OnBuEdit() 
{
	int max = lcomp->PointInflist()->Size();
	int* indices = new int[max];
	int num = m_lb_pinps.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	PINF* pinf = lcomp->PointInflist()->Get(indices[0]+1);
	delete [] indices;
	CEdLakePointInp EdLakePointInpDlg;
	EdLakePointInpDlg.DoModal(lcomp,pinf);

	UpdateDialog();
	return;
}


void CEdLakePointInps::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(217),            // DelPointInps?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = lcomp->PointInflist()->Size();
	int* indices = new int[max];
	int num = m_lb_pinps.GetSelItems(max,indices);
	int j,k;
	PINF* pinf;
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
			pinf = lcomp->PointInflist()->Get(indices[j]-k+1);
			if ( lcomp->DeletePointInf(pinf) == TRUE )
			{
				k++;
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(218)); // FailDelPointInf
				strcat(buffer," ");
				strcat(buffer,pinf->Symbol());
				MessageBox(buffer,
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


void CEdLakePointInps::OnSelchangeLbPinps() 
{
	UpdateActivation();
}


void CEdLakePointInps::OnDblclkLbPinps() 
{
	OnBuEdit();     
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInp dialog


CEdLakePointInp::CEdLakePointInp(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakePointInp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakePointInp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakePointInp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakePointInp)
	DDX_Control(pDX, IDC_LB_INPUTS, m_lb_inputs);
	DDX_Control(pDX, IDC_ED_WATINF, m_ed_watinf);
	DDX_Control(pDX, IDC_ED_RANGE, m_ed_range);
	DDX_Control(pDX, IDC_ED_NAME, m_ed_name);
	DDX_Control(pDX, IDC_ED_DESCRIPT, m_ed_descript);
	DDX_Control(pDX, IDC_ED_DEPTH, m_ed_depth);
	DDX_Control(pDX, IDC_BU_EDIT, m_bu_edit);
	DDX_Control(pDX, IDC_BU_DELETE, m_bu_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakePointInp, CDialog)
	//{{AFX_MSG_MAP(CEdLakePointInp)
	ON_BN_CLICKED(IDC_BU_ADD, OnBuAdd)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_LBN_SELCHANGE(IDC_LB_INPUTS, OnSelchangeLbInputs)
	ON_LBN_DBLCLK(IDC_LB_INPUTS, OnDblclkLbInputs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakePointInp::DoModal(LAKECOMP* com, PINF* p)
{
	lcomp = com;
	oldpinf = p;
	pinf = new PINF(oldpinf);
	return CDialog::DoModal();
}


void CEdLakePointInp::UpdateDialog()
{
	char buffer[1024]; char buffer1[1024];
	int topindex = m_lb_inputs.GetTopIndex();
	m_lb_inputs.ResetContent();
	for ( CARDINAL i=0; i<pinf->NumConc(); i++ )
	{
		strcpy(buffer,pinf->ConcVar(i)->Symbol());
		strcat(buffer," : ");
		pinf->ConcVal(i)->UnParse(buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		m_lb_inputs.InsertString(-1,buffer);
	}
	m_lb_inputs.SetTopIndex(topindex);
	UpdateActivation();
}


void CEdLakePointInp::UpdateActivation()
{
	switch ( m_lb_inputs.GetSelCount() )
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
// CEdLakePointInp message handlers

BOOL CEdLakePointInp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( pinf->Symbol() != 0 )
	{
		m_ed_name.SetWindowText(pinf->Symbol());
	}

	if ( pinf->Description() != 0 )
	{
		m_ed_descript.SetWindowText(pinf->Description());
	}

	if ( pinf->Depth() != 0 )
	{
		if ( pinf->Depth()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_depth.SetWindowText(buffer);
		}
	}

	if ( pinf->Width() != 0 )
	{
		if ( pinf->Width()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_range.SetWindowText(buffer);
		}
	}

	if ( pinf->WaterFlow() != 0 )
	{
		if ( pinf->WaterFlow()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_watinf.SetWindowText(buffer);
		}
	}

	UpdateDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakePointInp::OnBuAdd() 
{
	int max = pinf->NumConc();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CEdLakePointInpConc EdLakePointInpConcDlg;
	EdLakePointInpConcDlg.DoModal(pinf,TRUE,pos);
	UpdateDialog();
}


void CEdLakePointInp::OnBuEdit() 
{
	int max = pinf->NumConc();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CEdLakePointInpConc EdLakePointInpConcDlg;
	EdLakePointInpConcDlg.DoModal(pinf,FALSE,pos);
	UpdateDialog();
}


void CEdLakePointInp::OnBuDelete() 
{
	switch ( MessageBox(aqapp.ini.T(228),            // DelPointInpConcs?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = pinf->NumConc();
	int* indices = new int[max];
	int num = m_lb_inputs.GetSelItems(max,indices);
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
			if ( pinf->DeleteConc(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				MessageBox(aqapp.ini.T(229),
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;
	UpdateDialog();
}


void CEdLakePointInp::OnSelchangeLbInputs() 
{
	UpdateActivation();
}


void CEdLakePointInp::OnDblclkLbInputs() 
{
	OnBuEdit();
}


void CEdLakePointInp::OnCancel() 
{
	delete pinf;
	
	CDialog::OnCancel();
}


void CEdLakePointInp::OnOK() 
{
	char buffer[1024], buffer1[1024]; CARDINAL numerr;

	m_ed_name.GetWindowText(buffer,sizeof(buffer));
	if ( pinf->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(219),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_ed_descript.GetWindowText(buffer,sizeof(buffer));
	pinf->Description(buffer);

	m_ed_depth.GetWindowText(buffer,sizeof(buffer));
	if ( pinf->Depth(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(220), // ParseErrDepth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(221), // IllegalDepth
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_range.GetWindowText(buffer,sizeof(buffer));
	if ( pinf->Width(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(222), // ParseErrRange
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(223), // IllegalRange
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_watinf.GetWindowText(buffer,sizeof(buffer));
	if ( pinf->WaterFlow(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(224), // ParseErrWatInf
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(225), // IllegalWatInf
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( oldpinf == 0 )      // Add
	{
		if ( lcomp->AddPointInf(pinf) == FALSE )
		{
			MessageBox(aqapp.ini.T(226), // UnableAddPInf
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else                     // Edit
	{
		if ( lcomp->ReplacePointInf(oldpinf,pinf) == FALSE )
		{
			MessageBox(aqapp.ini.T(227), // IllegalPInf
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInpConc dialog


CEdLakePointInpConc::CEdLakePointInpConc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakePointInpConc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakePointInpConc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakePointInpConc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakePointInpConc)
	DDX_Control(pDX, IDC_LE_VAR, m_le_var);
	DDX_Control(pDX, IDC_ED_INPFLUX, m_ed_inpflux);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakePointInpConc, CDialog)
	//{{AFX_MSG_MAP(CEdLakePointInpConc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakePointInpConc::DoModal(PINF* p, BOOL a, int i)
{
	pinf = p;
	add = a;
	pos = i;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInpConc message handlers

BOOL CEdLakePointInpConc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( add==FALSE && pos>=pinf->NumConc())
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
		int varpos = aqsys.Varlist()->Position(pinf->ConcVar(pos));
		if ( varpos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_le_var.SetCurSel(varpos-1);
		}
		if ( pinf->ConcVal(pos) != 0 )
		{
			pinf->ConcVal(pos)->UnParse(buffer,sizeof(buffer));
			m_ed_inpflux.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CEdLakePointInpConc::OnOK() 
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
	m_ed_inpflux.GetWindowText(buffer1,sizeof(buffer1));
	if ( add == TRUE )
	{
		if ( pinf->AddConc(aqsys.Varlist(),var,
				       buffer1,buffer2,parseerrors,
						   pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(230),     // ParseErrConc
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(231),     // UnableAddConc
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	else
	{
		if ( pinf->ReplaceConc(aqsys.Varlist(),var,
					   buffer1,buffer2,parseerrors,
							   pos) == FALSE )
		{
			if ( parseerrors > 0 )
			{
				MessageBox(aqapp.ini.T(230),     // ParseErrConc
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			else
			{
				MessageBox(aqapp.ini.T(232),     // IllegalConc
						   aqapp.ini.T(111),
						   MB_OK+MB_ICONERROR);
			}
			return;
		}
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeAcc dialog


CEdLakeAcc::CEdLakeAcc(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeAcc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeAcc)
	//}}AFX_DATA_INIT
}


void CEdLakeAcc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeAcc)
	DDX_Control(pDX, IDC_ED_RELACCI, m_ed_relacci);
	DDX_Control(pDX, IDC_ED_ABSACCI, m_ed_absacci);
	DDX_Control(pDX, IDC_ED_RELACCESEICHE, m_ed_relacceseiche);
	DDX_Control(pDX, IDC_ED_ABSACCESEICHE, m_ed_absacceseiche);
	DDX_Control(pDX, IDC_ED_RELACCU, m_ed_relaccu);
	DDX_Control(pDX, IDC_ED_RELACCQ, m_ed_relaccq);
	DDX_Control(pDX, IDC_ED_RELACCN2, m_ed_relaccn2);
	DDX_Control(pDX, IDC_ED_ABSACCQ, m_ed_absaccq);
	DDX_Control(pDX, IDC_ED_ABSACCN2, m_ed_absaccn2);
	DDX_Control(pDX, IDC_ED_ABSACCTKE, m_ed_absacctke);
	DDX_Control(pDX, IDC_ED_RELACCTKE, m_ed_relacctke);
	DDX_Control(pDX, IDC_ED_ABSACCU, m_ed_absaccu);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeAcc, CDialog)
	//{{AFX_MSG_MAP(CEdLakeAcc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeAcc::DoModal(LAKECOMP* com)
{
	lcomp = com;
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeAcc message handlers


BOOL CEdLakeAcc::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char buffer[1024];
	
	NumToString(lcomp->RelAccQ(),buffer,sizeof(buffer));
	m_ed_relaccq.SetWindowText(buffer);
	NumToString(lcomp->AbsAccQ(),buffer,sizeof(buffer));
	m_ed_absaccq.SetWindowText(buffer);
	
	NumToString(lcomp->RelAccI(),buffer,sizeof(buffer));
	m_ed_relacci.SetWindowText(buffer);
	NumToString(lcomp->AbsAccI(),buffer,sizeof(buffer));
	m_ed_absacci.SetWindowText(buffer);
	
	NumToString(lcomp->RelAccU(),buffer,sizeof(buffer));
	m_ed_relaccu.SetWindowText(buffer);
	NumToString(lcomp->AbsAccU(),buffer,sizeof(buffer));
	m_ed_absaccu.SetWindowText(buffer);
	
	NumToString(lcomp->RelAccTKE(),buffer,sizeof(buffer));
	m_ed_relacctke.SetWindowText(buffer);
	NumToString(lcomp->AbsAccTKE(),buffer,sizeof(buffer));
	m_ed_absacctke.SetWindowText(buffer);
	
	NumToString(lcomp->RelAccEPS(),buffer,sizeof(buffer));
	m_ed_relaccn2.SetWindowText(buffer);
	NumToString(lcomp->AbsAccEPS(),buffer,sizeof(buffer));
	m_ed_absaccn2.SetWindowText(buffer);
	
	NumToString(lcomp->RelAccEseiche(),buffer,sizeof(buffer));
	m_ed_relacceseiche.SetWindowText(buffer);
	NumToString(lcomp->AbsAccEseiche(),buffer,sizeof(buffer));
	m_ed_absacceseiche.SetWindowText(buffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeAcc::OnOK() 
{
	char buffer[1024];
	REAL relacc, absacc;

	m_ed_relaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(556),  // FailRelAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccq.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(557),  // FailAbsAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->AccQ(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(558),  // FailAccQ
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relacci.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(647),  // FailRelAccI
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacci.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(648),  // FailAbsAccI
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->AccI(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(649),  // FailAccI
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccn2.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(559),  // FailRelAccN2
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccn2.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(560),  // FailRelAccEPS
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->AccEPS(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(561),  // FailAccEPS
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relaccu.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(562),  // FailRelAccU
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absaccu.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(563),  // FailRelAccU
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->AccU(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(564),  // FailAccU
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relacctke.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(565),  // FailRelAccTKE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacctke.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(566),  // FailAbsAccTKE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->AccTKE(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(567),  // FailAccTKE
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	m_ed_relacceseiche.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&relacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(568),  // FailRelAccEs
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absacceseiche.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(569),  // FailAbsAccEs
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( lcomp->AccEseiche(relacc,absacc) == FALSE )
	{
		MessageBox(aqapp.ini.T(570),  // FailAccEs
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedLayer dialog


CEdLakeSedLayer::CEdLakeSedLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CEdLakeSedLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdLakeSedLayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEdLakeSedLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdLakeSedLayer)
	DDX_Control(pDX, IDC_ED_ZONEIND, m_ed_zind);
	DDX_Control(pDX, IDC_ED_THICKNESS, m_ed_thick);
	DDX_Control(pDX, IDC_ED_POROSITY, m_ed_por);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdLakeSedLayer, CDialog)
	//{{AFX_MSG_MAP(CEdLakeSedLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEdLakeSedLayer::DoModal(LAKECOMP* com, BOOL a, int i)
{
	lcomp = com;
	add   = a;
	pos   = i;
	if ( add == TRUE )
	{
		sedlayer = new LAKESEDLAYER;
	}
	else
	{
		sedlayer = new LAKESEDLAYER(lcomp->SedLayer(pos));
	}
	return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedLayer message handlers


BOOL CEdLakeSedLayer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];
	
	NumToString(sedlayer->ZoneIndex(),buffer,sizeof(buffer));
	m_ed_zind.SetWindowText(buffer);

	if ( sedlayer->Thickness() != 0 )
	{
		if ( sedlayer->Thickness()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_thick.SetWindowText(buffer);
		}
	}
	
	if ( sedlayer->Porosity() != 0 )
	{
		if ( sedlayer->Porosity()->UnParse(buffer,sizeof(buffer)) == 0 )
		{
			m_ed_por.SetWindowText(buffer);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEdLakeSedLayer::OnOK() 
{
	char buffer[1024]; char buffer1[1024]; CARDINAL c,numerr;

	m_ed_zind.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&c) == FALSE )
	{
		MessageBox(aqapp.ini.T(621), // FailZoneInd
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	sedlayer->ZoneIndex(c);

	m_ed_thick.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->Thickness(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(145), // ParseErrThickness
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(146), // IllegalThickness
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	m_ed_por.GetWindowText(buffer,sizeof(buffer));
	if ( sedlayer->Porosity(buffer,aqsys.Varlist(),buffer1,numerr) == FALSE )
	{
		if ( numerr > 0 )
		{
			MessageBox(aqapp.ini.T(147), // ParseErrPorosity
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
		else
		{
			MessageBox(aqapp.ini.T(148), // IllegalPorosity
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}

	if ( add == TRUE )
	{
		if ( lcomp->AddSedLayer(sedlayer,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(622), // FailAddSedLayer
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( lcomp->ReplaceSedLayer(sedlayer,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(623), // IllegalSedlayer
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONERROR);
			return;
		}
	}
	
	CDialog::OnOK();
}


void CEdLakeSedLayer::OnCancel() 
{
	delete sedlayer;
	
	CDialog::OnCancel();
}
