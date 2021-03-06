#include "Engine/DevConsole/Profiler/Profiler.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
// CONSTRUCTOR
Profiler::Profiler()
{
	
}

// DESTRUCTOR
Profiler::~Profiler()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Sets the max samples
*@param   : max sample
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::SetMaxSamples(int maxSample)
{
	m_maxSample = maxSample;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Marks the begin or end of frame and 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::MarkFrame()
{
	if(m_activeNode != nullptr)
	{
		PushProfilerObjToQueue(m_activeNode);
		PoPProfiler();
	}
	PushProfiler("framestart"); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/02
*@purpose : Pushes the profiler object
*@param   : Function name
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::PushProfiler(std::string name)
{
	ProfileMeasurement_t *profiler = new ProfileMeasurement_t(name);
	if(m_activeNode == nullptr)
	{
		m_activeNode = profiler;
	}
	else
	{
		m_activeNode->AddChild(profiler);
		profiler->m_parent = m_activeNode;
		m_activeNode = profiler;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/02
*@purpose : Pops current profiler and go up the tree
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::PoPProfiler()
{
	if(m_activeNode == nullptr)
	{
		// ERROR
		return;
	}
	m_activeNode->Finish();
	m_activeNode = m_activeNode->m_parent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Pushes profiler into queue
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::PushProfilerObjToQueue(ProfileMeasurement_t* measurement)
{
	m_measurements.push_back(measurement);
	if(m_measurements.size() > m_maxSample)
	{
		ProfileMeasurement_t* frontMeasurement = m_measurements.front();
		delete frontMeasurement;
		m_measurements.erase(m_measurements.begin(),m_measurements.begin()+1);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Destroys the entire tree recursively and updates the reporter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::DestroyTreeRecursive(ProfileMeasurement_t *root)
{
	ProfilerManager::GenerateReportTree(root);
}
