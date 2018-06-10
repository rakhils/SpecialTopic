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
class Pipe;
class Map
{
public:
	bool m_init = false;
	Camera *m_camera;
	AABB2   m_cameraQuads;
	AABB2   m_block;
	Texture *m_textureBackground;
	Vector2 m_dimensions;
	Vector2 m_miniMapPosition;
	float   m_miniMapScaleFactor = 1 / 5.f;
	std::vector<Brick*> m_bricks;
	std::vector<Pipe*>  m_pipes;
	Mario *m_mario;

	Map(MapDefinition* def);
	void InitCamera();
	void CreateBricks(Vector2 position,Vector2 dimension);
	void CreatePipes(Vector2 position, Vector2 dimensions);
	void CreateGround();
	void CreateTiles();
	void CreateCharacters();
	void CreateMario();
	void InitScore();
	void InitTime();
	void SpawnEntities(Entity_Type tyep,Vector2 position,Vector2 orientation);

	void Update(float deltaTime);
	void UpdateCamera();
	void UpdateMiniMap();
	void UpdateBrick(float deltaTime);
	void UpdatePipes(float deltaTime);

	void Render();
	void RenderMiniMap();
	void RenderMario();
	void RenderBricks();
	void RenderPipes();
	void RenderGrid();
};
