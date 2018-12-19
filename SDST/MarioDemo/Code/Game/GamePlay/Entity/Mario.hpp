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

struct MiniMapObj
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
	Map*					    m_map								= nullptr;
	int							m_numOfJumps						= 0;
	float						m_fitness							= 0.f;
	Vector2						m_lastKnownPosition;
	float						m_timeElapsedInLastKnownLocation    = 0.f;
	DeathType					m_deathType;
	float						m_maxJumpForce						= 0.1f;
	float						m_currentJumpForce					= 0;
	std::string					m_characterTypeString				= "SmallMario";
	NeuralNetwork				*m_neuralNet						= nullptr;
	Vector2						m_miniMapPosition;
	AABB2					    m_minimapAABB;
	std::vector<MiniMapObj>   m_minimapObjs;
	AABB2					    m_minimapLastPosAABB;
	std::vector<MiniMapObj>   m_minimapLastPos;
	int							m_minimapPixelWidth						= 10;
	int							m_minimapPixelHeight						= 10;
	bool					    m_isDead							= false;
	bool						m_isJumping							= false;
	float						m_maxJumpFrames						= 0.5f;
	float						m_jumpTime							= 0.f;
	double						m_lastDeadTime						= 0.0;

	bool						m_isGrounded = false;
	bool						m_isPushed	 = false;
	float						m_upVelocity;
	float						m_horizontalVelocity						= 0.f;
	float						m_sideImpulse						= 305.f;
	float						m_upImpulseValue					= 1000.f;
	float						m_gravity							= -9.8f;
	//Vector3						m_lastKnownPosition;
	bool						m_isLevelCompleted					= false;
	//Static_Member_Variables

	//Methods

	Mario();
	Mario(EntityDefinition *definition);
	~Mario();
	//MINIMAP
	void				InitMiniMap();
	void				SetMiniMapValues(std::vector<MiniMapObj> &m_minimap, IntVector2 pos, Rgba color);
	std::vector<float>& GetInputsFromMiniMap();
	void				UpdateMiniMap(float deltaTime);
	//
	void				Update(float deltaTime);
	void				UpdateNN(float deltaTime);
	void				UpdateAIBotInputs(float deltaTime);
	void				UpdateGravity(float deltaTime);
	void				UpdateVelocity(float deltaTime);
	void				SetPosition(Vector3 positon);
	void				ResetJump();
	void				ResetPosition();
	bool				IsJumping();
	void				UpdateJump(float deltaTime, float force);
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
	void				RenderMiniMap(AABB2 aabbPos, std::vector<MiniMapObj>& minimap);

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
