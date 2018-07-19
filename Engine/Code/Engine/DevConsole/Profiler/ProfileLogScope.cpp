#include "Engine/DevConsole/Profiler/ProfileLogScope.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Logger/LogManager.hpp"
// CONSTRUCTOR
ProfileLogScope::ProfileLogScope(char * tag)
{
	m_tag = tag;
	m_startTime = GetCurrentTimeSeconds();
	m_startHPC  = Clock::g_theMasterClock->total.m_hpc;
}

// DESTRUCTOR
ProfileLogScope::~ProfileLogScope()
{
	double endTime = GetCurrentTimeSeconds();
	double timeDiff = endTime - m_startTime;
	uint64_t diff   = Clock::g_theMasterClock->total.m_hpc - m_startHPC;
	std::string text(m_tag);
	LogManager::GetInstance()->LogPrintf("%s Took Time : %f SECONDS ", m_tag,static_cast<double>(timeDiff));
}
