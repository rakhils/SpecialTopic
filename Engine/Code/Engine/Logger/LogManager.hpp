#pragma once
#include <string>
#include <map>
#include <stdarg.h>

/*\class  : LogManager		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 5/20/2018 2:03:29 AM
* \contact: srsrakhil@gmail.com
*/
class LogManager
{

public:
	//Member_Variables
	static std::map<std::string, FILE*> s_logIdMaps;
	//Static_Member_Variables

	//Methods

	LogManager();
	~LogManager();

	static void PushLog(std::string logId, char const *, ...);
	
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