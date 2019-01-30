#include "Game/GamePlay/Task/TaskGatherResource.hpp"

#include "Engine/Debug/DebugDraw.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/TownCenter.hpp"
// CONSTRUCTOR
TaskGatherResource::TaskGatherResource(Entity *entity,TaskType type)
{
	m_entity     = entity;
	//m_resource   = resource;
	m_townCenter = (TownCenter*)entity->FindMyTownCenter();
	m_map        = entity->m_map;
	SetStoragePosition(m_townCenter->GetPosition());
	m_taskType = type;
	int cellDistance = 100;
	m_targetPosition = Vector2(-1, -1);
	EntityType entityType;
	if(type == TASK_GATHER_RESOURCE_FOOD)
	{
		entityType = RESOURCE_FOOD;
	}
	if(type == TASK_GATHER_RESOURCE_STONE)
	{
		entityType = RESOURCE_STONE;
	}
	if (type == TASK_GATHER_RESOURCE_WOOD)
	{
		entityType = RESOURCE_WOOD;
	}
	for(int index = 0;index < m_map->m_resources.size();index++)
	{
		if(m_map->m_resources.at(index)->m_type == entityType)
		{
			int cellDistanceNew = m_map->GetCellDistance(m_entity->GetCordinates(), m_map->m_resources.at(index)->GetCordinates());
			if(cellDistanceNew < cellDistance)
			{
				Vector2 resourcePosition = m_map->m_resources.at(index)->GetPosition();
				IntVector2 resourceCords = m_map->GetFreeNeighbourTile(resourcePosition,1);
				if(resourceCords != IntVector2(-1,-1))
				{
					m_resource = m_map->m_resources.at(index);
					m_targetPosition = m_map->GetMapPosition(resourceCords);
					cellDistance = cellDistanceNew;
				}
			}
		}
	}
}

// DESTRUCTOR
TaskGatherResource::~TaskGatherResource()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Sets the final position
*@param   : Target
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskGatherResource::SetResourcePosition(Vector2 position)
{
	m_resourcePosition = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Sets storage position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskGatherResource::SetStoragePosition(Vector2 position)
{
	m_storagePosition = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Waits to gather resource
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskGatherResource::WaitAtResource(float deltaTime)
{
	m_waitTimeAtResource += deltaTime;
	if(m_waitTimeAtResource > m_maxWaitTimeAtResource)
	{
		m_waitTimeAtResource = 0.f;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskGatherResource::UpdateResourceStorageStat(Entity* entity,int count)
{
	switch (entity->m_type)
	{
	case RESOURCE_FOOD:
		entity->UpdateUnitStatForFoodGathered(count);
		break;
	case RESOURCE_STONE:
		entity->UpdateUnitStatForStoneGathered(count);
		break;
	case RESOURCE_WOOD:
		entity->UpdateUnitStatForWoodGathered(count);
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Moves object from point A to B
*@param   : Delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TaskGatherResource::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	if(((Civilian*)m_entity)->m_resourceTypeCarrying != nullptr)
	{
		return true;
	}
	if(m_targetPosition == Vector2(-1,-1))
	{
		return true;
	}
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction = m_targetPosition - currentPosition;
	direction = direction.GetNormalized();
	currentPosition += direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	if(m_map->m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN)
	{
		m_entity->SetPosition(m_targetPosition);
	}
	Vector2 distance = m_entity->GetPosition() - m_targetPosition;

	if (distance.GetLength() < 1)
	{
		UpdateResourceStorageStat(m_entity, 1);
		((Civilian*)m_entity)->m_resourceTypeCarrying = m_resource;
		CheckAndUpdateResourcesUsed();
		return true;
	}
	return false;


	/*std::vector<Entity*> resourceList = m_map->GetAllResourcesNearLocation(m_entity->GetPosition(), 1);
	if(resourceList.size() > 0)
	{
		UpdateResourceStorageStat(resourceList.at(0), 1);
		((Civilian*)m_entity)->m_resourceTypeCarrying = resourceList.at(0);
		CheckAndUpdateResourcesUsed();
	}
	return true;*/

	/*Vector2 targetPosition;
	switch (m_subTask)
	{
	case MOVE_TOWARDS_RESOURCE:
		targetPosition = m_resourcePosition;
		break;
	case GATHER_RESOURCE:
		if(WaitAtResource(deltaTime))
		{
			m_subTask = MOVE_TOWARDS_STORAGE;
		}
		return false;
	case MOVE_TOWARDS_STORAGE:
		targetPosition = m_storagePosition;
		break;
	default:
		break;
	}

	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction		= targetPosition - currentPosition;
	direction				= direction.GetNormalized();
	currentPosition			+= direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	Vector2 distance = m_entity->GetPosition() - targetPosition;
	if (distance.GetLength() < 1)
	{
		switch (m_subTask)
		{
		case MOVE_TOWARDS_RESOURCE:
			m_subTask = GATHER_RESOURCE;
			((Civilian*)m_entity)->m_resourceType = nullptr;
			break;
		case MOVE_TOWARDS_STORAGE:
			((Civilian*)m_entity)->m_resourceType = m_resource;
			UpdateResourceStorageStat(m_resource,1);
			m_subTask = MOVE_TOWARDS_RESOURCE;
			break;
		default:
			break;
		}
	}
	return false;*/
}
