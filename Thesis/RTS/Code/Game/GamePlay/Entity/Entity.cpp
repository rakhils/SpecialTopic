#include "Entity.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"

#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskGatherResource.hpp"
#include "Game/GameCommon.hpp"
Entity::Entity()
{
	
}

Entity::Entity(float x, float y)
{
	SetPosition(Vector2(x, y));

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
/*DATE    : 2018/09/14
*@purpose : Gets tile index of position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetTileIndex()
{
	return m_map->GetTileIndex(GetPosition());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::InitNeuralNet()
{
	m_neuralNet.CreateNeuralNetwork(m_map->m_maxHeight*m_map->m_maxWidth + 6, 600, static_cast<int>(m_taskTypeSupported.size() + 2));// + 2 for positions
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		if (IsPositionInside(mousePosition))
		{
			if (g_currentSelectedEntity == nullptr)
			{
				g_currentSelectedEntity = this;
			}
			if (g_currentSelectedEntity->m_type == WARRIOR_SHORT_RANGE || g_currentSelectedEntity->m_type == WARRIOR_LONG_RANGE)
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
}

void Entity::Update(float deltaTime)
{
	if(m_health <= 0)
	{
		return;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Updates NN of entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateNN(float deltaTime)
{
	TownCenter *townCenter = m_map->m_townCenters.at(m_teamID - 1);
	if(townCenter == nullptr)
	{
		return;
	}

	std::vector<float> m_gameStats;
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<float>(townCenter->m_resourceStat.m_food)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<float>(townCenter->m_resourceStat.m_stone)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<float>(townCenter->m_resourceStat.m_wood)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<float>(townCenter->m_resourceStat.m_buildings)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<float>(townCenter->m_resourceStat.m_units)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<float>(townCenter->m_resourceStat.m_unitsKilled)));

	m_neuralNet.FeedForward(m_map->m_minimapValue,m_gameStats);

	m_neuralNet.SetRandomWeight();
	TaskType task			= GetTaskFromNNOutput();
	IntVector2 taskPosition = GetTaskPositonFromNNOutput();
	float logTime = 3;
	std::string debugNN;
	debugNN.append("     ");
	for(int index = 0;index < m_taskTypeSupported.size();index++)
	{
		debugNN.append(Task::GetTaskTypeAsString(m_taskTypeSupported.at(index)));
		debugNN.append(" = ");
		debugNN.append(ToString(m_neuralNet.m_outputs->m_neurons.at(index).m_value));
		debugNN.append(" :: ");
	}
	deltaTime = logTime;
	switch (task)
	{
	case TASK_MOVE:
		{
			DebugDraw::GetInstance()->DebugRenderLogf(logTime,GetTeamColor(), "TASK MOVE POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
			EmptyTaskQueue();
			Vector2 mapPosition = m_map->GetMapPosition(taskPosition);
			Task *moveTask = new TaskMove(m_map, this, mapPosition);
			m_taskQueue.push(moveTask);
		}
		break;
	case TASK_GATHER_RESOURCE:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(deltaTime,GetTeamColor(), "TASK GATHER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		logTime = deltaTime;
	}
		break;
	case TASK_DROP_RESOURCE:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK DROP POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		logTime = deltaTime;
	}
		break;
	case TASK_BUILD_TOWNCENTER:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK BUILD TOWNCENTER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		logTime = deltaTime;
	}
		break;
	case TASK_BUILD_HOUSE:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK BUILD HOUSE POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		logTime = deltaTime;
	}
		break;
	case TASK_BUILD_ARMY_SPAWNER:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK BUILD ARMY CENTER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		logTime = deltaTime;
	}
		break;
	case TASK_LONG_ATTACK:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK LONG ATTACK POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
	}
		break;
	case TASK_SHORT_ATTACK:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SHORT ATTACK POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
	}
		break;
	case TASK_SPAWN_VILLAGER:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SPAWN VILLAGER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
	}
		break;
	case TASK_SPAWN_CLASSA_WARRIOR:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SPAWN CLASSA POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
	}
		break;
	case TASK_SPAWN_CLASSB_WARRIOR:
	{
		DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SPAWN CLASS B POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
	}
		break;
	default:
		break;
	}
	DebugDraw::GetInstance()->DebugRenderLogf(debugNN,logTime, GetTeamColor(),GetTeamColor());
}

void Entity::Render()
{
	if (m_health <= 0)
	{
		return;
	}	
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	switch (m_type)
	{
	case CIVILIAN:
	case WARRIOR_SHORT_RANGE:
	case WARRIOR_LONG_RANGE:
		g_theRenderer->DrawCircle(m_disc,GetTeamColor());
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		g_theRenderer->DrawAABB(m_aabb2,GetTeamColor());
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
	UNUSED(index);
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
	if(m_teamID == 2)
	{
		return Rgba::RED;
	}
	return Rgba::YELLOW;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : returns corresponding minimap value 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Entity::GetMiniMapValue()
{
	switch (m_type)
	{
	case CIVILIAN:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.05f;
		break;
	case WARRIOR_SHORT_RANGE:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.1f;
		break;
	case WARRIOR_LONG_RANGE:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.15f;
		break;
	case HOUSE:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.20f;
		break;
	case TOWN_CENTER:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.25f;
		break;
	case RESOURCE_FOOD:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.30f;
		break;
	case RESOURCE_STONE:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.35f;
		break;
	case RESOURCE_WOOD:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.40f;
		break;
	case ARMY_SPAWNER:
		return (static_cast<float>(m_teamID) * 0.5f) + 0.45f;
		break;
	default:
		break;
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Gets the task type from NN outputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType Entity::GetTaskFromNNOutput()
{
	float max = 0.f;
	TaskType type = m_taskTypeSupported.at(0);
	for (int outputIndex = 0; outputIndex < m_taskTypeSupported.size(); outputIndex++)
	{
		if(m_neuralNet.m_outputs->m_neurons.at(outputIndex).m_value > max)
		{
			type = m_taskTypeSupported.at(outputIndex);
			max = m_neuralNet.m_outputs->m_neurons.at(outputIndex).m_value;
		}
	}
	return type;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Gets the position to execute from NN Output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetTaskPositonFromNNOutput()
{
	float xPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size()).m_value;
	float yPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() + 1).m_value;
	float xRangedValue = RangeMapFloat(xPosition, 0.f, 1.f, 0.f, static_cast<float>(m_map->m_maxWidth));
	float yRangedValue = RangeMapFloat(yPosition, 0.f, 1.f, 0.f, static_cast<float>(m_map->m_maxHeight));
	return IntVector2(static_cast<int>(xRangedValue), static_cast<int>(yRangedValue));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetEntityTypeAsString(EntityType entityType)
{
	switch (entityType)
	{
	case CIVILIAN:
		return "CIVILIAN";
		break;
	case WARRIOR_SHORT_RANGE:
		return "ARMY SHORT RANGE";
		break;
	case WARRIOR_LONG_RANGE:
		return "ARMY LONG RANGE";
		break;
	case HOUSE:
		return "HOUSE";
		break;
	case TOWN_CENTER:
		return "TOWN CENTER";
		break;
	case RESOURCE_FOOD:
		return "RESOURCE FOOD";
		break;
	case RESOURCE_STONE:
		return "RESOURCE STONE";
		break;
	case RESOURCE_WOOD:
		return "RESOURCE WOOD";
		break;
	case ARMY_SPAWNER:
		return "ARMY SPAWNER";
		break;
	default:
		break;
	}
	return "";
}
