// KEGIESView.h : CKEGIESView 
//
#include "DialogControl.h"
#include "N2TMatcher.h"
#pragma once
// Define Program Mode
#define OFFLINE 0
#define INITIALIZE_TRACKING 1
#define ONLINE_TRACKING 2




class CKEGIESView : public CView
{
protected: // serialization
	CKEGIESView();
	DECLARE_DYNCREATE(CKEGIESView)

// 
public:
	CKEGIESDoc* GetDocument() const;

// function
public:
	//Initialize
	void InitGL();

	//Drawing
	void DrawView();
	void SetupView();
	void UpdateView();
	void GetObjTexture();			// Get OBJ Texture
	void UpdateControlData();		// Update CONTROL DIALOG	
	void GetFrame2GLTexture();		// Get Grabbed Frame	
	void DrawGrabbedFrame();		// Draw Camera image
	// Reading z buffer
	cv::Point3f GetOGLPos(float x, float y);
	void GetGLMatrices();
	bool UnProject;
// variable
public:
	HDC     m_hDC;
    HGLRC   m_hRC;
	GLuint	base;

	//flag
	bool LEFT_DOWN;
	bool RIGHT_DOWN;

	//window
	int m_WindowHeight;
	int m_WindowWidth;
	float m_near; // Third Person View parameters
	float m_far;
	float m_fovy;
	float m_near1;// Third person View parameters
	float m_far1;
	float m_fovy1;
	//mouse position
	vec3d m_MousePos;
	vec3d m_PreMousePos;
	vec3d m_DMousePos;

	//camera manipulation
	CCamera m_Cam;  // Third person Camera View;
	int m_CamMode;

	// Boool
	bool DRAW_FRUSTRUM;	

	vec3d* direc;
	int nbSphere;
	int renderMode;
	// OBJ and TEXTURE
	GLuint objTextureID;
	cv::Mat textureImage;
	bool textureInitialized;
		
	// Real Camera Position and Orientation
	float m_CameraPosition[3];
	float m_CameraAngle[3];

	// Dialog to control Real Camera Parameter
	CDialogControl* m_wndDialogControl;	

	// Feature detection, keypoints, descriptors
	//cv::SURF sift_cpu;
	cv::SIFT sift_cpu;									// using CPU version of SIFT
	std::vector<cv::KeyPoint> keypoints_cpu;			// vector to store SIFT keypoints
	cv::Mat descriptors_cpu;							// matrix to store SIFT descriptors.
	cv::Point3f objPoint;								// hold temporary obj 3D points
	GLint viewport[4];									// View-port matrix
	GLdouble modelview[16];								// Model view matrix
	GLdouble projection[16];							// Projection matrix
	std::vector<cv::Point3f> objPoints_cpu;				// Collection of 3D position and orientation of object point
	

	// OPENCV CAMERA and IMAGES
	cv::VideoCapture m_Capture;							// Camera Capture device
	cv::Mat m_CamIntrinsic;								// Intrinsic Camera Matrix
	cv::Mat m_CamExtrinsic;								// Extrinsic Camera Matrix
	cv::Mat distCoeffs;
	cv::Mat rvec;										// Rotation vectors
	cv::Mat tvec;										// Translation vectors
	double* _r;											// Pointer of rvec data
	double* _t;											// Pointer of tvec data;
	cv::Mat rMat;										// Rotation Matrix
	double* _rMat;										// pointer to Rotation matrix
	std::vector<int> inliers;							// Hold the inliers vectors

	cv::Mat m_CurrentFrame;								// Current frame image
	cv::Mat m_PreviousFrame;							// Previous frame image
	GLuint  camTextureID;								// Texture ID from the camera frame
	std::vector<cv::KeyPoint> currentKeypoints_cpu;		// Current frame keypoints
	cv::Mat currentDescriptors_cpu;						// Current frame descriptors

	cv::Mat keyframe1;
	std::vector<cv::KeyPoint> keypoints1;
	cv::Mat img_matches;
	int i;
	int ksize;
	// Feature detection & Matching
	//cv::BFMatcher flannMatcher;
	cv::FlannBasedMatcher flannMatcher;					// Define a Flann based matcher	
	cv::FlannBasedMatcher flannMatcher2;				// Define another flannbased matcher;
	N2TMatcher			  n2tMatcher;					// N2T matcher
	std::vector<std::vector<cv::DMatch >> matches;		// Matches vector
	std::vector<std::vector<cv::DMatch >> matches2;		// Matches vector 2
	std::vector<cv::DMatch> ratioMatches;				// ratio matches
	std::vector<cv::DMatch> ratioMatches2;				// ratio matches 2;
	std::vector<cv::DMatch> symmetryMatches;			// symmetry matches
	std::vector<cv::Mat> dbDescriptors;					// database descriptor
	std::vector<cv::Point3f> objPoints_selected;		// obj Points
	std::vector<cv::Point2f> keyPoints_selected;		// keypoints Selected
	// Program modes
	int m_ProgramMode;
	bool m_PreviousTrackingState;
	bool m_CurrrentTrackingState;
	// Auxiliary
	CString myText;
	int64 tinit;										// For calculating ticktime
	int64 utime;										// time using to calculate
	float freq;											// hold the freq tick time freq = 1000./cv::getTickFrequency();
	cv::FileStorage fs;
	int sizeM;											// Number of matched keypoints	
// 
public:
	virtual void OnDraw(CDC* pDC);						// 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// 
public:
	virtual ~CKEGIESView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void OnInitialUpdate();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	//Key board
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//Timer
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//Mouse function
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	//Menu
	afx_msg void OnRendermodeLine();
	afx_msg void OnRendermodeLineandsurface();
	afx_msg void OnRendermodeSurface();
	afx_msg void OnCammodeRealcam();
	afx_msg void OnCammodeThirdperson();
	afx_msg void OnUpdateCammodeRealcam(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCammodeThirdperson(CCmdUI *pCmdUI);
// User define
public:
		afx_msg void OnViewfrustrumYes();
		afx_msg void OnUpdateViewfrustrumYes(CCmdUI *pCmdUI);
		afx_msg void OnViewfrustrumNo();
		afx_msg void OnUpdateViewfrustrumNo(CCmdUI *pCmdUI);

protected:
	// Meassage handler (messages from the dialog)
	afx_msg LRESULT OnUpdateControlEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCalculateKeypoints(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitializeTracking(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartTracking(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopTracking(WPARAM wParam, LPARAM lParam);

};

#ifndef _DEBUG  // KEGIESView
inline CKEGIESDoc* CKEGIESView::GetDocument() const
   { return reinterpret_cast<CKEGIESDoc*>(m_pDocument); }
#endif

