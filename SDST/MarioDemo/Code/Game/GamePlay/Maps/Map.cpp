#include "Map.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"

#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
Map::Map(MapDefinition *mapDef)
{
	m_dimensions.x = mapDef->m_width;
	m_dimensions.y = mapDef->m_heigth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;

	m_camera = new OrthographicCamera();

	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/24
*@purpose : Creats abd set all characters
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::CreateCharacters()
{
	InitCamera();
	m_mario = new Mario(EntityDefinition::GetDefinition("Mario"));
	m_mario->SetPosition(Vector2::ZERO);
	m_mario->AddRigidBody3DComponent();
	((RigidBody3D*)m_mario->GetComponentByType(RIGID_BODY_3D))->m_friction = 0.8f;
	((RigidBody3D*)m_mario->GetComponentByType(RIGID_BODY_3D))->m_useGravity = true;
	((RigidBody3D*)m_mario->GetComponentByType(RIGID_BODY_3D))->m_gravity = Vector3(0, -9.8, 0);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/04
*@purpose : Spawn entities at given postion and orientation
*
*@param   : Entity type,Positoin,Orientation
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::SpawnEntities(Entity_Type type, Vector2 position, Vector2 orientation)
{
	switch (type)
	{
	case BRICK:
	{
		Brick * brick = new Brick(EntityDefinition::GetDefinition("Brick"));
		brick->SetPosition(position);
		m_bricks.push_back(brick);
	}
		break;
	case COIN_BOX:
		break;
	case PIPE:
		break;
	case POWER_UP:
		break;
	default:
		break;
	}
}

void Map::Update(float deltaTime)
{
	if(!m_init)
	{
		CreateCharacters();
		m_init = true;
	}
	m_camera->SetOrthoProjection();
	m_mario->Update(deltaTime);
	UpdateMarioVsBrickCollision();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateMarioVsBrickCollision()
{
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/22
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Material *defaultMaterial = Material::AquireResource("default");
	m_textureBackground = Texture::CreateOrGetTexture("Data//Images//level1.png",true,false);
	defaultMaterial->m_textures.at(0) = m_textureBackground;
	Texture::SetCurrentTexture(m_textureBackground);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	AABB2 m_aabb2(Vector2(0, 0), Vector2(6784.f,480.f));
	g_theRenderer->DrawTexturedAABB(m_aabb2, m_textureBackground, Vector2(0,1), Vector2(1,0), Rgba::WHITE);
	delete defaultMaterial;

	RenderBricks();
	RenderMario();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMario()
{
	m_mario->Render();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/04
*@purpose : Render all bricks
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Map::RenderBricks()
{
	for (size_t brickIndex = 0; brickIndex < m_bricks.size(); brickIndex++)
	{
		m_bricks.at(brickIndex)->Render();
	}
}

