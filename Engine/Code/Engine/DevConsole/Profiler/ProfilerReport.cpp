#include "Engine/DevConsole/Profiler/ProfilerReport.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtil.hpp"
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
	m_root->PopulateTree(root);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/02
*@purpose : Init profiler report
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerReport::Init()
{
	
}
