#include "Engine/Time/TimeSystem.hpp"

// CONSTRUCTOR
TimeSystem::TimeSystem()
{
	LARGE_INTEGER hpf;
	::QueryPerformanceFrequency(&hpf);

	m_frequency = (static_cast<float>((uint64_t)&hpf));
	m_seconds_per_count = static_cast<float>(1.0 / (double)m_frequency);
}

// DESTRUCTOR
TimeSystem::~TimeSystem()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Gets current system time in string
*@param   : NIL
*@return  : Time in String 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string TimeSystem::GetCurrentDateAndTimeInString()
{
	return GetCurrentDateAndTimeInString("%Y/%m/%d %H:%M:%S");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/26
*@purpose : Gets current system time in given format string
*@param   : Time format
*@return  : Time in string
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string TimeSystem::GetCurrentDateAndTimeInString(std::string format)
{
	time_t rawtime;
	char buffer[80];
	time(&rawtime);

	struct tm info;
	localtime_s(&info, &rawtime);
	strftime(buffer, 50, format.c_str(), &info);
	return std::string(buffer);
}
