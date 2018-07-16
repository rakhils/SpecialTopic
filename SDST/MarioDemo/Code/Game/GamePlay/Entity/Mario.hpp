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
struct MiniMapObject
{
	Rgba  m_color;
	float m_value;
};
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
class Map;
class Mario : public Entity
{

public:
	//Member_Variables
	Map*					    m_map						= nullptr;
	int							m_numOfJumps				= 0;
	float						m_fitness					= 0.f;
	Vector2						m_lastKnownPosition;
	float						m_timeElapsedAfterKnownLocation = 0.f;
	int							m_framesOnJump				= 0;
	CharacterType				m_currentCharacter			= SmallMario;
	Action						m_currentAction				= IDLE;
	DeathType					m_deathType;
	bool						m_hasReachedMaxHeight		= false;
	float						m_maxJumpForce				= 0.3f;
	float						m_currentJumpForce			= 0;
	float						m_minVelocityForIdle		= 0.5;
	float						m_movementForce				= 1.5;
	float						m_jumpForce					= 3;
	std::string					m_characterTypeString		= "SmallMario";
	NeuralNetwork				*m_neuralNet = nullptr;
	Vector2						m_miniMapPosition;
	AABB2					    m_minimapAABB;
	std::vector<MiniMapObject>  m_minimapObjs;
	AABB2					    m_minimapLastPosAABB;
	std::vector<MiniMapObject>  m_minimapLastPos;
	int							m_minimapWidth  = 10;
	int							m_minimapHeight = 10;
	bool					    m_isDead					= false;
	bool						m_isJumping					= false;
	float						m_maxJumpFrames				= 0.5f;
	float						m_jumpTime					= 0.f;
	double						m_lastDeadTime = 0.0;

	bool						m_isGrounded = false;
	bool						m_isPushed	 = false;
	float						m_upVelocity;
	float						m_sideVelocity	= 0.f;
	float						m_sideImpulse = 6.f;
	float						m_friction = 0.f;
	float						m_upImpulseValue = 9.5f;
	float						m_gravity        = -0.35;
	Vector3						m_lastSetPos;
	bool						m_isCompleted    = false;
	//Static_Member_Variables

	//Methods

	Mario();
	Mario(EntityDefinition *definition);
	~Mario();
	//MINIMAP
	void				InitMiniMap();
	void				SetMiniMapValues(std::vector<MiniMapObject> &m_minimap, IntVector2 pos, Rgba color);
	std::vector<float>& GetInputsFromMiniMap();
	void				UpdateMiniMap(float deltaTime);
	//
	void				Update(float deltaTime);
	void				UpdateNN(float deltaTime);
	void				UpdateGravity(float deltaTime);
	void				UpdateVelocity(float deltaTime);
	void				SetPosition(Vector3 positon);
	void				ResetJump();
	void				ResetPosition();
	bool				IsJumping();
	void				UpdateJump(float deltaTime, float force);
	void				ProcessInput(float deltaTime);
	void				ResetWeight();
	void				WalkWest(float deltaTime);
	void				WalkEast(float deltaTIme);
	void				StayIdle();
	void				Jump(float deltaTime);
	bool				IsGrounded(float deltaTime);
	void				CheckForBounds();
	void				EndOfPlay();
	void				QueryAndDie(float deltaTime);
	void				CalculateFitness();

	void				OnCollisionEnter(Collider *collider);

	void				Render();
	void				RenderMiniMap(AABB2 aabbPos, std::vector<MiniMapObject>& minimap);

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
