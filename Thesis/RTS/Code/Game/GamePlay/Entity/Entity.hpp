#include <queue>

#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"

#include "Game/GamePlay/Task/Task.hpp"
#pragma once
enum EntityType
{
	CIVILIAN,
	WARRIOR_SHORT_RANGE,
	WARRIOR_LONG_RANGE,
	HOUSE,
	TOWN_CENTER,
	RESOURCE_FOOD,
	RESOURCE_STONE,
	RESOURCE_WOOD,
	ARMY_SPAWNER
};

class Map;
class Entity
{
public:
	Disc2				  m_disc;
	AABB2				  m_aabb2;

	bool				  m_isSelected = false;
	EntityType			  m_type = CIVILIAN;
	int					  m_teamID = 0;
	Map *				  m_map = nullptr;
	float				  m_health = 10;
	NeuralNetwork		  m_neuralNet;

	Rgba				  m_color;
	std::queue<Task*>	  m_taskQueue;
	std::vector<TaskType> m_taskTypeSupported;

	std::vector<float>    m_lastInputState;

	int					  m_resourceFoodUsed					 = 0;
	int					  m_resourceStoneUsed					 = 0;
	int					  m_resourceWoodUsed					 = 0;
	int					  m_resourceFoodUsedBeforeTask			 = 0;
	int					  m_resourceStoneUsedBeforeTask			 = 0;
	int					  m_resourceWoodUsedBeforeTask			 = 0;

	float				  m_resourceFoodGathered				 = 0;
	float				  m_resourceFoodDropped					 = 0;
	float				  m_resourceStoneGathered				 = 0;
	float				  m_resourceStoneDropped				 = 0;
	float				  m_resourceWoodGathered				 = 0;
	float				  m_resourceWoodDropped					 = 0;
	float				  m_numberOfArmySpawnerBuilt			 = 0;
	float				  m_numberOfHouseBuilt					 = 0;				  										 
	float				  m_resourceFoodGatheredBeforeTask		 = 0;
	float				  m_resourceFoodDroppedBeforeTask		 = 0;
	float				  m_resourceStoneGatheredBeforeTask		 = 0;
	float				  m_resourceStoneDroppedBeforeTask		 = 0;
	float				  m_resourceWoodGatheredBeforeTask		 = 0;
	float				  m_resourceWoodDroppedBeforeTask		 = 0;
	float				  m_numberOfArmySpawnerBuiltBeforeTask	 = 0;
	float				  m_numberOfHouseBuiltBeforeTask		 = 0;

	int					  m_longRangeArmySpawned				 = 0;
	int					  m_shortRangeArmySpawned			     = 0;
	int					  m_longRangeArmySpawnedBeforeTask		 = 0;
	int					  m_shortRangeArmySpawnedBeforeTask		 = 0;
																 
	int					  m_enemiesAttacked						 = 0;
	int					  m_enemiesKilled						 = 0;
	int					  m_enemiesAttackedBeforeTask			 = 0;
	int					  m_enemiesKilledBeforeTask				 = 0;
																 
	int					  m_villagerSpawned						 = 0;
	int					  m_villagerSpawnedBeforeTask			 = 0;


	Entity();
	Entity(float x,float y);

	Vector2					GetPosition();
	int						GetTileIndex();
	void					InitNeuralNet();
	void					ProcessInputs(float deltaTime);

	
	void					Update(float deltaTime);
	virtual void			TrainNN();
	void					UpdateNN(float deltaTime);
	void					UpdateLastNNInputState(std::vector<float> &minmapvalue,std::vector<float> &gameStat);
	void					UpdateAllUnitStatWithCurrentValue();

	void					UpdateUnitStatForFoodGathered(int count);
	void					UpdateUnitStatForStoneGathered(int count);
	void					UpdateUnitStatForWoodGathered(int count);
	void					UpdateUnitStatForFoodDropped(int count);
	void					UpdateUnitStatForStoneDropped(int count);
	void					UpdateUnitStatForWoodDropped(int count);
	void					UpdateUnitStatForArmySpawnerBuilt(int count);
	void					UpdateUnitStatForHouseBuilt(int count);
	void					UpdateUnitStatForShortRangeArmySpawned(int count);
	void					UpdateUnitStatForLongRangeArmySpawned(int count);
	void					UpdateUnitStatForEnemiesAttacked(int count);
	void					UpdateUnitStatForEnemiesKilled(int count);
	void					UpdateUnitStatForVillagerSpawned(int count);

	void					UpdateUnitStatForFoodUsed(int count);
	void					UpdateUnitStatForStoneUsed(int count);
	void					UpdateUnitStatForWoodUsed(int count);
	int						GetIndexOfOutputTask(TaskType type);
	void					Render();

	void					SetTeam(int team);
	void					SetPosition(Vector2 position);
	void					SetPosition(int index);
	void					SetPositionInFloat(Vector2 position);

	Rgba					GetTeamColor();
	void					UpdateTask();
	void					EmptyTaskQueue();
	Entity*					FindMyTownCenter();

	void					TakeDamage(float hitPoint);
	void					DeleteEntity();

	bool					IsPositionInside(Vector2 position);
	bool					IsMovalbleObject();
	bool					IsDifferentFaction(Entity *entity);
	bool					IsResourceNearMe(int cellDistance);
	std::vector<Entity*>	GetAllEntitiesNearMe(int cellDistance);
	//NEURAL NET
	
	float					GetMiniMapValue();
	TaskType				GetTaskFromNNOutput();
	IntVector2				GetTaskPositonFromNNOutput();

	bool					CreateAndPushIdleTask(IntVector2 cordinate);
	bool					CreateAndPushMoveTask(IntVector2 cordinate);
	bool					CreateAndPushBuildHouseTask(IntVector2 cordinate);
	bool					CreateAndPushGatherResourceTask(IntVector2 cordinate);
	bool					CreateAndPushDropResourceTask(IntVector2 cordinate);
	bool					CreateAndPushBuildTownCenterTask(IntVector2 cordinate);
	bool					CreateAndPushBuildArmySpawnerTask(IntVector2 cordinate);
	bool					CreateAndPushLongRangeAttackTask(IntVector2 cordinate);
	bool					CreateAndPushShortRangeAttackTask(IntVector2 cordinate);
	bool					CreateAndPushSpawnVillagerTask(IntVector2 cordinate);
	bool					CreateAndPushSpawnClassAArmyTask(IntVector2 cordinate);
	bool					CreateAndPushSpawnClassBArmyTask(IntVector2 cordinate);

	static std::string		GetEntityTypeAsString(EntityType entityType);
};