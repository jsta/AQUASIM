// edvar_w.h : header file
//

#ifndef edvar_w_h
#define edvar_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdVars dialog

class CEdVars : public CDialog
{
// Construction
public:
	CEdVars(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEdVars)
	enum { IDD = IDD_EDIT_VARIABLES };
	CButton m_new;
	CButton m_exchange;
	CButton m_edittype;
	CButton m_duplicate;
	CButton m_delete;
	CButton m_edit;
	CListBox m_varlist;
	CStatic m_tx_type;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVars)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVars)
	virtual void OnOK();
	afx_msg void OnBuNew();
	afx_msg void OnSelchangeLbVarlist();
	afx_msg void OnBuDelete();
	afx_msg void OnBuDuplicate();
	afx_msg void OnBuEdit();
	afx_msg void OnBuEdittype();
	afx_msg void OnBuExchange();
	afx_msg void OnDblclkLbVarlist();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CSelVarDlg dialog

class CSelVarDlg : public CDialog
{
// Construction
public:
	CSelVarDlg(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* v);

	// Dialog Data
	//{{AFX_DATA(CSelVarDlg)
	enum { IDD = IDD_EDIT_SELVARTYPE };
	CButton m_statevar;
	CButton m_progvar;
	CButton m_parvar;
	CButton m_reallistvar;
	CButton m_varlistvar;
	CButton m_formvar;
	CButton m_probevar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelVarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelVarDlg)
	afx_msg void OnRbFormvar();
	afx_msg void OnRbParvar();
	afx_msg void OnRbProbevar();
	afx_msg void OnRbProgvar();
	afx_msg void OnRbReallistvar();
	afx_msg void OnRbStatevar();
	afx_msg void OnRbVarlistvar();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR* var;
};


#endif
