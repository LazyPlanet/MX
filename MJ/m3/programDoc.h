// programDoc.h : interface of the CProgramDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMDOC_H__459D9192_2243_4DC1_9C86_A6C864A3052C__INCLUDED_)
#define AFX_PROGRAMDOC_H__459D9192_2243_4DC1_9C86_A6C864A3052C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CProgramDoc : public CDocument
{
protected: // create from serialization only
	CProgramDoc();
	DECLARE_DYNCREATE(CProgramDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProgramDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CProgramDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMDOC_H__459D9192_2243_4DC1_9C86_A6C864A3052C__INCLUDED_)
