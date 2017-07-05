// edcomp_w.h : header file
//

#ifndef edcomp_w_h
#define edcomp_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdComps dialog

class CEdComps : public CDialog
{
// Construction
public:
	CEdComps(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEdComps)
	enum { IDD = IDD_EDIT_COMPARTMENTS };
	CButton m_new;
	CListBox m_complist;
	CButton m_edit;
	CButton m_duplicate;
	CButton m_delete;
	CButton m_bu_act;
	CButton m_bu_inact;
	CStatic m_tx_type;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdComps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdComps)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuDelete();
	afx_msg void OnBuDuplicate();
	afx_msg void OnBuEdit();
	afx_msg void OnBuNew();
	afx_msg void OnBuAct();
	afx_msg void OnBuInact();
	afx_msg void OnDblclkLbComplist();
	afx_msg void OnSelchangeLbComplist();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CSelCompDlg dialog

class CSelCompDlg : public CDialog
{
// Construction
public:
	CSelCompDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelCompDlg)
	enum { IDD = IDD_EDIT_SELCOMPTYPE };
	CButton m_mixcomp;
	CButton m_filmcomp;
	CButton m_advcomp;
	CButton m_soilcomp;
	CButton m_rivcomp;
	CButton m_lakecomp;
	CButton m_sedcomp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelCompDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelCompDlg)
	virtual void OnOK();
	afx_msg void OnSedcomp();
	afx_msg void OnLakecomp();
	afx_msg void OnMixedcomp();
	afx_msg void OnFilmcomp();
	afx_msg void OnAdvcomp();
	afx_msg void OnRivcomp();
	afx_msg void OnSoilcomp();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CEdCompActVar dialog

class CEdCompActVar : public CDialog
{
// Construction
public:
	CEdCompActVar(CWnd* pParent = NULL);   // standard constructor

	int  DoModal(AQCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdCompActVar)
	enum { IDD = IDD_EDIT_COMPACTVAR };
	CListBox        m_lb_avail;
	CListBox        m_lb_act;
	CButton m_bu_inact;
	CButton m_bu_act;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdCompActVar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdCompActVar)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLbAct();
	afx_msg void OnDblclkLbAct();
	afx_msg void OnSelchangeLbAvail();
	afx_msg void OnDblclkLbAvail();
	afx_msg void OnBuAct();
	afx_msg void OnBuInact();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP* xcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdCompActProc dialog

class CEdCompActProc : public CDialog
{
// Construction
public:
	CEdCompActProc(CWnd* pParent = NULL);   // standard constructor

	int  DoModal(AQCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdCompActProc)
	enum { IDD = IDD_EDIT_COMPACTPROC };
	CListBox        m_lb_avail;
	CListBox        m_lb_act;
	CButton m_bu_inact;
	CButton m_bu_act;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdCompActProc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdCompActProc)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAct();
	afx_msg void OnBuInact();
	afx_msg void OnSelchangeLbAct();
	afx_msg void OnDblclkLbAct();
	afx_msg void OnSelchangeLbAvail();
	afx_msg void OnDblclkLbAvail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP* xcomp;
};


#endif
