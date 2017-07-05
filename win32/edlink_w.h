// edlink_w.h : header file
//

#ifndef edlink_w_h
#define edlink_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdLinks dialog

class CEdLinks : public CDialog
{
// Construction
public:
	CEdLinks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEdLinks)
	enum { IDD = IDD_EDIT_LINKS };
	CListBox m_linklist;
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
	//{{AFX_VIRTUAL(CEdLinks)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLinks)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuDelete();
	afx_msg void OnBuDuplicate();
	afx_msg void OnBuEdit();
	afx_msg void OnBuNew();
	afx_msg void OnDblclkLbLinklist();
	afx_msg void OnSelchangeLbLinklist();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CSelLinkType dialog

class CSelLinkType : public CDialog
{
// Construction
public:
	CSelLinkType(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelLinkType)
	enum { IDD = IDD_EDIT_SELLINKTYPE };
	CButton m_rb_difflink;
	CButton m_rb_advlink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelLinkType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelLinkType)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRbAdvlink();
	afx_msg void OnRbDifflink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
