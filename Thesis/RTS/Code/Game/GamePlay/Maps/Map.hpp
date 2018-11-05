#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/ArmySpawner.hpp"
#include "Game/GamePlay/Entity/Civilian.hpp"
#include "Game/GamePlay/Entity/ClassAWarrior.hpp"
#include "Game/GamePlay/Entity/ClassBWarrior.hpp"
#include "Game/GamePlay/Entity/House.hpp"
#include "Game/GamePlay/Entity/TownCenter.hpp"
#include "Game/GamePlay/Entity/Resource.hpp"
#include "Game/GamePlay/Utils/Explosion.hpp"
#include "Game/GamePlay/Entity/DebugEntity.hpp"

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
struct ScoreBoard
{
public:
	int m_resourceFoodCollected     = 0;
	int m_resourceStoneCollected    = 0;
	int m_resourceWoodCollected     = 0;
	int m_housesBuilt				= 0;
	int m_armySpawnersBuilt    	    = 0;
	int m_civiliansSpawned			= 0;
	int m_shortRangeArmySpawned     = 0;
	int m_longRangeArmySpawned      = 0;
	int m_housesAttacked			= 0;
	int m_armySpawnerAttacked		= 0;
	int m_civiilansAttacked			= 0;
	int m_shortRangeArmyAttacked	= 0;
	int m_longRangeArmyAttacked		= 0;
	int m_townCenterAttacked		= 0;
	int m_housesDestroyed			= 0;
	int m_armySpawnerDestroyed      = 0;
	int m_civiliansKilled			= 0;
	int m_shortRangeArmyKilled		= 0;
	int m_longRangeArmyKilled		= 0;
	int m_townCenterDestroyed		= 0;
	int m_totalTimeSec				= 0;
	int m_totalScore				= 0;


	int m_resourceFoodCollectedPoints	= 1;
	int m_resourceStoneCollectedPoints	= 1;
	int m_resourceWoodCollectedPoints	= 1;
	int m_housesBuiltPoints				= 10;
	int m_armySpawnersBuiltPoints		= 16;
	int m_civiliansSpawnedPoints		= 20;
	int m_shortRangeArmySpawnedPoints	= 30;
	int m_longRangeArmySpawnedPoints	= 32;
	int m_housesAttackedPoints			= 2;
	int m_armySpawnerAttackedPoints		= 3;
	int m_civiilansAttackedPoints		= 4;
	int m_shortRangeArmyAttackedPoints  = 6;
	int m_longRangeArmyAttackedPoints	= 7;
	int m_townCenterAttackedPoints		= 12;
	int m_housesDestroyedPoints			= 4;
	int m_armySpawnerDestroyedPoints	= 6;
	int m_civiliansKilledPoints			= 8;
	int m_shortRangeArmyKilledPoints	= 12;
	int m_longRangeArmyKilledPoints		= 14;
	int m_townCenterDestroyedPoints		= 1000;

	void CalculateTotalScore()
	{
		m_totalScore = (m_resourceFoodCollected  *		 m_resourceFoodCollectedPoints      +
			            m_resourceStoneCollected *       m_resourceStoneCollectedPoints     +
			            m_resourceWoodCollected  *       m_resourceWoodCollectedPoints      +
			            m_housesBuilt			 *       m_housesBuiltPoints			    +
			            m_armySpawnersBuilt		 *       m_armySpawnersBuiltPoints		    +
			            m_civiliansSpawned		 *       m_civiliansSpawnedPoints		    +
			            m_shortRangeArmySpawned	 *       m_shortRangeArmySpawnedPoints      +
			            m_longRangeArmySpawned	 *       m_longRangeArmySpawnedPoints       +
			            m_housesAttacked		 *       m_housesAttackedPoints			    +
			            m_armySpawnerAttacked	 *       m_armySpawnerAttackedPoints	    +
			            m_civiilansAttacked		 *       m_civiilansAttackedPoints		    +
			            m_shortRangeArmyAttacked *       m_shortRangeArmyAttackedPoints     +
			            m_longRangeArmyAttacked	 *       m_longRangeArmyAttackedPoints      +
						m_townCenterAttacked	 *		 m_townCenterAttackedPoints			+
			            m_housesDestroyed		 *       m_housesDestroyedPoints		    +
			            m_armySpawnerDestroyed	 *       m_armySpawnerDestroyedPoints	    +
			            m_civiliansKilled		 *       m_civiliansKilledPoints		    +
			            m_shortRangeArmyKilled	 *       m_shortRangeArmyKilledPoints	    +
			            m_longRangeArmyKilled	 *       m_longRangeArmyKilledPoints	    +
			            m_townCenterDestroyed	 *		 m_townCenterDestroyedPoints);
		m_totalScore += m_totalTimeSec;
	}
	void SaveToFile(char const *fileName)
	{
		FILE *fp = FileOpenForWrite(fileName);
		FileAppendString(fp, ToString(m_totalScore)+  "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_FOOD_COLLECTED       : " + ToString(m_resourceFoodCollected)   + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_STONE_COLLECTED      : " + ToString(m_resourceStoneCollected)  + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_WOOD_COLLECTED       : " + ToString(m_resourceWoodCollected)   + "\n");
		FileAppendString(fp, "TOTAL_HOUSES_BUILT                  : " + ToString(m_housesBuilt)			    + "\n");
		FileAppendString(fp, "TOTAL_ARMY_CENTRE_SPAWNED           : " + ToString(m_armySpawnersBuilt)		+ "\n");
		FileAppendString(fp, "TOTAL_CIVILIANS_SPAWNED             : " + ToString(m_civiliansSpawned)		+ "\n");
		FileAppendString(fp, "TOTAL_SHORT_RANGE_ARMY_SPAWNED      : " + ToString(m_shortRangeArmySpawned)	+ "\n");
		FileAppendString(fp, "TOTAL_LONG_RANGE_ARMY_SPAWNED       : " + ToString(m_longRangeArmySpawned)	+ "\n");

		FileAppendString(fp, "TOTAL_ATTACK_ON_HOUSES              : " + ToString(m_housesAttacked)			+ "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_ARMY_SPAWNERS       : " + ToString(m_armySpawnerAttacked)		+ "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_CIVILIANS           : " + ToString(m_civiilansAttacked)		+ "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_SHORT_RANGE_ARMIES  : " + ToString(m_shortRangeArmyAttacked)  + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_LONG_RANGE_ARMIES   : " + ToString(m_longRangeArmyAttacked)	+ "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_TOWNCENTERS         : " + ToString(m_townCenterAttacked)		+ "\n");

		FileAppendString(fp, "TOTAL_KILL_ON_HOUSES                : " + ToString(m_housesDestroyed)			+ "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_ARMY_SPAWNERS         : " + ToString(m_armySpawnerDestroyed)	+ "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_CIVILIANS             : " + ToString(m_civiliansKilled)			+ "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_SHORT_RANGE_ARMIES    : " + ToString(m_shortRangeArmyKilled)	+ "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_LONG_RANGE_ARMIES     : " + ToString(m_longRangeArmyKilled)		+ "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_TOWNCENTERS           : " + ToString(m_townCenterDestroyed)		+ "\n");

		FileAppendString(fp, "TOTAL_TIME                          : " + ToString(m_totalTimeSec)				+ "\n");
		FileAppendString(fp, "TOTAL_SCORE                         : " + ToString(m_totalScore)				+ "\n");
		FileClose(fp);
	}

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
		m_coords					= IntVector2::MINUS_ONE;

		for(int index = 0;index < 15;index++)
		{
			m_entityNearness.push_back(0.f);
		}
	}
	IntVector2 m_coords;
	std::vector<float> m_entityNearness;
};

class Map
{
public:
	bool							m_init = false;
	bool							m_gameFinished = false;

	bool							m_displaySensoryFoodValue = false;
	bool							m_displaySensoryStoneValue = false;
	bool							m_displaySensoryWoodValue = false;
	bool							m_displaySensoryShortRangeArmy1Value = false;
	bool							m_displaySensoryShortRangeArmy2Value = false;
	bool							m_displaySensoryLongRangeArmy1Value = false;
	bool							m_displaySensoryLongRangeArmy2Value = false;
	bool							m_displaySensoryBuilding1Value = false;
	bool							m_displaySensoryBuilding2Value = false;
	bool							m_displaySensoryArmySpawner1Value = false;
	bool							m_displaySensoryArmySpawner2Value = false;
	bool							m_displaySensoryCivilian1Value = false;
	bool							m_displaySensoryCivilian2Value = false;
	bool							m_displaySensoryTC1Value = false;
	bool							m_displaySensoryTC2Value = false;

	Camera *						m_camera = nullptr;
	std::vector<ArmySpawner*>	    m_armySpawners;
	std::vector<Civilian*>			m_civilians;
	std::vector<ClassAWarrior*>		m_classAWarriors;
	std::vector<ClassBWarrior*>		m_classBWarriors;
	std::vector<House*>				m_houses;
	std::vector<TownCenter*>		m_townCenters;
	std::vector<Resource*>			m_resources;

	std::vector<Explosion*>			m_explosions;
	std::vector<DebugEntity*>       m_debugEntities;

	std::vector<Entity*>			m_movableEntities;
	std::vector<Entity*>			m_standAloneEntities;
	std::vector<EntityType>			m_entitiesHavingTraning;

	std::vector<double>				m_minimapValue;
	std::vector<CellSensoryValues>  m_cellSensoryValues;

	MapMode							m_mapMode = MAP_MODE_TRAINING_NONE;

	int								m_currentTileIndex = 0;
	int								m_maxWidth;
	int								m_maxHeight;
	float							m_xOffset			= g_mapXOffset;
	float							m_yOffset			= g_mapYOffset;
	Vector2							m_mousePosition;

	ScoreBoard						m_team1;
	ScoreBoard						m_team2;
	bool							m_isScoreBoardUpdated = false;

	Map();
	~Map();

	void							Initialize();
	void							InitCellSensoryValues();
	float							GetHeatMapDistanceFromEntity(IntVector2 cellposition, EntityType type,int teamID);
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
	bool							HasTrainingEnabled(Entity *entity);

	void							CreateCivilian(Vector2 position, int teamID);
	void							CreateArmySpawner(Vector2 position, int teamID);
	void							CreateClassAWarrior(Vector2 position, int teamID);
	void							CreateClassBWarrior(Vector2 position, int teamID);
	void							CreateHouse(Vector2 position, int teamID);
	void							CreateTownCenter(Vector2 position, int teamID);
	void							CreateResources(Vector2 position, EntityType type);
	void							CreateExplosions(Vector2 position,Rgba color);
	void							CreateDebugLetterEntity(Vector2 position,std::string str,float fontsize,Rgba color);

	bool							IsPositionInsideMap(Vector2 position);
	bool							IsFreeTile(IntVector2 position);
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
	void							UpdateCellSensoryValues();
	///////////////////////////////////////////////////////////////////////

	ScoreBoard &					GetMyScoreBoard(Entity *entity);
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
	void							CheckAndSaveBestStats();
	void							CheckAndSaveBestNNByType(bool saveTeam1,bool saveTeam2);
	void							SaveCurrentGamePlayNN();

	void							CheckAndClearEntityOverlap();

	void							DeleteFromMovableEntityList(Entity *entity);
	void							DeleteFromStandAlonEntityList(Entity *entity);

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
	void							UpdateDebugEntities(float deltaTime);

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
	void							RenderDebugEntites();
	void							RenderHUDGameStat();
	void							RenderHUDUnitStat();
	void							RenderUnitTask();

	void							RenderMousePosition();
	void							RenderWinState();

	void							RenderCensoryValues();

	static std::string				GetMapModeAsString(MapMode mode);
};