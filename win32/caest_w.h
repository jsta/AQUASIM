// caest_w.h : header file
//

#ifndef caparest_h
#define caparest_h

/////////////////////////////////////////////////////////////////////////////
// CCaParEst dialog

class CCaParEst : public CDialog
{
// Construction
public:
	CCaParEst(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCaParEst)
	enum { IDD = IDD_CALC_PARESTIMATION };
	CEdit   m_fititer;
	CListBox        m_availparlist;
	CListBox        m_availcalclist;
	CListBox        m_actparlist;
	CListBox        m_actcalclist;
	CButton m_start;
	CButton m_new;
	CButton m_inactpar;
	CButton m_inactfit;
	CButton m_duplicate;
	CButton m_edit;
	CButton m_delete;
	CButton m_actpar;
	CButton m_actfit;
	CButton m_methsecant;
	CButton m_methsimplex;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

	BOOL ReadFields(BOOL notify);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaParEst)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaParEst)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuActfit();
	afx_msg void OnBuActpar();
	afx_msg void OnBuDeletefit();
	afx_msg void OnBuDuplfit();
	afx_msg void OnBuEditfit();
	afx_msg void OnBuInactfit();
	afx_msg void OnBuInactpar();
	afx_msg void OnBuNewfit();
	afx_msg void OnBuStart();
	afx_msg void OnDblclkLbActcalc();
	afx_msg void OnSelchangeLbActcalc();
	afx_msg void OnSelchangeLbActpar();
	afx_msg void OnDblclkLbAvailcalc();
	afx_msg void OnSelchangeLbAvailcalc();
	afx_msg void OnSelchangeLbAvailpar();
	afx_msg void OnRbSecant();
	afx_msg void OnRbSimplex();
	afx_msg void OnDblclkLbActpar();
	afx_msg void OnDblclkLbAvailpar();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
	BOOL lastselact;
};


/////////////////////////////////////////////////////////////////////////////
// CCaParEstRes dialog

class CCaParEstRes : public CDialog
{
// Construction
public:
	CCaParEstRes(CWnd* pParent = NULL);   // standard constructor

	int DoModal(BOOL max, CARDINAL num);

// Dialog Data
	//{{AFX_DATA(CCaParEstRes)
	enum { IDD = IDD_CALC_PARESTRES };
	CStatic	m_tx_status;
	CStatic	m_tx_numiter;
	CStatic	m_tx_chiini;
	CStatic	m_tx_chiend;
	CListBox	m_lb_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaParEstRes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaParEstRes)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL     maxit;
	CARDINAL numit;
};


/////////////////////////////////////////////////////////////////////////////
// CCaParEstCalc dialog

class CCaParEstCalc : public CDialog
{
// Construction
public:
	CCaParEstCalc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FIT* f, BOOL a);

	void UpdateDialog();
	void UpdateActivation();

	// Dialog Data
	//{{AFX_DATA(CCaParEstCalc)
	enum { IDD = IDD_CALC_PARESTCALC };
	CButton	m_rb_steady;
	CButton	m_rb_given;
	CListBox	m_lb_targets;
	CEdit	m_ed_name;
	CEdit	m_ed_inittime;
	CEdit	m_ed_descript;
	CEdit	m_ed_calcnum;
	CButton	m_cb_status;
	CButton	m_bu_edit;
	CButton	m_bu_add;
	CButton	m_bu_delete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaParEstCalc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaParEstCalc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRbGiven();
	afx_msg void OnRbSteady();
	afx_msg void OnDblclkLbTargets();
	afx_msg void OnSelchangeLbTargets();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnBuAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FIT* oldfit;
	FIT* newfit;
	BOOL add;
};


/////////////////////////////////////////////////////////////////////////////
// CCaParEstTarget dialog

class CCaParEstTarget : public CDialog
{
// Construction
public:
	CCaParEstTarget(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FIT* f, CARDINAL p, FITDATA d, BOOL a);

	void UpdateZoneList();

	// Dialog Data
	//{{AFX_DATA(CCaParEstTarget)
	enum { IDD = IDD_CALC_PARESTTARGET };
	CButton	m_cb_spacerel;
	CStatic	m_tx_var;
	CStatic	m_tx_data;
	CStatic	m_tx_zone;
	CStatic	m_tx_comp;
	CComboBox	m_le_var;
	CComboBox	m_le_data;
	CComboBox	m_le_zone;
	CComboBox	m_le_comp;
	CEdit	m_ed_torx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaParEstTarget)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaParEstTarget)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCloseupLeComp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FIT*     fit;
	FITDATA  data;
	CARDINAL pos;
	BOOL     add;
};


#endif

