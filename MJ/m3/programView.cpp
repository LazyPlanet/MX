// programView.cpp : implementation of the CProgramView class
//

#include "stdafx.h"
#include "program.h"

#include "programDoc.h"
#include "programView.h"
#include "NetControl.h"
#include "Card.h"
#include "Managers.h"
#include "Enter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	Managers manager;
/////////////////////////////////////////////////////////////////////////////
// CProgramView

IMPLEMENT_DYNCREATE(CProgramView, CView)

BEGIN_MESSAGE_MAP(CProgramView, CView)
	//{{AFX_MSG_MAP(CProgramView)
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_BN_CLICKED(ID_GN,OnGn)
	ON_BN_CLICKED(ID_GO,OnGo)
	ON_BN_CLICKED(ID_CI,OnCi)
	ON_BN_CLICKED(ID_HU,OnHu)
	ON_BN_CLICKED(ID_START,OnStart)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramView construction/destruction

CProgramView::CProgramView()
{
	// TODO: add construction code here
	m_pControl = new NetControl(this);
/*	int	i(0);
	for(;i<=19;i++)
	{
		manager.PushCards[0][i].Num=2;
		manager.PushCards[0][i].Type=0;
		manager.PushCards[1][i].Num=4;
		manager.PushCards[1][i].Type=1;
		manager.PushCards[2][i].Num=5;
		manager.PushCards[2][i].Type=2;
		manager.PushCards[3][i].Num=6;
		manager.PushCards[3][i].Type=2;
	}
	for(i=0;i<=13;i++)
	{
		manager.PlayCards[0][i].Num=8;
		manager.PlayCards[0][i].Type=1;
		manager.PlayCards[0][i].Click=false;
		manager.PlayCards[1][i].Num=6;
		manager.PlayCards[1][i].Type=0;
		manager.PlayCards[2][i].Num=2;
		manager.PlayCards[2][i].Type=2;
		manager.PlayCards[3][i].Num=3;
		manager.PlayCards[3][i].Type=2;
	}
	for(i=0;i<=29;i++)
	{
		manager.PlayOuts[0][i].Num=8;
		manager.PlayOuts[0][i].Type=1;
		manager.PlayOuts[1][i].Num=6;
		manager.PlayOuts[1][i].Type=0;
		manager.PlayOuts[2][i].Num=2;
		manager.PlayOuts[2][i].Type=2;
		manager.PlayOuts[3][i].Num=3;
		manager.PlayOuts[3][i].Type=2;
	}
	for(i=0;i<=3;i++)
	{
		PlayerNames[i].Format("好好学习天天向上",i);
	}
*/
	move=15;
	manager.pView=this;
}

CProgramView::~CProgramView()
{

}

BOOL CProgramView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	m_bx.LoadBitmap(IDB_BX);
	m_by.LoadBitmap(IDB_BY);
	m_myfall.LoadBitmap(IDB_MYFALL);
	m_mystand.LoadBitmap(IDB_MYSTAND);
	m_bright.LoadBitmap(IDB_BRIGHT);
	m_bleft.LoadBitmap(IDB_BLEFT);
	m_bface.LoadBitmap(IDB_BFACE);
	m_xline.LoadBitmap(IDB_XLINE);
	m_yline.LoadBitmap(IDB_YLINE);
	m_background.LoadBitmap(IDB_BACKGROUND);
	m_cross.LoadBitmap(IDB_CROSS);
	m_button.LoadBitmap(IDB_BUTTON);
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CProgramView drawing

void CProgramView::OnDraw(CDC* pDC)
{
	CProgramDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	static	bool st=true;
	if(st)
	{	Back.CreateCompatibleDC(pDC);
		Cards.CreateCompatibleDC(pDC);
		Buffer.CreateCompatibleDC(pDC);
		MemBitmap.CreateCompatibleBitmap(pDC,900,620);
		Buffer.SelectObject(&MemBitmap);
		Buffer.FillSolidRect(0,0,900,620,RGB(255,255,255));
		st=false;
	}
	PrintAll();
}

/////////////////////////////////////////////////////////////////////////////
// CProgramView printing

BOOL CProgramView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CProgramView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CProgramView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CProgramView diagnostics

#ifdef _DEBUG
void CProgramView::AssertValid() const
{
	CView::AssertValid();
}

void CProgramView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CProgramDoc* CProgramView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProgramDoc)));
	return (CProgramDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProgramView message handlers

	void CProgramView::DrawPart(int start,int end,CDC *pDC)
{
	
	int	k(0);

		
	bool control=false;
	if((end-start)%2!=0)
		control=true;

//	pDC->BitBlt(50,50,23,23,&Back,0,0,SRCCOPY);
//	pDC->BitBlt(50,42,23,23,&Back,0,0,SRCCOPY);
//	pDC->BitBlt(50,66,23,23,&Back,0,0,SRCCOPY);
//	pDC->BitBlt(50,58,23,23,&Back,0,0,SRCCOPY);
	
	if(start>=0&&start<=25)
	{
		Back.SelectObject(&m_bx);
		for(;end>=start+2;end--)
		{
				pDC->BitBlt(165,133+((25-end)/2)*16,23,23,&Back,0,0,SRCCOPY);//下面，后面差8
				pDC->BitBlt(165,125+((25-end)/2)*16,23,23,&Back,0,0,SRCCOPY);//上面
		}
		pDC->BitBlt(165,133+((25-start)/2)*16,23,23,&Back,0,0,SRCCOPY);
	
		if(control)
		{
			pDC->BitBlt(165,125+((25-start)/2)*16,23,23,&Back,0,0,SRCCOPY);
		}
		
	}
//测试
/*	Back.SelectObject(&m_bx);
	for(k=0;k<=25;k++)
	{	if(k%2==0)
		{
			pDC->BitBlt(480,330+(k/2)*16,23,23,&Back,0,0,SRCCOPY);//下面，后面差8
		}
		else
		{
			pDC->BitBlt(480,322+(k/2)*16,23,23,&Back,0,0,SRCCOPY);//上面
		}
	}
*/
	if(start>=26&&start<=53)
	{
		Back.SelectObject(&m_by);
		pDC->BitBlt(310+((start-26)/2)*18,95,18,28,&Back,0,0,SRCCOPY);
		
			for(;end>=start+2;end--)
		{
			pDC->BitBlt(310+((end-26)/2)*18,95,18,28,&Back,0,0,SRCCOPY);//下面，后面差8
			pDC->BitBlt(310+((end-26)/2)*18,90,18,28,&Back,0,0,SRCCOPY);//上面
		}
		if(control)
			pDC->BitBlt(310+((start-26)/2)*18,90,18,28,&Back,0,0,SRCCOPY);
	}	
/*测试	
	Back.SelectObject(&m_by);
	for(k=0;k<=27;k++)
	{	if(k%2==0)
		{
			pDC->BitBlt(510+(k/2)*18,280,18,28,&Back,0,0,SRCCOPY);//下面，后面差8
		}
		else
		{
			pDC->BitBlt(510+(k/2)*18,275,18,28,&Back,0,0,SRCCOPY);//上面
		}
	}
*/
//	pDC->BitBlt(50,66,23,23,&Back,0,0,SRCCOPY);//前面差16
//	pDC->BitBlt(50,58,23,23,&Back,0,0,SRCCOPY);
//	pDC->BitBlt(50,82,23,23,&Back,0,0,SRCCOPY);
	if(start>=54&&start<=79)
	{	
		Back.SelectObject(&m_bx);
		pDC->BitBlt(700,133+((start-54)/2)*16,23,23,&Back,0,0,SRCCOPY);

		if(control)
		{
			pDC->BitBlt(700,125+((start-54)/2)*16,23,23,&Back,0,0,SRCCOPY);
		}

		for(;start+2<=end;start++)
		{
				pDC->BitBlt(700,133+((start+2-54)/2)*16,23,23,&Back,0,0,SRCCOPY);//下面，后面差8
				pDC->BitBlt(700,125+((start+2-54)/2)*16,23,23,&Back,0,0,SRCCOPY);//上面
		}
			
	}
/*测试

	Back.SelectObject(&m_bx);
	for(k=0;k<=25;k++)
	{	if(k%2==0)
		{
			pDC->BitBlt(760,330+(k/2)*16,23,23,&Back,0,0,SRCCOPY);//下面，后面差8
		}
		else
		{
			pDC->BitBlt(760,322+(k/2)*16,23,23,&Back,0,0,SRCCOPY);//上面
		}
	}

*/
	if(start>=80&&start<=107)
	{
		Back.SelectObject(&m_by);
		pDC->BitBlt(310+((107-start)/2)*18,450,18,28,&Back,0,0,SRCCOPY);
		
			for(;end>=start+2;end--)
		{
			pDC->BitBlt(310+((107-end)/2)*18,450,18,28,&Back,0,0,SRCCOPY);//下面，后面差8
			pDC->BitBlt(310+((107-end)/2)*18,445,18,28,&Back,0,0,SRCCOPY);//上面
		}
		
		if(control)
			pDC->BitBlt(310+((107-start)/2)*18,445,18,28,&Back,0,0,SRCCOPY);	

	}
/*测试	
	Back.SelectObject(&m_by);
	for(k=0;k<=27;k++)
	{	if(k%2==0)
		{
			pDC->BitBlt(500+(k/2)*18,555,18,28,&Back,0,0,SRCCOPY);
		}
		else
		{
			pDC->BitBlt(500+(k/2)*18,550,18,28,&Back,0,0,SRCCOPY);
		}
	}
*/	
//	pDC->BitBlt(100,100,18,28,&Back,0,0,SRCCOPY);//下面，左边差5
//	pDC->BitBlt(100,95,18,28,&Back,0,0,SRCCOPY);//上面
//	pDC->BitBlt(118,100,18,28,&Back,0,0,SRCCOPY);//右边差18
//	pDC->BitBlt(118,95,18,28,&Back,0,0,SRCCOPY);

}

void CProgramView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int	mx=point.x;
	int	my=point.y;



	
	if(mx>=165&&my>=500&&mx<=845&&my<=571)
	{
		if(mx>=795&&mx<=845)
			mx=13;
		else
			mx=(mx-795+(SelectNum()+1)*45)/45-1;
		if(mx!=move)
		{
			manager.PlayCards[manager.PlayerAc][mx].Click=true;
			manager.PlayCards[manager.PlayerAc][move].Click=false;
			move=mx;
		}
	
	}
	else
	{	manager.PlayCards[manager.PlayerAc][move].Click=false;
		move=15;
	}
	PrintAll();

/*	CDC	*pDC=GetDC();
	CString	st;
	st.Format("%d   ,%d   ",mx,my);
	pDC->TextOut(400,5,st);
	st.ReleaseBuffer();
	ReleaseDC(pDC);
*/	
	CView::OnMouseMove(nFlags, point);
}

void CProgramView::DrawWhole(int start, int end, CDC *pDC)
{
	if(start>=0&&start<=25)
	{
		if(end>=0&&end<=25)
			DrawPart(start,end,pDC);
		else if(end>=26&&end<=53)
		{
			DrawPart(start,25,pDC);
			DrawPart(26,end,pDC);
		}
		else if(end>=54&&end<=79)
		{
			DrawPart(start,25,pDC);
			DrawPart(26,53,pDC);
			DrawPart(54,end,pDC);
		}
		else if(end>=80&&end<=107)
		{
			DrawPart(start,25,pDC);
			DrawPart(26,53,pDC);
			DrawPart(54,79,pDC);
			DrawPart(80,end,pDC);
		}
	}
	else if(start>=26&&start<=53)
	{
		if(end>=26&&end<=53)
			DrawPart(start,end,pDC);
		else if(end>=54&&end<=79)
		{
			DrawPart(start,53,pDC);
			DrawPart(54,end,pDC);
		}
		else if(end>=80&&end<=107)
		{
			DrawPart(start,53,pDC);
			DrawPart(54,79,pDC);
			DrawPart(80,end,pDC);
		}
	}
	else if(start>=54&&start<=79)
	{
		if(end>=54&&end<=79)
			DrawPart(start,end,pDC);
		else if(end>=80&&end<=107)
		{
			DrawPart(start,79,pDC);
			DrawPart(80,end,pDC);
		}
	}
	else if(start>=80&&start<=107)
		DrawPart(start,end,pDC);
}

void CProgramView::DrawWall(int start, int end, CDC *pDC)
{
	if(end>=start)
		DrawWhole(start,end,pDC);
	else
	{
		DrawWhole(start,107,pDC);
		DrawWhole(0,end,pDC);
	}
}

void CProgramView::DrawStandCards(CDC *pDC)
{
	//当前玩家站立麻将 45*71
	int	i;
	int	k=SelectNum()-1;
	Cards.SelectObject(&m_mystand);

	for(i=0;i<=k;i++)
	{
		pDC->BitBlt(750-i*45,500-manager.PlayCards[manager.PlayerAc][k-i].Click*10,45,71,&Cards,manager.PlayCards[manager.PlayerAc][k-i].Type*405+(manager.PlayCards[manager.PlayerAc][k-i].Num-1)*45,0,SRCCOPY);
	}
	if(manager.PlayCards[manager.PlayerAc][13].Num<=9)
		pDC->BitBlt(800,500-manager.PlayCards[manager.PlayerAc][13].Click*10,45,71,&Cards,manager.PlayCards[manager.PlayerAc][13].Type*405+(manager.PlayCards[manager.PlayerAc][13].Num-1)*45,0,SRCCOPY);
	//右边玩家站立的麻将 14*47 20间距
	Back.SelectObject(&m_bright);
	for(i=0;manager.PlayCards[(manager.PlayerAc+1)%4][i].Num<=9&&i<=13;i++)
	{
		pDC->BitBlt(780,40+20*i,14,47,&Back,0,0,SRCCOPY);
	}
	posright=40+20*(i-1);
	//左边玩家站立的麻将 14*47 20间距
	Back.SelectObject(&m_bleft);
	for(i=0;manager.PlayCards[(manager.PlayerAc+3)%4][i].Num<=9&&i<=13;i++)
	{
		pDC->BitBlt(100,40+20*i,14,47,&Back,0,0,SRCCOPY);
	}
	posleft=40+20*(i-1);
	//中间玩家站立的麻将 14*47 20间距
	Back.SelectObject(&m_bface);
	for(i=0;manager.PlayCards[(manager.PlayerAc+2)%4][i].Num<=9&&i<=13;i++)
	{
		pDC->BitBlt(200+24*i,30,24,36,&Back,0,0,SRCCOPY);
	}	

}

void CProgramView::DrawFallCards(CDC *pDC)
{
	//当前玩家倒立麻将 40*61
	int	i;
	Cards.SelectObject(&m_myfall);
	for(i=0;manager.PushCards[manager.PlayerAc][i].Num<=9&&i<=15;i++)
	{	if(!(manager.PushCards[manager.PlayerAc][i].Click))
			pDC->BitBlt(10+i*40,505,40,61,&Cards,1720,0,SRCCOPY);
		else
			pDC->BitBlt(10+i*40,505,40,61,&Cards,manager.PushCards[manager.PlayerAc][i].Type*360+(manager.PushCards[manager.PlayerAc][i].Num-1)*40,0,SRCCOPY);
	}
	//右边倒立的麻将	30*27 间距20
	Cards.SelectObject(&m_xline);
	for(i=0;manager.PushCards[(manager.PlayerAc+1)%4][i].Num<=9&&i<=15;i++)
	{	if(!(manager.PushCards[(manager.PlayerAc+1)%4][i].Click))
			pDC->BitBlt(780,posright+20*i,30,27,&Cards,1290,0,SRCCOPY);
		else
			pDC->BitBlt(780,posright+20*i,30,27,&Cards,manager.PushCards[(manager.PlayerAc+1)%4][i].Type*270+(manager.PushCards[(manager.PlayerAc+1)%4][i].Num-1)*30,27,SRCCOPY);
	}
	//左边倒立的麻将	
	for(i=0;manager.PushCards[(manager.PlayerAc+3)%4][i].Num<=9&&i<=15;i++)
	{	if(!(manager.PushCards[(manager.PlayerAc+3)%4][i].Click))
			pDC->BitBlt(100,posleft+20*i,30,27,&Cards,1290,0,SRCCOPY);
		else
			pDC->BitBlt(100,posleft+20*i,30,27,&Cards,manager.PushCards[(manager.PlayerAc+3)%4][i].Type*270+(manager.PushCards[(manager.PlayerAc+3)%4][i].Num-1)*30,0,SRCCOPY);
	}
	//对家倒立的麻将	24*36 间距25
	Cards.SelectObject(&m_yline);
	for(i=0;manager.PushCards[(manager.PlayerAc+2)%4][i].Num<=9&&i<=15;i++)
	{
		if(!(manager.PushCards[(manager.PlayerAc+2)%4][i].Click))
			pDC->BitBlt(670-25*i,30,24,36,&Cards,1032,0,SRCCOPY);
		else
			pDC->BitBlt(670-25*i,30,24,36,&Cards,manager.PushCards[(manager.PlayerAc+2)%4][i].Type*216+(manager.PushCards[(manager.PlayerAc+2)%4][i].Num-1)*24,0,SRCCOPY);
	}
}

void CProgramView::DrawOutCards(CDC *pDC)
{
		int i;
		int k;
		int	num;
	//当前玩家已经出的麻将
		Cards.SelectObject(&m_yline);
		for(i=0,num=0;i<=2;i++)
		{
			for(k=0;k<=9&&manager.PlayOuts[manager.PlayerAc][num].Num<=9;k++,num++)
			{
				pDC->BitBlt(315+25*k,390-i*36,24,36,&Cards,manager.PlayOuts[manager.PlayerAc][num].Type*216+(manager.PlayOuts[manager.PlayerAc][num].Num-1)*24,36,SRCCOPY);
			}
		}
	//右边玩家已经出的麻将
		Cards.SelectObject(&m_xline);
		for(i=0,num=0;i<=2;i++)
		{
			for(k=0;k<=9&&manager.PlayOuts[(manager.PlayerAc+1)%4][num].Num<=9;k++,num++)
			{
				pDC->BitBlt(650-i*30,130+20*k,30,27,&Cards,manager.PlayOuts[(manager.PlayerAc+1)%4][num].Type*270+(manager.PlayOuts[(manager.PlayerAc+1)%4][num].Num-1)*30,27,SRCCOPY);
			}
		}
	//左边玩家已经出的麻将	
		for(i=0,num=0;i<=2;i++)
		{
			for(k=0;k<=9&&manager.PlayOuts[(manager.PlayerAc+3)%4][num].Num<=9;k++,num++)
			{
				pDC->BitBlt(200+i*30,130+20*k,30,27,&Cards,manager.PlayOuts[(manager.PlayerAc+3)%4][num].Type*270+(manager.PlayOuts[(manager.PlayerAc+3)%4][num].Num-1)*30,0,SRCCOPY);
			}
		}
	//对家已经出的牌
		Cards.SelectObject(&m_yline);
		for(i=0,num=0;i<=2;i++)
		{
			for(k=0;k<=9&&manager.PlayOuts[(manager.PlayerAc+2)%4][num].Num<=9;k++,num++)
			{
				pDC->BitBlt(315+24*k,130+36*i,24,36,&Cards,manager.PlayOuts[(manager.PlayerAc+2)%4][num].Type*216+(manager.PlayOuts[(manager.PlayerAc+2)%4][num].Num-1)*24,0,SRCCOPY);
			}
		}	


}

void CProgramView::PrintAll()
{
	CDC	*pDC=GetDC();
	Back.SelectObject(&m_background);
	Buffer.BitBlt(0,0,900,620,&Back,0,0,SRCCOPY);
	DrawWall(manager.leftstart,manager.leftend,&Buffer);
//	DrawWall(manager.leftstart,manager.leftend,&Buffer);
	DrawStandCards(&Buffer);
	DrawFallCards(&Buffer);
	DrawOutCards(&Buffer);
	PrintState(&Buffer);
	
	Back.SelectObject(&m_button);
	if(manager.Game_State>=0&&manager.Game_State<=3)
	{
		if(manager.Actions%10==1)
			Buffer.BitBlt(580,435,40,40,&Back,0,0,SRCCOPY);
		if(manager.Actions>=100)
			Buffer.BitBlt(630,435,40,40,&Back,40,0,SRCCOPY);
		if(manager.Actions%100>=10)
			Buffer.BitBlt(680,435,40,40,&Back,80,0,SRCCOPY);
		if( manager.OutCard.Num<=9 && manager.OutPlayer!=manager.PlayerAc && manager.Actions!=0)
			Buffer.BitBlt(730,435,40,40,&Back,120,0,SRCCOPY);
	}
	if(manager.Game_State<0||manager.Game_State>3)
		Buffer.BitBlt(800,435,60,40,&Back,160,0,SRCCOPY);


	pDC->BitBlt(0,0,900,620,&Buffer,0,0,SRCCOPY);
	m_gn.Invalidate(true);
	m_go.Invalidate(true);
	m_hu.Invalidate(true);
	m_ci.Invalidate(true);
	m_start.Invalidate(true);

	if(manager.isPlayBkmusic&&!manager.m_backmusic.IsPlaying())
	{
		manager.m_backmusic.SeekToStart();
		manager.m_backmusic.Play();
	}
	ReleaseDC(pDC);
}

void CProgramView::PrintState(CDC *pDC)
{

	CString	st;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(205,45,20));

	Back.SelectObject(&m_cross);
	if(manager.Game_State==-2)
	{	st.Format("游戏还没开始");
		pDC->TextOut(390,260,st);
	}
	else if(manager.Game_State==-1)
	{	st.Format("游戏准备中，还没丢筛子");
		pDC->TextOut(390,260,st);
	}
	else if(manager.PointPlayer==manager.PlayerAc)
	{
		pDC->BitBlt(410,240,73,73,&Back,0,73,MERGEPAINT);
		pDC->BitBlt(410,240,73,73,&Back,0,0,SRCAND);
	}
	else if(manager.PointPlayer==(manager.PlayerAc+1)%4)
	{
		pDC->BitBlt(410,240,73,73,&Back,73,73,MERGEPAINT);
		pDC->BitBlt(410,240,73,73,&Back,73,0,SRCAND);
	}
	else if(manager.PointPlayer==(manager.PlayerAc+2)%4)
	{
		pDC->BitBlt(410,240,73,73,&Back,146,73,MERGEPAINT);
		pDC->BitBlt(410,240,73,73,&Back,146,0,SRCAND);
	}
	else if(manager.PointPlayer==(manager.PlayerAc+3)%4)
	{
		pDC->BitBlt(410,240,73,73,&Back,219,73,MERGEPAINT);
		pDC->BitBlt(410,240,73,73,&Back,219,0,SRCAND);
	}
	else if(manager.Game_State>=4&&manager.Game_State<=7)
	{
		st.Format("玩家%d胜利",manager.Game_State-4);
			pDC->TextOut(390,260,st);
	}


	pDC->TextOut(370,480,PlayerNames[0]);
	pDC->TextOut(760,10,PlayerNames[1]);
	pDC->TextOut(390,5,PlayerNames[2]);
	pDC->TextOut(50,10,PlayerNames[3]);
	
}

int CProgramView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CEnter	enter;
	if(enter.DoModal()==IDOK)
	{
		PlayerNames[0]=enter.m_name;
	}
	m_pControl->OnSorc();

	// TODO: Add your specialized creation code here
	m_gn.Create("杠",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		CRect(580,435,620,475),this,ID_GN);
	m_go.Create("过",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		CRect(630,435,670,475),this,ID_GO);
	m_ci.Create("碰",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		CRect(680,435,720,475),this,ID_CI);
	m_hu.Create("胡",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		CRect(730,435,770,475),this,ID_HU);
	m_start.Create("开始",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		CRect(800,435,860,475),this,ID_START);
		
	m_gn.ShowWindow(SW_HIDE);
	m_go.ShowWindow(SW_HIDE);
	m_ci.ShowWindow(SW_HIDE);
	m_hu.ShowWindow(SW_HIDE);
	m_start.ShowWindow(SW_HIDE);

	manager.PlaySound(0);
	return 0;
}

void CProgramView::OnGn()
{
	MessageBox("杠");
	manager.DoMsg(0,5);
}

void CProgramView::OnGo()
{
	MessageBox("过");
	manager.DoMsg(0,1);
}

void CProgramView::OnCi()
{
	MessageBox("碰");
	manager.DoMsg(0,4);
}

void CProgramView::OnHu()
{
	MessageBox("胡");
	manager.DoMsg(0,9);
}

void CProgramView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	int	mx=point.x;
	int	my=point.y;
	
	if(mx>=580&&my>=435&&mx<=620&&my<=475)
	{
		manager.DoMsg(0,4);
	}
	else if(mx>=630&&my>=435&&mx<=670&&my<=475)
	{
		manager.DoMsg(0,9);
	}
	else if(mx>=680&&my>=435&&mx<=720&&my<=475)
	{
		manager.DoMsg(0,5);
	}
	else if(mx>=730&&my>=435&&mx<=770&&my<=475)
	{
		manager.DoMsg(0,1);
	}
	else if(mx>=800&&my>=435&&mx<=860&&my<=475)
	{
		if(m_pControl->m_Isserver && (manager.Game_State<0 || manager.Game_State>3))
		{	
			m_pControl->m_ready[0]=true;
			int k;
			for(k=0;k<4;k++)
				if(!m_pControl->m_ready[k])
					break;
			if(k==4)
			{
				//游戏开始
				char st[1000]="majiangstcad";
				do
				{
				manager.GameStart();
				m_pControl->MakeStartCards(st+12);
				if(strlen(st)!=122)
				{
					MessageBox(st,"sss",1);
				//	return;
				}
				}
				while (strlen(st)!=122);
				m_pControl->Broadcast(st);
			}
		}
		else
			m_pControl->SendGetReady();
	}


	CView::OnLButtonDown(nFlags, point);
}

void CProgramView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int	mx=point.x;
	int	my=point.y;
	if(mx>=165&&my>=500&&mx<=845&&my<=571)
	{
		if(mx>=795&&mx<=845)
			mx=13;
		else
			mx=(mx-795+(SelectNum()+1)*45)/45-1;
		if(manager.DoMsg(mx,2))
		{
			PrintAll();
		}
	
	}	
	
	CView::OnLButtonUp(nFlags, point);
}

int CProgramView::SelectNum()
{
	int	k(0);
	for(;manager.PlayCards[manager.PlayerAc][k].Num<=9&&k<=12;k++);
	return	k;
}

void CProgramView::OnStart()
{
	if(m_pControl->m_Isserver)
	{	
		m_pControl->m_ready[0]=true;
		int k;
		for(k=0;k<4;k++)
			if(!m_pControl->m_ready[k])
				break;
		if(k==4)
		{
			//游戏开始
			char st[1000]="majiangstcad";
			manager.GameStart();
			m_pControl->MakeStartCards(st+12);
			m_pControl->Broadcast(st);
		}
	}
	else
		m_pControl->SendGetReady();
}
