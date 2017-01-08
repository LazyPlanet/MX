#if !defined(AFX_ENTER_H__34D493F6_3BA2_4FE9_80EA_9052E3EBE2D1__INCLUDED_)
#define AFX_ENTER_H__34D493F6_3BA2_4FE9_80EA_9052E3EBE2D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Enter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnter dialog

class CEnter : public CDialog
{
// Construction
public:
	CEnter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnter)
	enum { IDD = IDD_DIALOG3 };
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnter)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTER_H__34D493F6_3BA2_4FE9_80EA_9052E3EBE2D1__INCLUDED_)
