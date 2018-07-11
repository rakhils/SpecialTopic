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
#include "Game/GamePlay/Entity/EntityDefinition.hpp"

class Game
{
public:
	Map *  m_map = nullptr;
	bool   isQuitTriggered = false;
	int    m_numberOfIterations = 1;
	static Game *s_game;
	bool   m_isTraining = false;
	
	Game();
	void Update(float deltaTime);
	void Render();

	void GenerateMap(std::string,std::string);

	static Game* GetInstance();
};