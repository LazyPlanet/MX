// Card.h: interface for the Card class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARD_H__C1686A48_DB45_43BA_BB18_B43B868BAE6C__INCLUDED_)
#define AFX_CARD_H__C1686A48_DB45_43BA_BB18_B43B868BAE6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Card  
{
public:
	Card();
	virtual ~Card();
	int Num;//牌面数目 1-9 大与9表示此牌不存在
	int Type;//牌的类型 条=0 万=1 饼=2
	bool Click;//被玩家选种的牌将是true，没被选种是false
};

#endif // !defined(AFX_CARD_H__C1686A48_DB45_43BA_BB18_B43B868BAE6C__INCLUDED_)
