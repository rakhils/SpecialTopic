#pragma once
#include <string>

/*\class  : BTreeService	   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :	   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/27/2019 8:47:23 PM
* \contact: srsrakhil@gmail.com
*/
class BehaviorTree;
class BTreeService
{

public:
	//Member_Variables
	BehaviorTree * m_btree;
	std::string m_name = "";
	//Static_Member_Variables

	//Methods

	BTreeService();
	~BTreeService();

	virtual void Update() = 0;
	
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