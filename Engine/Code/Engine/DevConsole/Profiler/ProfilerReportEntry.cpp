#include "Engine/DevConsole/Profiler/ProfilerReportEntry.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
ProfilerReportEntry::ProfilerReportEntry(std::string id)
{
	m_id = id;
	m_callCount = 0;
	m_totalTime = static_cast<uint64_t>(0);
	m_selfTime  = static_cast<uint64_t>(0);
	m_percentTime = static_cast<uint64_t>(0);
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
void ProfilerReportEntry::PopulateTree(ProfileMeasurement_t *m_frame)
{
	AccumulateData(m_frame);
	uint64_t childrenTotalTime = static_cast<uint64_t>(0);
	double   childTimeInSec = 0.0;
	for(int index = 0;index < m_frame->m_children.size();index++)
	{
		ProfileMeasurement_t *child = m_frame->m_children.at(index);
		ProfilerReportEntry *entry  = CreateOrGetChild(child->m_name.c_str());
		entry->PopulateTree(child);
		childrenTotalTime += entry->m_totalTime;
		childTimeInSec += entry->m_totalTimeInSec;
	}
	m_selfTime = m_totalTime - childrenTotalTime;
	m_selfTimeInSec = m_totalTimeInSec - childTimeInSec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Populates flat
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReportEntry::PopulateFlat(ProfileMeasurement_t *leafnode)
{
	UNUSED(leafnode);
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
	m_totalTime      += leafnode->m_elapsedTime;
	m_totalTimeInSec += leafnode->m_elapsedTimeInSec;
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
