#pragma once
#include <string>
#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Game/GameCommon.hpp"
class MapDefinition;
class GameObject;
class DebugDraw;
/*
class GameMapChunk
{
public:
	GameMapChunk()
		: m_chunk_index(Vector2::ZERO)
		, m_renderable(nullptr)
	{}

	~GameMapChunk() 
	{
		CleanUp();
	}

	void Setup(Map *map, int idx);
	void CleanUp();
	void UpdateMesh();

	Vector3 GenerateTerrain(Vector2);


public:
	Map*		 m_map			= nullptr;
	Vector2		 m_chunk_index;
	Renderable*  m_renderable	= nullptr;
};*/
class Scene;
class Terrain;
class Map
{
public:
	Vector2 mins;
	Vector2 maxs;
	DebugDraw *debugDraw = nullptr;
	Map();
	std::vector<GameObject*> m_mapObjects;
	Terrain *m_terrain = nullptr;
	AABB2 m_extents;
	float m_min_height;
	float m_max_height;

	Image *m_image;

	//GameMapChunk *m_chunks;
	Vector2 m_chunk_counts;

	void LoadFromImage(Scene *scene,Image *image,AABB2 const &extents,float min_height,float max_height,Vector2 chunk_counts);
	void FreeAllChunks();
	float GetHeigthAtIndex(int index);
	float GetHeight(Vector2 xz);

	AABB2 GetChunkExtents(Vector2 chunk_idx);

	void AddToMapObjects(GameObject *gameObject);
	void RemoveFromMapObject(GameObject *gameObject);

	void Update(float deltaTime);
	void Render();
};