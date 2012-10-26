#include "stdafx.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Utility::Utility()
{

}

Utility::~Utility()
{

}

//	Check the point on BC1
//	and check the element line
void Utility::processPointIndex()
{

	CString fileObjPath = OpenFileDialog(FILE_TYPE_OBJ, "Open whole object");
	CString fileBCPath = OpenFileDialog(FILE_TYPE_OBJ, "Open Boundary data");
	if(fileObjPath.IsEmpty() || fileBCPath.IsEmpty())
		return;
	
	CObj object;
	CObj BCObj;


	if(object.ReadObjData(fileObjPath.GetBuffer()) && BCObj.ReadObjData(fileBCPath.GetBuffer()))
	{
		int* BCNode = new int[BCObj.PointNum()];
		int index = 0;
		for (int i = 0; i< object.PointNum(); i++)
		{
			vec3d* curPoint = (object.Point() + i);
			//Check if this point belong to boundary
			for (int j = 0; j < BCObj.PointNum(); j++)
			{
				vec3d* curBCPoint = (BCObj.Point() + j);
				if(*curPoint == *curBCPoint)
				{
					*(BCNode + index++) = i;
					break;
				}
			}
		}

		//Write to file
		CString output = fileBCPath;
		output.Delete(output.GetLength()-3,3);
		output += "txt";
		if(FILE* fp = fopen(output.GetBuffer(), "w"))
		{
			for(int i=0; i<index;i++)
				fprintf(fp,"%d\n",*(BCNode + i));

			fclose(fp);

			AfxMessageBox("Process finished", MB_OK|MB_ICONINFORMATION);
		}
		delete []BCNode;
	}
}

CString Utility::OpenFileDialog(fileType type, char* Title)
{
	CString ext;
	if(type & FILE_TYPE_STL)
		ext += "STL Files (*.stl)|*.stl|";
	if(type & FILE_TYPE_OBJ)
		ext += "Defined obj Files (*.obj)|*.obj|";

	ext += "All Files (*.*)|*.*|";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, ext.GetBuffer());
	// Set title
	dlg.m_ofn.lpstrTitle = Title;

// 	// Set initial directory
// 	FileOpenDialog.m_ofn.lpstrInitialDir = "c:\\temp";

	if(dlg.DoModal() == IDOK)
	{
		return dlg.GetPathName();
	}

	return CString();
}