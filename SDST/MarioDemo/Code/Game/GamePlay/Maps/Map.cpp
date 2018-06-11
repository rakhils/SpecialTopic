#include "Map.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Debug/DebugDraw.hpp"

#include "Game/GamePlay/Entity/Pipe.hpp"
Map::Map(MapDefinition *mapDef)
{
	m_dimensions.x = mapDef->m_width;
	m_dimensions.y = mapDef->m_heigth;
	for(int index = 0;index < mapDef->m_genSteps.size();index++)
	{
		mapDef->m_genSteps.at(index)->Run(*this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_block = AABB2(Vector2::ZERO, 25, 25);
	m_camera = new OrthographicCamera();
	m_miniMapPosition = Vector2(width - 10 * (m_block.GetDimensions().x/2)*m_miniMapScaleFactor, height - 10 * (m_block.GetDimensions().y/2)*m_miniMapScaleFactor);
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
	((OrthographicCamera*)m_camera)->ZoomIn(Vector2(300,0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateBricks(Vector2 position,Vector2 dimension,bool hidden)
{
	int size = m_bricks.size();
	std::string name = ("brick_"+ToString(size));

	Brick *brick      = new Brick(name,EntityDefinition::GetDefinition("Brick"));
	brick->m_length   = dimension.x;
	brick->m_height   = dimension.y;
	brick->m_isHidden = hidden;
	brick->SetPosition(position);
	brick->AddBoxCollider2D(Vector3(0,0,0), Vector2(dimension.x/2.f, dimension.y/2.f), Vector3::FORWARD);
	brick->GetBoxCollider2D()->m_isStatic = true;
	m_bricks.push_back(brick);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreatePipes(Vector2 position, Vector2 dimensions)
{
	int size = m_pipes.size();
	std::string name = ("pipe_" + ToString(size));

	Pipe *pipe = new Pipe(name, EntityDefinition::GetDefinition("Pipe"));
	pipe->SetPosition(position);
	pipe->m_length = 2 * dimensions.x;
	pipe->m_height = 2 * dimensions.y;
	pipe->AddBoxCollider2D(Vector3(0, 0, 0), dimensions, Vector3::FORWARD);
	pipe->GetBoxCollider2D()->m_isStatic = true;
	m_pipes.push_back(pipe);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateGround()
{
	float stepSize = m_block.GetDimensions().x/2.f;
	for(int posX = stepSize/2.f;posX<7000;posX+=stepSize+20)
	{
		Vector2 position(posX, 38);
		//CreateBricks(position,m_block.GetDimensions());
	}
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
	m_textureBackground = Texture::CreateOrGetTexture("Data//Images//level1.png", true, false);
	InitCamera();
	//CreateGround();
//	CreateBricks(Vector2(200,200),m_block.GetDimensions());
	CreateMario();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateMario()
{
	m_mario = new Mario(EntityDefinition::GetDefinition("Mario"));
	m_mario->SetPosition(Vector2(200,400));
	m_mario->AddRigidBody3DComponent();
	m_mario->m_length = m_mario->m_definition->m_length;
	m_mario->m_height = m_mario->m_definition->m_height;
	Vector2 colliderPosition(0, -m_mario->m_definition->m_height / 2 + m_mario->m_definition->m_physicsRadius);
	m_mario->AddCircleCollider(Vector3(colliderPosition,0), m_mario->m_definition->m_physicsRadius);
	((RigidBody3D*)m_mario->GetComponentByType(RIGID_BODY_3D))->m_friction = 0.8f;
	((RigidBody3D*)m_mario->GetComponentByType(RIGID_BODY_3D))->m_useGravity = true;
	((RigidBody3D*)m_mario->GetComponentByType(RIGID_BODY_3D))->m_gravity = Vector3(0, -12000, 0);
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
		/*Brick * brick = new Brick(EntityDefinition::GetDefinition("Brick"));
		brick->SetPosition(position);
		m_bricks.push_back(brick);*/
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
	if(InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_Z))
	{
		((OrthographicCamera*)m_camera)->ZoomIn(Vector2(100*deltaTime,100*deltaTime));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_X))
	{
		((OrthographicCamera*)m_camera)->ZoomOut(Vector2(100*deltaTime,100*deltaTime));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		((OrthographicCamera*)m_camera)->MoveLeft(deltaTime * 120);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		((OrthographicCamera*)m_camera)->MoveDown(deltaTime*120);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		((OrthographicCamera*)m_camera)->MoveRigth(deltaTime*120);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		((OrthographicCamera*)m_camera)->MoveUp(deltaTime*120);
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_R))
	{
		Vector3 position = m_mario->GetWorldPosition();
		m_mario->m_transform.SetLocalPosition(Vector3(position.x, 450, 0));
	}
	m_camera->SetOrthoProjection();
	UpdateCamera();
	UpdateMiniMap();
	m_mario->Update(deltaTime);
	if(m_mario->m_transform.GetWorldPosition().y < 0)
	{
		m_mario->m_transform.SetLocalPosition(Vector3(200, 100, 0));
	}
	if (m_mario->m_transform.GetWorldPosition().x > 6700)
	{
		m_mario->m_transform.SetLocalPosition(Vector3(200, 100, 0));
	}

	//DebugDraw::GetInstance()->DebugRenderLogf("CREATED BY RAKHIL SOMAN", "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera()
{
	float minX = Windows::GetInstance()->GetDimensions().x / 2.f;
	float minY = Windows::GetInstance()->GetDimensions().y / 2.f;

	float maxX = m_textureBackground->getDimensions().x - Windows::GetInstance()->GetDimensions().x / 2.f;
	float maxY = Windows::GetInstance()->GetDimensions().y / 2.f;

	float cameraX = m_mario->m_transform.GetWorldPosition().x;
	float cameraY = 0;

	cameraX = ClampFloat(cameraX, minX, maxX);
	cameraY = ClampFloat(cameraY, minY, maxY);
	m_camera->m_transform.SetLocalPosition(Vector3(cameraX, cameraY, 0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateMiniMap()
{
	Vector2 position = m_mario->m_transform.GetWorldPosition().GetXY();
	m_cameraQuads = AABB2(position, 5 * m_block.GetDimensions().x / 2 + m_block.GetDimensions().x / 2, 5 * m_block.GetDimensions().y / 2 + m_block.GetDimensions().y / 2);
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateBrick(float deltaTime)
{
	for(Brick *brick : m_bricks)
	{
		brick->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdatePipes(float deltaTime)
{
	for (Pipe *pipe : m_pipes)
	{
		pipe->Update(deltaTime);
	}
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
	defaultMaterial->m_textures.at(0) = m_textureBackground;
	Texture::SetCurrentTexture(m_textureBackground);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	AABB2 m_aabb2(Vector2(0, 0), Vector2(6784.f,480.f));
	g_theRenderer->DrawTexturedAABB(m_aabb2, m_textureBackground, Vector2(0,1), Vector2(1,0), Rgba::WHITE);
	delete defaultMaterial;

	RenderBricks();
	//RenderPipes();
	RenderMario();
	//RenderMiniMap();
	//RenderGrid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMiniMap()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Vector2 marioPosition = m_mario->m_transform.GetWorldPosition().GetXY();
	AABB2 m_minimap(m_miniMapPosition, 10 * (m_block.GetDimensions().x/2)*m_miniMapScaleFactor, 10 * (m_block.GetDimensions().y/2)*m_miniMapScaleFactor);
	Renderer::GetInstance()->DrawAABB(m_minimap,Rgba::BLACK);
	Renderer::GetInstance()->DrawRectangle(m_minimap);
	for (int brickIndex = 0; brickIndex < m_bricks.size(); brickIndex++)
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
	}
	delete defaultMaterial;
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
		if(m_bricks.at(brickIndex)->m_isHidden)
		{
			continue;
		}
		m_bricks.at(brickIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderPipes()
{
	for (size_t pipeIndex = 0; pipeIndex < m_pipes.size(); pipeIndex++)
	{
		if (m_pipes.at(pipeIndex)->m_isHidden)
		{
			continue;
		}
		m_pipes.at(pipeIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderGrid()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	int incrementValue = 50;
	for(int x = 0;x< 1000;x+=incrementValue)
	{
		Renderer::GetInstance()->DrawLine(Vector2(x, 0), Vector2(x, 1000));
	}
	for (int y = 0; y < 1000; y += incrementValue)
	{
		Renderer::GetInstance()->DrawLine(Vector2(0, y), Vector2(1000, y));
	}
	delete defaultMaterial;
}

