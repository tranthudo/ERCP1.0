// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "afxdialogex.h"


// CDialogControl dialog

IMPLEMENT_DYNAMIC(CDialogControl, CDialog)

CDialogControl::CDialogControl(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogControl::IDD, pParent)
{
	pView = (CView*) pParent;
}

CDialogControl::~CDialogControl()
{
}

void CDialogControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogControl, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CDialogControl::OnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE_KEYPOINTS, &CDialogControl::OnClickedButtonCalculateKeypoints)
	ON_BN_CLICKED(IDC_BUTTON_INITIALIZE_TRACKING, &CDialogControl::OnClickedButtonInitializeTracking)
	ON_BN_CLICKED(IDC_BUTTON_START_TRACKING, &CDialogControl::OnClickedButtonStartTracking)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TRACKING, &CDialogControl::OnClickedButtonStopTracking)
END_MESSAGE_MAP()


// CDialogControl message handlers


void CDialogControl::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialogControl::OnClickedButtonUpdate();
	CDialog::OnOK();
}


void CDialogControl::OnClickedButtonUpdate()
{
	// TODO: Add your control notification handler code here
	if (pView!=NULL)
	{
		UpdateData(TRUE);
		pView->PostMessageA(WM_UPDATE_CONTROL_EDIT, IDC_BUTTON_UPDATE);
	}
}


BOOL CDialogControl::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	((CComboBox*)GetDlgItem(IDC_COMBO_KEYFRAME))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogControl::OnClickedButtonCalculateKeypoints()
{
	// TODO: Add your control notification handler code here
	if (pView!=NULL)
	{
		pView->PostMessageA(WM_CALCULATE_KEYPOINTS, IDC_BUTTON_CALCULATE_KEYPOINTS);
		
	}
}


void CDialogControl::OnClickedButtonInitializeTracking()
{
	if (pView!=NULL)
	{
		pView->PostMessageA(WM_INITIALIZE_TRACKING,IDC_BUTTON_INITIALIZE_TRACKING);

	}
}


void CDialogControl::OnClickedButtonStartTracking()
{
	if (pView!=NULL)
	{
		pView->PostMessageA(WM_START_TRACKING,IDC_BUTTON_START_TRACKING);

	}
}


void CDialogControl::OnClickedButtonStopTracking()
{
	if (pView!=NULL)
	{
		pView->PostMessageA(WM_STOP_TRACKING,IDC_BUTTON_STOP_TRACKING);

	}
}
