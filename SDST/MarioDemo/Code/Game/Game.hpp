#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtil.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\Image.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/EntityDefinition.hpp"

class Game
{
public:
	Map *  m_map				= nullptr;
	bool   m_isQuitTriggered	= false;
	int    m_numberOfIterations = 1;
	bool   m_isTraining			= false;
	static Game *s_game;
	
	Game();
	void Update(float deltaTime);
	void Render();

	void GenerateMap(std::string levelName,std::string mapName);

	static Game* GetInstance();
};