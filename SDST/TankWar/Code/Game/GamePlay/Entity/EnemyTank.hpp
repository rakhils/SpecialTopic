#pragma once
#include "Engine/GameObject/GameObject.hpp"
/*\class  : EnemyTank
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 6/10/2018 5:45:44 PM
*
* \contact: srsrakhil@gmail.com
*/
class Scene;
class EnemyTank : public GameObject
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	EnemyTank();
	EnemyTank(std::string name,Scene *scene, Vector3 position);
	~EnemyTank();

	void Update(float deltatime);
	void UpdateTankOrientation();
	void OnCollisionEnter(Collider *collider);
	
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