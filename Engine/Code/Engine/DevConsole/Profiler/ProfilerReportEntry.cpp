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
	double   childTimeInSec  = 0.0;
	double   parentTotalTimeInSec = 1.0;
	double   parentSelfTimeInSec = 1.0;
	double   parentTotalPercent  = 1.0;
	double   parentSelfPercent   = 1.0;
	if(m_parent != nullptr)
	{
		parentTotalTimeInSec     = m_parent->m_totalTimeInSec;
		parentSelfTimeInSec		 = m_parent->m_selfTimeInSec;
		parentTotalPercent		 = m_parent->m_totalPercentTimeInSec;
		parentSelfPercent		 = m_parent->m_selfPercentTimeInSec;	
	}
	if (m_parent != nullptr)
	{
		m_totalPercentTimeInSec = m_totalTimeInSec / parentTotalTimeInSec * parentTotalPercent;
	}
	for(int index = 0;index < m_frame->m_children.size();index++)
	{
		ProfileMeasurement_t *child = m_frame->m_children.at(index);
		ProfilerReportEntry *entry  = CreateOrGetChild(child->m_name.c_str());
		
		entry->PopulateTree(child,totalFrameTime);
		childTimeInSec    += entry->m_totalTimeInSec;
	}
	m_selfTimeInSec			    = m_totalTimeInSec - childTimeInSec;
	if (m_parent != nullptr)
	{
		m_selfPercentTimeInSec = m_selfTimeInSec / totalFrameTime *100 ;	
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Populates flat
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReportEntry::PopulateFlat(ProfileMeasurement_t *leafnode,double totalFrameTime)
{
	UNUSED(totalFrameTime);
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
