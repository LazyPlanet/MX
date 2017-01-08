// NetControl.cpp: implementation of the NetControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Program.h"
#include "NetControl.h"
#include "Net.h"
#include "Getin.h"
#include "Server.h"
#include "Managers.h"
#include "Num.h"

#include "ProgramDoc.h"
#include "ProgramView.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern Managers manager;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
NetControl::NetControl()
{
	manager.pControl=this;
	m_pSocket=NULL;
	m_Server="";
	m_Port=100;
	m_pListening=NULL;
}
NetControl::~NetControl()
{
	if(m_pSocket)
	delete m_pSocket;
	delete m_pListening;
}
void NetControl::FetchMsg(CNet *pRequest)
{
	CString pMsg;//用CString代替Char * 方便
	char Msg[10000],tempMsg[1000];
	int ByteCount;
	int End=0;
	CStringArray *temp=new CStringArray;
	strcpy(Msg,"");
	do
	{
		strcpy(tempMsg,"");
		ByteCount=pRequest->Receive(tempMsg,1000);
		if(ByteCount>1000||ByteCount<=0)
		{
			MessageBox(GetActiveWindow(),"接受网络信息发生错误","警告信息",MB_OK);
			return ;
        }
		else if(ByteCount<1000&&ByteCount>0)
		{
			End=1;
		}
		tempMsg[ByteCount]=0;
		strcat(Msg,tempMsg);
	}while(End==0);
	if(!strcmp("退出游戏",Msg))
	{
		MessageBox(GetActiveWindow(),"有人退出了","警告信息",MB_OK);
		if(m_Isserver)
			Broadcast(Msg);
		exit(0);
	}
	if(!m_Isserver)
	{
		if(!strcmp(Msg,sMsg))
			return ;
	}

	if(strncmp(Msg,"majiang",7))
		return;
	pMsg=Msg+7;
	if(m_Isserver)//服务器的处理
	{	
		if(!strncmp(pMsg,"start",5) && (manager.Game_State<0 || manager.Game_State>3))//收到准备消息
		{
			m_ready[pMsg[5]-100]=true;
			int k;
			for(k=0;k<4;k++)
				if(!m_ready[k])
					break;
			if(k==4)
			{
				//游戏开始
				char st[1000]="majiangstcad";
				do
				{
				manager.GameStart();
				
				MakeStartCards(st+12);
				int n2=strlen(st);
				if(strlen(st)!=122)
				{
					MessageBox(GetActiveWindow(),st,"sss",1);
				//	return;
				}
				}
				while (strlen(st)!=122);
				Broadcast(st);
			}
			m_pView->PrintAll();
		}
		if(!strncmp(pMsg,"names",5))//收到名字信息
		{
			int k=pMsg[5]-100;
			m_pView->PlayerNames[k].Format("%s",Msg+13);
			m_pView->PrintAll();

			
			char names[400];
			sprintf(names,"majiangnames%s%c%s%c%s%c%s%c",m_pView->PlayerNames[0],1,m_pView->PlayerNames[1],
				1,m_pView->PlayerNames[2],1,m_pView->PlayerNames[3],1);
			Broadcast(names);

		}
		if(!strncmp(pMsg,"doing",5))//收到游戏中的信息
		{
			
			Broadcast(Msg,pRequest);
DOMSG:
			int player,action;
			Card ca;
			player=pMsg[5]-100;
			action=pMsg[6]-100;
			if(pMsg.GetLength()>7 && action!=9)
			{
				ca.Num=pMsg[7]%10;
				ca.Type=pMsg[7]/10;
			}
			else
				ca.Num=10;
			manager.Game_State=player;
			if(action==1)
			{
				manager.Game_State=player;
				manager.Updata();
			}
			else if(action==2)
			{
				manager.PlayOuts[player][manager.Outsnum[player]]=ca;
				manager.Outsnum[player]++;
				manager.PlayOuts[player][manager.Outsnum[player]].Num=10;
				manager.OutCard=ca;
				manager.OutPlayer=player;	
				int num=manager.SearchCard(manager.PlayCards[player],ca);
				manager.Carddelete(num,manager.PlayCards[player],1);
				manager.SortCard(manager.PlayCards[player]);
				manager.PlayCards[player][13].Num=10;
				manager.Updata();
			}
			else if(action==3)
			{
				//不准许“吃”
			}
			else if(action==4)//碰
			{
				ca.Click=true;
				manager.Cardinsert(ca,manager.PushCards[player],3);
				int k=manager.SearchCard(manager.PlayCards[player],ca);
				manager.Carddelete(k,manager.PlayCards[player],2);
				manager.OutCard.Num=10;
				manager.Outsnum[manager.OutPlayer]--;
				manager.PlayOuts[manager.OutPlayer][manager.Outsnum[manager.OutPlayer]].Num=10;
				manager.PointPlayer=player;
				
			}
			else if(action==5)//用别人的牌杠
			{
				ca.Click=true;
				manager.Cardinsert(ca,manager.PushCards[player],4);
				int k=manager.SearchCard(manager.PlayCards[player],ca);
				manager.Carddelete(k,manager.PlayCards[player],3);
				manager.OutCard.Num=10;
				manager.PlayOuts[manager.OutPlayer][manager.Outsnum[manager.OutPlayer]-1].Num=10;
				manager.Outsnum[manager.OutPlayer]--;
				//杠了嘛，就得多摸牌
				manager.leftstart++;
				manager.leftstart%=108;
				manager.Power[player]*=2;


				if(manager.leftstart==manager.leftend)
					manager.Game_State=10;//牌摸完了 强制结束游戏
				else
				{
					manager.PlayCards[player][13]=manager.Sendleft[manager.LeftNum-1];
					manager.LeftNum--;
				}
			}
			else if(action==6)//用自己摸的牌暗杠
			{
				ca.Click=false;
				manager.Cardinsert(ca,manager.PushCards[player],4);
				int k=manager.SearchCard(manager.PlayCards[player],ca);
				manager.PlayCards[player][13].Num=10;	
				manager.Carddelete(k,manager.PlayCards[player],3);
				//杠了嘛，就得多摸牌
				manager.leftstart++;
				manager.leftstart%=108;
				manager.Power[player]*=2;
				if(manager.leftstart==manager.leftend)
					manager.Game_State=10;//牌摸完了 强制结束游戏
				else
				{
					manager.PlayCards[player][13]=manager.Sendleft[manager.LeftNum-1];
					manager.LeftNum--;
				}
			}
			else if(action==7)//用自己本来就有的牌暗杠
			{
				ca.Click=false;
				manager.Cardinsert(ca,manager.PushCards[player],4);
				int k=manager.SearchCard(manager.PlayCards[player],ca);
				manager.SortCard(manager.PlayCards[player]);
				
				manager.Carddelete(k,manager.PlayCards[player],4);
				//杠了嘛，就得多摸牌
				manager.leftstart++;
				manager.leftstart%=108;
				manager.Power[player]*=2;

				if(manager.leftstart==manager.leftend)
					manager.Game_State=10;//牌摸完了 强制结束游戏
				else
				{
					manager.PlayCards[player][13]=manager.Sendleft[manager.LeftNum-1];
					manager.LeftNum--;
				}
			}
			else if(action==8)//补花
			{
				ca.Click=true;
				manager.Cardinsert(ca,manager.PushCards[player],1);
				int m=manager.SearchCard(manager.PlayCards[player],ca);
				manager.Carddelete(m,manager.PlayCards[player],1);
				manager.SortCard(manager.PlayCards[player]);
				//杠了嘛，就得多摸牌
				manager.leftstart++;
				manager.leftstart%=108;
				manager.Power[player]*=2;

				if(manager.leftstart==manager.leftend)
					manager.Game_State=10;//牌摸完了 强制结束游戏
				else
				{
					manager.PlayCards[player][13]=manager.Sendleft[manager.LeftNum-1];
					manager.LeftNum--;
				}
			}
			else if(action==9)
			{
				CNum	score;
				if(manager.OutCard.Num<10 && manager.OutPlayer==manager.PlayerAc)
					score.m_cstring.Format("你点炮了");
				else
					score.m_cstring.Format("有人和牌了");
				manager.GameOver(pMsg[7]);
				score.m_player1=m_pView->PlayerNames[(4-manager.PlayerAc)%4];
				score.m_player2=m_pView->PlayerNames[(5-manager.PlayerAc)%4];
				score.m_player3=m_pView->PlayerNames[(6-manager.PlayerAc)%4];
				score.m_player4=m_pView->PlayerNames[(7-manager.PlayerAc)%4];
				score.m_score1.Format("%d",manager.PlayPoints[0]);
				score.m_score2.Format("%d",manager.PlayPoints[1]);
				score.m_score3.Format("%d",manager.PlayPoints[2]);
				score.m_score4.Format("%d",manager.PlayPoints[3]);
				score.DoModal();
			}
			m_pView->PrintAll();
			manager.PlaySound(action);
			return;
		}

	}
	else//非服务器的处理
	{
		if(!strncmp(pMsg,"hello",5))// 收到欢迎信息
		{
			manager.PlayerAc=pMsg[5]-100;
			char st[100]="majiangnames";
			sprintf(st+12,"%c%s",manager.PlayerAc+100,m_pView->PlayerNames[0]);
			SendMsg(m_pSocket,st);
			m_pView->PrintAll();
		}
		else if(!strncmp(pMsg,"names",5))
		{
			int i,j,k;
			j=5;
			i=0;
			for(k=5;k<pMsg.GetLength();k++)// 收到名字信息
				if(pMsg[k]==1)
				{
					if(k>j)
						m_pView->PlayerNames[(i+4-manager.PlayerAc)%4].Format("%s",pMsg.Mid(j,k-j));
					j=k+1;
					i++;
				}
			m_pView->PrintAll();
		}
		else if(!strncmp(pMsg,"stcad",5))// 收到发牌信息
		{
			Card te;
			int i,j,k;
			j=5;
			i=0;
			manager.GameStart();
			for(k=5;k<57;k++)
			{
				DeMakeCard(te,pMsg[k]);
				manager.PlayCards[(k-5)/13][(k-5)%13]=te;
				manager.PlayCards[(k-5)/13][(k-5)%13].Click=false;
			}
			for(k=0;k<4;k++)
				manager.PlayCards[k][13].Num=10;
			DeMakeCard(te,pMsg[57]);
			if(manager.LastWiner==manager.PlayerAc)
			{
				manager.Actions=manager.Cardinfo(manager.PlayCards[manager.LastWiner],
				manager.PushCards[manager.LastWiner],manager.LastWiner,te,manager.LastWiner);
			}
			else
				manager.Actions=0;
			manager.PlayCards[manager.LastWiner][13]=te;
			manager.PlayCards[manager.LastWiner][14].Num=10;
			manager.SortCard(manager.PlayCards[manager.LastWiner]);
			for(k=58;k<113;k++)
			{
				DeMakeCard(te,pMsg[k]);
				manager.Sendleft[k-58]=te;
			}
			manager.leftstart=(pMsg[113]-1+28*manager.PlayerAc)%108;
			manager.leftend=(pMsg[114]-1+28*manager.PlayerAc)%108;
			m_pView->PrintAll();
		}
		else if(!strncmp(pMsg,"doing",5))//收到游戏中的信息,处理方法和上面一样用GOTO代替
			goto DOMSG;
	}

}

void NetControl::OnSet()
{
	m_pListening=new CNet(this);
	if(m_pListening->Create(m_Port))
	{
		if(!m_pListening->Listen())
		{
			MessageBox(GetActiveWindow(),"端口设置错误!","网络信息",MB_OK);
		}
	}
}

void NetControl::SendMsg(CNet *pSend, char *pMsg)
{
	sMsg=pMsg;
	if(pMsg!=NULL && pSend!=NULL)
		pSend->Send(pMsg,strlen(pMsg));
}
  
void NetControl::OnSorc()
{
	int k;
	k=MessageBox(GetActiveWindow(),"你是否建立主机？（主机/客户机 Y/N）","建立主机",MB_OKCANCEL);
	if(k==1)//主机
	{
		CServer ser1;
		ser1.DoModal();
		m_Port=atoi(ser1.m_Port);
		OnSet();
		m_Isserver=true;
		manager.MainComputer=true;
		//manager
	}
	else
	{
		CGetin dlg1;
		if(dlg1.DoModal()==IDOK)
		{
			CString *p=new(CString);
			*p=dlg1.m_Ip;
			m_Server=*p;
			m_Port=atoi(dlg1.m_Port);
			OnOpen();
			m_Isserver=false;
			manager.MainComputer=false;
		}
	}
}

CNet* NetControl::ConnectServer()
{
	CNet *pSocket=new CNet(this);
	if(!(pSocket->Create()))
	{
		delete pSocket;
		MessageBox(GetActiveWindow(),"创建Socket失败","警告信息",MB_OK);
        return NULL;
	}
	//连接到服务器
	if(!pSocket->Connect(m_Server,m_Port))
	{
		delete pSocket;
		MessageBox(GetActiveWindow(),"请求连接到服务器失败","警告信息",MB_OK);
        return NULL;
	}
	return pSocket;
}

void NetControl::OnOpen()
{
	if(m_pSocket)
	{
		MessageBox(GetActiveWindow(),"已经连接到服务器,请关闭连接再试一次!","警告信息",MB_OK);
		return;
	}
	
//	m_pSocket=new CNet(this);
	if(!(m_pSocket=ConnectServer()))
	{
		m_pSocket=NULL;
		return;
	}
	else//连接成功就发送信息到服务器，获得服务器上的信息
	{

	}
}



void NetControl::Broadcast(char *pMsg,CNet *pl)
{
	if(pl!=pLink1)
		SendMsg(pLink1,pMsg);
	if(pl!=pLink2)
		SendMsg(pLink2,pMsg);
	if(pl!=pLink3)
		SendMsg(pLink3,pMsg);
}

NetControl::NetControl(CProgramView *p)
{
	Linknum=0;
	manager.pControl=this;
	m_pView=p;
	m_pSocket=NULL;
	m_Server="";
	m_Port=100;
	m_pListening=NULL;
	m_ready[0]=false;
	m_ready[1]=false;
	m_ready[2]=false;
	m_ready[3]=false;
	pLink1=NULL;
	pLink2=NULL;
	pLink3=NULL;
}





void NetControl::DeMakeCard(Card &ca , const char ch)
{
	ca.Type=ch/10;
	ca.Num=ch%10;
}

void NetControl::SendCard(int action,Card *ca)
{
	char st[100]="majiangdoing";
	char *p=st+12;
	p[0]=manager.PlayerAc+100;
	p[1]=action+100;
	if(ca!=NULL)
	{	
		p[2]=ca->Type*10+ca->Num;
		p[3]='\0';
	}
	else
		p[2]='\0';
	SendMsg(m_pSocket,st);
		
}

void NetControl::SendGetReady()
{
	char tem[200];
	sprintf(tem,"majiangstart%c",manager.PlayerAc+100);
	SendMsg(m_pSocket,tem);
}

void NetControl::MakeStartCards(char st[])
{	
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<13;j++)
			st[i*13+j]=manager.PlayCards[i][j].Type*10+manager.PlayCards[i][j].Num;
	st[52]=manager.PlayCards[manager.LastWiner][13].Type*10+manager.PlayCards[manager.LastWiner][13].Num;
	for(i=53;i<108;i++)
		st[i]=manager.Sendleft[i-53].Num+manager.Sendleft[i-53].Type*10;
	st[i]=manager.leftstart+1;
	st[i+1]=manager.leftend+1;
	st[i+2]='\0';
}

void NetControl::SendHello(CNet *p)
{
	char st[100];
	int player;
	if(p==pLink1)
		player=1;
	else if(p==pLink2)
		player=2;
	else
		player=3;
	sprintf(st,"majianghello%c",player+100);
	SendMsg(p,st);
}

void NetControl::BroadcastCard(int action, Card *ca, CNet *pl)
{
	char st[100]="majiangdoing";
	char *p=st+12;
	p[0]=manager.PlayerAc+100;
	p[1]=action+100;
	if(ca!=NULL)
	{	
		p[2]=ca->Type*10+ca->Num;
		p[3]='\0';
	}
	else
		p[2]='\0';
	Broadcast(st,pl);
}
