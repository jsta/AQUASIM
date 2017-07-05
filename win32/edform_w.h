// edform_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdVarForm dialog

class CEdVarForm : public CDialog
{
// Construction
public:
	CEdVarForm(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* var, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdVarForm)
	enum { IDD = IDD_EDIT_VARFORM };
	CEdit	m_ed_unit;
	CEdit	m_ed_name;
	CEdit	m_ed_expr;
	CEdit	m_ed_descript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarForm)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*   oldvar;
	FORMVAR* fvar;
	BOOL     add;
};
