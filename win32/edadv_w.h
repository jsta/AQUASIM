// edadv_w.h : header file
//

#ifndef edadvc_w_h
#define edadvc_w_h

#include "./aqcore/advcomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdAdv dialog

class CEdAdv : public CDialog
{
// Construction
public:
	CEdAdv(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdAdv)
	enum { IDD = IDD_EDIT_COMPADV };
	CButton	m_cb_active;
	CButton	m_rb_withoutdiff;
	CButton	m_rb_withdiff;
	CButton	m_rb_highres;
	CButton	m_rb_lowres;
	CEdit	m_ed_start;
	CEdit	m_ed_numgrid;
	CEdit	m_ed_crosssect;
	CEdit	m_ed_end;
	CEdit	m_ed_diff;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	CEdit	m_ed_compind;
	CButton	m_bu_vars;
	CButton	m_bu_procs;
	CButton	m_bu_inp;
	CButton	m_bu_inits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdv)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuVars();
	afx_msg void OnBuProcs();
	afx_msg void OnBuInits();
	afx_msg void OnBuInp();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnRbWithoutdiff();
	afx_msg void OnRbWithdiff();
	afx_msg void OnRbLowres();
	afx_msg void OnRbHighres();
	afx_msg void OnBuAcc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*   oldcomp;
	ADVCOMP*  acomp;
	BOOL      add;

	BOOL      withdiffusion;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInits dialog

class CEdAdvInits : public CDialog
{
// Construction
public:
	CEdAdvInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdAdvInits)
	enum { IDD = IDD_EDIT_COMPADVINITS };
	CListBox	m_lb_inits;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInit dialog

class CEdAdvInit : public CDialog
{
// Construction
public:
	CEdAdvInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdAdvInit)
	enum { IDD = IDD_EDIT_COMPADVINIT };
	CStatic	m_tx_zone;
	CComboBox	m_le_zone;
	CComboBox	m_le_var;
	CEdit	m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvSelInp dialog

class CEdAdvSelInp : public CDialog
{
// Construction
public:
	CEdAdvSelInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdAdvSelInp)
	enum { IDD = IDD_EDIT_COMPADVSELINP };
	CButton	m_rb_latinp;
	CButton	m_rb_inletinp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvSelInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvSelInp)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbInletinp();
	afx_msg void OnRbLatinp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInInps dialog

class CEdAdvInInps : public CDialog
{
// Construction
public:
	CEdAdvInInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdAdvInInps)
	enum { IDD = IDD_EDIT_COMPADVININPS };
	CEdit	m_ed_watinf;
	CListBox	m_lb_inputs;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvInInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvInInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvInInp dialog

class CEdAdvInInp : public CDialog
{
// Construction
public:
	CEdAdvInInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdAdvInInp)
	enum { IDD = IDD_EDIT_COMPADVININP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvInInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvInInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvLatInps dialog

class CEdAdvLatInps : public CDialog
{
// Construction
public:
	CEdAdvLatInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdAdvLatInps)
	enum { IDD = IDD_EDIT_COMPADVLATINPS };
	CListBox	m_lb_inputs;
	CEdit	m_ed_watinf;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvLatInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvLatInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvLatInp dialog

class CEdAdvLatInp : public CDialog
{
// Construction
public:
	CEdAdvLatInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdAdvLatInp)
	enum { IDD = IDD_EDIT_COMPADVLATINP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvLatInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvLatInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdAdvAcc dialog

class CEdAdvAcc : public CDialog
{
// Construction
public:
	CEdAdvAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(ADVCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdAdvAcc)
	enum { IDD = IDD_EDIT_COMPADVACC };
	CEdit	m_ed_relaccq;
	CEdit	m_ed_relaccd;
	CEdit	m_ed_absaccq;
	CEdit	m_ed_absaccd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdAdvAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdAdvAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	ADVCOMP* acomp;
};


#endif
