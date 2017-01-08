// CMajiangGameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CMajiangGame.h"
#include "CMajiangGameDlg.h"
#include "Picture.h"
#include "MajiangHufa.h"

#include "ThinkPklang.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCMajiangGameDlg dialog

CCMajiangGameDlg::CCMajiangGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCMajiangGameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCMajiangGameDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hCursor = AfxGetApp()->LoadCursor (IDC_CURSORHAND);
	m_hCursorDown = AfxGetApp()->LoadCursor (IDC_CURSORHAND_IN);
	m_bHandDown=FALSE;
	m_bTitle=TRUE;
	m_nDelay=0;
	m_nMouseOnTitle=-1;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCMajiangGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCMajiangGameDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCMajiangGameDlg, CDialog)
	//{{AFX_MSG_MAP(CCMajiangGameDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCMajiangGameDlg message handlers

BOOL CCMajiangGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
		//	pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->InsertMenu(0,MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		//	pSysMenu->InsertMenu(0,MF_SEPARATOR);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect rt1,rt2;
	GetWindowRect(rt1);
	GetClientRect(rt2);
	int cx=rt1.bottom-rt1.top-rt2.bottom;
	int cy=rt1.right-rt1.left-rt2.right;
	SetWindowText("PKLANG麻将");
	MoveWindow(0,0,800+cy,600+cx);
	CenterWindow();

	/////////////

	m_dcBuffer.CreateCompatibleDC(GetDC());
	m_dcMj.CreateCompatibleDC(GetDC());
	m_dcState.CreateCompatibleDC(GetDC());
	m_dcBG.CreateCompatibleDC(GetDC());
	m_dcMem.CreateCompatibleDC(GetDC());
	CBitmap bm;
	CPicture pic,pic2,pic3;
	CSize sz;
	pic.Load("images\\Mj.jpg");
	sz=pic.GetImageSize();
	bm.CreateCompatibleBitmap(GetDC(),sz.cx,sz.cy);
	m_dcMj.SelectObject(&bm);
	pic.Render(&m_dcMj);
	bm.DeleteObject();

	bm.CreateCompatibleBitmap(GetDC(),800,600);
	m_dcBG.SelectObject(&bm);
	pic2.Load("images\\bg.jpg");
	pic2.Render(&m_dcBG);

	bm.DeleteObject();

	bm.CreateCompatibleBitmap(GetDC(),800,600);
	m_dcMem.SelectObject(&bm);

	bm.DeleteObject();

	bm.CreateCompatibleBitmap(GetDC(),140,260);
	m_dcState.SelectObject(&bm);
	pic3.Load("images\\state.jpg");
	pic3.Render(&m_dcState);

	//create dc buffer
	bm.DeleteObject();
	bm.CreateCompatibleBitmap(GetDC(),800,600);
	m_dcBuffer.SelectObject(&bm);

	///////////////Title  
	m_rtTitle[0].SetRect(412,210,724,264);
	m_rtTitle[1].SetRect(412,280,724,334);
	m_rtTitle[2].SetRect(412,350,724,404);
	m_rtTitle[3].SetRect(536,422,620,506);
	
	///////////mingan rect
	m_rtMingan[0].SetRect(347,7,480,187);
	m_rtMingan[1].SetRect(280,188,550,270);
	m_rtMingan[2].SetRect(280,271,550,355);

	//restart button
	m_rtBtRestart.SetRect(230,420,230+362,420+90);

	//////////////////////////
	srand(time(NULL));

	

	::sndPlaySound("sound\\g_Title.wav",SND_ASYNC|SND_NODEFAULT);
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCMajiangGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCMajiangGameDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		if( m_bTitle )
		{
			CClientDC dc(this);
			CPicture pic;
			pic.Load("images\\title.jpg");
			pic.Render(&dc);

		}
		else
			UpdateFrame();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCMajiangGameDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCMajiangGameDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CCMajiangGameDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CCMajiangGameDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
//	pDC->BitBlt(0,0,800,600,&m_dcBG,0,0,SRCCOPY);
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}

void CCMajiangGameDlg::UpdateFrame()
{
	CClientDC dc(this);
	dc.BitBlt(0,0,800,600,&m_dcMem,0,0,SRCCOPY);
}
void CCMajiangGameDlg::DrawPai()
{
	int i;

	m_dcMem.BitBlt(95,505,39+17*39,65,&m_dcBG,95,505,SRCCOPY);
	m_dcMem.BitBlt(90,356, 18*31,48,&m_dcBG,90,356,SRCCOPY);

	for( i=0;i<m_nStartPosUser;i++ )//已经固定了的
		m_dcMem.BitBlt(95+i*39,518, 39,65,&m_dcMj,m_nMjUser[i]*39,64,SRCCOPY);

	for( i=m_nStartPosUser;i<16;i++ )//不是经固定了的
	{
		if( m_bOver )
			m_dcMem.BitBlt(95+i*39,505, 39,65,&m_dcMj,m_nMjUser[i]*39,64,SRCCOPY);
		else
			m_dcMem.BitBlt(95+i*39,505, 39,65,&m_dcMj,m_nMjUser[i]*39,1,SRCCOPY);
	}

	if( m_nChupaiUser!=-1 )//是否有出牌位置的
	{
		if( m_bOver )//正常牌
			m_dcMem.BitBlt(725,505, 39,65,&m_dcMj,m_nChupaiUser*39,64,SRCCOPY);
		else//倒牌
			m_dcMem.BitBlt(725,505, 39,65,&m_dcMj,m_nChupaiUser*39,1,SRCCOPY);
	}
	if( m_nChupaiComputer!=-1 )//电脑是否有出牌位置的
	{
		if( m_bOver )//正常牌
			m_dcMem.BitBlt(90,356, 31,48,&m_dcMj,m_nChupaiComputer*31,176,SRCCOPY);
		else//倒牌
			m_dcMem.BitBlt(90,356, 31,48,&m_dcMj,35*31,129,SRCCOPY);
		
	}


	if( m_nChupaiComputer==-1 )
	for( i=0;i<m_nOutPaiComputerNumber;i++ )//画电脑出了牌
		m_dcMem.BitBlt(647-i*31,422, 31,48,&m_dcMj,m_nOutPaiComputer[i]*31,176,SRCCOPY);

	for( i=0;i<18;i++ )     //吃碰杠的牌红色显示
	{
		if( m_bCheckComputer[i] ) 
			SetTrans(i,FALSE);
	}


	for( i=0;i<m_nOutPaiUserNumber;i++ )//画玩家出了牌
		m_dcMem.BitBlt(120+i*31,456, 31,48,&m_dcMj,m_nOutPaiUser[i]*31,129,SRCCOPY);

	for( i=0;i<18;i++ )     //吃碰杠的牌红色显示
	{
		if( m_bCheckUser[i] ) 
			SetTrans(i,TRUE);
	}

	//////////////电脑牌面
	for( i=0;i<m_nStartPosComputer;i++ )//已经固定了的
			m_dcMem.BitBlt(135+(15-i)*31,366, 31,48,&m_dcMj,m_nMjComputer[i]*31,176,SRCCOPY);

	for( i=m_nStartPosComputer;i<16;i++ )//画电脑牌
	{
		if( m_bOver )
			m_dcMem.BitBlt(135+(15-i)*31,356, 31,48,&m_dcMj,m_nMjComputer[i]*31,176,SRCCOPY);
		else
			m_dcMem.BitBlt(135+(15-i)*31,356, 31,48,&m_dcMj,35*31,129,SRCCOPY);
	}

	for( i=0;i<5;i++ )//画有杠的
		if( m_nGangUser[i]!=0 )
		{
			if( m_nGangUser[i]>0 )
			{
				for( int j=0;j<3;j++ )
					m_dcMem.BitBlt(95+(i*3+j)*39,515, 39,65,&m_dcMj,34*39,64,SRCCOPY);
				m_dcMem.BitBlt(95+(i*3+1)*39,500, 39,65,&m_dcMj,m_nMjUser[i*3]*39,64,SRCCOPY);
			}
			else
				m_dcMem.BitBlt(95+(i*3+1)*39,500, 39,65,&m_dcMj,m_nMjUser[i*3]*39,64,SRCCOPY);
		}


}

void CCMajiangGameDlg::ResetMj()
{
	int i,j;
	for( i=0;i<34;i++ )
		for( j=0;j<4;j++ )
		{
			m_nPai[i*4+j]=i;
			m_bSelect[i*4+j]=FALSE;
		}
	int rnd;
	for( i=0;i<16;i++ )
	{
		rnd=GetRndMj();
		m_nMjUser[i]=rnd;
		rnd=GetRndMj();
		m_nMjComputer[i]=rnd;	
	}
	for( i=0;i<5;i++ )
	{
		m_nGangUser[i]=0;
		m_nGangComputer[i]=0;
	}


	if( rand()%10==0 )
		Damanguan(TRUE);
	if( rand()%5==0 )
		Damanguan(FALSE);

/*	int kkk[16]={1,1,1,3,4,5,16,16,29,29,11,12,13,20,21,22};
	for( i=0;i<16;i++ )
		m_nMjComputer[i]=kkk[i];	
	
	
	m_nMjUser[0]=8;
	m_nMjUser[1]=18;
	m_nMjUser[2]=20;
	m_nMjUser[3]=4;
	m_nMjUser[4]=6;//
	m_nMjUser[5]=7;
	m_nMjUser[9]=m_nMjUser[10];
	m_nMjUser[13]=m_nMjUser[14]=m_nMjUser[15];
*/
	m_nChupaiUser=-1;//GetRndMj();
	m_nChupaiComputer=-1;
	m_nMouseOnPai=-1;
	m_nOutPaiUserNumber=0;
	m_nOutPaiComputerNumber=0;


	m_nStartPosUser=0;
	m_nStartPosComputer=0;
	m_bEat=m_bPeng=m_bGang=m_bHu=FALSE;
	m_nEatTeam=0;
	m_bOver=FALSE;
	m_bZimo=FALSE;
	m_bOnBtRestart=FALSE;
	for( i=0;i<18;i++ )
	{
		m_bCheckUser[i]=FALSE; 
		m_bCheckComputer[i]=FALSE; 
	}

	SetTimer(TIME_DELAY,1000,NULL);

	m_arrHupai.RemoveAll();

	CClientDC dc(this);
	m_dcMem.BitBlt(0,0,800,600,&m_dcBG,0,0,SRCCOPY);

	dc.BitBlt(0,0,800,600,&m_dcMem,0,0,SRCCOPY);
	for( i=0;i<4;i++ )
	{
		PlayChessSound(S_MOU,FALSE,FALSE);
		for( j=0;j<4;j++ )
		{
			m_dcMem.BitBlt(95+(i*4+j)*39,505, 39,65,&m_dcMj,m_nMjUser[i*4+j]*39,1,SRCCOPY);
			m_dcMem.BitBlt(135+(i*4+j)*31,356, 31,48,&m_dcMj,35*31,129,SRCCOPY);
		}
		UpdateFrame();
		Sleep(300);
	}

//	DrawPai();
	PlayChessSound(S_DAO,FALSE,FALSE);
	Sleep(500);
	for( i=0;i<16;i++ )
		m_dcMem.BitBlt(95+i*39,505, 39,65,&m_dcMj,42*39,1,SRCCOPY);

	dc.BitBlt(0,0,800,600,&m_dcMem,0,0,SRCCOPY);

	PlayChessSound(S_DAO,FALSE,FALSE);
	Sleep(500);
	SortMj(m_nMjUser);
	SortMj(m_nMjComputer);

	CheckHupai();
	CheckComputerHupai();
	DrawPai();
	dc.BitBlt(0,0,800,600,&m_dcMem,0,0,SRCCOPY);


	//取得电脑最佳分值
	CThinkPklang bb;
	int a[17];
	for(i=0;i<16;i++)
		a[i]=m_nMjComputer[i];
	bb.SetMjPaiArray(a,16);
	bb.Think();
	m_nComputerGoodScore=bb.GetMaxScore();	
		
}

int CCMajiangGameDlg::GetRndMj()
{

	CUIntArray ar;
	for( int i=0;i<136;i++ )
		if( !m_bSelect[i] )
			ar.Add(i);
	if( !ar.GetSize() )return -1;
	int ret=ar.GetAt( rand()%ar.GetSize() );
	m_bSelect[ret]=TRUE;
	return  m_nPai[ret];


}
BOOL CCMajiangGameDlg::GetFixMj(int pai)
{

	CUIntArray ar;
	BOOL bGet=FALSE;
	for( int i=0;i<136;i++ )
		if( !m_bSelect[i] &&  m_nPai[i]==pai )
		{
			m_bSelect[i]=TRUE;
			bGet=TRUE;
			break;
		}
	
	return  bGet;


}

void CCMajiangGameDlg::SortMj(int *who,int start,int end)
{
	int i,j,k,t;
	for( i = start ; i < end-1 ; i++ )
	{
		for( k = i ,j = i+1 ; j <end ; j++ )
            if( who[k] > who[j] )
				k = j;
		if( k != i )
		{
			t =  who[i];
			who[i]=who[k];
			who[k]= t;
		}
	}


}

void CCMajiangGameDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here



	CString str,str1;

	CString s="门清自摸";
	int k=5;
	str.Format("%-20s%d台",s,k);

		AfxMessageBox(str);

/*
//	if( m_nChupaiUser==-1 )return;
	int a[17]={1,2,2,3,3,4,14,16,15,22,23,24,30,30,30,9,9};
//	int a[17]={5,5,6,11,12,15,15,16,16,18,19,20,24,25,30,30,3};
	int i;
	CThinkPklang bb;
	
//	for(i=m_nStartPosUser;i<14;i++)
//		a[i-m_nStartPosUser]=a[i];
//	a[13]=m_nChupaiUser;
	bb.SetMjPaiArray(a,17);


//	DWORD t=timeGetTime();
	bb.Think();

	int aa[15];
	bb.GetHupaiTeam(aa);


	CString str;
	str.Format("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",aa[0],aa[1],aa[2],aa[3],aa[4],aa[5],aa[6],aa[7],aa[8],aa[9],aa[10],aa[11],aa[12],aa[13],aa[14]);
	MessageBox(str);
*/
//	dc.BitBlt(0,0,39,65,&m_dcMj,bb.GetMaxScore()*39,64,SRCCOPY);
}

void CCMajiangGameDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here

	ResetMj();
	UpdateFrame();
	
}

void CCMajiangGameDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseMove(nFlags, point);
	
	if( m_bTitle )
	{
		MouseMoveOnTitle(point);
		return;
	}


	if( m_bOver )
	{
		if( m_rtBtRestart.PtInRect(point ) )
		{
			if( !m_bOnBtRestart )
			{
				m_bOnBtRestart=TRUE;
				CPicture pic;
				pic.Load("images\\start2.jpg");
				pic.Render(&m_dcMem,m_rtBtRestart);
				UpdateFrame();
				PlayChessSound(S_STATE,TRUE,FALSE);
			}
		}
		else
		{
			if( m_bOnBtRestart )
			{
				m_bOnBtRestart=FALSE;
				CPicture pic;
				pic.Load("images\\start1.jpg");
				pic.Render(&m_dcMem,m_rtBtRestart);
				UpdateFrame();
			}
		}
		return;
	}
	
	BOOL bFind=FALSE;
	if( m_bEat )
	{
		CRect rt(725,300,725+65,40+300);
		if( rt.PtInRect(point) )
		{
			if( m_nMouseOnPai==BUTTON_EAT )return;
			else
			{
			/*	if( m_nMouseOnPai>=BUTTON_EAT )
				{
					if( m_nMouseOnPai==BUTTON_HU && m_bZimo )
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*4+1,SRCCOPY);
					else
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*(m_nMouseOnPai-100)+1,SRCCOPY);
					
				}	
			*/		
				m_nMouseOnPai=BUTTON_EAT;
				m_dcMem.BitBlt(725,300,65,40,&m_dcState,68,1,SRCCOPY);
				UpdateFrame();
				bFind=TRUE;
				PlayChessSound(S_STATE,TRUE,FALSE);
			}

		}


	}
	if( m_bPeng )
	{
		CRect rt(725,300+50,725+65,40+300+50);
		if( rt.PtInRect(point) )
		{
			if( m_nMouseOnPai==BUTTON_PENG )return;
			else
			{
			/*	if( m_nMouseOnPai>=BUTTON_EAT )
				{
					if( m_nMouseOnPai==BUTTON_HU && m_bZimo )
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*4+1,SRCCOPY);
					else
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*(m_nMouseOnPai-100)+1,SRCCOPY);

				}	
			*/		
				m_nMouseOnPai=BUTTON_PENG;
				m_dcMem.BitBlt(725,300+50*1,65,40,&m_dcState,68,1+53*1,SRCCOPY);
				UpdateFrame();
				bFind=TRUE;
				PlayChessSound(S_STATE,TRUE,FALSE);
			}

		}


	}
	if( m_bGang )
	{
		CRect rt(725,300+50*2,725+65,40+300+50*2);
		if( rt.PtInRect(point) )
		{
			if( m_nMouseOnPai==BUTTON_GANG )return;
			else
			{
			/*	if( m_nMouseOnPai>=BUTTON_EAT)
				{
					if( m_nMouseOnPai==BUTTON_HU && m_bZimo )
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*4+1,SRCCOPY);
					else
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*(m_nMouseOnPai-100)+1,SRCCOPY);

				}	
				*/	
				m_nMouseOnPai=BUTTON_GANG;
				m_dcMem.BitBlt(725,300+50*2,65,40,&m_dcState,68,1+53*2,SRCCOPY);
				UpdateFrame();
				bFind=TRUE;
				PlayChessSound(S_STATE,TRUE,FALSE);
			}
	}


	}
	if( m_bHu )
	{
		CRect rt(725,300+50*3,725+65,40+300+50*3);
		if( rt.PtInRect(point) )
		{
			if( m_nMouseOnPai==BUTTON_HU )return;
			else
			{
			/*	if( m_nMouseOnPai>=BUTTON_HU )
				{
					if( m_nMouseOnPai==BUTTON_HU && m_bZimo )
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*4+1,SRCCOPY);
					else
						m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*(m_nMouseOnPai-100)+1,SRCCOPY);

				}*/	
				m_nMouseOnPai=BUTTON_HU;
				if( m_bZimo )
					m_dcMem.BitBlt(725,300+50*3,65,40,&m_dcState,68,1+53*4,SRCCOPY);
				else
					m_dcMem.BitBlt(725,300+50*3,65,40,&m_dcState,68,1+53*3,SRCCOPY);
				UpdateFrame();
				bFind=TRUE;
				PlayChessSound(S_STATE,TRUE,FALSE);
			}
	
		}


	}

	if( bFind==FALSE && m_nMouseOnPai>=BUTTON_EAT )
	{
		if( m_nMouseOnPai==BUTTON_HU && m_bZimo )
			m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*4+1,SRCCOPY);
		else
			m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*(m_nMouseOnPai-100)+1,SRCCOPY);
	//	m_dcMem.BitBlt(725,300+50*(m_nMouseOnPai-100),65,40,&m_dcState,1,53*(m_nMouseOnPai-100)+1,SRCCOPY);
		UpdateFrame();
		m_nMouseOnPai=-1;
	}
	

	bFind=FALSE;
	if( m_nChupaiUser!=-1 )
	{
		CRect rt;
	//	if(  )m_nMouseOnPai=-1;
		if( m_nMouseOnPai!=-1 && m_nMouseOnPai<BUTTON_EAT)
		{
			rt.SetRect( 95+m_nMouseOnPai*39,495, 95+m_nMouseOnPai*39+39,65+495+10 );
			if( rt.PtInRect( point ) )
			{
				bFind=TRUE;
				return ;
			}
		}
		for( int i=m_nStartPosUser;i<16;i++ )
		{
			rt.SetRect( 95+i*39,505, 95+i*39+39,65+505 );
			if( rt.PtInRect( point ) )
			{
				if( m_nMouseOnPai!=-1 )
				{
					UpdateFrame();
					//ReDrawBG( m_nMouseOnPai,TRUE);
					//DrawPai();
				}
				DrawPaiUp(i);
				m_nMouseOnPai=i;
				bFind=TRUE;
				PlayChessSound(S_MOU,TRUE,FALSE);

			}
		}
		if( bFind==FALSE )
		{
			if( m_nMouseOnPai==16 && m_nMouseOnPai<BUTTON_EAT )
				rt.SetRect( 725,495,  725+39,495+65+10 );
			else
				rt.SetRect( 725,505,  725+39,505+65 );
			if( rt.PtInRect( point ) )
			{
				if( m_nMouseOnPai!=-1  )
					UpdateFrame();//ReDrawBG( m_nMouseOnPai,TRUE);
				DrawPaiUp(16);
				m_nMouseOnPai=16;
				bFind=TRUE;
			}

		}
		if( bFind== FALSE && m_nMouseOnPai!=-1 && m_nMouseOnPai<100 )
		{
		//	ReDrawBG( m_nMouseOnPai,TRUE);
			UpdateFrame();
			m_nMouseOnPai=-1;
		}


	}
	

}

void CCMajiangGameDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonUp(nFlags, point);
	
	MSG message;
	while(::PeekMessage(&message,this->m_hWnd,WM_LBUTTONUP,WM_LBUTTONUP,PM_REMOVE));

	::SetCursor(m_hCursor);
	m_bHandDown=FALSE;

	if( m_bTitle )
	{
	//	MouseMoveOnTitle();
		OnClickTitle();
		return;
	}

	////////////如果是结束了就重新开始
	if( m_bOver )
	{
		if( m_bOnBtRestart )
		   ResetMj();

		return;
	}
	//////////////是否点在敏感部位$%^*&^*&(*(
	for( int i=0;i<3;i++ )
	{
		if( m_rtMingan[i].PtInRect(point) )
		{
			switch( i)
			{
				case 0:
					DrawEyes(E_NU);
					PlayMinganSound(E_NU);
					break;
				case 1:
					DrawEyes(E_LE);
					PlayMinganSound(E_LE);
					break;
				case 2:
					DrawEyes(E_YUYE);
					PlayMinganSound(E_YUYE);
					break;
			}
	
			Sleep(600);
			UpdateFrame();
			return;

		}
	}




	////是否是在待选择吃牌状态
	if( m_nEatTeam>1 )
	{
		KillTimer(TIME_SELECT);
		CheckPengSelect( m_nOutPaiComputer[ m_nOutPaiComputerNumber-1 ] );
		m_nEatTeam=-1;
		return;
	}

	m_nDelay=-5;
	//////是否有可以吃碰胡杠标志
	if( m_bEat||m_bPeng||m_bGang||m_bHu )
	{
		m_bEat=m_bPeng=m_bGang=m_bHu=FALSE;
		m_dcMem.BitBlt(725,300,65,50*4,&m_dcBG,725,300,SRCCOPY);
		UpdateFrame();
		KillTimer(TIME_FLASH);
	}
	//是否鼠标点在吃碰胡杠标志
	if( m_nMouseOnPai>=BUTTON_EAT )
	{
		switch( m_nMouseOnPai )
		{
		case BUTTON_EAT:
			CheckEat( m_nOutPaiComputer[ m_nOutPaiComputerNumber-1 ] );
			PlayChessSound(S_EAT,FALSE);
			m_bCheckComputer[ m_nOutPaiComputerNumber-1 ]=TRUE;
			break;
		case BUTTON_PENG:
			CheckPeng( m_nOutPaiComputer[ m_nOutPaiComputerNumber-1 ] );
			PlayChessSound(S_PON,FALSE);
			m_bCheckComputer[ m_nOutPaiComputerNumber-1 ]=TRUE;
			break;
		case BUTTON_GANG:
			if( m_nChupaiUser==-1 )
			{
				CheckGang( m_nOutPaiComputer[ m_nOutPaiComputerNumber-1 ] );
			    m_bCheckComputer[ m_nOutPaiComputerNumber-1 ]=TRUE;
			}
			else
				CheckGangSelf( m_nChupaiUser );
			PlayChessSound(S_GUN,FALSE);
			FindHu();
			m_bGangSKH=TRUE;
			break;
		case BUTTON_HU:
			if( m_nChupaiUser==-1 )
			{
				m_nChupaiUser=m_nOutPaiComputer[ m_nOutPaiComputerNumber-1 ];
				ShowOver(S_HU);
				PlayChessSound(S_HU,FALSE);
				GetUserHupaiTeam();
				CalculateTaiNumber(1,FALSE);
			}
			else
			{
				ShowOver(S_ZIMO);
				PlayChessSound(S_ZIMO,FALSE);
				GetUserHupaiTeam();
				CalculateTaiNumber(1,TRUE);
			}
		
			return;
			break;
		}
	
		
		CheckHupai();
		m_nMouseOnPai=-1;
	//	UpdateFrame();
		return;	
	}

	m_bGangSKH=FALSE; //把杠上开花标志去掉

	//是否是在出牌状态

	if( m_nChupaiUser==-1 )
	{
		if( m_nOutPaiUserNumber>=18  ) 
		{
			ShowOver(S_LIU);
			PlayChessSound(S_LIU,FALSE);
			return;
		}	
		m_nChupaiUser=GetRndMj();
		DrawPai();
		POINT pt;
		::GetCursorPos(&pt);
		SetCursorPos(pt.x,pt.y);
		
		if( isCanGangSelf(m_nChupaiUser) )
		{
			m_bGang=TRUE;
			m_dcMem.BitBlt(725,300+50*2,65,40,&m_dcState,1,53*2+1,SRCCOPY);

		}
		for( int i=0;i<m_arrHupai.GetSize();i++ )
			if( m_nChupaiUser==m_arrHupai.GetAt(i) )
			{
				m_bHu=TRUE;
				m_bZimo=TRUE;
				m_dcMem.BitBlt(725,300+50*3,65,40,&m_dcState,1,53*4+1,SRCCOPY);
				break;
			}

		UpdateFrame();
	}
	else//不是出牌状态时就出来一个牌
	{



		if( m_nMouseOnPai!=-1 )
		{

			if( m_nMouseOnPai==16 )
			{
				m_nOutPaiUser[ m_nOutPaiUserNumber ]=m_nChupaiUser;
			}
			else
			{
				m_nOutPaiUser[ m_nOutPaiUserNumber ]= m_nMjUser[m_nMouseOnPai];
				m_nMjUser[ m_nMouseOnPai ]=m_nChupaiUser;
				SortMj(m_nMjUser,m_nStartPosUser);
				CheckHupai();

			}
			
			
		}
		else
		{
			m_nOutPaiUser[ m_nOutPaiUserNumber ]=m_nChupaiUser;
			m_nMouseOnPai=16;
		}
		m_nOutPaiUserNumber++;
		m_nChupaiUser=-1;
	//	DrawPaiUp(m_nMouseOnPai);
//		ReDrawBG(,TRUE);

	
		


		m_nMouseOnPai=-1;
		DrawPai();
		UpdateFrame();
		PlayPaiSound(m_nOutPaiUser[ m_nOutPaiUserNumber-1 ],FALSE);

		Sleep(200);

				//m_dcMem.BitBlt(90,356, 31,48,&m_dcMj,35*31,129,SRCCOPY);
		int rnd;
		rnd=CheckNeedGet( m_nOutPaiUser[ m_nOutPaiUserNumber-1 ] );
		
		if( rnd==1000 )
		{
			m_nChupaiComputer=m_nOutPaiUser[ m_nOutPaiUserNumber-1 ];
			ShowOver(S_HU);
			PlayChessSound(S_HU);
			CalculateTaiNumber(2,FALSE);
			return;
		}

		if( rnd==-1 )//看需不需要吃碰的
		{
			rnd=GetRndMj();
			int rett=ComputerThink(rnd);
			if( rett==1000 )
			{
				Sleep(200);
				m_nChupaiComputer=rnd;
				ShowOver(S_ZIMO);
				PlayChessSound(S_ZIMO);
				CalculateTaiNumber(2,TRUE);
				return;
			}
			rnd=rett;
			m_dcMem.BitBlt(90,356, 31,48,&m_dcMj,35*31,129,SRCCOPY);
			UpdateFrame();
			Sleep(300);
		}
		else
		{
			m_bCheckUser[ m_nOutPaiComputerNumber ]=TRUE;
		}

		Sleep(200);
		CheckComputerHupai();
		m_nOutPaiComputerNumber++;

	

		m_nChupaiComputer=-1;
		
	
		m_nOutPaiComputer[ m_nOutPaiComputerNumber-1 ]=rnd;

		
		if( m_nOutPaiUserNumber!=18 )
			CheckPai( rnd );

		for( int i=0;i<m_arrHupai.GetSize();i++ )
			if( rnd==m_arrHupai.GetAt(i) )
			{
				m_bHu=TRUE;
				m_bZimo=FALSE;
				m_dcMem.BitBlt(725,300+50*3,65,40,&m_dcState,1,53*3+1,SRCCOPY);
				break;
			}


		

	//	ReDrawBG(m_nMouseOnPai,TRUE);
		m_nChupaiUser=-1;
		DrawPai();
	//	DrawPaiUp(16);
	//	ReDrawBG(16,FALSE);
		
		m_nMouseOnPai=-1;
		UpdateFrame();

		DrawFace(F_OPEN1);
		PlayPaiSound(rnd,TRUE);
		DrawFace(F_OPEN2);
		Sleep(100);
		DrawFace(F_CLOSE);

		if( m_nOutPaiUserNumber>=18 && 	m_bHu==FALSE ) 
		{
			ShowOver(S_LIU);
			PlayChessSound(S_LIU,FALSE);
			return;
		}
	
	
	}
	
}

void CCMajiangGameDlg::DrawPaiUp(int pos)
{
//
//	CClientDC dc(this);
	UpdateFrame();
	CClientDC dc(this);
	if( pos==16 )
	{
			dc.BitBlt(725,505, 39,65,&m_dcBG,725,505,SRCCOPY);
			if( m_nChupaiUser!=-1 )
				dc.BitBlt(725,495, 39,65,&m_dcMj,m_nChupaiUser*39,1,SRCCOPY);
		
	}
	else
	{
	
			dc.BitBlt(95+pos*39,505, 39,65,&m_dcBG,95+pos*39,505,SRCCOPY);
			dc.BitBlt(95+pos*39,495, 39,65,&m_dcMj,m_nMjUser[pos]*39,1,SRCCOPY);
		
	}
	
}

BOOL CCMajiangGameDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bHandDown )
	::SetCursor(m_hCursorDown);
	else
	::SetCursor(m_hCursor);
	return TRUE;//
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CCMajiangGameDlg::isCanEat(int nPai)
{
//	return FALSE;
	if( nPai<27 )
	{
		int pos1,pos2;
		
		//0-8 万
		//9-17 筒
		//18-26 条
		if( nPai%9==0 )
		{
			pos1=FindPaiPos(nPai+1,m_nStartPosUser);
			pos2=FindPaiPos(nPai+2,pos1+1);
			return ( pos1!=-1 && pos2!=-1 );

		}
		else if( (nPai+1)%9==0 )
		{
			pos1=FindPaiPos(nPai-2,m_nStartPosUser);
			pos2=FindPaiPos(nPai-1,pos1+1);
			return ( pos1!=-1 && pos2!=-1 );
		}
		else
		{
			pos1=FindPaiPos(nPai-1,m_nStartPosUser);
			pos2=FindPaiPos(nPai+1,pos1+1);
			if( pos1!=-1 && pos2!=-1 )return TRUE;
			if( nPai%9<=6 )
			{
				pos1=FindPaiPos(nPai+1,m_nStartPosUser);
				pos2=FindPaiPos(nPai+2,pos1+1);
				if( pos1!=-1 && pos2!=-1 )return TRUE;
			}
			if( nPai%9>=2 )
			{
				pos1=FindPaiPos(nPai-2,m_nStartPosUser);
				pos2=FindPaiPos(nPai-1,pos1+1);
				if( pos1!=-1 && pos2!=-1 )return TRUE;
			}

		}

	}
	return FALSE;

}

BOOL CCMajiangGameDlg::isCanPeng(int nPai)
{
	int pos1,pos2;
	pos1=FindPaiPos(nPai,m_nStartPosUser);
	pos2=FindPaiPos(nPai,pos1+1);
	return ( pos1!=-1 && pos2!=-1 );

}

BOOL CCMajiangGameDlg::isCanGang(int nPai)
{
	int pos1,pos2,pos3;
	pos1=FindPaiPos(nPai,m_nStartPosUser);
	pos2=FindPaiPos(nPai,pos1+1);
	pos3=FindPaiPos(nPai,pos2+1);
	return ( pos1!=-1 && pos2!=-1 && pos3!=-1 );

}
BOOL CCMajiangGameDlg::isCanGangSelf(int nPai)
{
	int i,pos1,pos2,pos3;
	pos1=FindPaiPos(nPai,m_nStartPosUser);
	pos2=FindPaiPos(nPai,pos1+1);
	pos3=FindPaiPos(nPai,pos2+1);
	if ( pos1!=-1 && pos2!=-1 && pos3!=-1 ) return TRUE;

	pos1=FindPaiPos(nPai,0,m_nStartPosUser);
	pos2=FindPaiPos(nPai,pos1+1,m_nStartPosUser);
	pos3=FindPaiPos(nPai,pos2+2,m_nStartPosUser);
	if ( pos1!=-1 && pos2!=-1 && pos3!=-1 ) return TRUE;

	for( i=0;i<m_nStartPosUser/3;i++ )
		if( m_nMjUser[i*3]== m_nMjUser[i*3+1] && m_nMjUser[i*3]== m_nMjUser[i*3+2] )
			if( FindPaiPos( m_nMjUser[i*3],m_nStartPosUser )!=-1 )return TRUE;
	for( i=m_nStartPosUser;i<16-3;i++ )
		if( m_nMjUser[i]==m_nMjUser[i+1] &&
			m_nMjUser[i]==m_nMjUser[i+2] &&
			m_nMjUser[i]==m_nMjUser[i+3] ) return TRUE;

	return FALSE; 

}

int CCMajiangGameDlg::FindPaiPos(int nPai,int nStart,int nEnd)
{
	
	for( int i=nStart;i<nEnd;i++ )
		if( m_nMjUser[i]==nPai )
			return i;
	return -1;

}
int CCMajiangGameDlg::FindPaiComPos(int nPai,int nStart,int nEnd)
{
	
	for( int i=nStart;i<nEnd;i++ )
		if( m_nMjComputer[i]==nPai )
			return i;
	return -1;

}
void CCMajiangGameDlg::GetUserHupaiTeam()
{
	int a[17],i;
	CThinkPklang bb;
	for(i=0;i<16-m_nStartPosUser;i++)
		a[i]=m_nMjUser[i+m_nStartPosUser];
	a[16-m_nStartPosUser]=m_nChupaiUser;
	bb.SetMjPaiArray(a,17-m_nStartPosUser);
	bb.Think();
	bb.GetHupaiTeam(m_nHupaiTeam);
	

}
void CCMajiangGameDlg::CheckPai(int nPai)
{
	if( isCanEat(nPai) )
	{
		
		m_bEat=TRUE;
		m_dcMem.BitBlt(725,300+50*0,65,40,&m_dcState,1,53*0+1,SRCCOPY);
		UpdateFrame();

	}
	if( isCanPeng(nPai) )
	{
		m_bPeng=TRUE;
		m_dcMem.BitBlt(725,300+50*1,65,40,&m_dcState,1,53*1+1,SRCCOPY);
		UpdateFrame();

	}
	if( isCanGang(nPai) )
	{
		m_bGang=TRUE;
		m_dcMem.BitBlt(725,300+50*2,65,40,&m_dcState,1,53*2+1,SRCCOPY);
		UpdateFrame();

	}
    
	if( m_bEat||m_bPeng||m_bGang )
	{
		SetTimer(TIME_FLASH,300,NULL);
		OnTimer(TIME_FLASH);
	}
}


void CCMajiangGameDlg::CheckComputerHupai()
{
	int a[16],i;
	CMajiangHufa mf;
	for(i=0;i<16-m_nStartPosComputer;i++)
		a[i]=m_nMjComputer[i+m_nStartPosComputer];
	m_arrComHupai.RemoveAll();
	mf.SetMjPaiArray(a,16-m_nStartPosComputer);
	mf.GetHupai(&m_arrComHupai);

}

void CCMajiangGameDlg::CheckHupai()
{
	
	int a[16],i;
	m_arrHupai.RemoveAll();
	CMajiangHufa mf;
	
	m_dcMem.BitBlt(700-13*31,250, 31+15*31,48,&m_dcBG,700-13*31,250,SRCCOPY);

	for(i=0;i<16-m_nStartPosUser;i++)
		a[i]=m_nMjUser[i+m_nStartPosUser];
	mf.SetMjPaiArray(a,16-m_nStartPosUser);
	mf.GetHupai(&m_arrHupai);
	for( i=0;i<m_arrHupai.GetSize();i++ )
		m_dcMem.BitBlt(700-i*31,250, 31,48,&m_dcMj,m_arrHupai.GetAt(m_arrHupai.GetSize()-i-1)*31,129,SRCCOPY);
	if( m_arrHupai.GetSize() )
	{
		CPicture pic;
		pic.Load("images\\tingpai.jpg");
		pic.Render(&m_dcMem,CRect(740,250,740+51,250+18));
	}


}
void CCMajiangGameDlg::CheckGang(int nPai)
{
	int i;
	int pos1,pos2,pos3;
	int pai1,pai2,pai3;
	pos1=FindPaiPos(nPai,m_nStartPosUser);
	pai1=m_nMjUser[pos1];
	for( i=pos1;i>m_nStartPosUser;i-- )
		m_nMjUser[i]=m_nMjUser[i-1];
	m_nMjUser[m_nStartPosUser]=pai1;

	pos2=FindPaiPos(nPai,pos1+1);
	pai2=m_nMjUser[pos2];
	for( i=pos2;i>m_nStartPosUser+1;i-- )
		m_nMjUser[i]=m_nMjUser[i-1];

	pos3=FindPaiPos(nPai,pos2+1);
	pai3=m_nMjUser[pos3];
	for( i=pos3;i>m_nStartPosUser+2;i-- )
		m_nMjUser[i]=m_nMjUser[i-1];


			
	m_nMjUser[m_nStartPosUser+2]=pai2;
	m_nMjUser[m_nStartPosUser+1]=pai1;
	m_nMjUser[m_nStartPosUser]=nPai;
			

	m_nGangUser[m_nStartPosUser/3]=-1;
	m_nStartPosUser+=3;

	m_nChupaiUser=GetRndMj();

	DrawPai();
	UpdateFrame();
}
void CCMajiangGameDlg::CheckGangSelf(int nPai)
{
	int i,pos1,pos2,pos3;
	pos1=FindPaiPos(nPai,m_nStartPosUser);
	pos2=FindPaiPos(nPai,pos1+1);
	pos3=FindPaiPos(nPai,pos2+1);
	if ( pos1!=-1 && pos2!=-1 && pos3!=-1 )
	{

		m_nMjUser[pos1]=m_nMjUser[m_nStartPosUser];
		m_nMjUser[pos2]=m_nMjUser[m_nStartPosUser+1];
		m_nMjUser[pos3]=m_nMjUser[m_nStartPosUser+2];
	
		m_nMjUser[m_nStartPosUser+2]=nPai;
		m_nMjUser[m_nStartPosUser+1]=nPai;
		m_nMjUser[m_nStartPosUser]=nPai;
		
		m_nGangUser[m_nStartPosUser/3]=1;
		m_nStartPosUser+=3;

		m_nChupaiUser=GetRndMj();
		SortMj(m_nMjUser,m_nStartPosUser);
		DrawPai();
		UpdateFrame();

		return ;
	}
	pos1=FindPaiPos(nPai,0,m_nStartPosUser);
	pos2=FindPaiPos(nPai,pos1+1,m_nStartPosUser);
	pos3=FindPaiPos(nPai,pos2+1,m_nStartPosUser);
	if ( pos1!=-1 && pos2!=-1 && pos3!=-1 )
	{
		m_nGangUser[pos1/3]=-1;
		m_nChupaiUser=GetRndMj();
		SortMj(m_nMjUser,m_nStartPosUser);
		DrawPai();
		UpdateFrame();	
		//	MessageBox("kao");
		return ;
	}
	for( i=0;i<m_nStartPosUser/3;i++ )
		if( m_nMjUser[i*3]== m_nMjUser[i*3+1] && m_nMjUser[i*3]== m_nMjUser[i*3+2] )
		{
			pos1=FindPaiPos( m_nMjUser[i*3],m_nStartPosUser );
			if( pos1!=-1 )
			{
				m_nGangUser[i]=-1;
				m_nMjUser[pos1]=m_nChupaiUser;
				m_nChupaiUser=GetRndMj();
				SortMj(m_nMjUser,m_nStartPosUser);
			//	SortMj(m_nMjUser,0);
				DrawPai();
				UpdateFrame();	
			
				return ;
			}
		}
	for( i=m_nStartPosUser;i<16-3;i++ )
		if( m_nMjUser[i]==m_nMjUser[i+1] &&
			m_nMjUser[i]==m_nMjUser[i+2] &&
			m_nMjUser[i]==m_nMjUser[i+3] )
		{
			int pai= m_nMjUser[i];
		    m_nMjUser[i]=m_nMjUser[m_nStartPosUser];
		    m_nMjUser[i+1]=m_nMjUser[m_nStartPosUser+1];
		    m_nMjUser[i+2]=m_nMjUser[m_nStartPosUser+2];
		    m_nMjUser[i+3]=m_nMjUser[m_nStartPosUser+3];

			m_nMjUser[m_nStartPosUser+2]=pai;
			m_nMjUser[m_nStartPosUser+1]=pai;
			m_nMjUser[m_nStartPosUser]=pai;

			m_nMjUser[m_nStartPosUser+3]=m_nChupaiUser;

			m_nGangUser[m_nStartPosUser/3]=1;
			m_nStartPosUser+=3;

			m_nChupaiUser=GetRndMj();

			SortMj(m_nMjUser,m_nStartPosUser);;
		//	SortMj(m_nMjUser,0);
			DrawPai();
			UpdateFrame();

			return ;
		}


}

void CCMajiangGameDlg::CheckEat(int nPai)
{
		int pos1,pos2;
		int pai1,pai2;
		int i;
		if( nPai%9==0 )
		{
			pos1=FindPaiPos(nPai+1,m_nStartPosUser);
			pai1=m_nMjUser[pos1];
			for( i=pos1;i>m_nStartPosUser;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;
			m_nMjUser[m_nStartPosUser]=pai1;

			pos2=FindPaiPos(nPai+2,pos1+1);

			pai2=m_nMjUser[pos2];
			for( i=pos2;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			m_nMjUser[m_nStartPosUser+1]=pai2;

			m_nChupaiUser=m_nMjUser[15];
			for( i=15;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			
			m_nMjUser[m_nStartPosUser+2]=pai2;
			m_nMjUser[m_nStartPosUser+1]=pai1;
			m_nMjUser[m_nStartPosUser]=nPai;
			
			m_nStartPosUser+=3;
	

			DrawPai();
			UpdateFrame();


		}
		
		else if( (nPai+1)%9==0 )
		{
			pos1=FindPaiPos(nPai-2,m_nStartPosUser);
			pai1=m_nMjUser[pos1];
			for( i=pos1;i>m_nStartPosUser;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;
			m_nMjUser[m_nStartPosUser]=pai1;

			pos2=FindPaiPos(nPai-1,pos1+1);
			pai2=m_nMjUser[pos2];
			for( i=pos2;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			m_nMjUser[m_nStartPosUser+1]=pai2;

			m_nChupaiUser=m_nMjUser[15];
			for( i=15;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			
			m_nMjUser[m_nStartPosUser+2]=nPai;
			m_nMjUser[m_nStartPosUser+1]=pai2;
			m_nMjUser[m_nStartPosUser]=pai1;
			
			m_nStartPosUser+=3;
			DrawPai();
			UpdateFrame();
		}
		else
		{
			int m1=0,m2=0,m3=0;
			int pos3,pos4,pos5,pos6;

			pos1=FindPaiPos(nPai-1,m_nStartPosUser);
			pos2=FindPaiPos(nPai+1,pos1+1);
			if( pos1!=-1 && pos2!=-1 )m1=1;
	
			if( nPai%9<=6 )
			{
				pos3=FindPaiPos(nPai+1,m_nStartPosUser);
				pos4=FindPaiPos(nPai+2,pos3+1);
				if( pos3!=-1 && pos4!=-1 )m2=1;
			}
			if( nPai%9>=2 )
			{
				pos5=FindPaiPos(nPai-2,m_nStartPosUser);
				pos6=FindPaiPos(nPai-1,pos5+1);
				if( pos5!=-1 && pos6!=-1 )m3=1;
			}
			m_nEatTeam=m1+m2+m3;
			if( m_nEatTeam==1 )
			{
				if( m1 )
				{
					pai1=m_nMjUser[pos1];
					for( i=pos1;i>m_nStartPosUser;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;
					m_nMjUser[m_nStartPosUser]=pai1;

					pai2=m_nMjUser[pos2];
					for( i=pos2;i>m_nStartPosUser+1;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;

					m_nMjUser[m_nStartPosUser+1]=pai2;

					m_nChupaiUser=m_nMjUser[15];
					for( i=15;i>m_nStartPosUser+1;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;

					
					m_nMjUser[m_nStartPosUser+2]=pai2;
					m_nMjUser[m_nStartPosUser+1]=nPai;
					m_nMjUser[m_nStartPosUser]=pai1;
					
	
				}
				else if( m2)
				{
					pai1=m_nMjUser[pos3];
					for( i=pos3;i>m_nStartPosUser;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;
					m_nMjUser[m_nStartPosUser]=pai1;

					pai2=m_nMjUser[pos4];
					for( i=pos4;i>m_nStartPosUser+1;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;

					m_nMjUser[m_nStartPosUser+1]=pai2;

					m_nChupaiUser=m_nMjUser[15];
					for( i=15;i>m_nStartPosUser+1;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;

					
					m_nMjUser[m_nStartPosUser+2]=pai2;
					m_nMjUser[m_nStartPosUser+1]=pai1;
					m_nMjUser[m_nStartPosUser]=nPai;
	
				}
				else
				{
					pai1=m_nMjUser[pos5];
					for( i=pos5;i>m_nStartPosUser;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;
					m_nMjUser[m_nStartPosUser]=pai1;

					pai2=m_nMjUser[pos6];
					for( i=pos6;i>m_nStartPosUser+1;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;

					m_nMjUser[m_nStartPosUser+1]=pai2;

					m_nChupaiUser=m_nMjUser[15];
					for( i=15;i>m_nStartPosUser+1;i-- )
						m_nMjUser[i]=m_nMjUser[i-1];;

					
					m_nMjUser[m_nStartPosUser+2]=nPai;
					m_nMjUser[m_nStartPosUser+1]=pai1;
					m_nMjUser[m_nStartPosUser]=pai2;
	
				}

				m_nStartPosUser+=3;
				DrawPai();
				UpdateFrame();
		}////只可以有一种吃法的
        else///有两种或者三种吃法的,定时提示....
		{
			int k=0;
			if( m1 )
			{
				m_nEatPos[k++]=pos1;
				m_nEatPos[k++]=pos2;
			}
			if( m2 )
			{
				m_nEatPos[k++]=pos3;
				m_nEatPos[k++]=pos4;
			}
			if( m3 )
			{
				m_nEatPos[k++]=pos5;
				m_nEatPos[k++]=pos6;
			}
	
			m_nEatSelect=0;//m_nEatTeam;
		//	ReDrawBG(m_nEatPos[0],TRUE);
		//	ReDrawBG(m_nEatPos[1],TRUE);
			SetTimer(TIME_SELECT,1000,NULL);
			OnTimer(TIME_SELECT);
		//	CClientDC dc(this);
		//	dc.BitBlt(107+m_nEatPos[m_nEatSelect*2]*39,575,32,18,&m_dcBG,372,78,SRCCOPY);
		//	dc.BitBlt(107+m_nEatPos[m_nEatSelect*2+1]*39,575,32,18,&m_dcBG,420,78,SRCCOPY);
	

			
		}

	}
		
}

void CCMajiangGameDlg::CheckPeng(int nPai)
{
	int pos1,pos2;
		int pai1,pai2;
		int i;
			pos1=FindPaiPos(nPai,m_nStartPosUser);
			pai1=m_nMjUser[pos1];
			for( i=pos1;i>m_nStartPosUser;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;
			m_nMjUser[m_nStartPosUser]=pai1;

			pos2=FindPaiPos(nPai,pos1+1);

			pai2=m_nMjUser[pos2];
			for( i=pos2;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			m_nMjUser[m_nStartPosUser+1]=pai2;

			m_nChupaiUser=m_nMjUser[15];
			for( i=15;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

		
			m_nMjUser[m_nStartPosUser+2]=pai2;
			m_nMjUser[m_nStartPosUser+1]=pai1;
			m_nMjUser[m_nStartPosUser]=nPai;
			
			m_nStartPosUser+=3;
	

			DrawPai();
			UpdateFrame();


	
		
}

void CCMajiangGameDlg::OnTimer(UINT nIDEvent) 
{
		// TODO: Add your message handler code here and/or call default

	if( nIDEvent==TIME_SELECT )//吃牌提示效果
	{
		UpdateFrame();
		CClientDC dc(this);
		dc.BitBlt(102+m_nEatPos[m_nEatSelect*2]*39,572,32,18,&m_dcBG,372,78,SRCCOPY);
		dc.BitBlt(102+m_nEatPos[m_nEatSelect*2+1]*39,572,32,18,&m_dcBG,420,78,SRCCOPY);

		m_nEatSelect++;
		if( m_nEatSelect>=m_nEatTeam )m_nEatSelect=0;

	}
	else if( nIDEvent==TIME_DELAY ) //空闲统计
	{

		m_nDelay++;
		if( m_nDelay>= 8 )
		{
			CString strFile;
			m_nDelay=0;
			strFile.Format("Sound\\g_Hurry%02d.wav",rand()%11);
			::sndPlaySound(strFile,SND_ASYNC|SND_NODEFAULT);
			DrawEyes(E_BUNAI);
			DrawFace(F_OPEN1);
			Sleep(500);
			UpdateFrame();

		}
	}
	else if( nIDEvent==TIME_FLASH ) //空闲统计
	{
		static int step=0;
		step>=10000?step=0:step++;
		CClientDC dc(this);
		if( step%2 )
			dc.BitBlt(647-(m_nOutPaiComputerNumber-1)*31,422,31,48,&m_dcMem, 647-(m_nOutPaiComputerNumber-1)*31,422,SRCCOPY);

		else
		{
			if( m_nOutPaiUserNumber<10 )
				dc.BitBlt(647-(m_nOutPaiComputerNumber-1)*31,422,31,48,&m_dcBG,647-(m_nOutPaiComputerNumber-1)*31,422,SRCCOPY);
			else
				dc.BitBlt(647-(m_nOutPaiComputerNumber-1)*31,422,31,34,&m_dcBG,647-(m_nOutPaiComputerNumber-1)*31,422,SRCCOPY);
		}

	}
	CDialog::OnTimer(nIDEvent);
}

void CCMajiangGameDlg::CheckPengSelect( int nPai)
{
	int i;
	int pai1,pai2;
	int pos1,pos2;
	if( m_nEatSelect==0 )m_nEatSelect=m_nEatTeam-1;
	else m_nEatSelect--;
	pos1=m_nEatPos[m_nEatSelect*2];
	pos2=m_nEatPos[m_nEatSelect*2+1];

	pai1=m_nMjUser[pos1];
	for( i=pos1;i>m_nStartPosUser;i-- )
		m_nMjUser[i]=m_nMjUser[i-1];	
	m_nMjUser[m_nStartPosUser]=pai1;

	
			pai2=m_nMjUser[pos2];
			for( i=pos2;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			m_nMjUser[m_nStartPosUser+1]=pai2;

			m_nChupaiUser=m_nMjUser[15];
			for( i=15;i>m_nStartPosUser+1;i-- )
				m_nMjUser[i]=m_nMjUser[i-1];;

			
			m_nMjUser[m_nStartPosUser+2]=pai2;
			m_nMjUser[m_nStartPosUser+1]=pai1;
			m_nMjUser[m_nStartPosUser]=nPai;
			
			m_nStartPosUser+=3;
	
			DrawPai();
			UpdateFrame();

}


void CCMajiangGameDlg::ShowOver(int type)
{
	CString str;
	m_bOver=TRUE;
	KillTimer(TIME_DELAY);
	m_dcMem.BitBlt(725,300,65,50*4,&m_dcBG,725,300,SRCCOPY);
	switch(type)
	{
	case S_LIU:
		str="流  局";
		break;
	case S_ZIMO:
		str="自  摸";
		break;
	case S_HU:
		str="胡";
		break;
	}

	DrawPai();

	CFont ft,*oldft;
	CRect rect(10,160,800,600);
	ft.CreatePointFont(800,"隶书",NULL);
	oldft=m_dcMem.SelectObject(&ft);
	m_dcMem.SetTextColor(RGB(255,0,0));
	m_dcMem.SetBkMode(TRANSPARENT);
	m_dcMem.DrawText(str,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER     );
	m_dcMem.SelectObject(oldft);

	CPicture pic2;
	pic2.Load("images\\start1.jpg");
	pic2.Render(&m_dcMem,m_rtBtRestart);

	UpdateFrame();



	
}

void CCMajiangGameDlg::PlayPaiSound(int nPai,BOOL bGirl)
{
	CString strFile;
	if( nPai<9 )
		strFile.Format("One%02d.wav",nPai%9+1);
	else if( nPai<18 )
		strFile.Format("Ton%02d.wav",nPai%9+1);
	else if( nPai<27 )
		strFile.Format("Sou%02d.wav",nPai%9+1);
	else
	{
		switch(nPai)
		{
		case 27:
			strFile="Wind_E.wav";
			break;
		case 28:
			strFile="Wind_S.wav";
			break;
		case 29:
			strFile="Wind_W.wav";
			break;
		case 30:
			strFile="Wind_N.wav";
			break;
		case 31:
			strFile="Word_Jone.wav";
			break;
		case 32:
			strFile="Word_Far.wav";
			break;
		case 33:
			strFile="Word_Bai.wav";
			break;
		}
	}
	if( bGirl )
		strFile="g_"+strFile;
	else
		strFile="b_"+strFile;

	::sndPlaySound("sound\\"+strFile,SND_ASYNC|SND_NODEFAULT);		

}

void CCMajiangGameDlg::PlayChessSound(int type,BOOL bGirl,BOOL bNor)
{
	CString strFile;
	switch(type)
	{
	case S_HU:
		strFile="Op_Hu.wav";
		break;
	case S_GUN:
		strFile="Op_Gun.wav";
		break;
	case S_ZIMO:
		strFile="Op_Jimo.wav";
		break;
	case S_EAT:
		strFile="Op_Eat.wav";
		break;
	case S_PON:
		strFile="Op_Pon.wav";
		break;
	case S_LIU:
		strFile="Luju.wav";
		break;
	case S_STATE:
		strFile="Others03.wav";
		break;
	case S_MOU:
		strFile="Others00.wav";
		break;
	case S_DAO:
		strFile="Others07.wav";
		break;


	}
	if( !bNor )
	{
		::sndPlaySound("sound\\"+strFile,SND_ASYNC|SND_NODEFAULT);
		return;
	}

	if( bGirl )
		strFile="g_"+strFile;
	else
		strFile="b_"+strFile;
	
	::sndPlaySound("sound\\"+strFile,SND_ASYNC|SND_NODEFAULT);		

}

void CCMajiangGameDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	::SetCursor(m_hCursorDown);
	m_bHandDown=TRUE;
	CDialog::OnLButtonDown(nFlags, point);
	MSG message;
	while(::PeekMessage(&message,this->m_hWnd,WM_LBUTTONDOWN,WM_LBUTTONDOWN,PM_REMOVE));

}

void CCMajiangGameDlg::MouseMoveOnTitle(CPoint point)
{
	BOOL bFind=FALSE;
	for( int i=0;i<4;i++ )
		if( m_rtTitle[i].PtInRect(point) )
		{
			if( m_nMouseOnTitle==i )return;
		//	Invalidate();
			CClientDC dc(this);
			CPicture pic;
			pic.Load("images\\title2.jpg");
			pic.Render(&m_dcMem);
			dc.BitBlt(m_rtTitle[i].left,m_rtTitle[i].top,m_rtTitle[i].right-m_rtTitle[i].left,m_rtTitle[i].bottom-m_rtTitle[i].top,&m_dcMem,m_rtTitle[i].left,m_rtTitle[i].top,SRCCOPY);
			m_nMouseOnTitle=i;
			PlayChessSound(S_MOU,TRUE,FALSE);
			bFind=TRUE;
			break;
		}
	if( m_nMouseOnTitle!=-1 && bFind==FALSE )
	{
		m_nMouseOnTitle=-1;
		Invalidate();
	}
	
	
}

void CCMajiangGameDlg::OnClickTitle()
{

	switch( m_nMouseOnTitle )
	{
	case 0:
		m_bTitle=FALSE;
		ResetMj();
		break;
	case 2:
		EndDialog(IDOK);
		break;
	case 3:
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		break;

	}
}

void CCMajiangGameDlg::DrawFace(int type)
{
	CString strFile;
	switch( type )
	{
	case F_OPEN1:
		strFile="Open1.jpg";
		break;
	case F_OPEN2:
		strFile="Open2.jpg";
		break;
	case F_CLOSE:
		strFile="close.jpg";
		break;

	}
	CClientDC dc(this);
	CPicture pic;
	pic.Load("images\\"+strFile);
	pic.Render(&m_dcBuffer);
	dc.BitBlt(388,127,50,20,&m_dcBuffer,388,127,SRCCOPY);

}

void CCMajiangGameDlg::DrawEyes(int type)
{
	CString strFile;
	switch( type )
	{
	
	case E_XI:
		strFile="E_XI.jpg";
		break;
	case E_SAI:
		strFile="E_SAI.jpg";
		break;
	case E_NU:
		strFile="E_NU.jpg";
		break;
	case E_LE:
		strFile="E_LE.jpg";
		break;
	case E_BUNAI:
		strFile="E_BUNAI.jpg";
		break;
	case E_YUYE:
		strFile="E_YUYE.jpg";
		break;


	}
	CClientDC dc(this);
	CPicture pic;
	pic.Load("images\\"+strFile);
	pic.Render(&m_dcBuffer);
	dc.BitBlt(355,35,108,134,&m_dcBuffer,355,35,SRCCOPY);
}

void CCMajiangGameDlg::PlayMinganSound(int type)
{
	CString strFile;
	switch( type )
	{
	
	case E_NU:
		strFile.Format("g_hate%02d.wav",rand()%3);
		break;
	case E_LE:
		strFile.Format("g_Touch%02d.wav",rand()%4);
		break;
	case E_YUYE:
		strFile.Format("g_Sexy%02d.wav",rand()%5);
		break;

	}

	::sndPlaySound("sound\\"+strFile,SND_ASYNC|SND_NODEFAULT);
}

void CCMajiangGameDlg::SetTrans(int pos,BOOL who)
{
	COLORREF cr;
	int r1,g1,b1;

	for( int i=0;i<31;i++ )
		for( int j=0;j<48;j++ )
		{
			if( who )
				cr=m_dcMj.GetPixel(m_nOutPaiUser[pos]*31+i,129+j);//m_dcBG.GetPixel(95+pos*39+i,518+j);
			else
				cr=m_dcMj.GetPixel(m_nOutPaiComputer[pos]*31+i,176+j);//m_dcBG.GetPixel(95+pos*39+i,518+j);
			r1=100;//GetRValue(cr)*0.8;
			g1=GetGValue(cr);//*0.8;
			b1=GetBValue(cr);//*0.8;
	//		cr=m_dcMj.GetPixel(m_nMjUser[m_nPai[pos]]*39+i,64+j);
	//		r2=0;//GetRValue(cr)*0.2;
	//		g2=GetGValue(cr)*0.2;
	//		b2=GetBValue(cr)*0.2;
			if( who )
				m_dcMem.SetPixel(120+pos*31+i,456+j,RGB(r1,g1,b1));
			else
				m_dcMem.SetPixel(647-pos*31+i,422+j,RGB(r1,g1,b1));

		}
}

int CCMajiangGameDlg::ComputerThink(int pai)
{
//	return 0;
	int a[17],i;
//	CMajiangHufa mf;
	CThinkPklang bb;
	for(i=0;i<16-m_nStartPosComputer;i++)
		a[i]=m_nMjComputer[i+m_nStartPosComputer];
	a[16-m_nStartPosComputer]=pai;

	bb.SetMjPaiArray(a,16-m_nStartPosComputer+1);
	bb.Think();

	for( i=0;i<m_arrComHupai.GetSize();i++ )
		if( pai==m_arrComHupai.GetAt(i) )
		{
			bb.GetHupaiTeam(m_nHupaiTeam);
			return 1000;
		}

	m_nComputerGoodScore=bb.GetMaxScore();
	int ret=bb.GetBadly();
    if( pai!=ret )
	{
		for( i=m_nStartPosComputer;i<16;i++ )
			if( m_nMjComputer[i]==ret )
			{
				m_nMjComputer[i]=pai;
				
				break;
			}
		SortMj(m_nMjComputer,m_nStartPosComputer);
	}
	return ret;


}

void CCMajiangGameDlg::FindHu()
{
		for( int i=0;i<m_arrHupai.GetSize();i++ )
			if( m_nChupaiUser==m_arrHupai.GetAt(i) )
			{
				m_bHu=TRUE;
				m_bZimo=TRUE;
				m_dcMem.BitBlt(725,300+50*3,65,40,&m_dcState,1,53*4+1,SRCCOPY);
				UpdateFrame();
				break;
			}

}

int CCMajiangGameDlg::CheckNeedGet(int pai)
{
//	return -1;
	

	int a[17],i,j;
	CThinkPklang bb;
	for(i=0;i<16-m_nStartPosComputer;i++)
		a[i]=m_nMjComputer[i+m_nStartPosComputer];


	a[16-m_nStartPosComputer]=pai;
	bb.SetMjPaiArray(a,17-m_nStartPosComputer);
	bb.Think();

	for( i=0;i<m_arrComHupai.GetSize();i++ )
		if( pai==m_arrComHupai.GetAt(i) )
		{
			bb.GetHupaiTeam(m_nHupaiTeam);
			return 1000;
		}

	int ret=bb.GetBadly();
	if( ret==pai )return -1;
	if( bb.GetMaxScore() > m_nComputerGoodScore+150 )
	{
		m_nComputerGoodScore=bb.GetMaxScore()+m_arrComHupai.GetSize()*1000;

		int pai1,pai2,pai3,pos1,pos2,posbad;
		bb.GetGoodTeam(pai1,pai2,pai3);


		posbad=FindPaiComPos(ret,m_nStartPosComputer);
	//	m_nMjComputer[posbad]=m_nMjComputer[m_nStartPosComputer];;
		if( pai1==pai2 )
		{
			pos1=FindPaiComPos(pai1,m_nStartPosComputer);
			pos2=FindPaiComPos(pai1,pos1+1);
			PlayChessSound(S_PON);


		}
		else
		{
			PlayChessSound(S_EAT);

			if( pai1== pai )
			{
				pos1=FindPaiComPos(pai2,m_nStartPosComputer);
				pos2=FindPaiComPos(pai3,m_nStartPosComputer);


			}
			else if( pai2==pai )
			{
				pos1=FindPaiComPos(pai1,m_nStartPosComputer);
				pos2=FindPaiComPos(pai3,m_nStartPosComputer);




			}
			else
			{
				pos1=FindPaiComPos(pai1,m_nStartPosComputer);
				pos2=FindPaiComPos(pai2,m_nStartPosComputer);


			}

		}
/*		if( posbad==-1 || pos1==-1 || pos2==-1 )
		{
			CString str;
			str.Format("%d,%d,%d",pai1,pai2,pai3);
			AfxMessageBox(str);
		}
		*/
		m_nMjComputer[posbad]=-1;//m_nMjComputer[m_nStartPosComputer];
		m_nMjComputer[pos1]=-1;//m_nMjComputer[m_nStartPosComputer+1];
		m_nMjComputer[pos2]=-1;//m_nMjComputer[m_nStartPosComputer+2];;

		for( i=m_nStartPosComputer;i<16;i++ )
		{
			if( m_nMjComputer[i]==-1 )
			{
				for( j=i;j>m_nStartPosComputer;j-- )
					m_nMjComputer[j]=m_nMjComputer[j-1];
			}
		}


			
		m_nMjComputer[m_nStartPosComputer+2]=pai3;
		m_nMjComputer[m_nStartPosComputer+1]=pai2;
		m_nMjComputer[m_nStartPosComputer]=pai1;

		m_nStartPosComputer+=3;
		
		SortMj(m_nMjComputer,m_nStartPosComputer);
		

			return ret;//-1;
	}
	return -1;


}



int CCMajiangGameDlg::CalculateTaiNumber(int who,BOOL bZimo)
{

	CString huname[10];
	int n=0,taishu[10],totaltai=0,ret;



	if( bZimo==TRUE )
	{
		if( GetMenqing(who) )
		{
			huname[n]="门清自摸";
			taishu[n]=2;
			n++;
		}
		else
		{
			huname[n]="自摸";
			taishu[n]=1;
			n++;

		}

	}//zimo
	else
	{
		if( GetMenqing(who) )
		{
			huname[n]="门清";
			taishu[n]=1;
			n++;

		}
	}
	if( GetQuanqiu(who) )
	{
		huname[n]="全求";
		taishu[n]=2;
		n++;

	}
	if( GetDuting(who) )
	{
		huname[n]="独听";
		taishu[n]=1;
		n++;

	}
	if( GetHaidilaoyue(who) )
	{
		huname[n]="海底捞月";
		taishu[n]=1;
		n++;

	}
	if( GetPengpenghu(who) )
	{
		huname[n]="碰碰胡";
		taishu[n]=4;
		n++;

	}
	if( GetPinghu(who) )
	{
		huname[n]="平胡";
		taishu[n]=2;
		n++;

	}

	ret=GetSixiPai(who);
	if( ret==16 )
	{
		huname[n]="大四喜";
		taishu[n]=16;
		n++;

	}
	else if( ret==8 )
	{
		huname[n]="小四喜";
		taishu[n]=8;
		n++;

	}

	ret=GetSanyuanpai(who);
	if( ret==8 )
	{
		huname[n]="大三元";
		taishu[n]=8;
		n++;

	}
	else if( ret==4 )
	{
		huname[n]="小三元";
		taishu[n]=4;
		n++;

	}
	else if( ret>0 )
	{
		huname[n]="三元牌";
		taishu[n]=ret;
		n++;

	}

	ret=GetQingyise(who);
	if( ret==8 )
	{
		huname[n]="清一色";
		taishu[n]=8;
		n++;

	}
	else if( ret==4 )
	{
		huname[n]="混一色";
		taishu[n]=4;
		n++;

	}

	ret=GetSananke(who);
	if( ret==5 )
	{
		huname[n]="五暗刻";
		taishu[n]=8;
		n++;

	}
	else if( ret==4 )
	{
		huname[n]="四暗刻";
		taishu[n]=4;
		n++;

	}
	else if( ret==3 )
	{
		huname[n]="三暗刻";
		taishu[n]=4;
		n++;

	}

    if( m_bGangSKH )
	{
		huname[n]="杠上开花";
		taishu[n]=1;
		n++;
	}


	if( n==0 )
	{
		huname[n]="推倒胡";
		taishu[n]=1;
		n++;
	}

	CPicture pic;
	pic.Load("images\\HupaiTable.JPG");
	pic.Render(&m_dcMem,CRect(198,136,426+198,213+136));

	
	CFont ft,*oldft;
	CString str;
	CRect rect(10,160,800,600);
	ft.CreatePointFont(200,"隶书",NULL);
	oldft=m_dcMem.SelectObject(&ft);
	m_dcMem.SetTextColor(RGB(255,255,255));
	m_dcMem.SetBkMode(TRANSPARENT);

	int col=n/5;
	for( int i=0;i<col;i++ )
		for( int j=0;j<5;j++ )
		{
			str.Format("%-8s%2d台",huname[i*5+j],taishu[i*5+j]);
			m_dcMem.TextOut(230+i*200,166+j*25,str);
			totaltai+=taishu[i*5+j];

		}
	for( i=0;i<n%5;i++ )
	{
		str.Format("%-8s%2d台",huname[col*5+i],taishu[col*5+i]);
		m_dcMem.TextOut(230+col*200,166+i*25,str);
		totaltai+=taishu[col*5+i];

	}
	m_dcMem.SetTextColor(RGB(255,255,0));
	str.Format("合计%6d台",totaltai);
	m_dcMem.TextOut(230,176+5*25,str);
	m_dcMem.SelectObject(oldft);
	

	POINT pt;
	::GetCursorPos(&pt);
	SetCursorPos(pt.x,pt.y);
	
	UpdateFrame();
	

/*
	CString str;;
	int col=n/5;
	for( int i=0;i<col;i++ )
		for( int j=0;j<5;j++ )
		{
			str=str+huname[i*5+j];
			str+="\r\n";


		}
	for( i=0;i<n%5;i++ )
	{
		str=str+huname[col*5+i];
		str+="\r\n";

	}
		AfxMessageBox(str);
*/
	return 0;
}


////////计算[成牌台数
int CCMajiangGameDlg::GetMenqing(int type)//门清
{

	int n=0;
	if( type==1 )
	{
		for( int i=0;i<5;i++)
			if( m_nGangUser[i]==1 )n++;
		if( m_nStartPosUser-n*3==0 )return 1;
	}
	else
	{
		for( int i=0;i<5;i++)
			if( m_nGangComputer[i]==1 )n++;
		if( m_nStartPosComputer-n*3==0 )return 1;
	}
	return 0;

}

int CCMajiangGameDlg::GetSanyuanpai(int type)//三元牌
{
	int ret=0,pos1,pos2,pos3;
	BOOL bFind[3],bXiaosnayuan=FALSE;//小三元
	bFind[0]=bFind[1]=bFind[2]=FALSE;
	if( type==2 )
	{
		for( int i=0;i<3;i++ )
		{
			pos1=FindPaiComPos(31+i);
			pos2=FindPaiComPos(31+i,pos1+1);
			pos3=FindPaiComPos(31+i,pos2+1);
			if( pos1!=-1 && pos2!=-1 && pos3!=-1 )
			{
				bFind[i]=TRUE;
				ret++;
			}
		}
		if( ret==2 )
		{
			if( bFind[0]==FALSE )
			{
				pos1=FindPaiComPos(31);
				pos2=FindPaiComPos(31,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaosnayuan=TRUE;
			
			}
			else if( bFind[1]==FALSE )
			{
				pos1=FindPaiComPos(32);
				pos2=FindPaiComPos(32,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaosnayuan=TRUE;
			
			}
			else
			{
				pos1=FindPaiComPos(33);
				pos2=FindPaiComPos(33,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaosnayuan=TRUE;
			
			}
		}

	}
	else
	{
		for( int i=0;i<3;i++ )
		{
			pos1=FindPaiPos(31+i);
			pos2=FindPaiPos(31+i,pos1+1);
			pos3=FindPaiPos(31+i,pos2+1);
			if( pos1!=-1 && pos2!=-1 && pos3!=-1 )
			{
				bFind[i]=TRUE;
				ret++;
			}

		}
		if( ret==2 )
		{
			if( bFind[0]==FALSE )
			{
				pos1=FindPaiPos(31);
				pos2=FindPaiPos(31,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaosnayuan=TRUE;
			
			}
			else if( bFind[1]==FALSE )
			{
				pos1=FindPaiPos(32);
				pos2=FindPaiPos(32,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaosnayuan=TRUE;
			
			}
			else
			{
				pos1=FindPaiPos(33);
				pos2=FindPaiPos(33,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaosnayuan=TRUE;
			
			}
		}

	}

	if( ret==3 ) return 8;//大三元
	if( ret==2 && bXiaosnayuan==TRUE )return 4;//是小三元
	return ret;



}

int CCMajiangGameDlg::GetPinghu(int type)//平胡
{
	int i;
	if( type==1 )
	{
		for( i=0;i<m_nStartPosUser/3;i++ )
			if( m_nMjUser[i*3]==m_nMjUser[i*3+1] )return 0;
		for( i=0;i<(15-m_nStartPosUser)/3;i++ )
			if( m_nHupaiTeam[i*3]==m_nHupaiTeam[i*3+1] )return 0;


	}
	else
	{
		for( i=0;i<m_nStartPosComputer/3;i++ )
			if( m_nMjComputer[i*3]==m_nMjComputer[i*3+1] )return 0;
		for( i=0;i<(15-m_nStartPosComputer)/3;i++ )
			if( m_nHupaiTeam[i*3]==m_nHupaiTeam[i*3+1] )return 0;

	}
	
	return 2;


}

int CCMajiangGameDlg::GetSananke(int type)//N暗刻
{
	int i,n=0;
	if( type==1 )
	{
		for( i=0;i<m_nStartPosUser/3;i++ )
			if( m_nGangUser[i]==1 && m_nMjUser[i*3]==m_nMjUser[i*3+1] )n++;
		for( i=0;i<(15-m_nStartPosUser)/3;i++ )
			if( m_nHupaiTeam[i*3]==m_nHupaiTeam[i*3+1] )n++;


	}
	else
	{
		for( i=0;i<m_nStartPosComputer/3;i++ )
			if(  m_nGangComputer[i]==1 && m_nMjComputer[i*3]==m_nMjComputer[i*3+1] )n++;
		for( i=0;i<(15-m_nStartPosComputer)/3;i++ )
			if( m_nHupaiTeam[i*3]==m_nHupaiTeam[i*3+1] )n++;

	}
	return n;

}

int CCMajiangGameDlg::GetPengpenghu(int type)//碰碰胡
{
	int i,n=0;
	if( type==1 )
	{
		for( i=0;i<m_nStartPosUser/3;i++ )
			if( m_nMjUser[i*3]==m_nMjUser[i*3+1] )n++;
		for( i=0;i<(15-m_nStartPosUser)/3;i++ )
			if( m_nHupaiTeam[i*3]==m_nHupaiTeam[i*3+1] )n++;


	}
	else
	{
		for( i=0;i<m_nStartPosComputer/3;i++ )
			if( m_nMjComputer[i*3]==m_nMjComputer[i*3+1] )n++;
		for( i=0;i<(15-m_nStartPosComputer)/3;i++ )
			if( m_nHupaiTeam[i*3]==m_nHupaiTeam[i*3+1] )n++;

	}
	if( n==5 )
		return 4;

	return 0;
}



int CCMajiangGameDlg::GetQingyise(int type)//清一色
{
	int i,n=0;
	if( type==1 )
	{
		for( i=0;i<15;i++ )
			if( m_nMjUser[i]<27 && m_nMjUser[i+1]<27)
			{
				if(m_nMjUser[i]/9==m_nMjUser[i+1]/9 )n++;
				else return 0;
			}
	}
	else
	{
		for( i=0;i<15;i++ )
			if( m_nMjComputer[i]<27 && m_nMjComputer[i+1]<27 )
			{
				if(m_nMjComputer[i]/9==m_nMjComputer[i+1]/9 )n++;
				else return 0;
			}

	}
	
	if( n>=15 )
		return 8;//清一色

	return 4;//混一色

}

int CCMajiangGameDlg::GetSixiPai(int type)//大四喜或小四喜
{
	int ret=0,pos1,pos2,pos3;
	BOOL bFind[4],bXiaoSixi=FALSE;//小四喜
	bFind[0]=bFind[1]=bFind[2]=bFind[3]=FALSE;
	if( type==2 )
	{
		for( int i=0;i<4;i++ )
		{
			pos1=FindPaiComPos(27+i);
			pos2=FindPaiComPos(27+i,pos1+1);
			pos3=FindPaiComPos(27+i,pos2+1);
			if( pos1!=-1 && pos2!=-1 && pos3!=-1 )
			{
				bFind[i]=TRUE;
				ret++;
			}
		}
		if( ret==3 )
		{
			if( bFind[0]==FALSE )
			{
				pos1=FindPaiComPos(27);
				pos2=FindPaiComPos(27,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
			else if( bFind[1]==FALSE )
			{
				pos1=FindPaiComPos(28);
				pos2=FindPaiComPos(28,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
			else if( bFind[2]==FALSE )
			{
				pos1=FindPaiComPos(29);
				pos2=FindPaiComPos(29,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
			else
			{
				pos1=FindPaiComPos(30);
				pos2=FindPaiComPos(30,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
		}

	}
	else
	{
		for( int i=0;i<4;i++ )
		{
			pos1=FindPaiPos(27+i);
			pos2=FindPaiPos(27+i,pos1+1);
			pos3=FindPaiPos(27+i,pos2+1);
			if( pos1!=-1 && pos2!=-1 && pos3!=-1 )
			{
				bFind[i]=TRUE;
				ret++;
			}

		}
		if( ret==3 )
		{
			if( bFind[0]==FALSE )
			{
				pos1=FindPaiPos(27);
				pos2=FindPaiPos(27,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
			else if( bFind[1]==FALSE )
			{
				pos1=FindPaiPos(28);
				pos2=FindPaiPos(28,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
			else if( bFind[2]==FALSE )
			{
				pos1=FindPaiComPos(29);
				pos2=FindPaiComPos(29,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
			else
			{
				pos1=FindPaiPos(30);
				pos2=FindPaiPos(30,pos1+1);
				if( pos1!=-1 && pos2!=-1 )bXiaoSixi=TRUE;
			
			}
		}

	}


	if( ret==4 ) return 16;//大四喜
	if( ret==3 && bXiaoSixi==TRUE )return 8;//是小四喜
	return 0;


}

int CCMajiangGameDlg::GetHaidilaoyue(int type)//海底捞月
{

	if( type==1 )
	{
		if( m_nOutPaiUserNumber==17 )return 1;
	}
	else
	{
		if( m_nOutPaiComputerNumber==17 )return 1;
	}
	return 0;
}

int CCMajiangGameDlg::GetDuting(int type)//独听
{

	if( type==1 )
	{
		if( m_arrHupai.GetSize()==1 )return 1;
	}
	else
	{
		if( m_arrComHupai.GetSize()==1 )return 1;
	}
	return 0;

}

int CCMajiangGameDlg::GetQuanqiu(int type)//全求
{
	if( type==1 )
	{
		if( m_nStartPosUser==15 )return 1;
	}
	else
	{
		if( m_nStartPosComputer==15 )return 1;
	}
	return 0;
}

void CCMajiangGameDlg::Damanguan(BOOL m_bUser)
{
	int a[10][16]={
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,31,31,31,32,32,33,33},
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,31,31,32,33,33},
		{ 0, 0, 1, 1, 2, 3, 4, 4, 6, 6, 7,-1,-1,-1,-1,-1},
		{ 0, 1, 2, 2, 2, 3, 4, 4, 5,27,27,28,29,-1,-1,-1},
		{ 9,10,10,11,13,15,16,17,13,17,-1,-1,-1,-1,-1,-1},
		{ 9,10,10,10,14,14,15,17,13,17,15,-1,-1,-1,-1,-1},
		{18,18,19,20,20,23,24,26,26,-1,-1,-1,-1,-1,-1,-1},
		{18,19,19,20,23,23,24,25,26,27,27,29,-1,-1,-1,-1},
		{11,11,28, 30,30,26,26,27,27,27,28,29,29,30,-1,-1},
		{27, 27, 30, 19,20,27,30,28,28,28,28,29,29,-1,-1,-1},
	};
	int n=rand()%10;
	int *p;
	if( m_bUser )
		p=m_nMjUser;
	else p=m_nMjComputer;

	for( int i=0;i<16;i++ )
	{
		if( a[n][i]==-1 )
			p[i]=GetRndMj();
		else
		{
			if( GetFixMj(a[n][i]) )
				p[i]=a[n][i];
			else p[i]=GetRndMj();
		}
	}

}
