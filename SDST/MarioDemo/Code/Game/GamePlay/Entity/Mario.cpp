#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Core/Component.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Physics/Collider/CircleCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"
#include "Game/GamePlay/Entity/Mario.hpp"
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
	StayIdle();
}

// DESTRUCTOR
Mario::~Mario()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Update mario character 
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::Update(float deltaTime)
{
	Disc2 colliderOutline = GetCircleCollider()->m_disc;
	std::vector<float> NNOutputs;
	for (int index = 0; index < m_neuralNet->m_outputs->m_neurons.size(); index++)
	{
		float value = m_neuralNet->m_outputs->m_neurons.at(index).m_value;
		NNOutputs.push_back(value);
	}
	
	DebugDraw::GetInstance()->DebugRenderLogf("NN OUTPUT %f, %f", NNOutputs.at(0), NNOutputs.at(1));

	float random = GetRandomIntInRange(0, 3);
	//random = -1;
	if(g_theInput->isKeyPressed(InputSystem::KEYBOARD_LEFT_ARROW) || NNOutputs.at(0) < 0)
	{
		WalkWest(deltaTime);
	}
	if (g_theInput->wasKeyJustReleased(InputSystem::KEYBOARD_LEFT_ARROW))
	{
		//StayIdle();
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_RIGHT_ARROW) || NNOutputs.at(0) > 0)
	{
		WalkEast(deltaTime);
	}
	if (g_theInput->wasKeyJustReleased(InputSystem::KEYBOARD_UP_ARROW))
	{
		ResetJump();
	}
	
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_DOWN_ARROW))
	{
		((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->ApplyForce(Vector3(0, -2*m_jumpForce, 0), deltaTime);
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_UP_ARROW)|| NNOutputs.at(1) > 0.75f )
	{
		if(IsGrounded(deltaTime) || IsJumping())
		{
			UpdateJump(deltaTime);
			if (m_currentJumpForce < m_maxJumpForce)
			{
				Jump(deltaTime);
			}
		}
	}
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
	DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f, %f, %f", marioWorldPosition.x, marioWorldPosition.y, marioWorldPosition.z);
	//DebugDraw::GetInstance()->DebugRenderLogf("Previous %f, %f, %f", prevTransform.x, prevTransform.y, prevTransform.z);
	//DebugDraw::GetInstance()->DebugRenderLogf("Diff %f, %f, %f"    , (marioWorldPosition.x - prevTransform.x), (marioWorldPosition.y - prevTransform.y), (marioWorldPosition.z - prevTransform.z));
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
void Mario::UpdateJump(float deltaTime)
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
	Material *defaultMaterial = Material::AquireResource("default");
	m_texture = Texture::CreateOrGetTexture("Data\\Images\\player.png",true,true);
	defaultMaterial->m_textures.at(0) = m_texture;
	Texture::SetCurrentTexture(m_texture);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Entity::Render();

	//Disc2 colliderOutline = GetCircleCollider()->m_disc;
	//Renderer::GetInstance()->DrawCircle(colliderOutline.center,colliderOutline.radius);
	//DebugDraw::GetInstance()->DebugRenderLogf("COLLIDER POSITION %f, %f",colliderOutline.center.x, colliderOutline.center.y);

	/*Vector2 entityPosition = m_transform.GetWorldPosition().GetXY();
	AABB2 m_aabb2(entityPosition, GetDrawDimensions().x / 2, GetDrawDimensions().y / 2);
	Renderer::GetInstance()->DrawAABB(m_aabb2,Rgba::WHITE);*/
	delete defaultMaterial;
}
