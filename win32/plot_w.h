// plot_w.h : header file
//

#ifndef plot_w_h
#define plot_w_h

#include <afxtempl.h>
#include "plotvw_w.h"

class PlotManager
{
public:

	PlotManager();

//	void ReplacePlotPtr(PLOT* oldplot, PLOT* newplot);
	// ReplacePlotPtr to be called from
	//                CViewEdPlotDef::OnOK() [in VIRES.CPP]

	void AddPlot(CPlotView* p);
	void PlotIsOnTop(CPlotView* p);
	void RemovePlot(CPlotView* p);
	void RedrawAll(BOOLEAN cascade=FALSE, int winwidth=0, int winheight=0);
	void RedrawActive();
	void AddIcon(BOOLEAN removeISfalse);
	CPlotView* IsActive() { return Active; }
	int Count() { return plotlist.GetCount(); }
	void DeleteAllViews();

private:

	CList<CPlotView*, CPlotView*> plotlist;
	POSITION last;
	CPlotView* Active;
	int NIcons;
};
/////////////////////////////////////////////////////////////////////////////
// static PlotManager AllPlots;		 // *JR* aug96

JOBSTATUS DrawPlot(PLOT* plot);

void RedrawAllPlots(BOOLEAN cascade);

/////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////
