#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"

enum GameMode
{
	MAIN_MENU,
	GAME_PLAY
};
struct MainMenuItems
{
	int m_index;
	std::string m_menuItemString;
};
class Game
{
public:
	bool		 isQuitTriggered = false;
	GameMode     m_gameMode;
	Map *		 m_map			 = nullptr;
	Camera *     m_camera		 = nullptr;
	int			 m_currentIndex = 0;
	bool		 m_init			= false;
	std::vector<MainMenuItems> m_mainMenuItems;
	// STATIC
	static Game *s_game;

	// FUNCIONS
	Game();
	~Game();
	
	void Initialize();
	void InitMainMenuItems();
	void InitCamera();

	void InitMapAndStartGame();

	void Update(float deltaTime);
	void Render();

	void UpdateMap(float deltaTime);
	void UpdateMainMenu(float deltaTime);

	void RenderMap();
	void RenderMainMenu();

	void InitSampleNN();
	static Game* GetInstance();

};