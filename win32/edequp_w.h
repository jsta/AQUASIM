// edequp_w.h : header file
//

#ifndef edequp_w_h
#define edequp_w_h

/////////////////////////////////////////////////////////////////////////////
// CEdProcEqu dialog

class CEdProcEqu : public CDialog
{
// Construction
public:
	CEdProcEqu(CWnd* pParent = NULL);   // standard constructor

	int DoModal(AQPROC* proc, BOOL a);

// Dialog Data
	//{{AFX_DATA(CEdProcEqu)
	enum { IDD = IDD_EDIT_PROCEQU };
	CComboBox	m_le_var;
	CEdit	m_ed_name;
	CEdit	m_ed_equation;
	CEdit	m_ed_descript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdProcEqu)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdProcEqu)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQPROC*  oldp;
	EQUPROC* equp;
	BOOL     add;
};

#endif