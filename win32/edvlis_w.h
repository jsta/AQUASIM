// edvlis_w.h : header file
//

#ifndef edvlis_w_h
#define edvlis_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdVarVarList dialog

class CEdVarVarList : public CDialog
{
// Construction
public:
	CEdVarVarList(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* var, BOOL a);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdVarVarList)
	enum { IDD = IDD_EDIT_VARVARLIST };
	CButton	m_bu_replace;
	CComboBox	m_le_valdata;
	CStatic	m_tx_width;
	CStatic	m_tx_numpairs;
	CButton	m_rb_spline;
	CButton	m_rb_smooth;
	CButton	m_rb_linear;
	CComboBox	m_le_arg;
	CListBox	m_lb_list;
	CEdit	m_ed_width;
	CEdit	m_ed_argdata;
	CButton	m_bu_add;
	CButton	m_bu_delete;
	CEdit	m_ed_unit;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarVarList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarVarList)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuAddval();
	afx_msg void OnBuDelete();
	afx_msg void OnDblclkLbList();
	afx_msg void OnSelchangeLbList();
	afx_msg void OnRbLinear();
	afx_msg void OnRbSmooth();
	afx_msg void OnRbSpline();
	afx_msg void OnBuReplval();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*      oldvar;
	VARLISTVAR* vlvar;
	BOOL        add;
};


#endif
