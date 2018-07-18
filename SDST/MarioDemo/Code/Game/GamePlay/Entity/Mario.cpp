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
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Mario::Mario(EntityDefinition *definition) :Entity("mario")
{
	m_definition = definition;
	m_length = definition->m_length;
	m_height = definition->m_height;
	m_transform.Translate(Vector3(200, 200, 0));
	m_spriteAnimSet = new SpriteAnimSet(definition->m_spriteAnimSetDef);
	m_spriteAnimSet->SetAnimation(m_characterTypeString + "EastIdle");
	m_neuralNet = new NeuralNetwork(INPUT_NEURON_COUNT, HIDDEN_NEURON_COUNT, OUTPUT_NEURON_COUNT);
	InitMiniMap();

	StayIdle();
}

// DESTRUCTOR
Mario::~Mario()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::InitMiniMap()
{
	m_minimapAABB		 = AABB2(Vector2(0, 0), 50, 50);
	m_minimapLastPosAABB = AABB2(Vector2(0, 0), 50, 50);
	for (int indexY = 0; indexY < m_minimapHeight; indexY++)
	{
		for (int indexX = 0; indexX < m_minimapWidth; indexX++)
		{
			MiniMapObject obj;
			obj.m_color = Rgba::BLACK;
			obj.m_value = 0.f;
			m_minimapObjs.push_back(obj);
			m_minimapLastPos.push_back(obj);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::SetMiniMapValues(std::vector<MiniMapObject> &minimap, IntVector2 pos, Rgba color)
{
	int index = static_cast<int>(pos.y)*m_minimapHeight + static_cast<int>(pos.x);
	if (index < minimap.size())
	{
		minimap.at(index).m_color = color;
		if (color == Rgba::WHITE)
		{
			minimap.at(index).m_value = -0.5f;
		}
		else
			if (color == Rgba::RED)
			{
				minimap.at(index).m_value = -.75f;
			}
			else
				if (color == Rgba::GREEN)
				{
					minimap.at(index).m_value = 1.f;
				}
				else
				{
					minimap.at(index).m_value = 1;
				}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
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
*@purpose : NIL
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
	m_miniMapPosition.y += 100;
	m_minimapAABB = AABB2(m_miniMapPosition, m_minimapAABB.GetDimensions().x / 2.f, m_minimapAABB.GetDimensions().y / 2.f);
	m_minimapLastPosAABB = AABB2(m_miniMapPosition + Vector2(0, -150), m_minimapAABB.GetDimensions().x / 2.f, m_minimapAABB.GetDimensions().y / 2.f);

	// SQUARE CAN TAKE X OR Y
	Vector2 minBounds = marioWorldPosition - Vector2::ONE * (m_map->m_block.GetDimensions().x / 2.f) * (m_minimapWidth / 2.f);
	Vector2 maxBounds = marioWorldPosition + Vector2::ONE * (m_map->m_block.GetDimensions().x / 2.f) * (m_minimapHeight / 2.f);

	// MAKE ALL BLACK IN MINIMAP
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		m_minimapObjs.at(index).m_color = Rgba::CONSOLE_FADED_BLUE;
		m_minimapObjs.at(index).m_value = 0.0001f;
		/*if (IsGrounded(deltaTime))
		{
			m_minimapLastPos.at(index).m_color = Rgba::CONSOLE_FADED_BLUE;
			m_minimapLastPos.at(index).m_value = 0.0001f;
		}*/
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
				//DebugDraw::GetInstance()->DebugRenderLogf("RELATIVE %d,%d", relativeX, relativeY);
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
	if (m_transform.GetWorldPosition().y <= 115)
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
	if(m_sideVelocity > 0)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkEast");
	}
	if(m_sideVelocity < 0)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkWest");
	}

	/*if (IsGrounded(deltaTime) || IsJumping())
	{
		if(NNOutputs.at(1) > 0.5f)
		{
			Jump(deltaTime);

			//UpdateJump(deltaTime, NNOutputs.at(1));
			//if (m_currentJumpForce < m_maxJumpForce)
			//{
			//}
		}
	}*/

	//float xVelocity = ((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->m_velocity.x;
	//DebugRenderOptions option;
	//DebugDraw::GetInstance()->DebugRenderQuad2D(Vector3(300, 700, 0), AABB2(Vector2(0, 0), 300, 150), 0, nullptr, Rgba::BLACK, DEBUG_RENDER_FILL, option);
	//DebugDraw::GetInstance()->DebugRenderLogf("VELOCITY %f", xVelocity);
	/*if( xVelocity > m_minVelocityForIdle)
	{
		
		m_currentAction = WALK; 
	}
	else
	if (xVelocity < -m_minVelocityForIdle)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkWest");
		m_currentAction = WALK;
	}
	else
	{
		StayIdle();
	}*/
	//Vector3 prevTransform      = GetRigidBody3DComponent()->m_previousTransform.GetWorldPosition();
	//Vector3 marioWorldPosition = m_transform.GetWorldPosition();
	//DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f, %f, %f", marioWorldPosition.x, marioWorldPosition.y, marioWorldPosition.z);
	//DebugDraw::GetInstance()->DebugRenderLogf("Previous %f, %f, %f", prevTransform.x, prevTransform.y, prevTransform.z);
	//DebugDraw::GetInstance()->DebugRenderLogf("Diff %f, %f, %f"    , (marioWorldPosition.x - prevTransform.x), (marioWorldPosition.y - prevTransform.y), (marioWorldPosition.z - prevTransform.z));
	QueryAndDie(deltaTime);
	CheckForBounds();
	m_isPushed = false;
	m_isGrounded = false;
	if(m_lastSetPos.y < 227.f)
	{
		int a = 1;
	}
	Entity::Update(deltaTime);
	if(m_isPushed)
	{
		m_isGrounded = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateNN(float deltTime)
{
	std::vector<float> inputs;
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		inputs.push_back(m_minimapObjs.at(index).m_value);
	}
	m_neuralNet->FeedForward(inputs);

	std::vector<float> NNOutputs;
	for (int index = 0; index < m_neuralNet->m_outputs->m_neurons.size(); index++)
	{
		float value = m_neuralNet->m_outputs->m_neurons.at(index).m_value;
		NNOutputs.push_back(value);
	}
	if(g_controlMode)
	{
		return;
	}
	if(NNOutputs.at(0) > 0.5)
	{
		WalkEast(deltTime);
	}
	if(NNOutputs.at(1) > 0.5)
	{
		Jump(deltTime);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateGravity(float deltaTime)
{
	if (!IsGrounded(deltaTime))
	{
		Vector3 currentPosition = m_transform.GetWorldPosition();
		m_upVelocity		    += m_gravity;
		currentPosition		    += Vector3(0, m_upVelocity, 0);
		SetPosition(currentPosition);
		return;
	}
	m_upVelocity = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::UpdateVelocity(float deltaTime)
{
	if(GetAbsolute(m_sideVelocity) < 0.01)
	{
		return;
	}
	m_sideVelocity			= m_sideVelocity * 0.8f;
	Vector3 currentPosition = m_transform.GetWorldPosition();
	currentPosition			+= Vector3(m_sideVelocity, 0, 0);
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
	//m_transform.SetLocalPosition(positon);
	if (true)
	{
		//return;
	}
	/*<Brick actor = "Pit" position = "2235,40"  dimension = "30,30" hidden = "false" / >
	<Brick actor = "Pit" position = "2265,40"  dimension = "30,30" hidden = "false" / >

	<Brick actor = "Pit" position = "2775,40"  dimension = "30,30" hidden = "false" / >
	<Brick actor = "Pit" position = "2805,40"  dimension = "30,30" hidden = "false" / >
	<Brick actor = "Pit" position = "2835,40"  dimension = "30,30" hidden = "false" / >
	<Brick actor = "Pit" position = "4950,40"  dimension = "30,30" hidden = "false" / >
	<Brick actor = "Pit" position = "4980,40"  dimension = "30,30" hidden = "false" / >*/

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
	/*if (positon.x > 6048 && positon.x < 6080 && positon.y <= 368)
	{
		m_isGrounded = true;
		positon.y = 368;
	}*/




	if (positon.y <= 115)
	{
		m_isGrounded = true;
		positon.y = 115;
	}
	m_lastSetPos = positon;
	m_transform.SetLocalPosition(positon);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::ResetJump()
{
	m_currentJumpForce = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::ResetPosition()
{
	m_transform.SetLocalPosition(Vector3(200, 150, 0));
	m_numOfJumps = 0;
	m_isDead = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
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
*@purpose : NIL
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
	m_sideVelocity		    = -m_sideImpulse;
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
	m_sideVelocity		    = m_sideImpulse;
	
	//((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(m_movementForce, 0, 0), deltaTime);
	m_forwardVector = Vector2::EAST;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Makes character stay idle in the last moved direction
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
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
	m_currentAction = IDLE;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/25
*@purpose : Makes mario jump
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Mario::Jump(float deltaTime)
{
	if (IsGrounded(deltaTime))
	{
		m_numOfJumps++;
		m_upVelocity = m_upImpulseValue;

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
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Mario::IsGrounded(float deltaTime)
{
	/*Vector3 velocity = ((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->GetVelocity(deltaTime);
	velocity.x = 0;
	if(IsAlmostEqual(velocity,Vector3::ZERO,0.05))
	{
		return true;
	}*/
	//if(m_upVelocity)
	
	return m_isGrounded;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
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
		m_isCompleted = true;
		EndOfPlay();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::EndOfPlay()
{
	m_isDead       = true;
	double diff    = Clock::g_theMasterClock->total.m_seconds - m_lastDeadTime;
	m_lastDeadTime = Clock::g_theMasterClock->total.m_seconds;
	m_lastKnownPosition = Vector2::ZERO;
	m_timeElapsedAfterKnownLocation = 0.f;
	CalculateFitness();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::QueryAndDie(float deltaTime)
{
	if(g_controlMode)
	{
		return;
	}
	m_timeElapsedAfterKnownLocation += deltaTime;
	if (m_timeElapsedAfterKnownLocation > 2.f)
	{
		float distanceDiff = m_transform.GetWorldPosition().x - m_lastKnownPosition.x;
		m_lastKnownPosition = m_transform.GetWorldPosition().GetXY();
		if (distanceDiff < m_map->m_block.GetDimensions().x/2)
		{
			m_isCompleted = false;
			EndOfPlay();
			m_lastKnownPosition = Vector2::ZERO;
		}
		m_timeElapsedAfterKnownLocation = 0.f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::CalculateFitness()
{
	float xValue     = m_transform.GetWorldPosition().x;
	float jumpElement = 0.0225*static_cast<float>(m_numOfJumps);
	m_fitness = xValue * (1 - jumpElement*jumpElement);
	if(m_isCompleted)
	{
		//DebugDraw::GetInstance()->DebugRenderLogf(5,"FINISHEDDDD %f",m_fitness);
		m_fitness    += 1200;
		m_isCompleted = false;
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
		//DebugDraw::GetInstance()->DebugRenderLogf("")
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
			
			Vector3 diskPos					 = GetCircleCollider()->m_transform.GetWorldPosition();
			Vector3 distanceact				 = diskPos - Vector3(northWestCornerPosition,0);
			float   len						 = distanceact.GetLength();
			int a = 1;

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

			Vector3 diskPos					 = GetCircleCollider()->m_transform.GetWorldPosition();
			Vector3 distanceact				 = diskPos - Vector3(northEastCornerPosition, 0);
			float   len						 = distanceact.GetLength();
			int a = 1;
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

			Vector3 diskPos = GetCircleCollider()->m_transform.GetWorldPosition();
			Vector3 distanceact = diskPos - Vector3(southWestCornerPosition, 0);
			float   len = distanceact.GetLength();
			int a = 1;
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

			Vector3 diskPos = GetCircleCollider()->m_transform.GetWorldPosition();
			Vector3 distanceact = diskPos - Vector3(southEastCornerPosition, 0);
			float   len = distanceact.GetLength();
			int a = 1;
		}

	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Renders mario
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Mario::Render()
{
	if (m_isDead)
	{
		return;
	}
	//RenderMiniMap(m_minimapAABB, m_minimapObjs);
	//RenderMiniMap(m_minimapLastPosAABB, m_minimapLastPos);
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::RenderMiniMap(AABB2 minmapAABB, std::vector<MiniMapObject>& minimap)
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Vector2 marioPosition = m_transform.GetWorldPosition().GetXY();
	//Renderer::GetInstance()->DrawAABB(m_minimapAABB,Rgba::BLACK);
	Renderer::GetInstance()->DrawRectangle(minmapAABB);

	Vector2 minPosition		     = minmapAABB.mins;
	Vector2 marioMiniMapPosition = minmapAABB.GetCenter();
	for(int indexY = 0;indexY < m_minimapHeight;indexY++)
	{
		for(int indexX = 0;indexX < m_minimapWidth;indexX++)
		{
			int index = indexY * m_minimapHeight + indexX;
			Vector2 currentPos(indexX * 10, indexY * 10);
			Rgba color = minimap.at(index).m_color;
			if(color == Rgba::BLACK)
			{
				continue;
			}
			AABB2 miniMapobj(minPosition + currentPos,5,5);
			Renderer::GetInstance()->DrawAABB(miniMapobj,color);// , color);
		}
	}


	/*for (int brickIndex = 0; brickIndex < m_bricks.size(); brickIndex++)
	{
	Vector2 brickPosition = m_bricks.at(brickIndex)->m_transform.GetWorldPosition().GetXY();
	if (m_cameraQuads.IsPointInside(brickPosition))
	{
	Vector2 relativePosition =  brickPosition - marioPosition;
	relativePosition = relativePosition * m_miniMapScaleFactor;
	Vector2 absolutePosition = m_miniMapPosition + relativePosition;
	AABB2 brickMiniMap(absolutePosition, 10, 10);
	Renderer::GetInstance()->DrawAABB(brickMiniMap, Rgba::GREEN);
	}
	}*/
	delete defaultMaterial;
}
