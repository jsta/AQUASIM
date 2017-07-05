// WaitDlg.h : header file
//

#ifndef wait_w_h
#define wait_w_h

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog

class CWaitDlg : public CDialog
{
// Construction
public:
	CWaitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitDlg)
	enum { IDD = IDD_WAIT_CANCEL };
	CStatic m_wait_text;
	//}}AFX_DATA

// Functions
	void Display(const char* txt);
	void Remove();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CWaitSimDlg dialog

class CWaitSimDlg : public CDialog
{
// Construction
public:
	CWaitSimDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitSimDlg)
	enum { IDD = IDD_WAIT_SIM };
	CStatic	m_wait_calc;
	CStatic m_wait_time;
	CStatic m_wait_text;
	//}}AFX_DATA

// Functions
	void Display(const char* txt);
	void SetCalcname(char* name);
	void SetTime(REAL t);
	void Remove();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitSimDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitSimDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CWaitSensDlg dialog

class CWaitSensDlg : public CDialog
{
// Construction
public:
	CWaitSensDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitSensDlg)
	enum { IDD = IDD_WAIT_SENS };
	CStatic	m_wait_text;
	CStatic	m_wait_numsim;
	CStatic	m_wait_cursim;
	//}}AFX_DATA

// Functions
	void Display(const char* txt);
	void SetSim(CARDINAL num, CARDINAL cur);
	void Remove();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitSensDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitSensDlg)
	virtual void OnOK();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


/////////////////////////////////////////////////////////////////////////////
// CWaitFitDlg dialog

class CWaitFitDlg : public CDialog
{
// Construction
public:
	CWaitFitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitFitDlg)
	enum { IDD = IDD_WAIT_FIT };
	CStatic	m_wait_text;
	CStatic	m_wait_inichi;
	CStatic	m_wait_curchi;
	CStatic	m_wait_bestchi;
	//}}AFX_DATA

// Functions
	void Display(const char* txt);
	void SetChi(REAL inichi, REAL bestchi, REAL curchi);
	void Remove();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitFitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitFitDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int created;
};


#endif


