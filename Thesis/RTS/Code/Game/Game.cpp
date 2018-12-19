#include "Game.hpp"
#include "Engine/Net/Socket.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetObjectType.hpp"
#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Net/NetObject.hpp"

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
	InitCamera();
	InitMainMenuItems();
	m_gameMode = MAIN_MENU;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Inits main menu items
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitMainMenuItems()
{
	for(int index = 0;index < MAP_MODE_NUM_ITEMS;index++)
	{
		MainMenuItems mmitem;
		mmitem.m_index = index + 1;
		mmitem.m_menuItemString = Map::GetMapModeAsString((MapMode)index);
		m_mainMenuItems.push_back(mmitem);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Inits main camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Inits map and start game
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitMapAndStartGame()
{
	if (m_map == nullptr)
	{
		m_map = new Map();
		m_map->SetMapType(MapMode(m_currentIndex));
		m_map->Initialize();
		m_gameMode = GAME_PLAY;
	}
}

void Game::Update(float deltaTime)
{
	if(!m_init)
	{
		Initialize();
		m_init = true;
	}
	
	m_camera->SetOrthoProjection();
	switch (m_gameMode)
	{
	case MAIN_MENU:
		UpdateMainMenu(deltaTime);
		break;
	case GAME_PLAY:
		UpdateMap(deltaTime);
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
		RenderMap();
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Updates map if exist
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateMap(float deltaTime)
{
	if(m_map != nullptr)
	{
		if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_E))
		{
			g_skipRendering        = g_skipRendering ? false : true;
			g_isGlobalyCurrentlyTraining = g_isGlobalyCurrentlyTraining ? false : true;
		}
		
		if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
		{
			isQuitTriggered = true;
		}
		if(m_map->m_gameFinished)
		{
			g_lastTrainingStopTime += deltaTime;
			if(g_lastTrainingStopTime > 5)
			{
				if(g_isGlobalyCurrentlyTraining)
				{
					g_skipRendering = true;
				}
				g_lastTrainingStopTime = 0.f;
				m_map->RestartMap();
				m_map->m_gameFinished = false;
			}
		}
		
		m_map->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Updates mainmenu items
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateMainMenu(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_UP_ARROW))
	{
		m_currentIndex--;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_DOWN_ARROW))
	{
		m_currentIndex++;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_1))
	{
		m_currentIndex = 0;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_2))
	{
		m_currentIndex = 1;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_3))
	{
		m_currentIndex = 2;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_4))
	{
		m_currentIndex = 3;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_5))
	{
		m_currentIndex = 4;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_6))
	{
		m_currentIndex = 5;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_7))
	{
		m_currentIndex = 6;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_8))
	{
		m_currentIndex = 7;
		InitMapAndStartGame();
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_9))
	{
		m_currentIndex = 8;
		InitMapAndStartGame();
	}

	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ENTER))
	{
		InitMapAndStartGame();
	}
	m_currentIndex = ClampInt(m_currentIndex, 0, static_cast<int>(m_mainMenuItems.size() - 1));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Renders map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderMap()
{
	if(m_map != nullptr)
	{
		m_map->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Render main menu
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderMainMenu()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Vector2 menuItemStartPosition(g_mainMenuStartX, g_mainMenuStartY);
	//Material *defaultMaterial = Material::AquireResource("default");
	//Renderer::GetInstance()->BindMaterial(defaultMaterial);
	//Renderer::GetInstance()->DrawAABB(AABB2(menuItemStartPosition + Vector2(-100, m_currentIndex*g_fontSize * 4),500,50),Rgba::WHITE);
	for(int menuItemIndex = 0;menuItemIndex < m_mainMenuItems.size();menuItemIndex++)
	{
		std::string menuItemString = ToString(menuItemIndex + 1);
		menuItemString.append(" :: "+m_mainMenuItems.at(menuItemIndex).m_menuItemString);
		Rgba color = Rgba::WHITE;
		if(m_currentIndex == menuItemIndex)
		{
			color = Rgba::YELLOW;
		}
		//color = Rgba(GetRandomFloatInRange(0,255), GetRandomFloatInRange(0,255),GetRandomFloatInRange(0,255));

		g_theRenderer->DrawTextOn3DPoint(menuItemStartPosition, Vector3::RIGHT, Vector3::UP, menuItemString, g_fontSize, color);
		menuItemStartPosition.y -= g_fontSize*4;
		break;
	}
	delete textMaterial;
}


Game* Game::GetInstance()
{
	if (s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}

void Game::InitSampleNN()
{
	NeuralNetwork neuralNet(2, 2, 2);
	neuralNet.m_inputs->m_neurons.at(0).m_value = 0.05f;
	neuralNet.m_inputs->m_neurons.at(1).m_value = 0.1f;
	neuralNet.m_inputs->m_bias.m_value = 1;

	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(0) = 0.15f;
	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(1) = 0.25f;

	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(0) = 0.20f;
	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(1) = 0.30f;

	neuralNet.m_inputs->m_bias.m_weights.at(0) = 0.35f;
	neuralNet.m_inputs->m_bias.m_weights.at(1) = 0.35f;

	///////////////////////////////////////////////////////////////

	/*neuralNet.m_hiddenLayers->m_neurons.at(0).m_value = 0.40;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_value = 0.1;*/
	neuralNet.m_hiddenLayers->m_bias.m_value = 1;

	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(0) = 0.40f;
	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(1) = 0.50f;

	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(0) = 0.45f;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(1) = 0.55f;

	neuralNet.m_hiddenLayers->m_bias.m_weights.at(0) = 0.60f;
	neuralNet.m_hiddenLayers->m_bias.m_weights.at(1) = 0.60f;

	neuralNet.FeedForwardNN();
	
	std::vector<double> inputs;
	inputs.push_back(.05);
	inputs.push_back(.1);

	std::vector<double> m_targetOuputs;
	m_targetOuputs.push_back(.5);
	m_targetOuputs.push_back(.5);
	for(int index = 0;index < 2000;index ++)
	{
		inputs.at(0) = GetRandomFloatZeroToOne();
		inputs.at(1) = GetRandomFloatZeroToOne();
		neuralNet.FeedForward(inputs);
		neuralNet.BackPropogateOuputToHiddenLayer(m_targetOuputs);
		neuralNet.BackPropogateHiddenToInputLayer(m_targetOuputs);
	}
	neuralNet.FeedForward(inputs);
	neuralNet;
}