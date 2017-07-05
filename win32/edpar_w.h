// edpar_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdVarPar dialog

class CEdVarPar : public CDialog
{
// Construction
public:
	CEdVarPar(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* var, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdVarPar)
	enum { IDD = IDD_EDIT_VARPAR };
	CEdit	m_ed_value;
	CEdit	m_ed_unit;
	CEdit	m_ed_stdev;
	CEdit	m_ed_name;
	CEdit	m_ed_min;
	CEdit	m_ed_max;
	CEdit	m_ed_descript;
	CButton	m_cb_sensact;
	CButton	m_cb_fitact;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarPar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarPar)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*    oldvar;
	CONSTVAR* cvar;
	BOOL      add;
};
