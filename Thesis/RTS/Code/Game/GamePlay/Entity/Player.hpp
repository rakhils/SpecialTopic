#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Rgba.hpp"

/*\class  : Player	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/2/2018 10:07:09 PM
* \contact: srsrakhil@gmail.com
*/
struct PlayerSnapShot_t
{
	Vector2		m_position;
	float		m_angle;
	float		m_primary;

};
class NetObject;
class Player
{

public:
	//Member_Variables
	bool		m_isLastSent0 = false;
	std::string m_name;
	Rgba		m_color;
	uint8_t		m_ownerConnectionIndex;
	uint8_t		m_index;

	uint8_t		m_killCount;
	uint8_t		m_deathCount;

	Vector2		m_position;
	float 		m_angle;
	float		m_primary;
	float		m_secondary;

	PlayerSnapShot_t  m_localPlayerSnapshot;
	PlayerSnapShot_t *m_playerSnapshot  = nullptr;
	NetObject		 *m_netObject		= nullptr;
	//Entity *m_target;
	//Static_Member_Variables

	//Methods

	Player();
	~Player();
	void    SetNetObject(NetObject *netObject);

	void	Update(float deltaTime);
	void	Render();

	void *	UpdateAndGetLocalSnapshot();
	void	UpdateLocalPlayerSnapshot();
	void    SendLocalSnapshot();

	void	ApplySnapshot(PlayerSnapShot_t *snapShot);
	void *  GetLocalSnapShot();

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