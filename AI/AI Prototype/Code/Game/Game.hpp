#pragma once

#include "Engine/Debug/DebugDraw.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"

class Game
{
public:
	bool						isQuitTriggered					= false;
	Map *						m_map							= nullptr;
	bool						m_init							= false;

	// STATIC
	static Game *s_game;

	// FUNCIONS
	Game();
	~Game();

	void						Initialize();

	void						UpdateMainMenu(float deltaTime);
	void						UpdateGame	  (float deltaTime);
	void						Update		  (float deltaTime);
								
	void						Render();
	void						RenderMainMenu();
	void						RenderGame();
								
	static Game*				GetInstance();

};


