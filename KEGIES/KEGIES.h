// KEGIES.h : 
//
#pragma once

#ifndef __AFXWIN_H__
	#error "Error window"
#endif

#include "resource.h"       
// #include "./header/OpenGL.h"
// #include "./header/DataStruct.h"
// #include "./header/Obj.h"
// #include "./header/Camera.h"
// #include "./header/stl.h"

// CKEGIESApp:

class CKEGIESApp : public CWinApp
{
public:
	CKEGIESApp();

public:
	virtual BOOL InitInstance();


	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnToolConvertSTLtoObj();
	afx_msg void OnToolTest();
};

extern CKEGIESApp theApp;