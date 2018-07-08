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
enum DeathType
{
	NONE,DEATH_PIT,DEATH_ENEMY,DEATH_IDLE
};
class NeuralNetwork;
class Mario : public Entity
{

public:
	//Member_Variables
	int					m_framesOnJump				= 0;
	CharacterType		m_currentCharacter			= SmallMario;
	Action				m_currentAction				= IDLE;
	DeathType			m_deathType;
	bool				m_hasReachedMaxHeight		= false;
	float				m_maxJumpForce				= 0.3f;
	float				m_currentJumpForce			= 0;
	float				m_minVelocityForIdle		= 0.5;
	float				m_movementForce				= 15000;
	float				m_jumpForce					= 40000;
	std::string			m_characterTypeString		= "SmallMario";
	NeuralNetwork	    *m_neuralNet = nullptr;

	//Static_Member_Variables

	//Methods

	Mario();
	Mario(EntityDefinition *definition);
	~Mario();

	void Update(float deltaTime);
	void ResetJump();
	bool IsJumping();
	void UpdateJump(float deltaTime,float force);
	void ProcessInput(float deltaTime);
	void ResetWeight();
	void WalkWest(float deltaTime);
	void WalkEast(float deltaTIme);
	void Walk(float deltaTime, float force);
	void StayIdle();
	void Jump(float deltaTime);
	bool IsGrounded(float deltaTime);
	void ShootFireBalls(float deltaTime);

	void UpgradeToLevel1Character();
	void UpgradeToLevel2Characte();

	void OnCollisionEnter(Collider *collider);
	
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
