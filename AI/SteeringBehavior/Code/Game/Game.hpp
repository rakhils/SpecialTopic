#pragma once

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"

enum GameMode
{
	MAIN_MENU,
	GAME_PLAY
};

class Game
{
public:
	GameMode					m_gameMode;
	bool						isQuitTriggered					= false;
	Map *						m_map							= nullptr;
	bool						m_init							= false;
	Camera *					m_camera						= nullptr;

	// STATIC
	static Game *s_game;

	// FUNCIONS
	Game();
	~Game();

	void						Initialize();
	void						InitCamera();

	void						UpdateCamera  (float deltaTime);
	void						UpdateMainMenu(float deltaTime);
	void						UpdateGame	  (float deltaTime);
	void						Update		  (float deltaTime);
								
	void						Render();
	void						RenderMainMenu();
	void						RenderGame();
								
	static Game*				GetInstance();

};


