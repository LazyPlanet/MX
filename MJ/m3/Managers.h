// Managers.h: interface for the Managers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANAGERS_H__653B297F_ACBA_4A5F_8280_99B83D57EA8A__INCLUDED_)
#define AFX_MANAGERS_H__653B297F_ACBA_4A5F_8280_99B83D57EA8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Card.h"
#include "NetControl.h"
#include "Mp3Sound.h"
#include "WaveSound.h"

class CProgramView;

class Managers  
{
public:
	bool isPlayBkmusic;
	WaveSound m_sound;
	Mp3Sound m_backmusic;
	void PlaySound(int action);
	int SearchCard(Card ca[],Card cad);
	int Cardinfo(Card ca[],Card pushca[],int inp, Card inc,int player);
	void Carddelete(int start, Card ca[],int num);
	void Cardinsert(const Card inc, Card ca[],int num);
	bool DoMsg(int num,int action);
	void GameStart();//游戏初始化，发牌。
	bool MainComputer;//主机，是主机则是true 不是则为false;
	Managers();
	virtual ~Managers();

	Card PlayCards[4][15];//最多14张牌
	Card OutCard;//走的牌，一次1张牌	
	Card Sendleft[140];//记录剩下的牌
	int leftstart;
	int leftend;
	Card PushCards[4][20];//倒下的牌
	Card PlayOuts[4][30];//玩家仍出去在牌池中的牌
	int Outsnum[4];//在牌池中某玩家出牌的总数
	int OutPlayer;//走牌的玩家
	int PlayerAc;//这台机器上的玩家
	int Game_State;
	// -2游戏没开始
	// -1游戏准备中，选地主
	// 0-3 游戏玩家做出动作
	// 4-7 对应0-3玩家胜利
	int Actions;//记录当前可以用的动作
	int Power[4];//计算当前游戏各个玩家的翻数
	int LastWiner;//上一次赢的玩家
	int PlayPoints[4];//每个玩家的得分
	int LastPoints[4];//上一次游戏得的分数
	int PointPlayer;
	void Updata();
	NetControl *pControl;
	CProgramView	*pView;
	friend class NetControl;
protected:
	void GameOver(int num);
	int GetNext(Card *ca, int num,bool dou,int type,bool fu);
	void SortCard(Card ca[]);
	void SendCard();
	int LeftNum;//还剩下多少牌可以摸
};

#endif // !defined(AFX_MANAGERS_H__653B297F_ACBA_4A5F_8280_99B83D57EA8A__INCLUDED_)