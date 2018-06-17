#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"

#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

enum GAME_STATE
{
	LOADING_SCREEN,
	MAIN_MENU,
	READY_UP,
	LEVEL1,
};
class ForwardRenderingPath;
class Renderable;
class Scene;
class Ship;
class Game
{
public:
	bool				   m_init				   = false;
	GAME_STATE			   m_state;

	Ship				   *m_tank			       = nullptr;
	Scene				   *m_currentScene	       = nullptr;
	Scene				   *m_nextScene		       = nullptr;
	Scene				   *m_sceneLoading	       = nullptr;
	Scene				   *m_sceneMainMenu        = nullptr;
	Scene				   *m_sceneReadyUp	       = nullptr;
	Scene				   *m_sceneLevel1	       = nullptr;

	ForwardRenderingPath   *m_forwardRenderingPath = nullptr;

	static Vector3			s_minBounds;
	static Vector3			s_maxBounds;

	Game();
	void Init();
	void InitScenes();
	void SetCurrentScene(Scene *scene);
	void SelectNextScene();
	void CreatePlayer();
	void CreateTerrain();
	void Update(float deltaTime);
	void Render();

	static bool		IsAlly(int team1, int team2);
	static Vector3  GetMinBounds();
	static void     SetMinBounds(Vector3 bounds);
	static Vector3  GetMaxBounds();
	static void     SetMaxBounds(Vector3 bounds);
};