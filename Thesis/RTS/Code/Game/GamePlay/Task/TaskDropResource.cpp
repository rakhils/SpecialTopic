#include "Game/GamePlay/Task/TaskDropResource.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/TownCenter.hpp"
// CONSTRUCTOR
TaskDropResource::TaskDropResource(Entity *entity,Entity* townCenter)
{
	m_entity	 = entity;
	m_townCenter = (TownCenter*)townCenter;
	m_map		 = m_entity->m_map;
	m_targetPosition = m_townCenter->GetPosition();
	m_taskType = TASK_DROP_RESOURCE;
	Vector2 townCenterPos = m_map->m_townCenters.at(m_entity->m_teamID - 1)->GetPosition();
	IntVector2 freeCords = m_map->GetFreeNeighbourTile(townCenterPos, 1);
	m_targetPosition = m_map->GetMapPosition(freeCords);
}

// DESTRUCTOR
TaskDropResource::~TaskDropResource()
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TaskDropResource::UpdateResourceStorageStat(Entity *entityResourceType, int count)
{
	switch (entityResourceType->m_type)
	{
	case RESOURCE_FOOD:
		m_townCenter->m_resourceStat.m_food += count;
		m_entity->UpdateUnitStatForFoodDropped(count);
		break;
	case RESOURCE_STONE:
		m_townCenter->m_resourceStat.m_stone += count;
		m_entity->UpdateUnitStatForStoneDropped(count);
		break;
	case RESOURCE_WOOD:
		m_townCenter->m_resourceStat.m_wood += count;
		m_entity->UpdateUnitStatForWoodDropped(count);
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
bool TaskDropResource::DoTask(float deltaTime)
{
	UNUSED(deltaTime);
	if(((Civilian*)m_entity)->m_resourceTypeCarrying == nullptr)
	{
		return true;
	}
	if (m_targetPosition == Vector2(-1, -1))
	{
		return true;
	}
	Vector2 currentPosition = m_entity->GetPosition();
	Vector2 direction = m_targetPosition - currentPosition;
	direction = direction.GetNormalized();
	currentPosition += direction * m_speed * deltaTime;
	m_entity->SetPositionInFloat(currentPosition);

	Vector2 distance = m_entity->GetPosition() - m_targetPosition;
	if (distance.GetLength() < 1)
	{
		UpdateResourceStorageStat(((Civilian*)m_entity)->m_resourceTypeCarrying, 1);
		CheckAndUpdateResourcesUsed();
		m_entity->m_state.m_hasResource = false;
		((Civilian*)m_entity)->m_resourceTypeCarrying = nullptr;
		return true;
	}
	return false;
	
	/*if (m_map->IsNeighbours(m_map->GetCordinates(m_townCenter->GetPosition()), m_map->GetCordinates(m_entity->GetPosition())))
	{
		UpdateResourceStorageStat(((Civilian*)m_entity)->m_resourceTypeCarrying, 1);
		CheckAndUpdateResourcesUsed();
		m_entity->m_state.m_hasResource = false;
		((Civilian*)m_entity)->m_resourceTypeCarrying = nullptr;
	}
	return true;*/
}
