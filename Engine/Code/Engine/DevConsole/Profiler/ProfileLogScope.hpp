#pragma once
#include "Engine/Time/Clock.hpp"
/*\class  : ProfileLogScope		   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 7/18/2018 5:42:28 PM
* \contact: srsrakhil@gmail.com
*/
 
class ProfileLogScope
{

public:
	//Member_Variables
	double	    m_startTime;
	uint64_t    m_startHPC;
	char const *m_tag;
	//Static_Member_Variables

	//Methods

	ProfileLogScope(char* tag);
	~ProfileLogScope();
	
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