// edadvl_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdv dialog

class CEdLinkAdv : public CDialog
{
// Construction
public:
	CEdLinkAdv(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQLINK* link, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdLinkAdv)
	enum { IDD = IDD_EDIT_LINKADV };
	CStatic	m_tx_connout;
	CStatic	m_tx_connin;
	CStatic	m_tx_compout;
	CStatic	m_tx_compin;
	CComboBox	m_le_connout;
	CComboBox	m_le_connin;
	CComboBox	m_le_compout;
	CComboBox	m_le_compin;
	CListBox	m_lb_bifs;
	CEdit	m_ed_name;
	CEdit	m_ed_linkind;
	CEdit	m_ed_description;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLinkAdv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLinkAdv)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDblclkLbBifs();
	afx_msg void OnSelchangeLbBifs();
	afx_msg void OnCloseupLeCompin();
	afx_msg void OnCloseupLeCompout();
	afx_msg void OnCloseupLeConnin();
	afx_msg void OnCloseupLeConnout();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQLINK*  oldlink;
	ADVLINK* alink;
	BOOL     add;
	CARDINAL connin;
	CARDINAL connout;

	void     UpdateConnectListIn();
	void     UpdateConnectListOut();
	void     UpdateBifList();
	void     UpdateActivation();
};


/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdvBif dialog

class CEdLinkAdvBif : public CDialog
{
// Construction
public:
	CEdLinkAdvBif(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVLINK* l, BIF* b, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdLinkAdvBif)
	enum { IDD = IDD_EDIT_LINKADVBIF };
	CStatic	m_tx_connect;
	CComboBox	m_le_compart;
	CStatic	m_tx_fluxlist;
	CStatic	m_tx_compart;
	CButton	m_rb_withwater;
	CButton	m_rb_asgiven;
	CComboBox	m_le_connect;
	CListBox	m_lb_fluxes;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	CEdit	m_ed_waterflow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLinkAdvBif)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLinkAdvBif)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnDblclkLbFluxes();
	afx_msg void OnCloseupLeConnect();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCloseupLeCompart();
	afx_msg void OnRbAsgiven();
	afx_msg void OnRbWithwater();
	afx_msg void OnSelchangeLbFluxes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVLINK* alink;
	BIF*     oldbif;
	BIF*     bif;
	BOOL     add;
	CARDINAL connect;

	void     UpdateConnectList();
	void     UpdateFluxList();
	void     UpdateActivation();
};
/////////////////////////////////////////////////////////////////////////////
// CEdLinkAdvBifFlux dialog

class CEdLinkAdvBifFlux : public CDialog
{
// Construction
public:
	CEdLinkAdvBifFlux(CWnd* pParent = NULL);   // standard constructor

	int DoModal(BIF* b, BOOL a, int i);

	// Dialog Data
	//{{AFX_DATA(CEdLinkAdvBifFlux)
	enum { IDD = IDD_EDIT_LINKADVBIFFLUX };
	CComboBox	m_le_var;
	CEdit	m_ed_flux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLinkAdvBifFlux)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLinkAdvBifFlux)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BIF* bif;
	BOOL add;
	int  pos;
};
