#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Core/Component.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Physics/Collider/CircleCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"
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
			minimap.at(index).m_value = 25;
		}
		else
			if (color == Rgba::RED)
			{
				minimap.at(index).m_value = 25;
			}
			else
				if (color == Rgba::GREEN)
				{
					minimap.at(index).m_value = 50;
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
		m_minimapObjs.at(index).m_value = 0.75f;
		if (IsGrounded(deltaTime))
		{
			m_minimapLastPos.at(index).m_color = Rgba::CONSOLE_FADED_BLUE;
			m_minimapLastPos.at(index).m_value = 0.75f;
		}
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
	std::vector<float> inputs;
	for (int index = 0; index < m_minimapObjs.size(); index++)
	{
		inputs.push_back(m_minimapObjs.at(index).m_value);
	}
	m_neuralNet->FeedForward(inputs);

	Disc2 colliderOutline = GetCircleCollider()->m_disc;
	std::vector<float> NNOutputs;
	for (int index = 0; index < m_neuralNet->m_outputs->m_neurons.size(); index++)
	{
		float value = m_neuralNet->m_outputs->m_neurons.at(index).m_value;
		NNOutputs.push_back(value);
	}
	
	//DebugDraw::GetInstance()->DebugRenderLogf("NN OUTPUT %f, %f, %f", NNOutputs.at(0), NNOutputs.at(1));

	float random = GetRandomIntInRange(0, 3);
	//random = -1;
	if(NNOutputs.at(0) > 0.5f)
	{
		Walk(deltaTime, NNOutputs.at(0));
	}
	if (NNOutputs.at(1) > 0.5f)
	{
		//Walk(deltaTime, -NNOutputs.at(1));
	}
	if(g_theInput->isKeyPressed(InputSystem::KEYBOARD_LEFT_ARROW))
	{
		WalkWest(deltaTime);
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_RIGHT_ARROW))
	{
		WalkEast(deltaTime);
	}
	if (g_theInput->wasKeyJustReleased(InputSystem::KEYBOARD_UP_ARROW))
	{
		ResetJump();
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_UP_ARROW) || NNOutputs.at(1) > 0.5f)
	{
		if(IsGrounded(deltaTime) && m_jumpTime == m_maxJumpFrames)
		{
			m_isJumping = true;
		}
		/*if(IsGrounded(deltaTime) || IsJumping())
		{
			UpdateJump(deltaTime,1);
			if (m_currentJumpForce < m_maxJumpForce)
			{
				Jump(deltaTime);
			}
		}*/
	}
	if(m_isJumping)
	{
		Jump(deltaTime);
		m_jumpTime--;
		if(m_jumpTime <=0)
		{
			m_isJumping = false;
			m_jumpTime = m_maxJumpFrames;
		}
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

	float xVelocity = ((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->m_velocity.x;
	DebugRenderOptions option;
	//DebugDraw::GetInstance()->DebugRenderQuad2D(Vector3(300, 700, 0), AABB2(Vector2(0, 0), 300, 150), 0, nullptr, Rgba::BLACK, DEBUG_RENDER_FILL, option);
	//DebugDraw::GetInstance()->DebugRenderLogf("VELOCITY %f", xVelocity);
	if( xVelocity > m_minVelocityForIdle)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkEast");
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
	}
	Vector3 prevTransform = GetRigidBody3DComponent()->m_previousTransform.GetWorldPosition();
	Vector3 marioWorldPosition = m_transform.GetWorldPosition();
	//DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f, %f, %f", marioWorldPosition.x, marioWorldPosition.y, marioWorldPosition.z);
	UpdateMiniMap(deltaTime);
	//DebugDraw::GetInstance()->DebugRenderLogf("Previous %f, %f, %f", prevTransform.x, prevTransform.y, prevTransform.z);
	//DebugDraw::GetInstance()->DebugRenderLogf("Diff %f, %f, %f"    , (marioWorldPosition.x - prevTransform.x), (marioWorldPosition.y - prevTransform.y), (marioWorldPosition.z - prevTransform.z));
	QueryAndDie(deltaTime);
	CheckForBounds();
	Entity::Update(deltaTime);

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
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(-m_movementForce, 0, 0), deltaTime);
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
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(m_movementForce, 0, 0), deltaTime);
	m_forwardVector = Vector2::EAST;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::Walk(float deltaTime, float force)
{
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(force*m_movementForce, 0, 0), deltaTime);
	if(force > 0)
	{
		m_forwardVector = Vector2::EAST;
	}
	else
	{
		m_forwardVector = Vector2::WEST;
	}
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
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->AddForce(Vector3(0, m_jumpForce, 0), deltaTime);
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
	Vector3 velocity = ((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->GetVelocity(deltaTime);
	velocity.x = 0;
	if(IsAlmostEqual(velocity,Vector3::ZERO,0.05))
	{
		return true;
	}
	return false;
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
	m_isDead = true;
	m_fitness = m_transform.GetWorldPosition().x/100.f - m_numOfJumps;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mario::QueryAndDie(float deltaTime)
{
	m_timeElapsedAfterKnownLocation += deltaTime;
	if (m_timeElapsedAfterKnownLocation > 2)
	{
		Vector2 distance = (m_transform.GetWorldPosition().GetXY() - m_lastKnownPosition);
		if (distance.GetLength() < m_map->m_block.GetDimensions().x * 2)
		{
			EndOfPlay();
		}
		m_lastKnownPosition = m_transform.GetWorldPosition().GetXY();
		m_timeElapsedAfterKnownLocation = 0.f;
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
		BoxCollider2D *boxcollider = (BoxCollider2D*)collider;
		Vector3 position = m_transform.GetWorldPosition();
		float   radius = GetCircleCollider()->m_disc.radius;
		Vector2 circleColliderPosition = GetCircleCollider()->m_disc.center;
		if(boxcollider->m_colliderDirection == SOUTH)
		{
			float distanceToPush = ((circleColliderPosition.y + radius) - boxcollider->m_aabb2.mins.y); 
			m_transform.SetLocalPosition(Vector3(position.x, position.y - distanceToPush, 0));
		}
		if (boxcollider->m_colliderDirection == NORTH)
		{
			float distanceToPush = (boxcollider->m_aabb2.maxs.y - (circleColliderPosition.y - radius));
			m_transform.SetLocalPosition(Vector3(position.x, position.y + distanceToPush, 0));
		}
		if (boxcollider->m_colliderDirection == EAST)
		{
			float distanceToPush = (boxcollider->m_aabb2.maxs.x - (circleColliderPosition.x - radius));
			m_transform.SetLocalPosition(Vector3(position.x + distanceToPush, position.y, 0));
		}
		if (boxcollider->m_colliderDirection == WEST)
		{
			float distanceToPush = ((circleColliderPosition.x + radius) - boxcollider->m_aabb2.mins.x);
			m_transform.SetLocalPosition(Vector3(position.x - distanceToPush, position.y, 0));
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (boxcollider->m_colliderDirection == NORTH_WEST)
		{
			Vector2 northWestCornerPosition(boxcollider->m_aabb2.mins.x, boxcollider->m_aabb2.maxs.y);
			Vector2 distance				 = circleColliderPosition - northWestCornerPosition;
			Vector2 pushDirection			 = distance.GetNormalized();
			Vector2 requiredDistance		 = pushDirection*radius;
			float   differenceX				 = requiredDistance.x - distance.x;
			float   differenceY				 = requiredDistance.y - distance.y;

			Vector2 finalPosition = position.GetXY() + Vector2(differenceX, differenceY);

			m_transform.SetLocalPosition(Vector3(finalPosition, 0));
		}
		if (boxcollider->m_colliderDirection == NORTH_EAST)
		{
			Vector2 northEastCornerPosition(boxcollider->m_aabb2.maxs.x, boxcollider->m_aabb2.maxs.y);
			Vector2 distance = circleColliderPosition - northEastCornerPosition;
			Vector2 pushDirection = distance.GetNormalized();
			Vector2 requiredDistance = pushDirection * radius;
			float   differenceX = requiredDistance.x - distance.x;
			float   differenceY = requiredDistance.y - distance.y;

			Vector2 finalPosition = position.GetXY() + Vector2(differenceX, differenceY);

			m_transform.SetLocalPosition(Vector3(finalPosition, 0));
		}
		if (boxcollider->m_colliderDirection == SOUTH_WEST)
		{
			Vector2 southWestCornerPosition(boxcollider->m_aabb2.mins.x, boxcollider->m_aabb2.mins.y);
			Vector2 distance = circleColliderPosition - southWestCornerPosition;
			Vector2 pushDirection = distance.GetNormalized();
			Vector2 requiredDistance = pushDirection * radius;
			float   differenceX = requiredDistance.x - distance.x;
			float   differenceY = requiredDistance.y - distance.y;

			Vector2 finalPosition = position.GetXY() + Vector2(differenceX, differenceY);

			m_transform.SetLocalPosition(Vector3(finalPosition, 0));
		}
		if (boxcollider->m_colliderDirection == SOUTH_EAST)
		{
			Vector2 southEastCornerPosition(boxcollider->m_aabb2.maxs.x, boxcollider->m_aabb2.mins.y);
			Vector2 distance = circleColliderPosition - southEastCornerPosition;
			Vector2 pushDirection = distance.GetNormalized();
			Vector2 requiredDistance = pushDirection * radius;
			float   differenceX = requiredDistance.x - distance.x;
			float   differenceY = requiredDistance.y - distance.y;

			Vector2 finalPosition = position.GetXY() + Vector2(differenceX, differenceY);

			m_transform.SetLocalPosition(Vector3(finalPosition, 0));
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
