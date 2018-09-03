#include "Entity.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskGatherResource.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity()
{

}

Entity::Entity(float x, float y)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Gets the current position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Entity::GetPosition()
{
	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		return m_disc.center;
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		return m_aabb2.GetCenter();
		break;
	default:
		break;
	}
	return Vector2::ZERO;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ProcessInputs(float deltaTime)
{

}

void Entity::Update(float deltaTime)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y		  = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	if(InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		if(IsPositionInside(mousePosition))
		{
			if(g_currentSelectedEntity == nullptr)
			{
				g_currentSelectedEntity = this;
			}
			if(g_currentSelectedEntity->m_type == WARRIOR_SHORT_RANGE || g_currentSelectedEntity->m_type == WARRIOR_LONG_RANGE)
			{
				switch (this->m_type)
				{
				case RESOURCE_FOOD:
				case RESOURCE_STONE:
				case RESOURCE_WOOD:
				case ARMY_SPAWNER:
				case TOWN_CENTER:
				case HOUSE:
					g_currentSelectedEntity = this;
					break;
				default:
					break;
				}
			}
			else
			{
				switch (this->m_type)
				{
				case RESOURCE_FOOD:
				case RESOURCE_STONE:
				case RESOURCE_WOOD:
					break;
				case ARMY_SPAWNER:
				case TOWN_CENTER:
				case HOUSE:
				case CIVILIAN:
				case WARRIOR_LONG_RANGE:
				case WARRIOR_SHORT_RANGE:
					g_currentSelectedEntity = this;
					break;
				default:
					break;
				}
			}
		}
	}
	
	if(!m_taskQueue.empty())
	{
		bool isCompleted = (m_taskQueue.front())->DoTask(deltaTime);
		if(isCompleted)
		{
			Task *task = m_taskQueue.front();
			delete task;
			m_taskQueue.pop();
		}
	}
}

void Entity::Render()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		g_theRenderer->DrawCircle(m_disc,m_color);
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		g_theRenderer->DrawAABB(m_aabb2,m_color);
		break;
	default:
		break;
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Sets team
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetTeam(int team)
{
	m_teamID = team;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Sets the position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetPosition(Vector2 position)
{
	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		m_disc.center = m_map->GetMapPosition(m_map->GetTileIndex(position));
		m_disc.radius = g_radius;
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		m_aabb2		  = AABB2(m_map->GetMapPosition(m_map->GetTileIndex(position)), g_radius, g_radius);
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetPosition(int index)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetPositionInFloat(Vector2 position)
{
	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		m_disc.center = position;
		m_disc.radius = g_radius;
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		m_aabb2 = AABB2(position, g_radius, g_radius);
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Gets color for current team
*@param   : NIL
*@return  : Rgba 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Rgba Entity::GetTeamColor()
{
	if(m_teamID == 1)
	{
		return Rgba::GREEN;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates the current task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateTask()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Deletes all task in queue
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::EmptyTaskQueue()
{
	while(!m_taskQueue.empty())
	{
		Task *task = m_taskQueue.front();
		delete task;
		m_taskQueue.pop();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/02
*@purpose : Finds my town center
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Entity::FindMyTownCenter()
{
	for(int index = 0;index < m_map->m_townCenters.size();index++)
	{
		if(m_map->m_townCenters.at(index)->m_teamID == m_teamID)
		{
			return m_map->m_townCenters.at(index);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Takes damage
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::TakeDamage(float hitPoint)
{
	m_health -= hitPoint;
	if(m_health <= 0)
	{

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Check if the position is inside
*@param   : Position
*@return  : if position inside entity true else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsPositionInside(Vector2 position)
{
	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		return m_disc.IsPointInside(position);
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		return m_aabb2.IsPointInside(position);
		break;
	default:
		break;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Check if the entity is movable
*@param   : NIL
*@return  : True if movable else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsMovalbleObject()
{
	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		return true;
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		return false;
		break;
	default:
		break;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Checks if both entities are different teams
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsDifferentFaction(Entity *entity)
{
	if(m_teamID == entity->m_teamID)
	{
		return false;
	}
	return true;
}
