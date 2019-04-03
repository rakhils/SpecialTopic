#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
class Map;
class Entity 
{
public:
	Map *						m_map						= nullptr;
	Vector2						m_position;
	Vector2						m_forward;
	float						m_angle;
	float						m_scale						= 25;

	Vector2						m_velocity;
	Vector2						m_acceleration;
	float						m_maxSpeed					= 40.f;
	float						m_maxForce					= .2f;

	Entity();
	Entity(Vector2 position);

	void						Init();

	void						SetMap(Map *map);

	void						ProcessInputs(float deltaTime);
	void						Update(float deltaTime);

	void						ApplyForce(Vector2 force);

	void						Render();

};