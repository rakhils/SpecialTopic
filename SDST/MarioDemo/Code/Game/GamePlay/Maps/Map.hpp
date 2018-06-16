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
class GeneticAlgorithm;
struct MiniMapObject
{
	Rgba m_color;
};
struct Pit
{
	Vector2 m_position;
	Rgba    m_color;
};
class Pipe;
class Map
{
public:
	bool m_init = false;
	bool gafirstsample = false;
	Camera *m_camera;
	AABB2   m_cameraQuads;
	AABB2   m_block;
	Texture *m_textureBackground;
	Vector2 m_dimensions;
	Vector2 m_miniMapPosition;

	float   m_miniMapScaleFactor = 1 / 5.f;
	std::vector<Brick*> m_bricks;
	std::vector<Pipe*>  m_pipes;
	std::vector<Pit>    m_pits;

	Mario *m_mario;
	//MINIMAP
	AABB2					   m_minimapAABB;
	std::vector<MiniMapObject> m_minimapObjs;
	int m_minimapWidth  = 10;
	int m_minimapHeight = 10;
	GeneticAlgorithm *m_ga = nullptr;
	Map(MapDefinition* def);
	void InitCamera();
	void InitGA();
	void CreatePit(Vector2 position);
	void CreateBricks(Vector2 position,Vector2 dimension,bool hidden,bool physics);
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
	void SetMiniMapValues(IntVector2 pos, Rgba color);
	void UpdateBrick(float deltaTime);
	void UpdatePipes(float deltaTime);

	void Render();
	void RenderMiniMap();
	void RenderMario();
	void RenderBricks();
	void RenderPipes();
	void RenderGrid();
};
