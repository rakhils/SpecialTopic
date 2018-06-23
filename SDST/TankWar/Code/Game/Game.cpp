#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderDefinitions.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Physics/Collider/SphereCollider.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"   
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/SceneManagement/SceneManager.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleSystem.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Audio/AudioComponent.hpp"
#include "Engine/Logger/LogManager.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GamePlay/Scenes/SceneLoading.hpp"
#include "Game/GamePlay/Scenes/SceneMainMenu.hpp"
#include "Game/GamePlay/Scenes/SceneReadyUp.hpp"
Vector3 Game::s_minBounds = Vector3::ZERO;
Vector3 Game::s_maxBounds = Vector3::ZERO;

Game::Game()
{
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/10
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::Init()
{
	if(m_init)
	{
		return;
	}
	m_forwardRenderingPath = new ForwardRenderingPath(g_theRenderer);
	InitScenes();
	InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_RELATIVE;
	m_init = true;

	/*Light *light = Light::CreateAmbientLight();
	light->EnableAmbientLight(Rgba::FADED_WHITE.GetAsFloats());*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Initialize all scenes
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitScenes()
{
	m_sceneLevel1                             = new SceneLevel1("Level1");
	m_sceneMainMenu                           = new SceneMainMenu("MainMenu");
	m_sceneReadyUp                            = new SceneReadyUp("SceneReadUp");
	m_sceneLoading                            = new SceneLoading("LoadingScene");
	((SceneLoading*)m_sceneLoading)->m_level1Scene = m_sceneLevel1;
	//m_sceneLoading->m_forwardRenderingPath    = m_forwardRenderingPath;
	//m_sceneMainMenu->m_forwardRenderingPath   = m_forwardRenderingPath;
	//m_sceneReadyUp->m_forwardRenderingPath    = m_forwardRenderingPath;
	//m_sceneLevel1->m_forwardRenderingPath     = m_forwardRenderingPath;
	//SetCurrentScene(m_sceneLoading);
	m_state = LOADING_SCREEN;
	SetCurrentScene(m_sceneLoading);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Sets the current scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::SetCurrentScene(Scene *scene)
{
	if(m_currentScene)
	{
		m_currentScene->Reset();
	}
	m_currentScene = scene;
	m_currentScene->EnterScene();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::SelectNextScene()
{
	m_currentScene->Reset();
	switch (m_state)
	{
	case LOADING_SCREEN:
		SetCurrentScene(m_sceneMainMenu);
		m_state = MAIN_MENU;
		break;
	case MAIN_MENU:
		SetCurrentScene(m_sceneReadyUp);
		m_state = READY_UP;
		break;
	case READY_UP:
		SetCurrentScene(m_sceneLevel1);
		m_state = LEVEL1;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : Update Game
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Game::Update(float deltaTime)
{	
	Init();
	float deltaSecond = static_cast<float>(Clock::g_theMasterClock->frame.m_seconds);
	deltaSecond = ClampFloat(deltaSecond, 0, 0.016f);
	if(m_currentScene->m_isExitTaskComplete)
	{
		SelectNextScene();
	}
	m_currentScene->Update(deltaTime);
	DebugDraw::Update();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : Render Game
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Game::Render()
{
	//LogManager::PushLog("game.txt", "RENDER \n", "");
	m_currentScene->Render();
	DebugDraw::Render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::IsAlly(int team1, int team2)
{
	if(team1 == team2)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Game::GetMinBounds()
{
	return s_minBounds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::SetMinBounds(Vector3 bounds)
{
	s_minBounds = bounds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Game::GetMaxBounds()
{
	return s_maxBounds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::SetMaxBounds(Vector3 bounds)
{
	s_maxBounds = bounds;
}

