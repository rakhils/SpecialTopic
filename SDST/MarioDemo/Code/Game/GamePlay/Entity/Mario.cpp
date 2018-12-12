#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Core/Component.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Physics/Collider/CircleCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"
#include "Engine/Time/Clock.hpp"
#include "Game/GamePlay/Entity/Mario.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/Brick.hpp"


// CONSTRUCTOR
Mario::Mario():Entity("mario")
{
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Constructs with def
*@param   : Entity defenition
*@return  : NIL
*//////////////////////////////////////////////////////////////
Mario::Mario(EntityDefinition *definition) :Entity("mario")
{
	m_definition = definition;
	m_length	 = definition->m_length;
	m_height	 = definition->m_height;
	m_transform.Translate(g_startPosition);
	m_spriteAnimSet = new SpriteAnimSet(definition->m_spriteAnimSetDef);
	m_spriteAnimSet->SetAnimation(m_characterTypeString + "EastIdle");
	m_neuralNet = new NeuralNetwork(INPUT_NEURON_COUNT, HIDDEN_NEURON_COUNT, OUTPUT_NEURON_COUNT);
	m_neuralNet->LoadFromFile(g_neuralNetFilePath.c_str());
	InitMiniMap();

	StayIdle();
}

// DESTRUCTOR
Mario::~Mario()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Initlize minimap with MinimapObjects values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::InitMiniMap()
{
	m_minimapAABB		 = AABB2(Vector2(0, 0), g_minimapWidth, g_minimapWidth);
	m_minimapLastPosAABB = AABB2(Vector2(0, 0), g_minimapWidth, g_minimapWidth);
	for (int indexY = 0; indexY < m_minimapPixelHeight; indexY++)
	{
		for (int indexX = 0; indexX < m_minimapPixelWidth; indexX++)
		{
			MiniMapObj obj;
			obj.m_color = Rgba::BLACK;
			obj.m_value = 0.f;
			m_minimapObjs.push_back(obj);
			m_minimapLastPos.push_back(obj);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Sets minimap object values according to color
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::SetMiniMapValues(std::vector<MiniMapObj> &minimap, IntVector2 pos, Rgba color)
{
	int index = static_cast<int>(pos.y)*m_minimapPixelHeight + static_cast<int>(pos.x);
	if (index < minimap.size())
	{
		minimap.at(index).m_color = color;
		if (color == Rgba::WHITE)
		{
			minimap.at(index).m_value = g_minmapBrickValue;
		}
		else
		if (color == Rgba::RED)
		{
			minimap.at(index).m_value = g_minmapPitValue;
		}
		else
		if (color == Rgba::GREEN)
		{
			minimap.at(index).m_value = g_minmapMarioValue;
		}
		else
		{
			minimap.at(index).m_value = .15f;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Retrieves minimap values as vector
*@param   : NIL
*@return  : vector of minimap values
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<float>& Mario::GetInputsFromMiniMap()
{
	std::vector<float> inputs;
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		inputs.push_back(m_minimapObjs.at(index).m_value);
	}
	return inputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Updates minimap values with current mario position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateMiniMap(float deltaTime)
{
	float width = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);

	Vector2 marioWorldPosition = m_transform.GetWorldPosition().GetXY();
	m_miniMapPosition.x = m_map->m_camera->m_transform.GetWorldPosition().x + width / 2 - m_minimapAABB.GetDimensions().x * 2;
	m_miniMapPosition.y = m_map->m_camera->m_transform.GetWorldPosition().y + height / 2 - m_minimapAABB.GetDimensions().y * 2;
	m_miniMapPosition.y += 50;
	m_miniMapPosition.x -= 100;
	m_minimapAABB = AABB2(m_miniMapPosition, m_minimapAABB.GetDimensions().x / 2.f, m_minimapAABB.GetDimensions().y / 2.f);
	m_minimapLastPosAABB = AABB2(m_miniMapPosition + Vector2(0, -150), m_minimapAABB.GetDimensions().x / 2.f, m_minimapAABB.GetDimensions().y / 2.f);

	// SQUARE CAN TAKE X OR Y
	Vector2 minBounds = marioWorldPosition - Vector2::ONE * (m_map->m_block.GetDimensions().x / 2.f) * (m_minimapPixelWidth / 2.f);
	Vector2 maxBounds = marioWorldPosition + Vector2::ONE * (m_map->m_block.GetDimensions().x / 2.f) * (m_minimapPixelHeight / 2.f);

	// MAKE ALL BLACK IN MINIMAP
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		m_minimapObjs.at(index).m_color = Rgba::CONSOLE_FADED_BLUE;
		m_minimapObjs.at(index).m_value = 0.0001f;
	}

	for (int index = 0; index < m_map->m_bricks.size(); index++)
	{
		Vector2 brickPosition = m_map->m_bricks.at(index)->m_transform.GetWorldPosition().GetXY();
		if (brickPosition.x > minBounds.x && brickPosition.y > minBounds.y)
		{
			int relativeX = RoundToNearestInt((brickPosition.x - minBounds.x) / (m_map->m_block.GetDimensions().x / 2.f));
			int relativeY = RoundToNearestInt((brickPosition.y - minBounds.y) / (m_map->m_block.GetDimensions().x / 2.f));
			relativeY++;
			if (brickPosition.x < maxBounds.x && brickPosition.y < maxBounds.y)
			{
				SetMiniMapValues(m_minimapObjs, IntVector2(relativeX, relativeY), Rgba::WHITE);
				if (IsGrounded(deltaTime))
				{
					SetMiniMapValues(m_minimapLastPos, IntVector2(relativeX, relativeY), Rgba::WHITE);
				}
			}
		}
	}
	for (int index = 0; index < m_map->m_pits.size(); index++)
	{
		Vector2 pitPosition = m_map->m_pits.at(index).m_position;
		if (pitPosition.x > minBounds.x && pitPosition.y > minBounds.y)
		{
			int relativeX = RoundToNearestInt((pitPosition.x - minBounds.x) / (m_map->m_block.GetDimensions().x / 2.f));
			int relativeY = RoundToNearestInt((pitPosition.y - minBounds.y) / (m_map->m_block.GetDimensions().x / 2.f));
			relativeY++;
			if (pitPosition.x < maxBounds.x && pitPosition.y < maxBounds.y)
			{
				SetMiniMapValues(m_minimapObjs, IntVector2(relativeX, relativeY), m_map->m_pits.at(index).m_color);
				if (IsGrounded(deltaTime))
				{
					SetMiniMapValues(m_minimapLastPos, IntVector2(relativeX, relativeY), m_map->m_pits.at(index).m_color);
				}
				//DebugDraw::GetInstance()->DebugRenderLogf("RELATIVE PIT %d,%d", relativeX, relativeY);
			}
		}
	}
	int positionX = static_cast<int>((marioWorldPosition.x - minBounds.x) / (m_map->m_block.GetDimensions().x / 2.f));
	int positionY = static_cast<int>((marioWorldPosition.y - minBounds.y) / (m_map->m_block.GetDimensions().x / 2.f));
	//DebugDraw::GetInstance()->DebugRenderLogf("RELATIVE MARIO %d,%d", positionX, positionY);
	SetMiniMapValues(m_minimapObjs, IntVector2(positionX, positionY), Rgba::GREEN);

	if (IsGrounded(deltaTime))
	{
		SetMiniMapValues(m_minimapLastPos, IntVector2(positionX, positionY), Rgba::GREEN);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Update mario character 
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::Update(float deltaTime)
{
	if(m_isDead)
	{
		return;
	}
	if (m_transform.GetWorldPosition().y <= g_minYValueForDeath) // not on floor
	{
		Vector3 position = m_transform.GetWorldPosition();
		SetPosition(position);
	}
	UpdateMiniMap(deltaTime);
	UpdateNN(deltaTime);
	UpdateGravity(deltaTime);
	UpdateVelocity(deltaTime);

	if(g_theInput->isKeyPressed(InputSystem::KEYBOARD_LEFT_ARROW))
	{
		WalkWest(deltaTime);
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_1))
	{
		m_sideImpulse = 25.f;
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_2))
	{
		m_sideImpulse = 5.f;
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_RIGHT_ARROW))
	{
		WalkEast(deltaTime);
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_UP_ARROW))
	{
		Jump(deltaTime);
	}
	if(m_horizontalVelocity > 0)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkEast");
	}
	if(m_horizontalVelocity < 0)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkWest");
	}

	QueryAndDie(deltaTime);
	CheckForBounds();
	m_isPushed = false;
	m_isGrounded = false;
	Entity::Update(deltaTime);
	if(m_isPushed)
	{
		m_isGrounded = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : Updates neural net inputs with minimap values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateNN(float deltaTime)
{
	std::vector<double> inputs;
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		inputs.push_back(static_cast<double>(m_minimapObjs.at(index).m_value));
	}
	m_neuralNet->FeedForward(inputs);
	UpdateAIBotInputs(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/10
*@purpose : Updates controls using Neural net values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateAIBotInputs(float deltaTime)
{
	//if (true) return;
	std::vector<float> NNOutputs;
	for (int index = 0; index < m_neuralNet->m_outputs->m_neurons.size(); index++)
	{
		float value = m_neuralNet->m_outputs->m_neurons.at(index).m_value;
		NNOutputs.push_back(value);
	}
	if (NNOutputs.at(0) > g_minNeuralNetValueForWalk)
	{
		WalkEast(deltaTime);
	}
	if (NNOutputs.at(1) > g_minNeuralNetValueForJump)
	{
		Jump(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : Updates gravity push when not grounded
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateGravity(float deltaTime)
{
	if (!IsGrounded(deltaTime))
	{
		Vector3 currentPosition = m_transform.GetWorldPosition();
		m_upVelocity		    += m_gravity * deltaTime;
		currentPosition		    += Vector3(0, m_upVelocity, 0);
		SetPosition(currentPosition);
		return;
	}
	m_upVelocity = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : Update horizontal velocity according to 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateVelocity(float deltaTime)
{
	if(GetAbsolute(m_horizontalVelocity) < 0.01)
	{
		return;
	}
	m_horizontalVelocity			= m_horizontalVelocity * 0.8f * deltaTime;
	Vector3 currentPosition			= m_transform.GetWorldPosition();
	currentPosition					+= Vector3(m_horizontalVelocity, 0, 0);
	m_transform.SetLocalPosition(currentPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::SetPosition(Vector3 positon)
{
	if(positon.x > 2220 && positon.x < 2280 && positon.y <= 115)
	{
		EndOfPlay();
		return;
	}
	if(positon.x > 2760 && positon.x < 2850 && positon.y <= 115)
	{
		EndOfPlay();
		return;
	}
	if(positon.x > 4935 && positon.x < 4995 && positon.y <= 115)
	{
		EndOfPlay();
		return;
	}
	if(positon.x > 4325 && positon.x < 4357 && positon.y <= 144)
	{
		m_isGrounded = true;
		positon.y = 144;
	}
	if (positon.x > 4357 && positon.x < 4389 && positon.y <= 176)
	{
		m_isGrounded = true;
		positon.y = 176;
	}
	if (positon.x > 4389 && positon.x < 4421 && positon.y <= 208)
	{
		m_isGrounded = true;
		positon.y = 208;
	}

	//////4790
	if (positon.x > 4774 && positon.x < 4806 && positon.y <= 144)
	{
		m_isGrounded = true;
		positon.y = 144;
	}
	if (positon.x > 4806 && positon.x < 4838 && positon.y <= 176)
	{
		m_isGrounded = true;
		positon.y = 176;
	}
	if (positon.x > 4838 && positon.x < 4870 && positon.y <= 208)
	{
		m_isGrounded = true;
		positon.y = 208;
	}
	///////
	if (positon.x > 5824 && positon.x < 5856 && positon.y <= 144)
	{
		m_isGrounded = true;
		positon.y = 144;
	}
	if (positon.x > 5856 && positon.x < 5888 && positon.y <= 176)
	{
		m_isGrounded = true;
		positon.y = 176;
	}
	if (positon.x > 5888 && positon.x < 5920 && positon.y <= 208)
	{
		m_isGrounded = true;
		positon.y = 208;
	}
	if (positon.x > 5920 && positon.x < 5952 && positon.y <= 240)
	{
		m_isGrounded = true;
		positon.y = 240;
	}
	if (positon.x > 5952 && positon.x < 5984 && positon.y <= 272)
	{
		m_isGrounded = true;
		positon.y = 272;
	}
	if (positon.x > 5984 && positon.x < 6016 && positon.y <= 304)
	{
		m_isGrounded = true;
		positon.y = 304;
	}
	if (positon.x > 6018 && positon.x < 6050 && positon.y <= 336)
	{
		m_isGrounded = true;
		positon.y = 336;
	}
	
	if (positon.y <= g_minYValueForDeath)
	{
		m_isGrounded = true;
		positon.y = g_minYValueForDeath;
	}
	m_transform.SetLocalPosition(positon);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : Reset jump values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::ResetJump()
{
	m_currentJumpForce = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : Resets position (start from first)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::ResetPosition()
{
	m_transform.SetLocalPosition(g_startPosition);
	m_numOfJumps = 0;
	m_isDead = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : Checks if mario is jumping
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Mario::IsJumping()
{
	if(m_currentJumpForce > 0 && m_currentJumpForce < m_maxJumpForce)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateJump(float deltaTime,float force)
{
	m_currentJumpForce += deltaTime;
	if(m_currentJumpForce > m_maxJumpForce)
	{
		ResetJump();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/25
*@purpose : Resets all neural net weights
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::ResetWeight()
{
	m_neuralNet->SetRandomWeight();
}

/*DATE    : 2018/02/25/////////////////////////////////////////
*@purpose : Make character walk west by a tile
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::WalkWest(float deltaTime)
{
	m_horizontalVelocity		    = -m_sideImpulse;
	//((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(-m_movementForce, 0, 0), deltaTime);
	m_forwardVector = Vector2::WEST;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Make character walk east by a tile
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::WalkEast(float deltaTime)
{
	m_horizontalVelocity		    = m_sideImpulse;
	
	//((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(m_movementForce, 0, 0), deltaTime);
	m_forwardVector = Vector2::EAST;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Makes character stay idle in the last moved direction
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::StayIdle()
{
	if(m_forwardVector == Vector2::EAST)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "EastIdle");
	}
	if (m_forwardVector == Vector2::WEST)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WestIdle");
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Makes mario jump
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::Jump(float deltaTime)
{
	if (IsGrounded(deltaTime))
	{
		m_numOfJumps++;
		m_upVelocity = m_upImpulseValue * deltaTime;

		Vector3 currentPosition = m_transform.GetWorldPosition();
		m_upVelocity += m_gravity;
		currentPosition += Vector3(0, m_upVelocity, 0);
		m_transform.SetLocalPosition(currentPosition);
		m_isGrounded = false;
		m_isJumping = true;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/28
*@purpose : Checks if mario is on ground
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
bool Mario::IsGrounded(float deltaTime)
{
	return m_isGrounded;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Check whole map east and west bounds
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::CheckForBounds()
{
	if (m_transform.GetWorldPosition().y < 0)
	{
		EndOfPlay();
	}
	if (m_transform.GetWorldPosition().x > 6700)
	{
		m_isLevelCompleted = true;
		EndOfPlay();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Ends the play calculates total fitness
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::EndOfPlay()
{
	m_isDead       = true;
	double diff    = Clock::g_theMasterClock->total.m_seconds - m_lastDeadTime;
	m_lastDeadTime = Clock::g_theMasterClock->total.m_seconds;
	m_lastKnownPosition = Vector2::ZERO;
	m_timeElapsedInLastKnownLocation = 0.f;
	CalculateFitness();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Query and check if death conditions are satisfied
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::QueryAndDie(float deltaTime)
{
	m_timeElapsedInLastKnownLocation += deltaTime;
	if (m_timeElapsedInLastKnownLocation > 2.f)
	{
		float distanceDiff = m_transform.GetWorldPosition().x - m_lastKnownPosition.x;
		m_lastKnownPosition = m_transform.GetWorldPosition().GetXY();
		if (distanceDiff < m_map->m_block.GetDimensions().x/2)
		{
			m_isLevelCompleted = false;
			EndOfPlay();
			m_lastKnownPosition = Vector2::ZERO;
		}
		m_timeElapsedInLastKnownLocation = 0.f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : Calculates fitness values for this mario
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::CalculateFitness()
{
	float xValue			 = m_transform.GetWorldPosition().x;
	float fitnessJumpScore   = g_fitnessJumpFactor*static_cast<float>(m_numOfJumps);
	m_fitness				 = xValue * (1 - fitnessJumpScore*fitnessJumpScore);
	if(m_isLevelCompleted)
	{
		m_fitness    += g_fitnessBonusScore;
		m_isLevelCompleted = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::OnCollisionEnter(Collider *collider)
{
	if(collider->m_colliderType == BOX_COLLIDER2D)
	{
		m_isPushed = false;
		BoxCollider2D *boxcollider = (BoxCollider2D*)collider;
		Vector3 position = m_transform.GetWorldPosition();
		float   radius   = GetCircleCollider()->m_disc.radius;
		Vector2 circleColliderPosition = GetCircleCollider()->m_transform.GetWorldPosition().GetXY();
		if (boxcollider->m_colliderDirection == NORTH)// || boxcollider->m_colliderDirection == CENTRE)
		{
			m_isPushed = true;
			float distanceToPush = (boxcollider->m_aabb2.maxs.y - (circleColliderPosition.y - radius));
			m_transform.SetLocalPosition(Vector3(position.x, position.y + distanceToPush, 0));
		}
		if(boxcollider->m_colliderDirection == SOUTH)
		{
			m_upVelocity = 0;
			circleColliderPosition = GetCircleCollider()->m_disc.center;
			float distanceToPush = ((circleColliderPosition.y + radius) - boxcollider->m_aabb2.mins.y); 
			m_transform.SetLocalPosition(Vector3(position.x, position.y - distanceToPush, 0));
		}
		
		if (boxcollider->m_colliderDirection == EAST)
		{
			circleColliderPosition = GetCircleCollider()->m_disc.center;
			float distanceToPush   = (boxcollider->m_aabb2.maxs.x - (circleColliderPosition.x - radius));
			m_transform.SetLocalPosition(Vector3(position.x + distanceToPush, position.y, 0));
		}
		if (boxcollider->m_colliderDirection == WEST)
		{
			circleColliderPosition = GetCircleCollider()->m_disc.center;
			float distanceToPush   = ((circleColliderPosition.x + radius) - boxcollider->m_aabb2.mins.x);
			m_transform.SetLocalPosition(Vector3(position.x - distanceToPush , position.y, 0));
		}
		if (boxcollider->m_colliderDirection == CENTRE)
		{
			/*circleColliderPosition = GetCircleCollider()->m_disc.center;
			if(circleColliderPosition.y  > boxcollider->m_aabb2.GetCenter().y)
			{
				float distanceToPush = (boxcollider->m_aabb2.maxs.y - (circleColliderPosition.y - radius));
				m_transform.SetLocalPosition(Vector3(position.x, position.y + distanceToPush, 0));
			}*/

		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (boxcollider->m_colliderDirection == NORTH_WEST)
		{
			circleColliderPosition		     = GetCircleCollider()->m_disc.center;
			Vector2 northWestCornerPosition(boxcollider->m_aabb2.mins.x, boxcollider->m_aabb2.maxs.y);
			Vector2 distance				 = circleColliderPosition - northWestCornerPosition;
			Vector2 pushDirection			 = distance.GetNormalized();
			Vector2 requiredDistance		 = pushDirection*radius + northWestCornerPosition;

			Vector2 translatedDistance		 = requiredDistance - circleColliderPosition;
			m_transform.Translate(translatedDistance);
			
			//Vector3 diskPos					 = GetCircleCollider()->m_transform.GetWorldPosition();
			//Vector3 distanceact				 = diskPos - Vector3(northWestCornerPosition,0);
			//float   len						 = distanceact.GetLength();

		}
		if (boxcollider->m_colliderDirection == NORTH_EAST)
		{
			circleColliderPosition			 = GetCircleCollider()->m_disc.center;
			Vector2 northEastCornerPosition(boxcollider->m_aabb2.maxs.x, boxcollider->m_aabb2.maxs.y);
			Vector2 distance				 = circleColliderPosition - northEastCornerPosition;
			Vector2 pushDirection			 = distance.GetNormalized();
			Vector2 requiredDistance		 = pushDirection*radius + northEastCornerPosition;

			Vector2 translatedDistance		 = requiredDistance - circleColliderPosition;
			m_transform.Translate(translatedDistance);

			//Vector3 diskPos					 = GetCircleCollider()->m_transform.GetWorldPosition();
			//Vector3 distanceact				 = diskPos - Vector3(northEastCornerPosition, 0);
			//float   len						 = distanceact.GetLength();
		}
		if (boxcollider->m_colliderDirection == SOUTH_WEST)
		{
			m_upVelocity = 0;

			circleColliderPosition = GetCircleCollider()->m_disc.center;
			Vector2 southWestCornerPosition(boxcollider->m_aabb2.mins.x, boxcollider->m_aabb2.mins.y);
			Vector2 distance = circleColliderPosition - southWestCornerPosition;
			Vector2 pushDirection = distance.GetNormalized();
			Vector2 requiredDistance = pushDirection * radius + southWestCornerPosition;

			Vector2 translatedDistance = requiredDistance - circleColliderPosition;
			m_transform.Translate(translatedDistance);

			//Vector3 diskPos = GetCircleCollider()->m_transform.GetWorldPosition();
			//Vector3 distanceact = diskPos - Vector3(southWestCornerPosition, 0);
			//float   len = distanceact.GetLength();
		}
		if (boxcollider->m_colliderDirection == SOUTH_EAST)
		{
			m_upVelocity = 0;
			circleColliderPosition   = GetCircleCollider()->m_disc.center;
			Vector2 southEastCornerPosition(boxcollider->m_aabb2.maxs.x, boxcollider->m_aabb2.mins.y);
			Vector2 distance		  = circleColliderPosition - southEastCornerPosition;
			Vector2 pushDirection     = distance.GetNormalized();
			Vector2 requiredDistance  = pushDirection * radius + southEastCornerPosition;

			Vector2 translatedDistance = requiredDistance - circleColliderPosition;
			m_transform.Translate(translatedDistance);

			//Vector3 diskPos = GetCircleCollider()->m_transform.GetWorldPosition();
			//Vector3 distanceact = diskPos - Vector3(southEastCornerPosition, 0);
			//float   len = distanceact.GetLength();
		}

	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Renders mario
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
void Mario::Render()
{
	if (m_isDead)
	{
		return;
	}
	Material *defaultMaterial = Material::AquireResource("default");
	m_texture			      = Texture::CreateOrGetTexture("Data\\Images\\player.png",true,true);
	defaultMaterial->m_textures.at(0) = m_texture;
	Texture::SetCurrentTexture(m_texture);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Entity::Render();
	if (m_map->m_bestMode)
	{
		RenderMiniMap(m_minimapAABB, m_minimapObjs);
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : Renders minimap when not training
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::RenderMiniMap(AABB2 minmapAABB, std::vector<MiniMapObj>& minimap)
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Vector2 marioPosition = m_transform.GetWorldPosition().GetXY();
	Renderer::GetInstance()->DrawRectangle(minmapAABB);

	Vector2 minPosition		     = minmapAABB.mins;
	Vector2 marioMiniMapPosition = minmapAABB.GetCenter();
	for(int indexY = 0;indexY < m_minimapPixelHeight;indexY++)
	{
		for(int indexX = 0;indexX < m_minimapPixelWidth;indexX++)
		{
			int index = indexY * m_minimapPixelHeight + indexX;
			Vector2 currentPos(indexX * 10, indexY * 10);
			Rgba color = minimap.at(index).m_color;
			if(color == Rgba::BLACK)
			{
				continue;
			}
			AABB2 miniMapobj(minPosition + currentPos,5,5);
			Renderer::GetInstance()->DrawAABB(miniMapobj,color);
		}
	}
	delete defaultMaterial;
}
