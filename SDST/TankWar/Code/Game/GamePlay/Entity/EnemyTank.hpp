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
	Vector3 m_forward;
	Vector3 m_seekPosition;
	float   m_velocity = 500;
	float   m_minDistanceToFollow = 70;
	float   m_minDistanceForSeperation = 5;
	float   m_minDistanceForCohesion   = 15;
	float   m_minDistanceForAlignment  = 10;
	float   m_hitInterval = 0;
	int     m_teamNumber;
	bool    m_markAsDead = false;
	float   m_timeLeftForNextDirectionChange = 10.f;
	//Static_Member_Variables

	//Methods

	EnemyTank();
	EnemyTank(std::string name,Scene *scene, Vector3 position);
	~EnemyTank();
	Vector3 GetCurrentForward();

	void Update(float deltatime);
	void UpdateHitFeedBack(float deltatime);

	void UpdateBounds(float deltaTime);
	void UpdateSeekDirection(float weight);
	void UpdateSeperationDirection(float weight);
	void UpdateCohesionDirection(float weight);
	void UpdateAlignmentDirection(float weight);
	void UpdateRandomDirection(float weight);

	void UpdateTankTurn();
	void UpdateTankOrientation();
	void UpdateTankHeightOnTerrain();
	void UpdateForceOnTank(float deltaTime);
	void OnCollisionEnter(Collider *collider);
	
	void ChangeForwardDirection(Vector3 direction);
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