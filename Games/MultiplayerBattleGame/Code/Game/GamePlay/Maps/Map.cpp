#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"

#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Math/MathUtil.hpp"

Map::Map()
{

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
/*DATE    : 2018/09/01
*@purpose : Creates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateExplosions(Vector2 position)
{
	Explosion *explosion = new Explosion(position);
	m_explosions.push_back(explosion);
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

	if(InputSystem::GetInstance()->WasRButtonJustPressed())
	{
		g_currentSelectedEntity = nullptr;
	}

}

void Map::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	UpdateCamera(deltaTime);

	UpdateExplosions(deltaTime);
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
/*DATE    : 2018/09/01
*@purpose : Updates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateExplosions(float deltaTime)
{
	for (int explosionIndex = 0; explosionIndex < m_explosions.size(); explosionIndex++)
	{
		Explosion *explosion = m_explosions.at(explosionIndex);
		explosion->Update(deltaTime);
		if(explosion->m_markForDelete)
		{
			delete explosion;
			m_explosions.erase(m_explosions.begin() + explosionIndex, m_explosions.begin() + explosionIndex + 1);
			explosionIndex--;
		}
	}
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	
	RenderGrids();
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
	/*for(float x = 0; x < (static_cast<float>(m_maxWidth) + 1)*g_unitDistance ;x += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(x, 0.f, 0.f), Vector3(x, (static_cast<float>(m_maxHeight) + 2)*g_unitDistance, 0));
	}
	for (float y = 0; y < (static_cast<float>(m_maxHeight) + 2)* g_unitDistance; y += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(0.f, y, 0.f), Vector3((m_maxWidth + 1.f)* g_unitDistance,y,0.f));
	}*/
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderExplosions()
{
	for(int explosionIndex = 0;explosionIndex < m_explosions.size();explosionIndex++)
	{
		m_explosions.at(explosionIndex)->Render();
	}
}
