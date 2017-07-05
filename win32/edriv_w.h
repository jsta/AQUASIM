// edriv_w.h : header file
//

#ifndef edriv_w_h
#define edriv_w_h

#include "./aqcore/rivcomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdRiv dialog

class CEdRiv : public CDialog
{
// Construction
public:
	CEdRiv(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdRiv)
	enum { IDD = IDD_EDIT_COMPRIV };
	CButton	m_rb_sedyes;
	CButton	m_rb_sedno;
	CButton	m_bu_sed;
	CButton	m_cb_active;
	CButton	m_bu_acc;
	CButton	m_rb_highres;
	CButton	m_rb_withoutdisp;
	CStatic	m_tx_endlevel;
	CButton	m_rb_withdisp;
	CButton	m_rb_methkin;
	CButton	m_rb_methdiff;
	CButton	m_rb_levnorm;
	CButton	m_rb_levgiven;
	CButton	m_rb_lowres;
	CButton	m_rb_levcrit;
	CEdit	m_ed_width;
	CEdit	m_ed_start;
	CEdit	m_ed_perimeter;
	CEdit	m_ed_numgrid;
	CEdit	m_ed_gravit;
	CEdit	m_ed_frict;
	CEdit	m_ed_end;
	CEdit	m_ed_endlevel;
	CEdit	m_ed_disp;
	CEdit	m_ed_crosssect;
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
	//{{AFX_VIRTUAL(CEdRiv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRiv)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuProcs();
	afx_msg void OnBuVars();
	afx_msg void OnBuInits();
	afx_msg void OnBuInp();
	afx_msg void OnRbHighres();
	afx_msg void OnRbLevcrit();
	afx_msg void OnRbLevgiven();
	afx_msg void OnRbLevnorm();
	afx_msg void OnRbLowres();
	afx_msg void OnRbMethdiff();
	afx_msg void OnRbMethkin();
	afx_msg void OnRbWithdisp();
	afx_msg void OnRbWithoutdisp();
	afx_msg void OnBuAcc();
	afx_msg void OnRbSedno();
	afx_msg void OnRbSedyes();
	afx_msg void OnBuSed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*      oldcomp;
	RIVCOMP*     rcomp;
	BOOL         add;

	BOOL         withdispersion;
	RIVCOMPLEVEL leveltype;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivInits dialog

class CEdRivInits : public CDialog
{
// Construction
public:
	CEdRivInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdRivInits)
	enum { IDD = IDD_EDIT_COMPRIVINITS };
	CListBox	m_lb_inits;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivInit dialog

class CEdRivInit : public CDialog
{
// Construction
public:
	CEdRivInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdRivInit)
	enum { IDD = IDD_EDIT_COMPRIVINIT };
	CStatic	m_tx_zone;
	CComboBox	m_le_zone;
	CComboBox	m_le_var;
	CEdit	m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivSelInp dialog

class CEdRivSelInp : public CDialog
{
// Construction
public:
	CEdRivSelInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdRivSelInp)
	enum { IDD = IDD_EDIT_COMPRIVSELINP };
	CButton	m_rb_upstreaminp;
	CButton	m_rb_latinp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivSelInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivSelInp)
	virtual BOOL OnInitDialog();
	afx_msg void OnRbLatinp();
	afx_msg void OnRbUpstreaminp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivUpInps dialog

class CEdRivUpInps : public CDialog
{
// Construction
public:
	CEdRivUpInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

	// Dialog Data
	//{{AFX_DATA(CEdRivUpInps)
	enum { IDD = IDD_EDIT_COMPRIVUPINPS };
	CEdit	m_ed_watinf;
	CListBox	m_lb_inputs;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivUpInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivUpInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivUpInp dialog

class CEdRivUpInp : public CDialog
{
// Construction
public:
	CEdRivUpInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdRivUpInp)
	enum { IDD = IDD_EDIT_COMPRIVUPINP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivUpInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivUpInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivLatInps dialog

class CEdRivLatInps : public CDialog
{
// Construction
public:
	CEdRivLatInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdRivLatInps)
	enum { IDD = IDD_EDIT_COMPRIVLATINPS };
	CListBox	m_lb_inputs;
	CEdit	m_ed_watinf;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivLatInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivLatInps)
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

	RIVCOMP* rcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivLatInp dialog

class CEdRivLatInp : public CDialog
{
// Construction
public:
	CEdRivLatInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com, BOOL a, int i);

	// Dialog Data
	//{{AFX_DATA(CEdRivLatInp)
	enum { IDD = IDD_EDIT_COMPRIVLATINP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivLatInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivLatInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivAcc dialog

class CEdRivAcc : public CDialog
{
// Construction
public:
	CEdRivAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdRivAcc)
	enum { IDD = IDD_EDIT_COMPRIVACC };
	CEdit	m_ed_relaccz;
	CEdit	m_ed_relaccq;
	CEdit	m_ed_relacce;
	CEdit	m_ed_relacca;
	CEdit	m_ed_absaccz;
	CEdit	m_ed_absaccq;
	CEdit	m_ed_absacce;
	CEdit	m_ed_absacca;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivSedLayer dialog

class CEdRivSedLayer : public CDialog
{
// Construction
public:
	CEdRivSedLayer(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdRivSedLayer)
	enum { IDD = IDD_EDIT_COMPRIVSEDLAYER };
	CEdit	m_ed_dispersion;
	CEdit	m_ed_zoneind;
	CEdit	m_ed_velocity;
	CEdit	m_ed_porosity;
	CEdit	m_ed_exchcoeff;
	CEdit	m_ed_area;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivSedLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivSedLayer)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP*     rcomp;
	RIVSEDLAYER* sedlayer;
	BOOL         add;
	int          pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdRivSed dialog

class CEdRivSed : public CDialog
{
// Construction
public:
	CEdRivSed(CWnd* pParent = NULL);   // standard constructor

	int DoModal(RIVCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdRivSed)
	enum { IDD = IDD_EDIT_COMPRIVSED };
	CListBox	m_lb_sedlayers;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdRivSed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdRivSed)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbSedlayers();
	afx_msg void OnDblclkLbSedlayers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIVCOMP* rcomp;
};


#endif

