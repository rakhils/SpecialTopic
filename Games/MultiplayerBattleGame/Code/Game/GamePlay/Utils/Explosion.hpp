#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Game/GameCommon.hpp"
/*\class  : Explosion		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/1/2018 9:06:55 PM
* \contact: srsrakhil@gmail.com
*/
class SpriteAnimation;
class SpriteSheet;
class NetObject;
class Explosion
{

public:
	//Member_Variables
	float m_startTime;

	Vector2			 m_position;
	float			 m_radius						= g_playerWidth;
	SpriteAnimation *m_explosionAnimation			= nullptr;
	SpriteSheet *	 m_explosionSpriteSheet			= nullptr;
	bool			 m_markForDelete				= false;
	float			 m_duration						= 2.f;

	NetObject *		 m_netObject					= nullptr;
	//Static_Member_Variables

	//Methods

	Explosion(Vector2 position);
	~Explosion();

	void Update(float deltaTime);
	void Render();
	
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