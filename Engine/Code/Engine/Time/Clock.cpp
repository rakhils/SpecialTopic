#include "Engine/Time/Clock.hpp"
#include "Engine/Debug/DebugDraw.hpp"
Clock *Clock::g_theMasterClock = nullptr;
Clock::Clock(Clock *parent /*= nullptr */)
{
	if (this == g_theMasterClock || g_theMasterClock == nullptr)
	{
		m_parent = nullptr;
		m_lastFrameHPC = GetPerformanceCounter();
		return;
	}
	if(parent != nullptr)
	{
		m_parent = parent;
	}
	else
	{
		m_parent = g_theMasterClock;
		g_theMasterClock->AddChild(this);
		m_lastFrameHPC = GetPerformanceCounter();
		return;
	}
	if(m_parent != nullptr)
	{
		m_parent->AddChild(this);
	}
	m_lastFrameHPC = GetPerformanceCounter();
	return;
	
}

// DESTRUCTOR
Clock::~Clock()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Clock::ResetClock()
{
	m_startHPC = GetPerformanceCounter();
	m_lastFrameHPC = m_startHPC;
	m_frameCount = 0;

	frame.TimeReset();
	total.TimeReset();
}

void Clock::BeginFrame()
{
	//ASSERT(parent == nullptr);
	uint64_t hpc = GetPerformanceCounter();
	uint64_t delta = hpc - m_lastFrameHPC;
	m_lastFrameHPC = hpc;
	StepClock(delta);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : Adds childrn
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Clock::AddChild(Clock *child)
{
	m_children.push_back(child);
}

void Clock::ClockSystemStartUp()
{
	ResetClock();
}

void Clock::StepClock(uint64_t elapsed)
{
	m_frameCount++;
	if (m_paused)
	{
		elapsed = 0;
	}
	else
	{
		elapsed = static_cast<uint64_t>((double)elapsed * m_timeScale);
	}

	double elapsed_seconds = PerformanceCounterToSeconds(elapsed);
	frame.m_seconds = elapsed_seconds;
	frame.m_hpc = elapsed;
	frame.m_milliSeconds = static_cast<unsigned int>(frame.m_seconds * 1000);

	total.m_seconds		 += frame.m_seconds;
	total.m_hpc			 += frame.m_hpc;
	total.m_milliSeconds += frame.m_milliSeconds;
	for (size_t index = 0; index < m_children.size(); index++)
	{
		m_children.at(index)->StepClock(elapsed);
	}
}

double Clock::GetMasterDeltaTime()
{
	//return frame.m_seconds;
	return g_theMasterClock->frame.m_seconds;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Clock::SetPausedState(bool paused)
{
	m_paused = paused;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/20
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Clock::SetScale(float scale)
{
	m_timeScale = scale;
}
