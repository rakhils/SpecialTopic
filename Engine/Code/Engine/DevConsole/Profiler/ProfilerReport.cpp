#include "Engine/DevConsole/Profiler/ProfilerReport.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/DevConsole/Profiler/ProfilerConstants.hpp"
#include "Engine/DevConsole/Profiler/ProfilerReportEntry.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
// CONSTRUCTOR
ProfilerReport::ProfilerReport()
{
	
}

// DESTRUCTOR
ProfilerReport::~ProfilerReport()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReport::GenerateTreeReportFromFrame(ProfileMeasurement_t *root)
{
	m_root = new ProfilerReportEntry(root->m_name);
	m_root->PopulateTree(root,root->m_elapsedTimeInSec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReport::GenerateFlatReportFromFrame(ProfileMeasurement_t *root)
{
	m_root = new ProfilerReportEntry(root->m_name.c_str());
	m_root->AddChild(m_root);
	m_root->PopulateFlat(root, root->m_elapsedTimeInSec);
	switch (ProfilerManager::s_profilerReportSortType)
	{
	case TOTAL:
		SortFlatReportByTotalTime();
		break;
	case SELF:
		SortFlatReportBySelfTime();
		break;
	case NONE:
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Sorts flat report by total time
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReport::SortFlatReportByTotalTime()
{
	std::map<std::string, ProfilerReportEntry*>::iterator it1;
	m_sortedByTime.clear();
	for (it1 = m_root->m_children.begin(); it1 != m_root->m_children.end(); it1++)
	{
		m_sortedByTime.push_back(it1->second);
	}

	for (int index1 = 0; index1 < m_sortedByTime.size(); index1++)
	{
		for (int index2 = index1 + 1; index2 < m_sortedByTime.size(); index2++)
		{
			if(m_sortedByTime.at(index1)->m_totalTimeInSec < m_sortedByTime.at(index2)->m_totalTimeInSec)
			{
				ProfilerReportEntry* temp = m_sortedByTime.at(index1);
				m_sortedByTime.at(index1) = m_sortedByTime.at(index2);
				m_sortedByTime.at(index2) = temp;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Sorts by self time
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReport::SortFlatReportBySelfTime()
{
	std::map<std::string, ProfilerReportEntry*>::iterator it1;
	m_sortedBySelfTime.clear();
	for (it1 = m_root->m_children.begin(); it1 != m_root->m_children.end(); it1++)
	{
		m_sortedBySelfTime.push_back(it1->second);
	}

	for (int index1 = 0; index1 < m_sortedBySelfTime.size(); index1++)
	{
		for (int index2 = index1 + 1; index2 < m_sortedBySelfTime.size(); index2++)
		{
			if (m_sortedBySelfTime.at(index1)->m_selfTimeInSec < m_sortedBySelfTime.at(index2)->m_selfTimeInSec)
			{
				ProfilerReportEntry* temp = m_sortedBySelfTime.at(index1);
				m_sortedBySelfTime.at(index1) = m_sortedBySelfTime.at(index2);
				m_sortedBySelfTime.at(index2) = temp;
			}
		}
	}
}
