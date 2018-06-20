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
	//Static_Member_Variables

	//Methods

	Tank();
	~Tank();

	Vector3 GetTurretForward();
	void    FireBullet(float deltaTime);
	void	Respawn(int tryCount);
	void Update(float deltaTime);
	void UpdateBullet(float deltaTime);
	void UpdateCameraPosition(float deltaTime);
	void UpdateCameraOrientation(float deltaTime);
	void UpdateTankOrientation();
	void UpdateBreadCrumb(float deltatime);
	void UpdateTurretOrientation(float deltaTime);
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