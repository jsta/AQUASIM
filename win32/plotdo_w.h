// PlotDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlotDoc document
class PLOT;
class PLOTDATA;


class CPlotDoc : public CDocument
{
public:
	CPlotDoc();           // protected constructor used by dynamic creation

	JOBSTATUS GetPlotData(PLOT* newplot);

protected:
	DECLARE_DYNCREATE(CPlotDoc)

//	CSize m_sizeDoc;

// Attributes
public:
	PLOT* plot;										  // *JR*
	PLOTDATA data;
	JOBSTATUS collect_status;

//	void Setm_size() { m_sizeDoc=ScrI.scrsize; }
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlotDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlotDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
