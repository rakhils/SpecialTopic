#include "Game.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
Game *Game::s_game = nullptr;

Game::Game()
{
	
}

Game::~Game()
{

}

// INIT GAME
void Game::Initialize()
{
	m_gameMode = MAIN_MENU;
	InitCamera();
}

void Game::InitCamera()
{
	int width = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_camera = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetCurrentCamera(m_camera);
	Camera::SetGameplayCamera(m_camera);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/16
*@purpose : Updates main menu
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateMainMenu(float deltaTime)
{
	UNUSED(deltaTime);
	if(InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_1))
	{
		m_map = new Map();
		m_map->m_mapType = MAP_TYPE_SEEK_ARRIVE_BEHAVIOR;
		m_gameMode = GAME_PLAY;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_2))
	{
		m_map = new Map();
		m_map->m_mapType = MAP_TYPE_FLOW_FIELD_BEHAVIOR;
		m_gameMode = GAME_PLAY;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_3))
	{
		m_map = new Map();
		m_map->m_mapType = MAP_TYPE_PATH_FOLLOWING;
		m_gameMode = GAME_PLAY;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_4))
	{
		m_map = new Map();
		m_map->InitEntities();
		m_map->m_mapType = MAP_TYPE_UNKNOWN;
		m_gameMode = GAME_PLAY;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_ESCAPE))
	{
		isQuitTriggered = true;
	}
}

void Game::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
}

void Game::Update(float deltaTime)
{
	if(!m_init)
	{
		Initialize();
		m_init = true;
	}
	
	UpdateCamera(deltaTime);
	switch (m_gameMode)
	{
	case MAIN_MENU:
		UpdateMainMenu(deltaTime);
		break;
	case GAME_PLAY:
		UpdateGame(deltaTime);
		break;
	default:
		break;
	}
}


void Game::Render()
{
	switch (m_gameMode)
	{
	case MAIN_MENU:
		RenderMainMenu();
		break;
	case GAME_PLAY:
		RenderGame();
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderMainMenu()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();

	Vector3 position(400.f, 900.f,0.f);
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	position.y -= 100;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "1. SEEK & ARRIVE BEHAVIOR", 20, Rgba::RED);
	position.y -= 100;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "2. FLOW FIELD BEHAVIOR", 20, Rgba::RED);
	position.y -= 100;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "3. PATH FOLLOWING", 20, Rgba::RED);
	position.y -= 100;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "PRESS KEY FROM (1 to 3)", 20, Rgba::RED);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Updates map if exist
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateGame(float deltaTime)
{
	if(m_map != nullptr)
	{
		m_map->Update(deltaTime);
		if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
		{
			m_map->DestroyAllEntity();
			delete m_map;
			m_map = nullptr;
			Camera::SetCurrentCamera(m_camera);
			Camera::SetGameplayCamera(m_camera);
			m_gameMode = MAIN_MENU;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderGame()
{
	if(m_map)
	{
		m_map->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Game* Game::GetInstance()
{
	if(s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}
