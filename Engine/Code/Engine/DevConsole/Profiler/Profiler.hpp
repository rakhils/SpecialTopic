#pragma once
#include <vector>
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Time/TimeSystem.hpp"
/*\class  : Profiler		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 7/1/2018 7:06:36 PM
* \contact: srsrakhil@gmail.com
*/
struct ProfileMeasurement_t
{
	std::string m_name;
	uint64_t    m_start;
	uint64_t    m_end;
	uint64_t	m_elapsedTime;

	double      m_startTimeInSec;
	double      m_endTimeInSec;
	double      m_elapsedTimeInSec;

	ProfileMeasurement_t* m_parent     = nullptr;
	std::vector<ProfileMeasurement_t*> m_children;
	ProfileMeasurement_t(std::string name)
	{
		m_name			 = name;
		m_start			 = Clock::g_theMasterClock->total.m_hpc;
		m_startTimeInSec = GetCurrentTimeSeconds();
	}
	~ProfileMeasurement_t()
	{
		for(int index = 0;index < m_children.size();index++)
		{
			delete m_children.at(index);
		}
	}
	
	void Finish()
	{
		UpdateEndTime();
	}
	uint64_t GetElapsedTime()
	{
		return m_elapsedTime;
	}
	double GetElapsedTimeInSeconds()
	{
		return m_elapsedTimeInSec;
	}
	void UpdateEndTime()
	{
		m_end		       = Clock::g_theMasterClock->total.m_hpc;
		m_elapsedTime      = m_end - m_start;
		m_endTimeInSec     = GetCurrentTimeSeconds();
		m_elapsedTimeInSec = m_endTimeInSec - m_startTimeInSec;
	}
	void AddChild(ProfileMeasurement_t* measurement)
	{
		m_children.push_back(measurement);
	}
};

class Profiler
{

public:
	//Member_Variables
	ProfileMeasurement_t*			   m_activeNode;
	std::vector<ProfileMeasurement_t*> m_measurements;
	int								   m_maxSample;
	//Static_Member_Variables
	//Methods

	Profiler();
	~Profiler();
	void   Update(float deltaTime);
	void   Render();
	void   SetMaxSamples(int maxSample);

	void   MarkFrame();
	void   PushProfiler(std::string name);
	void   PoPProfiler();
	void   PushProfilerObjToQueue(ProfileMeasurement_t* measurement);
	void   DestroyTreeRecursive(ProfileMeasurement_t *root);
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