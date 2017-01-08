// programView.h : interface of the CProgramView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMVIEW_H__1D8278F0_67DE_479D_BA34_04DDF7883EF3__INCLUDED_)
#define AFX_PROGRAMVIEW_H__1D8278F0_67DE_479D_BA34_04DDF7883EF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"NetControl.h"
#include"Card.h"

#define	ID_GN	100
#define	ID_GO	101
#define	ID_CI	102
#define	ID_HU	103
#define	ID_START 104	


class CProgramView : public CView
{
protected: // create from serialization only
	CProgramView();
	DECLARE_DYNCREATE(CProgramView)

// Attributes
public:
	CProgramDoc* GetDocument();

// Operations
public:
	CDC	Back;
	CDC	Cards;
	CDC	Buffer;

	CBitmap	m_bx;
	CBitmap	m_by;
	CBitmap	m_mystand;
	CBitmap	m_myfall;
	CBitmap	m_bleft;
	CBitmap	m_bright;
	CBitmap	m_bface;
	CBitmap	m_xline;
	CBitmap	m_yline;
	CBitmap	m_background;
	CBitmap	m_cross;	//73*42
	CBitmap	m_button;

	CBitmap	MemBitmap;

	CButton	m_gn;
	CButton	m_go;
	CButton	m_ci;
	CButton	m_hu;
	CButton	m_start;
	int	posright;
	int	posleft;
	int	move;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnStart();
	int SelectNum();
	void OnHu();
	void OnCi();
	void OnGo();
	void OnGn();
	void PrintState(CDC *pDC);
	void PrintAll();
	void DrawOutCards(CDC *pDC);
	void DrawFallCards(CDC *pDC);
	void DrawStandCards(CDC *pDC);
	void DrawWall(int start,int end,CDC *pDC);
	void DrawWhole(int start,int end,CDC *pDC);
	void DrawPart(int start,int end,CDC *pDC);
	NetControl *m_pControl;
	CString PlayerNames[4];
	virtual ~CProgramView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Generated message map functions
protected:
	//{{AFX_MSG(CProgramView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in programView.cpp
inline CProgramDoc* CProgramView::GetDocument()
   { return (CProgramDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMVIEW_H__1D8278F0_67DE_479D_BA34_04DDF7883EF3__INCLUDED_)
