#pragma once
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Entity/EntityDefinition.hpp"

/*\class  : Mario	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0		   
* \date   : 2/24/2018 4:30:30 PM
* \contact: srsrakhil@gmail.com
*/
 
enum CharacterType
{
	SmallMario,SuperMario,FireSuperMario
};
enum Action
{
	IDLE,WALK,RUN,FIRE,JUMP
};
class Mario : public Entity
{

public:
	//Member_Variables
	int m_framesOnJump = 0;
	CharacterType m_currentCharacter = SmallMario;
	Action m_currentAction = IDLE;
	float  m_minVelocityForIdle = 0.5;
	float  m_movementForce = 100;
	float  m_jumpForce	   = 2000;
	std::string m_characterTypeString = "SmallMario";
	//Static_Member_Variables

	//Methods

	Mario();
	Mario(EntityDefinition *definition);
	~Mario();

	void Update(float deltaTime);
	void ProcessInput(float deltaTime);

	void WalkWest(float deltaTime);
	void WalkEast(float deltaTIme);
	void StayIdle();
	void Jump(float deltaTime);
	bool IsGrounded();
	void ShootFireBalls(float deltaTime);

	void UpgradeToLevel1Character();
	void UpgradeToLevel2Characte();
	
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
