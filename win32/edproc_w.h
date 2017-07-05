// edproc_w.h : header file
//

#ifndef edproc_w_h
#define edproc_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdProcs dialog

class CEdProcs : public CDialog
{
// Construction
public:
	CEdProcs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEdProcs)
	enum { IDD = IDD_EDIT_PROCESSES };
	CListBox m_proclist;
	CButton m_new;
	CButton m_edit;
	CButton m_duplicate;
	CButton m_delete;
	CStatic m_tx_type;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdProcs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdProcs)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuDelete();
	afx_msg void OnBuDuplicate();
	afx_msg void OnBuEdit();
	afx_msg void OnBuNew();
	afx_msg void OnDblclkLbProclist();
	afx_msg void OnSelchangeLbProclist();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CSelProcDlg dialog

class CSelProcDlg : public CDialog
{
// Construction
public:
	CSelProcDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelProcDlg)
	enum { IDD = IDD_EDIT_SELPROCTYPE };
	CButton m_rb_eqproc;
	CButton m_rb_dynproc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelProcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelProcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbDynproc();
	afx_msg void OnRbEqproc();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif

