// CMajiangGameDlg.h : header file
//

#if !defined(AFX_CMAJIANGGAMEDLG_H__8D71C186_4473_11D8_905D_00E04C5E03BB__INCLUDED_)
#define AFX_CMAJIANGGAMEDLG_H__8D71C186_4473_11D8_905D_00E04C5E03BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCMajiangGameDlg dialog
#define TIME_DELAY      120
#define TIME_SELECT		121
#define TIME_FLASH		122

#define BUTTON_EAT		100
#define BUTTON_PENG		101
#define BUTTON_GANG		102
#define BUTTON_HU		103



class CCMajiangGameDlg : public CDialog
{
// Construction
public:
	int GetQuanqiu(int type);//是否有全求
	int GetDuting(int type);//是否有独听
	int GetHaidilaoyue(int type);//是否有海底捞月
	int GetSixiPai(int type);//是否有大四喜或小四喜
	int GetQingyise(int type);//是否有清一色或混一色
	int GetPengpenghu(int type);//是否有碰碰胡
	int GetSananke(int type);//是否有三暗刻或四暗刻或五暗刻
	int GetPinghu(int type);//是否有平胡
	int GetSanyuanpai(int type);//是否有三元牌或小三元或大三元
	int GetMenqing(int type);//是否是门清

	int CalculateTaiNumber(int who,BOOL bZimo);
	void Damanguan(BOOL m_bUser);
	void GetUserHupaiTeam();
	int CheckNeedGet(int pai);
	void FindHu();
	int ComputerThink(int pai);
	void SetTrans(int pos,BOOL who);
	void PlayMinganSound(int type);
	void DrawEyes(int type);
	void DrawFace(int type);
	void OnClickTitle();
	void MouseMoveOnTitle(CPoint point);
	void PlayChessSound(int type,BOOL bGirl=TRUE,BOOL bNor=TRUE);
	void PlayPaiSound(int nPai,BOOL bGirl=TRUE);
	void ShowOver(int type);
	void CheckGang(int nPai);
	void CheckGangSelf(int nPai);
	void CheckPengSelect(int nPai);
	void CheckPeng(int nPai);
	void CheckEat(int nPai);
	void CheckComputerHupai();
	void CheckHupai();
	void DrawPai();
	void CheckPai(int nPai);
	int FindPaiPos(int nPai,int nStart=0,int nEnd=16);
	int FindPaiComPos(int nPai,int nStart=0,int nEnd=16);
	BOOL isCanGang(int nPai);
	BOOL isCanGangSelf(int nPai);
	BOOL isCanPeng(int nPai);
	BOOL isCanEat(int nPai);
	void DrawPaiUp(int pos);
	void SortMj(int *who,int start=0,int end=16);
	int GetRndMj();
	BOOL GetFixMj(int pai);
	void ResetMj();
	void UpdateFrame();
	CCMajiangGameDlg(CWnd* pParent = NULL);	// standard constructor

    //处理吃牌
	int m_nEatTeam; //符合数量
	int m_nEatPos[6];  //位置
	int m_nEatSelect;//当前选择

	int m_nPai[136];
	BOOL m_bSelect[136];
	CDC m_dcMj,m_dcBG,m_dcMem,m_dcState,m_dcBuffer;
	int m_nMjUser[16];			//用户麻将牌
	int m_nMjComputer[16];		//电脑麻将
	int m_nChupaiUser;				//待出牌
	int m_nChupaiComputer;				//待出牌
	int m_nMouseOnPai;			//鼠标所在牌的
	int m_nOutPaiUser[18];
	int m_nOutPaiComputer[18];
	int m_nOutPaiUserNumber;
	int m_nOutPaiComputerNumber;

	BOOL m_bCheckUser[18];      //电脑对玩家吃碰的牌
	BOOL m_bCheckComputer[18];

	int m_nComputerGoodScore;

	int m_nGangUser[5];
	int m_nGangComputer[5];

	int m_nStartPosUser;
	int m_nStartPosComputer;

	BOOL m_bEat,m_bPeng,m_bGang,m_bHu;
	BOOL m_bZimo;
	BOOL m_bOver;

	BOOL m_bGangSKH;//是否是杠上开花

	CUIntArray m_arrHupai,m_arrComHupai;

	int m_nHupaiTeam[15];//成牌时各组牌信息

	enum{S_PON,S_GUN,S_EAT,S_ZIMO,S_HU,S_LIU,S_MOU,S_STATE,S_DAO};
	enum{F_CLOSE,F_OPEN1,F_OPEN2};
    enum{E_XI,E_NU,E_SAI,E_LE,E_BUNAI,E_YUYE};

	BOOL m_bHandDown;
	HCURSOR  m_hCursor;
	HCURSOR  m_hCursorDown;

	int m_nDelay;

	BOOL m_bOnBtRestart;//是否在开始按钮上
	CRect m_rtBtRestart;

	//start
	BOOL m_bTitle;
	CRect m_rtTitle[4];
	int m_nMouseOnTitle;
	
	//敏感区域 
	CRect m_rtMingan[3];
// Dialog Data
	//{{AFX_DATA(CCMajiangGameDlg)
	enum { IDD = IDD_CMAJIANGGAME_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCMajiangGameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCMajiangGameDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMAJIANGGAMEDLG_H__8D71C186_4473_11D8_905D_00E04C5E03BB__INCLUDED_)
