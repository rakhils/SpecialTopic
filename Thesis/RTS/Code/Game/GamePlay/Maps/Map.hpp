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
#include "Game/GamePlay/Utils/ScoreCard.hpp"
enum Personality
{
	AGGRESSIVE,
	DEFENSIVE
};
enum MapMode
{
	MAP_MODE_AGGRESSIVE_VS_RANDOM,
	MAP_MODE_DEFENSIVE_VS_RANDOM,
	MAP_MODE_AGGRESSIVE_VS_HUMAN,
	MAP_MODE_DEFENSIVE_VS_HUMAN,

	MAP_MODE_TRAINING_AGGRESSIVE,
	MAP_MODE_TRAINING_DEFENSIVE,
	MAP_MODE_NUM_ITEMS
};
struct GameStats
{
	int m_numOfArmySpawnerTeam1			= 0;
	int m_numOfArmySpawnerTeam2			= 0;
	int m_numOfCiviliansTeam1			= 0;
	int m_numOfCiviliansTeam2			= 0;
	int m_numOfShortRangeArmyTeam1		= 0;
	int m_numOfShortRangeArmyTeam2		= 0;
	int m_numOfLongRangeArmyTeam1		= 0;	
	int m_numOfLongRangeArmyTeam2		= 0;
	int m_numOfHousesTeam1				= 0;
	int m_numOfHousesTeam2				= 0;
	int GetTotalEntityCountTeam1() { return m_numOfCiviliansTeam1 + m_numOfLongRangeArmyTeam1 + m_numOfShortRangeArmyTeam1; }
	int GetTotalEntityCountTeam2() { return m_numOfCiviliansTeam2 + m_numOfLongRangeArmyTeam2 + m_numOfShortRangeArmyTeam2; }
	void Reset();
};

struct Tile
{
	Entity *m_entity = nullptr;
	IntVector2 m_position;
	int 	m_covered = 0;
	int     m_teamID = 0;
	Tile()
	{

	}
	Tile(Entity* entity,IntVector2 position)
	{
		m_entity = entity;
		position = m_position;
	}
	void SetCovered(int coveredValue )
	{
		if(m_covered == 0)
		{
			m_covered = coveredValue;
			return;
		}
		if(m_covered != 0 && m_covered != coveredValue)
		{
			m_covered = 3;
		}
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
	
	float							m_gameFinishedTime = 0.f;
	float							m_gameTime		   = 0.f;

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

	std::vector<Entity*>			m_selectedEntities;
	std::vector<Entity*>			m_movableEntities;
	std::vector<Entity*>			m_standAloneEntities;
	std::vector<EntityType>			m_entitiesHavingTraning;

	std::vector<Tile*>				m_tiles;

	GameStats						m_gameStats;
	MapMode							m_mapMode = MAP_MODE_AGGRESSIVE_VS_RANDOM;
	Personality						m_personality;
	std::string						m_folder;

	bool							m_multipleEntitySelectionJustStarted = false;
	bool							m_multipleEntitySelectionJustEnded	   = false;
	Vector2							m_multipleEntitySelectionStartPoint;
	Vector2							m_multipleEntitySelectionEndPoint;

	int								m_currentTileIndex = 0;
	int								m_maxWidth;
	int								m_maxHeight;
	int								m_maxEntityCount	= 20;
	float							m_xOffset			= g_mapXOffset;
	float							m_yOffset			= g_mapYOffset;
	Vector2							m_mousePosition;

	bool							m_isScoreBoardUpdated = false;
	bool							m_firstTime = true;
	int								m_counter = 0;
	float							m_diff = 1.f;

	ScoreCardByActions				m_team1ScoreCard;
	ScoreCardByActions				m_team2ScoreCard;

	ScoreCardByActions				m_localBestCivilianScoreCardTeam1;
	ScoreCardByActions				m_localBestCivilianScoreCardTeam2;
	ScoreCardByActions				m_localBestShortRangeArmyScoreCardTeam1;
	ScoreCardByActions				m_localBestShortRangeArmyScoreCardTeam2;
	ScoreCardByActions				m_localBestLongRangeArmyScoreCardTeam1;
	ScoreCardByActions				m_localBestLongRangeArmyScoreCardTeam2;
	ScoreCardByActions				m_localBestTownCenterScoreCardTeam1;
	ScoreCardByActions				m_localBestTownCenterScoreCardTeam2;

	Map();
	~Map();

	void							Initialize();
	void							InitTiles();
	void							CreateDirectoryForNN();
	void							ResetAllScores();
	void							InitAndStoreBestScoreFromFile();
	void							InitCamera();
	void							RestartMap();

	IntVector2						GetRandomFreePosition(IntVector2 positionStart, IntVector2 positionEnd);

	void							SetMapType(MapMode type);
	void							InitAggressiveVsRandom();
	void							InitDefensiveVsRandom();
	void							InitAggressiveVsHuman();
	void							InitDefensiveVsHuman();
	void							InitTrainingForRandomGenMaps();

	bool							IsNonTrainingMode();
	bool							HasTrainingEnabled(Entity *entity);
	bool							HasFeedForwardEnabled(Entity *entity);
	bool							HasRandomBehaviorEnabled(Entity *entity);
	bool							HasHumanBehaviorEnabled(Entity *entity);

	Entity*							CreateCivilian(Vector2 position, int teamID);
	Entity*							CreateArmySpawner(Vector2 position, int teamID);
	Entity*							CreateClassAWarrior(Vector2 position, int teamID);
	Entity*							CreateClassBWarrior(Vector2 position, int teamID);
	Entity*							CreateHouse(Vector2 position, int teamID);
	Entity*							CreateTownCenter(Vector2 position, int teamID);
	Entity*							CreateResources(Vector2 position, EntityType type);
	Entity*							GetResourceByType(EntityType type);
	void							CreateExplosions(Vector2 position,Rgba color);
	void							CreateDebugLetterEntity(Vector2 position,std::string str,float fontsize,Rgba color);

	bool							IsPositionInsideMap(Vector2 position);
	bool							IsFreeTile(IntVector2 position);
	bool							IsNeighbours(IntVector2 position1, IntVector2 position2);
	bool							IsNeighbours(IntVector2 position1, IntVector2 position2,int distance);
	bool							IsResource(Entity * entity);
	bool							ShouldSaveLocalResultInDirectory();

	IntVector2						GetFreeNeighbourTile(Vector2 position);
	IntVector2						GetFreeNeighbourTile(Vector2 position,int distance);
	IntVector2						GetTilePosition(int tilePosition);
	IntVector2						GetTilePosition(Vector2 position);
	IntVector2						GetRandomNeighbour(IntVector2 cords,int cellDistance);
	std::vector<IntVector2>			GetAllNeighbourCoordinates(IntVector2 cords,int distance);
	std::vector<IntVector2>			GetAllNeighbouringCoordinates(IntVector2 cords, int distance);

	IntVector2						ClampCoordinates(IntVector2 cords);
	Vector2							GetRelativePosition(IntVector2 cords, IntVector2 targetCords,int maxCoverage);

	int								GetTileIndex(Vector2 mapPosition);
	int								GetTileIndex(IntVector2 position);
	int								GetCellDistance(Vector2 position, Vector2 position2);
	int								GetCellDistance(IntVector2 position, IntVector2 position2);

	bool							HasAnyEntityInTile(int index);
	bool							HasAnyEntityInTile(IntVector2 index);
	bool							HasAnyEntityInTile(Vector2 index);
	bool							HasEnoughResourceToSpawnCivilian(int teamID);
	bool							HasEnoughResourceToBuildHouse(int teamID);
	bool							HasEnoughResourceToBuildArmySpawnCenter(int teamID);
	bool							HasEnoughResourceToSpawnShortRangeArmy(int teamID);
	bool							HasEnoughResourceToSpawnLongRangeArmy(int teamID);

	Entity *						GetEntityFromPosition(int index);
	Entity *						GetEntityFromPosition(IntVector2 index);
	Entity *						GetEntityFromPosition(Vector2 index);
	std::vector<Entity*>			GetAllEntitiesNearLocation(Vector2 mapPosition,int distance);
	std::vector<Entity*>			GetAllResourcesNearLocation(Vector2 mapPosition, int distance);
	std::vector<Entity*>			GetAllTownCentersNearLocation(Vector2 mapPosition, int distance);
	std::vector<Entity*>			GetAllEnemiesNearLocation(int teamID, Vector2 mapPosition, int distance);
	std::vector<Entity*>			GetAllAlliesNearLocation (int teamID, Vector2 mapPosition, int distance);

	bool							IsValidCordinate(IntVector2 cords);

	Entity *						AttackOnPosition(int tileIndex,    float damagePoint,int teamID);
	Entity *						AttackOnPosition(IntVector2 cords, float damagePoint,int teamID);
	Entity *						AttackOnPosition(Vector2 cords,    float damagePoint,int teamID);

	bool							AreEnemies(Entity *entityOne, Entity *entityTwo);
	void							DestroyEntity(Entity *entity);
	
	Vector2							GetMapPosition(int tileIndex);
	Vector2							GetMapPosition(IntVector2 tilecords);

	IntVector2						GetCordinates(int tileIndex);
	IntVector2						GetCordinates(Vector2 mapPosition);

	void							ProcessInputs(float deltaTime);
	
	void							CheckAndUpdateOnWinCondition(float deltaTime);
	void							CheckAndUpdateOnRandomMapGen(float deltaTime);

	void							CheckAndSaveBestStats();
	void							CheckAndSaveBestTeamStats();
	void							CheckAndSaveBestEntities();

	void							CheckAndSaveBestEntityNN(EntityType type,int teamID);
	Entity*							FindLocalBestByEntity(EntityType type,int teamID);

	void							SaveEntityBestGlobalScore(Entity *entity);
	void							SaveEntityBestLocalScore(Entity *entity);

	void							CheckAndClearEntityOverlap();

	void							DeleteFromMovableEntityList(Entity *entity);
	void							DeleteFromSelectedEntityList(Entity *entity);
	void							DeleteFromStandAlonEntityList(Entity *entity);

	void							ProcessInputForMultipleSelection(float deltaTime);
	void							SelectAllEntitiesUnderMouseSelection();

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
	void							UpdateEntities(float deltaTime);
	void							UpdateMultipleSelectionEntityTasks(float deltaTime);

	void							Render();
	void							RenderCivilians();
	void							RenderArmySpawners();
	void							RenderClassAWarriors();
	void							RenderClassBWarriors();
	void							RenderHouses();
	void							RenderTownCenters();
	void							RenderResources();
	void							RenderGrids();
	void							RenderTiles();
	void							RenderExplosions();
	void							RenderDebugEntites();
	void							RenderHUDGameStat();
	void							RenderHUDUnitStat();
	void							RenderUnitTask();

	void							RenderMultipleSelectionBox();

	void							RenderMousePosition();
	void							RenderWinState();

	static std::string				GetMapModeAsString(MapMode mode);
};