#include "Engine/DevConsole/Profiler/ProfilerReportEntry.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
ProfilerReportEntry::ProfilerReportEntry(std::string id)
{
	m_id = id;
	m_callCount = 0;
}

// DESTRUCTOR
ProfilerReportEntry::~ProfilerReportEntry()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Populate report entry tree with given root measurement pointer
*@param   : Measurement pointer(root)
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReportEntry::PopulateTree(ProfileMeasurement_t *m_frame,double totalFrameTime)
{
	AccumulateData(m_frame);
	double childTimeInSec = 0;
	for(int index = 0;index < m_frame->m_children.size();index++)
	{
		ProfileMeasurement_t *child = m_frame->m_children.at(index);
		ProfilerReportEntry *entry  = CreateOrGetChild(child->m_name.c_str());
		entry->PopulateTree(child,totalFrameTime);
		childTimeInSec    += entry->m_totalTimeInSec;
	}
	m_selfTimeInSec			    = m_totalTimeInSec - childTimeInSec;
	m_selfPercentTimeInSec      = m_selfTimeInSec  / totalFrameTime * 100;
	m_totalPercentTimeInSec		= m_totalTimeInSec / totalFrameTime * 100;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Populates flat
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReportEntry::PopulateFlat(ProfileMeasurement_t *m_frame,double totalFrameTime)
{
	double childTime = 0;
	for (int index = 0; index < m_frame->m_children.size(); index++)
	{
		ProfileMeasurement_t *child = m_frame->m_children.at(index);
		ProfilerReportEntry *entry  = CreateOrGetChild(child->m_name.c_str());
		PopulateFlat(child, totalFrameTime);
		childTime += entry->m_totalTimeInSec;
	}
	ProfilerReportEntry *entry         = CreateOrGetChild(m_frame->m_name.c_str());
	entry->m_callCount++;
	entry->m_totalTimeInSec			   += m_frame->m_elapsedTimeInSec;
	entry->m_selfTimeInSec             += m_frame->m_elapsedTimeInSec - childTime;
	entry->m_selfPercentTimeInSec	   += (entry->m_selfTimeInSec  / totalFrameTime) * static_cast<double>(100);
	entry->m_totalPercentTimeInSec     += (entry->m_totalTimeInSec / totalFrameTime) * static_cast<double>(100);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Accumulates all values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReportEntry::AccumulateData(ProfileMeasurement_t *leafnode)
{
	m_callCount++;
	m_totalTimeInSec += leafnode->m_elapsedTimeInSec;
	m_averageTimeInSec += leafnode->m_elapsedTimeInSec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Creates child profiler report entry
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProfilerReportEntry* ProfilerReportEntry::CreateOrGetChild(char const *str)
{
	std::string id(str);
	std::map<std::string, ProfilerReportEntry*>::iterator it;
	it = m_children.find(id);
	if(it != m_children.end())
	{
		return it->second;
	}
	ProfilerReportEntry *entry = new ProfilerReportEntry(id);
	m_children[id]			   = entry;
	entry->m_parent			   = this;
	return entry;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Adds a child
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReportEntry::AddChild(ProfilerReportEntry* entry)
{
	m_children[entry->m_id] = entry;
}
