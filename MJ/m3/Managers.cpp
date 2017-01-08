// Managers.cpp: implementation of the Managers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "program.h"
#include "Managers.h"
#include "ProgramDoc.h"
#include "ProgramView.h"
#include "Num.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
bool operator ==(const Card &a,const Card &t)
{
	if(a.Type==t.Type && a.Num==t.Num)
		return true;
	return false;
}
/////////////////////////////////////////////////////////

Managers::Managers()
{
	srand(time(NULL));
	Game_State=-2;
	LastWiner=0;
	OutCard.Num=10;
	OutPlayer=4;
	Power[0]=1;
	Power[1]=1;
	Power[2]=1;
	Power[3]=1;
	PlayPoints[0]=100;
	PlayPoints[1]=100;
	PlayPoints[2]=100;
	PlayPoints[3]=100;
	PlayCards[0][13].Num=10;
	leftstart=-1;
	leftend=-1;
	PointPlayer=0;
	isPlayBkmusic = false;
}

Managers::~Managers()
{

}

void Managers::GameStart()
{
	leftstart=-1;
	leftend=-1;
	OutCard.Num=10;
	OutPlayer=(LastWiner+3)%4+1;
	int i,j;
	for(i=0;i<4;i++)
	{
		Outsnum[i]=0;
		for(j=0;j<20;j++)
		{
			PushCards[i][j].Num=10;
			PlayOuts[i][j].Num=10;
		}
	}
	Game_State=LastWiner;
	PointPlayer=LastWiner;
	Power[0]=1;
	Power[1]=1;
	Power[2]=1;
	Power[3]=1;
	LeftNum=55;
//	if(MainComputer)
	{
		SendCard();
		for(i=0;i<4;i++)
			SortCard(PlayCards[i]);
		leftstart=rand()%54*2;
		leftend=(leftstart+55)%108;
	}
	isPlayBkmusic = true;
}


////////////////////////////////////////////////////////////////////////
/*
* 函数名称：SendCard()
* 函数介绍：发牌，完成初始化各玩家的牌
* 输入参数：无
* 输出参数：无
* 返回值  ：无
*/
void Managers::SendCard()
{
	int x,y,k;
	bool cards[108]={false};
	for(x=0;x<4;x++)
	{
		for(y=0;y<13;y++)
		{
			do
			{
				k=rand()%108;
			}
			while(cards[k]);
			cards[k]=true;
			PlayCards[x][y].Num=k%9+1;
			PlayCards[x][y].Type=k/36;
			PlayCards[x][y].Click=false;
		}
	}
	PlayCards[0][13].Num=10;
	PlayCards[1][13].Num=10;
	PlayCards[2][13].Num=10;
	PlayCards[3][13].Num=10;

	do
	{
		k=rand()%108;
	}
	while(cards[k]);
	Card te;
	te.Num=k%9+1;
	te.Type=k/36;
	te.Click=false;
	if(LastWiner==PlayerAc)
		Actions=Cardinfo(PlayCards[LastWiner],PushCards[LastWiner],LastWiner,te,LastWiner);
	else
		Actions=0;
	PlayCards[LastWiner][13]=te;
	cards[k]=true;
	y=0;
	while(y<55)
	{
		do
		{
			k=rand()%108;
		}
		while(cards[k]);
		Sendleft[y].Num=k%9+1;
		Sendleft[y].Type=k/36;
		Sendleft[y].Click=false;
		y++;
		cards[k]=true;
	}
	Sendleft[y].Num=10;
}
////////////////////////////////////////////////////////////////////////
/*
* 函数名称：DoMSG(	int num		//第几的一张牌
					int action)	//行为
* 函数介绍：消息响应
* 输入参数：action含义 
	  1过 2 出牌 3吃 4碰 5杠 9和
* 输出参数：bool
* 返回值  ：true 表示响应成功 false为失败
*/
bool Managers::DoMsg(int num, int action)
{
	Card *ca=PlayCards[PlayerAc];
	if(Game_State==PlayerAc)
	{
		if(action==1 && OutCard.Num<=9 && OutPlayer!=PlayerAc && Actions!=0)
		{
			if(!MainComputer)
				pControl->SendCard(1,NULL);
			else
				pControl->BroadcastCard(1,NULL);
			Updata();	
			pView->PrintAll();
			PlaySound(action);
			return true;
		}

		if(action==2 && OutCard.Num==10 && Game_State==PlayerAc)
		{
			PlayOuts[PlayerAc][Outsnum[PlayerAc]]=ca[num];
			Outsnum[PlayerAc]++;
			PlayOuts[PlayerAc][Outsnum[PlayerAc]].Num=10;
			OutCard=ca[num];
			OutCard.Click=true;
			OutPlayer=PlayerAc;
			Carddelete(num,ca,1);
			SortCard(ca);
			PlayCards[PlayerAc][13].Num=10;
			if(!pControl->m_Isserver)
				pControl->SendCard(2,&OutCard);
			else
				pControl->BroadcastCard(2,&OutCard);
			Updata();	
			pView->PrintAll();
			PlaySound(action);
			return true;
		}
		if(action==3)
		{
			return false;//在这里，本麻将不准许“吃”
		}
		if(action==4 && Actions%10==1)//碰
		{
			int i;
			for(i=0;ca[i+1].Num<10 && i<14;i++)
				if(ca[i]==OutCard && ca[i+1]==OutCard)
				{
					char st[100];
					if(ca[i].Type==0)
						sprintf(st,"您是要碰%d条吗?",ca[i].Num);
					else if(ca[i].Type==1)
						sprintf(st,"您是要碰%d万吗?",ca[i].Num);
					else if(ca[i].Type==2)
						sprintf(st,"您是要碰%d饼吗?",ca[i].Num);
					int k;
					k=MessageBox(GetActiveWindow(),st,"询问",MB_OKCANCEL);
					if(k!=1)
						break;
					Card te=OutCard;
					OutCard.Click=true;

					Cardinsert(OutCard,PushCards[PlayerAc],3);
					Carddelete(i,ca,2);
					OutCard.Num=10;
					PlayOuts[OutPlayer][Outsnum[OutPlayer]-1].Num=10;
					Outsnum[OutPlayer]--;
					
					if(!MainComputer)
						pControl->SendCard(4,&te);
					else
						pControl->BroadcastCard(4,&te);
					PointPlayer=PlayerAc;
					Actions=0;	
					pView->PrintAll();
					PlaySound(action);
					return true;
				}
		}
		if(action==5 && Actions%100>=10)//杠在 这里分4种讨论，这会影响到后面的UpData
		{
			int i;
			for(i=0;ca[i+2].Num<10 && i<13;i++)//用别人出的牌杠
				if(ca[i]==OutCard && ca[i+1]==OutCard && ca[i+2]==OutCard)
				{
					char st[100];
					if(ca[i].Type==0)
						sprintf(st,"您是要杠%d条吗?",ca[i].Num);
					else if(ca[i].Type==1)
						sprintf(st,"您是要杠%d万吗?",ca[i].Num);
					else if(ca[i].Type==2)
						sprintf(st,"您是要杠%d饼吗?",ca[i].Num);
					int k;
					k=MessageBox(GetActiveWindow(),st,"询问",MB_OKCANCEL);
					if(k!=1)
						break;

					Card te=OutCard;
					OutCard.Click=true;

					Cardinsert(OutCard,PushCards[PlayerAc],4);
					Carddelete(i,ca,3);
					OutCard.Num=10;
					PlayOuts[OutPlayer][Outsnum[OutPlayer]-1].Num=10;
					Outsnum[OutPlayer]--;

					if(!MainComputer)
						pControl->SendCard(5,&te);
					else
						pControl->BroadcastCard(5,&te);

					LeftNum--;
					if(LeftNum!=0)
					{
						Actions=Cardinfo(ca,PushCards[Game_State],Game_State,Sendleft[LeftNum],Game_State);
						ca[13]=Sendleft[LeftNum];
						leftstart=(leftstart+1)%108;

						Power[PlayerAc]*=2;
						PointPlayer=PlayerAc;
					}
					else
						Game_State=10;	
					pView->PrintAll();
					PlaySound(action);
					return true;
				}
			for(i=0;ca[i+2].Num<10 && i<13;i++)//自己摸的牌暗杠
				if(ca[i]==ca[13] && ca[i+1]==ca[13] && ca[i+2]==ca[13])
				{
					char st[100];
					if(ca[i].Type==0)
						sprintf(st,"您是要杠%d条吗?",ca[i].Num);
					else if(ca[i].Type==1)
						sprintf(st,"您是要杠%d万吗?",ca[i].Num);
					else if(ca[i].Type==2)
						sprintf(st,"您是要杠%d饼吗?",ca[i].Num);
					int k;
					k=MessageBox(GetActiveWindow(),st,"询问",MB_OKCANCEL);
					if(k!=1)
						break;

					Card te=ca[13];
					ca[13].Click=false;

					Cardinsert(ca[13],PushCards[PlayerAc],4);
					Carddelete(13,ca,1);
					Carddelete(i,ca,3);

					if(!MainComputer)
						pControl->SendCard(6,&te);
					else
						pControl->BroadcastCard(6,&te);

					if(LeftNum==0)
					{
						Game_State=10;
						if(MainComputer)
						{
							pControl->m_ready[0]=false;
							pControl->m_ready[1]=false;
							pControl->m_ready[2]=false;
							pControl->m_ready[3]=false;
						}
						return true;
					}
					LeftNum--;
					if(LeftNum!=0)
					{
						Actions=Cardinfo(ca,PushCards[Game_State],Game_State,Sendleft[LeftNum],Game_State);
						ca[13]=Sendleft[LeftNum];
						leftstart=(leftstart+1)%108;

						Power[PlayerAc]*=2;
						PointPlayer=PlayerAc;
					}
					else
						Game_State=10;	
					pView->PrintAll();
					PlaySound(action);
					return true;
				}
			for(i=0;ca[i+3].Num<10 && i<14;i++)//自己本来就有的牌暗杠
				if(ca[i]==ca[i+1] && ca[i]==ca[i+2] && ca[i]==ca[i+3])
				{
					char st[100];
					if(ca[i].Type==0)
						sprintf(st,"您是要杠%d条吗?",ca[i].Num);
					else if(ca[i].Type==1)
						sprintf(st,"您是要杠%d万吗?",ca[i].Num);
					else if(ca[i].Type==2)
						sprintf(st,"您是要杠%d饼吗?",ca[i].Num);
					int k;
					k=MessageBox(GetActiveWindow(),st,"询问",MB_OKCANCEL);
					if(k!=1)
						continue;
					Card te=ca[i];
					ca[i].Click=false;

					Cardinsert(ca[i],PushCards[PlayerAc],4);
					Carddelete(i,ca,4);

					if(!pControl->m_Isserver)
						pControl->SendCard(7,&te);
					else
						pControl->BroadcastCard(7,&te);

					if(LeftNum==0)
					{
						Game_State=10;
						if(MainComputer)
						{
							pControl->m_ready[0]=false;
							pControl->m_ready[1]=false;
							pControl->m_ready[2]=false;
							pControl->m_ready[3]=false;
						}
						return true;
					}
					LeftNum--;
					if(LeftNum!=0)
					{
						Actions=Cardinfo(ca,PushCards[Game_State],Game_State,Sendleft[LeftNum],Game_State);
						ca[13]=Sendleft[LeftNum];
						leftstart=(leftstart+1)%108;

						Power[PlayerAc]*=2;
						PointPlayer=PlayerAc;
					}
					else
						Game_State=10;
				//	OutCard.Num=10;		
					pView->PrintAll();
					PlaySound(action);	
					return true;
				}
			//补花(也是杠的一种)
			for(i=0;PushCards[PlayerAc][i+2].Num<10 && i<14;i++)
				if(PushCards[PlayerAc][i+2]==PushCards[PlayerAc][i] && PushCards[PlayerAc][i+2]==PushCards[PlayerAc][i+1])
				{
					if(PushCards[PlayerAc][i]==PushCards[PlayerAc][i+3])
					{
						i+=3;
						continue;
					}
					int j;
					for(j=0;ca[j].Num<10 && j<14;j++)
						if(ca[j]==PushCards[PlayerAc][i])
							break;
					if(ca[13]==PushCards[PlayerAc][i])
					{
						j=13;
					}
					else if(ca[j].Num>=10)
					{
						i+=2;
						continue;
					}
					char st[100];
					if(ca[j].Type==0)
						sprintf(st,"您是要杠%d条吗?",ca[j].Num);
					else if(ca[j].Type==1)
						sprintf(st,"您是要杠%d万吗?",ca[j].Num);
					else if(ca[j].Type==2)
						sprintf(st,"您是要杠%d饼吗?",ca[j].Num);
					int k;
					k=MessageBox(GetActiveWindow(),st,"询问",MB_OKCANCEL);
					if(k!=1)
					{
						i+=2;
						continue;
					}

					Card te=ca[j];
					te.Click=true;
					Cardinsert(te,PushCards[PlayerAc],1);
					Carddelete(j,ca,1);

					if(!pControl->m_Isserver)
						pControl->SendCard(8,&te);
					else
						pControl->BroadcastCard(6,&te);

					if(LeftNum==0)
					{
						Game_State=10;
						if(MainComputer)
						{
							pControl->m_ready[0]=false;
							pControl->m_ready[1]=false;
							pControl->m_ready[2]=false;
							pControl->m_ready[3]=false;
						}
						return true;
					}
					LeftNum--;
					if(LeftNum!=0)
					{
						Actions=Cardinfo(ca,PushCards[Game_State],Game_State,Sendleft[LeftNum],Game_State);
						ca[13]=Sendleft[LeftNum];
						leftstart=(leftstart+1)%108;

						Power[PlayerAc]*=2;
						PointPlayer=PlayerAc;
					}
					else
						Game_State=10;
				//	OutCard.Num=10;		
					pView->PrintAll();
					PlaySound(action);	
					return true;
				}
		}
		if(action==9 && Actions>=100)
		{		
			CNum	score;
			score.m_cstring.Format("恭喜你胜利了!");
			char st[100];
			sprintf(st,"majiangdoing%c%c%c",PlayerAc+100,109,(Actions/100));
			if(!MainComputer)
				pControl->SendMsg(pControl->m_pSocket,st);
			else
			{
				pControl->Broadcast(st,NULL);
			}
			GameOver(Actions/100);
			score.m_player1=pView->PlayerNames[(4-PlayerAc)%4];
			score.m_player2=pView->PlayerNames[(5-PlayerAc)%4];
			score.m_player3=pView->PlayerNames[(6-PlayerAc)%4];
			score.m_player4=pView->PlayerNames[(7-PlayerAc)%4];
			score.m_score1.Format("%d",PlayPoints[0]);
			score.m_score2.Format("%d",PlayPoints[1]);
			score.m_score3.Format("%d",PlayPoints[2]);
			score.m_score4.Format("%d",PlayPoints[3]);
			score.DoModal();	
			pView->PrintAll();
			PlaySound(action);
		}
	}
	return false;
}

void Managers::Updata()
{
	if(OutCard.Num==10)//没有人出牌，就等Game_State出牌咯 Game_State不改变
		return ;
	//现在有玩家OutPlayer出了牌，判断有没有玩家需要这牌,当然出牌的玩家是不会要这张牌了
	int i,j;
	for(i=(Game_State+1)%4;i!=OutPlayer;i=(i+1)%4)
	{
		j=Cardinfo(PlayCards[i],PushCards[i],OutPlayer,OutCard,i);
		if(j!=0)
		{
			Game_State=i;
			if(Game_State!=PlayerAc)
				Actions=0;
			else
				Actions=j;
			return;
		}
		
	}
	Game_State=(OutPlayer+1)%4;
	OutCard.Num=10;
	leftstart=(leftstart+1)%108;
	if(LeftNum==0)
	{
		Game_State=10;
		if(MainComputer)
		{
			pControl->m_ready[0]=false;
			pControl->m_ready[1]=false;
			pControl->m_ready[2]=false;
			pControl->m_ready[3]=false;
		}
		return;
	}

	if(Game_State==PlayerAc)
	{
		Actions=Cardinfo(PlayCards[Game_State],PushCards[Game_State],Game_State,Sendleft[LeftNum-1],Game_State);
	}
	else
	{
		Actions=0;
	}
	PlayCards[Game_State][13]=Sendleft[LeftNum-1];
	PlayCards[Game_State][13].Click=false;
	LeftNum--;
	PointPlayer=(OutPlayer+1)%4;
	OutCard.Num=10;
}
////////////////////////////////////////////////////////////////////////
/*
* 函数名称：SortCard(Card ca[])
* 函数介绍：理牌
* 输入参数：需要整理的牌数组
* 输出参数：无
* 返回值  ：无
*/
void Managers::SortCard(Card ca[])
{
	int i,j;
	Card temp;
	if(ca[13].Num<=9)
	{
		for(j=0;j<13;j++)
			if(ca[j].Num==10)
				break;
		ca[j]=ca[13];
		ca[j+1].Num=10;
	}
	for(i=0;i<14 && ca[i].Num<=9;i++)
		for(j=i+1;j<14 && ca[j].Num<=9;j++)
			if(ca[i].Type*10+ca[i].Num>ca[j].Type*10+ca[j].Num)
			{
				temp=ca[i];
				ca[i]=ca[j];
				ca[j]=temp;
			}
}

void Managers::Cardinsert(const Card inc, Card ca[],int num)
{
	int t,k;
	for(k=0;k<20 && ca[k].Num<=9;k++);
	for(t=k;t<k+num;t++)
		ca[t]=inc;
	SortCard(ca);
}

void Managers::Carddelete(int start, Card ca[],int num)
{
	int t;
	if(start==13)
	{	
		ca[13].Num=10;
		return;
	}
	for(t=start;t<20 && ca[t+num].Num<=9;t++)
		ca[t]=ca[t+num];
	ca[t].Num=10;
}


////////////////////////////////////////////////////////////////////////
/*
* 函数名称：Cardinfo(Card ca[],Card pushca[],//手上的和桌子上自己打的牌
			int inp, Card inc	//摸或者别人打出来的牌 以及摸或者打牌的人
			int player)			//ca所属于的玩家
* 函数介绍：获得牌信息
* 输入参数：略
* 输出参数：int
* 返回值  ：个位：时候能碰 十位 是否能杠，百位极以 如果能和牌那么他是多少翻
*/
int Managers::Cardinfo(Card ca[],Card pushca[],int inp, Card inc,int player)
{
	
	Card cad[14];//存储如果别人打的牌在用户手里会有什么结果
	int num;
	int state=0;
	int i,j;

	//复制所有牌，为判断和牌做准备
	for(num=0;ca[num].Num<10 && num<14;num++)
	{
		cad[num]=ca[num];
		cad[num].Click=false;
	}	
	ca[num].Num=10;
	//杠 如果是inc引起的杠
	j=0;
	for(i=0;ca[i+2].Num<=9;i++)
	{
		if(inc==ca[i] && inc==ca[i+1] && inc==ca[i+2]) 
		{
			state+=10;
			break;
		}
	}
	//杠 如果本身牌就有杠

	for(i=0;ca[i+3].Num<=9 && inp==player;i++)
	{
		if(ca[i]==ca[i+1] && ca[i]==ca[i+2] && ca[i]==ca[i+3]) 
		{
			state+=10;
			break;
		}
	}
	//杠 补花，即把自己的碰牌补充为杠
	for(i=0;pushca[i+2].Num<=9 && inp==player;i++)
	{
		if(pushca[i+2]==pushca[i] && pushca[i+2]==pushca[i+1]) 
		{
			if(pushca[i+3]==pushca[i])
			{
				i+=3;
				continue;
			}
		/*	if(inc==ca[i])
			{
				state+=10;
				break;
			}*/
			for (j=0;ca[j].Num<10 && j<14;j++)
				if(ca[j]==pushca[i])
				{
					state+=10;
					break;
				}
			if(inc==pushca[i])
			{
				state+=10;
				break;
			}
			i+=2;
		}
	}
	//碰
	{
		for(i=0;ca[i+1].Num<=9 && inp!=player;i++)
			if(inc==ca[i] && inc==ca[i+1])
			{
				state+=1;
				break;
			}
	}
	//吃
/*	{
		for(i=0;ca[i+1].Num<=9 && inp==(player+3)%4;i++)
			if(inc.Type==ca[i].Type && inc.Type==ca[i+1].Type &&
				((inc.Num==ca[i].Num-1 && inc.Num==ca[i+1].Num-2)|| 
				(inc.Num==ca[i].Num+1 && inc.Num==ca[i+1].Num-1)||
				(inc.Num==ca[i].Num+2 && inc.Num==ca[i+1].Num+1)))
			{
				state+=5;
				break;
			}
	}*/

	
	

	//和牌判断
	j=0;
	inc.Click=false;
	Cardinsert(inc,cad,1);//把inc插如牌序列
	num++;
	if (num==14)
	{
		j=0;
		//七对判断
		for(i=0;i<=num-1;i+=2)
		{
			if(cad[i].Num!=cad[i+1].Num || cad[i].Type!=cad[i+1].Type)
				break;
			if(cad[i].Num==cad[i+3].Num && cad[i].Type==cad[i+3].Type)
				j++;//龙七对
		}
		if(i==12)
		{
			//七对成功如果j>=1则龙七对成功 
			state+=300;
		}
	}

	i=0;
	i=GetNext(cad,num,true,cad[0].Type+1,true);
	if(i>0)
	{
		//和牌,或者准确点说叫可以和牌了
		state+=i*100;
	}
	
	return state;
}

int Managers::GetNext(Card *ca, int num,bool dou,int type,bool fu)
{
	int k,tem,i,j,temp;
	tem=0;
	k=0;
	if(num==0)
	{
		k=1;
		if(fu)//对子福
			k*=2;
		i=type%10;//判断是否是清一色
		while(type!=0)
		{	
			if(type%10!=i)
				break;
			type/=10;
		}
		if(type==0)//这也是
			k*=3;
	}
	else if(num>=2)
	{
		for(i=1;i<num-1 && ca[i].Type == ca[0].Type;i++)
			if(!ca[i].Click)
			{
				ca[i].Click=true;
				for(j=i+1;j<num && ca[j].Type == ca[0].Type;j++)
					if(!ca[j].Click)
					{	
						ca[j].Click=true;
						for(temp=1;temp<num && ca[temp].Click;temp++);

						if(ca[0].Num==ca[i].Num && ca[0].Num==ca[j].Num)
							tem=GetNext(ca+temp,num-temp,(dou && true),ca[0].Type+1+type*10,(fu && true));
						if (tem>k)
							k=tem;

						if(ca[0].Num==ca[i].Num-1 && ca[0].Num==ca[j].Num-2)
							tem=GetNext(ca+temp,num-temp,(dou && true),ca[0].Type+1+type*10,false);
						if(tem>k)
							k=tem;

						ca[j].Click=false;
					}
				ca[i].Click=false;			
			}	
		if(ca[0]==ca[1] && dou)
			tem=GetNext(ca+2,num-2,false,ca[0].Type+type*10,(fu && true));
		if(tem>k)
			k=tem;
	}
	return k;
}

int Managers::SearchCard(Card ca[],Card cad)
{
	int i;
	for(i=0;ca[i].Num<10;i++)
		if(ca[i]==cad)
			return i;
	if(ca[13]==cad)
		return 13;
	return -1;
}

void Managers::GameOver(int num)
{
	
	if(MainComputer)
	{
		pControl->m_ready[0]=false;
		pControl->m_ready[1]=false;
		pControl->m_ready[2]=false;
		pControl->m_ready[3]=false;
	}
	LastWiner=Game_State;
	//算分
	if(OutCard.Num<10)//有人点炮
	{
		PlayPoints[OutPlayer]-=3*Power[Game_State]*num;
		PlayPoints[Game_State]+=3*Power[Game_State]*num;
		int i;
		for(i=0;i<4;i++)
			if(i!=OutPlayer)
				LastPoints[i]=0;
			else if(i==OutPlayer)
				LastPoints[i]=-3*Power[Game_State]*num;
			else
				LastPoints[i]=3*Power[Game_State]*num;
	}
	else//自摸
	{
		PlayPoints[Game_State]+=9*Power[Game_State]*num;
		LastPoints[Game_State]=9*Power[Game_State]*num;
		int i;
		for(i=0;i<4;i++)
			if(i!=Game_State)
			{
				PlayPoints[i]-=3*Power[Game_State]*num;
				LastPoints[i]=-3*Power[Game_State]*num;
			}
	}
	
	Power[0]=1;
	Power[1]=1;
	Power[2]=1;
	Power[3]=1;
	Game_State+=4;
}
////////////////////////////////////////////////////////////////////////
/*
* 函数名称：PlaySound
* 函数介绍：播放声音
* 输入参数：action含义 
	  1过 2 出牌 3吃 4碰 5杠 9和
* 输出参数：无
* 返回值  ：无			int Num;//牌面数目 1-9 大与9表示此牌不存在
						int Type;//牌的类型 条=0 万=1 饼=2
*/
void Managers::PlaySound(int action)
{	
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, '\\'))[1] = '\0'; //删除文件名，只获得路径
	switch(action)
	{
	case -1:
		strcpy(szFilePath+strlen(szFilePath), "sound\\dice.wav");
		m_sound.PlayWaveSound(szFilePath);
		break;
	case 0:
		m_backmusic.Play();
		break;
	case 1:
		strcpy(szFilePath+strlen(szFilePath), "sound\\wrong.wav");
		m_sound.PlayWaveSound(szFilePath);
		break;
	case 2:
		strcpy(szFilePath+strlen(szFilePath), "sound\\click.wav");
		m_sound.PlayWaveSound(szFilePath);
		break;
	case 3:
		strcpy(szFilePath+strlen(szFilePath), "sound\\give.wav");
		m_sound.PlayWaveSound(szFilePath);
		break;
	case 4:
		strcpy(szFilePath+strlen(szFilePath), "sound\\give.wav");
		m_sound.PlayWaveSound(szFilePath);
		break;
	case 5:
		strcpy(szFilePath+strlen(szFilePath), "sound\\give.wav");
		m_sound.PlayWaveSound(szFilePath);
		break;
	case 9:
		strcpy(szFilePath+strlen(szFilePath), "sound\\win.wav");
		m_backmusic.Stop();
		isPlayBkmusic=false;
		m_sound.PlayWaveSound(szFilePath);
		break;
	default:
		break;
	}
}