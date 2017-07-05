// plot_w.cpp : implementation file
//

#include "stdafx.h"
#include "AQUASIM.h"
#include "MAINFR_W.H"	// *JR*
#include "plot_w.h"
//#include "WaitDlg_W.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

JOBSTATUS DrawPlot(PLOT* plot)
{
 /*
	PLOTDATA data;
	WaitBox collect("...collecting data...");

	JOBSTATUS status = aqsys.GetAllCurvesData(plot,&data);

	collect.stop();

	if ( status != OK ) return status;   
 */  
	aqapp.MainFrame->currentplot=plot;
	JOBSTATUS status=aqapp.MainFrame->MakeChild();

	return status;
}


void RedrawAllPlots(BOOLEAN cascade)
{
 	aqapp.MainFrame->plotmanager.RedrawAll(cascade, 
										   aqsys.PlotScrOpt()->WinWidth(),
										   aqsys.PlotScrOpt()->WinHeight());	// *JR* aug96

	return;
}

/////////////////////////////////////////////////////////////////////////////
PlotManager::PlotManager()
{
	Active=NULL;
	NIcons=0;
}

/*
void PlotManager::ReplacePlotPtr(PLOT* oldplot, PLOT* newplot)
{
	CPlotView* p;
	last=plotlist.GetHeadPosition();
	CArray <CPlotView*,CPlotView*> stale;
	int check;
	BOOLEAN changed=FALSE;
	POSITION tempPos=last;
	while (tempPos)
	{
		p=plotlist.GetNext(tempPos);
//		tempPos=last;
		check=p->ReplacePlotPtr(oldplot,newplot);
		switch(check)
		{  
		case 2:	stale.Add(p);	  changed=TRUE;
			break;
		case 1:	p->Invalidate();  changed=TRUE;
			break;
		case 0:	check=0;
			break;
		}
	}
	if (stale.GetSize())
		for (int i=0; i < stale.GetSize(); i++)
	{
			stale[i]->GetParentFrame()->DestroyWindow();

//			stale[i]->Suicide();
	}
	if (changed)
	{
		aqapp.MainFrame->RefreshFrameTitle();
	}
}
*/

void PlotManager::AddIcon(BOOLEAN removeISfalse)
{
	if (removeISfalse) NIcons++;
	else NIcons--;
	ASSERT (-1 < NIcons);
}

void PlotManager::AddPlot(CPlotView* p)
{
	plotlist.AddTail(p);
	Active=p;
}

void PlotManager::PlotIsOnTop(CPlotView* p)
{
	last=plotlist.Find(p);
	plotlist.RemoveAt(last);
	plotlist.AddTail(p);
	Active=p;
}

void PlotManager::RemovePlot(CPlotView* p)
{
	last=plotlist.Find(p);
	plotlist.RemoveAt(last);
	if (0 == plotlist.GetCount())
		aqapp.MainFrame->SetTitleForNoPlots();
}

void PlotManager::RedrawActive()
{
	if (Active) Active->Redraw();
}

void PlotManager::DeleteAllViews()
{
	CPlotView* p;
	last=plotlist.GetHeadPosition();
	while (last)
	{
		p=plotlist.GetNext(last);
		p->GetDocument()->OnCloseDocument( );

		last=plotlist.GetHeadPosition();
	}
	// aqapp.MainFrame->SetFrameTitle(); // command eliminate 2.10.98
}

void PlotManager::RedrawAll(BOOLEAN cascade, int width, int height)
{
	if (!Count()) return;
	if (Active) if (Active->GetParentFrame()->IsZoomed() ) return;
	ASSERT(0 < width);
	ASSERT(0 < height);
	CPlotView* p;
	const int X=5;
	const int Y=5;
	const int dx=24;
	const int dy=24;
	int x0, y0, x, y, mwid, mhei, left, top;
	x0=5; x=x0-dx;
	y0=5; y=y0-dy;

	CRect appwindow;
	aqapp.MainFrame->GetWindowRect(appwindow);
	CRect mainframe;
	aqapp.MainFrame->GetClientRect(mainframe);
	mwid=mainframe.Width()  / 2;
	mhei=mainframe.Height() / 2;

	int Icons=0;

	last=plotlist.GetHeadPosition();
	while (last)
	{
		p=plotlist.GetNext(last);
		CFrameWnd* plotframe=p->GetParentFrame( );
		if (plotframe->IsIconic( ))
		{
			Icons++;
		}
		else
		{
			p->Invalidate();
			CRect plotrect;
			if (cascade)
			{
				x=x+dx;
				y=y+dy;
				if (mwid < x)
				{
					x0=x0+dx;
					if (mwid < x0) x0=X;
					x=x0;
				}
				if (mhei < y)
				{
					y0=y0+dy/2;
					if (mhei < y0) y0=Y;
					y=y0;
				}
				left=x;
				top=y;
				plotframe->MoveWindow(left,top,width,height,TRUE);
			}
			else
			{
				p->SetWindowSize(width, height);
			}
		}
//		p->Redraw();
	}
	if (Icons) aqapp.MainFrame->MDIIconArrange( );
	aqapp.MainFrame->RefreshFrameTitle();

}



