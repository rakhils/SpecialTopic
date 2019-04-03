#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
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
class Mesh;
class Player : public Entity
{

public:
	//Member_Variables
	std::string			m_name;
	float				m_multiplier = 15.f;
	//Static_Member_Variables

	//Methods

	Player(Vector2 position);
	~Player();

	Vector2	GetBulletSpawnPosition();
	void    SetPosition(Vector2 position);
	void	SetDirection(float angle);

	void    ProcessInputs(float deltaTime);
	void	Update(float deltaTime);
	void	Render();

	void    SendPlayerSnapshot();
	void    SendCreateBulletMsg();

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