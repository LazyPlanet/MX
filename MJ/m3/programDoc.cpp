// programDoc.cpp : implementation of the CProgramDoc class
//

#include "stdafx.h"
#include "program.h"

#include "programDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgramDoc

IMPLEMENT_DYNCREATE(CProgramDoc, CDocument)

BEGIN_MESSAGE_MAP(CProgramDoc, CDocument)
	//{{AFX_MSG_MAP(CProgramDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramDoc construction/destruction

CProgramDoc::CProgramDoc()
{
	// TODO: add one-time construction code here

}

CProgramDoc::~CProgramDoc()
{
}

BOOL CProgramDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CProgramDoc serialization

void CProgramDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProgramDoc diagnostics

#ifdef _DEBUG
void CProgramDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProgramDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProgramDoc commands
