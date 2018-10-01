#include "Game/GamePlay/Task/TaskGatherResource.hpp"

#include "Engine/Debug/DebugDraw.hpp"
#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/TownCenter.hpp"
// CONSTRUCTOR
TaskGatherResource::TaskGatherResource(Entity *entity,Entity *resource,Entity* townCenter)
{
	m_entity     = entity;
	m_resource   = resource;
	m_townCenter = (TownCenter*)townCenter;
	m_map        = entity->m_map;
	m_targetPosition = m_resource->GetPosition();
	SetStoragePosition(m_townCenter->GetPosition());
	SetResourcePosition(resource->GetPosition());
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
void TaskGatherResource::UpdateResourceStorageStat(Entity *entityResourceType,int count)
{
	switch (entityResourceType->m_type)
	{
	case RESOURCE_FOOD:
		m_entity->UpdateUnitStatForFoodGathered(count);
		break;
	case RESOURCE_STONE:
		m_entity->UpdateUnitStatForStoneGathered(count);
		break;
	case RESOURCE_WOOD:
		m_entity->UpdateUnitStatForWoodGathered(count);
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
	if (m_map->IsNeighbours(m_map->GetCordinates(m_resource->GetPosition()), m_map->GetCordinates(m_entity->GetPosition())))
	{
		((Civilian*)m_entity)->m_resourceType = m_resource;
		DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, m_entity->GetTeamColor(), "TASK GATHERED RESOURCE");
		UpdateResourceStorageStat(((Civilian*)m_entity)->m_resourceType,1);
	}
	return true;

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
