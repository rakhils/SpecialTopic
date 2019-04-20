#include "Game/GamePlay/Entity/Civilian.hpp"

#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetworkConstants.h"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskGatherResource.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskBuildArmySpawner.hpp"
#include "Game/GamePlay/Task/TaskBuildHouse.hpp"
#include "Game/GamePlay/Task/TaskDropResource.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
Civilian::Civilian()
{

}

Civilian::Civilian(Map *map,Vector2 position, int teamID)
{
	m_map		  = map;
	m_teamID	  = teamID;
	SetPosition(position);
	m_taskTypeSupported.push_back(TASK_GATHER_RESOURCE_FOOD);
	m_taskTypeSupported.push_back(TASK_GATHER_RESOURCE_STONE);
	m_taskTypeSupported.push_back(TASK_GATHER_RESOURCE_WOOD);
	m_taskTypeSupported.push_back(TASK_DROP_RESOURCE);
	m_taskTypeSupported.push_back(TASK_BUILD_ARMY_SPAWNER);
	m_taskTypeSupported.push_back(TASK_BUILD_HOUSE);
	//m_taskTypeSupported.push_back(TASK_MOVE);
	m_taskTypeSupported.push_back(TASK_IDLE);
	//m_taskTypeSupported.push_back(TASK_MOVEX);
	//m_taskTypeSupported.push_back(TASK_MOVEY);
	InitNeuralNet();
	InitStates();
	SetRandomTaskInQueue();
	
}

// DESTRUCTOR
Civilian::~Civilian()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::ProcessInputs(float deltaTime)
{
	if(g_currentSelectedEntity == this)
	{
		if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_DOWN_ARROW))
		{
			NEURALNET_LEARNING_RATE-=0.5;
		}
		if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_UP_ARROW))
		{
			NEURALNET_LEARNING_RATE+=0.5;
		}

		Vector2 mousePosition   = InputSystem::GetInstance()->GetMouseClientPosition();
		mousePosition.y			= Windows::GetInstance()->GetDimensions().y - mousePosition.y;

		if (InputSystem::GetInstance()->WasLButtonJustPressed() && m_map->IsPositionInsideMap(mousePosition))
		{
			int tileIndex = m_map->GetTileIndex(mousePosition);
			Entity *entity = m_map->GetEntityFromPosition(tileIndex);
			if (m_pendingTask == BUILD_ARMYCENTER)
			{
				Task *task = new TaskBuildArmySpawner(m_map, this, mousePosition);
				if(task->CheckAndReduceResources())
				{
					m_taskQueue.push(task);
					m_pendingTask = NOTASK;
					return;
				}
				delete task;
			}
			if (m_pendingTask == BUILD_HOUSE)
			{
				Task *task = new TaskBuildHouse(m_map, this, mousePosition);
				if(task->CheckAndReduceResources())
				{
					m_taskQueue.push(task);
					m_pendingTask = NOTASK;
					return;
				}
				delete task;
			}
			if(entity == nullptr)
			{
				ClearTaskQueue();
				Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskMove(m_map,this,mapPosition);
				m_taskQueue.push(task);
			}
			else if(entity != nullptr && (entity->m_type == RESOURCE_FOOD || entity->m_type == RESOURCE_STONE|| entity->m_type == RESOURCE_WOOD))
			{
				Task * task = nullptr;
				switch (entity->m_type)
				{
					case RESOURCE_FOOD:
					{
						task = new TaskGatherResource(this, TASK_GATHER_RESOURCE_FOOD);
						m_taskQueue.push(task);
					}
					break;
					case RESOURCE_STONE:
					{	task = new TaskGatherResource(this, TASK_GATHER_RESOURCE_STONE);
						m_taskQueue.push(task);
					}
					break;
					case RESOURCE_WOOD:
					{
						task = new TaskGatherResource(this, TASK_GATHER_RESOURCE_WOOD);
						m_taskQueue.push(task);
					}
					break;
					default:
						break;
				}
			}
			else if(entity != nullptr && (entity->m_type == TOWN_CENTER))
			{
				ClearTaskQueue();
				Task *task = new TaskDropResource(this,FindMyTownCenter());
				m_taskQueue.push(task);
			}
		}
		if (g_unitStatHUDFirstButton.IsPointInside(mousePosition))
		{
			m_pendingTask = BUILD_ARMYCENTER;
		}
		if (g_unitStatHUDSecondButton.IsPointInside(mousePosition))
		{
			m_pendingTask = BUILD_HOUSE;
		}
	}
	Entity::ProcessInputs(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates civilian
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType Civilian::GetTaskFromNNOutput(double &max)
{
	TaskType type = m_taskTypeSupported.at(0);
	int subtractTaskCount = 2;
	for (int outputIndex = 0; outputIndex < m_taskTypeSupported.size() - subtractTaskCount; outputIndex++)
	{
		if (m_neuralNet.m_outputs->m_neurons.at(outputIndex).m_value > max)
		{
			if(m_resourceTypeCarrying == nullptr && m_taskTypeSupported.at(outputIndex) == TASK_DROP_RESOURCE)
			{
				continue;
			}
			if (m_resourceTypeCarrying != nullptr && 
				(m_taskTypeSupported.at(outputIndex) == TASK_GATHER_RESOURCE_FOOD || 
					m_taskTypeSupported.at(outputIndex) == TASK_GATHER_RESOURCE_STONE || 
					   m_taskTypeSupported.at(outputIndex) == TASK_GATHER_RESOURCE_WOOD))
			{
				continue;
			}
			type = m_taskTypeSupported.at(outputIndex);
			max = m_neuralNet.m_outputs->m_neurons.at(outputIndex).m_value;
		}
	}
	return type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Civilian::GetGlobalBestScore()
{
	if (m_teamID == 1)
	{
		return g_globalMaxScoreCivilianTeam1;
	}
	if (m_teamID == 2)
	{
		return g_globalMaxScoreCivilianTeam2;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			return g_globalAttackMaxScoreCivilianTeam1;
		}
		if (m_teamID == 2)
		{
			return g_globalAttackMaxScoreCivilianTeam2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_globalDefenseMaxScoreCivilianTeam1;
		}
		if (m_teamID == 2)
		{
			return g_globalDefenseMaxScoreCivilianTeam2;
		}
		break;
	default:
		break;
	}*/
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Civilian::GetLocalBestScore()
{
	if (m_teamID == 1)
	{
		return g_localMaxScoreCivilianTeam1;
	}
	if (m_teamID == 2)
	{
		return g_localMaxScoreCivilianTeam2;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetGlobalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_globalMaxScoreCivilianTeam1 = value;
	}
	if (m_teamID == 2)
	{
		g_globalMaxScoreCivilianTeam2 = value;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_globalAttackMaxScoreCivilianTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalAttackMaxScoreCivilianTeam2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_globalDefenseMaxScoreCivilianTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalDefenseMaxScoreCivilianTeam2 = value;
		}
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetLocalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_localMaxScoreCivilianTeam1 = value;
	}
	if (m_teamID == 2)
	{
		g_localMaxScoreCivilianTeam2 = value;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_localAttackMaxScoreCivilianTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_localAttackMaxScoreCivilianTeam2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_localDefenseMaxScoreCivilianTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_localDefenseMaxScoreCivilianTeam2 = value;
		}
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Civilian::HasMoreResources()
{
	if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_food > g_maxResourceCountRequired && ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_wood > g_maxResourceCountRequired && ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_stone > g_maxResourceCountRequired)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : Evaluate the previous NN task executed
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateNN(Task * task,EntityState previousState,IntVector2 cords)
{
	if(!g_isCurrentlyTraining)
	{
		return;
	}

	UNUSED(task);
	CopyDesiredOutputs();

	if(m_state.m_health < previousState.m_health)
	{
		m_state.m_neuralNetPoints++;
		return;
	}

	TrainOnDropingResources(previousState, cords);
	TrainOnBuildHouseTask(previousState, cords);
	TrainOnBuildArmySpawnerTask(previousState, cords);
	TrainToGatherAndDropResource(previousState, cords);

	switch (task->m_taskType)
	{
	case TASK_MOVE:
		EvaluateMoveTask(previousState,cords);
		break;
	case TASK_GATHER_RESOURCE_FOOD:
		EvaluateGatherResourceFoodTask(previousState,cords);
		break;
	case TASK_GATHER_RESOURCE_STONE:
		EvaluateGatherResourceStoneTask(previousState, cords);
		break;;
	case TASK_GATHER_RESOURCE_WOOD:
		EvaluateGatherResourceWoodTask(previousState, cords);
		break;
	case TASK_DROP_RESOURCE:
		EvaluateDropResourceTask(previousState,cords);
		break;
	case TASK_BUILD_HOUSE:
		EvaluateBuildHouseTask(previousState, cords);
		break;
	case TASK_BUILD_ARMY_SPAWNER:
		EvaluateBuildArmySpawnerTask(previousState, cords);
		break;
	case TASK_IDLE:
		EvaluateIdleTask(previousState);
		break;
	default:
		break;
	}
	
	Entity::EvaluateNN(task, previousState, cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateMoveTask(EntityState previousState,IntVector2 cords)
{
	if(m_map->GetCordinates(previousState.m_position) == cords)
	{
		m_state.m_neuralNetPoints++;
		return;
	}

	Entity *targetPlaceEntity = m_map->GetEntityFromPosition(cords);
	if (targetPlaceEntity != nullptr && targetPlaceEntity != this)
	{
		m_state.m_neuralNetPoints++;
		return;
	}
	//SetDesiredOutputForTask(TASK_MOVE, 0);
	//m_state.m_neuralNetPoints++;

	/*if(true)
	{
		return;
	}
	if (m_resourceTypeCarrying != nullptr)
	{
		EvaluateMoveTaskToTownCenter(previousState, cords);
		return;
	}
	EvaluateMoveTaskToResource(previousState, cords);
	return;*/
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateIdleTask(EntityState prevState)
{
	SetDesiredOutputForTask(TASK_IDLE, 0);
	//SetDesiredOutputToMoveToNeighbour(prevState, 8);
	m_state.m_neuralNetPoints++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateGatherResourceFoodTask(EntityState prevState,IntVector2 cords)
{
	UNUSED(cords);
	if (m_state.m_resourceFoodGathered == prevState.m_resourceFoodGathered)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 0);
		return;
	}
	Resources resourceStat = ((TownCenter*)FindMyTownCenter())->m_resourceStat;
	if (resourceStat.m_food > g_maxResourceCountRequired
		&& resourceStat.m_stone > g_maxResourceCountRequired
		&& resourceStat.m_wood > g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (resourceStat.m_food > g_maxResourceCountRequired)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 0);
		if (resourceStat.m_wood < g_maxResourceCountRequired)
		{
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
		}
		if (resourceStat.m_stone < g_maxResourceCountRequired)
		{
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		}
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateGatherResourceStoneTask(EntityState prevState, IntVector2 cords)
{
	UNUSED(cords);
	if (m_state.m_resourceStoneGathered == prevState.m_resourceStoneGathered)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 0);
		return;
	}
	Resources resourceStat = ((TownCenter*)FindMyTownCenter())->m_resourceStat;
	if (resourceStat.m_food > g_maxResourceCountRequired
		&& resourceStat.m_stone > g_maxResourceCountRequired
		&& resourceStat.m_wood > g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (resourceStat.m_stone > g_maxResourceCountRequired)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 0);
		if (resourceStat.m_food < g_maxResourceCountRequired)
		{
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
		}
		if (resourceStat.m_wood < g_maxResourceCountRequired)
		{
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
		}
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateGatherResourceWoodTask(EntityState prevState, IntVector2 cords)
{
	UNUSED(cords);
	if (m_state.m_resourceWoodGathered == prevState.m_resourceWoodGathered)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 0);
		return;
	}
	Resources resourceStat = ((TownCenter*)FindMyTownCenter())->m_resourceStat;
	if (resourceStat.m_food     > g_maxResourceCountRequired
		&& resourceStat.m_stone > g_maxResourceCountRequired
		&& resourceStat.m_wood  > g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (resourceStat.m_wood > g_maxResourceCountRequired)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 0);
		if (resourceStat.m_food < g_maxResourceCountRequired)
		{
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
		}
		if (resourceStat.m_stone < g_maxResourceCountRequired)
		{
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		}
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateDropResourceTask(EntityState prevState, IntVector2 cords)
{
	UNUSED(cords);
	if (HasResource())
	{
		SetDesiredOutputForTask(TASK_DROP_RESOURCE, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 0);
		m_state.m_neuralNetPoints++;
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_DROP_RESOURCE, 0.0f);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateBuildHouseTask(EntityState prevState, IntVector2 location)
{
	UNUSED(location);
	if(prevState.m_numberOfHouseBuilt == m_state.m_numberOfHouseBuilt)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
		SetDesiredOutputForTask(TASK_MOVE, 0.0f);
		return;
	}

	int numHouses = ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_houses;
	if(prevState.m_numberOfHouseBuilt < m_state.m_numberOfHouseBuilt)
	{
		if(numHouses > g_maxHousesBuilt)
		{
			m_state.m_neuralNetPoints++;
			SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
			SetDesiredOutputForTask(TASK_MOVE, 0.0f);
			return;
		}
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
	SetDesiredOutputForTask(TASK_MOVE, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateBuildArmySpawnerTask(EntityState prevState, IntVector2 location)
{
	UNUSED(location);
	if(prevState.m_numberOfArmySpawnerBuilt == m_state.m_numberOfArmySpawnerBuilt)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
		SetDesiredOutputForTask(TASK_MOVE, 0.0f);
		return;
	}
	int numArmySpawners = ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_armySpawners;
	if (prevState.m_numberOfArmySpawnerBuilt < m_state.m_numberOfArmySpawnerBuilt)
	{
		if (numArmySpawners > g_maxArmySpawnerBuilt)
		{
			m_state.m_neuralNetPoints++;
			SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
			SetDesiredOutputForTask(TASK_MOVE, 0.0f);
			return;
		}
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
	SetDesiredOutputForTask(TASK_MOVE, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : Trains o build house task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainOnBuildHouseTask(EntityState previousState, IntVector2 cords)
{
	TownCenter* townCenter = (TownCenter*)FindMyTownCenter();
	UNUSED(cords);
	if (townCenter->m_resourceStat.m_houses >= g_maxHousesBuilt)
	{
		return;
	}
	
	if (townCenter->m_resourceStat.m_food > g_maxResourceCountRequired &&
		 townCenter->m_resourceStat.m_stone > g_maxResourceCountRequired &&
		  townCenter->m_resourceStat.m_wood > g_maxResourceCountRequired   && 
		   townCenter->m_resourceStat.m_houses < g_maxHousesBuilt)
	{

		std::vector<Entity*> neighbourEntities = m_map->GetAllEntitiesNearLocation(previousState.m_position, 1);
		for (int index = 0; index < neighbourEntities.size(); index++)
		{
			if (neighbourEntities.at(index)->IsStationaryEntity())
			{
				SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.f);
				SetDesiredOutputForTask(TASK_MOVE, 1.0f);
				return;
			}
		}
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_HOUSE, 1.0f);
		SetDesiredOutputForTask(TASK_MOVE, 1.0f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 0);
		return;
	}
	
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_MOVE, 0.0f);
	SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.f);
	Resources resourceStat = ((TownCenter*)FindMyTownCenter())->m_resourceStat;
	if (resourceStat.m_food > g_maxResourceCountRequired
		&& resourceStat.m_stone > g_maxResourceCountRequired
		&& resourceStat.m_wood > g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (resourceStat.m_food <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
	}
	if (resourceStat.m_stone <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
	}
	if (resourceStat.m_wood <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : NI
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainOnBuildArmySpawnerTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	IntVector2 cordsNew = m_map->GetCordinates(previousState.m_position);
	TownCenter* townCenter = (TownCenter*)FindMyTownCenter();
	if(townCenter->m_resourceStat.m_armySpawners >= g_maxArmySpawnerBuilt)
	{
		return;
	}
	if(m_map->HasEnoughResourceToBuildArmySpawnCenter(m_teamID) && townCenter->m_resourceStat.m_armySpawners < g_maxArmySpawnerBuilt)
	{
		std::vector<Entity*> neighbourEntities = m_map->GetAllEntitiesNearLocation(GetPosition(), 1);
		for (int index = 0; index < neighbourEntities.size(); index++)
		{
			if (neighbourEntities.at(index)->IsStationaryEntity())
			{
				SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.f);
				SetDesiredOutputForTask(TASK_MOVE, 1.f);
				return;
			}
		}
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 1.0f);
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 0);
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_MOVE, 0.0f);
	SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.f);
	Resources resourceStat = ((TownCenter*)FindMyTownCenter())->m_resourceStat;
	if (resourceStat.m_food > g_maxResourceCountRequired
		&& resourceStat.m_stone > g_maxResourceCountRequired
		&& resourceStat.m_wood > g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD,  1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD,  1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (resourceStat.m_food <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
	}
	if (resourceStat.m_stone <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
	}
	if (resourceStat.m_wood <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainOnDropingResources(EntityState previousState, IntVector2 cords)
{
	UNUSED(previousState);
	UNUSED(cords);
	if(m_resourceTypeCarrying == nullptr)
	{
		return;
	}
	SetDesiredOutputForTask(TASK_DROP_RESOURCE, 1.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainToGatherAndDropResource(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	m_state.m_neuralNetPoints++;
	if(HasResource())
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD,  0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 0);
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD,  0);
		if(!(GetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER) > 0 || GetDesiredOutputForTask(TASK_BUILD_HOUSE) > 0))
		{
			SetDesiredOutputForTask(TASK_DROP_RESOURCE, 1);
		}
		return;
	}
	Resources resourceStat = ((TownCenter*)FindMyTownCenter())->m_resourceStat;

	if(resourceStat.m_food > g_maxResourceCountRequired &&
		resourceStat.m_stone > g_maxResourceCountRequired &&
		 resourceStat.m_wood > g_maxResourceCountRequired)
		 {
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
			SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
			return;
		 }

	if (resourceStat.m_food <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_FOOD, 1.f);
	}
	if (resourceStat.m_stone <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_STONE, 1.f);
	}
	if (resourceStat.m_wood <= g_maxResourceCountRequired)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE_WOOD, 1.f);
	}
	SetDesiredOutputForTask(TASK_DROP_RESOURCE, 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : Sets the desired output to move to safe area
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToMoveToSafeArea()
{
	IntVector2 neighbour = GetRandomSafeArea();
	float xPosition = RangeMapFloat(static_cast<float>(neighbour.x), 0.f, static_cast<float>(m_map->m_maxWidth),  0.f, 1.f);
	float yPosition = RangeMapFloat(static_cast<float>(neighbour.y), 0.f, static_cast<float>(m_map->m_maxHeight), 0.f, 1.f);
	SetDesiredOutputForTask(TASK_MOVE, 1.f);
	SetDesiredOutputForTask(TASK_MOVEX, xPosition);
	SetDesiredOutputForTask(TASK_MOVEY, yPosition);
	//m_map->CreateExplosions(m_map->GetMapPosition(neighbour));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToMoveToRandomArea()
{
	IntVector2 neighbour = GetRandomTeritaryArea();
	float xPosition = RangeMapFloat(static_cast<float>(neighbour.x), 0.f, static_cast<float>(m_map->m_maxWidth),  0.f, 1.f);
	float yPosition = RangeMapFloat(static_cast<float>(neighbour.y), 0.f, static_cast<float>(m_map->m_maxHeight), 0.f, 1.f);
	SetDesiredOutputForTask(TASK_MOVE, 1.f);
	SetDesiredOutputForTask(TASK_MOVEX, xPosition);
	SetDesiredOutputForTask(TASK_MOVEY, yPosition);
	//m_map->CreateExplosions(m_map->GetMapPosition(neighbour));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToRandomTask()
{
	int index = GetRandomIntLessThan(static_cast<int>(m_desiredOuputs.size()));
	SetDesiredOutputForTask(m_taskTypeSupported.at(index), GetRandomFloatZeroToOne());
	SetDesiredOutputForTask(TASK_IDLE, 0.1f);
	m_state.m_neuralNetPoints++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToMoveToTownCenter(float value,EntityState previousState,IntVector2 cords)
{
	UNUSED(value);
	UNUSED(previousState);
	UNUSED(cords);
	//float currentTownCenterNearnessValue = 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOuputForMoveToResourceTask(float value, EntityState previousState, IntVector2 cords)
{
	if (m_map->GetCordinates(previousState.m_position) == cords)
	{
		SetDesiredOutputForTask(TASK_MOVE, g_desiredOutputForSameRandomPosition);
		return;
	}

	SetDesiredOutputForTask(TASK_MOVE, value);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : trains NeuralNet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainNN(Task *task)
{
	Entity::TrainNN(task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::Render()
{
	if (m_health <= 0)
	{
		return;
	}
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	if(m_teamID == 1)
	{
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "C", g_fontSize, Rgba::BLACK);
	}
	else
	{
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "C", g_fontSize, Rgba::WHITE);
	}
	delete textMaterial;
}
