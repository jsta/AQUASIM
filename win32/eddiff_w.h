// eddiff_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdLinkDiff dialog

class CEdLinkDiff : public CDialog
{
// Construction
public:
	CEdLinkDiff(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQLINK* link, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdLinkDiff)
	enum { IDD = IDD_EDIT_LINKDIFF };
	CStatic	m_tx_conn2;
	CStatic	m_tx_conn1;
	CStatic	m_tx_comp2;
	CStatic	m_tx_comp1;
	CComboBox	m_le_conn2;
	CComboBox	m_le_conn1;
	CComboBox	m_le_comp2;
	CComboBox	m_le_comp1;
	CListBox	m_lb_excoeff;
	CEdit	m_ed_name;
	CEdit	m_ed_linkind;
	CEdit	m_ed_description;
	CButton	m_bu_edit;
	CButton	m_bu_add;
	CButton	m_bu_delete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLinkDiff)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLinkDiff)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnDblclkLbExcoeff();
	afx_msg void OnSelchangeLbExcoeff();
	afx_msg void OnCloseupLeComp1();
	afx_msg void OnCloseupLeComp2();
	afx_msg void OnCloseupLeConn1();
	afx_msg void OnCloseupLeConn2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQLINK*   oldlink;
	DIFFLINK* dlink;
	BOOL      add;
	CARDINAL  conn1;
	CARDINAL  conn2;

	void      UpdateConnectList1();
	void      UpdateConnectList2();
	void      UpdateExchList();
	void      UpdateActivation();
};
/////////////////////////////////////////////////////////////////////////////
// CEdLinkDiffExch dialog

class CEdLinkDiffExch : public CDialog
{
// Construction
public:
	CEdLinkDiffExch(CWnd* pParent = NULL);   // standard constructor

	int DoModal(DIFFLINK* l, BOOL a, int i);

	// Dialog Data
	//{{AFX_DATA(CEdLinkDiffExch)
	enum { IDD = IDD_EDIT_LINKDIFFEXCH };
	CComboBox	m_le_var;
	CEdit	m_ed_exch;
	CEdit	m_ed_conv1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLinkDiffExch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLinkDiffExch)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	DIFFLINK* dlink;
	BOOL      add;
	int       pos;
};
