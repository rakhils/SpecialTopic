#include "Engine/Logger/LogManager.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Time/TimeSystem.hpp"
bool LogManager::s_logEnabled = false;
std::map<std::string, FILE*> LogManager::s_logIdMaps;
// CONSTRUCTOR
LogManager::LogManager()
{

}

// DESTRUCTOR
LogManager::~LogManager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogManager::PushLog(std::string logId, char const *format, ...)
{
	if(!s_logEnabled)
	{
		return;
	}
	va_list args;
	va_start(args, format);
	char buffer[1000];
	vsnprintf_s(buffer, 1000, format, args);
	va_end(args);

	std::string datetime = TimeSystem::GetCurrentDateAndTimeInString();

	FILE *filePtr = s_logIdMaps[logId];

	if(filePtr == nullptr)
	{
		filePtr = FileAppend(logId);
		s_logIdMaps[logId] = filePtr;
	}
	FileAppendString(filePtr, datetime + " :: " + std::string(buffer));
}
