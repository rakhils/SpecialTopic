#pragma once
#include <string>
#include <map>
#include <vector>

#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GamePlay/Tiles/Tiles.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
//#include "Game/GamePlay/MapGenSteps.hpp"
#include "Game/GamePlay/Entity/Brick.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Entity/Mario.hpp"

class Map
{
public:
	bool m_init = false;
	Camera *m_camera;
	Texture *m_textureBackground;
	Vector2 m_dimensions;
	std::vector<Tiles*> m_tiles;
	std::vector<Brick*> m_bricks;
	Mario *m_mario;

	Map(MapDefinition* def);
	void InitCamera();
	void CreateTiles();
	void CreateCharacters();
	void InitScore();
	void InitTime();
	void SpawnEntities(Entity_Type tyep,Vector2 position,Vector2 orientation);

	void Update(float deltaTime);
	void UpdateMarioVsBrickCollision();
	void Render();
	void RenderMario();
	void RenderBricks();
};
