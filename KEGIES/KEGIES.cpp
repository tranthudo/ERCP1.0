// KEGIES.cpp 
//

#include "stdafx.h"
#include "KEGIES.h"
#include "MainFrm.h"

#include "KEGIESDoc.h"
#include "KEGIESView.h"

#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKEGIESApp

BEGIN_MESSAGE_MAP(CKEGIESApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CKEGIESApp::OnAppAbout)

	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)

	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_TOOL_CONVERTSTLTOOBJ, &CKEGIESApp::OnToolConvertSTLtoObj)
	ON_COMMAND(ID_TOOL_TEST, &CKEGIESApp::OnToolTest)
END_MESSAGE_MAP()


// CKEGIESApp 

CKEGIESApp::CKEGIESApp()
{
	// TODO: 
	// InitInstance
}




CKEGIESApp theApp;




BOOL CKEGIESApp::InitInstance()
{
	// InitCommonControlsEx()¸¦ 
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("Registry for KEGIES"));
	LoadStdProfileSettings(4);  // MRU¸

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CKEGIESDoc),
		RUNTIME_CLASS(CMainFrame),      
		RUNTIME_CLASS(CKEGIESView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// commandline
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// ¸Shell command
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Show main window
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	//  DragAcceptFiles
	//  ProcessShellCommand
	return TRUE;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CKEGIESApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/*
 * Convert STL to obj
 */
void CKEGIESApp::OnToolConvertSTLtoObj()
{
	CString path = Utility::OpenFileDialog(FILE_TYPE_STL);
	if(!path.IsEmpty())
	{
		CString outName = CString(path);
		outName.Delete(outName.GetLength()-3,3);
		outName += "obj";

		CSTL stl;
 		stl.ReadData(path.GetBuffer());
		if(stl.WriteToObj((char*)outName.GetBuffer()))
		{
			AfxMessageBox("Convert finished", MB_OK|MB_ICONINFORMATION);
		}
	}
}


void CKEGIESApp::OnToolTest()
{
	Utility::processPointIndex();
	// TODO: Add your command handler code here
}
