#pragma once

/*\class  : ProfileLogScoped		   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 7/6/2018 12:09:09 AM
* \contact: srsrakhil@gmail.com
*/
class ProfileLogScoped
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods
public:
	ProfileLogScoped(char const *tag);
	~ProfileLogScoped();

	double   m_seconds;
	char const *tag;
	
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