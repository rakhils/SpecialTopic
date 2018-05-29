#pragma once
#include <string>
#include <crtdbg.h>
#include <algorithm>
#include <time.h>
#include<stdint.h>
#include<windows.h>
/*\class  : TimeSystem		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0	   
* \date   : 3/6/2018 4:28:45 PM
* \contact: srsrakhil@gmail.com
*/
class TimeSystem
{

public:
	//Member_Variables
	float m_frequency;
	float m_seconds_per_count;
	//Static_Member_Variables

	//Methods

	TimeSystem();
	~TimeSystem();

	static std::string GetCurrentDateAndTimeInString();
	static std::string GetCurrentDateAndTimeInString(std::string format);
	
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