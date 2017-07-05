// delsta_w.h : header file
//

#ifndef delsta_w_h
#define delsta_w_h

/////////////////////////////////////////////////////////////////////////////
// CDelStates dialog

class CDelStates : public CDialog
{
// Construction
public:
	CDelStates(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelStates)
	enum { IDD = IDD_EDIT_DELSTATES };
	CListBox        m_statelist;
	CListBox        m_calclist;
	CButton m_deletestates;
	CButton m_deletecalcs;
	CStatic m_tx_states;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();
	void UpdateCalcList();
	void UpdateStateList();
	void UpdateNumStates();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelStates)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelStates)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLbStatelist();
	afx_msg void OnSelchangeLbCalclist();
	afx_msg void OnBuDeletecalcs();
	afx_msg void OnBuDeletestates();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int      created;
	CARDINAL calcnum;
};

#endif
