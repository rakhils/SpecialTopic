#pragma once
#include "Engine/AI/BehaviorTree/Node.hpp"
#include "Engine/AI/BehaviorTree/Decorators/BTreeDecorator.hpp"
#include "Engine/AI/BehaviorTree/Services/BTreeService.hpp"
/*\class  : Composite		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/24/2019 2:23:11 PM
* \contact: srsrakhil@gmail.com
*/
 
class Composite : public Node
{

public:
	//Member_Variables
	std::vector<BTreeDecorator*> m_decorators;
	std::vector<BTreeService*>   m_services;

	//Static_Member_Variables

	//Methods

	Composite();
	~Composite();

	void UpdateServices();
	bool ExecuteDecorators();
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