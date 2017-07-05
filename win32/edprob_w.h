// edprob_w.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdVarProbe dialog

class CEdVarProbe : public CDialog
{
// Construction
public:
	CEdVarProbe(CWnd* pParent = NULL);   // standard constructor

	int  DoModal(AQVAR* var, BOOL a);
	void UpdateZoneList();

// Dialog Data
	//{{AFX_DATA(CEdVarProbe)
	enum { IDD = IDD_EDIT_VARPROBE };
	CButton	m_cb_spacerel;
	CEdit	m_ed_unit;
	CEdit	m_ed_name;
	CEdit	m_ed_descript;
	CComboBox	m_le_var;
	CComboBox	m_le_comp;
	CStatic	m_tx_comp;
	CComboBox	m_le_zone;
	CStatic	m_tx_zone;
	CEdit	m_ed_space;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdVarProbe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdVarProbe)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCloseupLeComp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	AQVAR*    oldvar;
	PROBEVAR* probevar;
	BOOL      add;
};
