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
	int Num;//������Ŀ 1-9 ����9��ʾ���Ʋ�����
	int Type;//�Ƶ����� ��=0 ��=1 ��=2
	bool Click;//�����ѡ�ֵ��ƽ���true��û��ѡ����false
};

#endif // !defined(AFX_CARD_H__C1686A48_DB45_43BA_BB18_B43B868BAE6C__INCLUDED_)
