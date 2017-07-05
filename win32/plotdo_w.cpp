// PlotDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "PLOTDO_W.H"

#include "mainfr_w.h"			// *JR*
#include "waidlg_w.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc

IMPLEMENT_DYNCREATE(CPlotDoc, CDocument)

CPlotDoc::CPlotDoc()
{
	aqapp.MainFrame->currentdocument=this;
	PLOT* orgplot=aqapp.MainFrame->currentplot;

	collect_status=GetPlotData(orgplot);
// make a unique copy of orgplot which will not change after edits
	plot=new PLOT (orgplot);

}

JOBSTATUS CPlotDoc::GetPlotData(PLOT* newplot)
{
   aqapp.CancelDlg.Display(aqapp.ini.T(124));

   plot=newplot;
   collect_status = aqsys.GetAllCurvesData(plot,&data);
   if ( collect_status == OK )
   {
      if ( data.IsFinite() == FALSE )
      {
         *cjob << "\n\n   *** " 
               << "Undefined value in plot " 
               << plot->Symbol() << " ***";
         cjob->flush();
         collect_status = PROBLEM;
      }
   }
   aqapp.CancelDlg.Remove();
   return collect_status;
}

BOOL CPlotDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPlotDoc::~CPlotDoc()
{
	delete plot;
}


BEGIN_MESSAGE_MAP(CPlotDoc, CDocument)
	//{{AFX_MSG_MAP(CPlotDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc diagnostics

#ifdef _DEBUG
void CPlotDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlotDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc serialization

void CPlotDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc commands
