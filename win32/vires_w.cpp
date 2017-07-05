// vires_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "vires_w.h"
#include "mainfr_w.h"					 // *JR* aug 96 ???
//#include "plot_w.h"					 // *JR* aug 96 ???
#include "waidlg_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViRes dialog


CViRes::CViRes(CWnd* pParent /*=NULL*/)
	: CDialog(CViRes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViRes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	created = 0;
}


void CViRes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViRes)
	DDX_Control(pDX, IDC_LB_PLOTLIST, m_plotlist);
	DDX_Control(pDX, IDC_BU_PLOTTOSCREEN, m_plottoscreen);
	DDX_Control(pDX, IDC_BU_PLOTTOFILE, m_plottofile);
	DDX_Control(pDX, IDC_BU_LISTTOFILE, m_listtofile);
	DDX_Control(pDX, IDC_BU_EDIT, m_edit);
	DDX_Control(pDX, IDC_BU_DUPLICATE, m_duplicate);
	DDX_Control(pDX, IDC_BU_DELETE, m_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViRes, CDialog)
	//{{AFX_MSG_MAP(CViRes)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BU_DELETE, OnBuDelete)
	ON_BN_CLICKED(IDC_BU_DUPLICATE, OnBuDuplicate)
	ON_BN_CLICKED(IDC_BU_EDIT, OnBuEdit)
	ON_BN_CLICKED(IDC_BU_FILEOPT, OnBuFileopt)
	ON_BN_CLICKED(IDC_BU_LISTOPT, OnBuListopt)
	ON_BN_CLICKED(IDC_BU_NEW, OnBuNew)
	ON_BN_CLICKED(IDC_BU_SCROPT, OnBuScropt)
	ON_BN_CLICKED(IDC_BU_PLOTTOSCREEN, OnBuPlottoscreen)
	ON_BN_CLICKED(IDC_BU_PLOTTOFILE, OnBuPlottofile)
	ON_BN_CLICKED(IDC_BU_LISTTOFILE, OnBuListtofile)
	ON_LBN_SELCHANGE(IDC_LB_PLOTLIST, OnSelchangeLbPlotlist)
	ON_LBN_DBLCLK(IDC_LB_PLOTLIST, OnDblclkLbPlotlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CViRes::UpdateActivation()
{
	switch ( m_plotlist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(FALSE);
		m_plottoscreen.EnableWindow(FALSE);
		m_plottofile.EnableWindow(FALSE);
		m_listtofile.EnableWindow(FALSE);
		break;
	case 1:
		m_duplicate.EnableWindow(TRUE);
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		m_plottoscreen.EnableWindow(TRUE);
		m_plottofile.EnableWindow(TRUE);
		m_listtofile.EnableWindow(TRUE);
		break;
	default:
		m_duplicate.EnableWindow(FALSE);
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		m_plottoscreen.EnableWindow(FALSE);
		m_plottofile.EnableWindow(FALSE);
		m_listtofile.EnableWindow(FALSE);
	   break;
	}
	return;
}


void CViRes::UpdateDialog()
{
	if ( created == 0 ) return;

	int topindex = m_plotlist.GetTopIndex();
	m_plotlist.ResetContent();
	PLOT* plot = aqsys.Plotlist()->First();
	while ( plot != 0 )
	{
	   m_plotlist.InsertString(-1,plot->Symbol());
	   plot = plot->Next();
	}
	m_plotlist.SetTopIndex(topindex);

	UpdateActivation();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CViRes message handlers

BOOL CViRes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	created = 1;
	UpdateDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CViRes::OnOK() // Close button
{
	DestroyWindow();
	return;
}


void CViRes::OnCancel() // Close button 
{
	DestroyWindow();
	return;
}


void CViRes::OnClose() // Close box
{
	DestroyWindow();

	CDialog::OnClose();
}


void CViRes::OnDestroy() 
{
	CDialog::OnDestroy();
	
	created = 0;
}


void CViRes::OnBuNew() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CViewEdPlotDef EdPlotDefDlg;
	EdPlotDefDlg.DoModal(0,TRUE);

	UpdateDialog();
	return;
}


void CViRes::OnBuDuplicate() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Plotlist()->Size();
	int* indices = new int[max];
	int num = m_plotlist.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	PLOT* plot = aqsys.Plotlist()->Get(indices[0]+1);
	delete [] indices;
	CViewEdPlotDef EdPlotDefDlg;
	EdPlotDefDlg.DoModal(plot,TRUE);

	UpdateDialog();
	return;
}


void CViRes::OnBuEdit() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Plotlist()->Size();
	int* indices = new int[max];
	int num = m_plotlist.GetSelItems(max,indices);
	if ( num == 0 )
	{
		aqapp.ProgErr("Illegal selection");
        delete [] indices;
		return;
	}
    if ( num == 1 )
    {
	   PLOT* plot = aqsys.Plotlist()->Get(indices[0]+1);
	   CViewEdPlotDef EdPlotDefDlg;
	   EdPlotDefDlg.DoModal(plot,FALSE);
    }
    else
    {
	   CViewEdScalings EdScalingsDlg;
	   EdScalingsDlg.DoModal(num,indices);
    }

	delete [] indices;
	UpdateDialog();
	return;
}


void CViRes::OnBuDelete() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	switch ( MessageBox(aqapp.ini.T(16),             // DelPlots?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = aqsys.Plotlist()->Size();
	int* indices = new int[max];
	int num = m_plotlist.GetSelItems(max,indices);
	int j,k;
	PLOT* plot;
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
			plot = aqsys.Plotlist()->Get(indices[j]-k+1);
//			PLOT* oldplot=plot;								// *JR* sep96
			if ( aqsys.DeletePlot(plot) == TRUE )
			{
				k++;
				// *JR* sep96:
//				aqapp.MainFrame->plotmanager.ReplacePlotPtr(oldplot,NULL);
			}
			else
			{
				char buffer[128];
				strcpy(buffer,aqapp.ini.T(17)); // FailDelPlots
				strcat(buffer," ");
				strcat(buffer,plot->Symbol());
				MessageBox(buffer,
					       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			}
		}
		break;
	}
	delete [] indices;

	UpdateDialog();	
}  // end CViRes::OnBuDelete()									// *JR* sep96


void CViRes::OnBuScropt() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CViewScreenOpt OptPlotScreenDlg;
	OptPlotScreenDlg.DoModal();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


void CViRes::OnBuFileopt() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CViewPSOpt OptPlotFileDlg;
	OptPlotFileDlg.DoModal();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


void CViRes::OnBuListopt() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	CViewListOpt OptListFileDlg;
	OptListFileDlg.DoModal();
	aqapp.MainFrame->RefreshFrameTitle();
	return;
}


void CViRes::OnBuPlottoscreen() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Plotlist()->Size();
	int* indices = new int[max];
	int num = m_plotlist.GetSelItems(max,indices);
	PLOT* plot;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("No plot selected");
		break;
	case 1:
		plot = aqsys.Plotlist()->Get(indices[0]+1);
		if ( plot == 0 )
		{
			aqapp.ProgErr("Plot not found");
			break;
		}
		switch ( DrawPlot(plot) )
		{
		case OK:
			break;
		case INTERRUPT:
			MessageBox(aqapp.ini.T(25),     // PlotInterrupt
				       aqapp.ini.T(111),
					   MB_OK+MB_ICONEXCLAMATION);
			break;
		case PROBLEM:
			MessageBox(aqapp.ini.T(26),     // PlotFailure
				       aqapp.ini.T(111),
						   MB_OK+MB_ICONEXCLAMATION);
			break;
		}
		break;
	default:
		aqapp.ProgErr("More than one plot selected");
		break;
	}
	delete [] indices;
	return;
}


void CViRes::OnBuPlottofile() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Plotlist()->Size();
	int* indices = new int[max];
	int num = m_plotlist.GetSelItems(max,indices);
	PLOT* plot;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("No plot selected");
		break;
	case 1:
		plot = aqsys.Plotlist()->Get(indices[0]+1);
		if ( plot == 0 )
		{
			aqapp.ProgErr("Plot not found");
			break;
		}
		{
			char filter[260];
			char filename[260];
			char filetitle[260]; 
			char title[260];
			char dirname[MAX_PATH];
			char fileextension[4];
			strcpy(filename,aqapp.plotfilename);
			filetitle[0] = '\0';
			strcpy(title,"PostScript (*.ps) or Encapsulated PostScript (*.eps) file:");
			dirname[0] = '\0';
			if (aqsys.PlotFilOpt()->Eps() == TRUE) {
				strcpy(fileextension,"eps");
			//	strcpy(filter,"Encapsulated PostScript Files (*.eps)0*.eps0PostScript Files (*.ps)0*.ps0");
			//	filter[37] = '\0'; filter[43] = '\0';
			//	filter[67] = '\0'; filter[72] = '\0';
			}
			else {
				strcpy(fileextension,"ps");
			//	strcpy(filter,"PostScript Files (*.ps)0*.ps0Encapsulated PostScript Files (*.eps)0*.eps0");
			//	filter[23] = '\0'; filter[28] = '\0';
			//	filter[66] = '\0'; filter[72] = '\0';
			}
			strcpy(filter,"PostScript Files (*.ps;*.eps)0*.ps;*.eps0");
			filter[29] = '\0'; filter[40] = '\0';
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
											   OFN_LONGNAMES;
			filenamestruct.lpstrDefExt       = fileextension; 
			filenamestruct.lCustData         = NULL; 
			filenamestruct.lpfnHook          = NULL; 
			filenamestruct.lpTemplateName    = NULL; 

			BOOLEAN FileWrite=TRUE;
			if ( GetSaveFileName(&filenamestruct) )
			{
				BOOLEAN EPS = aqsys.PlotFilOpt()->Eps();
				std::ifstream oldplotfile;
				std::ofstream newplotfile;
				if ( ExistFile(filename) == TRUE )					// *JR*
				{													// *JR*
					if (!EPS) oldplotfile.open(filename,std::ios::in);	// *JR*
					else { // overwrite warning						// *JR*
						CString f;
						f=f+"Overwrite " + filename;
						FileWrite=IDYES == AfxMessageBox(f,MB_YESNO);
					}
				}
				if (FileWrite) {									// *JR*
				aqapp.CancelDlg.Display(aqapp.ini.T(124)); // Plotting
				newplotfile.open(ptstmpfile,std::ios::out|std::ios::trunc);
			    SetFileType(ptstmpfile,FTpts,FCpts);
				JOBSTATUS status = aqsys.PlotResults(oldplotfile,newplotfile,plot,EPS);	// *JR*
				oldplotfile.close();
				newplotfile.close();
				aqapp.CancelDlg.Remove();
				aqapp.UpdateDialogs();
				switch ( status )
				{
					case OK:
						strcpy(aqapp.plotfilename,filename);
						if ( ExistFile(filename) == TRUE ) RemoveFile(filename);
						RenameFile(ptstmpfile,filename);
					   AfxMessageBox(aqapp.ini.T(125),MB_OK+MB_ICONINFORMATION); // PlotSuccess
						break;
					case INTERRUPT:
                        RemoveFile(ptstmpfile);
						AfxMessageBox(aqapp.ini.T(126),MB_OK+MB_ICONEXCLAMATION); // PlotInterrupt
						break;
					case PROBLEM:
                        RemoveFile(ptstmpfile);
						AfxMessageBox(aqapp.ini.T(127),MB_OK+MB_ICONERROR); // PlotError
						break;
				}
				} // end if FileWrite
			}
			else
			{
				// no file selected
			}
		}
		break;
	default:
		aqapp.ProgErr("More than one plot selected");
		break;
	}
	delete [] indices;
	return;
}


void CViRes::OnBuListtofile() 
{
	if ( aqapp.InnerMessageLoop() )
	{
		aqapp.NoteInnerMessageLoop();
		return;
	}

	int max = aqsys.Plotlist()->Size();
	int* indices = new int[max];
	int num = m_plotlist.GetSelItems(max,indices);
	PLOT* plot;
	switch ( num )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		aqapp.ProgErr("No plot selected");
		break;
	case 1:
		plot = aqsys.Plotlist()->Get(indices[0]+1);
		if ( plot == 0 )
		{
			aqapp.ProgErr("Plot not found");
			break;
		}
		{
			char filter[260];
			char filename[260];
			char filetitle[260]; 
			char title[260];
			char dirname[MAX_PATH];
			char fileextension[4];

			strcpy(filename,aqapp.listfilename);
			strcpy(filter,"List Files0*.lis0");
			filter[10] = '\0'; filter[16] = '\0';
			filetitle[0] = '\0';
			strcpy(title,"Select list file name:");
			dirname[0] = '\0';
			strcpy(fileextension,"lis");
 
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
											   OFN_LONGNAMES;
			filenamestruct.lpstrDefExt       = fileextension; 
			filenamestruct.lCustData         = NULL; 
			filenamestruct.lpfnHook          = NULL; 
			filenamestruct.lpTemplateName    = NULL; 

			if ( GetSaveFileName(&filenamestruct) )
			{
				aqapp.CancelDlg.Display(aqapp.ini.T(120)); // Listing
				std::ofstream listfile;
				listfile.open(filename,std::ios::out|std::ios::app);
				SetFileType(filename,FTlis,FClis);
				JOBSTATUS status = aqsys.ListResults(listfile,plot);
				listfile.close();
				aqapp.CancelDlg.Remove();
				aqapp.UpdateDialogs();
				switch ( status )
				{
					case OK:
						strcpy(aqapp.listfilename,filename);
						AfxMessageBox(aqapp.ini.T(121),MB_OK+MB_ICONINFORMATION); // ListSuccess
						break;
					case INTERRUPT:
						AfxMessageBox(aqapp.ini.T(122),MB_OK+MB_ICONEXCLAMATION); // ListInterrupt
						break;
					case PROBLEM:
						AfxMessageBox(aqapp.ini.T(123),MB_OK+MB_ICONERROR); // ListError
						break;
				}
			}
			else
			{
				// no file selected
			}
		}
		break;
	default:
		aqapp.ProgErr("More than one plot selected");
		break;
	}
	delete [] indices;
	return;
}


void CViRes::OnSelchangeLbPlotlist() 
{
	UpdateActivation();
}


void CViRes::OnDblclkLbPlotlist() 
{
	OnBuEdit();
}


/////////////////////////////////////////////////////////////////////////////
// CViewScreenOpt dialog


CViewScreenOpt::CViewScreenOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CViewScreenOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewScreenOpt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewScreenOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewScreenOpt)
	DDX_Control(pDX, IDC_CASCADE, m_cascade);
	DDX_Control(pDX, IDC_WINWIDTH, m_winwidth);
	DDX_Control(pDX, IDC_WINHEIGHT, m_winheight);
	DDX_Control(pDX, IDC_MARGINTOP, m_margintop);
	DDX_Control(pDX, IDC_MARGINRIGHT, m_marginright);
	DDX_Control(pDX, IDC_MARGINLEFT, m_marginleft);
	DDX_Control(pDX, IDC_MARGINBOTTOM, m_marginbottom);
	DDX_Control(pDX, IDC_LEGWIDTH, m_legwidth);
	DDX_Control(pDX, IDC_FONTTITLE, m_fonttitle);
	DDX_Control(pDX, IDC_FONTNUMBERS, m_fontnumber);
	DDX_Control(pDX, IDC_FONTLEGEND, m_fontlegend);
	DDX_Control(pDX, IDC_FONTLABEL, m_fontlabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewScreenOpt, CDialog)
	//{{AFX_MSG_MAP(CViewScreenOpt)
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_CASCADE, OnCascade)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CViewScreenOpt::InitFields()
{
	char buffer[1024];

	NumToString(scropt->WinWidth(),buffer,sizeof(buffer));
	m_winwidth.SetWindowText(buffer);
	NumToString(scropt->WinHeight(),buffer,sizeof(buffer));
	m_winheight.SetWindowText(buffer);
	NumToString(scropt->BorderLeft(),buffer,sizeof(buffer));
	m_marginleft.SetWindowText(buffer);
	NumToString(scropt->BorderRight(),buffer,sizeof(buffer));
	m_marginright.SetWindowText(buffer);
	NumToString(scropt->BorderTop(),buffer,sizeof(buffer));
	m_margintop.SetWindowText(buffer);
	NumToString(scropt->BorderBottom(),buffer,sizeof(buffer));
	m_marginbottom.SetWindowText(buffer);
	NumToString(scropt->WidthLegend(),buffer,sizeof(buffer));
	m_legwidth.SetWindowText(buffer);
	NumToString(scropt->FontTitle(),buffer,sizeof(buffer));
	m_fonttitle.SetWindowText(buffer);
	NumToString(scropt->FontLegend(),buffer,sizeof(buffer));
	m_fontlegend.SetWindowText(buffer);
	NumToString(scropt->FontAxes(),buffer,sizeof(buffer));
	m_fontlabel.SetWindowText(buffer);
	NumToString(scropt->FontNumber(),buffer,sizeof(buffer));
	m_fontnumber.SetWindowText(buffer);

	return;
}

/////////////////////////////////////////////////////////////////////////////
// CViewScreenOpt message handlers


BOOL CViewScreenOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	scropt = new PLOTSCROPT(aqsys.PlotScrOpt());

	InitFields();

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewScreenOpt::OnDefaults() 
{
	delete scropt;
	scropt = new PLOTSCROPT;
	InitFields();
	return;
}


void CViewScreenOpt::OnCancel() 
{
	delete scropt;
	
	CDialog::OnCancel();
}


void CViewScreenOpt::OnOK() 
{
	char buffer[1024];
	CARDINAL winwidth, winheight,
		     margleft, margright, margtop, margbottom,
			 legwidth,
			 fonttit, fontleg, fontlab, fontnum;

	m_winwidth.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&winwidth) == FALSE )
	{
		MessageBox(aqapp.ini.T(27), // FailWinWidth
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_winheight.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&winheight) == FALSE )
	{
		MessageBox(aqapp.ini.T(28), // FailWinHeight
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_marginleft.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margleft) == FALSE )
	{
		MessageBox(aqapp.ini.T(29), // FailMargLeft
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_marginright.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margright) == FALSE )
	{
		MessageBox(aqapp.ini.T(30), // FailMargRight
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_margintop.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margtop) == FALSE )
	{
		MessageBox(aqapp.ini.T(31), // FailMargTop
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_marginbottom.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margbottom) == FALSE )
	{
		MessageBox(aqapp.ini.T(32), // FailMargBottom
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_legwidth.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&legwidth) == FALSE )
	{
		MessageBox(aqapp.ini.T(33), // FailLegWidth
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_fonttitle.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fonttit) == FALSE )
	{
		MessageBox(aqapp.ini.T(34), // FailFontTit
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_fontlegend.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fontleg) == FALSE )
	{
		MessageBox(aqapp.ini.T(35), // FailFontLeg
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_fontlabel.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fontlab) == FALSE )
	{
		MessageBox(aqapp.ini.T(36), // FailFontLab
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_fontnumber.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fontnum) == FALSE )
	{
		MessageBox(aqapp.ini.T(37), // FailFontNum
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( scropt->Measures(winwidth,winheight,
				  margleft,margright,margtop,margbottom,
						  legwidth) == FALSE )
	{
		MessageBox(aqapp.ini.T(38), // IllegalLayout
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( scropt->Fonts(fonttit,fontlab,
			       fontnum,fontleg) == FALSE )
	{
		MessageBox(aqapp.ini.T(39), // IllegalFontSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	BOOLEAN cascade = FALSE;
	switch ( aqsys.PlotScrOpt(scropt) )
	{
	case 0:
		if ( m_cascade.GetCheck() == 1 )
		{
			cascade = TRUE;
		}
		RedrawAllPlots(cascade);
		break;
	case 1:
		MessageBox(aqapp.ini.T(40), // InvalWinWidth
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	case 2:
		MessageBox(aqapp.ini.T(41), // InvalWinHeight
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	case 3:
		MessageBox(aqapp.ini.T(42), // InvalWinSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	default:
		aqapp.ProgErr("Unknown return type of AQUASYS::PlotScrOpt"); // InconsMeas
		return;
	}

	CDialog::OnOK();
}


void CViewScreenOpt::OnCascade() 
{
	// TODO: Add your control notification handler code here
	
}


/////////////////////////////////////////////////////////////////////////////
// CViewPSOpt dialog


CViewPSOpt::CViewPSOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CViewPSOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewPSOpt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewPSOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewPSOpt)
	DDX_Control(pDX, IDC_PAPERSIZE, m_papersize);
	DDX_Control(pDX, IDC_RB_PORTRAIT, m_portrait);
	DDX_Control(pDX, IDC_RB_LANDSCAPE, m_landscape);
	DDX_Control(pDX, IDC_PLOTCOLUMNS, m_columns);
	DDX_Control(pDX, IDC_PLOTROWS, m_rows);
	DDX_Control(pDX, IDC_MARGINTOP, m_margintop);
	DDX_Control(pDX, IDC_MARGINRIGHT, m_marginright);
	DDX_Control(pDX, IDC_MARGINLEFT, m_marginleft);
	DDX_Control(pDX, IDC_MARGINBOTTOM, m_marginbottom);
	DDX_Control(pDX, IDC_LEGWIDTH, m_legwidth);
	DDX_Control(pDX, IDC_FONTTITLE, m_fonttitle);
	DDX_Control(pDX, IDC_FONTNUMBERS, m_fontnumber);
	DDX_Control(pDX, IDC_FONTLEGEND, m_fontlegend);
	DDX_Control(pDX, IDC_FONTLABEL, m_fontlabel);
	DDX_Control(pDX, IDC_RB_COLOR, m_color);
	DDX_Control(pDX, IDC_RB_BLACKWHITE, m_blackwhite);
	DDX_Control(pDX, IDC_RB_CM, m_cm);
	DDX_Control(pDX, IDC_RB_INCH, m_inch);
	DDX_Control(pDX, IDC_RB_PTS, m_file_pts);
	DDX_Control(pDX, IDC_RB_EPS, m_file_eps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewPSOpt, CDialog)
	//{{AFX_MSG_MAP(CViewPSOpt)
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_RB_BLACKWHITE, OnRbBlackwhite)
	ON_BN_CLICKED(IDC_RB_CM, OnRbCm)
	ON_BN_CLICKED(IDC_RB_COLOR, OnRbColor)
	ON_BN_CLICKED(IDC_RB_INCH, OnRbInch)
	ON_BN_CLICKED(IDC_RB_LANDSCAPE, OnRbLandscape)
	ON_BN_CLICKED(IDC_RB_PORTRAIT, OnRbPortrait)
	ON_BN_CLICKED(IDC_RB_PTS, OnRbPTS)
	ON_BN_CLICKED(IDC_RB_EPS, OnRbEPS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CViewPSOpt::InitFields()
{
	char buffer[1024];

	m_papersize.SetCurSel((int)filopt->PaperSize());

	if ( filopt->Portrait() == TRUE )
	{
		m_portrait.SetCheck(1);
		m_landscape.SetCheck(0);
	}
	else
	{
		m_landscape.SetCheck(1);
		m_portrait.SetCheck(0);
	}

	NumToString(filopt->NumCol(),buffer,sizeof(buffer));
	m_columns.SetWindowText(buffer);
	NumToString(filopt->NumRow(),buffer,sizeof(buffer));
	m_rows.SetWindowText(buffer);
	NumToString(filopt->BorderLeft(),buffer,sizeof(buffer));
	m_marginleft.SetWindowText(buffer);
	NumToString(filopt->BorderRight(),buffer,sizeof(buffer));
	m_marginright.SetWindowText(buffer);
	NumToString(filopt->BorderTop(),buffer,sizeof(buffer));
	m_margintop.SetWindowText(buffer);
	NumToString(filopt->BorderBottom(),buffer,sizeof(buffer));
	m_marginbottom.SetWindowText(buffer);
	NumToString(filopt->WidthLegend(),buffer,sizeof(buffer));
	m_legwidth.SetWindowText(buffer);
	NumToString(filopt->FontTitle(),buffer,sizeof(buffer));
	m_fonttitle.SetWindowText(buffer);
	NumToString(filopt->FontLegend(),buffer,sizeof(buffer));
	m_fontlegend.SetWindowText(buffer);
	NumToString(filopt->FontAxes(),buffer,sizeof(buffer));
	m_fontlabel.SetWindowText(buffer);
	NumToString(filopt->FontNumber(),buffer,sizeof(buffer));
	m_fontnumber.SetWindowText(buffer);

	if ( filopt->Color() == TRUE )
	{
		m_color.SetCheck(1);
		m_blackwhite.SetCheck(0);
	}
	else
	{
		m_blackwhite.SetCheck(1);
		m_color.SetCheck(0);
	}

	if ( filopt->Cm() == TRUE )
	{
		m_cm.SetCheck(1);
		m_inch.SetCheck(0);
	}
	else
	{
		m_inch.SetCheck(1);
		m_cm.SetCheck(0);
	}

	if ( filopt->Eps() == TRUE ) 				// *JR* jun97 
	{
		m_file_eps.SetCheck(1);
		m_file_pts.SetCheck(0);
	}
	else
	{
		m_file_pts.SetCheck(1);
		m_file_eps.SetCheck(0);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CViewPSOpt message handlers


BOOL CViewPSOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	filopt = new PLOTFILOPT(aqsys.PlotFilOpt());

	m_papersize.ResetContent();
	for ( CARDINAL i=0; i<filopt->NumPaperSizes(); i++ )
	{
		m_papersize.AddString(filopt->PaperSizeName(i));
	}

	InitFields();

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewPSOpt::OnCancel() 
{
	delete filopt;
	
	CDialog::OnCancel();
}


void CViewPSOpt::OnOK() 
{
	char buffer[1024];
	CARDINAL numcols, numrows;
	REAL     margleft, margright, margtop, margbottom,
			 legwidth;
	CARDINAL fonttit, fontleg, fontlab, fontnum;

	m_columns.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&numcols) == FALSE )
	{
		MessageBox(aqapp.ini.T(44), // FailNumCols
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_rows.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&numrows) == FALSE )
	{
		MessageBox(aqapp.ini.T(45), // FailNumRows
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_marginleft.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margleft) == FALSE )
	{
		MessageBox(aqapp.ini.T(46), // FailMargLeft
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_marginright.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margright) == FALSE )
	{
		MessageBox(aqapp.ini.T(47), // FailMargRight
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_margintop.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margtop) == FALSE )
	{
		MessageBox(aqapp.ini.T(48), // FailMargTop
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_marginbottom.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&margbottom) == FALSE )
	{
		MessageBox(aqapp.ini.T(49), // FailMargBottom
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_legwidth.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&legwidth) == FALSE )
	{
		MessageBox(aqapp.ini.T(50), // FailLegWidth
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	m_fonttitle.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fonttit) == FALSE )
	{
		MessageBox(aqapp.ini.T(51), // FailFontTit
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_fontlegend.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fontleg) == FALSE )
	{
		MessageBox(aqapp.ini.T(52), // FailFontLeg
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_fontlabel.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fontlab) == FALSE )
	{
		MessageBox(aqapp.ini.T(53), // FailFontLab
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_fontnumber.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&fontnum) == FALSE )
	{
		MessageBox(aqapp.ini.T(54), // FailFontNum
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	PAPERSIZE size = (PAPERSIZE)m_papersize.GetCurSel();
	if ( filopt->Measures(size,
				  numcols, numrows,
				  margleft,margright,margtop,margbottom,
						  legwidth) == FALSE )
	{
		MessageBox(aqapp.ini.T(55), // IllegalLayout
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( filopt->Fonts(fonttit,fontlab,
			       fontnum,fontleg) == FALSE )
	{
		MessageBox(aqapp.ini.T(56), // IllegalFontSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	switch ( aqsys.PlotFilOpt(filopt) )
	{
	case 0:
		break;
	case 1:
		MessageBox(aqapp.ini.T(57), // InvalWinWidth
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	case 2:
		MessageBox(aqapp.ini.T(58), // InvalWinHeight
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	case 3:
		MessageBox(aqapp.ini.T(59), // InvalWinSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	default:
		aqapp.ProgErr("Unknown return type of AQUASYS::PlotFilOpt"); // InconsMeas
		return;
	}

	CDialog::OnOK();
}


void CViewPSOpt::OnDefaults() 
{
	delete filopt;
	filopt = new PLOTFILOPT;
	InitFields();
	return;
}


void CViewPSOpt::OnRbPortrait() 
{
	filopt->Portrait(TRUE);
	return;
}


void CViewPSOpt::OnRbLandscape() 
{
	filopt->Portrait(FALSE);
	return;
}


void CViewPSOpt::OnRbColor() 
{
	filopt->Color(TRUE);
	return;
}


void CViewPSOpt::OnRbBlackwhite() 
{
	filopt->Color(FALSE);
	return;
}


void CViewPSOpt::OnRbCm() 
{
	if ( filopt->Cm() == FALSE )
	{
		char buffer[1024]; REAL r;
		m_marginleft.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r*filopt->Inch(),buffer,sizeof(buffer));
			m_marginleft.SetWindowText(buffer);
		}
		m_marginright.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r*filopt->Inch(),buffer,sizeof(buffer));
			m_marginright.SetWindowText(buffer);
		}
		m_margintop.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r*filopt->Inch(),buffer,sizeof(buffer));
			m_margintop.SetWindowText(buffer);
		}
		m_marginbottom.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r*filopt->Inch(),buffer,sizeof(buffer));
			m_marginbottom.SetWindowText(buffer);
		}
		m_legwidth.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r*filopt->Inch(),buffer,sizeof(buffer));
			m_legwidth.SetWindowText(buffer);
		}
	}
	filopt->Cm(TRUE);
	return;
}


void CViewPSOpt::OnRbInch() 
{
	if ( filopt->Cm() == TRUE )
	{
		char buffer[1024]; REAL r;
		m_marginleft.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r/filopt->Inch(),buffer,sizeof(buffer));
			m_marginleft.SetWindowText(buffer);
		}
		m_marginright.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r/filopt->Inch(),buffer,sizeof(buffer));
			m_marginright.SetWindowText(buffer);
		}
		m_margintop.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r/filopt->Inch(),buffer,sizeof(buffer));
			m_margintop.SetWindowText(buffer);
		}
		m_marginbottom.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r/filopt->Inch(),buffer,sizeof(buffer));
			m_marginbottom.SetWindowText(buffer);
		}
		m_legwidth.GetWindowText(buffer,sizeof(buffer));
		if ( StringToNum(buffer,&r) == TRUE )
		{
			NumToString(r/filopt->Inch(),buffer,sizeof(buffer));
			m_legwidth.SetWindowText(buffer);
		}
	}
	filopt->Cm(FALSE);
	return;
}


void CViewPSOpt::OnRbPTS()				// *JR* jun97 
{
	// TODO: Add your control notification handler code here
	filopt->Eps(FALSE);
	m_file_pts.SetCheck(1);
}

void CViewPSOpt::OnRbEPS()				// *JR* jun97 
{
	// TODO: Add your control notification handler code here
	filopt->Eps(TRUE);
	m_file_eps.SetCheck(1);
	
}

/////////////////////////////////////////////////////////////////////////////
// CViewListOpt dialog


CViewListOpt::CViewListOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CViewListOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewListOpt)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewListOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewListOpt)
	DDX_Control(pDX, IDC_DIGITS, m_digits);
	DDX_Control(pDX, IDC_RB_TAB, m_septab);
	DDX_Control(pDX, IDC_RB_Comma, m_sepcomma);
	DDX_Control(pDX, IDC_RB_SPACE, m_sepspace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewListOpt, CDialog)
	//{{AFX_MSG_MAP(CViewListOpt)
	ON_BN_CLICKED(IDC_RB_Comma, OnRBComma)
	ON_BN_CLICKED(IDC_RB_SPACE, OnRbSpace)
	ON_BN_CLICKED(IDC_RB_TAB, OnRbTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CViewListOpt::InitFields()
{
	char buffer[1024];

	NumToString(lisopt->Precision(),buffer,sizeof(buffer));
	m_digits.SetWindowText(buffer);

	switch ( lisopt->Separator() )
	{
	case SepTab:
		m_septab.SetCheck(1);
		break;
	case SepComma:
		m_sepcomma.SetCheck(1);
		break;
	case SepSpace:
		m_sepspace.SetCheck(1);
		break;
	default:
		aqapp.ProgErr("Illegal separator");
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CViewListOpt message handlers


BOOL CViewListOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	lisopt = new PLOTLISOPT(aqsys.PlotLisOpt());

	InitFields();

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewListOpt::OnOK() 
{
	char buffer[1024];
	CARDINAL digits;

	m_digits.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&digits) == FALSE )
	{
		MessageBox(aqapp.ini.T(43), // FailDigits
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	lisopt->Precision(digits);

	if ( aqsys.PlotLisOpt(lisopt) == FALSE )
	{
		aqapp.ProgErr("Unable to replace list options"); // Inconsistency
		return;
	}

	CDialog::OnOK();
}


void CViewListOpt::OnCancel() 
{
	delete lisopt;
	
	CDialog::OnCancel();
}


void CViewListOpt::OnRBComma() 
{
	lisopt->Separator(SepComma);
	return;
}


void CViewListOpt::OnRbSpace() 
{
	lisopt->Separator(SepSpace);
	return;
}


void CViewListOpt::OnRbTab() 
{
	lisopt->Separator(SepTab);
	return;
}



/////////////////////////////////////////////////////////////////////////////
// CViewEdPlotDef dialog


CViewEdPlotDef::CViewEdPlotDef(CWnd* pParent /*=NULL*/)
	: CDialog(CViewEdPlotDef::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewEdPlotDef)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewEdPlotDef::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewEdPlotDef)
	DDX_Control(pDX, IDC_TITLE, m_title);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_ABSCISSALABEL, m_abscissalabel);
	DDX_Control(pDX, IDC_ORDINATELABEL, m_ordinatelabel);
	DDX_Control(pDX, IDC_RB_ABSCTIME, m_absctime);
	DDX_Control(pDX, IDC_RB_ABSCSPACE, m_abscspace);
	DDX_Control(pDX, IDC_ADD, m_add);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_DELETE, m_delete);
	DDX_Control(pDX, IDC_CURVELIST, m_curvelist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewEdPlotDef, CDialog)
	//{{AFX_MSG_MAP(CViewEdPlotDef)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_SCALING, OnScaling)
	ON_LBN_DBLCLK(IDC_CURVELIST, OnDblclkCurvelist)
	ON_LBN_SELCHANGE(IDC_CURVELIST, OnSelchangeCurvelist)
	ON_BN_CLICKED(IDC_RB_ABSCSPACE, OnRbAbscspace)
	ON_BN_CLICKED(IDC_RB_ABSCTIME, OnRbAbsctime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CViewEdPlotDef::DoModal(PLOT* p, BOOL a)
{
   oldplot = p;
   add     = a;
   return CDialog::DoModal();
}


void CViewEdPlotDef::UpdateActivation()
{
	switch ( m_curvelist.GetSelCount() )
	{
	case LB_ERR:
		aqapp.ProgErr("Illegal list box selection");
		break;
	case 0:
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(FALSE);
		break;
	case 1:
		m_edit.EnableWindow(TRUE);
		m_delete.EnableWindow(TRUE);
		break;
	default:
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(TRUE);
	   break;
	}
	return;
}


void CViewEdPlotDef::UpdateCurvelist()
{
	int topindex = m_curvelist.GetTopIndex();
	m_curvelist.ResetContent();
	for ( CARDINAL i=0; i<newplot->NumCurves(); i++ )
	{
		const CURVE* curve = newplot->Curve(i);
		char buffer[1024], buffer1[1024];
		switch ( curve->Type() )
		{
		case CurveVal:
			strncpy(buffer,aqapp.ini.T(62),sizeof(buffer)-1);
			break;
		case CurveSensErr:
			strncpy(buffer,aqapp.ini.T(63),sizeof(buffer)-1);
			break;
		case CurveSensAbsAbs:
			strncpy(buffer,aqapp.ini.T(64),sizeof(buffer)-1);
			break;
		case CurveSensRelAbs:
			strncpy(buffer,aqapp.ini.T(65),sizeof(buffer)-1);
			break;
		case CurveSensAbsRel:
			strncpy(buffer,aqapp.ini.T(66),sizeof(buffer)-1);
			break;
		case CurveSensRelRel:
			strncpy(buffer,aqapp.ini.T(67),sizeof(buffer)-1);
			break;
		default:
			aqapp.ProgErr("Unknown curve type");
			break;
		}
		strncat(buffer," : ",sizeof(buffer)-strlen(buffer)-1);
		if ( curve->Variable() != 0 )
		{
			strncat(buffer,curve->Variable()->Symbol(),
				sizeof(buffer)-strlen(buffer)-1);
		}
		if ( curve->Type() != CurveVal )
		{
			if ( curve->Parameter() != 0 )
			{
				strncat(buffer,"(",sizeof(buffer)-strlen(buffer)-1);
				strncat(buffer,curve->Parameter()->Symbol(),
					    sizeof(buffer)-strlen(buffer)-1);
				strncat(buffer,")",sizeof(buffer)-strlen(buffer)-1);
			}
		}
		strncat(buffer," [",sizeof(buffer)-strlen(buffer)-1);
		NumToString(curve->CalcNum(),buffer1,sizeof(buffer1)-1);
		strncat(buffer,buffer1,sizeof(buffer)-strlen(buffer)-1);
		strncat(buffer,",",sizeof(buffer)-strlen(buffer)-1);
		if ( curve->Compartment() != 0 )
		{
			strncat(buffer,curve->Compartment()->Symbol(),
			       sizeof(buffer)-strlen(buffer)-1);
			strncat(buffer,",",sizeof(buffer)-strlen(buffer)-1);
			strncat(buffer,curve->Compartment()->ZoneName(curve->Zone()),
			       sizeof(buffer)-strlen(buffer)-1);
		}
		strncat(buffer,",",sizeof(buffer)-strlen(buffer)-1);
		NumToString(curve->XorT(),buffer1,sizeof(buffer1)-1);
		strncat(buffer,buffer1,sizeof(buffer)-strlen(buffer)-1);
        if ( curve->SpaceRel() == TRUE )
		{
			strcat(buffer," rel.space");
		}
		strncat(buffer,"]",sizeof(buffer)-strlen(buffer)-1);
		m_curvelist.InsertString(-1,buffer);
	}
	m_curvelist.SetTopIndex(topindex);

	UpdateActivation();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CViewEdPlotDef message handlers

BOOL CViewEdPlotDef::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	newplot = new PLOT(oldplot);

	if ( newplot->Symbol() != 0 )
	{
		m_name.SetWindowText(newplot->Symbol());
	}
	if ( newplot->Description() != 0 )
	{
		m_description.SetWindowText(newplot->Description());
	}
	if ( newplot->Title() != 0 )
	{
		m_title.SetWindowText(newplot->Title());
	}
	if ( newplot->xTitle() != 0 )
	{
		m_abscissalabel.SetWindowText(newplot->xTitle());
	}
	if ( newplot->yTitle() != 0 )
	{
		m_ordinatelabel.SetWindowText(newplot->yTitle());
	}
	if  ( newplot->AbscType() == AbscTime )
	{
		m_absctime.SetCheck(1);
	}
	else
	{
		m_abscspace.SetCheck(1);
	}

	UpdateCurvelist();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewEdPlotDef::OnAdd() 
{
	int max = newplot->NumCurves();
	int* indices = new int[max];
	int num = m_curvelist.GetSelItems(max,indices);
	int pos = max;
	if ( num >= 1 )
	{
		pos = indices[0];
	}
	delete [] indices;
	CViewEdCurveDef EdCurveDefDlg;
	EdCurveDefDlg.DoModal(newplot,TRUE,pos);

	UpdateCurvelist();
	return;
}


void CViewEdPlotDef::OnEdit() 
{
	int max = newplot->NumCurves();
	int* indices = new int[max];
	int num = m_curvelist.GetSelItems(max,indices);
	if ( num != 1 )
	{
		aqapp.ProgErr("Illegal selection");
		return;
	}
	int pos = indices[0];
	delete [] indices;
	CViewEdCurveDef EdCurveDefDlg;
	EdCurveDefDlg.DoModal(newplot,FALSE,pos);

	UpdateCurvelist();
	return;
}


void CViewEdPlotDef::OnDelete() 
{
	switch ( MessageBox(aqapp.ini.T(69),              // DelCurves?
				aqapp.ini.T(111),
						MB_YESNO+MB_DEFBUTTON2+MB_ICONQUESTION) )
	{
	case IDYES:
		break;
	default:
		return;
	}
	int max = newplot->NumCurves();
	int* indices = new int[max];
	int num = m_curvelist.GetSelItems(max,indices);
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
			if ( newplot->DeleteCurve(indices[j]-k) == TRUE )
			{
				k++;
			}
			else
			{
				aqapp.ProgErr("Illegal curve index");
			}
		}
		break;
	}
	delete [] indices;

	UpdateCurvelist();
	return; 
}


void CViewEdPlotDef::OnScaling() 
{
	PLOT* plot = new PLOT(newplot);
	CViewEdScaling EdScalingDlg;
	if ( EdScalingDlg.DoModal(plot) == IDOK )
	{
//		PLOT* oldplot=newplot;								  		  // *JR* sep96
		delete newplot;
		newplot = plot;
//		aqapp.MainFrame->plotmanager.ReplacePlotPtr(oldplot,newplot); // *JR* sep96
	}
	else
	{
		delete plot;
	}
	return;
}


void CViewEdPlotDef::OnDblclkCurvelist() 
{
	OnEdit();
	return;
}


void CViewEdPlotDef::OnSelchangeCurvelist() 
{
	UpdateActivation();
	return;
}


void CViewEdPlotDef::OnOK() 
{
	char buffer[1024];
	m_name.GetWindowText(buffer,sizeof(buffer));
	if ( newplot->Symbol(buffer) == FALSE )
	{
		MessageBox(aqapp.ini.T(68),
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_description.GetWindowText(buffer,sizeof(buffer));
	newplot->Description(buffer);
	m_title.GetWindowText(buffer,sizeof(buffer));
	newplot->Title(buffer);
	m_abscissalabel.GetWindowText(buffer,sizeof(buffer));
	newplot->xTitle(buffer);
	m_ordinatelabel.GetWindowText(buffer,sizeof(buffer));
	newplot->yTitle(buffer);

	if ( add == TRUE )
	{
		if ( aqsys.AddPlot(newplot) == FALSE )
		{
			MessageBox(aqapp.ini.T(60),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( aqsys.ReplacePlot(oldplot,newplot) == FALSE )
		{
			MessageBox(aqapp.ini.T(61),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
//		else   // *JR* aug96 
//			aqapp.MainFrame->plotmanager.ReplacePlotPtr(oldplot,newplot);
	}
	
	CDialog::OnOK();
} // end CViewEdPlotDef::OnOK()								// *JR* aug9


void CViewEdPlotDef::OnCancel() 
{
	delete newplot;

	CDialog::OnCancel();
}


void CViewEdPlotDef::OnRbAbscspace() 
{
	newplot->AbscType(AbscSpace);
	return;
}


void CViewEdPlotDef::OnRbAbsctime() 
{
	newplot->AbscType(AbscTime);
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CViewEdCurveDef dialog


CViewEdCurveDef::CViewEdCurveDef(CWnd* pParent /*=NULL*/)
	: CDialog(CViewEdCurveDef::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewEdCurveDef)
	//}}AFX_DATA_INIT
}


void CViewEdCurveDef::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewEdCurveDef)
	DDX_Control(pDX, IDC_CB_SPACEREL, m_cb_spacerel);
	DDX_Control(pDX, IDC_TX_ZONE, m_tx_zone);
	DDX_Control(pDX, IDC_TX_COMP, m_tx_comp);
	DDX_Control(pDX, IDC_TX_PAR, m_tx_par);
	DDX_Control(pDX, IDC_ZONE, m_zone);
	DDX_Control(pDX, IDC_VAR, m_var);
	DDX_Control(pDX, IDC_TIMESPACE, m_timespace);
	DDX_Control(pDX, IDC_PAR, m_par);
	DDX_Control(pDX, IDC_MARKERSTYLE, m_markerstyle);
	DDX_Control(pDX, IDC_MARKERSIZE, m_markersize);
	DDX_Control(pDX, IDC_MARKERCOLOR, m_markercolor);
	DDX_Control(pDX, IDC_MARKERACT, m_markeract);
	DDX_Control(pDX, IDC_LINEWIDTH, m_linewidth);
	DDX_Control(pDX, IDC_LINESTYLE, m_linestyle);
	DDX_Control(pDX, IDC_LINECOLOR, m_linecolor);
	DDX_Control(pDX, IDC_LINEACT, m_lineact);
	DDX_Control(pDX, IDC_COMP, m_comp);
	DDX_Control(pDX, IDC_CALCNUM, m_calcnum);
	DDX_Control(pDX, IDC_RB_VALUE, m_typevalue);
	DDX_Control(pDX, IDC_RB_ERROR, m_typeerror);
	DDX_Control(pDX, IDC_RB_SENS, m_typesens);
	DDX_Control(pDX, IDC_RB_ABSABS, m_sensabsabs);
	DDX_Control(pDX, IDC_RB_RELABS, m_sensrelabs);
	DDX_Control(pDX, IDC_RB_ABSREL, m_sensabsrel);
	DDX_Control(pDX, IDC_RB_RELREL, m_sensrelrel);
	DDX_Control(pDX, IDC_LEGEND, m_legend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewEdCurveDef, CDialog)
	//{{AFX_MSG_MAP(CViewEdCurveDef)
	ON_BN_CLICKED(IDC_RB_ABSABS, OnRbAbsabs)
	ON_BN_CLICKED(IDC_RB_ABSREL, OnRbAbsrel)
	ON_BN_CLICKED(IDC_RB_ERROR, OnRbError)
	ON_BN_CLICKED(IDC_RB_RELABS, OnRbRelabs)
	ON_BN_CLICKED(IDC_RB_RELREL, OnRbRelrel)
	ON_BN_CLICKED(IDC_RB_SENS, OnRbSens)
	ON_BN_CLICKED(IDC_RB_VALUE, OnRbValue)
	ON_CBN_CLOSEUP(IDC_COMP, OnCloseupComp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CViewEdCurveDef::DoModal(PLOT* p, BOOL a, int i)
{
   plot     = p;
   add      = a;
   pos      = i;
   return CDialog::DoModal();
}


void CViewEdCurveDef::UpdateZoneList()
{
	int i = m_comp.GetCurSel();
	if ( i == CB_ERR )
	{
		m_zone.ResetContent();
		m_zone.EnableWindow(FALSE);
		m_tx_zone.EnableWindow(FALSE);
	}
	else
	{
		AQCOMP* com = aqsys.Complist()->Get(i+1);
		if ( com == 0 )
		{
			m_zone.ResetContent();
			m_zone.EnableWindow(FALSE);
			m_tx_zone.EnableWindow(FALSE);
		}
		else
		{
			m_zone.ResetContent();
			for ( CARDINAL j=0; j<com->NumZones(); j++ )
			{
				m_zone.AddString(com->ZoneName(j));
			}
			if ( curve->Zone() >= com->NumZones() ) curve->Zone(0);
			m_zone.SetCurSel(curve->Zone());
			if ( com->NumZones() > 1 )
			{
				m_zone.EnableWindow(TRUE);
				m_tx_zone.EnableWindow(TRUE);
			}
			else
			{
				m_zone.EnableWindow(FALSE);
				m_tx_zone.EnableWindow(FALSE);
			}
		}
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CViewEdCurveDef message handlers

BOOL CViewEdCurveDef::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char buffer[1024];

	if ( add == TRUE )
	{
		curve = new CURVE;
	}
	else
	{
		const CURVE* c = plot->Curve(pos);
		if ( c == 0 )
		{
			curve = new CURVE;
			aqapp.ProgErr("Curve not found");
		}
		else
		{
			curve = new CURVE(c);
		}
	}

	sensmeth = 3;
	switch ( curve->Type() )
	{
	case CurveVal:
		m_typevalue.SetCheck(1);
		m_sensabsabs.EnableWindow(FALSE);
		m_sensrelabs.EnableWindow(FALSE);
		m_sensabsrel.EnableWindow(FALSE);
		m_sensrelrel.EnableWindow(FALSE);
		m_par.EnableWindow(FALSE);
		m_tx_par.EnableWindow(FALSE);
		break;
	case CurveSensErr:
		m_typeerror.SetCheck(1);
		m_sensabsabs.EnableWindow(FALSE);
		m_sensrelabs.EnableWindow(FALSE);
		m_sensabsrel.EnableWindow(FALSE);
		m_sensrelrel.EnableWindow(FALSE);
		break;
	case CurveSensAbsAbs:
		sensmeth = 1;
		m_typesens.SetCheck(1);
		m_sensabsabs.SetCheck(1);
		break;
	case CurveSensRelAbs:
		sensmeth = 2;
		m_typesens.SetCheck(1);
		m_sensrelabs.SetCheck(1);
		break;
	case CurveSensAbsRel:
		sensmeth = 3;
		m_typesens.SetCheck(1);
		m_sensabsrel.SetCheck(1);
		break;
	case CurveSensRelRel:
		sensmeth = 4;
		m_typesens.SetCheck(1);
		m_sensrelrel.SetCheck(1);
		break;
	}

	m_var.ResetContent();
	{
		AQVAR* var = aqsys.Varlist()->First();
		while ( var != 0 )
		{
			m_var.AddString(var->Symbol());
			var = var->Next();
		}
	}
	if ( curve->Variable() != 0 )
	{
		int pos = aqsys.Varlist()->Position(curve->Variable());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Variable not found");
		}
		else
		{
			m_var.SetCurSel(pos-1);
		}
	}
	else
	{
		m_var.SetCurSel(0);
	}

	m_par.ResetContent();
	{
		AQVAR* par = aqsys.Varlist()->First();
		while ( par != 0 )
		{
			m_par.AddString(par->Symbol());
			par = par->Next();
		}
	}
	if ( curve->Parameter() != 0 )
	{
		int pos = aqsys.Varlist()->Position(curve->Parameter());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Parameter not found");
		}
		else
		{
			m_par.SetCurSel(pos-1);
		}
	}

	NumToString(curve->CalcNum(),buffer,sizeof(buffer));
	m_calcnum.SetWindowText(buffer);

	m_comp.ResetContent();
	{
		AQCOMP* com = aqsys.Complist()->First();
		while ( com != 0 )
		{
			m_comp.AddString(com->Symbol());
			com = com->Next();
		}
	}
	if ( curve->Compartment() != 0 )
	{
		int pos = aqsys.Complist()->Position(curve->Compartment());
		if ( pos <= 0 )
		{
			aqapp.ProgErr("Compartment not found");
		}
		else
		{
			m_comp.SetCurSel(pos-1);
		}
	}
	else
	{
		m_comp.SetCurSel(0);
	}
	if ( aqsys.Complist()->Size() == 1 )
	{
		m_comp.EnableWindow(FALSE);
		m_tx_comp.EnableWindow(FALSE);
	}

	UpdateZoneList();

    if ( curve->SpaceRel() == TRUE ) m_cb_spacerel.SetCheck(1);
	else                             m_cb_spacerel.SetCheck(0);

	NumToString(curve->XorT(),buffer,sizeof(buffer));
	m_timespace.SetWindowText(buffer);

	if ( curve->Legend() != 0 )
	{
		m_legend.SetWindowText(curve->Legend());
	}

	if ( curve->LineActive() == TRUE )
	{
		m_lineact.SetCheck(1);
	}

	m_linestyle.ResetContent();
	m_linestyle.AddString(aqapp.ini.T(78));   // LineStyle0
	m_linestyle.AddString(aqapp.ini.T(79));   // LineStyle1
	m_linestyle.AddString(aqapp.ini.T(80));   // LineStyle2
	m_linestyle.AddString(aqapp.ini.T(81));   // LineStyle3
	m_linestyle.AddString(aqapp.ini.T(82));   // LineStyle4
	m_linestyle.AddString(aqapp.ini.T(83));   // LineStyle5
	m_linestyle.AddString(aqapp.ini.T(84));   // LineStyle6
	m_linestyle.SetCurSel(curve->LineStyle());

	NumToString(curve->LineWidth(),buffer,sizeof(buffer));
	m_linewidth.SetWindowText(buffer);

	m_linecolor.ResetContent();
	m_linecolor.AddString(aqapp.ini.T(86));   // Color0
	m_linecolor.AddString(aqapp.ini.T(87));   // Color1
	m_linecolor.AddString(aqapp.ini.T(88));   // Color2
	m_linecolor.AddString(aqapp.ini.T(89));   // Color3
	m_linecolor.AddString(aqapp.ini.T(90));   // Color4
	m_linecolor.AddString(aqapp.ini.T(91));   // Color5
	m_linecolor.AddString(aqapp.ini.T(92));   // Color6
	m_linecolor.AddString(aqapp.ini.T(93));   // Color7
	m_linecolor.SetCurSel(curve->LineColor());

	if ( curve->MarkActive() == TRUE )
	{
		m_markeract.SetCheck(1);
	}

	m_markerstyle.ResetContent();
	m_markerstyle.AddString(aqapp.ini.T(94)); // MarkerStyle0
	m_markerstyle.AddString(aqapp.ini.T(95)); // MarkerStyle1
	m_markerstyle.AddString(aqapp.ini.T(96)); // MarkerStyle2
	m_markerstyle.AddString(aqapp.ini.T(97)); // MarkerStyle3
	m_markerstyle.SetCurSel(curve->MarkStyle());

	NumToString(curve->MarkSize(),buffer,sizeof(buffer));
	m_markersize.SetWindowText(buffer);

	m_markercolor.ResetContent();
	m_markercolor.AddString(aqapp.ini.T(86)); // Color0
	m_markercolor.AddString(aqapp.ini.T(87)); // Color1
	m_markercolor.AddString(aqapp.ini.T(88)); // Color2
	m_markercolor.AddString(aqapp.ini.T(89)); // Color3
	m_markercolor.AddString(aqapp.ini.T(90)); // Color4
	m_markercolor.AddString(aqapp.ini.T(91)); // Color5
	m_markercolor.AddString(aqapp.ini.T(92)); // Color6
	m_markercolor.AddString(aqapp.ini.T(93)); // Color7
	m_markercolor.SetCurSel(curve->MarkColor());

	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewEdCurveDef::OnCancel() 
{
	delete curve;

	CDialog::OnCancel();
}

void CViewEdCurveDef::OnOK() 
{
	char buffer[1024];
	int  i;

	i = m_var.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(72), // IllegalVar
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
		curve->Variable(var);
	}

	i = m_par.GetCurSel();
	if ( i == CB_ERR )
	{
		if ( curve->Type() != CurveVal )
		{
			MessageBox(aqapp.ini.T(73), // IllegalPar
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		AQVAR* par = aqsys.Varlist()->Get(i+1);
		if ( par == 0 )
		{
			aqapp.ProgErr("Parameter not found");
			return;
		}
		curve->Parameter(par);
	}

	CARDINAL calcnum;
	m_calcnum.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&calcnum) == FALSE )
	{
		MessageBox(aqapp.ini.T(74), // IllegalCalcnum
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		curve->CalcNum(calcnum);
	}

	i = m_comp.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(75), // IllegalComp
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		AQCOMP* com = aqsys.Complist()->Get(i+1);
		if ( com == 0 )
		{
			aqapp.ProgErr("Compartment not found");
			return;
		}
		curve->Compartment(com);
	}

	i = m_zone.GetCurSel();
	if ( i == CB_ERR )
	{
		MessageBox(aqapp.ini.T(76), // IllegalZone
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		curve->Zone(i);
	}

	if ( m_cb_spacerel.GetCheck() ) curve->SpaceRel(TRUE);
	else                            curve->SpaceRel(FALSE);

	REAL timespace;
	m_timespace.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&timespace) == FALSE )
	{
		MessageBox(aqapp.ini.T(77), // IllegalXorT
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		curve->XorT(timespace);
	}

	m_legend.GetWindowText(buffer,sizeof(buffer));
	curve->Legend(buffer);

	if ( m_lineact.GetCheck() )
	{
		curve->LineActive(TRUE);
	}
	else
	{
		curve->LineActive(FALSE);
	}

	curve->LineStyle((LINESTYLE)m_linestyle.GetCurSel());

	CARDINAL linewidth;
	m_linewidth.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&linewidth) == FALSE )
	{
		MessageBox(aqapp.ini.T(85), // IllegalLineWidth
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		curve->LineWidth(linewidth);
	}

	curve->LineColor((AQUACOLOR)m_linecolor.GetCurSel());

	if ( m_markeract.GetCheck() )
	{
		curve->MarkActive(TRUE);
	}
	else
	{
		curve->MarkActive(FALSE);
	}

	curve->MarkStyle((MARKSTYLE)m_markerstyle.GetCurSel());

	CARDINAL markersize;
	m_markersize.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&markersize) == FALSE )
	{
		MessageBox(aqapp.ini.T(98), // IllegalMarkerSize
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	else
	{
		curve->MarkSize(markersize);
	}

	curve->MarkColor((AQUACOLOR)m_markercolor.GetCurSel());

	if ( add == TRUE )
	{
		if ( plot->AddCurve(curve,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(70),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}
	else
	{
		if ( plot->ReplaceCurve(curve,pos) == FALSE )
		{
			MessageBox(aqapp.ini.T(71),
				   aqapp.ini.T(111),
				       MB_OK+MB_ICONERROR);
			return;
		}
	}


	CDialog::OnOK();
}	// end of CViewEdCurveDef::OnOK()		 // *JR* aug96


void CViewEdCurveDef::OnRbValue() 
{
	curve->Type(CurveVal);
	m_sensabsabs.EnableWindow(FALSE);
	m_sensrelabs.EnableWindow(FALSE);
	m_sensabsrel.EnableWindow(FALSE);
	m_sensrelrel.EnableWindow(FALSE);
	m_par.EnableWindow(FALSE);
	m_tx_par.EnableWindow(FALSE);
	return;
}


void CViewEdCurveDef::OnRbError() 
{
	curve->Type(CurveSensErr);
	m_sensabsabs.EnableWindow(FALSE);
	m_sensrelabs.EnableWindow(FALSE);
	m_sensabsrel.EnableWindow(FALSE);
	m_sensrelrel.EnableWindow(FALSE);
	m_par.EnableWindow(TRUE);
	m_tx_par.EnableWindow(TRUE);
	return;
}


void CViewEdCurveDef::OnRbSens() 
{
	m_sensabsabs.EnableWindow(TRUE);
	m_sensrelabs.EnableWindow(TRUE);
	m_sensabsrel.EnableWindow(TRUE);
	m_sensrelrel.EnableWindow(TRUE);
	m_par.EnableWindow(TRUE);
	m_tx_par.EnableWindow(TRUE);
	switch ( sensmeth )
	{
	case 1:
		curve->Type(CurveSensAbsAbs);
		m_sensabsabs.SetCheck(1);
		break;
	case 2:
		curve->Type(CurveSensRelAbs);
		m_sensrelabs.SetCheck(1);
		break;
	case 4:
		curve->Type(CurveSensRelRel);
		m_sensrelrel.SetCheck(1);
		break;
	default:
		curve->Type(CurveSensAbsRel);
		m_sensabsrel.SetCheck(1);
		break;
	}
	return;
}


void CViewEdCurveDef::OnRbAbsabs() 
{
	sensmeth = 1;
	curve->Type(CurveSensAbsAbs);
	return;
}


void CViewEdCurveDef::OnRbRelabs() 
{
	sensmeth = 2;
	curve->Type(CurveSensRelAbs);
	return;
}


void CViewEdCurveDef::OnRbAbsrel() 
{
	sensmeth = 3;
	curve->Type(CurveSensAbsRel);
	return;
}


void CViewEdCurveDef::OnRbRelrel() 
{
	sensmeth = 4;
	curve->Type(CurveSensRelRel);
	return;
}


void CViewEdCurveDef::OnCloseupComp() 
{
	UpdateZoneList();
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CViewEdScaling dialog


CViewEdScaling::CViewEdScaling(CWnd* pParent /*=NULL*/)
	: CDialog(CViewEdScaling::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewEdScaling)
	//}}AFX_DATA_INIT
}


void CViewEdScaling::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewEdScaling)
	DDX_Control(pDX, IDC_CB_ABSMAX, m_cb_abscmax);
	DDX_Control(pDX, IDC_CB_ABSCTDIST, m_cb_absctdist);
	DDX_Control(pDX, IDC_CB_ABSCMIN, m_cb_abscmin);
	DDX_Control(pDX, IDC_ED_ABSCTDIST, m_ed_absctdist);
	DDX_Control(pDX, IDC_ED_ORDTPOS, m_ed_ordtpos);
	DDX_Control(pDX, IDC_ED_ORDTDIST, m_ed_ordtdist);
	DDX_Control(pDX, IDC_ED_ORDMIN, m_ed_ordmin);
	DDX_Control(pDX, IDC_ED_ORDMAX, m_ed_ordmax);
	DDX_Control(pDX, IDC_ED_ABSCTPOS, m_ed_absctpos);
	DDX_Control(pDX, IDC_ED_ABSCMIN, m_ed_abscmin);
	DDX_Control(pDX, IDC_ED_ABSCMAX, m_ed_abscmax);
	DDX_Control(pDX, IDC_CB_ORDTPOS, m_cb_ordtpos);
	DDX_Control(pDX, IDC_CB_ORDTDIST, m_cb_ordtdist);
	DDX_Control(pDX, IDC_CB_ORDMIN, m_cb_ordmin);
	DDX_Control(pDX, IDC_CB_ORDMAX, m_cb_ordmax);
	DDX_Control(pDX, IDC_CB_ABSCTPOS, m_cb_absctpos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewEdScaling, CDialog)
	//{{AFX_MSG_MAP(CViewEdScaling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CViewEdScaling::DoModal(PLOT* p)
{
   plot     = p;
   return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CViewEdScaling message handlers

BOOL CViewEdScaling::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char buffer[1024];

	if ( plot->xMinAuto() ) m_cb_abscmin.SetCheck(1);
	NumToString(plot->xMin(),buffer,sizeof(buffer));
	m_ed_abscmin.SetWindowText(buffer);
	
	if ( plot->xMaxAuto() ) m_cb_abscmax.SetCheck(1);
	NumToString(plot->xMax(),buffer,sizeof(buffer));
	m_ed_abscmax.SetWindowText(buffer);
	
	if ( plot->xTickPosAuto() ) m_cb_absctpos.SetCheck(1);
	NumToString(plot->xTickPos(),buffer,sizeof(buffer));
	m_ed_absctpos.SetWindowText(buffer);
	
	if ( plot->xTickDistAuto() ) m_cb_absctdist.SetCheck(1);
	NumToString(plot->xTickDist(),buffer,sizeof(buffer));
	m_ed_absctdist.SetWindowText(buffer);
	
	if ( plot->yMinAuto() ) m_cb_ordmin.SetCheck(1);
	NumToString(plot->yMin(),buffer,sizeof(buffer));
	m_ed_ordmin.SetWindowText(buffer);
	
	if ( plot->yMaxAuto() ) m_cb_ordmax.SetCheck(1);
	NumToString(plot->yMax(),buffer,sizeof(buffer));
	m_ed_ordmax.SetWindowText(buffer);
	
	if ( plot->yTickPosAuto() ) m_cb_ordtpos.SetCheck(1);
	NumToString(plot->yTickPos(),buffer,sizeof(buffer));
	m_ed_ordtpos.SetWindowText(buffer);
	
	if ( plot->yTickDistAuto() ) m_cb_ordtdist.SetCheck(1);
	NumToString(plot->yTickDist(),buffer,sizeof(buffer));
	m_ed_ordtdist.SetWindowText(buffer);
		
	return TRUE;  // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewEdScaling::OnOK() 
{
	char buffer[1024]; REAL r1,r2;

	if ( m_cb_abscmin.GetCheck() ) plot->xMinAuto(TRUE);
	else                           plot->xMinAuto(FALSE);
	if ( m_cb_abscmax.GetCheck() ) plot->xMaxAuto(TRUE);
	else                           plot->xMaxAuto(FALSE);
	m_ed_abscmin.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(99), // FailAbscMin
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_abscmax.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(100), // FailAbscMax
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( plot->xMinMax(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(101), // IllAbscMinMax
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_absctpos.GetCheck() )  plot->xTickPosAuto(TRUE);
	else                             plot->xTickPosAuto(FALSE);
	if ( m_cb_absctdist.GetCheck() ) plot->xTickDistAuto(TRUE);
	else                             plot->xTickDistAuto(FALSE);
	m_ed_absctpos.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(102), // FailAbscTPos
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_absctdist.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(103), // FailAbscTDist
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	plot->xTick(r1,r2);
	
	if ( m_cb_ordmin.GetCheck() ) plot->yMinAuto(TRUE);
	else                          plot->yMinAuto(FALSE);
	if ( m_cb_ordmax.GetCheck() ) plot->yMaxAuto(TRUE);
	else                          plot->yMaxAuto(FALSE);
	m_ed_ordmin.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(104), // FailOrdMin
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_ordmax.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(105), // FailOrdMax
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	if ( plot->yMinMax(r1,r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(106), // IllOrdMinMax
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}

	if ( m_cb_ordtpos.GetCheck() )  plot->yTickPosAuto(TRUE);
	else                            plot->yTickPosAuto(FALSE);
	if ( m_cb_ordtdist.GetCheck() ) plot->yTickDistAuto(TRUE);
	else                            plot->yTickDistAuto(FALSE);
	m_ed_ordtpos.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r1) == FALSE )
	{
		MessageBox(aqapp.ini.T(107), // FailOrdTPos
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	m_ed_ordtdist.GetWindowText(buffer,sizeof(buffer));
	if ( StringToNum(buffer,&r2) == FALSE )
	{
		MessageBox(aqapp.ini.T(108), // FailOrdTDist
			       aqapp.ini.T(111),
				   MB_OK+MB_ICONERROR);
		return;
	}
	plot->yTick(r1,r2);
	
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CViewEdScalings dialog


CViewEdScalings::CViewEdScalings(CWnd* pParent /*=NULL*/)
	: CDialog(CViewEdScalings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewEdScalings)
	//}}AFX_DATA_INIT
}


void CViewEdScalings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewEdScalings)
	DDX_Control(pDX, IDC_CB_ORDTPOS, m_cb_ordtpos);
	DDX_Control(pDX, IDC_CB_ORDTDIST, m_cb_ordtdist);
	DDX_Control(pDX, IDC_CB_ORDMIN, m_cb_ordmin);
	DDX_Control(pDX, IDC_CB_ORDMAX, m_cb_ordmax);
	DDX_Control(pDX, IDC_CB_ABSMAX, m_cb_absmax);
	DDX_Control(pDX, IDC_CB_ABSCTPOS, m_cb_absctpos);
	DDX_Control(pDX, IDC_CB_ABSCTDIST, m_cb_absctdist);
	DDX_Control(pDX, IDC_CB_ABSCMIN, m_cb_abscmin);
	DDX_Control(pDX, IDC_ED_ABSCMAX, m_ed_abscmax);
	DDX_Control(pDX, IDC_ED_ORDTPOS, m_ed_ordtpos);
	DDX_Control(pDX, IDC_ED_ORDTDIST, m_ed_ordtdist);
	DDX_Control(pDX, IDC_ED_ORDMIN, m_ed_ordmin);
	DDX_Control(pDX, IDC_ED_ORDMAX, m_ed_ordmax);
	DDX_Control(pDX, IDC_ED_ABSCTPOS, m_ed_absctpos);
	DDX_Control(pDX, IDC_ED_ABSCTDIST, m_ed_absctdist);
	DDX_Control(pDX, IDC_ED_ABSCMIN, m_ed_abscmin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewEdScalings, CDialog)
	//{{AFX_MSG_MAP(CViewEdScalings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CViewEdScalings::DoModal(int n, int* ind)
{
   num = n;
   indices = ind;
   return CDialog::DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CViewEdScalings message handlers

void CViewEdScalings::OnCancel() 
{
	CDialog::OnCancel();
}


void CViewEdScalings::OnOK() 
{
	char buffer[1024]; REAL r1,r2;
	PLOTLIST* plotlist = aqsys.Plotlist();
	for ( CARDINAL i=0; i<num; i++ )
	{
		PLOT* plot = plotlist->Get(indices[i]+1);
		if ( plot != 0 )
		{
			r1 = plot->xMin();
			r2 = plot->xMax();
			if ( m_cb_abscmin.GetCheck() )
			{
				plot->xMinAuto(TRUE);
			}
			else
			{
				m_ed_abscmin.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r1) == TRUE )
				{
					plot->xMinAuto(FALSE);
				}
				else
				{
					r1 = plot->xMin();
				}
			}
			if ( m_cb_absmax.GetCheck() )
			{
				plot->xMaxAuto(TRUE);
			}
			else
			{
				m_ed_abscmax.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r2) == TRUE )
				{
					plot->xMaxAuto(FALSE);
				}
				else
				{
					r2 = plot->xMax();
				}
			}
			plot->xMinMax(r1,r2);

			r1 = plot->xTickPos();
			r2 = plot->xTickDist();
			if ( m_cb_absctpos.GetCheck() )
			{
				plot->xTickPosAuto(TRUE);
			}
			else
			{
				m_ed_absctpos.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r1) == TRUE )
				{
					plot->xTickPosAuto(FALSE);
				}
				else
				{
					r1 = plot->xTickPos();
				}
			}
			if ( m_cb_absctdist.GetCheck() )
			{
				plot->xTickDistAuto(TRUE);
			}
			else
			{
				m_ed_absctdist.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r2) == TRUE )
				{
					plot->xTickDistAuto(FALSE);
				}
				else
				{
					r2 = plot->xTickDist();
				}
			}
			plot->xTick(r1,r2);

			r1 = plot->yMin();
			r2 = plot->yMax();
			if ( m_cb_ordmin.GetCheck() )
			{
				plot->yMinAuto(TRUE);
			}
			else
			{
				m_ed_ordmin.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r1) == TRUE )
				{
					plot->yMinAuto(FALSE);
				}
				else
				{
					r1 = plot->yMin();
				}
			}
			if ( m_cb_ordmax.GetCheck() )
			{
				plot->yMaxAuto(TRUE);
			}
			else
			{
				m_ed_ordmax.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r2) == TRUE )
				{
					plot->yMaxAuto(FALSE);
				}
				else
				{
					r2 = plot->yMax();
				}
			}
			plot->yMinMax(r1,r2);

			r1 = plot->yTickPos();
			r2 = plot->yTickDist();
			if ( m_cb_ordtpos.GetCheck() )
			{
				plot->yTickPosAuto(TRUE);
			}
			else
			{
				m_ed_ordtpos.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r1) == TRUE )
				{
					plot->yTickPosAuto(FALSE);
				}
				else
				{
					r1 = plot->yTickPos();
				}
			}
			if ( m_cb_ordtdist.GetCheck() )
			{
				plot->yTickDistAuto(TRUE);
			}
			else
			{
				m_ed_ordtdist.GetWindowText(buffer,sizeof(buffer));
				if ( StringToNum(buffer,&r2) == TRUE )
				{
					plot->yTickDistAuto(FALSE);
				}
				else
				{
					r2 = plot->yTickDist();
				}
			}
			plot->yTick(r1,r2);
}
	}
	
	CDialog::OnOK();
}



