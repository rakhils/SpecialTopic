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
#include "Game/GamePlay/Task/TaskDropResource.hpp"
#include "Game/GamePlay/Task/TaskBuildHouse.hpp"
#include "Game/GamePlay/Task/TaskBuildArmySpawner.hpp"
#include "Game/GamePlay/Task/TaskLongRangeAttack.hpp"
#include "Game/GamePlay/Task/TaskShortRangeAttack.hpp"
#include "Game/GamePlay/Task/TaskSpawnVillager.hpp"
#include "Game/GamePlay/Task/TaskSpawnClassAWarrior.hpp"
#include "Game/GamePlay/Task/TaskSpawnClassBWarrior.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
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
	m_neuralNet.CreateNeuralNetwork(m_map->m_maxHeight*m_map->m_maxWidth + g_extraNNInputs, 1000, static_cast<int>(m_taskTypeSupported.size() + 2));
	m_neuralNet.SetRandomWeight();
	// input + 6 -> for game stat && output + 2 for positions x,y
	for(int inputIndex = 0;inputIndex < m_map->m_maxHeight*m_map->m_maxWidth + g_extraNNInputs;inputIndex++)
	{
		m_lastInputState.push_back(0.f);
	}

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Trains NN with the snapshot input
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::TrainNN()
{
	/*m_neuralNet.FeedForward(m_lastInputState);
	m_neuralNet.DoBackPropogation()*/
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
			TaskType type = task->m_taskType;
			if(g_enableNeuralNet)
			{
				if (type == TASK_MOVE)
				{
					Vector2 pos = ((TaskMove*)task)->m_targetPosition;
					//DebugDraw::GetInstance()->DebugRenderLogf("TASK COMPLETED TYPE " + Task::GetTaskTypeAsString(type) + " POS X = " + ToString(pos.x) + " Y = " + ToString(pos.y), 1, Rgba::WHITE, Rgba::WHITE);
				}
				else
				{
					//DebugDraw::GetInstance()->DebugRenderLogf("TASK COMPLETED TYPE " + Task::GetTaskTypeAsString(type), 1, Rgba::WHITE, Rgba::WHITE);
				}
			}
			delete task;
			m_taskQueue.pop();
			if(g_enableNeuralNet)
			{
				if(m_teamID == 1)
				{
					int a = 1;
				}
				TrainNN();
				UpdateAllUnitStatWithCurrentValue();
				if (m_taskQueue.size() == 0  && m_type == CIVILIAN)
				{
					UpdateNN(deltaTime);
				}
			}
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
	if(!g_enableNeuralNet)
	{
		return;
	}
	if(m_type == RESOURCE_FOOD || m_type == RESOURCE_STONE || m_type == RESOURCE_WOOD || m_type == HOUSE)
	{
		return;	
	}

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
	UpdateLastNNInputState(m_map->m_minimapValue, m_gameStats);
	//m_neuralNet.FeedForward(m_map->m_minimapValue,m_gameStats);

	// not needed after training
	//m_neuralNet.SetRandomWeight();
	////
	TaskType task			= GetTaskFromNNOutput();
	IntVector2 taskPosition = GetTaskPositonFromNNOutput();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	deltaTime = logTime;
	switch (task)
	{
	case TASK_IDLE:
		{
			if (g_enableDebugPrints)
			{
				DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK IDLE");

			}
			CreateAndPushIdleTask(taskPosition);
			break;
		}
	case TASK_MOVE:
		{
			if(g_enableDebugPrints)
			{
				DebugDraw::GetInstance()->DebugRenderLogf(logTime,GetTeamColor(), "TASK MOVE POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
			}
			CreateAndPushMoveTask(taskPosition);
		}
		break;
	case TASK_GATHER_RESOURCE:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK GATHER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		logTime = deltaTime;
		CreateAndPushGatherResourceTask(taskPosition);
	}
		break;
	case TASK_DROP_RESOURCE:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK DROP POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		logTime = deltaTime;
		CreateAndPushDropResourceTask(taskPosition);
	}
		break;
	case TASK_BUILD_TOWNCENTER:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK BUILD TOWNCENTER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		logTime = deltaTime;
		CreateAndPushBuildTownCenterTask(taskPosition);
	}
		break;
	case TASK_BUILD_HOUSE:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK BUILD HOUSE POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		logTime = deltaTime;
		CreateAndPushBuildHouseTask(taskPosition);
	}
		break;
	case TASK_BUILD_ARMY_SPAWNER:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, GetTeamColor(), "TASK BUILD ARMY CENTER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		logTime = deltaTime;
		CreateAndPushBuildArmySpawnerTask(taskPosition);
	}
		break;
	case TASK_LONG_ATTACK:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK LONG ATTACK POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		CreateAndPushLongRangeAttackTask(taskPosition);
	}
		break;
	case TASK_SHORT_ATTACK:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SHORT ATTACK POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		CreateAndPushShortRangeAttackTask(taskPosition);
	}
		break;
	case TASK_SPAWN_VILLAGER:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SPAWN VILLAGER POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		CreateAndPushSpawnVillagerTask(taskPosition);
	}
		break;
	case TASK_SPAWN_CLASSA_WARRIOR:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SPAWN CLASSA POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		CreateAndPushSpawnClassAArmyTask(taskPosition);
	}
		break;
	case TASK_SPAWN_CLASSB_WARRIOR:
	{
		if (g_enableDebugPrints)
		{
			DebugDraw::GetInstance()->DebugRenderLogf(logTime, GetTeamColor(), "TASK SPAWN CLASS B POSX = %d POSY = %d", taskPosition.x, taskPosition.y);
		}
		CreateAndPushSpawnClassBArmyTask(taskPosition);
	}
		break;
	default:
		break;
	}
	if (g_enableDebugPrints)
	{
		DebugDraw::GetInstance()->DebugRenderLogf(debugNN,logTime, GetTeamColor(),GetTeamColor());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/29
*@purpose : Stores the snapshot at the time of task decision
*@param   : INputs of NN (minimap values and game stats)
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateLastNNInputState(std::vector<float> &minmapvalue, std::vector<float> &gameStat)
{
	for(int index = 0;index < m_lastInputState.size();index++)
	{
		if(index >= minmapvalue.size())
		{
			m_lastInputState.at(index) = gameStat.at(index - minmapvalue.size());
		}
		else
		{
			m_lastInputState.at(index) = minmapvalue.at(index);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateAllUnitStatWithCurrentValue()
{
	m_resourceFoodUsedBeforeTask			= m_resourceFoodUsed;
	m_resourceStoneUsedBeforeTask			= m_resourceStoneUsed;
	m_resourceWoodUsedBeforeTask			= m_resourceWoodUsed;

	m_resourceFoodGatheredBeforeTask		= m_resourceFoodGathered;
	m_resourceFoodDroppedBeforeTask			= m_resourceFoodDropped;
	m_resourceStoneGatheredBeforeTask		= m_resourceStoneGathered;
	m_resourceStoneDroppedBeforeTask		= m_resourceStoneDropped;
	m_resourceWoodGatheredBeforeTask		= m_resourceWoodGathered;
	m_resourceWoodDroppedBeforeTask			= m_resourceWoodDropped;
	m_numberOfArmySpawnerBuiltBeforeTask	= m_numberOfArmySpawnerBuilt;
	m_numberOfHouseBuiltBeforeTask			= m_numberOfHouseBuilt;

	m_longRangeArmySpawnedBeforeTask		= m_longRangeArmySpawned;
	m_shortRangeArmySpawnedBeforeTask		= m_shortRangeArmySpawned;

	m_enemiesAttackedBeforeTask				= m_enemiesAttacked;
	m_enemiesKilledBeforeTask				= m_enemiesKilled;

	m_villagerSpawnedBeforeTask				= m_villagerSpawned;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates unit stat for food gathered
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForFoodGathered(int count)
{
	m_resourceFoodGathered += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with stone gathered
*@param   : count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForStoneGathered(int count)
{
	m_resourceStoneGathered += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates with wood gathered
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForWoodGathered(int count)
{
	m_resourceWoodGathered += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates stat with food dropped
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForFoodDropped(int count)
{
	m_resourceFoodDropped += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with stone dropped
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForStoneDropped(int count)
{
	m_resourceStoneDropped += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with wood dropped
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForWoodDropped(int count)
{
	m_resourceWoodDropped += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update by army spawned built
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForArmySpawnerBuilt(int count)
{
	m_numberOfArmySpawnerBuilt += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with house built
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForHouseBuilt(int count)
{
	m_numberOfHouseBuilt += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with short army spawned
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForShortRangeArmySpawned(int count)
{
	m_shortRangeArmySpawned += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with long army spawned
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForLongRangeArmySpawned(int count)
{
	m_longRangeArmySpawned += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with unit stat for enemies attacked
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForEnemiesAttacked(int count)
{
	m_enemiesAttacked += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with enemies killed
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForEnemiesKilled(int count)
{
	m_enemiesKilled += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with villager spawned
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForVillagerSpawned(int count)
{
	m_villagerSpawned += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with food used
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForFoodUsed(int count)
{
	m_resourceFoodUsed += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with stone used
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForStoneUsed(int count)
{
	m_resourceStoneUsed += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates unit stat with wood used
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForWoodUsed(int count)
{
	m_resourceWoodUsed += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Gets the task index from type
*@param   : Task type
*@return  : index
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetIndexOfOutputTask(TaskType type)
{
	for(int index = 0;index < m_taskTypeSupported.size();index++)
	{
		if(m_taskTypeSupported.at(index) == type)
		{
			return index;
		}
	}
	return -1;
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
/*DATE    : 2018/09/30
*@purpose : Checks if any resource is near me
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsResourceNearMe(int cellDistance)
{
	std::vector<Entity*> entityList = GetAllEntitiesNearMe(cellDistance);
	for(int index = 0;index < entityList.size();index++)
	{
		if(entityList.at(index)->m_type == RESOURCE_FOOD || entityList.at(index)->m_type == RESOURCE_STONE || entityList.at(index)->m_type == RESOURCE_WOOD)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Get all entities of cell distance 
*@param   : Cell distance
*@return  : Entity list
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Entity::GetAllEntitiesNearMe(int cellDistance)
{
	std::vector<Entity*> entityList;
	IntVector2 cords = m_map->GetCordinates(GetPosition());
	Entity *entity   = m_map->GetEntityFromPosition(cords + IntVector2(cellDistance,0));
	if(entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(cellDistance,cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(0, cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(-cellDistance,cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(-cellDistance,0));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(-cellDistance, -cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(0, -cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = m_map->GetEntityFromPosition(cords + IntVector2(cellDistance, -cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	return entityList;
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
	float xPosition    = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size()).m_value;
	float yPosition    = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() + 1).m_value;
	float xRangedValue = RangeMapFloat(xPosition, -1.f, 1.f, 0.f, static_cast<float>(m_map->m_maxWidth));
	float yRangedValue = RangeMapFloat(yPosition, -1.f, 1.f, 0.f, static_cast<float>(m_map->m_maxHeight));
	return IntVector2(static_cast<int>(xRangedValue), static_cast<int>(yRangedValue));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Creates idle task and push to queue
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushIdleTask(IntVector2 cordinate)
{
	EmptyTaskQueue();
	Task *idleTask = new TaskIdle();
	m_taskQueue.push(idleTask);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates task to move to position and push to queue
*@param   : Move cordinate in map
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushMoveTask(IntVector2 cordinate)
{
	EmptyTaskQueue();
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *moveTask = new TaskMove(m_map, this, mapPosition);
	m_taskQueue.push(moveTask);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and push build house task at position
*@param   : Map cord where house should be built
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushBuildHouseTask(IntVector2 cordinate)
{
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskBuildHouse(m_map, this, mapPosition);
	if (task->CheckAndReduceResources())
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and pushes task to gather resources 
*@param   : Resource cords
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushGatherResourceTask(IntVector2 cordinate)
{
	if(m_map->GetEntityFromPosition(cordinate) == nullptr)
	{
		CreateAndPushIdleTask(IntVector2::ONE);
		return false;
	}
	EmptyTaskQueue();
	Task *task = new TaskGatherResource(this,m_map->GetEntityFromPosition(cordinate), FindMyTownCenter());
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and push drop resource task in queue
*@param   : 
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushDropResourceTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	EmptyTaskQueue();
	Task *task = new TaskDropResource(this, FindMyTownCenter());
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creats and pushes building town center task in queue
*@param   : Town center build location
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushBuildTownCenterTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and pushes build army spawner task into the queue
*@param   : Build location in map
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushBuildArmySpawnerTask(IntVector2 cordinate)
{
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskBuildArmySpawner(m_map, this, mapPosition);
	if (task->CheckAndReduceResources())
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and pushes long range attack task in queue
*@param   : Attack position in map
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushLongRangeAttackTask(IntVector2 cordinate)
{
	EmptyTaskQueue();
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskLongRangeAttack(m_map, this, m_map->GetTileIndex(mapPosition));
	if (task->m_isValid)
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushShortRangeAttackTask(IntVector2 cordinate)
{
	EmptyTaskQueue();
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskShortRangeAttack(m_map, this, m_map->GetTileIndex(mapPosition));
	if (task->m_isValid)
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and push task to push villager task in queue
*@param   : Spawn villager task
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushSpawnVillagerTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskSpawnVillager(m_map, (TownCenter*)this);
	if (task->CheckAndReduceResources())
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and push spawn class A army in queue
*@param   : Cords to spawn//(NOT NEEDED)
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushSpawnClassAArmyTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskSpawnClassAWarrior(m_map, this);
	if (task->CheckAndReduceResources())
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and push spawn class B army task
*@param   : Cords not needed
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushSpawnClassBArmyTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskSpawnClassBWarrior(m_map, this);
	if (task->CheckAndReduceResources())
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;
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
