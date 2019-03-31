#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GamePlay/Tiles/Tiles.hpp"

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

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	
	RenderGrids();
	RenderTiles();
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
		g_theRenderer->DrawLine(Vector3(0.f, y, 0.f), Vector3(g_maxWidth* g_unitDistance, y, 0.f),Rgba::BLUE);
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
