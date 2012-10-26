// MainFrm.cpp : CMainFrame
//

#include "stdafx.h"
#include "KEGIES.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 

CMainFrame::CMainFrame()
{
	// TODO:
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Error toolbar 1\n");
		return -1;      
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Error toolbar 2\n");
		return -1;      
	}

	// TODO:
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Create a window without min/max buttons or sizable border 
	cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER;

	// Size the window to 1/3 screen size and center it 
	//Manual change value here so that CVIEW class could have wxh == 640x 480
	cs.cy = 575; 
	cs.cx = 650; 
	cs.y = 10; 
	cs.x = 10;

	// Call the base-class version
	return CFrameWnd::PreCreateWindow(cs); 
	//if( !CFrameWnd::PreCreateWindow(cs) )
	//	return FALSE;
	//// TODO: CREATESTRUCT 
	//
	//
	//return TRUE;
}


// CMainFrame 

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame



