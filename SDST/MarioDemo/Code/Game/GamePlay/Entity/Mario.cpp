#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Core/Component.hpp"
#include "Engine/Debug/DebugDraw.hpp"

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
	if(g_theInput->isKeyPressed(InputSystem::KEYBOARD_LEFT_ARROW))
	{
		WalkWest(deltaTime);
	}
	if (g_theInput->wasKeyJustReleased(InputSystem::KEYBOARD_LEFT_ARROW))
	{
		//StayIdle();
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_RIGHT_ARROW))
	{
		WalkEast(deltaTime);
	}
	if (g_theInput->wasKeyJustReleased(InputSystem::KEYBOARD_RIGHT_ARROW))
	{
		//StayIdle();
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_UP_ARROW))
	{
		if(IsGrounded())
		{
			Jump(deltaTime);
		}
	}
	float xVelocity = ((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->m_velocity.x;
	DebugRenderOptions option;
	DebugDraw::GetInstance()->DebugRenderQuad2D(Vector3(300, 700, 0), AABB2(Vector2(0, 0), 300, 200), 0, nullptr, Rgba::BLACK, DEBUG_RENDER_FILL, option);
	DebugDraw::GetInstance()->DebugRenderLogf("VELOCITY %f", xVelocity);
	if( xVelocity > m_minVelocityForIdle)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkEast");
		m_currentAction = WALK; 
	}
	else
	if (xVelocity < m_minVelocityForIdle)
	{
		m_spriteAnimSet->SetAnimation(m_characterTypeString + "WalkWest");
		m_currentAction = WALK;
	}
	else
	{
		StayIdle();
	}
	Entity::Update(deltaTime);
}

/*DATE    : 2018/02/25/////////////////////////////////////////
*@purpose : Make character walk west by a tile
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Mario::WalkWest(float deltaTime)
{
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->ApplyForce(Vector3(-m_movementForce, 0, 0), deltaTime);
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
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->ApplyForce(Vector3(m_movementForce, 0, 0), deltaTime);
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
	((RigidBody3D*)GetComponentByType(RIGID_BODY_3D))->ApplyForce(Vector3(0, m_jumpForce, 0), deltaTime);
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
bool Mario::IsGrounded()
{
	/*if(m_velocity.y == 0)
	{
		return true;
	}
	return false;*/
	return true;
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
	m_texture = Texture::CreateOrGetTexture("Data\\Images\\player.png",true,false);
	defaultMaterial->m_textures.at(0) = m_texture;
	Texture::SetCurrentTexture(m_texture);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Entity::Render();
	delete defaultMaterial;
}
