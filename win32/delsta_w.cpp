// delsta_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "mainfr_w.h"
#include "delsta_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelStates dialog


CDelStates::CDelStates(CWnd* pParent /*=NULL*/)
	: CDialog(CDelStates::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelStates)
	//}}AFX_DATA_INIT

	created = 0;
	calcnum = 0;
}


void CDelStates::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelStates)
	DDX_Control(pDX, IDC_LB_STATELIST, m_statelist);
	DDX_Control(pDX, IDC_LB_CALCLIST, m_calclist);
	DDX_Control(pDX, IDC_BU_DELETESTATES, m_deletestates);
	DDX_Control(pDX, IDC_BU_DELETECALCS, m_deletecalcs);
	DDX_Control(pDX, IDC_TX_STATES, m_tx_states);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelStates, CDialog)
	//{{AFX_MSG_MAP(CDelStates)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LB_STATELIST, OnSelchangeLbStatelist)
	ON_LBN_SELCHANGE(IDC_LB_CALCLIST, OnSelchangeLbCalclist)
	ON_BN_CLICKED(IDC_BU_DELETECALCS, OnBuDeletecalcs)
	ON_BN_CLICKED(IDC_BU_DELETESTATES, OnBuDeletestates)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDelStates::UpdateActivation()
{
	switch ( m_calclist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("List box selection error");
		break;
	case 0:
		m_deletecalcs.EnableWindow(FALSE);
		break;
	default:
		m_deletecalcs.EnableWindow(TRUE);
		break;
	}
	switch ( m_statelist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("List box selection error");
		break;
	case 0:
		m_deletestates.EnableWindow(FALSE);
		break;
	default:
		m_deletestates.EnableWindow(TRUE);
		break;
	}
	UpdateNumStates();
	return;
}


void CDelStates::UpdateDialog()
{
	if ( created == 0 ) return;
	UpdateCalcList();
	UpdateActivation();
	return;
}


void CDelStates::UpdateCalcList()
{
	char buffer[1024]; CARDINAL calcnum;
	int topindex = m_calclist.GetTopIndex();
	m_calclist.ResetContent();
	for ( CARDINAL i=0; i<aqsys.NumCalc(); i++)
	{
		calcnum = aqsys.CalcNums(i);
		NumToString(calcnum,buffer,sizeof(buffer));
		m_calclist.InsertString(-1,buffer);
	}
	m_calclist.SetTopIndex(topindex);
	m_statelist.ResetContent();
	return;
}


void CDelStates::UpdateStateList()
{
	char buffer[1024];
	int      lbindex;
	CARDINAL index;
	switch ( m_calclist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("List box selection error");
		break;
	case 0:
		m_statelist.ResetContent();
		break;
	case 1:
		if ( m_calclist.GetSelItems(1,&lbindex) != 1 )
		{
			aqapp.ProgErr("List box selection error");
			break;
		}
		if ( lbindex < 0 )
		{
			aqapp.ProgErr("Illegal calculation number index");
			break;
		}
		index = (CARDINAL)lbindex;
		if ( index >= aqsys.NumCalc() )
		{
			aqapp.ProgErr("Illegal calculation number index");
			break;
		}
		else
		{
			int topindex = 0;
			if ( aqsys.CalcNums(index) == calcnum )
			{
				topindex = m_statelist.GetTopIndex();
			}
			else
			{
				calcnum = aqsys.CalcNums(index);
			}
			m_statelist.ResetContent();
			for ( CARDINAL i=0; i<aqsys.NumStates(calcnum); i++ )
			{
				const STATE* state = aqsys.State(calcnum,i);
				NumToString(state->Time(),buffer,sizeof(buffer));
				m_statelist.InsertString(-1,buffer);
			}
			m_statelist.SetTopIndex(topindex);
		}
		break;
	default:
		m_statelist.ResetContent();
		break;
	}
	UpdateNumStates();
	return;
}


void CDelStates::UpdateNumStates()
{
	char buffer[1024]; char buffer1[1024];
	strcpy(buffer,"States");
	if ( m_calclist.GetSelCount() != 1 )
	{
		strcat(buffer,":");
	}
	else
	{
		strcat(buffer," (");
		NumToString(aqsys.NumStates(calcnum),buffer1,sizeof(buffer1));
		strcat(buffer,buffer1);
		strcat(buffer,"):");
	}
	m_tx_states.SetWindowText(buffer);
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CDelStates message handlers

BOOL CDelStates::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CDelStates::OnOK() // Close button
{
	DestroyWindow();
	return;
}


void CDelStates::OnCancel() // Close button 
{
	DestroyWindow();
	return;
}


void CDelStates::OnClose() // Close box
{
	DestroyWindow();

	CDialog::OnClose();
	return;
}


void CDelStates::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
	return;
}


void CDelStates::OnBuDeletecalcs() 
{
	int max = aqsys.NumCalc();
	int* indices = new int[max];
	int num = m_calclist.GetSelItems(max,indices);
	int j,k;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box index");
		break;
	case 0:
		aqapp.ProgErr("Illegal list box index");
		break;
	default:
		k = 0;
		for ( j=0; j<num; j++ )
		{
			calcnum = aqsys.CalcNums(indices[j]-k);
			if ( aqsys.DeleteStates(calcnum) == TRUE )
			{
				k++;
			}
			else
			{
			}
		}
		break;
	}
	delete [] indices;
	UpdateCalcList();
	UpdateActivation();     
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


void CDelStates::OnBuDeletestates() 
{
	int max = aqsys.NumStates(calcnum);
	if ( max <= 0 )
	{
		aqapp.ProgErr("Illegal calculation number");
		return;
	}
	int* indices = new int[max];
	int num = m_statelist.GetSelItems(max,indices);
	int j,k;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box index");
		break;
	case 0:
		aqapp.ProgErr("Illegal list box index");
		break;
	default:
		k = 0;
		for ( j=0; j<num; j++ )
		{
			if ( aqsys.DeleteState(calcnum,indices[j]-k) == TRUE )
			{
				if ( m_statelist.DeleteString(indices[j]-k) == LB_ERR )
				{
					aqapp.ProgErr("Unable to delete list box item");
					UpdateStateList();
				}
				k++;
			}
			else
			{
				aqapp.ProgErr("Unable to delete state");
			}
		}
		break;
	}
	delete [] indices;
	if ( aqsys.NumStates(calcnum) == 0 )
	{
		UpdateCalcList();
	}
	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


void CDelStates::OnSelchangeLbStatelist() 
{
	UpdateActivation();
}

void CDelStates::OnSelchangeLbCalclist() 
{
	// TODO: Add your control notification handler code here
	UpdateStateList();
	UpdateActivation();
}


