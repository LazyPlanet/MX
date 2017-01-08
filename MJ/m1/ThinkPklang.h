// ThinkPklang.h: interface for the CThinkPklang class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THINKPKLANG_H__4D94AFF9_7B2B_4A8D_9CD5_5932AEE5638D__INCLUDED_)
#define AFX_THINKPKLANG_H__4D94AFF9_7B2B_4A8D_9CD5_5932AEE5638D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThinkPklang  
{
public:
	BOOL IsOnlySameTwo();
	void GetHupaiTeam(int *p);
	void CheckOnlyTwo();
	void ThinkGood2(int start);
	void ThinkGood(int start);
	BOOL IsStepTeam3(int start,UINT pai,int &pos1,int &pos2);
	BOOL IsSameTeam3(int start,UINT pai,int &pos1,int &pos2);

	void GetGoodTeam(int &pai1,int &pai2,int &pai3);
	void SetMjPaiArray(int *nPailist, int nNum);
	BOOL IsNeed();
	int GetBadly();
	int GetMaxScore();
	void Think();

	CThinkPklang();
	virtual ~CThinkPklang();
private:
	void SetTeamPai(int type);
	int AddGood(int pai);
	BOOL IsEdge(int pai1,int pai2);
	void SortMj(int *who,int end);
	void Think1();
	void Think2();
	void Think3();
	BOOL IsStepTeam(UINT pai,int &pos1,int &pos2);
	BOOL IsSameTeam(UINT pai,int &pos1,int &pos2);

	BOOL IsStepTeam2(UINT pai,int type,int &pos1,int nStart);
	BOOL IsSameTeam2(UINT pai,int &pos1,int nStart);

	int FindPos(int pai,int nStart=0);
	int FindPos2(int pai,int nStart=0);

private:
	int m_nTowgoodNum;
	int m_nTowgood[16];

	int m_nMajiang[17];
	int m_nRemain3[17];
	int m_nRemain2[17];
	int m_nRemain1[17];
	BOOL m_bMjSelect[17];

	int m_nPaiTeam3[15];
	int m_nCheckNum;

	int m_nGoodTeam3[3];
	int m_nNeedPai;

	int m_nRemainNum3,m_nRemainNum2,m_nRemainNum1;
	int m_nMjNum;
	int m_nScore3,m_nScore2;
	int m_nMaxScore3,m_nMaxScore2,m_nMaxScore1;
	int m_nBadly;

	BOOL m_bHaveDui;
	
//	CStdioFile m_fp;


	
};

#endif // !defined(AFX_THINKPKLANG_H__4D94AFF9_7B2B_4A8D_9CD5_5932AEE5638D__INCLUDED_)
