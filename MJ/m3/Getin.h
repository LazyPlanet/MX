#if !defined(AFX_GETIN_H__68C52AB4_BFDA_4B6B_8832_6B98BE6D05E1__INCLUDED_)
#define AFX_GETIN_H__68C52AB4_BFDA_4B6B_8832_6B98BE6D05E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Getin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetin dialog

class CGetin : public CDialog
{
// Construction
public:
	CGetin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetin)
	enum { IDD = IDD_DIALOG2 };
	CString	m_Ip;
	CString	m_Port;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetin)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETIN_H__68C52AB4_BFDA_4B6B_8832_6B98BE6D05E1__INCLUDED_)
