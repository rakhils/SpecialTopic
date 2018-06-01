#pragma once
#include <string>
#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Game/GameCommon.hpp"
class MapDefinition;
class GameObject;
class DebugDraw;
class Map
{
public:
	Vector2 mins;
	Vector2 maxs;
	DebugDraw *debugDraw = nullptr;
	Map();
	std::vector<GameObject*> m_mapObjects;

	void AddToMapObjects(GameObject *gameObject);
	void RemoveFromMapObject(GameObject *gameObject);

	void Update(float deltaTime);
	void Render();
};