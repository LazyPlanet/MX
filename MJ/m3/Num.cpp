// Num.cpp : implementation file
//

#include "stdafx.h"
#include "program.h"
#include "Num.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNum dialog


CNum::CNum(CWnd* pParent /*=NULL*/)
	: CDialog(CNum::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNum)
	m_player1 = _T("");
	m_player2 = _T("");
	m_player3 = _T("");
	m_player4 = _T("");
	m_score1 = _T("");
	m_score2 = _T("");
	m_score3 = _T("");
	m_score4 = _T("");
	m_cstring = _T("");
	//}}AFX_DATA_INIT
}


void CNum::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNum)
	DDX_Text(pDX, IDC_PLAYER1, m_player1);
	DDX_Text(pDX, IDC_PLAYER2, m_player2);
	DDX_Text(pDX, IDC_PLAYER3, m_player3);
	DDX_Text(pDX, IDC_PLAYER4, m_player4);
	DDX_Text(pDX, IDC_SCORE1, m_score1);
	DDX_Text(pDX, IDC_SCORE2, m_score2);
	DDX_Text(pDX, IDC_SCORE3, m_score3);
	DDX_Text(pDX, IDC_SCORE4, m_score4);
	DDX_Text(pDX, IDC_CSTRING, m_cstring);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNum, CDialog)
	//{{AFX_MSG_MAP(CNum)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNum message handlers
