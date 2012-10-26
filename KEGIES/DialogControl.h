#pragma once
#include "resource.h"
#define WM_UPDATE_CONTROL_EDIT WM_USER + 5
#define WM_CALCULATE_KEYPOINTS WM_USER + 6
#define WM_INITIALIZE_TRACKING WM_USER + 7
#define WM_START_TRACKING WM_USER + 8
#define WM_STOP_TRACKING WM_USER + 9

// CDialogControl dialog

class CDialogControl : public CDialog
{
	DECLARE_DYNAMIC(CDialogControl)
private:
	CView* pView;

public:
	CDialogControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogControl();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	afx_msg void OnClickedButtonUpdate();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButtonCalculateKeypoints();
	afx_msg void OnClickedButtonInitializeTracking();
	afx_msg void OnClickedButtonStartTracking();
	afx_msg void OnClickedButtonStopTracking();
};
