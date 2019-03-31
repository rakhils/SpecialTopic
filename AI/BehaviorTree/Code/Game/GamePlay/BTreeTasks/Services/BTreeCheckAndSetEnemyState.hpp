#pragma once
#include "Engine/AI/BehaviorTree/Services/BTreeService.hpp"
/*\class  : BTreeCheckAndSetEnemyState		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/27/2019 8:51:14 PM
* \contact: srsrakhil@gmail.com
*/
class Map;
class BTreeCheckAndSetEnemyState : public BTreeService
{

public:
	//Member_Variables
	Map * m_map = nullptr;
	//Static_Member_Variables

	//Methods

	BTreeCheckAndSetEnemyState();
	~BTreeCheckAndSetEnemyState();
	
	void Update();
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