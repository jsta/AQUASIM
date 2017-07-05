// casim_w.h : header file
//

#ifndef casim_w_h
#define casim_w_h

/////////////////////////////////////////////////////////////////////////////
// CCaSim dialog

class CCaSim : public CDialog
{
// Construction
public:
	CCaSim(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCaSim)
	enum { IDD = IDD_CALC_SIMULATION };
	CListBox        m_actcalclist;
	CListBox        m_availcalclist;
	CButton m_new;
	CButton m_duplicate;
	CButton m_edit;
	CButton m_delete;
	CButton m_inactcalc;
	CButton m_actcalc;
	CButton m_start;
	CButton m_initialize;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

	BOOL ReadFields(BOOL notify);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaSim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaSim)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuInitialize();
	afx_msg void OnBuStart();
	afx_msg void OnBuActcalc();
	afx_msg void OnBuDelete();
	afx_msg void OnBuInactcalc();
	afx_msg void OnBuNew();
	afx_msg void OnDblclkLbActcalc();
	afx_msg void OnSelchangeLbActcalc();
	afx_msg void OnDblclkLbAvailcalc();
	afx_msg void OnSelchangeLbAvailcalc();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDuplicate();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
	BOOL lastselact;
};

/////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////
