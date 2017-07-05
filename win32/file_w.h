// file_w.h : header file
//

#ifndef file_w_h
#define file_w_h

/////////////////////////////////////////////////////////////////////////////
// CFilePrintOpt dialog

class CFilePrintOpt : public CDialog
{
// Construction
public:
	CFilePrintOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilePrintOpt)
	enum { IDD = IDD_FILE_PRINTOPT };
	CButton	m_rb_short;
	CButton	m_rb_long;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePrintOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilePrintOpt)
	afx_msg void OnRbLong();
	afx_msg void OnRbShort();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL printshort;
};

#endif