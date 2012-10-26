

#pragma once
 
#pragma warning(disable:4996)

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		//
#endif

//
#ifndef WINVER				// Windows XP.
#define WINVER 0x0501		//
#endif

#ifndef _WIN32_WINNT		// Windows XP                 
#define _WIN32_WINNT 0x0501	//
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 
#define _WIN32_WINDOWS 0x0410 // Windows Me 
#endif

#ifndef _WIN32_IE			// IE 6.0
#define _WIN32_IE 0x0600	//
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	//

// MFC
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 
#include <afxext.h>         // MFC


#include <afxdisp.h>        // MFC 


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// Internet Explorer 4 
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 
#endif // _AFX_NO_AFXCMN_SUPPORT









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "./header/OpenGL.h"
#include "./header/DataStruct.h"
#include "./header/Obj.h"
#include "./header/Camera.h"
#include "./header/stl.h"