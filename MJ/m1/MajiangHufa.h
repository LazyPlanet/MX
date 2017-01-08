// MajiangHufa.h: interface for the CMajiangHufa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAJIANGHUFA_H__F1518920_3ECD_11D8_905D_00E04C5E03BB__INCLUDED_)
#define AFX_MAJIANGHUFA_H__F1518920_3ECD_11D8_905D_00E04C5E03BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMajiangHufa  
{
public:
	void GetHupai(CUIntArray *arr);
	void SetMjPaiArray(CUIntArray *nPai);
	void SetMjPaiArray(int *nPailist,int nNum);
	CMajiangHufa();
	virtual ~CMajiangHufa();

private:
	void Thing4Only();
	void DeleteStepTeam(UINT pai,int type);
	void DeleteSameTeam(UINT pai);
	void AddHupai(UINT pai);
	BOOL IsStepTeam(UINT pai,int type);
	BOOL IsSameTeam(UINT pai);
	void ThingHupai();

	CStdioFile m_fp;

	CUIntArray m_MajiaList,m_HupaiList;


};

#endif // !defined(AFX_MAJIANGHUFA_H__F1518920_3ECD_11D8_905D_00E04C5E03BB__INCLUDED_)
