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
class NetObject;
class Bullets
{

public:
	//Member_Variables
	float				m_lifeTime				= 2;
	float				m_spawnTime				= 0;
	bool				m_markAsDead			= false;
	float				m_velocity				= 150.f;
	uint8_t				m_index;

	Vector2				m_position;
	float				m_angle;
	float				m_primary;

	BulletSnapShot_t	m_localBulletSnapshot;
	BulletSnapShot_t *	m_bulletSnapshot		= nullptr;
	NetObject		 *	m_netObject				= nullptr;
	//Static_Member_Variables

	//Methods

	Bullets();
	~Bullets();
	
	void	Update(float deltaTime);
	void	Render();

	void    SetNetObject(NetObject *netObject);
	void	ApplySnapshot(BulletSnapShot_t *snapShot);
	void    SendLocalSnapshot();
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