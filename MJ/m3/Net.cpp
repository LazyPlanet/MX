// Net.cpp : implementation file
//

#include "stdafx.h"
#include "Program.h"
#include "Net.h"
#include "MainFrm.h"
#include "NetControl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNet

CNet::CNet()
{
}

CNet::~CNet()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CNet, CSocket)
	//{{AFX_MSG_MAP(CNet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CNet member functions

CNet::CNet(NetControl *m_Ncc)
{
	m_Nc=m_Ncc;
}

void CNet::OnAccept(int nErrorCode)
{
	CNet *pSocket=new CNet(m_Nc);
	if(m_Nc->Linknum<3 && m_Nc->m_pListening->Accept(*pSocket))
	{
		if(m_Nc->pLink1==NULL)
			m_Nc->pLink1=pSocket;
		else if(m_Nc->pLink2==NULL)
			m_Nc->pLink2=pSocket;
		else if(m_Nc->pLink3==NULL)
			m_Nc->pLink3=pSocket;
		else 
		{
			delete pSocket;
			return;
		}
		m_Nc->Linknum++;
		m_Nc->SendHello(pSocket);
	}
	else 
		delete pSocket;
	
	CSocket::OnAccept(nErrorCode);
}

void CNet::OnReceive(int nErrorCode)
{
	m_Nc->FetchMsg(this);
	
	CSocket::OnReceive(nErrorCode);
}
