//-----------------------------------------------------------------------------------------------
// Time.cpp
//	
//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define   WIN32_LEAN_AND_MEAN
#include <Windows.h>

class LocalTimeData
{
public:
	LocalTimeData()
	{
		::QueryPerformanceFrequency((LARGE_INTEGER*)&m_hpc_per_second);

		// do the divide now, to not pay the cost later
		m_seconds_per_hpc = 1.0 / (double)m_hpc_per_second;
	}

public:
	uint64_t m_hpc_per_second;
	double m_seconds_per_hpc;
};

static LocalTimeData gLocalTimeData;
//-----------------------------------------------------------------------------------------------

double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/06
*@purpose : Gets raw performance counter
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
uint64_t GetPerformanceCounter()
{
	LARGE_INTEGER hpc;
	::QueryPerformanceCounter(&hpc);

	return (uint64_t)(hpc.QuadPart);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/06
*@purpose : converts a performance count the seconds it represents
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
double PerformanceCounterToSeconds(uint64_t seconds)
{
	return (double)(seconds*gLocalTimeData.m_seconds_per_hpc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t SecondsToPerformanceCounter(double seconds)
{
	return static_cast<uint64_t>(seconds) * gLocalTimeData.m_hpc_per_second;
}

