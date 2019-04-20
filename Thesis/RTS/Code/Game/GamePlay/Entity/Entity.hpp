#include <queue>

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GamePlay/Task/Task.hpp"
#include "Game/GamePlay/Utils/ScoreCard.hpp"
#pragma once

enum Strategy
{
	ATTACK,
	EXPLORE,
	FOLLOW,
	PATROL,
	RETREAT,
	IDLE,
	MAX_NUM_STRATEGY
};
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
										
		m_civilianSpawned				= state.m_civilianSpawned;
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

	int		m_civilianSpawned			= 0;
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
enum NNInputEnums
{
	NNInput_allyArmySpawnerCount,
	NNInput_allyCiviliansCount,
	NNInput_allyLongRangeArmyCount,
	NNInput_allyShortRangeArmyCount,
	NNInput_allyHouseCount,
	NNInput_allyTownCenterHealth,
	NNInput_allyTownCenterHealthLastFrame,

	NNInput_enemyArmySpawnerCount,
	NNInput_enemyCiviliansCount,
	NNInput_enemyHouseCount,
	NNInput_enemyLongRangeArmyCount,
	NNInput_enemyShortRangeArmyCount,
	NNInput_enemyTownCenterHealth,
	NNInput_enemyTownCenterHealthLastFrame,

	NNInput_resourceFoodCount,
	NNInput_resourceStoneCount,
	NNInput_resourceWoodCount,

	NNInput_inRangeAllyArmySpawnerCount,
	NNInput_inRangeAllyCiviliansCount,
	NNInput_inRangeAllyLongRangeArmyCount,
	NNInput_inRangeAllyShortRangeArmyCount,
	NNInput_inRangeAllyHouseCount,
	NNInput_inRangeAllyTownCenterCount,
	NNInput_inRangeAttackingAllyHeatMapValue,
	NNInput_inRangeStationaryAllyHeatMapValue,

	NNInput_inRangeEnemyArmySpawnerCount,
	NNInput_inRangeEnemyCiviliansCount,
	NNInput_inRangeEnemyHouseCount,
	NNInput_inRangeEnemyLongRangeArmyCount,
	NNInput_inRangeEnemyShortRangeArmyCount,
	NNInput_inRangeEnemyTownCenterCount,
	NNInput_inRangeAttackingEnemyHeatMapValue,
	NNInput_inRangeStationaryEnemyHeatMapValue,

	NNInput_allyInAttackMode,
	NNInput_allyInExploreMode,
	NNInput_allyInFollowMode,
	NNInput_allyInPatrolMode,
	NNInput_allyInRetreatMode,

	NNInput_health,
	NNInput_healthLastFrame,
	NNInput_resourceCarrying,
	NNInput_InputMaxCount,
};
struct NNInputs
{
	double m_allyArmySpawnerCount;
	double m_allyCiviliansCount;
	double m_allyLongRangeArmyCount;
	double m_allyShortRangeArmyCount;
	double m_allyHouseCount;
	double m_allyTownCenterHealth;
	double m_allyTownCenterHealthLastFrame;

	//
	double m_enemyArmySpawnerCount;
	double m_enemyCiviliansCount;
	double m_enemyHouseCount;
	double m_enemyLongRangeArmyCount;
	double m_enemyShortRangeArmyCount;
	double m_enemyTownCenterHealth;
	double m_enemyTownCenterHealthLastFrame;

	double m_resourceFoodCount;
	double m_resourceStoneCount;
	double m_resourceWoodCount;
	//15 count till now
	// IN RANGE
	double m_inRangeAllyArmySpawnerCount;
	double m_inRangeAllyCiviliansCount;
	double m_inRangeAllyLongRangeArmyCount;
	double m_inRangeAllyShortRangeArmyCount;
	double m_inRangeAllyHouseCount;
	double m_inRangeAllyTownCenterCount;
	double m_inRangeAttackingAllyHeatMapValue;
	double m_inRangeStationaryAllyHeatMapValue;

	double m_inRangeEnemyArmySpawnerCount;
	double m_inRangeEnemyCiviliansCount;
	double m_inRangeEnemyHouseCount;
	double m_inRangeEnemyLongRangeArmyCount;
	double m_inRangeEnemyShortRangeArmyCount;
	double m_inRangeEnemyTownCenterCount;
	double m_inRangeAttackingEnemyHeatMapValue;
	double m_inRangeStationaryEnemyHeatMapValue;

	double m_allyInAttackMode ;
	double m_allyInExploreMode;
	double m_allyInFollowMode ;
	double m_allyInPatrolMode ;
	double m_allyInRetreatMode;

	double m_health;
	double m_healthLastFrame;
	double m_resourceCarrying;
};

typedef bool (*FunctionStrategy)(std::vector<double>&, Strategy &, NNInputs);

class Map;
class Entity
{
public:
	Disc2						m_disc;
	AABB2						m_aabb2;
	bool						m_isSelected			= false;
	EntityType					m_type					= CIVILIAN;
	EntityState					m_state;
	EntityState					m_previousState;
	Entity *					m_resourceTypeCarrying  = nullptr;

	int							m_teamID				= 0;
	int							m_attackRange			= 0;
	Map *						m_map					= nullptr;
	float						m_health				= 10;
	float						m_healthLastFrame		= 10;
	float						m_neuralNetTrainCount	= 0;
	NeuralNetwork				m_neuralNet;
	float						m_lastDebug				= 0;
	float						m_debugPrintDelay		= 1;
	bool						m_miniMapDebug			= false;
	float						m_lastAttackTime		= 0.f;
	float						m_townCentreHealthLastFrame      = 0.f;

	Rgba						m_color;
	std::queue<Task*>			m_taskQueue;
	std::vector<TaskType>		m_taskTypeSupported;

	std::vector<double>			 m_desiredOuputs;
	std::vector<double>			 m_desiredStrategyValues;

	IntVector2					m_minSafeArea;
	IntVector2					m_maxSafeArea;
	IntVector2					m_minTeritaryArea;
	IntVector2					m_maxTeritaryArea;

	ScoreCardByActions			m_scoreBoard;

	Strategy					m_desiredStrategy;
	Strategy					m_currentStrategy;

	std::vector<IntVector2>     m_exploredPlacesOfNoInterest;

	std::vector<FunctionStrategy> m_strategies;

	Entity();
	Entity(float x,float y);
	~Entity();

	Vector2							GetPosition();
	IntVector2						GetCordinates();
	int								GetTileIndex();
	Rgba							GetTeamColor();
	int								GetIndexOfMoveXPosition();
	int								GetIndexOfMoveYPosition();
	float							GetMiniMapValue();
	double							GetTaskValueFromNNOutput(TaskType type);
	double							GetTaskValueFromDesiredOutput(TaskType type);
	virtual TaskType				GetTaskFromNNOutput(double &max);
	IntVector2						GetTaskPositonFromNNOutput(Vector2 prevPosition);
	Vector2							GetPredictedNNOutputFromMapPosition(IntVector2 entityCords,IntVector2 mapCords,int width,int height);
	IntVector2						GetTaskPositonFromNNOutput(Vector2 prevPosition,int width,int height);
	IntVector2						GetTaskPositonFromNNOutput(Vector2 prevPosition,Vector2 nnOutput, int width, int height);
	IntVector2						GetRelativeCellLocation(float x, float y);
	IntVector2						GetRandomSafeArea();
	IntVector2						GetRandomTeritaryArea();
	IntVector2						GetMiniMapMins();
	IntVector2						GetMiniMapMaxs();
	Task *							GetRandomTaskByType(TaskType type);
	TaskType						GetMyCurrentTask();

	std::string						GetGlobalBestFilePath();
	std::string						GetLocalBestFilePath();
	std::string						GetGlobalBestStatFilePath();
	std::string						GetLocalBestStatFilePath();
	
	virtual int						GetGlobalBestScore();
	virtual int						GetLocalBestScore();
	virtual void					SetGlobalBestScore(int globalScore);
	virtual void					SetLocalBestScore (int globalScore);

	NNInputs						GetMyNNInputs();

	bool							IsStationaryEntity();
	bool							HasResource();
	bool							IsUnderAttack();

	int								GetIndexOfTaskInNN(TaskType type);
	Entity*							FindMyTownCenter();
	Entity*							FindEnemyTownCenter();
	std::vector<Entity*>			GetAllEntitiesNearMe(int cellDistance);

	std::vector<Entity*>			GetResourceEntityFromList(std::vector<Entity*> &list);
	std::vector<Entity*>			GetTownCenterEntityFromList(std::vector<Entity*> &list);
	std::vector<Entity*>			GetMyTownCenterEntityFromList(std::vector<Entity*> &list);
	
	IntVector2						GetBestNeighbour();
	int								GetMyFoodCount();
	int								GetMyStoneCount();
	int								GetMyWoodCount();

	void							InitNeuralNet();
	void							InitStates();
	void							InitPersonality();
	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCoveredArea();
	void							UpdateTask();
	virtual void					EvaluateNN(Task *task,EntityState previousState,IntVector2 cords);
	virtual void					TrainNN(Task *task);
	void							UpdateNN(float deltaTime);
	void							UpdateTaskFromNN(float deltaTime);
	
	void							UpdateEntityState();
	void							PrintDebugNN();
	void							Render();
	void							RenderTaskType();
	void							RenderSelection();

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

	void							SetTeam(int team);
	void							SetPosition(Vector2 position);
	void							SetPosition(int index);
	void							SetPositionInFloat(Vector2 position);

	void							SetRandomTaskInQueue();
	void							ClearTaskQueue();
	void							CopyDesiredOutputs();
	void							CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	void							ClearDesiredStrategyValues();
	void							SetDesiredStrategyAsOutputForNN(Strategy strategy,int incrementValue);
	void							SetDefaultStrategyAsOutputForNN();
	void							ClearDesiredOutputs();
	void							SetDesiredOutputForTask(TaskType type,float value);
	double							GetDesiredOutputForTask(TaskType type);

	void							TakeDamage(float hitPoint);

	bool							IsPositionInside(Vector2 position);
	bool							IsMovalbleObject();
	bool							IsDifferentFaction(Entity *entity);
	bool							IsResourceNearMe(int cellDistance);

	bool							CreateAndPushTask(TaskType taskType,IntVector2 taskPosition);
	bool							CreateAndPushIdleTask(IntVector2 cordinate);
	bool							CreateAndPushMoveTask(IntVector2 cordinate);
	bool							CreateAndPushBuildHouseTask(IntVector2 cordinate);
	bool							CreateAndPushGatherResourceTask(IntVector2 cordinate,TaskType type);
	bool							CreateAndPushDropResourceTask(IntVector2 cordinate);
	bool							CreateAndPushBuildTownCenterTask(IntVector2 cordinate);
	bool							CreateAndPushBuildArmySpawnerTask(IntVector2 cordinate);
	bool							CreateAndPushLongRangeAttackTask(IntVector2 cordinate);
	bool							CreateAndPushShortRangeAttackTask(IntVector2 cordinate);
	bool							CreateAndPushSpawnVillagerTask(IntVector2 cordinate);
	bool							CreateAndPushSpawnClassAArmyTask(IntVector2 cordinate);
	bool							CreateAndPushSpawnClassBArmyTask(IntVector2 cordinate);

	bool							CreateAndPushDefenseTask(IntVector2 cordinate);
	bool							CreateAndPushFollowTask(IntVector2 cordinate);
	bool							CreateAndPushRetreatTask(IntVector2 cordinate);
	bool							CreateAndPushPatrolTask(IntVector2 cordinate);
	bool							CreateAndPushExploreTask(IntVector2 cordinate);
	bool							CreateAndPushAttackTask(IntVector2 cordinate);

/*
	bool							CheckAndSetStrategyIfNoEnemiesAliveToAttack		(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetStrategyIfNoEnemiesAliveToExplore    (std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetStrategyIfTownCenterUnderAttack		(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetAttackStrategyIfEntityUnderAttack	(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetRetreatStrategyIfEntityUnderAttack	(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetAttackStrategyIfEnemiesOutweighs		(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetRetreatStrategyIfEnemiesOutweighs	(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetStrategyPatrol						(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetStrategyExplore						(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
	bool							CheckAndSetStrategyAttack						(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);*/

	/*bool							CheckAndSetStrategyIfNoEnemiesAliveToExplore        (std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs);
	bool							CheckAndSetStrategyIfTownCenterUnderAttack (std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs);
	bool							CheckAndSetAttackStrategyIfEntityUnderAttack     (std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs);
	bool							CheckAndSetStrategyIfEnemiesOutweighsAllies(std::vector<double> &NNInputVectors, Strategy strategy,double priority, NNInputs inputs);
	bool							CheckAndSetStrategyPatrol			       (std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs);
	bool							CheckAndSetStrategyExplore				   (std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs);
	bool							CheckAndSetStrategyAttack				   (std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs);

	bool CheckAndSetStrategyIfNoEnemiesAliveToExplore        (std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);
	bool CheckAndSetStrategyIfTownCenterUnderAttack		(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);
	bool CheckAndSetAttackStrategyIfEntityUnderAttack     (std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);
	bool CheckAndSetStrategyIfEnemiesOutweighsAllies(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);
	bool CheckAndSetStrategyPatrol			        (std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);
	bool CheckAndSetStrategyExplore				    (std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);
	bool CheckAndSetStrategyAttack				    (std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value);*/


	static std::string				GetEntityTypeAsString(EntityType entityType);
	static std::string				GetStrategyAsString(Strategy strategy);
	static Strategy					GetStrategyFromString(std::string strategyStr);
};

float							ClampAndExtendTo01(float value);
bool							ConvertValueToFunctionBool(float value);
bool							FunctionToCheckEquality(float value1, float value2);

bool							CheckAndSetAttackStrategyIfNoEnemiesAliveTo			(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetExploreStrategyIfNoEnemiesAliveToExplore (std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetStrategyRetreatIfTownCenterUnderAttack	(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetStrategyAttackIfTownCenterUnderAttack	(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetAttackStrategyIfEntityUnderAttack		(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetRetreatStrategyIfEntityUnderAttack		(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetAttackStrategyIfEnemiesOutweighs			(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetRetreatStrategyIfEnemiesOutweighs		(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetStrategyPatrol							(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetStrategyExplore							(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);
bool							CheckAndSetStrategyAttack							(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs);

