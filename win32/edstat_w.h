// edstat_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdVarState dialog

class CEdVarState : public CDialog
{
// Construction
public:
	CEdVarState(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* var, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdVarState)
	enum { IDD = IDD_EDIT_VARSTATE };
	CButton	m_rb_eq;
	CButton	m_rb_dyn;
	CButton	m_rb_vol;
	CButton	m_rb_surf;
	CEdit	m_ed_unit;
	CEdit	m_ed_relacc;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	CEdit	m_ed_absacc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarState)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarState)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbDyn();
	afx_msg void OnRbEq();
	afx_msg void OnRbVol();
	afx_msg void OnRbSurf();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*    oldvar;
	STATEVAR* svar;
	BOOL      add;
	BOOL      volifdyn;
};
