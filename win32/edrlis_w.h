// edrlis_w.h : header file
//

#ifndef edrlis_w_h
#define edrlis_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdVarRealList dialog

class CEdVarRealList : public CDialog
{
// Construction
public:
	CEdVarRealList(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQVAR* var, BOOL a);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdVarRealList)
	enum { IDD = IDD_EDIT_VARREALLIST };
	CButton	m_bu_replace;
	CStatic	m_tx_stddata;
	CButton	m_rb_individual;
	CButton	m_rb_global;
	CStatic	m_tx_width;
	CStatic	m_tx_relstd;
	CStatic	m_tx_numpairs;
	CStatic	m_tx_absstd;
	CButton	m_rb_spline;
	CButton	m_rb_smooth;
	CButton	m_rb_linear;
	CComboBox	m_le_arg;
	CListBox	m_lb_list;
	CEdit	m_ed_width;
	CEdit	m_ed_valdata;
	CEdit	m_ed_stddata;
	CEdit	m_ed_relstd;
	CEdit	m_ed_min;
	CEdit	m_ed_max;
	CEdit	m_ed_argdata;
	CEdit	m_ed_absstd;
	CButton	m_cb_active;
	CButton	m_bu_write;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	CButton	m_bu_read;
	CEdit	m_ed_unit;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarRealList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarRealList)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuAddval();
	afx_msg void OnBuDelete();
	afx_msg void OnBuRead();
	afx_msg void OnBuWrite();
	afx_msg void OnSelchangeLbList();
	afx_msg void OnRbGlobal();
	afx_msg void OnRbIndividual();
	afx_msg void OnRbLinear();
	afx_msg void OnRbSmooth();
	afx_msg void OnRbSpline();
	afx_msg void OnBuReplval();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*       oldvar;
	REALLISTVAR* rlvar;
	BOOL         add;
};


/////////////////////////////////////////////////////////////////////////////
// CEdVarRealListData dialog

class CEdVarRealListData : public CDialog
{
// Construction
public:
	CEdVarRealListData(CWnd* pParent = NULL);   // standard constructor

	int DoModal(REALLISTVAR* var, std::ifstream* file);

// Dialog Data
	//{{AFX_DATA(CEdVarRealListData)
	enum { IDD = IDD_EDIT_VARREALLISTREAD };
	CStatic	m_tx_stdev;
	CEdit	m_ed_value;
	CEdit	m_ed_stdev;
	CEdit	m_ed_startrow;
	CEdit	m_ed_endrow;
	CEdit	m_ed_argument;
	CButton	m_cb_delete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarRealListData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarRealListData)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	REALLISTVAR* rlvar;
	std::ifstream*    readfile;

	static INTEGER firstrow;
	static INTEGER lastrow;
	static INTEGER colarg;
	static INTEGER colval;
	static INTEGER colstd;
	static BOOLEAN delpairs;
};


#endif


