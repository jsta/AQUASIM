// casens_w.h : header file
//

#ifndef casens_w_h
#define casens_w_h

/////////////////////////////////////////////////////////////////////////////
// CCaSensAn dialog

class CCaSensAn : public CDialog
{
// Construction
public:
	CCaSensAn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCaSensAn)
	enum { IDD = IDD_CALC_SENSANALYSIS };
	CListBox        m_availparlist;
	CListBox        m_availcalclist;
	CListBox        m_actparlist;
	CListBox        m_actcalclist;
	CButton m_new;
	CButton m_duplicate;
	CButton m_edit;
	CButton m_inactcalc;
	CButton m_inactpar;
	CButton m_delete;
	CButton m_actcalc;
	CButton m_actpar;
	CButton m_start;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

	BOOL ReadFields(BOOL notify);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaSensAn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaSensAn)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuActpar();
	afx_msg void OnBuActcalc();
	afx_msg void OnBuDelete();
	afx_msg void OnBuInactpar();
	afx_msg void OnBuInactcalc();
	afx_msg void OnBuNew();
	afx_msg void OnBuStart();
	afx_msg void OnDblclkLbActcalc();
	afx_msg void OnSelchangeLbActcalc();
	afx_msg void OnDblclkLbAvailcalc();
	afx_msg void OnSelchangeLbAvailcalc();
	afx_msg void OnSelchangeLbActpar();
	afx_msg void OnSelchangeLbAvailpar();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDuplicate();
	afx_msg void OnDblclkLbActpar();
	afx_msg void OnDblclkLbAvailpar();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int  created;
	BOOL lastselact;
};


/////////////////////////////////////////////////////////////////////////////
// CCaSensCalc dialog

class CCaSensCalc : public CDialog
{
// Construction
public:
	CCaSensCalc(CWnd* pParent = NULL);   // standard constructor

	int     DoModal(CALC* c, BOOL a);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CCaSensCalc)
	enum { IDD = IDD_CALC_SENSCALC };
	CButton	m_bu_replace;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	CListBox	m_lb_list;
	CButton m_rb_steady;
	CButton m_rb_given;
	CEdit   m_ed_stepsize;
	CEdit   m_ed_numstep;
	CEdit   m_ed_name;
	CEdit   m_ed_inittime;
	CEdit   m_ed_descript;
	CEdit   m_ed_calcnum;
	CButton m_cb_simstatus;
	CButton m_cb_sensstatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaSensCalc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaSensCalc)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbGiven();
	afx_msg void OnRbSteady();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuReplace();
	afx_msg void OnSelchangeLbList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CALC* oldcalc;
	CALC* newcalc;
	BOOL  add;
};


#endif
