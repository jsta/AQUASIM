// aqdoc_w.cpp : implementation of the CAQUASIMDoc class
//

#include "stdafx.h"
#include "AQUASIM.h"

#include "aqdoc_w.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMDoc

IMPLEMENT_DYNCREATE(CAQUASIMDoc, CDocument)

BEGIN_MESSAGE_MAP(CAQUASIMDoc, CDocument)
	//{{AFX_MSG_MAP(CAQUASIMDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMDoc construction/destruction

CAQUASIMDoc::CAQUASIMDoc()
{
	// TODO: add one-time construction code here

}

CAQUASIMDoc::~CAQUASIMDoc()
{
}

BOOL CAQUASIMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMDoc serialization

void CAQUASIMDoc::Serialize(CArchive& ar)
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
// CAQUASIMDoc diagnostics

#ifdef _DEBUG
void CAQUASIMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAQUASIMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAQUASIMDoc commands
