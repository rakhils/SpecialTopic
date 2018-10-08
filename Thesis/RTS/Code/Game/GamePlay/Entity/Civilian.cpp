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
float Civilian::EvaluateNN(Task * task,EntityState previousState)
{
	ClearDesiredOutputs();
	int xPosition = static_cast<int>(m_desiredOuputs.size() - 2);
	int yPosition = static_cast<int>(m_desiredOuputs.size() - 1);

	std::vector<Entity*> entityList     = m_map->GetAllEntitiesFromPosition(previousState.m_position, 1);
	std::vector<Entity*> townCenterList = GetMyTownCenterEntityFromList(entityList);
	std::vector<Entity*> resourceList   = GetResourceEntityFromList(entityList);

	if(previousState.m_hasResource)
	{
		if(townCenterList.size() > 0)
		{
			SetOuputsToTrainToDropResource();
			return 1;
		}
		SetOuputsToTrainToMoveToPosition(m_map->GetFreeNeighbourTile(FindMyTownCenter()->GetPosition()));
		return 1;
	}

	if(resourceList.size() > 0)
	{
		SetOuputsToTrainToGatherResource();
		return 1;
	}
	if(townCenterList.size() > 0)
	{
		SetOuputsToTrainToMoveToPosition(m_map->GetFreeNeighbourTile(m_map->m_resources.at(0)->GetPosition()));
		return 1;
	}
	SetOuputsToTrainToMoveToPosition(m_map->GetFreeNeighbourTile(FindMyTownCenter()->GetPosition()));
	return 1;












/*
	float evaluatedValue = 0.f;
	switch (task->m_taskType)
	{
	case TASK_MOVE:
		evaluatedValue = EvaluateMoveTask(previousState);
		break;
	case TASK_GATHER_RESOURCE:
		evaluatedValue = EvaluateGatherResourceTask(previousState);
		break;
	case TASK_DROP_RESOURCE:
		evaluatedValue = EvaluateDropResourceTask(previousState);
		break;
	case TASK_IDLE:
		return 0.f;
		break;
	default:
		break;
	}
	
	return 0.f;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Civilian::EvaluateMoveTask(EntityState previousState)
{
	float evaluatedValue = 0.f;
	if (previousState.m_hasResource)
	{
		evaluatedValue = EvaluateDropResourceTask(previousState);
		if(evaluatedValue == 0)
		{
			return EvaluateOnRandomMoveTask(previousState);
		}
		
		return evaluatedValue;
	}
	evaluatedValue = EvaluateGatherResourceTask(previousState);
	if(evaluatedValue == 0)
	{
		return EvaluateOnRandomMoveTask(previousState);
	}
	return evaluatedValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Civilian::EvaluateOnRandomMoveTask(EntityState prevState)
{
	int xPosition = static_cast<int>(m_desiredOuputs.size() - 2);
	int yPosition = static_cast<int>(m_desiredOuputs.size() - 1);

	std::vector<Entity*> entityList = GetAllEntitiesNearMe(1);
	std::vector<Entity*> townCenterList = GetTownCenterEntityFromList(entityList);
	for(int index = 0;index < townCenterList.size();index++)
	{
		if(townCenterList.at(index)->m_teamID == m_teamID)
		{
			Entity* resource = m_map->m_resources.at(0);
			IntVector2 entityPosition = m_map->GetFreeNeighbourTile(resource->GetPosition());
			m_desiredOuputs.at(xPosition) = RangeMapFloat(entityPosition.x, 0, m_map->m_maxWidth, 0, 1);
			m_desiredOuputs.at(yPosition) = RangeMapFloat(entityPosition.y, 0, m_map->m_maxHeight, 0, 1);

			int taskIndex = GetIndexOfOutputTask(TASK_MOVE);
			m_desiredOuputs.at(taskIndex) = 1;
			return 1;
		}
	}

	IntVector2 townCenterPosition = m_map->GetFreeNeighbourTile(FindMyTownCenter()->GetPosition());

	m_desiredOuputs.at(xPosition) = RangeMapFloat(townCenterPosition.x, 0, m_map->m_maxWidth, 0, 1);
	m_desiredOuputs.at(yPosition) = RangeMapFloat(townCenterPosition.y, 0, m_map->m_maxHeight, 0, 1);
	int taskIndex = GetIndexOfOutputTask(TASK_MOVE);
	m_desiredOuputs.at(taskIndex) = 1;
	return 1.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Civilian::EvaluateIdleTask(EntityState prevState)
{
	int xPosition = static_cast<int>(m_desiredOuputs.size() - 2);
	int yPosition = static_cast<int>(m_desiredOuputs.size() - 1);
	if(prevState.m_hasResource)
	{
		std::vector<Entity*> entityList = GetAllEntitiesNearMe(1);
		std::vector<Entity*> townCenterList = GetTownCenterEntityFromList(entityList);
		for (int index = 0; index < townCenterList.size(); index++)
		{
			if (townCenterList.at(index)->m_teamID == m_teamID)
			{
				Entity* resource = m_map->m_resources.at(0);
				IntVector2 entityPosition = m_map->GetFreeNeighbourTile(resource->GetPosition());
				m_desiredOuputs.at(xPosition) = RangeMapFloat(entityPosition.x, 0, m_map->m_maxWidth, 0, 1);
				m_desiredOuputs.at(yPosition) = RangeMapFloat(entityPosition.y, 0, m_map->m_maxHeight, 0, 1);

				int taskIndex = GetIndexOfOutputTask(TASK_MOVE);
				m_desiredOuputs.at(taskIndex) = 1;
				return 1;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : trains NeuralNet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainNN(Task *task,float evaluationValue)
{
	m_neuralNet.DoBackPropogation(m_desiredOuputs);
	Entity::TrainNN(task,evaluationValue);
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
