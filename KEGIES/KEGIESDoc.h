// KEGIESDoc.h : CKEGIESDoc 
//


#pragma once
#include "objload.h"

class CKEGIESDoc : public CDocument
{
protected: // serialization
	CKEGIESDoc();
	DECLARE_DYNCREATE(CKEGIESDoc)

public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();

	virtual void Serialize(CArchive& ar);

public:
	virtual ~CKEGIESDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	//graphic model
	CString filePath;
	CObj* m_Obj;
	Obj::File *objFile;
	
	

public:
	BOOL openLastDoc();
	void GetObjTexture();
protected:
	DECLARE_MESSAGE_MAP()
};


