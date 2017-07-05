// edprog_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdVarProg dialog

class CEdVarProg : public CDialog
{
// Construction
public:
	CEdVarProg(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* var, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdVarProg)
	enum { IDD = IDD_EDIT_VARPROG };
	CComboBox	m_le_refto;
	CEdit	m_ed_unit;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarProg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarProg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*   oldvar;
	PROGVAR* pvar;
	BOOL     add;
};
