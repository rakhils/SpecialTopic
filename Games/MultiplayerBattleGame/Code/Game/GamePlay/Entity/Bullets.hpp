#pragma once
#include "Engine/Math/Vector2.hpp"
/*\class  : Bullets		   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/4/2018 10:06:11 PM
* \contact: srsrakhil@gmail.com
*/
struct BulletSnapShot_t
{
	Vector2		m_position;
	float		m_angle;
	float		m_primary;
};
class Mesh;
class NetObject;
class Bullets
{

public:
	//Member_Variables
	float				m_lifeTime				= 5;
	float				m_spawnTime				= 0;
	bool				m_markAsDead			= false;
	float				m_velocity				= 20.f;
	uint8_t				m_index;
	uint8_t				m_playerID				= -1;

	Vector2				m_position;
	float				m_angle;
	float				m_health;

	Mesh *				m_bulletMesh;
	NetObject *			m_netObject				= nullptr;
	//Static_Member_Variables

	//Methods

	Bullets(Vector2 position,float angle,float health);
	~Bullets();
	
	void	Update(float deltaTime);
	void	Render();

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