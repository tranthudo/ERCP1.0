#ifndef UTILITY_H
#define UTILITY_H

typedef enum _fileType
{
	FILE_TYPE_STL			= 0x0001,
	FILE_TYPE_OBJ			= 0x0010,
	FILE_TYPE_STL_OBJ		= 0x0011,
}fileType;
//Utility function
class Utility
{
public:
	Utility();
	~Utility();

	static void processPointIndex();
	static CString OpenFileDialog(fileType type, char* Title = "Open file");
};

#endif