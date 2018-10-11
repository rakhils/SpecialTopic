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
	m_taskQueue.push(new TaskGatherResource(this,m_map->m_resources.at(0),FindMyTownCenter()));
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
				//Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskGatherResource(this, entity, FindMyTownCenter());
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
	ClearDesiredOutputs();
	
	float evaluatedValue = 0.f;
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
	if(m_map->GetCordinates(previousState.m_position) == cords)
	{
		SetDesiredOutputToMoveToNeighbour();
		return;
	}

	float evaluatedValue     = 0.f;
	//float overallPoint       = 100.f;
	float townCenterNearness = 0.f;
	float resourceNearness   = 0.f;
	float xPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
	float yPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;
	if(m_resourceTypeCarrying != nullptr)
	{
		townCenterNearness = EvaluateMoveTaskToTownCenter(previousState,cords);
		SetDesiredOutputForTask(TASK_MOVE, townCenterNearness);
		if(townCenterNearness == 0.f)
		{
			SetDesiredOutputToMoveToSafeArea();
			return;
		}
		if(townCenterNearness > 0 && townCenterNearness < 1.f)
		{
			IntVector2 neighbour = m_map->GetRandomNeighbour(m_map->GetCordinates(previousState.m_position),1);
			xPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxWidth, 0, 1);
			yPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxHeight, 0, 1);
			SetDesiredOutputForTask(TASK_MOVEX, xPosition);
			SetDesiredOutputForTask(TASK_MOVEY, yPosition);
			m_map->CreateExplosions(m_map->GetMapPosition(neighbour));
			return;
		}
		m_map->CreateExplosions(m_map->GetMapPosition(GetTaskPositonFromNNOutput()));
		SetDesiredOutputForTask(TASK_MOVEX, xPosition);
		SetDesiredOutputForTask(TASK_MOVEY, yPosition);
		return;
	}
	resourceNearness = EvaluateMoveTaskToResource(previousState,cords);
	SetDesiredOutputForTask(TASK_MOVE, resourceNearness);
	if (resourceNearness == 0.f)
	{
		SetDesiredOutputToMoveToRandomArea();
		return;
	}
	if (resourceNearness > 0 && resourceNearness < 1.f)
	{
		IntVector2 neighbour = m_map->GetRandomNeighbour(m_map->GetCordinates(previousState.m_position),1);
		xPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxWidth, 0, 1);
		yPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxHeight, 0, 1);
		SetDesiredOutputForTask(TASK_MOVEX, xPosition);
		SetDesiredOutputForTask(TASK_MOVEY, yPosition);
		m_map->CreateExplosions(m_map->GetMapPosition(neighbour));
		return;
	}
	m_map->CreateExplosions(m_map->GetMapPosition(GetTaskPositonFromNNOutput()));

	SetDesiredOutputForTask(TASK_MOVEX, xPosition);
	SetDesiredOutputForTask(TASK_MOVEY, yPosition);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateIdleTask(EntityState prevState)
{
	SetDesiredOutputToMoveToNeighbour();
	SetDesiredOutputToRandomTask();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateGatherResourceTask(EntityState prevState,IntVector2 cords)
{
	std::vector<Entity*> resourceList = m_map->GetAllResourcesNearLocation(prevState.m_position, 1);
	Entity * resourceEntity = m_map->GetEntityFromPosition(cords);
	if(m_map->IsResource(resourceEntity))
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 1.f);
		return;
	}
	if(resourceList.size() > 0)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 0.5f);
		return;
	}
	SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 0.f);
	SetDesiredOutputToRandomTask();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateDropResourceTask(EntityState prevState, IntVector2 cords)
{
	std::vector<Entity*> entityList = m_map->GetAllEntitiesNearLocation(prevState.m_position, 1);
	std::vector<Entity*> mytownCenters = GetMyTownCenterEntityFromList(entityList);
	Entity *townCenter = m_map->GetEntityFromPosition(cords);
	if (townCenter != nullptr)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 1.f);
		return;
	}
	if (mytownCenters.size() > 0)
	{
		SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 0.5f);
		return;
	}
	SetDesiredOutputForTask(TASK_GATHER_RESOURCE, 0.f);
	SetDesiredOutputToRandomTask();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateBuildHouseTask(EntityState prevState, IntVector2 location)
{
	SetDesiredOutputForTask(TASK_BUILD_HOUSE, 0);
	SetDesiredOutputToMoveToNeighbour();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::EvaluateBuildArmySpawnerTask(EntityState prevState, IntVector2 location)
{
	SetDesiredOutputForTask(TASK_BUILD_ARMY_SPAWNER, 0);
	SetDesiredOutputToMoveToNeighbour();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::SetDesiredOutputToMoveToNeighbour()
{
	IntVector2 neighbour = m_map->GetRandomNeighbour(GetCordinates(),GetRandomIntLessThan(3));
	float xPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxWidth, 0, 1);
	float yPosition = RangeMapFloat(neighbour.y, 0, m_map->m_maxHeight, 0, 1);
	SetDesiredOutputForTask(TASK_MOVE, 1);
	SetDesiredOutputForTask(TASK_MOVEX, xPosition);
	SetDesiredOutputForTask(TASK_MOVEY, yPosition);
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
	float xPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxWidth, 0, 1);
	float yPosition = RangeMapFloat(neighbour.y, 0, m_map->m_maxHeight, 0, 1);
	SetDesiredOutputForTask(TASK_MOVE, 1);
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
	float xPosition = RangeMapFloat(neighbour.x, 0, m_map->m_maxWidth, 0, 1);
	float yPosition = RangeMapFloat(neighbour.y, 0, m_map->m_maxHeight, 0, 1);
	SetDesiredOutputForTask(TASK_MOVE, 1);
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
	int index = GetRandomIntLessThan(m_desiredOuputs.size());
	SetDesiredOutputForTask(m_taskTypeSupported.at(index), GetRandomFloatZeroToOne());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Civilian::EvaluateMoveTaskToTownCenter(EntityState prevState,IntVector2 cords)
{
	Entity * townCenter = FindMyTownCenter();
	int cellDistance = m_map->GetCellDistance(cords, townCenter->GetCordinates());
	int maxDistance = 5;
	if (cellDistance == 1)
	{
		return 1.f;
	}
	if (cellDistance < maxDistance)
	{
		return RangeMapFloat(static_cast<float>(cellDistance), 0.f, static_cast<float>(maxDistance), 1.f, 0.f);
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Civilian::EvaluateMoveTaskToResource(EntityState prevState,IntVector2 cords)
{
	float evaluatedValue = 0.f;
	float totalCount = 0;
	float maxRange = 7;
	for (int index = 0; index < m_map->m_resources.size(); index++)
	{
		int cellDistance = m_map->GetCellDistance(cords, m_map->GetCordinates(m_map->m_resources.at(index)->GetPosition()));
		if (cellDistance > maxRange)
		{
			continue;
		}
		if (cellDistance == 1)
		{
			return 1.f;
		}
		evaluatedValue += cellDistance;
		totalCount++;
	}
	if(totalCount == 0)
	{
		return 0.f;
	}
	evaluatedValue /= totalCount;
	return RangeMapFloat(evaluatedValue, 0, maxRange, 1, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : trains NeuralNet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainNN(Task *task)
{
	m_neuralNet.DoBackPropogation(m_desiredOuputs);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Start a task of gather resource
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::GatherResource(Vector2 resourcePosition)
{
	/*Task *task = new TaskGatherResource(this);
	((TaskGatherResource*)task)->SetResourcePosition(resourcePosition);
	((TaskGatherResource*)task)->SetStoragePosition(m_map->m_townCenters.at(0)->GetPosition());
	m_taskQueue.push(task);*/
}
