#pragma once
#include <string>
/*\class  : BTreeDecorator		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/27/2019 8:48:30 PM
* \contact: srsrakhil@gmail.com
*/
class BehaviorTree;
class BTreeDecorator
{

public:
	//Member_Variables
	BehaviorTree * m_btree = nullptr;
	std::string    m_name = "";
	//Static_Member_Variables

	//Methods

	BTreeDecorator();
	~BTreeDecorator();
	
	virtual bool CheckAndReturnResult() = 0;
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