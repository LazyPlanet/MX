// NetControl.h: interface for the NetControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETCONTROL_H__5FDC8688_52D4_4F84_B4B5_C4B081ABE48D__INCLUDED_)
#define AFX_NETCONTROL_H__5FDC8688_52D4_4F84_B4B5_C4B081ABE48D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Net.h"
#include "Card.h"


class CProgramView;

class NetControl  
{
public:
	void BroadcastCard(int action,Card *ca,CNet *pl=NULL);
	void SendHello(CNet *p);
	void MakeStartCards(char st[]);
	void SendGetReady();
	void SendCard(int action,Card *ca);
	NetControl(CProgramView *p);
	void Broadcast(char *pMsg,CNet *pl=NULL);
	void SendMsg(CNet *pSend, char *pMsg);
	void OnSorc();
	void OnSet();
	void OnOpen();
	void FetchMsg(CNet *pRequest);
	CNet* ConnectServer();
	CNet *pLink1,*pLink2,*pLink3;//����3̨�ͻ���
	int Linknum;
	NetControl();
	virtual ~NetControl();
	UINT m_Port;//�˿�
//	CString m_PassWord;
	CProgramView *m_pView;
	LPCTSTR m_Server;//����������
	CNet *m_pSocket;//��������
	CNet *m_pListening;//����Socket
	CString sMsg;
	bool m_Isserver;
	bool m_ready[4];
private:
	void DeMakeCard(Card &ca,const char ch);
};

#endif // !defined(AFX_NETCONTROL_H__5FDC8688_52D4_4F84_B4B5_C4B081ABE48D__INCLUDED_)
