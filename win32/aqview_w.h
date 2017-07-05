// aqview_w.h : interface of the CAQUASIMView class
//
/////////////////////////////////////////////////////////////////////////////

class CAQUASIMView : public CView
{
protected: // create from serialization only
	CAQUASIMView();
	DECLARE_DYNCREATE(CAQUASIMView)

// Attributes
public:
	CAQUASIMDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAQUASIMView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAQUASIMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAQUASIMView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in aqview_w.cpp
inline CAQUASIMDoc* CAQUASIMView::GetDocument()
   { return (CAQUASIMDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
