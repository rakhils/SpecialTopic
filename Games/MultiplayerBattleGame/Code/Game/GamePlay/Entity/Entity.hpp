#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Matrix44.hpp"
struct EntitySnapShot_t
{
	Vector2 	m_position;
	float		m_angle;
	float		m_health;
};
enum EntityType
{
	PLAYER,
	BULLET,
	EXPLOSION
};
class NetObject;
class Map;
class Mesh;
class Entity 
{
public:
	Disc2						m_disc2D;
	Vector2						m_position;
	float						m_angle						= 0;
	float						m_health					= 0;

	float						m_width						= 0;
	float						m_height					= 0;
	int							m_teamID					= 0;
	EntityType					m_entityType;

	Mesh *						m_entityMesh;

	uint8_t						m_ownerConnectionIndex;
	uint8_t						m_index;

	Map *						m_map						= nullptr;

	EntitySnapShot_t *			m_localEntitySnapshot		= nullptr;
	EntitySnapShot_t *			m_netObjectSnapshot			= nullptr;
	NetObject		 *			m_netObject					= nullptr;

	Entity(Vector2 position,EntityType type);

	void						Init();

	void						SetNetObject(NetObject *netObject);
	void						SetPosition(Vector2 position);
	void						SetAngle(float angle);
	void						SetTeam(int team);
	void						SetHealth(float health);
	void						SetHeight(float height);
	void						SetWidth(float width);
	void						SetMap(Map *map);

	bool						IsCurrentPlayer();
	void						TakeDamage(float healthReduction);

	void						ApplySnapshot(EntitySnapShot_t *snapShot);

	void						ProcessInputs(float deltaTime);
	void						Update(float deltaTime);
	void						UpdateLocalEntitySnapshot();

	void						Render();

};