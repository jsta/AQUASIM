// edmix_w.h : header file
//

#ifndef edmix_w_h
#define edmix_w_h

#include "./aqcore/mixcomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdMix dialog

class CEdMix : public CDialog
{
// Construction
public:
	CEdMix(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdMix)
	enum { IDD = IDD_EDIT_COMPMIX };
	CButton	m_cb_active;
	CStatic m_tx_vol;
	CStatic m_tx_outflow;
	CButton m_rb_constvol;
	CButton m_rb_varvol;
	CEdit   m_ed_vol;
	CEdit   m_ed_outflow;
	CEdit   m_ed_name;
	CEdit   m_ed_descript;
	CEdit   m_ed_compind;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdMix)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdMix)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuVars();
	afx_msg void OnBuProcs();
	afx_msg void OnBuInits();
	afx_msg void OnBuInp();
	afx_msg void OnRbConstvol();
	afx_msg void OnRbVarvol();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBuAcc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*   oldcomp;
	MIXCOMP*  mcomp;
	BOOL      add;
};


/////////////////////////////////////////////////////////////////////////////
// CEdMixInits dialog

class CEdMixInits : public CDialog
{
// Construction
public:
	CEdMixInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(MIXCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdMixInits)
	enum { IDD = IDD_EDIT_COMPMIXINITS };
	CListBox        m_lb_inits;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdMixInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdMixInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	MIXCOMP* mcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdMixInit dialog

class CEdMixInit : public CDialog
{
// Construction
public:
	CEdMixInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(MIXCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdMixInit)
	enum { IDD = IDD_EDIT_COMPMIXINIT };
	CStatic m_tx_zone;
	CComboBox       m_le_zone;
	CComboBox       m_le_var;
	CEdit   m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdMixInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdMixInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	MIXCOMP* mcomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdMixInps dialog

class CEdMixInps : public CDialog
{
// Construction
public:
	CEdMixInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(MIXCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdMixInps)
	enum { IDD = IDD_EDIT_COMPMIXINPS };
	CListBox        m_lb_inputs;
	CEdit   m_ed_watinf;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdMixInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdMixInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	virtual void OnOK();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	MIXCOMP* mcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdMixInp dialog

class CEdMixInp : public CDialog
{
// Construction
public:
	CEdMixInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(MIXCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdMixInp)
	enum { IDD = IDD_EDIT_COMPMIXINP };
	CComboBox       m_le_var;
	CEdit   m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdMixInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdMixInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	MIXCOMP* mcomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdMixAcc dialog

class CEdMixAcc : public CDialog
{
// Construction
public:
	CEdMixAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(MIXCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdMixAcc)
	enum { IDD = IDD_EDIT_COMPMIXACC };
	CEdit	m_ed_relaccv;
	CEdit	m_ed_relaccq;
	CEdit	m_ed_absaccv;
	CEdit	m_ed_absaccq;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdMixAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdMixAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	MIXCOMP* mcomp;
};


#endif

