// edsoil_w.h : header file
//

#ifndef edsoil_w_h
#define edsoil_w_h

#include "./aqcore/soilcomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdSoil dialog

class CEdSoil : public CDialog
{
// Construction
public:
	CEdSoil(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

	void UpdateDialog();
	void UpdateActivation();

	// Dialog Data
	//{{AFX_DATA(CEdSoil)
	enum { IDD = IDD_EDIT_COMPSOIL };
	CButton	m_cb_active;
	CButton	m_bu_acc;
	CButton	m_rb_withdisp;
	CButton	m_rb_withoutdisp;
	CEdit	m_ed_disp;
	CEdit	m_ed_crosssect;
	CEdit	m_ed_start;
	CEdit	m_ed_end;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	CEdit	m_ed_numgrid;
	CListBox	m_lb_parzones;
	CButton	m_rb_lowres;
	CButton	m_rb_highres;
	CEdit	m_ed_advvolfract;
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
	//{{AFX_VIRTUAL(CEdSoil)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoil)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuVars();
	afx_msg void OnBuProcs();
	afx_msg void OnBuInp();
	afx_msg void OnBuInits();
	afx_msg void OnRbWithdiff();
	afx_msg void OnRbWithoutdiff();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbParzones();
	afx_msg void OnDblclkLbParzones();
	afx_msg void OnRbLowres();
	afx_msg void OnRbHighres();
	afx_msg void OnBuAcc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*   oldcomp;
	SOILCOMP* scomp;
	BOOL      add;

	BOOL      withdispersion;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInits dialog

class CEdSoilInits : public CDialog
{
// Construction
public:
	CEdSoilInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSoilInits)
	enum { IDD = IDD_EDIT_COMPSOILINITS };
	CListBox	m_lb_inits;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInit dialog

class CEdSoilInit : public CDialog
{
// Construction
public:
	CEdSoilInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSoilInit)
	enum { IDD = IDD_EDIT_COMPSOILINIT };
	CStatic	m_tx_zone;
	CComboBox	m_le_zone;
	CComboBox	m_le_var;
	CEdit	m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilSelInp dialog

class CEdSoilSelInp : public CDialog
{
// Construction
public:
	CEdSoilSelInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdSoilSelInp)
	enum { IDD = IDD_EDIT_COMPSOILSELINP };
	CButton	m_rb_latinp;
	CButton	m_rb_inletinp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilSelInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilSelInp)
	afx_msg void OnRbInletinp();
	afx_msg void OnRbLatinp();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInInps dialog

class CEdSoilInInps : public CDialog
{
// Construction
public:
	CEdSoilInInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSoilInInps)
	enum { IDD = IDD_EDIT_COMPSOILININPS };
	CListBox	m_lb_inputs;
	CEdit	m_ed_watinf;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilInInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilInInps)
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

	SOILCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilInInp dialog

class CEdSoilInInp : public CDialog
{
// Construction
public:
	CEdSoilInInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSoilInInp)
	enum { IDD = IDD_EDIT_COMPSOILININP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilInInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilInInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilLatInps dialog

class CEdSoilLatInps : public CDialog
{
// Construction
public:
	CEdSoilLatInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSoilLatInps)
	enum { IDD = IDD_EDIT_COMPSOILLATINPS };
	CListBox	m_lb_inputs;
	CEdit	m_ed_watinf;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilLatInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilLatInps)
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

	SOILCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilLatInp dialog

class CEdSoilLatInp : public CDialog
{
// Construction
public:
	CEdSoilLatInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSoilLatInp)
	enum { IDD = IDD_EDIT_COMPSOILLATINP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilLatInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilLatInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilParZone dialog

class CEdSoilParZone : public CDialog
{
// Construction
public:
	CEdSoilParZone(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com, PARZONE* zone);

	void UpdateDialog();
	void UpdateActivation();

	// Dialog Data
	//{{AFX_DATA(CEdSoilParZone)
	enum { IDD = IDD_EDIT_COMPSOILPARZONE };
	CListBox	m_lb_serzones;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilParZone)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilParZone)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbSerzones();
	afx_msg void OnDblclkLbSerzones();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
	PARZONE*  oldzone;
	PARZONE*  parzone;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilSerZone dialog

class CEdSoilSerZone : public CDialog
{
// Construction
public:
	CEdSoilSerZone(CWnd* pParent = NULL);   // standard constructor

	int DoModal(PARZONE* zone, BOOL a, int i);

	void UpdateDialog();
	void UpdateActivation();

	// Dialog Data
	//{{AFX_DATA(CEdSoilSerZone)
	enum { IDD = IDD_EDIT_COMPSOILSERZONE };
	CListBox	m_lb_exchpars;
	CEdit	m_ed_zoneind;
	CEdit	m_ed_volfract;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilSerZone)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilSerZone)
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbExchpars();
	afx_msg void OnDblclkLbExchpars();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	PARZONE* parzone;
	SERZONE* serzone;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilExchCoeff dialog

class CEdSoilExchCoeff : public CDialog
{
// Construction
public:
	CEdSoilExchCoeff(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SERZONE* zone, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSoilExchCoeff)
	enum { IDD = IDD_EDIT_CONVSOILEXCH };
	CComboBox	m_le_var;
	CEdit	m_ed_exchcoeff;
	CEdit	m_ed_convfact;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilExchCoeff)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilExchCoeff)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SERZONE* serzone;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSoilAcc dialog

class CEdSoilAcc : public CDialog
{
// Construction
public:
	CEdSoilAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SOILCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdSoilAcc)
	enum { IDD = IDD_EDIT_COMPSOILACC };
	CEdit	m_ed_relaccq;
	CEdit	m_ed_relaccd;
	CEdit	m_ed_absaccq;
	CEdit	m_ed_absaccd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSoilAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSoilAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SOILCOMP* scomp;
};


#endif
