// aqview_w.cpp : implementation of the CAQUASIMView class
//

#include "stdafx.h"
#include "AQUASIM.h"

#include "aqdoc_w.h"
#include "aqview_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMView

IMPLEMENT_DYNCREATE(CAQUASIMView, CView)

BEGIN_MESSAGE_MAP(CAQUASIMView, CView)
	//{{AFX_MSG_MAP(CAQUASIMView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMView construction/destruction

CAQUASIMView::CAQUASIMView()
{
	// TODO: add construction code here

}

CAQUASIMView::~CAQUASIMView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMView drawing

void CAQUASIMView::OnDraw(CDC* pDC)
{
	CAQUASIMDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMView printing

BOOL CAQUASIMView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAQUASIMView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAQUASIMView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMView diagnostics

#ifdef _DEBUG
void CAQUASIMView::AssertValid() const
{
	CView::AssertValid();
}

void CAQUASIMView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAQUASIMDoc* CAQUASIMView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAQUASIMDoc)));
	return (CAQUASIMDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMView message handlers
/*
void CAQUASIMView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CAQUASIMDoc* pDoc = GetDocument();
	pDoc->SetTitle(aqapp.systemfilename);

}
*/
