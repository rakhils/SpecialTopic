#pragma once
#include "Engine/GameObject/GameObject.hpp"
#include "Game/GameCommon.hpp"
/*\class  : Bullet
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
* \date   : 5/4/2018 4:06:31 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class Bullet : public GameObject
{

public:
	//Member_Variables
	int m_index = 0;
	float m_lifeTime	= 15;
	Vector3 m_startPos;
	float   m_launchSpeed;
	float   m_startTime = 0.f;
	float m_radius		= 1;
	float m_speed		= 0;
	int   m_teamNumber  = 0;
	bool  m_markForDelete = false;
	float m_forceApplied = 1000;
	bool  m_isTrajectory = false;
	float m_launchAngle;
	Vector3 m_newDirection;
	//Static_Member_Variables

	//Methods

	Bullet(std::string name);
	Bullet(std::string name,int team,Vector3 position, Vector3 direction,float speed,bool isArcher,Vector3 endpos);
	~Bullet();
	
	void Update(float deltaTime);
	void Render();
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