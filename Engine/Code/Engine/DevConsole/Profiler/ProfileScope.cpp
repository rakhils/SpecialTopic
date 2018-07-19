#include "Engine/DevConsole/Profiler/ProfileScope.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProfileScope::ProfileScope(char const *tag)
{
	ProfilerManager::PushProfiler(std::string(tag));
}

// DESTRUCTOR
ProfileScope::~ProfileScope()
{
	ProfilerManager::PoPProfiler();
}