#pragma once
#include "Engine/DevConsole/Profiler/Profiler.hpp"
/*\class  : ProfilerReportEntry	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 7/3/2018 9:42:47 PM
* \contact: srsrakhil@gmail.com
*/
 
class ProfilerReportEntry
{

public:
	//Member_Variables
	std::string m_id;
	int			m_callCount  ;
	/*uint64_t	m_totalTime  ; // inclusive time including functions called by itself; 
	uint64_t	m_selfTime   ;  // own time = total time - function calls
	uint64_t	m_percentTime;*/

	double		m_totalTimeInSec        = 0.0;
	double		m_selfTimeInSec         = 0.0;
	double		m_totalPercentTimeInSec = 0.0;
	double		m_selfPercentTimeInSec  = 0.0;
	double		m_averageTimeInSec		= 0.0;

	ProfilerReportEntry *m_parent;
	std::map<std::string, ProfilerReportEntry*> m_children;
	//Static_Member_Variables

	//Methods

	ProfilerReportEntry(std::string id);
	~ProfilerReportEntry();
	
	void						PopulateTree(ProfileMeasurement_t *m_frame, double totalFrameTime);
	void						PopulateFlat(ProfileMeasurement_t *leafnode, double totalFrameTime);
	void						AccumulateData(ProfileMeasurement_t *leafnode);
	ProfilerReportEntry*	    CreateOrGetChild(char const *str);
	void						AddChild(ProfilerReportEntry* entry);

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