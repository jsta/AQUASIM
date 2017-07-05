// edlake_w.h : header file
//

#ifndef edlake_w_h
#define edlake_w_h

#include "./aqcore/lakecomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdLake dialog

class CEdLake : public CDialog
{
// Construction
public:
	CEdLake(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdLake)
	enum { IDD = IDD_EDIT_EDLAKECOMP };
	CButton	m_cb_active;
	CButton m_rb_tkeno;
	CButton m_rb_tkeyes;
	CButton m_rb_sedno;
	CButton m_rb_sedyes;
	CButton m_rb_lowres;
	CButton m_rb_highres;
	CEdit   m_ed_top;
	CEdit   m_ed_numgrid;
	CEdit   m_ed_name;
	CEdit   m_ed_gravit;
	CEdit   m_ed_diffusion;
	CEdit   m_ed_isurf;
	CEdit   m_ed_eta;
	CEdit   m_ed_descript;
	CEdit   m_ed_density;
	CEdit   m_ed_crosssect;
	CEdit   m_ed_compind;
	CEdit   m_ed_bottom;
	CButton m_bu_tke;
	CButton m_bu_sed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLake)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLake)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBuVars();
	afx_msg void OnBuProcs();
	afx_msg void OnBuInits();
	afx_msg void OnBuInp();
	afx_msg void OnBuSed();
	afx_msg void OnBuTke();
	afx_msg void OnRbSedno();
	afx_msg void OnRbSedyes();
	afx_msg void OnRbTkeno();
	afx_msg void OnRbTkeyes();
	afx_msg void OnRbLowres();
	afx_msg void OnRbHighres();
	afx_msg void OnBuPartvars();
	afx_msg void OnBuDissvars();
	afx_msg void OnBuAcc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*   oldcomp;
	LAKECOMP* lcomp;
	BOOL      add;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSed dialog

class CEdLakeSed : public CDialog
{
// Construction
public:
	CEdLakeSed(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

	// Dialog Data
	//{{AFX_DATA(CEdLakeSed)
	enum { IDD = IDD_EDIT_EDLAKESED };
	CEdit   m_ed_volflux;
	CEdit   m_ed_resusp;
	CListBox	m_lb_sedlayers;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSed)
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

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeTKE dialog

class CEdLakeTKE : public CDialog
{
// Construction
public:
	CEdLakeTKE(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdLakeTKE)
	enum { IDD = IDD_EDIT_EDLAKETKE };
	CEdit   m_ed_surfshear;
	CEdit   m_ed_pressgrad;
	CEdit   m_ed_pwind;
	CEdit   m_ed_prantl;
	CEdit   m_ed_sigtke;
	CEdit   m_ed_sigeps;
	CEdit   m_ed_kzmin;
	CEdit   m_ed_pint;
	CEdit   m_ed_pbottom;
	CEdit   m_ed_dissip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeTKE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeTKE)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeParts dialog

class CEdLakeParts : public CDialog
{
// Construction
public:
	CEdLakeParts(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakeParts)
	enum { IDD = IDD_EDIT_COMPLAKEPARTS };
	CListBox        m_lb_partlist;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeParts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeParts)
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnSelchangeLbVars();
	afx_msg void OnDblclkLbVars();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeDisss dialog

class CEdLakeDisss : public CDialog
{
// Construction
public:
	CEdLakeDisss(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakeDisss)
	enum { IDD = IDD_EDIT_COMPLAKEDISSS };
	CListBox        m_lb_disslist;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeDisss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeDisss)
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnDblclkLbVars();
	afx_msg void OnSelchangeLbVars();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakePart dialog

class CEdLakePart : public CDialog
{
// Construction
public:
	CEdLakePart(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakePart)
	enum { IDD = IDD_EDIT_COMPLAKEPART };
	CComboBox       m_le_var;
	CEdit   m_ed_sedveloc;
	CEdit   m_ed_density;
	CEdit   m_ed_mobility;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakePart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakePart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeDiss dialog

class CEdLakeDiss : public CDialog
{
// Construction
public:
	CEdLakeDiss(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakeDiss)
	enum { IDD = IDD_EDIT_COMPLAKEDISS };
	CComboBox       m_le_var;
	CEdit   m_ed_sedexch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeDiss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeDiss)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeInits dialog

class CEdLakeInits : public CDialog
{
// Construction
public:
	CEdLakeInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakeInits)
	enum { IDD = IDD_EDIT_COMPLAKEINITS };
	CListBox        m_lb_inits;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeInit dialog

class CEdLakeInit : public CDialog
{
// Construction
public:
	CEdLakeInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakeInit)
	enum { IDD = IDD_EDIT_COMPLAKEINIT };
	CStatic m_tx_zone;
	CComboBox       m_le_zone;
	CComboBox       m_le_var;
	CEdit   m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSelInp dialog

class CEdLakeSelInp : public CDialog
{
// Construction
public:
	CEdLakeSelInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdLakeSelInp)
	enum { IDD = IDD_EDIT_COMPLAKESELINP };
	CButton m_rb_surfinp;
	CButton m_rb_sedinp;
	CButton m_rb_pointinps;
	CButton m_rb_latinp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSelInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSelInp)
	afx_msg void OnRbLatinp();
	afx_msg void OnRbPointinps();
	afx_msg void OnRbSedinp();
	afx_msg void OnRbSurfinp();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSurfInps dialog

class CEdLakeSurfInps : public CDialog
{
// Construction
public:
	CEdLakeSurfInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakeSurfInps)
	enum { IDD = IDD_EDIT_COMPLAKESURFINPS };
	CListBox        m_lb_inputs;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSurfInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSurfInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSurfInp dialog

class CEdLakeSurfInp : public CDialog
{
// Construction
public:
	CEdLakeSurfInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakeSurfInp)
	enum { IDD = IDD_EDIT_COMPLAKESURFINP };
	CComboBox       m_le_var;
	CEdit   m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSurfInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSurfInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeLatInps dialog

class CEdLakeLatInps : public CDialog
{
// Construction
public:
	CEdLakeLatInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakeLatInps)
	enum { IDD = IDD_EDIT_COMPLAKELATINPS };
	CListBox        m_lb_inputs;
	CEdit   m_ed_watinf;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeLatInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeLatInps)
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

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeLatInp dialog

class CEdLakeLatInp : public CDialog
{
// Construction
public:
	CEdLakeLatInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakeLatInp)
	enum { IDD = IDD_EDIT_COMPLAKELATINP };
	CComboBox       m_le_var;
	CEdit   m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeLatInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeLatInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedInps dialog

class CEdLakeSedInps : public CDialog
{
// Construction
public:
	CEdLakeSedInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakeSedInps)
	enum { IDD = IDD_EDIT_COMPLAKESEDINPS };
	CListBox        m_lb_inputs;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSedInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSedInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedInp dialog

class CEdLakeSedInp : public CDialog
{
// Construction
public:
	CEdLakeSedInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakeSedInp)
	enum { IDD = IDD_EDIT_COMPLAKESEDINP };
	CComboBox       m_le_var;
	CEdit   m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSedInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSedInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInps dialog

class CEdLakePointInps : public CDialog
{
// Construction
public:
	CEdLakePointInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakePointInps)
	enum { IDD = IDD_EDIT_COMPLAKEPOINTINPS };
	CListBox        m_lb_pinps;
	CButton m_bu_edit;
	CButton m_bu_delete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakePointInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakePointInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnSelchangeLbPinps();
	afx_msg void OnDblclkLbPinps();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInp dialog

class CEdLakePointInp : public CDialog
{
// Construction
public:
	CEdLakePointInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, PINF* p);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdLakePointInp)
	enum { IDD = IDD_EDIT_COMPLAKEPOINTINP };
	CListBox        m_lb_inputs;
	CEdit   m_ed_watinf;
	CEdit   m_ed_range;
	CEdit   m_ed_name;
	CEdit   m_ed_descript;
	CEdit   m_ed_depth;
	CButton m_bu_edit;
	CButton m_bu_delete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakePointInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakePointInp)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInputs();
	afx_msg void OnDblclkLbInputs();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
	PINF*     oldpinf;
	PINF*     pinf;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakePointInpConc dialog

class CEdLakePointInpConc : public CDialog
{
// Construction
public:
	CEdLakePointInpConc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(PINF* p, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdLakePointInpConc)
	enum { IDD = IDD_EDIT_COMPLAKEPOINTINPCONC };
	CComboBox       m_le_var;
	CEdit   m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakePointInpConc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakePointInpConc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	PINF* pinf;
	BOOL  add;
	int   pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeAcc dialog

class CEdLakeAcc : public CDialog
{
// Construction
public:
	CEdLakeAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdLakeAcc)
	enum { IDD = IDD_EDIT_COMPLAKEACC };
	CEdit	m_ed_relacci;
	CEdit	m_ed_absacci;
	CEdit	m_ed_relacceseiche;
	CEdit	m_ed_absacceseiche;
	CEdit	m_ed_relaccu;
	CEdit	m_ed_relaccq;
	CEdit	m_ed_relaccn2;
	CEdit	m_ed_absaccq;
	CEdit	m_ed_absaccn2;
	CEdit	m_ed_absacctke;
	CEdit	m_ed_relacctke;
	CEdit	m_ed_absaccu;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP* lcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdLakeSedLayer dialog

class CEdLakeSedLayer : public CDialog
{
// Construction
public:
	CEdLakeSedLayer(CWnd* pParent = NULL);   // standard constructor

	int DoModal(LAKECOMP* com, BOOL a, int i);

	// Dialog Data
	//{{AFX_DATA(CEdLakeSedLayer)
	enum { IDD = IDD_EDIT_COMPLAKESEDLAYER };
	CEdit	m_ed_zind;
	CEdit	m_ed_thick;
	CEdit	m_ed_por;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdLakeSedLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdLakeSedLayer)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LAKECOMP*     lcomp;
	LAKESEDLAYER* sedlayer;
	BOOL          add;
	int           pos;
};


#endif

