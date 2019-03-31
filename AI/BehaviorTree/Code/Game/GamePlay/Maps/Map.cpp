#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GamePlay/Tiles/Tiles.hpp"
#include "Game/GamePlay/Entity/Player.hpp"
#include "Game/GamePlay/Entity/Bullets.hpp"
#include "Game/GamePlay/Enemy.hpp"

#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"

Map::Map()
{
	Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Init the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Initialize()
{
	InitCamera();
	InitBlocks();
	InitObstacles();
	InitGameEntities();
	m_bullets.reserve(10);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Inits camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_camera   = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitBlocks()
{
	for(int tileIndex = 0;tileIndex < g_maxWidth * g_maxHeight;tileIndex++)
	{
		m_tiles.push_back(new Tiles());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitObstacles()
{
	for(int index = 0;index < g_maxWidth * g_maxHeight;index++)
	{
		if(GetRandomFloatZeroToOne() > 0.75f)
		{
			m_tiles.at(index)->m_type = BLOCK;
		}
		int xCords = index % g_maxWidth;
		int yCords = index / g_maxWidth;

		m_tiles.at(index)->m_tileCoords = IntVector2(xCords*g_unitDistance, yCords * g_unitDistance);

		IntVector2 mins = m_tiles.at(index)->m_tileCoords - IntVector2(g_unitDistance / 2, g_unitDistance / 2) + IntVector2(g_unitDistance / 2, g_unitDistance / 2);
		IntVector2 maxs = m_tiles.at(index)->m_tileCoords + IntVector2(g_unitDistance / 2, g_unitDistance / 2) + IntVector2(g_unitDistance / 2, g_unitDistance / 2);

		m_tiles.at(index)->m_aabb2 = AABB2(mins.GetAsVector2(), maxs.GetAsVector2());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : Creates player and Enemy
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitGameEntities()
{
	m_player = new Player();
	m_enemy  = new Enemy();
	m_enemy->m_map = this;
	m_enemy->InitBTree();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateBullet(Vector2 position, Vector2 direction)
{
	Bullets *bullet = new Bullets();
	m_bullets.push_back(bullet);
	bullet->m_direction = direction;
	bullet->m_position = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckBulletVsPlayerCollision()
{
	for(int index = 0;index < m_bullets.size();index++)
	{
		if(m_bullets.at(index)->m_isDead)
		{
			continue;
		}
		Vector2 bulletPosition = m_bullets.at(index)->m_position;
		Vector2 playerPosition = m_player->m_position;


		float distance = (bulletPosition - playerPosition).GetLength();
		if(distance < m_bullets.at(index)->m_radius + m_player->m_radius)
		{
			m_player->Attack();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
}

void Map::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	UpdateCamera(deltaTime);
	UpdatePlayer(deltaTime);
	UpdateEnemy(deltaTime);
	UpdateBullet(deltaTime);

	CheckBulletVsPlayerCollision();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Updates camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdatePlayer(float deltaTime)
{
	m_player->Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateEnemy(float deltaTime)
{
	m_enemy->Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateBullet(float deltaTime)
{
	for(int index = 0;index < m_bullets.size();index++)
	{
		m_bullets.at(index)->Update(deltaTime);
		if(m_bullets.at(index)->m_isDead)
		{
			Bullets *bullet = m_bullets.at(index);
			delete bullet;
			m_bullets.erase(m_bullets.begin() + index, m_bullets.begin() + index + 1);
			index--;
		}
	}
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	
	//RenderGrids();
	//RenderTiles();
	
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawLine(Vector3(0, 325.0f, 0.f), Vector3(1920, 325, 0),Rgba::BLUE);
	delete defaultMaterial;

	RenderPlayer();
	RenderEnemy();
	RenderBullet();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/22
*@purpose : render grids
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderGrids()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);


	for (float x = 0; x < static_cast<float>(g_maxWidth*g_unitDistance); x += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(x, 0.f, 0.f), Vector3(x, static_cast<float>(g_maxHeight*g_unitDistance), 0),Rgba::BLUE);
	}
	for (float y = 0; y < static_cast<float>(g_maxHeight* g_unitDistance); y += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(0.f, y, 0.f), Vector3(static_cast<float>(g_maxWidth* g_unitDistance), y, 0.f),Rgba::BLUE);
	}

	g_theRenderer->DrawLine(Vector3(0.f, 0.f, 0.f), Vector3(0.f,static_cast<float>(g_maxHeight*g_unitDistance), 0));
	g_theRenderer->DrawLine(Vector3(0.f, 0.f, 0.f), Vector3(static_cast<float>(g_maxWidth*g_unitDistance), 0.f, 0));

	g_theRenderer->DrawLine(Vector3(0.f, static_cast<float>(g_maxHeight*g_unitDistance), 0),Vector3(static_cast<float>(g_maxWidth*g_unitDistance),static_cast<float>(g_maxHeight)*g_unitDistance,0.f));
	g_theRenderer->DrawLine(Vector3(static_cast<float>(g_maxWidth*g_unitDistance),static_cast<float>(g_maxHeight)*g_unitDistance,0.f), Vector3(static_cast<float>(g_maxWidth)*g_unitDistance, 0.f,0.f));


	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderTiles()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(int tileIndex = 0;tileIndex < m_tiles.size();tileIndex++)
	{
		if(m_tiles.at(tileIndex)->m_type == BLOCK)
		{
			g_theRenderer->DrawAABB(m_tiles.at(tileIndex)->m_aabb2, Rgba::BLUE);
		}
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderPlayer()
{
	m_player->Render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderEnemy()
{
	m_enemy->Render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderBullet()
{
	for (int index = 0; index < m_bullets.size(); index++)
	{
		m_bullets.at(index)->Render();
	}
}
