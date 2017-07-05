// edfilm_w.h : header file
//

#ifndef edfilm_w_h
#define edfilm_w_h

#include "./aqcore/filmcomp.h"

/////////////////////////////////////////////////////////////////////////////
// CEdFilm dialog

class CEdFilm : public CDialog
{
// Construction
public:
	CEdFilm(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQCOMP* com, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdFilm)
	enum { IDD = IDD_EDIT_COMPFILM };
	CButton	m_cb_active;
	CButton	m_rb_highres;
	CStatic	m_tx_volreact;
	CStatic	m_tx_volbulk;
	CButton	m_rb_unconfined;
	CButton	m_rb_porepart;
	CButton	m_rb_poreliq;
	CButton	m_rb_matrixrigid;
	CButton	m_rb_matrixdiff;
	CButton	m_rb_lowres;
	CButton	m_rb_detachindi;
	CButton	m_rb_detachglob;
	CButton	m_rb_confined;
	CEdit	m_ed_volreact;
	CEdit	m_ed_volbulk;
	CEdit	m_ed_ratepor;
	CEdit	m_ed_numgrid;
	CEdit	m_ed_detach;
	CButton	m_bu_dissvars;
	CEdit	m_ed_area;
	CButton	m_bu_partvars;
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
	//{{AFX_VIRTUAL(CEdFilm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilm)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuVars();
	afx_msg void OnBuProcs();
	afx_msg void OnBuInits();
	afx_msg void OnBuInp();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBuPartvars();
	afx_msg void OnBuDissvars();
	afx_msg void OnRbConfined();
	afx_msg void OnRbUnconfined();
	afx_msg void OnRbPoreliq();
	afx_msg void OnRbPorepart();
	afx_msg void OnRbMatrixrigid();
	afx_msg void OnRbMatrixdiff();
	afx_msg void OnRbDetachindi();
	afx_msg void OnRbDetachglob();
	afx_msg void OnRbLowres();
	afx_msg void OnRbHighres();
	afx_msg void OnBuAcc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQCOMP*   oldcomp;
	FILMCOMP* fcomp;
	BOOL      add;

	BOOLEAN   conf;
	BOOLEAN   matrixdiff;
	BOOLEAN   detachglobal;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInits dialog

class CEdFilmInits : public CDialog
{
// Construction
public:
	CEdFilmInits(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdFilmInits)
	enum { IDD = IDD_EDIT_COMPFILMINITS };
	CListBox	m_lb_inits;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmInits)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmInits)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbInits();
	afx_msg void OnDblclkLbInits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInit dialog

class CEdFilmInit : public CDialog
{
// Construction
public:
	CEdFilmInit(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdFilmInit)
	enum { IDD = IDD_EDIT_COMPFILMINIT };
	CStatic	m_tx_zone;
	CComboBox	m_le_zone;
	CComboBox	m_le_var;
	CEdit	m_ed_initcond;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmInit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmInit)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInps dialog

class CEdFilmInps : public CDialog
{
// Construction
public:
	CEdFilmInps(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdFilmInps)
	enum { IDD = IDD_EDIT_COMPFILMINPS };
	CListBox	m_lb_inputs;
	CEdit	m_ed_watinf;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmInps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmInps)
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

	FILMCOMP* fcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmInp dialog

class CEdFilmInp : public CDialog
{
// Construction
public:
	CEdFilmInp(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdFilmInp)
	enum { IDD = IDD_EDIT_COMPFILMINP };
	CComboBox	m_le_var;
	CEdit	m_ed_inpflux;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmInp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmInp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmParts dialog

class CEdFilmParts : public CDialog
{
// Construction
public:
	CEdFilmParts(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdFilmParts)
	enum { IDD = IDD_EDIT_COMPFILMPARTS };
	CListBox	m_lb_vars;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmParts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmParts)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuDelete();
	afx_msg void OnBuEdit();
	afx_msg void OnSelchangeLbVars();
	afx_msg void OnDblclkLbVars();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmPart dialog

class CEdFilmPart : public CDialog
{
// Construction
public:
	CEdFilmPart(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CEdFilmPart)
	enum { IDD = IDD_EDIT_COMPFILMPART };
	CStatic	m_tx_var;
	CComboBox	m_le_var;
	CEdit	m_ed_voldet;
	CEdit	m_ed_volatt;
	CEdit	m_ed_surfdet;
	CEdit	m_ed_surfatt;
	CEdit	m_ed_porediff;
	CEdit	m_ed_matrixdiff;
	CEdit	m_ed_density;
	CEdit	m_ed_boundres;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmPart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmPart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmDisss dialog

class CEdFilmDisss : public CDialog
{
// Construction
public:
	CEdFilmDisss(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com);

	void UpdateDialog();
	void UpdateActivation();

// Dialog Data
	//{{AFX_DATA(CEdFilmDisss)
	enum { IDD = IDD_EDIT_COMPFILMDISSS };
	CListBox	m_lb_vars;
	CButton	m_bu_edit;
	CButton	m_bu_delete;
	CButton	m_bu_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmDisss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmDisss)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuAdd();
	afx_msg void OnBuEdit();
	afx_msg void OnBuDelete();
	afx_msg void OnSelchangeLbVars();
	afx_msg void OnDblclkLbVars();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
};


/////////////////////////////////////////////////////////////////////////////
// CFilmDiss dialog

class CFilmDiss : public CDialog
{
// Construction
public:
	CFilmDiss(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com, BOOL a, int i);

// Dialog Data
	//{{AFX_DATA(CFilmDiss)
	enum { IDD = IDD_EDIT_COMPFILMDISS };
	CStatic	m_tx_var;
	CComboBox	m_le_var;
	CEdit	m_ed_porediff;
	CEdit	m_ed_boundres;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilmDiss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilmDiss)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
	BOOL      add;
	int       pos;
};


/////////////////////////////////////////////////////////////////////////////
// CEdFilmAcc dialog

class CEdFilmAcc : public CDialog
{
// Construction
public:
	CEdFilmAcc(CWnd* pParent = NULL);   // standard constructor

	int DoModal(FILMCOMP* com);

// Dialog Data
	//{{AFX_DATA(CEdFilmAcc)
	enum { IDD = IDD_EDIT_COMPFILMACC };
	CEdit	m_ed_absaccq;
	CEdit	m_ed_relaccz;
	CEdit	m_ed_relaccv;
	CEdit	m_ed_relaccq;
	CEdit	m_ed_relacceps;
	CEdit	m_ed_absaccz;
	CEdit	m_ed_absaccv;
	CEdit	m_ed_absacceps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdFilmAcc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdFilmAcc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	FILMCOMP* fcomp;
};


#endif

