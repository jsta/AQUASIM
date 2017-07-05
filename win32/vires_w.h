// vires_w.h : header file
//

#ifndef vires_w_h
#define vires_w_h

/////////////////////////////////////////////////////////////////////////////
// CViRes dialog

class CViRes : public CDialog
{
// Construction
public:
	CViRes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViRes)
	enum { IDD = IDD_VIEW_RESULTS };
	CListBox        m_plotlist;
	CButton m_plottoscreen;
	CButton m_plottofile;
	CButton m_listtofile;
	CButton m_edit;
	CButton m_duplicate;
	CButton m_delete;
	//}}AFX_DATA

// Functions

	int Created()
	{ return created; }

	void UpdateDialog();
	void UpdateActivation();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViRes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViRes)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBuDelete();
	afx_msg void OnBuDuplicate();
	afx_msg void OnBuEdit();
	afx_msg void OnBuFileopt();
	afx_msg void OnBuListopt();
	afx_msg void OnBuNew();
	afx_msg void OnBuScropt();
	afx_msg void OnBuPlottoscreen();
	afx_msg void OnBuPlottofile();
	afx_msg void OnBuListtofile();
	afx_msg void OnSelchangeLbPlotlist();
	afx_msg void OnDblclkLbPlotlist();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int         created;
};


/////////////////////////////////////////////////////////////////////////////
// CViewScreenOpt dialog

class CViewScreenOpt : public CDialog
{
// Construction
public:
	CViewScreenOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewScreenOpt)
	enum { IDD = IDD_VIEW_SCREENOPT };
	CButton m_cascade;
	CEdit   m_winwidth;
	CEdit   m_winheight;
	CEdit   m_margintop;
	CEdit   m_marginright;
	CEdit   m_marginleft;
	CEdit   m_marginbottom;
	CEdit   m_legwidth;
	CEdit   m_fonttitle;
	CEdit   m_fontnumber;
	CEdit   m_fontlegend;
	CEdit   m_fontlabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewScreenOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewScreenOpt)
	afx_msg void OnDefaults();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCascade();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void        InitFields();

	PLOTSCROPT* scropt;
};


/////////////////////////////////////////////////////////////////////////////
// CViewPSOpt dialog

class CViewPSOpt : public CDialog
{
// Construction
public:
	CViewPSOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewPSOpt)
	enum { IDD = IDD_VIEW_PSOPT };
	CComboBox       m_papersize;
	CButton m_portrait;
	CButton m_landscape;
	CEdit   m_columns;
	CEdit   m_rows;
	CEdit   m_margintop;
	CEdit   m_marginright;
	CEdit   m_marginleft;
	CEdit   m_marginbottom;
	CEdit   m_legwidth;
	CEdit   m_fonttitle;
	CEdit   m_fontnumber;
	CEdit   m_fontlegend;
	CEdit   m_fontlabel;
	CButton m_color;
	CButton m_blackwhite;
	CButton m_cm;
	CButton m_inch;
	CButton m_file_pts;
	CButton m_file_eps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewPSOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewPSOpt)
	afx_msg void OnDefaults();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnRbBlackwhite();
	afx_msg void OnRbCm();
	afx_msg void OnRbColor();
	afx_msg void OnRbInch();
	afx_msg void OnRbLandscape();
	afx_msg void OnRbPortrait();
	afx_msg void OnRbPTS();
	afx_msg void OnRbEPS();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void        InitFields();

	PLOTFILOPT* filopt;
};


/////////////////////////////////////////////////////////////////////////////
// CViewListOpt dialog

class CViewListOpt : public CDialog
{
// Construction
public:
	CViewListOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewListOpt)
	enum { IDD = IDD_VIEW_LISTOPT };
	CEdit   m_digits;
	CButton m_septab;
	CButton m_sepcomma;
	CButton m_sepspace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewListOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewListOpt)
	virtual void OnOK();
	afx_msg void OnRBComma();
	afx_msg void OnRbSpace();
	afx_msg void OnRbTab();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void        InitFields();

	PLOTLISOPT* lisopt;
};



/////////////////////////////////////////////////////////////////////////////
// CViewEdPlotDef dialog

class CViewEdPlotDef : public CDialog
{
// Construction
public:
	CViewEdPlotDef(CWnd* pParent = NULL);   // standard constructor

	int     DoModal(PLOT* p, BOOL a);

// Dialog Data
	//{{AFX_DATA(CViewEdPlotDef)
	enum { IDD = IDD_VIEW_EDPLOTDEF };
	CEdit   m_name;
	CEdit   m_description;
	CEdit   m_title;
	CEdit   m_abscissalabel;
	CEdit   m_ordinatelabel;
	CButton m_absctime;
	CButton m_abscspace;
	CButton m_add;
	CButton m_edit;
	CButton m_delete;
	CListBox        m_curvelist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewEdPlotDef)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewEdPlotDef)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnScaling();
	afx_msg void OnDblclkCurvelist();
	afx_msg void OnSelchangeCurvelist();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRbAbscspace();
	afx_msg void OnRbAbsctime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void  UpdateActivation();
	void  UpdateCurvelist();

	PLOT* oldplot;
	PLOT* newplot;
	BOOL  add;
};


/////////////////////////////////////////////////////////////////////////////
// CViewEdCurveDef dialog

class CViewEdCurveDef : public CDialog
{
// Construction
public:
	CViewEdCurveDef(CWnd* pParent = NULL);   // standard constructor

	int     DoModal(PLOT* p, BOOL a, int i);
	void    UpdateZoneList();

	// Dialog Data
	//{{AFX_DATA(CViewEdCurveDef)
	enum { IDD = IDD_VIEW_EDCURVEDEF };
	CButton	m_cb_spacerel;
	CStatic m_tx_zone;
	CStatic m_tx_comp;
	CStatic m_tx_par;
	CComboBox       m_zone;
	CComboBox       m_var;
	CEdit   m_timespace;
	CComboBox       m_par;
	CComboBox       m_markerstyle;
	CEdit   m_markersize;
	CComboBox       m_markercolor;
	CButton m_markeract;
	CEdit   m_linewidth;
	CComboBox       m_linestyle;
	CComboBox       m_linecolor;
	CButton m_lineact;
	CComboBox       m_comp;
	CEdit   m_calcnum;
	CButton m_typevalue;
	CButton m_typeerror;
	CButton m_typesens;
	CButton m_sensabsabs;
	CButton m_sensrelabs;
	CButton m_sensabsrel;
	CButton m_sensrelrel;
	CEdit   m_legend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewEdCurveDef)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewEdCurveDef)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnRbAbsabs();
	afx_msg void OnRbAbsrel();
	afx_msg void OnRbError();
	afx_msg void OnRbRelabs();
	afx_msg void OnRbRelrel();
	afx_msg void OnRbSens();
	afx_msg void OnRbValue();
	afx_msg void OnCloseupComp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	PLOT*  plot;
	CURVE* curve;
	BOOL   add;
	int    pos;
	int    sensmeth;
};


/////////////////////////////////////////////////////////////////////////////
// CViewEdScaling dialog

class CViewEdScaling : public CDialog
{
// Construction
public:
	CViewEdScaling(CWnd* pParent = NULL);   // standard constructor

	int     DoModal(PLOT* p);

// Dialog Data
	//{{AFX_DATA(CViewEdScaling)
	enum { IDD = IDD_VIEW_EDSCALING };
	CButton m_cb_abscmax;
	CButton m_cb_absctdist;
	CButton m_cb_abscmin;
	CEdit   m_ed_absctdist;
	CEdit   m_ed_ordtpos;
	CEdit   m_ed_ordtdist;
	CEdit   m_ed_ordmin;
	CEdit   m_ed_ordmax;
	CEdit   m_ed_absctpos;
	CEdit   m_ed_abscmin;
	CEdit   m_ed_abscmax;
	CButton m_cb_ordtpos;
	CButton m_cb_ordtdist;
	CButton m_cb_ordmin;
	CButton m_cb_ordmax;
	CButton m_cb_absctpos;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewEdScaling)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewEdScaling)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	PLOT* plot;
};


/////////////////////////////////////////////////////////////////////////////
// CViewEdScalings dialog

class CViewEdScalings : public CDialog
{
// Construction
public:
	CViewEdScalings(CWnd* pParent = NULL);   // standard constructor

	int     DoModal(int n, int* ind);

	// Dialog Data
	//{{AFX_DATA(CViewEdScalings)
	enum { IDD = IDD_VIEW_EDSCALINGS };
	CButton	m_cb_ordtpos;
	CButton	m_cb_ordtdist;
	CButton	m_cb_ordmin;
	CButton	m_cb_ordmax;
	CButton	m_cb_absmax;
	CButton	m_cb_absctpos;
	CButton	m_cb_absctdist;
	CButton	m_cb_abscmin;
	CEdit	m_ed_abscmax;
	CEdit	m_ed_ordtpos;
	CEdit	m_ed_ordtdist;
	CEdit	m_ed_ordmin;
	CEdit	m_ed_ordmax;
	CEdit	m_ed_absctpos;
	CEdit	m_ed_absctdist;
	CEdit	m_ed_abscmin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewEdScalings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewEdScalings)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int num;
	int* indices;
};


#endif


