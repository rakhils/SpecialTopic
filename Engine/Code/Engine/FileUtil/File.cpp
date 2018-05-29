
#include "Engine/FileUtil/File.h"
//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Reads file to char buffer and returns the same
*@param   : File name
*@return  : File content as buffer
*//////////////////////////////////////////////////////////////
void* FileReadToBuffer(char const *filename)
{
	FILE *fp = nullptr;
	fopen_s(&fp, filename, "r");
	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	char* buffer = (char*)malloc(size + 1U); // space for NULL 

	size_t read = fread(buffer, 1, size, fp);
	fclose(fp);

	buffer[read] = NULL;
	return buffer;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : writes char pointer with lenth into a file
*@param   : File name,content and length
*@return  : If write success true else false
*//////////////////////////////////////////////////////////////
bool FileWrite(std::string filename, char *content, int length)
{
	FILE *file;
	fopen_s(&file, filename.c_str(), "w"); 
	if (file != nullptr)
	{
		std::string contentstring(content);
		for (int index = 0; index < length; index++)
		{
			fprintf(file, "%c", *(content + index));
		}
		fclose(file);
		return true;
	}
	return false;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Opens file in append mode
*@param   : filename
*@return  : FilePointer 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FILE * FileAppend(std::string fileName)
{
	FILE *file;
	fopen_s(&file, fileName.c_str(), "a");
	return file;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Appends string to a file
*@param   : File pointer, string content
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FileAppendString(FILE* fp, std::string content)
{
	fputs(content.c_str(),fp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Closes files
*@param   : File pointer
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CloseFile(FILE *fp)
{
	fclose(fp);
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/17
*@purpose : Reads entire file and return as string
*@param   : filename
*@return  : file content as string
*//////////////////////////////////////////////////////////////
std::string GetFileContentAsString(char const* filename)
{
	FILE *fp = nullptr;
	fopen_s(&fp, filename, "r");
	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	char* buffer = (char*)malloc(size + 1U); // space for NULL 

	size_t readLength = fread(buffer, 1, size, fp);
	fclose(fp);

	buffer[readLength] = NULL;
	std::string fileContent(buffer, 0, readLength);
	return fileContent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Checks if file exist or not
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DoFileExist(std::string filepath)
{
	FILE *fp = nullptr;
	fopen_s(&fp, filepath.c_str(), "r");
	if (fp == nullptr)
	{
		return false;
	}
	return true;
}
