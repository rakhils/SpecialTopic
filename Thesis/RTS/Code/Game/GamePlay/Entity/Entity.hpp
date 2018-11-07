#include <queue>

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GamePlay/Task/Task.hpp"
#pragma once
enum EntityType
{
	CIVILIAN,
	SHORT_RANGE_ARMY,
	LONG_RANGE_ARMY,
	HOUSE,
	TOWN_CENTER,
	RESOURCE_FOOD,
	RESOURCE_STONE,
	RESOURCE_WOOD,
	ARMY_SPAWNER
};
enum FavoredMoveStats
{
	FAVORED_MOVETO_RESOURCE_FOOD,
	FAVORED_MOVETO_RESOURCE_STONE,
	FAVORED_MOVETO_RESOURCE_WOOD,
	FAVORED_MOVETO_TEAM1_ARMY_SHORT_RANGE,
	FAVORED_MOVETO_TEAM2_ARMY_SHORT_RANGE,
	FAVORED_MOVETO_TEAM1_ARMY_LONG_RANGE,
	FAVORED_MOVETO_TEAM2_ARMY_LONG_RANGE,
	FAVORED_MOVETO_TEAM1_ARMYSPAWNER,
	FAVORED_MOVETO_TEAM2_ARMYSPAWNER,
	FAVORED_MOVETO_TEAM1_BUILDING,
	FAVORED_MOVETO_TEAM2_BUILDING,
	FAVORED_MOVETO_TEAM1_CIVILIAN,
	FAVORED_MOVETO_TEAM2_CIVILIAN,
	FAVORED_MOVETO_TEAM1_TOWNCENTER,
	FAVORED_MOVETO_TEAM2_TOWNCENTER,
};

enum Stats
{
	HEALTH,
	RESOURCE_COUNT,
};
struct EntityScoreBoard
{
public:
	int m_resourceFoodCollected = 0;
	int m_resourceStoneCollected = 0;
	int m_resourceWoodCollected = 0;
	int m_housesBuilt = 0;
	int m_armySpawnersBuilt = 0;
	int m_civiliansSpawned = 0;
	int m_shortRangeArmySpawned = 0;
	int m_longRangeArmySpawned = 0;
	int m_housesAttacked = 0;
	int m_armySpawnerAttacked = 0;
	int m_civiilansAttacked = 0;
	int m_shortRangeArmyAttacked = 0;
	int m_longRangeArmyAttacked = 0;
	int m_townCenterAttacked = 0;
	int m_housesDestroyed = 0;
	int m_armySpawnerDestroyed = 0;
	int m_civiliansKilled = 0;
	int m_shortRangeArmyKilled = 0;
	int m_longRangeArmyKilled = 0;
	int m_townCenterDestroyed = 0;
	int m_totalTimeInSec = 0;
	int m_totalScore = 0;


	int m_resourceFoodCollectedPoints = 1;
	int m_resourceStoneCollectedPoints = 1;
	int m_resourceWoodCollectedPoints = 1;
	int m_housesBuiltPoints = 10;
	int m_armySpawnersBuiltPoints = 16;
	int m_civiliansSpawnedPoints = 20;
	int m_shortRangeArmySpawnedPoints = 30;
	int m_longRangeArmySpawnedPoints = 32;
	int m_housesAttackedPoints = 2;
	int m_armySpawnerAttackedPoints = 3;
	int m_civiilansAttackedPoints = 4;
	int m_shortRangeArmyAttackedPoints = 6;
	int m_longRangeArmyAttackedPoints = 7;
	int m_townCenterAttackedPoints = 12;
	int m_housesDestroyedPoints = 4;
	int m_armySpawnerDestroyedPoints = 6;
	int m_civiliansKilledPoints = 8;
	int m_shortRangeArmyKilledPoints = 12;
	int m_longRangeArmyKilledPoints = 14;
	int m_townCenterDestroyedPoints = 1000;

	void CalculateTotalScore()
	{
		m_totalScore = (m_resourceFoodCollected  *		 m_resourceFoodCollectedPoints +
			m_resourceStoneCollected * m_resourceStoneCollectedPoints +
			m_resourceWoodCollected * m_resourceWoodCollectedPoints +
			m_housesBuilt * m_housesBuiltPoints +
			m_armySpawnersBuilt * m_armySpawnersBuiltPoints +
			m_civiliansSpawned * m_civiliansSpawnedPoints +
			m_shortRangeArmySpawned * m_shortRangeArmySpawnedPoints +
			m_longRangeArmySpawned * m_longRangeArmySpawnedPoints +
			m_housesAttacked * m_housesAttackedPoints +
			m_armySpawnerAttacked * m_armySpawnerAttackedPoints +
			m_civiilansAttacked * m_civiilansAttackedPoints +
			m_shortRangeArmyAttacked * m_shortRangeArmyAttackedPoints +
			m_longRangeArmyAttacked * m_longRangeArmyAttackedPoints +
			m_townCenterAttacked * m_townCenterAttackedPoints +
			m_housesDestroyed * m_housesDestroyedPoints +
			m_armySpawnerDestroyed * m_armySpawnerDestroyedPoints +
			m_civiliansKilled * m_civiliansKilledPoints +
			m_shortRangeArmyKilled * m_shortRangeArmyKilledPoints +
			m_longRangeArmyKilled * m_longRangeArmyKilledPoints +
			m_townCenterDestroyed * m_townCenterDestroyedPoints);
		m_totalScore += m_totalTimeInSec;
	}
	void SaveToFile(char const *fileName)
	{
		FILE *fp = FileOpenForWrite(fileName);
		FileAppendString(fp, ToString(m_totalScore) + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_FOOD_COLLECTED       : " + ToString(m_resourceFoodCollected) + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_STONE_COLLECTED      : " + ToString(m_resourceStoneCollected) + "\n");
		FileAppendString(fp, "TOTAL_RESOURCE_WOOD_COLLECTED       : " + ToString(m_resourceWoodCollected) + "\n");
		FileAppendString(fp, "TOTAL_HOUSES_BUILT                  : " + ToString(m_housesBuilt) + "\n");
		FileAppendString(fp, "TOTAL_ARMY_CENTRE_SPAWNED           : " + ToString(m_armySpawnersBuilt) + "\n");
		FileAppendString(fp, "TOTAL_CIVILIANS_SPAWNED             : " + ToString(m_civiliansSpawned) + "\n");
		FileAppendString(fp, "TOTAL_SHORT_RANGE_ARMY_SPAWNED      : " + ToString(m_shortRangeArmySpawned) + "\n");
		FileAppendString(fp, "TOTAL_LONG_RANGE_ARMY_SPAWNED       : " + ToString(m_longRangeArmySpawned) + "\n");

		FileAppendString(fp, "TOTAL_ATTACK_ON_HOUSES              : " + ToString(m_housesAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_ARMY_SPAWNERS       : " + ToString(m_armySpawnerAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_CIVILIANS           : " + ToString(m_civiilansAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_SHORT_RANGE_ARMIES  : " + ToString(m_shortRangeArmyAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_LONG_RANGE_ARMIES   : " + ToString(m_longRangeArmyAttacked) + "\n");
		FileAppendString(fp, "TOTAL_ATTACK_ON_TOWNCENTERS         : " + ToString(m_townCenterAttacked) + "\n");

		FileAppendString(fp, "TOTAL_KILL_ON_HOUSES                : " + ToString(m_housesDestroyed) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_ARMY_SPAWNERS         : " + ToString(m_armySpawnerDestroyed) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_CIVILIANS             : " + ToString(m_civiliansKilled) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_SHORT_RANGE_ARMIES    : " + ToString(m_shortRangeArmyKilled) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_LONG_RANGE_ARMIES     : " + ToString(m_longRangeArmyKilled) + "\n");
		FileAppendString(fp, "TOTAL_KILL_ON_TOWNCENTERS           : " + ToString(m_townCenterDestroyed) + "\n");

		FileAppendString(fp, "TOTAL_TIME                          : " + ToString(m_totalTimeInSec) + "\n");
		FileAppendString(fp, "TOTAL_SCORE                         : " + ToString(m_totalScore) + "\n");
		FileClose(fp);
	}
};
struct EntityState
{
public:
	void operator=(const EntityState &state)
	{
		m_resourceFoodUsed				= state.m_resourceFoodUsed;
		m_resourceStoneUsed				= state.m_resourceStoneUsed;
		m_resourceWoodUsed				= state.m_resourceWoodUsed;
										
		m_resourceFoodGathered			= state.m_resourceFoodGathered;
		m_resourceFoodDropped			= state.m_resourceFoodDropped;
		m_resourceStoneGathered			= state.m_resourceStoneGathered;
										
		m_resourceStoneDropped			= state.m_resourceStoneDropped;
		m_resourceWoodGathered			= state.m_resourceWoodGathered;
		m_resourceWoodDropped			= state.m_resourceWoodDropped;
										
		m_numberOfArmySpawnerBuilt		= state.m_numberOfArmySpawnerBuilt;
		m_numberOfHouseBuilt			= state.m_numberOfHouseBuilt;
										
		m_villagerSpawned				= state.m_villagerSpawned;
		m_longRangeArmySpawned			= state.m_longRangeArmySpawned;
		m_shortRangeArmySpawned			= state.m_shortRangeArmySpawned;
										
		m_enemiesAttacked				= state.m_enemiesAttacked;
		m_enemiesKilled					= state.m_enemiesKilled;
										
		m_health						= state.m_health;
		m_hasResource					= state.m_hasResource;
		m_position						= state.m_position;

		m_neuralNetPoints				= state.m_neuralNetPoints;

		for(int index = 0;index < m_favoredMoveTaskCount.size();index++)
		{
			m_favoredMoveTaskCount.at(index) = state.m_favoredMoveTaskCount.at(index);
		}
	}

	int		m_resourceFoodUsed			= 0;
	int		m_resourceStoneUsed			= 0;
	int		m_resourceWoodUsed			= 0;

	int		m_resourceFoodGathered		= 0;
	int		m_resourceFoodDropped		= 0;
	int		m_resourceStoneGathered		= 0;

	int		m_resourceStoneDropped		= 0;
	int		m_resourceWoodGathered		= 0;
	int		m_resourceWoodDropped		= 0;

	int		m_numberOfArmySpawnerBuilt	= 0;
	int		m_numberOfHouseBuilt		= 0;

	int		m_villagerSpawned			= 0;
	int		m_longRangeArmySpawned		= 0;
	int		m_shortRangeArmySpawned		= 0;
		
	int		m_enemiesAttacked			= 0;
	int		m_enemiesKilled				= 0;

	float	m_health					= 0.f;
	bool	m_hasResource				= false;

	std::vector<float> m_favoredMoveTaskCount;

	/*int   m_numTimesFavoredMoveToResource	     = 0;
											     
	int   m_numTimesFavoredMoveToEnemyCivilian   = 0;
	int   m_numTimesFavoredMoveToEnemyBuilding   = 0;
	int   m_numTimesFavoredMoveToEnemyArmy       = 0;
	int   m_numTimesFavoredMoveToEnemyTownCenter = 0;

	int   m_numTimesFavoredMoveToAllyCivilian    = 0;
	int   m_numTimesFavoredMoveToAllyBuilding    = 0;
	int   m_numTimesFavoredMoveToAllyArmy		 = 0;
	int   m_numTimesFavoredMoveToAllyTownCenter  = 0;*/

	int		m_neuralNetPoints			= 0;
	Vector2	m_position;

};
class Map;
class Entity
{
public:
	Disc2						m_disc;
	AABB2						m_aabb2;
	Entity *					m_resourceTypeCarrying = nullptr;

	bool						m_isSelected			= false;
	EntityType					m_type					= CIVILIAN;
	EntityState					m_state;
	EntityState					m_previousState;
	int							m_teamID				= 0;
	Map *						m_map					= nullptr;
	float						m_health				= 10;
	float						m_neuralNetTrainCount	= 0;
	NeuralNetwork				m_neuralNet;
	float						m_lastDebug				= 0;
	float						m_debugPrintDelay		= 1;
	bool						m_miniMapDebug			= false;

	Rgba						m_color;
	std::queue<Task*>			m_taskQueue;
	std::vector<TaskType>		m_taskTypeSupported;
	std::vector<Stats>			m_statsSupported;

	std::vector<double>			 m_desiredOuputs;

	IntVector2					m_minSafeArea;
	IntVector2					m_maxSafeArea;
	IntVector2					m_minTeritaryArea;
	IntVector2					m_maxTeritaryArea;

	EntityScoreBoard			m_scoreBoard;

	Entity();
	Entity(float x,float y);

	Vector2							GetPosition();
	IntVector2						GetCordinates();
	int								GetTileIndex();
	Rgba							GetTeamColor();
	int								GetIndexOfMoveXPosition();
	int								GetIndexOfMoveYPosition();
	float							GetMiniMapValue();
	double							GetTaskValueFromNNOutput(TaskType type);
	double							GetTaskValueFromDesiredOutput(TaskType type);
	TaskType						GetTaskFromNNOutput(double &max);
	IntVector2						GetTaskPositonFromNNOutput(Vector2 prevPosition);
	IntVector2						GetRelativeCellLocation(float x, float y);
	IntVector2						GetRandomSafeArea();
	IntVector2						GetRandomTeritaryArea();

	std::string						GetGlobalBestFilePath();
	std::string						GetLocalBestFilePath();
	std::string						GetGlobalBestStatFilePath();
	std::string						GetLocalBestStatFilePath();
	
	bool							IsStationaryEntity();
	bool							HasResource();

	int								GetIndexOfTaskInNN(TaskType type);
	Entity*							FindMyTownCenter();
	std::vector<Entity*>			GetAllEntitiesNearMe(int cellDistance);

	std::vector<Entity*>			GetResourceEntityFromList(std::vector<Entity*> &list);
	std::vector<Entity*>			GetTownCenterEntityFromList(std::vector<Entity*> &list);
	std::vector<Entity*>			GetMyTownCenterEntityFromList(std::vector<Entity*> &list);
	
	IntVector2						GetBestNeighbour();

	void							InitNeuralNet();
	void							InitStates();
	void							ProcessInputs(float deltaTime);
	std::vector<double>				GetMyMiniMap();

	void							Update(float deltaTime);
	void							UpdateTask();
	virtual void					EvaluateNN(Task *task,EntityState previousState,IntVector2 cords);
	virtual void					TrainNN(Task *task);
	void							UpdateNN(float deltaTime);
	void							UpdateTaskFromNN(float deltaTime);
	void							UpdateEntityState();
	void							PrintDebugNN();
	void							Render();

	void							UpdateUnitStatForFoodGathered(int count);
	void							UpdateUnitStatForStoneGathered(int count);
	void							UpdateUnitStatForWoodGathered(int count);
	void							UpdateUnitStatForFoodDropped(int count);
	void							UpdateUnitStatForStoneDropped(int count);
	void							UpdateUnitStatForWoodDropped(int count);
	void							UpdateUnitStatForArmySpawnerBuilt(int count);
	void							UpdateUnitStatForHouseBuilt(int count);
	void							UpdateUnitStatForShortRangeArmySpawned(int count);
	void							UpdateUnitStatForLongRangeArmySpawned(int count);
	void							UpdateUnitStatForEnemiesAttacked(Entity* attackedEntity,int count);
	void							UpdateUnitStatForEnemiesKilled(Entity* attackedEntity,int count);
	void							UpdateUnitStatForVillagerSpawned(int count);

	void							UpdateUnitStatForFoodUsed(int count);
	void							UpdateUnitStatForStoneUsed(int count);
	void							UpdateUnitStatForWoodUsed(int count);

	void							UpdateMostFavoredMoveTask(EntityState prevState,IntVector2 cords);
	int 							GetMostFavoredMoveTask(float *maxValue);
	std::vector<FavoredMoveStats>   GetPastFavoredMoveTaskOrderer();
	bool							IsCurrentMoveFavoredByPastMoveTask(EntityState prevState,IntVector2 cords);

	void							SetTeam(int team);
	void							SetPosition(Vector2 position);
	void							SetPosition(int index);
	void							SetPositionInFloat(Vector2 position);

	void							SetDesiredOutputToMoveToNeighbour(EntityState prevState,int cellDistance);
	void							SetDesiredOutputToMoveToNeighbourOpposite(int cellDistance,IntVector2 cords);
	void							SetDesiredOutputToMoveToPrevPosition();
	void							SetDesiredOutputToChooseRandomNeighbourLocation(int cellDistance);

	void							ClearTaskQueue();
	void							CopyDesiredOutputs();
	void							ClearDesiredOutputs();
	void							SetDesiredOutputForTask(TaskType type,float value);

	void							TakeDamage(float hitPoint);

	bool							IsPositionInside(Vector2 position);
	bool							IsMovalbleObject();
	bool							IsDifferentFaction(Entity *entity);
	bool							IsResourceNearMe(int cellDistance);

	bool							CreateAndPushIdleTask(IntVector2 cordinate);
	bool							CreateAndPushMoveTask(IntVector2 cordinate);
	bool							CreateAndPushBuildHouseTask(IntVector2 cordinate);
	bool							CreateAndPushGatherResourceTask(IntVector2 cordinate);
	bool							CreateAndPushDropResourceTask(IntVector2 cordinate);
	bool							CreateAndPushBuildTownCenterTask(IntVector2 cordinate);
	bool							CreateAndPushBuildArmySpawnerTask(IntVector2 cordinate);
	bool							CreateAndPushLongRangeAttackTask(IntVector2 cordinate);
	bool							CreateAndPushShortRangeAttackTask(IntVector2 cordinate);
	bool							CreateAndPushSpawnVillagerTask(IntVector2 cordinate);
	bool							CreateAndPushSpawnClassAArmyTask(IntVector2 cordinate);
	bool							CreateAndPushSpawnClassBArmyTask(IntVector2 cordinate);

	static std::string				GetEntityTypeAsString(EntityType entityType);
	static std::string				GetFavoredMoveToAsString(FavoredMoveStats stats);
};