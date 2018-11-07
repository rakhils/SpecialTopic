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
	m_taskTypeSupported.push_back(TASK_GATHER_RESOURCE);
	m_taskTypeSupported.push_back(TASK_DROP_RESOURCE);
	m_taskTypeSupported.push_back(TASK_BUILD_ARMY_SPAWNER);
	m_taskTypeSupported.push_back(TASK_BUILD_HOUSE);
	m_taskTypeSupported.push_back(TASK_MOVE);
	m_taskTypeSupported.push_back(TASK_IDLE);
	m_taskTypeSupported.push_back(TASK_MOVEX);
	m_taskTypeSupported.push_back(TASK_MOVEY);
	m_resourceTypeCarrying = m_map->m_resources.at(0);
	InitNeuralNet();
	InitStates();
	m_taskQueue.push(new TaskGatherResource(this));
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
				ClearTaskQueue();
				Task *task = new TaskGatherResource(this);
				m_taskQueue.push(task);
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
/*DATE    : 2018/10/07
*@purpose : Evaluate the previous NN task executed
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateNN(Task * task,EntityState previousState,IntVector2 cords)
{
	UNUSED(task);
	//ClearDesiredOutputs();

	if(m_state.m_health < previousState.m_health)
	{
		SetDesiredOutputToMoveToNeighbour(previousState,3);
		m_state.m_neuralNetPoints++;
		return;
	}
	CopyDesiredOutputs();

	switch (task->m_taskType)
	{
	case TASK_MOVE:
		EvaluateMoveTask(previousState,cords);
		break;
	case TASK_GATHER_RESOURCE:
		EvaluateGatherResourceTask(previousState,cords);
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
	int moveIndex = GetIndexOfTaskInNN(TASK_MOVE);
	TrainOnBuildHouseTask(previousState, cords);
	TrainOnBuildArmySpawnerTask(previousState, cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateMoveTask(EntityState previousState,IntVector2 cords)
{
	//Entity *targetPlaceEntity = m_map->GetEntityFromPosition(GetTaskPositonFromNNOutput());
	/*if (targetPlaceEntity != nullptr && targetPlaceEntity != this)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputToMoveToNeighbour(2);
		return;
	}*/
	if (m_resourceTypeCarrying != nullptr)
	{
		EvaluateMoveTaskToTownCenter(previousState, cords);
		return;
	}
	EvaluateMoveTaskToResource(previousState, cords);
	return;
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
	m_state.m_neuralNetPoints++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateGatherResourceTask(EntityState prevState,IntVector2 cords)
{
	UNUSED(cords);
	if(m_state.m_resourceFoodGathered > prevState.m_resourceFoodGathered 
		|| m_state.m_resourceStoneGathered > prevState.m_resourceStoneGathered
			|| m_state.m_resourceWoodGathered > prevState.m_resourceWoodGathered)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 0);
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
	if (m_state.m_resourceFoodDropped > prevState.m_resourceFoodDropped
		|| m_state.m_resourceStoneDropped > prevState.m_resourceStoneDropped
			|| m_state.m_resourceWoodDropped > prevState.m_resourceWoodDropped)
	{
		SetDesiredOutputForTask(TASK_DROP_RESOURCE, 1.f);
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
	if(m_map->m_mapMode == MAP_MODE_TRAINING_CIVILIAN_GATHER_FOOD || m_map->m_mapMode == MAP_MODE_TRAINING_CIVILIAN_GATHER_ALL_RESOURCES)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
		return;
	}

	if(prevState.m_numberOfHouseBuilt == m_state.m_numberOfHouseBuilt)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
		return;
	}

	int numHouses = ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_houses;
	if(prevState.m_numberOfHouseBuilt < m_state.m_numberOfHouseBuilt)
	{
		if(numHouses > g_maxHousesBuilt)
		{
			m_state.m_neuralNetPoints++;
			SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
			return;
		}
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0.0f);
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
	if (m_map->m_mapMode == MAP_MODE_TRAINING_CIVILIAN_GATHER_FOOD || m_map->m_mapMode == MAP_MODE_TRAINING_CIVILIAN_GATHER_ALL_RESOURCES)
	{
		SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
		m_state.m_neuralNetPoints++;
	}
	if (prevState.m_numberOfArmySpawnerBuilt == m_state.m_numberOfArmySpawnerBuilt)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
		return;
	}

	int numArmySpawners = ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_armySpawners;
	if (prevState.m_numberOfArmySpawnerBuilt < m_state.m_numberOfArmySpawnerBuilt)
	{
		if (numArmySpawners > g_maxArmySpawnerBuilt)
		{
			m_state.m_neuralNetPoints++;
			SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
			return;
		}
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : Trains o buildhouse task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainOnBuildHouseTask(EntityState previousState, IntVector2 cords)
{
	TownCenter* townCenter = (TownCenter*)FindMyTownCenter();
	UNUSED(cords);

	if(townCenter->m_resourceStat.m_houses < g_maxHousesBuilt)
	{
		if(townCenter->m_resourceStat.m_food > g_maxResourceCountRequired && 
			townCenter->m_resourceStat.m_stone > g_maxResourceCountRequired &&
				townCenter->m_resourceStat.m_wood > g_maxResourceCountRequired && townCenter->m_resourceStat.m_houses < 3)
		{
			SetDesiredOutputForTask(TASK_BUILD_HOUSE, 1.0f);
		}
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
	TownCenter* townCenter = (TownCenter*)FindMyTownCenter();

	if (townCenter->m_resourceStat.m_armySpawners < g_maxHousesBuilt)
	{
		if (townCenter->m_resourceStat.m_food > g_maxResourceCountRequired &&
			townCenter->m_resourceStat.m_stone > g_maxResourceCountRequired &&
			townCenter->m_resourceStat.m_wood > g_maxResourceCountRequired && townCenter->m_resourceStat.m_armySpawners < 2)
		{
			SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 1.0f);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Finds the best neighbour
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Civilian::GetBestNeighbour()
{
	/*std::vector<IntVector2> neighbours = m_map->GetAllNeighbourCoordinates(GetCordinates(), 1);
	float townCenterNearnessValue = static_cast<float>(GetMax(m_map->m_maxHeight,m_map->m_maxWidth)) + 1.f;
	float resourceNearnessValue   = static_cast<float>(GetMax(m_map->m_maxHeight,m_map->m_maxWidth)) + 1.f;
	IntVector2 bestNeighbour;

	for(int index = 0;index < neighbours.size();index++)
	{
		CellSensoryValues cellValue = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(neighbours.at(index)));
		if(HasResource())
		{
			/ *if(townCenterNearnessValue > cellValue.m_team2TownCenterNearness)
			{
				if (m_map->GetEntityFromPosition(cellValue.m_coords) == nullptr)
				{
					townCenterNearnessValue = cellValue.m_team2TownCenterNearness;
					bestNeighbour			= cellValue.m_coords;
				}
			}* /
		}
		else
		{
			/ *if(resourceNearnessValue > cellValue.m_resourceNearnessForFood)
			{
				if(m_map->GetEntityFromPosition(cellValue.m_coords) == nullptr)
				{
					resourceNearnessValue = cellValue.m_resourceNearnessForFood;
					bestNeighbour		  = cellValue.m_coords;
				}
			}* /
		}
	}
	return bestNeighbour;*/
	return IntVector2::ZERO;
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
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateMoveTaskToTownCenter(EntityState prevState,IntVector2 cords)
{
	Rgba color = Rgba::GREEN;
	float tcNearnessCurrent = 0.f;//m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_team2TownCenterNearness;
	float tcNearnessOld     = 0.f;
	if(m_teamID == 1)
	{
		tcNearnessCurrent = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_entityNearness.at(FAVORED_MOVETO_TEAM1_TOWNCENTER);
		tcNearnessOld	  = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(prevState.m_position)).m_entityNearness.at(FAVORED_MOVETO_TEAM1_TOWNCENTER);
	}
	else
	{
		tcNearnessCurrent = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_entityNearness.at(FAVORED_MOVETO_TEAM2_TOWNCENTER);
		tcNearnessOld     = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(prevState.m_position)).m_entityNearness.at(FAVORED_MOVETO_TEAM2_TOWNCENTER);
	}

	IntVector2 cords1 = GetCordinates();
	IntVector2 cords2 = m_map->GetCordinates(m_previousState.m_position);
	UNUSED(cords1);
	UNUSED(cords2);
	int index1 = m_map->GetTileIndex(cords);
	int index2 = m_map->GetTileIndex(prevState.m_position);
	UNUSED(index1);
	UNUSED(index2);

	if (tcNearnessCurrent == 1 && tcNearnessOld == 1)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_map->CreateExplosions(GetPosition(), Rgba::RED);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (tcNearnessCurrent == 1 && tcNearnessOld > 1)
	{
		if (m_map->GetEntityFromPosition(cords) == this)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), Rgba::RED);
			m_state.m_neuralNetPoints++;
			return;
		}
		m_map->CreateExplosions(GetPosition(), Rgba::RED);
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		//SetDesiredOuputForMoveToTownCenterTask()
		m_state.m_neuralNetPoints++;
		return;
	}
	if (tcNearnessCurrent > 1 && tcNearnessOld == 1)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		m_map->CreateExplosions(GetPosition(), Rgba::RED);
		return;
	}



	if (tcNearnessCurrent - tcNearnessOld < 0) // moving towards
	{
		if (GetCordinates() == cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), Rgba::MAGENTA);
			//DebugDraw::GetInstance()->DebugRenderLogf(2,"moving towards cords = this");
			m_state.m_neuralNetPoints++;
			return;
		}
		//DebugDraw::GetInstance()->DebugRenderLogf(2,"moving towards");
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_map->CreateExplosions(GetPosition(), Rgba::WHITE);
		m_state.m_neuralNetPoints++;
		return;
	}
	else if (tcNearnessCurrent - tcNearnessOld == 0)
	{
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		//DebugDraw::GetInstance()->DebugRenderLogf(2,"moving same");
		m_state.m_neuralNetPoints++;
		return;
	}
	else if (tcNearnessCurrent - tcNearnessOld > 0.f)
	{
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		//DebugDraw::GetInstance()->DebugRenderLogf(2, "moving away");
		m_state.m_neuralNetPoints++;
		return;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateMoveTaskToResource(EntityState prevState,IntVector2 cords)
{
	Rgba color = Rgba::GREEN;
	TownCenter *myTownCenter = ((TownCenter*)FindMyTownCenter());
	float currentResourceNearnessForFood  
		= m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_FOOD);
	float oldResourceNearnessForFood      
		= m_map->m_cellSensoryValues.at(m_map->GetTileIndex(prevState.m_position)).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_FOOD);

	float currentResourceNearnessForStone 
		= m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_STONE);
	float oldResourceNearnessForStone     
		= m_map->m_cellSensoryValues.at(m_map->GetTileIndex(prevState.m_position)).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_STONE);

	float currentResourceNearnessForWood  
		= m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_WOOD);
	float oldResourceNearnessForWood      
		= m_map->m_cellSensoryValues.at(m_map->GetTileIndex(prevState.m_position)).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_WOOD);


	float currentResourceNearness = 0;
	float oldResourceNearness	  = 0;

	currentResourceNearness += GetMaxOf2(currentResourceNearnessForFood,0);
	currentResourceNearness += GetMaxOf2(currentResourceNearnessForStone,0);
	currentResourceNearness += GetMaxOf2(currentResourceNearnessForWood,0);

	oldResourceNearness     += GetMaxOf2(oldResourceNearnessForFood,0);
	oldResourceNearness		+= GetMaxOf2(oldResourceNearnessForStone,0);
	oldResourceNearness		+= GetMaxOf2(oldResourceNearnessForWood,0);

	float max = g_maxResourceCountRequired + 5000;
	float foodWeight  = (max - myTownCenter->m_resourceStat.m_food) / max;
	float stoneWeight = (max - myTownCenter->m_resourceStat.m_stone)/ max;
	float woodWeight  = (max - myTownCenter->m_resourceStat.m_wood) / max;
	foodWeight  = ClampFloat(foodWeight, 0, 1);
	stoneWeight = ClampFloat(stoneWeight, 0, 1);
	woodWeight  = ClampFloat(woodWeight, 0, 1);


	if(currentResourceNearnessForFood == 1 && oldResourceNearnessForFood == 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_food > max)
		{
			SetDesiredOutputToMoveToNeighbour(prevState,2);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() != cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (currentResourceNearnessForStone == 1 && oldResourceNearnessForStone == 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_stone > max)
		{
			SetDesiredOutputToMoveToNeighbour(prevState,2);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() != cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (currentResourceNearnessForWood == 1 && oldResourceNearnessForWood == 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_wood > max)
		{
			SetDesiredOutputToMoveToNeighbour(prevState,2);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() != cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		return;
	}

	/////////////////////////////////////////////////////////////////////


	if(currentResourceNearnessForFood == 1 && oldResourceNearnessForFood > 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_food > max)
		{
			SetDesiredOutputToMoveToNeighbour(prevState,2);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() == cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_state.m_neuralNetPoints++;
		return;
	}

	if (currentResourceNearnessForStone == 1 && oldResourceNearnessForStone > 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_stone > max)
		{
			SetDesiredOutputToMoveToNeighbour(prevState,2);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() == cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_state.m_neuralNetPoints++;
		return;
	}

	if (currentResourceNearnessForWood == 1 && oldResourceNearnessForWood > 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_wood > max)
		{
			SetDesiredOutputToMoveToNeighbour(prevState,2);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() == cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_state.m_neuralNetPoints++;
		return;
	}
	/////////////////////////////////////////////////////////////////////
	if (currentResourceNearnessForFood > 1 && oldResourceNearnessForFood == 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_food > max)
		{
			if (GetCordinates() != cords)
			{
				SetDesiredOutputToMoveToNeighbour(prevState,2);
				m_state.m_neuralNetPoints++;
				return;
			}
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() != cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (currentResourceNearnessForStone > 1 && oldResourceNearnessForStone == 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_stone > max)
		{
			if (GetCordinates() == cords)
			{
				SetDesiredOutputToMoveToNeighbour(prevState,2);
				m_state.m_neuralNetPoints++;
				return;
			}
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() != cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	if (currentResourceNearnessForWood > 1 && oldResourceNearnessForWood == 1)
	{
		if (((TownCenter*)FindMyTownCenter())->m_resourceStat.m_wood > max)
		{
			if (GetCordinates() != cords)
			{
				SetDesiredOutputToMoveToNeighbour(prevState,2);
				m_state.m_neuralNetPoints++;
				return;
			}
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		if (GetCordinates() != cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	/////////////////////////////////////////////////////////////////////

	if (currentResourceNearness - oldResourceNearness < 0) // moving towards
	{
		if (GetCordinates() == cords)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1.f);
			m_map->CreateExplosions(GetPosition(), color);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_state.m_neuralNetPoints++;
		return;
	}
	else if (currentResourceNearness - oldResourceNearness == 0)
	{
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_state.m_neuralNetPoints++;
		return;
	}
	else if (currentResourceNearness - oldResourceNearness > 0.f)
	{
		SetDesiredOutputToMoveToNeighbour(prevState,2);
		m_state.m_neuralNetPoints++;
		return;
	}
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
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "C", g_fontSize, GetTeamColor());
	

	//std::vector<Entity*> entityList = m_map->GetAllEntitiesFromPosition(GetPosition(), 1);
	//std::vector<Entity*> townCenterList = GetMyTownCenterEntityFromList(entityList);
	//std::vector<Entity*> resourceList = GetResourceEntityFromList(entityList);
	//
	//DebugDraw::GetInstance()->DebugRenderLogf("NEIGHBOURS %d %d %d", entityList.size(), townCenterList.size(), resourceList.size());

	//int cellDistance = m_map->GetCellDistance(FindMyTownCenter()->GetPosition(), GetPosition());
	//DebugDraw::GetInstance()->DebugRenderLogf("CELL DISTANCE : %d", cellDistance);
	delete textMaterial;
}
