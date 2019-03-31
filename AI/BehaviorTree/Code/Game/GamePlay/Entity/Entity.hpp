#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
class Map;
class Entity 
{
public:
	Disc2						m_disc2D;
	Vector2						m_position;
	float						m_health					= 0;
	Map *						m_map						= nullptr;

	Entity(Vector2 position);

	void						Init();

	void						SetMap(Map *map);

	void						TakeDamage(float healthReduction);

	void						ProcessInputs(float deltaTime);
	void						Update(float deltaTime);

	void						Render();

};