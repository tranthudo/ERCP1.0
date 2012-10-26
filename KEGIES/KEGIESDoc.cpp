// KEGIESDoc.cpp : CKEGIESDoc 
//

#include "stdafx.h"
#include "KEGIES.h"
#include "KEGIESDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKEGIESDoc

IMPLEMENT_DYNCREATE(CKEGIESDoc, CDocument)

BEGIN_MESSAGE_MAP(CKEGIESDoc, CDocument)
END_MESSAGE_MAP()


// CKEGIESDoc

CKEGIESDoc::CKEGIESDoc():
m_Obj(NULL)
{
	objFile = new Obj::File;

}

CKEGIESDoc::~CKEGIESDoc()
{
	
}

BOOL CKEGIESDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 
	// Loading Obj and Texture
	
	objFile->Load("./data/Full box/fullbox.obj");	
	return TRUE;
}

BOOL CKEGIESDoc::openLastDoc()
{
	CFileDialog dlg(TRUE, "obj", "*.obj", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);

	if(dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		return OnOpenDocument((char*)filePath.GetBuffer());
	}
	else
	{
		return FALSE;
	}
}



// CKEGIESDoc serialization

void CKEGIESDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:
	}
	else
	{
		// TODO:
	}
}


// CKEGIESDoc 

#ifdef _DEBUG
void CKEGIESDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKEGIESDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CKEGIESDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here
	if(m_Obj)
	{
		delete m_Obj;
		m_Obj = NULL;
	}

	m_Obj=new CObj;

	if(m_Obj->ReadObjData((char*)lpszPathName))
	{
		vec3d mid=m_Obj->m_MidPoint;
		m_Obj->TranslateObj(mid*-1);
	}

	return TRUE;
}


void CKEGIESDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_Obj)
	{
		delete m_Obj;
		m_Obj = NULL;
	}
	delete objFile;
	CDocument::OnCloseDocument();
}
