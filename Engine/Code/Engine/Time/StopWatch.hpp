#pragma once
#include "Engine/Time/Clock.hpp"
/*\class  : StopWatch
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 6/6/2018 9:26:51 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class StopWatch
{

public:
	//Member_Variables
	Clock * m_clock = nullptr;
	//Static_Member_Variables

	//Methods
	StopWatch(){}
	StopWatch(Clock *ref_clock);
	~StopWatch();

public:
	// should default to Master if ref_clock is set to nullptr

	// change our reference clock. 
	// should maintain elapsed time 
	void SetClock(Clock *ref_clock);

	// sets the "interval time" for
	// this stopwtach.
	// takes seconds to be easy to use
	// but under the hood it will use hpc
	bool SetTimer(float seconds);

	// if we really just want to use it as a timer
	// this returns how long since the last reset
	float GetElapsedTime();

	// elapsed time divided by the interval
	float GetNormalizedElapsedTime();

	// returns true if the interval has elapsed;
	bool HasElapsed();

	// resets the interval
	// (get_elapsed_time() should return 0.0f);
	void Reset();

	// checks the interval, if it has elapsed, will
	// return true and reset.  If not, will just return false (no reset)
	bool CheckAndReset();

	// returns has_elapsed(), and if so
	// will subtract one interval from 
	// the elapsed time (but maintaining 
	// any extra time we have accrued)
	bool Decrement();

	// like decrement, but returns how many 
	// times we have elapsed our timer
	// so say our timer was 0.5 seconds, 
	// and get_elapsed_time() was 2.2s.  
	// decrement_all() would return 4, and 
	// update itself so that get_elapsed_time()
	// would return 0.2s;
	//uint decrement_all();

	// pause and resume [extra/optional]
	// void pause(); 
	// void resume()

	double m_start;
	double m_interval;
	double m_originalInterval;

	//uint64_t m_start_hpc;
	//uint64_t m_interval_hpc;
	
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};