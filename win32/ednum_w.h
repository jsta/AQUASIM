// ednum_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdNumPar dialog

class CEdNumPar : public CDialog
{
// Construction
public:
	CEdNumPar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEdNumPar)
	enum { IDD = IDD_EDIT_NUMPAR };
	CEdit	m_ed_fittol;
	CEdit	m_ed_stepsize;
	CEdit	m_ed_order;
	CEdit	m_ed_maxstep;
	CEdit	m_ed_codiag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdNumPar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdNumPar)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
