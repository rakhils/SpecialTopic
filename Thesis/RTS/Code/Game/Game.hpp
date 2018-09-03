#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtil.hpp"
#include "GameCommon.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\Image.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

class Game
{
public:
	bool isQuitTriggered = false;
	static Game *s_game;

	Map *m_map			 = nullptr;

	Game::Game();
	void Game::Update(float deltaTime);
	void Game::Render();

	static Game* Game::GetInstance();


};