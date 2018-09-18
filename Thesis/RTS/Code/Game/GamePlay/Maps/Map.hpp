#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/ArmySpawner.hpp"
#include "Game/GamePlay/Entity/Civilian.hpp"
#include "Game/GamePlay/Entity/ClassAWarrior.hpp"
#include "Game/GamePlay/Entity/ClassBWarrior.hpp"
#include "Game/GamePlay/Entity/House.hpp"
#include "Game/GamePlay/Entity/TownCenter.hpp"
#include "Game/GamePlay/Entity/Resource.hpp"
#include "Game/GamePlay/Utils/Explosion.hpp"


struct Tile
{
	Entity *m_entity = nullptr;
	IntVector2 m_position;
	Tile(Entity* entity,IntVector2 position)
	{
		m_entity = entity;
		position = m_position;
	}
};
class Map
{
public:
	bool							m_init = false;
	bool							m_gameFinished = false;
	Camera *						m_camera;
	std::vector<ArmySpawner*>	    m_armySpawners;
	std::vector<Civilian*>			m_civilians;
	std::vector<ClassAWarrior*>		m_classAWarriors;
	std::vector<ClassBWarrior*>		m_classBWarriors;
	std::vector<House*>				m_houses;
	std::vector<TownCenter*>		m_townCenters;
	std::vector<Resource*>			m_resources;

	std::vector<Explosion*>			m_explosions;

	std::vector<Entity*>			m_movableEntities;
	std::vector<Entity*>			m_standAloneEntities;

	int								m_currentTileIndex = 0;
	int								m_maxWidth;
	int								m_maxHeight;
	float							m_xOffset;
	float							m_yOffset;

	Map();

	void		Initialize();
	void		InitCamera();

	void		CreateCivilian(Vector2 position, int teamID);
	void		CreateArmySpawner(Vector2 position, int teamID);
	void		CreateClassAWarrior(Vector2 position, int teamID);
	void		CreateClassBWarrior(Vector2 position, int teamID);
	void		CreateHouse(Vector2 position, int teamID);
	void		CreateTownCenter(Vector2 position, int teamID);
	void		CreateResources(Vector2 position, EntityType type);
	void		CreateExplosions(Vector2 position);

	bool        IsPositionInsideMap(Vector2 position);
	bool        IsNeighbours(IntVector2 position1, IntVector2 position2);
	IntVector2  GetFreeNeighbourTile(Vector2 position);
	IntVector2  GetTilePosition(int tilePosition);
	IntVector2  GetTilePosition(Vector2 position);
	int		    GetTileIndex(Vector2 mapPosition);
	int		    GetTileIndex(IntVector2 position);

	bool	    HasAnyEntityInTile(int index);
	bool        HasAnyEntityInTile(IntVector2 index);
	bool	    HasAnyEntityInTile(Vector2 index);

	Entity *    GetEntityFromPosition(int index);
	Entity *    GetEntityFromPosition(IntVector2 index);
	Entity *    GetEntityFromPosition(Vector2 index);

	bool	    IsValidCordinate(IntVector2 cords);

	void		AttackOnPosition(int tileIndex   ,float damagePoint);
	void		AttackOnPosition(IntVector2 cords,float damagePoint);
	void		AttackOnPosition(Vector2 cords   ,float damagePoint);

	bool		IsEnemies(Entity *entityOne, Entity *entityTwo);
	void		DestroyEntity(Entity *entity);
	
	Vector2     GetMapPosition(int tileIndex);
	Vector2	    GetMapPosition(IntVector2 tilecords);

	IntVector2  GetCordinates(int tileIndex);
	IntVector2  GetCordinates(Vector2 mapPosition);

	void		ProcessInputs(float deltaTime);
	void		ProcessInputsOnHUD(float deltaTime);
	
	void		CheckAndUpdateOnWinCondition();
	void		CheckAndClearEntityOverlap();

	void		Update(float deltaTime);
	void		UpdateCamera(float deltaTime);
	void		UpdateCivilans(float deltaTime);
	void        UpdateArmySpawners(float deltaTime);
	void		UpdateClassAWarriors(float deltaTime);
	void		UpdateClassBWarriors(float deltaTime);
	void		UpdateHouses(float deltaTime);
	void		UpdateTownCenters(float deltaTime);
	void		UpdateResources(float deltaTime);
	void		UpdateExplosions(float deltaTime);

	void		Render();
	void		RenderCivilians();
	void        RenderArmySpawners();
	void		RenderClassAWarriors();
	void		RenderClassBWarriors();
	void		RenderHouses();
	void		RenderTownCenters();
	void		RenderResources();
	void		RenderGrids();
	void		RenderExplosions();
	void		RenderHUDGameStat();
	void		RenderHUDUnitStat();

	void		RenderMousePosition();
	void		RenderWinState();
};