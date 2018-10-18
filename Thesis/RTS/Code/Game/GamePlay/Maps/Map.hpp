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

enum MapMode
{
	MAP_MODE_TRAINING_CIVILIAN_GATHER_FOOD,
	MAP_MODE_TRAINING_CIVILIAN_GATHER_ALL_RESOURCES,
	MAP_MODE_TRAINING_CIVILIAN_BUILD,
	MAP_MODE_TRAINING_CIVILIAN,
	MAP_MODE_TRAINING_TOWNCENTER,
	MAP_MODE_TRAINING_SHORTRANGE_ARMY,
	MAP_MODE_TRAINING_LONGRANGE_ARMY,
	MAP_MODE_TRAINING_ARMYSPAWNER,
	MAP_MODE_TRAINING_NONE,
	MAP_MODE_NUM_ITEMS
};
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
struct CellSensoryValues
{
	CellSensoryValues()
	{
		m_coords = IntVector2::MINUS_ONE;
		m_townCenter1Nearness		= 0.f;
		m_townCenter2Nearness		= 0.f;
		m_resourceNearnessForFood	= 0.f;
		m_resourceNearnessForStone	= 0.f;
		m_resourceNearnessForWood   = 0.f;
	}
	IntVector2 m_coords;
	float	   m_townCenter1Nearness;
	float	   m_townCenter2Nearness;
	float      m_resourceNearnessForWood;
	float	   m_resourceNearnessForFood;
	float	   m_resourceNearnessForStone;
};

class Map
{
public:
	bool							m_init = false;
	bool							m_gameFinished = false;
	bool							m_debugSensoryValue = false;
	Camera *						m_camera = nullptr;
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

	std::vector<double>				m_minimapValue;
	std::vector<CellSensoryValues>  m_cellSensoryValues;

	MapMode							m_mapMode = MAP_MODE_TRAINING_NONE;

	int								m_currentTileIndex = 0;
	int								m_maxWidth;
	int								m_maxHeight;
	float							m_xOffset			= g_mapXOffset;
	float							m_yOffset			= g_mapYOffset;
	Vector2							m_mousePosition;

	Map();
	~Map();

	void							Initialize();
	void							InitCellSensoryValues();
	void							InitCamera();

	void							SetMapType(MapMode type);
	void							InitTrainingForCivilianGatherFood();
	void							InitTrainingForCivilianGatherAllResources();
	void							InitTrainingForCivilianBuildAll();
	void							InitTrainingForCivilian();
	void							InitTrainingForTownCenter();
	void							InitTrainingForShortRangeArmy();
	void							InitTrainingForLongRangeArmy();
	void							InitTrainingForArmySpawner();
	void							InitNonTrainingMode();

	bool							IsNonTrainingMode();

	void							CreateCivilian(Vector2 position, int teamID);
	void							CreateArmySpawner(Vector2 position, int teamID);
	void							CreateClassAWarrior(Vector2 position, int teamID);
	void							CreateClassBWarrior(Vector2 position, int teamID);
	void							CreateHouse(Vector2 position, int teamID);
	void							CreateTownCenter(Vector2 position, int teamID);
	void							CreateResources(Vector2 position, EntityType type);
	void							CreateExplosions(Vector2 position);

	bool							IsPositionInsideMap(Vector2 position);
	bool							IsNeighbours(IntVector2 position1, IntVector2 position2);
	bool							IsNeighbours(IntVector2 position1, IntVector2 position2,int distance);
	bool							IsResource(Entity * entity);
	//MINIMAP
	void							InitMiniMap();
	void							UpdateMiniMap();
	double							GetMiniMapValueAtPosition(int row, int column);
	void							SetMiniMapValues(int row, int column, float minimapValue);

	///////////////////////////////////////////////////////////////////////

	// CELL VALUE
	float							NearnessValueToTownCenter(IntVector2 coords,int teamID);
	float							NearnessValueToResources (IntVector2 coords, EntityType type);
	///////////////////////////////////////////////////////////////////////


	IntVector2						GetFreeNeighbourTile(Vector2 position);
	IntVector2						GetFreeNeighbourTile(Vector2 position,int distance);
	IntVector2						GetTilePosition(int tilePosition);
	IntVector2						GetTilePosition(Vector2 position);
	IntVector2						GetRandomNeighbour(IntVector2 cords,int cellDistance);
	std::vector<IntVector2>			GetAllNeighbourCoordinates(IntVector2 cords,int distance);
	IntVector2						ClampCoordinates(IntVector2 cords);
	int								GetTileIndex(Vector2 mapPosition);
	int								GetTileIndex(IntVector2 position);
	int								GetCellDistance(Vector2 position, Vector2 position2);
	int								GetCellDistance(IntVector2 position, IntVector2 position2);

	bool							HasAnyEntityInTile(int index);
	bool							HasAnyEntityInTile(IntVector2 index);
	bool							HasAnyEntityInTile(Vector2 index);

	Entity *						GetEntityFromPosition(int index);
	Entity *						GetEntityFromPosition(IntVector2 index);
	Entity *						GetEntityFromPosition(Vector2 index);
	std::vector<Entity*>			GetAllEntitiesNearLocation(Vector2 mapPosition,int distance);
	std::vector<Entity*>			GetAllResourcesNearLocation(Vector2 mapPosition, int distance);
	std::vector<Entity*>			GetAllTownCentersNearLocation(Vector2 mapPosition, int distance);
	std::vector<Entity*>			GetAllEnemiesNearLocation(int teamID, Vector2 mapPosition, int distance);

	bool							IsValidCordinate(IntVector2 cords);

	Entity *						AttackOnPosition(int tileIndex, float damagePoint);
	Entity *						AttackOnPosition(IntVector2 cords, float damagePoint);
	Entity *						AttackOnPosition(Vector2 cords, float damagePoint);

	bool							IsEnemies(Entity *entityOne, Entity *entityTwo);
	void							DestroyEntity(Entity *entity);
	
	Vector2							GetMapPosition(int tileIndex);
	Vector2							GetMapPosition(IntVector2 tilecords);

	IntVector2						GetCordinates(int tileIndex);
	IntVector2						GetCordinates(Vector2 mapPosition);

	void							ProcessInputs(float deltaTime);
	void							ProcessInputsOnHUD(float deltaTime);
	
	void							CheckAndUpdateOnWinCondition();
	void							CheckAndClearEntityOverlap();

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);
	void							UpdateCivilans(float deltaTime);
	void							UpdateArmySpawners(float deltaTime);
	void							UpdateClassAWarriors(float deltaTime);
	void							UpdateClassBWarriors(float deltaTime);
	void							UpdateHouses(float deltaTime);
	void							UpdateTownCenters(float deltaTime);
	void							UpdateResources(float deltaTime);
	void							UpdateExplosions(float deltaTime);

	void							Render();
	void							RenderCivilians();
	void							RenderArmySpawners();
	void							RenderClassAWarriors();
	void							RenderClassBWarriors();
	void							RenderHouses();
	void							RenderTownCenters();
	void							RenderResources();
	void							RenderGrids();
	void							RenderExplosions();
	void							RenderHUDGameStat();
	void							RenderHUDUnitStat();
	void							RenderUnitTask();

	void							RenderMousePosition();
	void							RenderWinState();

	void							RenderCensoryValues();

	static std::string				GetMapModeAsString(MapMode mode);
};