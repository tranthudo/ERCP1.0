// MainFrm.h : CMainFrame 
//


#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // serialization
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)


//
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  //
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

//
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


