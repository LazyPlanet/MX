#if !defined(AFX_NUM_H__2631120D_0E60_491D_80F6_C0B3C695AD4E__INCLUDED_)
#define AFX_NUM_H__2631120D_0E60_491D_80F6_C0B3C695AD4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Num.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNum dialog

class CNum : public CDialog
{
// Construction
public:
	CNum(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNum)
	enum { IDD = IDD_DIALOG5 };
	CString	m_player1;
	CString	m_player2;
	CString	m_player3;
	CString	m_player4;
	CString	m_score1;
	CString	m_score2;
	CString	m_score3;
	CString	m_score4;
	CString	m_cstring;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNum)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUM_H__2631120D_0E60_491D_80F6_C0B3C695AD4E__INCLUDED_)
