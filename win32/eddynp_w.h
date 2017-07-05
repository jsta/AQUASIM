// eddynp_w.h : header file
//

#ifndef eddynp_w_h
#define eddynp_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdProcDyn dialog

class CEdProcDyn : public CDialog
{
// Construction
public:
	CEdProcDyn(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQPROC* proc, BOOL a);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdProcDyn)
	enum { IDD = IDD_EDIT_PROCDYN };
	CListBox	m_lb_stoich;
	CEdit	m_ed_rate;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdProcDyn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdProcDyn)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnDblclkLbStoich();
	afx_msg void OnSelchangeLbStoich();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQPROC*  oldp;
	DYNPROC* dynp;
	BOOL     add;
};


/////////////////////////////////////////////////////////////////////////////
// CEdProcDynStoich dialog

class CEdProcDynStoich : public CDialog
{
// Construction
public:
	CEdProcDynStoich(CWnd* pParent = NULL);   // standard constructor

	int DoModal(DYNPROC* p, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdProcDynStoich)
	enum { IDD = IDD_EDIT_PROCDYNSTOICH };
	CComboBox	m_le_var;
	CEdit	m_ed_stoich;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdProcDynStoich)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdProcDynStoich)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	DYNPROC* dynp;
	BOOL     add;
	int      pos;
};


#endif
