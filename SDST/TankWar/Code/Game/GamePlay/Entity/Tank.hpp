#pragma once
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Time/StopWatch.hpp"
/*\class  : Ship		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 5/7/2018 12:25:55 AM
* \contact: srsrakhil@gmail.com
*/
class Bullet;
class Tank : public GameObject
{

public:
	//Member_Variables
	float m_lastBulletSpawnTime;
	float m_bulletSpawnInterval  = 0.25;
	float m_health = 10; 
	float m_velocity = 50.f;
	bool  m_markForDead = false;
	float m_timeElapsed = 0;
	float m_timeLeftToRespawn = 5;
	StopWatch			  m_stopWatch;
	Vector3				  m_mousePosForward;
	Vector3				  m_mouseWorldPos;
	std::vector<Bullet*>  m_bullets;
	GameObject			 *m_turret = nullptr;
	GameObject			 *m_turretHead = nullptr;
	Vector3				  m_turretForwardDirection;
	float				  m_bulletCoolDown = 0;
	bool				  m_isArcherBullet = false;
	Vector3				  m_newDirection;
	float				  m_timeToLookNewDir = 0.f;
	//Static_Member_Variables

	//Methods

	Tank();
	~Tank();

	Vector3 GetTurretForward();
	void    FireBullet(float deltaTime,Vector3 direction,bool isArcher);
	void	Respawn(int startCount,int tryCount);
	void Update(float deltaTime);
	void UpdateBullet(float deltaTime);
	void UpdateCameraPosition(float deltaTime);
	void UpdateCameraOrientation(float deltaTime);
	void UpdateTankOrientation();
	void UpdateBreadCrumb(float deltatime);
	void UpdateRaycastFromTurret(float deltaTime);
	void UpdateTurretOrientation(float deltaTime);
	void UpdateTurretOrientation1(float deltaTime,Vector3 direction);
	void UpdateProjectedLines(float deltaTime);
	void OnCollisionEnter(Collider* collider);
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