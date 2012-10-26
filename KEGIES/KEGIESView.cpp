// KEGIESView.cpp : CKEGIESView 
//

#include "stdafx.h"
#include "KEGIES.h"

#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "cameraSimple.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKEGIESView

IMPLEMENT_DYNCREATE(CKEGIESView, CView)

BEGIN_MESSAGE_MAP(CKEGIESView, CView)
	// 
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_RENDERMODE_LINE, &CKEGIESView::OnRendermodeLine)
	ON_COMMAND(ID_RENDERMODE_LINEANDSURFACE, &CKEGIESView::OnRendermodeLineandsurface)
	ON_COMMAND(ID_RENDERMODE_SURFACE, &CKEGIESView::OnRendermodeSurface)
	ON_COMMAND(ID_CAMMODE_REALCAM, &CKEGIESView::OnCammodeRealcam)
	ON_COMMAND(ID_CAMMODE_THIRDPERSON, &CKEGIESView::OnCammodeThirdperson)
	ON_UPDATE_COMMAND_UI(ID_CAMMODE_REALCAM, &CKEGIESView::OnUpdateCammodeRealcam)
	ON_UPDATE_COMMAND_UI(ID_CAMMODE_THIRDPERSON, &CKEGIESView::OnUpdateCammodeThirdperson)
	
	ON_COMMAND(ID_VIEWFRUSTRUM_YES, &CKEGIESView::OnViewfrustrumYes)
	ON_UPDATE_COMMAND_UI(ID_VIEWFRUSTRUM_YES, &CKEGIESView::OnUpdateViewfrustrumYes)
	ON_COMMAND(ID_VIEWFRUSTRUM_NO, &CKEGIESView::OnViewfrustrumNo)
	ON_UPDATE_COMMAND_UI(ID_VIEWFRUSTRUM_NO, &CKEGIESView::OnUpdateViewfrustrumNo)

	//Messages passed from Dialog control
	ON_MESSAGE(WM_UPDATE_CONTROL_EDIT, &CKEGIESView::OnUpdateControlEdit)
	ON_MESSAGE(WM_CALCULATE_KEYPOINTS, &CKEGIESView::OnCalculateKeypoints)
	ON_MESSAGE(WM_INITIALIZE_TRACKING, &CKEGIESView::OnInitializeTracking)
	ON_MESSAGE(WM_START_TRACKING, &CKEGIESView::OnStartTracking)
	ON_MESSAGE(WM_STOP_TRACKING, &CKEGIESView::OnStopTracking)

	
END_MESSAGE_MAP()

// CKEGIESView 

CKEGIESView::CKEGIESView():
	renderMode(1), m_CamMode(2), DRAW_FRUSTRUM(TRUE), textureInitialized(FALSE), 
	UnProject(FALSE), m_ProgramMode(OFFLINE), m_PreviousTrackingState(FALSE), m_CurrrentTrackingState(FALSE),
	//sift_cpu(50,3,2,false) // construct surf detector
	sift_cpu(1000, 4, 0.01, 10, 1.6)
	/*,flannMatcher(cv::NORM_L2)*/
{
	// TODO:	
	// Initialize camera
	m_CameraPosition[0] = -22.699;
	m_CameraPosition[1] = 5.599;
	m_CameraPosition[2] = 5.907;

	m_CameraAngle[0] = 62.746;
	m_CameraAngle[1] = -15.389;
	m_CameraAngle[2] = -83.981;

	m_fovy = 36.069725;
	m_near = 1;
	m_far = 60;

	m_fovy1 = 45;
	m_near1 = 1;
	m_far1 = 10000;
	// Initialize Intrinsic Camera Matrix, code is calculated by matlab file
	float intrinsic[3][3] = {-737.11764, 0.00000,   320.00000,
		0.00000,   737.11764,   240.00000,
		0.00000,     0.00000,     1.00000};
	cv::Mat temp_CamIntrinsic = cv::Mat(3,3,CV_32F,intrinsic);
	temp_CamIntrinsic.copyTo(m_CamIntrinsic);	
	
	// Initialize Discoeffs matrix
	double __d[4] = {0,0,0,0}; 
	cv::Mat tempDistCoeffs = cv::Mat(1,4,CV_64FC1,__d);
	tempDistCoeffs.copyTo(distCoeffs);
	
	// Initialize rotation vector as a matrix size 3x1
	std::vector<double> rv = std::vector<double>(3); 
	rv[0]=-0.6904;rv[1]=0.9673;rv[2]=1.1565;
	cv::Mat tempRvec = cv::Mat(rv);
	tempRvec.copyTo(rvec);

	// Initialize full rotation matrix
	double _R[9] = {0.1011,   -0.9588,    0.2654,
		0.4307 ,   0.2826,    0.8571,
		-0.8968,    0.0276,    0.4415} ;
	cv::Mat tempRotMatrix = cv::Mat(3,3,CV_64FC1,_R);
	tempRotMatrix.copyTo(rotMatrix);
	cv::Rodrigues(rotMatrix,rvec);				// Re-initialize rvec if needed
	
	// Initialize translation vector
	std::vector<double> tv = std::vector<double>(3); 
	tv[0]=-22.699;tv[1]=5.599;tv[2]=5.5990; 
	cv::Mat tempTvec = cv::Mat(tv);
	tempTvec.copyTo(tvec);	
	

	// Initialize pointers
	_r = rvec.ptr<double>();
	_t = tvec.ptr<double>();
	_rot = rotMatrix.ptr<double>();
	freq = 1000./cv::getTickFrequency();

	sizeM = 100;	
	m_wndDialogControl = new CDialogControl(this);
	
}

CKEGIESView::~CKEGIESView()
{
	delete _r;
	delete _t;
}

#pragma OPEN_GL

void CKEGIESView::InitGL()
{
	COpenGL Initgl;
	
	//Init
	Initgl.SetHWND(m_hWnd);
	Initgl.SetupPixelFormat();
	base=Initgl.base;
	
	m_hDC=Initgl.m_hDC;
	m_hRC=Initgl.m_hRC;

//	Initgl.SetupShader();
}

// CKEGIESView 

void CKEGIESView::OnDraw(CDC* /*pDC*/)
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 
	wglMakeCurrent(m_hDC,m_hRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0,1.0,1.0, 1.0);
	DrawView();
	SwapBuffers(m_hDC);
}

void CKEGIESView::DrawView()
{
	
	UpdateView();
	SetupView();
	switch (m_ProgramMode)
	{
	case OFFLINE:
		#pragma region CASE OFFLINE in DrawView
		// OFFLINE DRAWING	
		// Check valid Glunproject
		if (UnProject)
		{
			glPointSize(2);
			glBegin(GL_POINTS);
			glColor3f(1.0,0.0,0.0);
			for (int i=0; i<(int)objPoints_cpu.size();i++)
			{
				glVertex3d(objPoints_cpu[i].x,objPoints_cpu[i].y,objPoints_cpu[i].z);
			}
			glEnd();
		}
		else 
		{
			//////////////////////////////////////////////////////////////////////////
			// Draw Object here
			/*if(GetDocument()->m_Obj)
			{
				vec3d color(1,0,0);
				GetDocument()->m_Obj->DrawObject(renderMode,color);
			}*/

			if (GetDocument()->objFile)
			{
				if (textureInitialized==FALSE)
				{
					textureImage = cv::imread("./data/Full box/fullbox_TX/Cube_MG_2_TX.jpg");	
					cv::flip(textureImage,textureImage,0);
					GetObjTexture();
					textureInitialized = TRUE;
				}
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,objTextureID);
				GetDocument()->objFile->Draw();
				glDisable(GL_TEXTURE_2D);
			}
		}	
		//////////////////////////////////////////////////////////////////////////
		// Draw Camera and Frustrum for debuging purpose (mode 2 only)
		if (m_CamMode == 2)
		{
			DrawAxis(3);  // Draw Obj coordinate axis
			if (DRAW_FRUSTRUM)
			{
				glPushMatrix();
				glTranslatef(m_CameraPosition[0],m_CameraPosition[1],m_CameraPosition[2]);						
				glRotatef(m_CameraAngle[2],0,0,1);
				glRotatef(m_CameraAngle[1],0,1,0);
				glRotatef(m_CameraAngle[0],1,0,0);
				drawCamera();		
				DrawAxis(3);  // Draw camera coordinate axes
				float aspect = (float)m_WindowWidth/(float)m_WindowHeight;
				drawFrustum(m_fovy,aspect,m_near,m_far);
				glPopMatrix();
			}		
		}
		break; // End OFFLINE CASE
		#pragma endregion CASE OFFLINE in DrawView

	case INITIALIZE_TRACKING:
		GetFrame2GLTexture();
		DrawGrabbedFrame();
		break; // End TRACKING INITIALIZATION READY FOR ONLINE TRACKING
	case ONLINE_TRACKING:
		GetFrame2GLTexture();
		DrawGrabbedFrame();
		if (GetDocument()->objFile)
		{			
			SetupView();
			UpdateView();
			glPolygonMode(GL_FRONT,GL_LINE);
			glLineWidth(4.0);
			glDisable(GL_TEXTURE_2D);			
			GetDocument()->objFile->Draw();
			glEnable(GL_TEXTURE_2D);
		}
		break;
	default:
		break;
	}// end switch m_ProgramMode
	
}

void CKEGIESView::UpdateView()
{
	glViewport(0,0,m_WindowWidth,m_WindowHeight);
	//glViewport(0,0,640,480);
	//m_fovy=45;
	float aspect=float(m_WindowWidth)/float(m_WindowHeight);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	if (m_CamMode == 1)
	{
		gluPerspective(m_fovy, aspect, m_near, m_far);
	}
	else 
	{
		gluPerspective(m_fovy1, aspect, m_near1, m_far1);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (m_CamMode == 2)  // From third person view
	{
		gluLookAt(m_Cam.m_Pos.x,m_Cam.m_Pos.y,m_Cam.m_Pos.z,
			m_Cam.m_Center.x,m_Cam.m_Center.y,m_Cam.m_Center.z,
			m_Cam.m_Up.x,m_Cam.m_Up.y,m_Cam.m_Up.z);
	}	
	else if (m_CamMode == 1) // From virtual camera view
	{		
		/*GLfloat matrixmodelview[16] = { 0.4549,   -0.7438,   -0.4897,         0,
										0.8194,    0.1342,    0.5573,         0,
										-0.3487,   -0.6548,    0.6705,         0,
										-3.5753 ,   0.2133,   14.0598,    1.0000};
		glLoadMatrixf(matrixmodelview);*/
		/*GLfloat matrixmodelview[16] = { -0.4393,    0.8038,   -0.4011,         0,
			-0.8301,   -0.1925,    0.5234,         0,
			0.3435,    0.5629,    0.7518,         0,
			3.6277,    0.0361,  -14.2743,    1.0000};
		glLoadMatrixf(matrixmodelview);
*/
		if (m_ProgramMode !=ONLINE_TRACKING)
		{
			glRotatef(-m_CameraAngle[0],1,0,0);  /// x ->y ->z rotate order that follow maya
			glRotatef(-m_CameraAngle[1],0,1,0);
			glRotatef(-m_CameraAngle[2],0,0,1);			
			glTranslatef(-m_CameraPosition[0], -m_CameraPosition[1], -m_CameraPosition[2]);
			GetGLMatrices();
		}
		else 
		{
			// Update camera pose
			modelview[0] = -_rot[0];
			modelview[1] = -_rot[3];
			modelview[2] = _rot[6];
			modelview[3] = 0;

			modelview[4] = -_rot[1];
			modelview[5] = -_rot[4];
			modelview[6] = _rot[7];
			modelview[7] = 0;

			modelview[8] = -_rot[2];
			modelview[9] = -_rot[5];
			modelview[10] = _rot[8];
			modelview[11] = 0;
			
			modelview[12] = -_t[0];
			modelview[13] = -_t[1];
			modelview[14] = -_t[2];
			modelview[15] = 1.0;
			glLoadMatrixd(modelview);

		}
		
	}
}

void CKEGIESView::SetupView()
{
	GLfloat diffuseLight[] = {0.4f,0.4f,0.4f,1.0f};
	GLfloat ambientLight[] = {0.2f,0.2f,0.2f,1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	glEnable(GL_DEPTH_TEST);                                        
	glEnable(GL_CULL_FACE);  
		//glDisable(GL_DEPTH_TEST); 
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glEnable(GL_BLEND);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_LIGHTING);   
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glEnable(GL_LIGHT0);
	
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH); 
	//	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT, GL_FILL);
}

void CKEGIESView::GetObjTexture()
{
	glEnable(GL_TEXTURE_2D) ;
	glGenTextures(1, &objTextureID) ;
	glBindTexture(GL_TEXTURE_2D, objTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 
		3, 
		textureImage.cols, 
		textureImage.rows,
		GL_BGR_EXT, 
		GL_UNSIGNED_BYTE, 
		textureImage.data) ;
	glDisable(GL_TEXTURE_2D) ;
}

cv::Point3f CKEGIESView::GetOGLPos( float x, float y )
{

	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;
	winX = x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels( (int)x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	return cv::Point3f(posX, posY, posZ); 

}

void CKEGIESView::GetGLMatrices()
{
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	/*for (i= 0; i<16; i++)
	{
		TRACE("Modelview(%d) = %f \n",i+1,modelview[i]);
	}*/
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	/*for (i= 0; i<16; i++)
	{
		TRACE("Projection(%d) = %f \n",i+1,projection[i]);
	}*/
	glGetIntegerv( GL_VIEWPORT, viewport );
	/*for (i= 0; i<16; i++)
	{
		TRACE("ViewPort(%d) = %f \n",i+1,viewport[i]);
	}*/
}

void CKEGIESView::GetFrame2GLTexture()
{
	if (m_CurrentFrame.data)
	{
		cv::Mat tmpImage;
		cv::flip(m_CurrentFrame,tmpImage,0);
		glEnable(GL_TEXTURE_2D) ;
		glGenTextures(1, &camTextureID) ;
		glBindTexture(GL_TEXTURE_2D, camTextureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 
			3, 
			tmpImage.cols, 
			tmpImage.rows,
			GL_BGR_EXT, 
			GL_UNSIGNED_BYTE, 
			tmpImage.data) ;
		glDisable(GL_TEXTURE_2D) ;
	}
}

void CKEGIESView::DrawGrabbedFrame()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glPushMatrix();
	glViewport(0,0, m_WindowWidth , m_WindowHeight) ;

	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	glOrtho(0, m_WindowWidth, 0, m_WindowHeight, 0, 2000) ;

	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity() ;

	glEnable(GL_TEXTURE_2D) ;

	glBindTexture(GL_TEXTURE_2D, camTextureID) ;
	glBegin(GL_QUADS) ;
	glTexCoord2f(0,0) ;
	glVertex3f(0,0, -1999) ;

	glTexCoord2f(1,0) ;
	glVertex3f(m_WindowWidth,0, -1999) ;

	glTexCoord2f(1,1) ;
	glVertex3f(m_WindowWidth,m_WindowHeight, -1999) ;

	glTexCoord2f(0,1) ;
	glVertex3f(0,m_WindowHeight, -1999) ;
	glEnd() ;

	glDisable(GL_TEXTURE_2D) ;
	glDeleteTextures(1,&camTextureID);
	glPopMatrix();
	glPopAttrib();
}



// CKEGIESView

#ifdef _DEBUG
void CKEGIESView::AssertValid() const
{
	CView::AssertValid();
}

void CKEGIESView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKEGIESDoc* CKEGIESView::GetDocument() const // 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKEGIESDoc)));
	return (CKEGIESDoc*)m_pDocument;
}
#endif //_DEBUG

#pragma INITIALIZE
// CKEGIESView
int CKEGIESView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (m_wndDialogControl->GetSafeHwnd()==0)
	{
		m_wndDialogControl->Create(IDD_DIALOG_CONTROL);
		m_wndDialogControl->ShowWindow(SW_SHOW);
	}
	return 0;
}

void CKEGIESView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	LEFT_DOWN=false;
	RIGHT_DOWN=false;
	

	// Update Dialog Control Text
	CString text;
	text.Format("%f",m_CameraPosition[0]);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_CAM_POS_1,text);
	text.Format("%f",m_CameraPosition[1]);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_CAM_POS_2,text);
	text.Format("%f",m_CameraPosition[2]);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_CAM_POS_3,text);

	text.Format("%f",m_CameraAngle[0]);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_CAM_ANG_1,text);
	text.Format("%f",m_CameraAngle[1]);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_CAM_ANG_2,text);
	text.Format("%f",m_CameraAngle[2]);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_CAM_ANG_3,text);


	text.Format("%d",m_WindowWidth);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_WINDOW_WIDTH,text);
	text.Format("%d",m_WindowHeight);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_WINDOW_HEIGHT,text);
	text.Format("%f",m_fovy);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_FOVY,text);
	text.Format("%f",m_near);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_NEAR,text);
	text.Format("%f",m_far);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_FAR,text);


	SetTimer(1,20,NULL);

	InitGL();
}

BOOL CKEGIESView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT 
	cs.cx = 640;
	cs.cy = 480;
	//cs.style = ~SW_MAXIMIZE | ~SW_RESTORE;
	return CView::PreCreateWindow(cs);
}




#pragma  WINDOW_EVENT

void CKEGIESView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char lsChar;
	lsChar = char(nChar);
	
	if(lsChar=='Q')
	{
	}
	else if (lsChar == 'W')
	{
		std::vector<cv::Point3f> objpoints;
		std::vector<cv::Point2f> imgpoints;
		
		// Load data test
		cv::FileStorage fs("output/keyPoints_selected.yml", cv::FileStorage::READ);
		fs["keyPoints_selected"] >> imgpoints;
		fs.release();
		fs.open("output/objPoints_selected.yml",cv::FileStorage::READ);
		fs["objPoints_selected"] >> objpoints;
		fs.release();
		
		myText.Format("Number of image points = %d\n", imgpoints.size());
		TRACE(myText);
		for (int i = 0; i<imgpoints.size();i++)
		{
			myText.Format("{%3.6f, %3.6f}   ", imgpoints[i].x,imgpoints[i].y);
			TRACE(myText);
			if ((i+1)%3==0) printf("\n");
		}
		TRACE("\n\n");
		myText.Format("Number of object points = %d\n", objpoints.size());
		TRACE(myText);
		for (int i = 0; i<objpoints.size();i++)
		{
			myText.Format("{%3.6f, %3.6f, %3.6f}   ", objpoints[i].x,objpoints[i].y,objpoints[i].z);
			TRACE(myText);
			if ((i+1)%3==0) printf("\n");
		}
		TRACE("\n\n");
		int iterationsCount = 100;
		float reprojectionerror = 8.0f;
		int minInliers = 100;
		int flags = cv::P3P;
		//////////////////////////////////////////////////////////////////////////
		tinit = cv::getTickCount();	
		cv::solvePnPRansac(objpoints,imgpoints,m_CamIntrinsic,distCoeffs,rvec,tvec,false,iterationsCount,reprojectionerror,minInliers, inliers,cv::ITERATIVE);
		myText.Format("TIME TO SOLVE ITERACTIVE problem = %f ms;  number of inliers = %d\n",(cv::getTickCount()-tinit)*freq,inliers.size());
		TRACE(myText);
		myText.Format("T = [%f,%f,%f]; R =[%f, %f, %f]\n\n\n",_t[0],_t[1],_t[2],_r[0],_r[1],_r[2]);
		TRACE(myText);
		
		
		tinit = cv::getTickCount();	
		cv::solvePnPRansac(objpoints,imgpoints,m_CamIntrinsic,distCoeffs,rvec,tvec,false,iterationsCount,reprojectionerror,minInliers, inliers,cv::P3P);
		myText.Format("TIME TO SOLVE P3P problem = %f ms;  number of inliers = %d\n",(cv::getTickCount()-tinit)*freq,inliers.size());
		TRACE(myText);
		myText.Format("T = [%f,%f,%f]; R =[%f, %f, %f]\n\n\n",_t[0],_t[1],_t[2],_r[0],_r[1],_r[2]);
		TRACE(myText);
		
		
		tinit = cv::getTickCount();	
		cv::solvePnPRansac(objpoints,imgpoints,m_CamIntrinsic,distCoeffs,rvec,tvec,false,iterationsCount,reprojectionerror,minInliers, inliers,cv::EPNP);
		myText.Format("TIME TO SOLVE EPNP problem = %f ms;  number of inliers = %d\n",(cv::getTickCount()-tinit)*freq,inliers.size());
		TRACE(myText);
		myText.Format("T = [%f,%f,%f]; R =[%f, %f, %f]\n\n\n",_t[0],_t[1],_t[2],_r[0],_r[1],_r[2]);
		TRACE(myText);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CKEGIESView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);	
	CSize size(cx,cy);
	m_WindowHeight=size.cy;
	m_WindowWidth=size.cx;
	CString text;
	text.Format("%d",m_WindowWidth);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_WINDOW_WIDTH,text);
	text.Format("%d",m_WindowHeight);
	m_wndDialogControl->SetDlgItemText(IDC_EDIT_WINDOW_HEIGHT,text);
}
void CKEGIESView::OnTimer(UINT_PTR nIDEvent)
{
	switch (m_ProgramMode)
	{
	case OFFLINE:
		break;
	case INITIALIZE_TRACKING:
		m_CurrentFrame.copyTo(m_PreviousFrame);
		m_Capture>>m_CurrentFrame;
		break;
	case ONLINE_TRACKING:
		m_CurrentFrame.copyTo(m_PreviousFrame);
	m_Capture>>m_CurrentFrame;

		// Test tracking algorithms here without multithread
		sift_cpu(m_CurrentFrame,cv::Mat(),currentKeypoints_cpu,currentDescriptors_cpu,false);
		
		/************************************************************************/
		/* Matching Process                                                     */
		/************************************************************************/

		// Using flannbased normal match
		/*flannMatcher.match(currentDescriptors_cpu, matches);
		
		if (matches.size()>sizeM)
		{			
		std::nth_element(matches.begin(),matches.begin()+sizeM-1,matches.end());
		}
		else
		{
		int sizeM = matches.size();
		std::nth_element(matches.begin(),matches.begin()+sizeM-1,matches.end());
		}
		matches.erase(matches.begin()+sizeM, matches.end());
		*/

		// Using knnMatching and ratioTest
		
		flannMatcher.knnMatch(currentDescriptors_cpu,matches,2);
		n2tMatcher.ratioTest(matches);		
		
		flannMatcher2.knnMatch(descriptors_cpu,currentDescriptors_cpu,matches2,2);
		n2tMatcher.ratioTest(matches2);
		
		n2tMatcher.symmetryTest(matches,matches2,symmetryMatches);
		// something like radius matches
		/*sizeM = 50;
		if ((int)symmetryMatches.size()>sizeM)
		{			
			std::nth_element(symmetryMatches.begin(),symmetryMatches.begin()+sizeM-1,symmetryMatches.end());
		}
		else
		{
			sizeM = symmetryMatches.size()-1;
			std::nth_element(symmetryMatches.begin(),symmetryMatches.begin()+sizeM-1,symmetryMatches.end());
		}
		if (symmetryMatches.size()>=50)
		{
			symmetryMatches.erase(symmetryMatches.begin()+sizeM, symmetryMatches.end());
		}*/
		
		
		// drawing the results
		/*cv::namedWindow("matches", 1);		
		cv::drawMatches(m_CurrentFrame, currentKeypoints_cpu, keyframe1, keypoints_cpu, symmetryMatches, img_matches);
		cv::imshow("matches", img_matches);*/
		
		

		// Used if using .match method but not knnmatch because knnmatch require vector<vector<Dmatch>>
		/*for (i = 0; i < (int)matches.size(); i++)
		{
			objPoints_selected.push_back(objPoints_cpu[matches[0][i].trainIdx]);
			keyPoints_selected.push_back(currentKeypoints_cpu[matches[0][i].queryIdx].pt);
		}		*/
		keyPoints_selected.clear();                                         
		objPoints_selected.clear();		
		for (i = 0; i <(int)symmetryMatches.size(); i++)
		{
			objPoints_selected.push_back(objPoints_cpu[symmetryMatches[i].trainIdx]);
			keyPoints_selected.push_back(currentKeypoints_cpu[symmetryMatches[i].queryIdx].pt);
		}

		myText.Format("Number of matches = %d; Number of current keypoints = %d\n\n",symmetryMatches.size(),currentKeypoints_cpu.size());		
		::OutputDebugStringA(myText);
		
		
		// Write keypoints to output folder
		/*fs.open("output/keyPoints_selected.yml",cv::FileStorage::WRITE);
		fs<<"keyPoints_selected"<<keyPoints_selected;
		fs.release();
		fs.open("output/objPoints_selected.yml",cv::FileStorage::WRITE);
		fs<<"objPoints_selected"<<objPoints_selected;
		fs.release();	*/

		// Note that to use solvePnP we need to initialize distCoeffs, rvec and tvec first
		

		// Note that to use solvePnP objPoints and keypoints should be Point3f and Point2f types, do not use Point3d

		matches.clear();
		matches2.clear();
		symmetryMatches.clear();
		tinit = cv::getTickCount();
		//solvePnP(objPoints_selected,keyPoints_selected, m_CamIntrinsic, distCoeffs, rvec, tvec, false, cv::EPNP);
		solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(keyPoints_selected), 
						m_CamIntrinsic, distCoeffs, rvec, tvec, 
						true, 100, 10.0f,100,inliers,cv::EPNP);		
		
		utime = cv::getTickCount()-tinit;
		myText.Format("TIME TO SOLVE PNP problem = %f ms;  number of inliers = %d\n",freq*utime,inliers.size());
		::OutputDebugStringA(myText);
		

		myText.Format("T = [%f,%f,%f]; R =[%f, %f, %f]\n",_t[0],_t[1],_t[2],_r[0],_r[1],_r[2]);
		cv::Rodrigues(rvec,rotMatrix);	
		::OutputDebugStringA(myText);
		
		break;
	default:
		break;
	}
	UpdateControlData();
	InvalidateRect(NULL, FALSE);
	CView::OnTimer(nIDEvent);
}

void CKEGIESView::OnLButtonDown(UINT nFlags, CPoint point)
{
	LEFT_DOWN=true;
	CView::OnLButtonDown(nFlags, point);
}

void CKEGIESView::OnLButtonUp(UINT nFlags, CPoint point)
{
	LEFT_DOWN=false;
	CView::OnLButtonUp(nFlags, point);
}

void CKEGIESView::OnRButtonDown(UINT nFlags, CPoint point)
{
	RIGHT_DOWN=true;
	CView::OnRButtonDown(nFlags, point);
}

void CKEGIESView::OnRButtonUp(UINT nFlags, CPoint point)
{
	RIGHT_DOWN=false;
	CView::OnRButtonUp(nFlags, point);
}

void CKEGIESView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_MousePos.x=point.x;
	m_MousePos.y=-point.y;
	m_DMousePos=m_MousePos-m_PreMousePos;
	if (m_CamMode == 2)
	{
		if(LEFT_DOWN)
			m_Cam.RotCamPos(m_DMousePos);
		if(RIGHT_DOWN)
			m_Cam.MoveCamPos(m_DMousePos);
	}
		
	m_PreMousePos=m_MousePos;
	CView::OnMouseMove(nFlags, point);
}

BOOL CKEGIESView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	vec3d temp;
	if (m_CamMode == 2)
	{
		m_Cam.m_Distance-=zDelta*0.1;
		m_Cam.RotCamPos(temp);
	}	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}




#pragma SIDE_FUNCTION


void CKEGIESView::OnRendermodeLine()
{
	renderMode = 1;
}


void CKEGIESView::OnRendermodeLineandsurface()
{
	renderMode = 2;
}


void CKEGIESView::OnRendermodeSurface()
{
	renderMode = 3;
}


void CKEGIESView::OnCammodeRealcam()
{	
	m_CamMode = 1;
}


void CKEGIESView::OnCammodeThirdperson()
{
	m_CamMode = 2;
}


void CKEGIESView::OnUpdateCammodeRealcam(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CamMode == 1);
}


void CKEGIESView::OnUpdateCammodeThirdperson(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CamMode == 2);
}


//////////////////////////////////////////////////////////////////////////
void CKEGIESView::OnViewfrustrumYes()
{
	// TODO: Add your command handler code here
	DRAW_FRUSTRUM = TRUE;
}


void CKEGIESView::OnUpdateViewfrustrumYes(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(DRAW_FRUSTRUM==TRUE);
}


void CKEGIESView::OnViewfrustrumNo()
{
	// TODO: Add your command handler code here
	DRAW_FRUSTRUM = FALSE;
}


void CKEGIESView::OnUpdateViewfrustrumNo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(DRAW_FRUSTRUM==FALSE);
}


void CKEGIESView::UpdateControlData()
{
	float epsilon = 0.000001;
	float param = 0;
	int parami = 0;
	CString text;
	param = m_CameraPosition[0];
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_CAM_POS_1,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_CAM_POS_1,text);
	}

	param = m_CameraPosition[1];
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_CAM_POS_2,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_CAM_POS_2,text);
	}

	param = m_CameraPosition[2];
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_CAM_POS_3,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_CAM_POS_3,text);
	}

	param = m_CameraAngle[0];
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_CAM_ANG_1,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_CAM_ANG_1,text);
	}
	param = m_CameraAngle[1];
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_CAM_ANG_2,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_CAM_ANG_2,text);
	}

	param = m_CameraAngle[2];
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_CAM_ANG_3,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_CAM_ANG_3,text);
	}

	//////////////////////////////////////////////////////////////////////////

	parami = m_WindowWidth;
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_WINDOW_WIDTH,text);
	if (!(abs(parami-atoi(text))<1))
	{
		text.Format("%d",parami);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_WINDOW_WIDTH,text);
	}

	parami = m_WindowHeight;
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_WINDOW_HEIGHT,text);
	if (!(abs(parami-atof(text))<1))
	{
		text.Format("%d",parami);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_WINDOW_HEIGHT,text);
	}

	param = m_fovy;
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_FOVY,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_FOVY,text);
	}

	param = m_near;
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_NEAR,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_NEAR,text);
	}

	param = m_far;
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CURRENT_FAR,text);
	if (!(abs(param-atof(text))<=epsilon))
	{
		text.Format("%f",param);
		m_wndDialogControl->SetDlgItemText(IDC_EDIT_CURRENT_FAR,text);
	}
}





#pragma DIALOG_POST_MESSAGE_HANDLERS

// Update the Edit control to Doc-camera;
afx_msg LRESULT CKEGIESView::OnUpdateControlEdit(WPARAM wParam, LPARAM lParam)
{
	CString str;
	// Update Camera Pose
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CAM_POS_1,str);
	m_CameraPosition[0] = atof(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CAM_POS_2,str);
	m_CameraPosition[1] = atof(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CAM_POS_3,str);
	m_CameraPosition[2] = atof(str);

	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CAM_ANG_1,str);
	m_CameraAngle[0] = atof(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CAM_ANG_2,str);
	m_CameraAngle[1] = atof(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_CAM_ANG_3,str);
	m_CameraAngle[2] = atof(str);

	/*m_wndDialogControl->GetDlgItemText(IDC_EDIT_WINDOW_WIDTH,str);
	m_WindowWidth = atoi(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_WINDOW_HEIGHT,str);
	m_WindowHeight = atoi(str);*/
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_FOVY,str);
	m_fovy = atof(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_NEAR,str);
	m_near = atof(str);
	m_wndDialogControl->GetDlgItemText(IDC_EDIT_FAR,str);
	m_far = atof(str);

	// UPdate Viewport Parameter
	return 0;
}



/************************************************************************/
/* Calculate 2D keypoints and generate 3D points                        */
/************************************************************************/
LRESULT CKEGIESView::OnCalculateKeypoints( WPARAM wParam, LPARAM lParam )
{
	UnProject = FALSE;
	InvalidateRect(NULL, FALSE);

	//////////////////////////////////////////////////////////////////////////
	// Get the opengl frame and convert to opencv Mat
	cv::Mat img(m_WindowHeight,m_WindowWidth, CV_8UC3);
	//use fast 4-byte alignment (default anyway) if possible
	int k = img.step; // kq = 1920
	size_t	elemsize = img.elemSize(); // kq = 3
	int h = (img.step & 3) ? 1 : 4;
	if (img.step&3)
	{
		TRACE("HELLO\n");
	}
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);// with step = 1920, 4 is selected

	//set length of one complete row in destination data (doesn't need to equal img.cols)
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize()); //elemSize = 3 => number of pixels per rows = 1920/3 = 640
	
	// copy from pixel buffer to img storage
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);  // change GL_BGR to GL_BGR(A) if needed
	
	// flip the image because opencv stores images from top to bottom, left to right
	cv::flip(img, img, 0);
	img.copyTo(keyframe1);	
	

	//////////////////////////////////////////////////////////////////////////
	// Calculate the 2D keypoints
	sift_cpu(img,cv::Mat(),keypoints_cpu,descriptors_cpu,false);
	
	// Traing the Descriptors by Flann based matcher
	dbDescriptors.push_back(descriptors_cpu);
	flannMatcher.add(dbDescriptors);
	flannMatcher.train();	
	
	// Drawing for debug
	cv::Mat tempImg;
	img.copyTo(tempImg);
	
	// Draw only one keypoints
	int kx = (int)keypoints_cpu[10].pt.x;
	int ky = (int)keypoints_cpu[10].pt.y;
	cv::circle(tempImg,cv::Point(kx,ky),4,cv::Scalar(0,0,255));
	//drawKeypoints(tempImg,keypoints_cpu,tempImg,cv::Scalar::all(-1),  cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	cv::imshow("OPENGL FROM TO OPENCV MAT",tempImg);
	CString siftText;
	siftText.Format("Number of keypoints = %d", keypoints_cpu.size());
	MessageBox(siftText,NULL,MB_OK);

	//////////////////////////////////////////////////////////////////////////
	// From 2D keypoints -> interpolate 3D positions of these keypoints	
	// Initialize a Z buffer	
	GetGLMatrices();
	objPoints_cpu.clear();
	keyPoints_selected.clear();
	for (int i = 0; i<(int)keypoints_cpu.size();i++)
	{
		float x = keypoints_cpu[i].pt.x;
		float y = keypoints_cpu[i].pt.y;
		objPoint = GetOGLPos(x,y);
		//TRACE("ObjPoint = [%f; %f; %f; 1]; ImgPoint = [%f;%f]\n",objPoint.x,objPoint.y,objPoint.z, x,y);
		objPoints_cpu.push_back(objPoint);
		keyPoints_selected.push_back(keypoints_cpu[i].pt);
	}	

	//Save keyframe1 and its corresponding keypoints and objectpoints
	
	cv::imwrite("output/keyframe1.jpg",keyframe1);
	fs.open("output/keyframe1.yml",cv::FileStorage::WRITE);
	fs<<"objPoints_cpu"<<objPoints_cpu<<"keyPoints_cpu"<<keyPoints_selected;
	fs.release();
	UnProject = TRUE;
	return 0;
}

LRESULT CKEGIESView::OnInitializeTracking( WPARAM wParam, LPARAM lParam )
{
	if (!m_Capture.isOpened())
	{
		if (m_Capture.open(0))
		{
			m_ProgramMode = INITIALIZE_TRACKING;
		}
		else 
		{
			MessageBox("Initialization Failed");
		}
	}
	else 
	{
		m_ProgramMode = INITIALIZE_TRACKING;
	}
	if (m_ProgramMode == INITIALIZE_TRACKING)
	{
		m_Capture>>m_CurrentFrame;
	}
	
	return 0;
}

LRESULT CKEGIESView::OnStartTracking( WPARAM wParam, LPARAM lParam )
{
	if (keypoints_cpu.size()==0)
	{
		MessageBox("Come back to OFFLINE mode calculate keypoints of keyframe first!!",MB_OK);
		return 0;
	}
	if (!m_Capture.isOpened())
	{
		m_Capture.open(0);
	}
	m_ProgramMode = ONLINE_TRACKING;

	



	return 0;
}

LRESULT CKEGIESView::OnStopTracking( WPARAM wParam, LPARAM lParam )
{
	m_ProgramMode = OFFLINE;
	return 0;
}

#pragma DIALOG_POST_MESSAGE_HANDLERS