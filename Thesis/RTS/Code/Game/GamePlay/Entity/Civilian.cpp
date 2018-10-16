#include "Game/GamePlay/Entity/Civilian.hpp"

#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Debug/DebugDraw.hpp"

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
				EmptyTaskQueue();
				Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskMove(m_map,this,mapPosition);
				m_taskQueue.push(task);
			}
			else if(entity != nullptr && (entity->m_type == RESOURCE_FOOD || entity->m_type == RESOURCE_STONE|| entity->m_type == RESOURCE_WOOD))
			{
				EmptyTaskQueue();
				Task *task = new TaskGatherResource(this);
				m_taskQueue.push(task);
			}
			else if(entity != nullptr && (entity->m_type == TOWN_CENTER))
			{
				EmptyTaskQueue();
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

	//EvaluateMoveTask(previousState,cords);
	//EvaluateGatherResourceTask(previousState,cords);
	//EvaluateDropResourceTask(previousState,cords);
	//EvaluateBuildHouseTask(previousState, cords);
	//EvaluateBuildArmySpawnerTask(previousState, cords);
	//EvaluateIdleTask(previousState);

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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateMoveTask(EntityState previousState,IntVector2 cords)
{
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Finds the best neighbour
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Civilian::GetBestNeighbour()
{
	std::vector<IntVector2> neighbours = m_map->GetAllNeighbourCoordinates(GetCordinates(), 1);
	float townCenterNearnessValue = static_cast<float>(GetMax(m_map->m_maxHeight,m_map->m_maxWidth)) + 1.f;
	float resourceNearnessValue   = static_cast<float>(GetMax(m_map->m_maxHeight,m_map->m_maxWidth)) + 1.f;
	IntVector2 bestNeighbour;

	for(int index = 0;index < neighbours.size();index++)
	{
		CellSensoryValues cellValue = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(neighbours.at(index)));
		if(HasResource())
		{
			if(townCenterNearnessValue > cellValue.m_townCenter2Nearness)
			{
				if (m_map->GetEntityFromPosition(cellValue.m_coords) == nullptr)
				{
					townCenterNearnessValue = cellValue.m_townCenter2Nearness;
					bestNeighbour			= cellValue.m_coords;
				}
			}
		}
		else
		{
			if(resourceNearnessValue > cellValue.m_resourceNearnessForFood)
			{
				if(m_map->GetEntityFromPosition(cellValue.m_coords) == nullptr)
				{
					resourceNearnessValue = cellValue.m_resourceNearnessForFood;
					bestNeighbour		  = cellValue.m_coords;
				}
			}
		}
	}
	return bestNeighbour;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToMoveToNeighbour(int distance)
{
	UNUSED(distance);
	IntVector2 neighbour = GetBestNeighbour();
	float xPosition = RangeMapFloat(static_cast<float>(neighbour.x), 0.f, static_cast<float>(m_map->m_maxWidth-1),  0.f, 1.f);
	float yPosition = RangeMapFloat(static_cast<float>(neighbour.y), 0.f, static_cast<float>(m_map->m_maxHeight-1), 0.f, 1.f);
	SetDesiredOutputForTask(TASK_MOVE, 1);
	SetDesiredOutputForTask(TASK_MOVEX, xPosition);
	SetDesiredOutputForTask(TASK_MOVEY, yPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToMoveToNeighbour()
{
	SetDesiredOutputToMoveToNeighbour(4);
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
	m_map->CreateExplosions(m_map->GetMapPosition(neighbour));
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
	m_map->CreateExplosions(m_map->GetMapPosition(neighbour));
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
void Civilian::SetDesiredOuputForMoveToTownCenterTask(float value,EntityState previousState,IntVector2 cords)
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
	float tcNearnessCurrent = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_townCenter2Nearness;
	float tcNearnessOld = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_townCenter2Nearness;
	if (tcNearnessCurrent - tcNearnessOld > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	else if(tcNearnessCurrent - tcNearnessCurrent == 0)
	{
		if(tcNearnessCurrent == 0.f)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		else
		{
			SetDesiredOutputToMoveToNeighbour(1);
			m_state.m_neuralNetPoints++;
			return;
		}
	}
	else if(tcNearnessCurrent - tcNearnessCurrent < 0.f)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
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
	float resourceNearnessCurrent = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_resourceNearnessForFood;
	float resourceNearnessOld = m_map->m_cellSensoryValues.at(m_map->GetTileIndex(cords)).m_resourceNearnessForFood;
	if (resourceNearnessCurrent - resourceNearnessOld > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 1.f);
		m_state.m_neuralNetPoints++;
		return;
	}
	else if (resourceNearnessCurrent - resourceNearnessCurrent == 0)
	{
		if (resourceNearnessCurrent == 0.f)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0.f);
			m_state.m_neuralNetPoints++;
			return;
		}
		else
		{
			SetDesiredOutputToMoveToNeighbour(1);
			m_state.m_neuralNetPoints++;
			return;
		}
	}
	else if (resourceNearnessCurrent - resourceNearnessCurrent < 0.f)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0.f);
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
