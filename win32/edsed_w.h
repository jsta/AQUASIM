// edsed_w.h : header file
//

#ifndef edsed_w_h
#define edsed_w_h

#include "./aqcore/sedcomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdSed dialog

class CEdSed : public CDialog
{
// Construction
public:
	CEdSed(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

	// Dialog Data
	//{{AFX_DATA(CEdSed)
	enum { IDD = IDD_EDIT_COMPSED };
	CButton	m_cb_active;
	CButton m_rb_lowres;
	CButton m_rb_highres;
	CEdit   m_ed_top;
	CEdit   m_ed_numgrid;
	CEdit   m_ed_name;
	CEdit   m_ed_descript;
	CEdit   m_ed_ratepor;
	CEdit   m_ed_compind;
	CEdit   m_ed_bottom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSed)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBuVars();
	afx_msg void OnBuProcs();
	afx_msg void OnBuInits();
	afx_msg void OnBuInp();
	afx_msg void OnRbLowres();
	afx_msg void OnRbHighres();
	afx_msg void OnBuPartvars();
	afx_msg void OnBuDissvars();
	afx_msg void OnBuAcc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*   oldcomp;
	SEDCOMP*  scomp;
	BOOL      add;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedParts dialog

class CEdSedParts : public CDialog
{
// Construction
public:
	CEdSedParts(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSedParts)
	enum { IDD = IDD_EDIT_COMPSEDPARTS };
	CListBox m_lb_partlist;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedParts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedParts)
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnSelchangeLbVars();
	afx_msg void OnDblclkLbVars();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedDisss dialog

class CEdSedDisss : public CDialog
{
// Construction
public:
	CEdSedDisss(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSedDisss)
	enum { IDD = IDD_EDIT_COMPSEDDISSS };
	CListBox        m_lb_disslist;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedDisss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedDisss)
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnDblclkLbVars();
	afx_msg void OnSelchangeLbVars();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedPart dialog

class CEdSedPart : public CDialog
{
// Construction
public:
	CEdSedPart(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSedPart)
	enum { IDD = IDD_EDIT_COMPSEDPART };
	CComboBox       m_le_var;
	CEdit   m_ed_bioturb;
	CEdit   m_ed_density;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedPart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedPart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedDiss dialog

class CEdSedDiss : public CDialog
{
// Construction
public:
	CEdSedDiss(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSedDiss)
	enum { IDD = IDD_EDIT_COMPSEDDISS };
	CComboBox       m_le_var;
	CEdit   m_ed_sedexch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedDiss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedDiss)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedInits dialog

class CEdSedInits : public CDialog
{
// Construction
public:
	CEdSedInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSedInits)
	enum { IDD = IDD_EDIT_COMPSEDINITS };
	CListBox        m_lb_inits;
	CButton m_bu_edit;
	CButton m_bu_delete;
	CButton m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedInit dialog

class CEdSedInit : public CDialog
{
// Construction
public:
	CEdSedInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSedInit)
	enum { IDD = IDD_EDIT_COMPSEDINIT };
	CStatic m_tx_zone;
	CComboBox       m_le_zone;
	CComboBox       m_le_var;
	CEdit   m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedInps dialog

class CEdSedInps : public CDialog
{
// Construction
public:
	CEdSedInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdSedInps)
	enum { IDD = IDD_EDIT_COMPSEDINPS };
	CEdit        m_ed_surfpor;
	CListBox        m_lb_topbcs;
	CButton m_bu_edittopbc;
	CButton m_bu_deletetopbc;
	CButton m_bu_addtopbc;
	CListBox        m_lb_bottombcs;
	CButton m_bu_editbottombc;
	CButton m_bu_deletebottombc;
	CButton m_bu_addbottombc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedInps)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAddTopBC();
	afx_msg void OnBuDeleteTopBC();
	afx_msg void OnBuEditTopBC();
	afx_msg void OnSelchangeLbTopBCs();
	afx_msg void OnDblclkLbTopBCs();
	afx_msg void OnBuAddBottomBC();
	afx_msg void OnBuDeleteBottomBC();
	afx_msg void OnBuEditBottomBC();
	afx_msg void OnSelchangeLbBottomBCs();
	afx_msg void OnDblclkLbBottomBCs();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedTopBC dialog

class CEdSedTopBC : public CDialog
{
// Construction
public:
	CEdSedTopBC(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSedTopBC)
	enum { IDD = IDD_EDIT_COMPSEDTOPBC };
	CComboBox       m_le_var;
	CEdit   m_ed_conc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedTopBC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedTopBC)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedBottomBC dialog

class CEdSedBottomBC : public CDialog
{
// Construction
public:
	CEdSedBottomBC(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdSedBottomBC)
	enum { IDD = IDD_EDIT_COMPSEDBOTTOMBC };
	CComboBox       m_le_var;
	CEdit   m_ed_conc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedBottomBC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedBottomBC)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
	BOOL     add;
	int      pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdSedAcc dialog

class CEdSedAcc : public CDialog
{
// Construction
public:
	CEdSedAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(SEDCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdSedAcc)
	enum { IDD = IDD_EDIT_COMPSEDACC };
	CEdit	m_ed_relaccq;
	CEdit	m_ed_absaccq;
	CEdit	m_ed_relaccpor;
	CEdit	m_ed_absaccpor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdSedAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdSedAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	SEDCOMP* scomp;
};


#endif
