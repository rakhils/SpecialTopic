#include "Engine/System/System.hpp"
#include <windows.h>		
#include <string>
// CONSTRUCTOR
System::System()
{

}

// DESTRUCTOR
System::~System()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void System::CreateDirectoryFromPath(char const *directoryPath)
{
	std::string str(directoryPath);
	std::wstring folder(str.length(), L' ');
	std::copy(str.begin(), str.end(), folder.begin());

	if (CreateDirectory(folder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		
	}
}
