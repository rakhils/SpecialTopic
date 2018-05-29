#pragma once
#include <stdint.h>
#include <vector>
#include <vcruntime_string.h>

#include "Engine/Core/Time.hpp"
/*\class  : Clock	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/6/2018 4:15:45 PM
* \contact: srsrakhil@gmail.com
*/

struct TimeUnit
{
	uint64_t    m_hpc;
	double      m_hpcSeconds;
	double		m_seconds;
	uint64_t    m_milliSeconds;

	TimeUnit()
    {
		memset(this,0,sizeof(TimeUnit));  // setting to 0
    }

	void TimeReset()
	{
		memset(this,0,sizeof(TimeUnit));
	}
};

class Clock
{

public:
	//Member_Variables
	uint64_t m_startHPC;
	uint64_t m_lastFrameHPC;
	uint64_t m_frameCount;
	double   m_timeScale = 1;
	bool     m_paused;

	TimeUnit frame;
	TimeUnit total;
	Clock *m_parent = nullptr;
	std::vector<Clock*> m_children;

	//Static_Member_Variables
	static Clock *g_theMasterClock;
	//Methods
	
	Clock( Clock *parent = nullptr );
	~Clock();
	
	void	ResetClock();
	void	BeginFrame();
	
	void	AddChild(Clock *child);
	void	StepClock(uint64_t hpc);
	void	SetPausedState(bool paused);
	void	SetScale(float scale);

	//Static_Methods
	void    ClockSystemStartUp();
	double  GetMasterDeltaTime();

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
