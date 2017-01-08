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
	void GameStart();//��Ϸ��ʼ�������ơ�
	bool MainComputer;//����������������true ������Ϊfalse;
	Managers();
	virtual ~Managers();

	Card PlayCards[4][15];//���14����
	Card OutCard;//�ߵ��ƣ�һ��1����	
	Card Sendleft[140];//��¼ʣ�µ���
	int leftstart;
	int leftend;
	Card PushCards[4][20];//���µ���
	Card PlayOuts[4][30];//����Գ�ȥ���Ƴ��е���
	int Outsnum[4];//���Ƴ���ĳ��ҳ��Ƶ�����
	int OutPlayer;//���Ƶ����
	int PlayerAc;//��̨�����ϵ����
	int Game_State;
	// -2��Ϸû��ʼ
	// -1��Ϸ׼���У�ѡ����
	// 0-3 ��Ϸ�����������
	// 4-7 ��Ӧ0-3���ʤ��
	int Actions;//��¼��ǰ�����õĶ���
	int Power[4];//���㵱ǰ��Ϸ������ҵķ���
	int LastWiner;//��һ��Ӯ�����
	int PlayPoints[4];//ÿ����ҵĵ÷�
	int LastPoints[4];//��һ����Ϸ�õķ���
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
	int LeftNum;//��ʣ�¶����ƿ�����
};

#endif // !defined(AFX_MANAGERS_H__653B297F_ACBA_4A5F_8280_99B83D57EA8A__INCLUDED_)