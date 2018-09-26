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

	Entity();
	Entity(float x,float y);

	Vector2		GetPosition();
	int			GetTileIndex();
	void		InitNeuralNet();
	void		ProcessInputs(float deltaTime);
	void		Update(float deltaTime);
	void		UpdateNN(float deltaTime);
	void		Render();

	void		SetTeam(int team);
	void		SetPosition(Vector2 position);
	void		SetPosition(int index);
	void		SetPositionInFloat(Vector2 position);

	Rgba		GetTeamColor();
	void		UpdateTask();
	void		EmptyTaskQueue();
	Entity*		FindMyTownCenter();

	void		TakeDamage(float hitPoint);
	void		DeleteEntity();

	bool		IsPositionInside(Vector2 position);
	bool		IsMovalbleObject();
	bool		IsDifferentFaction(Entity *entity);

	//NEURAL NET
	float		GetMiniMapValue();
	TaskType	GetTaskFromNNOutput();
	IntVector2  GetTaskPositonFromNNOutput();

	bool		CreateAndPushMoveTask(IntVector2 cordinate);
	bool		CreateAndPushBuildHouseTask(IntVector2 cordinate);
	bool        CreateAndPushGatherResourceTask(IntVector2 cordinate);
	bool		CreateAndPushDropResourceTask(IntVector2 cordinate);
	bool		CreateAndPushBuildTownCenterTask(IntVector2 cordinate);
	bool		CreateAndPushBuildArmySpawnerTask(IntVector2 cordinate);
	bool		CreateAndPushLongRangeAttackTask(IntVector2 cordinate);
	bool		CreateAndPushShortRangeAttackTask(IntVector2 cordinate);
	bool		CreateAndPushSpawnVillagerTask(IntVector2 cordinate);
	bool		CreateAndPushSpawnClassAArmyTask(IntVector2 cordinate);
	bool		CreateAndPushSpawnClassBArmyTask(IntVector2 cordinate);

	static std::string GetEntityTypeAsString(EntityType entityType);
};