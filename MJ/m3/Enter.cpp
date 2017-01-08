// Enter.cpp : implementation file
//

#include "stdafx.h"
#include "program.h"
#include "Enter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnter dialog


CEnter::CEnter(CWnd* pParent /*=NULL*/)
	: CDialog(CEnter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnter)
	m_name = _T("Â·ÈË¼×");
	//}}AFX_DATA_INIT
}


void CEnter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnter)
	DDX_Text(pDX, IDC_EDIT1, m_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnter, CDialog)
	//{{AFX_MSG_MAP(CEnter)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnter message handlers
