#include "Entity.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/AI/NeuralNetwork/NeuralNetwork.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"

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
#include "Game/GamePlay/Task/TaskDefense.hpp"
#include "Game/GamePlay/Task/TaskAttack.hpp"
#include "Game/GamePlay/Task/TaskPatrol.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
#include "Game/GamePlay/Task/TaskRetreat.hpp"
#include "Game/GamePlay/Task/TaskFollow.hpp"
#include "Game/GamePlay/Task/TaskExplore.hpp"
#include "Game/GameCommon.hpp"

Entity::Entity()
{
	
}

Entity::Entity(float x, float y)
{
	SetPosition(Vector2(x, y));
}

Entity::~Entity()
{
	while (!m_taskQueue.empty())
	{
		Task *task = m_taskQueue.front();
		m_taskQueue.pop();
		delete task;
	}
	m_taskTypeSupported .clear();
	m_desiredOuputs		.clear();
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
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
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
/*DATE    : 2018/10/10
*@purpose : Gets the cordinate and returns
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetCordinates()
{
	return m_map->GetCordinates(GetPosition());
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
	m_neuralNet.CreateNeuralNetwork(g_NNInputCount, g_NNHiddenLayerCount, static_cast<int>(m_taskTypeSupported.size()));

	bool loadSuccess = m_neuralNet.LoadFromFile(GetGlobalBestFilePath().c_str());
	if(!loadSuccess)
	{
		m_neuralNet.SetRandomWeight();
	}

	for(int index = 0;index < m_neuralNet.m_outputs->m_neurons.size();index++)
	{
		m_desiredOuputs.push_back(0.f);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : Init all state values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::InitStates()
{
	m_state.m_health   = m_health;
	m_state.m_position = GetPosition();
	m_previousState    = m_state;
	//m_scoreBoard.m_entity = this;

	for (int index = 0; index < 15; index++)
	{
		m_state.m_favoredMoveTaskCount.push_back(0);
		m_previousState.m_favoredMoveTaskCount.push_back(0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::InitPersonality()
{
	switch (m_map->m_personality)
	{
	case AGGRESSIVE:
	{
		m_strategies.push_back(CheckAndSetAttackStrategyIfNoEnemiesAliveTo);
		m_strategies.push_back(CheckAndSetExploreStrategyIfNoEnemiesAliveToExplore);
		m_strategies.push_back(CheckAndSetStrategyAttackIfTownCenterUnderAttack);
		m_strategies.push_back(CheckAndSetStrategyRetreatIfTownCenterUnderAttack);
		m_strategies.push_back(CheckAndSetAttackStrategyIfEntityUnderAttack);
		m_strategies.push_back(CheckAndSetRetreatStrategyIfEntityUnderAttack);
		m_strategies.push_back(CheckAndSetAttackStrategyIfEnemiesOutweighs);
		m_strategies.push_back(CheckAndSetRetreatStrategyIfEnemiesOutweighs);
		m_strategies.push_back(CheckAndSetStrategyPatrol);
		m_strategies.push_back(CheckAndSetStrategyExplore);
		m_strategies.push_back(CheckAndSetStrategyAttack);
	}
		break;
	case DEFENSIVE:
		m_strategies.push_back(CheckAndSetStrategyAttackIfTownCenterUnderAttack);
		m_strategies.push_back(CheckAndSetStrategyRetreatIfTownCenterUnderAttack);
		m_strategies.push_back(CheckAndSetRetreatStrategyIfEntityUnderAttack);
		m_strategies.push_back(CheckAndSetRetreatStrategyIfEnemiesOutweighs);
		m_strategies.push_back(CheckAndSetAttackStrategyIfEntityUnderAttack);
		m_strategies.push_back(CheckAndSetStrategyPatrol);
		break;
	default:
		break;
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

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_S))
	{
		if(g_currentSelectedEntity == this)
		{
			m_neuralNet.StoreToFile(GetGlobalBestFilePath().c_str());
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_D))
	{
		if(g_currentSelectedEntity == this)
		{
			m_health = 0;
			g_currentSelectedEntity = nullptr;
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_SPACE))
	{
		if(g_currentSelectedEntity == this)
		{
			m_miniMapDebug = m_miniMapDebug == true ? false : true;
		}
	}
	if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		if (IsPositionInside(mousePosition))
		{
			if (g_currentSelectedEntity == nullptr)
			{
				g_currentSelectedEntity = this;
			}
			/*if (g_currentSelectedEntity->m_type == SHORT_RANGE_ARMY || g_currentSelectedEntity->m_type == LONG_RANGE_ARMY)
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
				case LONG_RANGE_ARMY:
				case SHORT_RANGE_ARMY:
					g_currentSelectedEntity = this;
					break;
				default:
					break;
				}
			}*/
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Trains NN with the snapshot input
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::TrainNN(Task *task)
{
	PrintDebugNN();

	if(!m_map->HasTrainingEnabled(this))
	{
		return;
	}

	UNUSED(task);
	for (int index = 0; index < m_taskTypeSupported.size(); index++)
	{
		if (m_neuralNet.m_outputs->m_neurons.at(index).m_sumOfPreviousLayer < -5.f)
		{
			m_desiredOuputs.at(index) = 1.01;
		}
		if (m_neuralNet.m_outputs->m_neurons.at(index).m_sumOfPreviousLayer > 5.f)
		{
			m_desiredOuputs.at(index) = 0.001;
		}
	}

	if (m_state.m_neuralNetPoints > m_previousState.m_neuralNetPoints)
	{
		for (int index = 0; index < 3; index++)
		{
			m_neuralNet.DoBackPropogation(m_desiredOuputs);
		}
	}
	m_neuralNetTrainCount++;
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
			task->UpdateScoreCard();
			m_taskQueue.pop();
			if(m_taskQueue.size() == 0)
			{
				EvaluateNN(task,m_previousState,GetTaskPositonFromNNOutput(m_previousState.m_position));
				TrainNN(task);
				UpdateEntityState();
				UpdateNN(deltaTime);
				UpdateTaskFromNN(deltaTime);
			}
			delete task;
		}
	}
	UpdateCoveredArea();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateCoveredArea()
{
	/*if (g_skipRendering)
	{
		return;
	}*/
	if(m_type == SHORT_RANGE_ARMY || m_type == LONG_RANGE_ARMY)
	{
		IntVector2 minimapMins = GetMiniMapMins();
		IntVector2 minimapMaxs = GetMiniMapMaxs();
		for(int indexX = minimapMins.x;indexX < minimapMaxs.x;indexX++)
		{
			for (int indexY = minimapMins.y; indexY < minimapMaxs.y; indexY++)
			{
				int index = indexY * m_map->m_maxWidth + indexX;
				if(m_map->m_tiles.size() > index)
				{
					if(m_teamID == 1)
					{
						m_map->m_tiles.at(index)->SetCovered(1);
					}
					else
					{
						m_map->m_tiles.at(index)->SetCovered(2);
					}
					m_map->m_tiles.at(index)->m_teamID = m_teamID;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::EvaluateNN(Task * type,EntityState previousState,IntVector2 cords)
{
	UNUSED(previousState);
	UNUSED(cords);
	UNUSED(type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Updates NN of entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateNN(float deltaTime)
{
	UNUSED(deltaTime);
	
	if(!m_map->HasFeedForwardEnabled(this))
	{
		return;
	}

	std::vector<double> m_gameStats;
	NNInputs inputs = GetMyNNInputs();
	m_gameStats.push_back(inputs.m_allyArmySpawnerCount				);
	m_gameStats.push_back(inputs.m_allyCiviliansCount				);
	m_gameStats.push_back(inputs.m_allyLongRangeArmyCount			);
	m_gameStats.push_back(inputs.m_allyShortRangeArmyCount			);
	m_gameStats.push_back(inputs.m_allyHouseCount					);
	m_gameStats.push_back(inputs.m_allyTownCenterHealth				);
																	
	m_gameStats.push_back(inputs.m_enemyArmySpawnerCount			);
	m_gameStats.push_back(inputs.m_enemyCiviliansCount				);
	m_gameStats.push_back(inputs.m_enemyHouseCount					);
	m_gameStats.push_back(inputs.m_enemyLongRangeArmyCount			);
	m_gameStats.push_back(inputs.m_enemyShortRangeArmyCount			);
	m_gameStats.push_back(inputs.m_enemyTownCenterHealth			);
																	
	m_gameStats.push_back(inputs.m_inRangeAllyArmySpawnerCount		);
	m_gameStats.push_back(inputs.m_inRangeAllyCiviliansCount		);
	m_gameStats.push_back(inputs.m_inRangeAllyLongRangeArmyCount	);
	m_gameStats.push_back(inputs.m_inRangeAllyShortRangeArmyCount	);
	m_gameStats.push_back(inputs.m_inRangeAllyHouseCount			);
	m_gameStats.push_back(inputs.m_inRangeAllyTownCenterCount		);
	m_gameStats.push_back(inputs.m_inRangeAttackingAllyHeatMapValue	);
	m_gameStats.push_back(inputs.m_inRangeStationaryAllyHeatMapValue);
																	
	m_gameStats.push_back(inputs.m_inRangeEnemyArmySpawnerCount		);
	m_gameStats.push_back(inputs.m_inRangeEnemyCiviliansCount		);
	m_gameStats.push_back(inputs.m_inRangeEnemyHouseCount			);
	m_gameStats.push_back(inputs.m_inRangeEnemyLongRangeArmyCount	);
	m_gameStats.push_back(inputs.m_inRangeEnemyShortRangeArmyCount	);
	m_gameStats.push_back(inputs.m_inRangeEnemyTownCenterCount		);
	m_gameStats.push_back(inputs.m_inRangeAttackingEnemyHeatMapValue);
	m_gameStats.push_back(inputs.m_inRangeStationaryEnemyHeatMapValue);

	m_gameStats.push_back(inputs.m_allyInAttackMode					);
	m_gameStats.push_back(inputs.m_allyInExploreMode				);
	m_gameStats.push_back(inputs.m_allyInFollowMode					);
	m_gameStats.push_back(inputs.m_allyInPatrolMode					);
	m_gameStats.push_back(inputs.m_allyInRetreatMode				);

	m_gameStats.push_back(inputs.m_resourceFoodCount				);
	m_gameStats.push_back(inputs.m_resourceStoneCount				);
	m_gameStats.push_back(inputs.m_resourceWoodCount				);														
																	
	m_gameStats.push_back(inputs.m_health							);
	m_gameStats.push_back(inputs.m_resourceCarrying					);

	m_neuralNet.FeedForward(m_gameStats);
}

void Entity::UpdateTaskFromNN(float deltaTime)
{
	if(m_map->HasHumanBehaviorEnabled(this))
	{
		return;
	}

	if (m_map->HasRandomBehaviorEnabled(this))
	{
		int randomTask = GetRandomIntLessThan(m_taskTypeSupported.size());
		TaskType task = m_taskTypeSupported.at(randomTask);
		int randomIntX = GetRandomIntLessThan(8);
		int randomIntY = GetRandomIntLessThan(8);

		CreateAndPushTask(task, IntVector2(randomIntX,randomIntY));
		return;
	}

	double max = 0;
	TaskType task			= GetTaskFromNNOutput(max);
	IntVector2 taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position);
	m_previousState			= m_state;
	CreateAndPushTask(task, taskPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateEntityState()
{
	m_previousState = m_state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::PrintDebugNN()
{
	if (g_skipRendering == false && g_currentSelectedEntity == this && g_enableDebugPrints && static_cast<double>(m_lastDebug + m_debugPrintDelay) < GetCurrentTimeSeconds())
	{
		m_lastDebug = static_cast<float>(GetCurrentTimeSeconds());
		double max = 0;
		TaskType task = GetTaskFromNNOutput(max);
		IntVector2	taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position);
		DebugDraw::GetInstance()->DebugRenderLogf(m_debugPrintDelay, GetTeamColor(), (Task::GetTaskTypeAsString(task) + " POSX = %d POSY = %d").c_str(), taskPosition.x, taskPosition.y);

		for (int index = 0; index < m_taskTypeSupported.size(); index++)
		{
			std::string debugNN;
			task = m_taskTypeSupported.at(index);
			debugNN.append("  ");
			debugNN.append(Task::GetTaskTypeAsString(task));
			debugNN.append(" :: ");
			debugNN.append(ToString(GetTaskValueFromNNOutput(task)));
			debugNN.append(" :: ");
			debugNN.append(ToString(GetTaskValueFromDesiredOutput(task)));
			DebugDraw::GetInstance()->DebugRenderLogf(debugNN, m_debugPrintDelay, GetTeamColor(), GetTeamColor());
		}
		float maxValue = 0;
		//int index = GetMostFavoredMoveTask(&maxValue);
		//DebugDraw::GetInstance()->DebugRenderLogf(taskStr, m_debugPrintDelay, GetTeamColor(), GetTeamColor());
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates unit stat for food gathered
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForFoodGathered(int count)
{
	m_state.m_resourceFoodGathered += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with stone gathered
*@param   : count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForStoneGathered(int count)
{
	m_state.m_resourceStoneGathered += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates with wood gathered
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForWoodGathered(int count)
{
	m_state.m_resourceWoodGathered += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates stat with food dropped
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForFoodDropped(int count)
{
	m_state.m_resourceFoodDropped += count;
	//m_scoreBoard.UpdateScoreResourceFoodCollected(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreResourceFoodCollected(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with stone dropped
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForStoneDropped(int count)
{
	m_state.m_resourceStoneDropped += count;
	//m_scoreBoard.UpdateScoreResourceStoneCollected(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreResourceStoneCollected(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with wood dropped
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForWoodDropped(int count)
{
	m_state.m_resourceWoodDropped += count;
	//m_scoreBoard.UpdateScoreResourceWoodCollected(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreResourceWoodCollected(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update by army spawned built
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForArmySpawnerBuilt(int count)
{
	m_state.m_numberOfArmySpawnerBuilt += count;
	//m_scoreBoard.UpdateScoreArmySpawnersBuilt(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreArmySpawnersBuilt(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with house built
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForHouseBuilt(int count)
{
	m_state.m_numberOfHouseBuilt += count;
	//m_scoreBoard.UpdateScoreHousesBuilt(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreHousesBuilt(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with short army spawned
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForShortRangeArmySpawned(int count)
{
	m_state.m_shortRangeArmySpawned += count;
	//m_scoreBoard.UpdateScoreShortRangeArmySpawned(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreShortRangeArmySpawned(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with long army spawned
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForLongRangeArmySpawned(int count)
{
	m_state.m_longRangeArmySpawned += count;
	//m_scoreBoard.UpdateScoreLongRangeArmySpawned(count);
	//m_map->GetMyScoreBoard(this).UpdateScoreLongRangeArmySpawned(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with unit stat for enemies attacked
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForEnemiesAttacked(Entity* attackedEntity ,int count)
{
/*
	m_state.m_enemiesAttacked += count;
	switch (attackedEntity->m_type)
	{
	case HOUSE:
		m_scoreBoard.UpdateScoreHousesAttacked(count);
		m_map->GetMyScoreBoard(this).UpdateScoreHousesAttacked(count);
		break;
	case ARMY_SPAWNER:
		m_scoreBoard.UpdateScoreArmySpawnersAttacked(count);
		m_map->GetMyScoreBoard(this).UpdateScoreArmySpawnersAttacked(count);
		break;
	case CIVILIAN:
		m_scoreBoard.UpdateScoreCiviliansAttacked(count);
		m_map->GetMyScoreBoard(this).UpdateScoreCiviliansAttacked(count);
		break;
	case SHORT_RANGE_ARMY:
		m_scoreBoard.UpdateScoreShortRangeArmySpawned(count);
		m_map->GetMyScoreBoard(this).UpdateScoreShortRangeArmySpawned(count);
		break;
	case LONG_RANGE_ARMY:
		m_scoreBoard.UpdateScoreLongRangeArmyAttacked(count);
		m_map->GetMyScoreBoard(this).UpdateScoreLongRangeArmyAttacked(count);
		break;
	case TOWN_CENTER:
		m_scoreBoard.UpdateScoreTownCenterAttacked(count);
		m_map->GetMyScoreBoard(this).UpdateScoreTownCenterAttacked(count);
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with enemies killed
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForEnemiesKilled(Entity* attackedEntity,int count)
{
	/*m_state.m_enemiesKilled += count;
	switch (attackedEntity->m_type)
	{
	case HOUSE:
		m_scoreBoard.UpdateScoreHousesAttacked(count);
		m_map->GetMyScoreBoard(this).UpdateScoreHousesDestroyed(count);
		break;
	case ARMY_SPAWNER:
		m_scoreBoard.UpdateScoreArmySpawnersDestroyed(count);
		m_map->GetMyScoreBoard(this).UpdateScoreArmySpawnersDestroyed(count);
		break;
	case CIVILIAN:
		m_scoreBoard.UpdateScoreCiviliansKilled(count);
		m_map->GetMyScoreBoard(this).UpdateScoreCiviliansKilled(count);
		break;
	case SHORT_RANGE_ARMY:
		m_scoreBoard.UpdateScoreShortRangeArmyKilled(count);
		m_map->GetMyScoreBoard(this).UpdateScoreShortRangeArmyKilled(count);
		break;
	case LONG_RANGE_ARMY:
		m_scoreBoard.UpdateScoreLongRangeArmyKilled(count);
		m_map->GetMyScoreBoard(this).UpdateScoreLongRangeArmyKilled(count);
		break;
	case TOWN_CENTER:
		m_scoreBoard.UpdateScoreTownCenterDestroyed(count);
		m_map->GetMyScoreBoard(this).UpdateScoreTownCenterDestroyed(count);
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with villager spawned
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForVillagerSpawned(int count)
{
	m_state.m_civilianSpawned += count;
	//m_map->GetMyScoreBoard(this).m_civiliansSpawned++;
	//m_scoreBoard.m_civiliansSpawned++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with food used
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForFoodUsed(int count)
{
	m_state.m_resourceFoodUsed += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with stone used
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForStoneUsed(int count)
{
	m_state.m_resourceStoneUsed += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Updates unit stat with wood used
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForWoodUsed(int count)
{
	m_state.m_resourceWoodUsed += count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Gets the task index from type
*@param   : Task type
*@return  : index
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetIndexOfTaskInNN(TaskType type)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetIndexOfMoveXPosition()
{
	return static_cast<int>(m_taskTypeSupported.size() - 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetIndexOfMoveYPosition()
{
	return static_cast<int>(m_taskTypeSupported.size() - 1);
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
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
		g_theRenderer->DrawSolidCircle(m_disc.center,m_disc.radius,GetTeamColor());
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
	RenderTaskType();
	RenderSelection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/09
*@purpose : Renders the task currently carrying on
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::RenderTaskType()
{
	if(m_type == TOWN_CENTER)
	{
		return;
	}
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	TaskType taksType = GetMyCurrentTask();
	std::string taskTypeStr = Task::GetTaskTypeAsShortString(taksType);
	g_theRenderer->DrawTextOn3DPoint(Vector3(GetPosition(),0) + Vector3(0,g_radius + g_fontSize, 0), Vector3::RIGHT, Vector3::UP, taskTypeStr, g_fontSize, Rgba::YELLOW);

	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::RenderSelection()
{
	bool isSelected = false;
	for(size_t index = 0;index < m_map->m_selectedEntities.size();index++)
	{
		if(m_map->m_selectedEntities.at(index) == this)
		{
			isSelected = true;
		}
	}
	if(!isSelected)
	{
		return;
	}

	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);

	Disc2 selectionDisc(m_disc.center, m_disc.radius - 5.f);
	switch (m_type)
	{
	case CIVILIAN:
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
		g_theRenderer->DrawCircle(selectionDisc, Rgba::YELLOW);
		break;
	case HOUSE:
	case ARMY_SPAWNER:
	case TOWN_CENTER:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
	//	g_theRenderer->DrawAABB(m_aabb2, GetTeamColor());
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
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
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
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
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
/*DATE    : 2019/01/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetRandomTaskInQueue()
{
	if (m_map->m_mapMode == MAP_MODE_TRAINING_AGGRESSIVE || m_map->m_mapMode == MAP_MODE_TRAINING_DEFENSIVE)
	{
		int randomNum = GetRandomIntLessThan(static_cast<int>(m_taskTypeSupported.size()));
		TaskType taskType = m_taskTypeSupported.at(randomNum);
		m_taskQueue.push(GetRandomTaskByType(taskType));
		return;
	}
	m_taskQueue.push(new TaskIdle());
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
void Entity::ClearTaskQueue()
{
	while(!m_taskQueue.empty())
	{
		Task *task = m_taskQueue.front();
		delete task;
		m_taskQueue.pop();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::CopyDesiredOutputs()
{
	for(int index = 0;index < m_desiredOuputs.size();index++)
	{
		m_desiredOuputs.at(index) = m_neuralNet.m_outputs->m_neurons.at(index).m_value;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/09
*@purpose : Copies desired strategy values into NN outputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::CopyDesiredStrategyValuesIntoDesiredNNOuputs()
{
	double max = -1;
	int maxIndex = 0;
	for(int index = 0;index < m_desiredStrategyValues.size();index++)
	{
		if(m_desiredStrategyValues.at(index) > max)
		{
			max = m_desiredStrategyValues.at(index);
			maxIndex = index;
		}
	}
	for (int index = 0; index < m_desiredStrategyValues.size(); index++)
	{
		m_desiredOuputs.at(index) = RangeMap(m_desiredStrategyValues.at(index), 0, max, 0, 1);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ClearDesiredStrategyValues()
{
	if(m_desiredStrategyValues.size() == 0)
	{
		for(int index = 0;index < MAX_NUM_STRATEGY;index++)
		{
			m_desiredStrategyValues.push_back(0);
		}
	}
	for (int index = 0; index < MAX_NUM_STRATEGY; index++)
	{
		m_desiredStrategyValues.at(index) = 0;;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredStrategyAsOutputForNN(Strategy strategy,int incrementValue)
{
	m_desiredStrategy = strategy;
	m_desiredStrategyValues.at(strategy)+= incrementValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDefaultStrategyAsOutputForNN()
{
	if(m_map->m_personality == DEFENSIVE)
	{
		SetDesiredStrategyAsOutputForNN(PATROL, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return;
	}
	if (m_map->m_personality == AGGRESSIVE)
	{
		SetDesiredStrategyAsOutputForNN(RETREAT, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return;
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
/*DATE    : 2019/01/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Entity::FindEnemyTownCenter()
{
	for (int index = 0; index < m_map->m_townCenters.size(); index++)
	{
		if (m_map->m_townCenters.at(index)->m_teamID != m_teamID)
		{
			return m_map->m_townCenters.at(index);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::ClearDesiredOutputs()
{
	for(int index = 0;index < m_desiredOuputs.size();index++)
	{
		m_desiredOuputs.at(index) = 0.f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : Set
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredOutputForTask(TaskType type,float value)
{
	int taskIndex = GetIndexOfTaskInNN(type);
	if(taskIndex >=0 && taskIndex < m_taskTypeSupported.size())
	{
		m_desiredOuputs.at(taskIndex) = value;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/26
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Entity::GetDesiredOutputForTask(TaskType type)
{
	int taskIndex = GetIndexOfTaskInNN(type);
	if (taskIndex >= 0 && taskIndex < m_taskTypeSupported.size())
	{
		return m_desiredOuputs.at(taskIndex);
	}
	return 0;
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
		m_map->CheckAndSaveBestEntityNN(m_type,m_teamID);
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
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
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
	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
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
/*DATE    : 2019/02/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushTask(TaskType taskType, IntVector2 taskPosition)
{
	switch (taskType)
	{
		case TASK_IDLE:
		{
			CreateAndPushIdleTask(taskPosition);
		}
		break;
		case TASK_MOVE:
		{
			CreateAndPushMoveTask(taskPosition);
		}
		break;
		case TASK_GATHER_RESOURCE_FOOD:
		{
			CreateAndPushGatherResourceTask(taskPosition, TASK_GATHER_RESOURCE_FOOD);
		}
		break;
		case TASK_GATHER_RESOURCE_STONE:
		{
			CreateAndPushGatherResourceTask(taskPosition, TASK_GATHER_RESOURCE_STONE);
		}
		break;
		case TASK_GATHER_RESOURCE_WOOD:
		{
			CreateAndPushGatherResourceTask(taskPosition, TASK_GATHER_RESOURCE_WOOD);
		}
		break;
		case TASK_DROP_RESOURCE:
		{
			CreateAndPushDropResourceTask(taskPosition);
		}
		break;
		case TASK_BUILD_TOWNCENTER:
		{
			CreateAndPushBuildTownCenterTask(taskPosition);
		}
		break;
		case TASK_BUILD_HOUSE:
		{
			CreateAndPushBuildHouseTask(taskPosition);
		}
		break;
		case TASK_BUILD_ARMY_SPAWNER:
		{
			CreateAndPushBuildArmySpawnerTask(taskPosition);
		}
		break;
		case TASK_RETREAT:
		{
			if(m_desiredStrategy == RETREAT)
			{
				m_scoreBoard.m_numOfRetreatActions++;
			}
			CreateAndPushRetreatTask(taskPosition);
		}
		break;
		case TASK_EXPLORE:
		{
			if (m_desiredStrategy == EXPLORE)
			{
				m_scoreBoard.m_numOfExploreActions++;
			}
			CreateAndPushExploreTask(taskPosition);
		}
		break;
		case TASK_PATROL:
		{
			if (m_desiredStrategy == PATROL)
			{
				m_scoreBoard.m_numOfPatrolActions++;
			}
			CreateAndPushPatrolTask(taskPosition);
		}
		break;
		case TASK_FOLLOW:
		{
			if (m_desiredStrategy == FOLLOW)
			{
				m_scoreBoard.m_numOfFollowActions++;
			}
			CreateAndPushFollowTask(taskPosition);
		}
		break;
		case TASK_ATTACK:
		{
			if (m_desiredStrategy == ATTACK)
			{
				m_scoreBoard.m_numOfAttackActions++;
			}
			CreateAndPushAttackTask(taskPosition);
		}
		break;
		case TASK_LONG_ATTACK:
		{
			IntVector2 currentPosition = m_map->GetCordinates(m_previousState.m_position);
			taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position, 4, 4);
			CreateAndPushLongRangeAttackTask(taskPosition);
		}
		break;
		case TASK_SHORT_ATTACK:
		{
			IntVector2 currentPosition = m_map->GetCordinates(m_previousState.m_position);
			taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position, 2, 2);
			CreateAndPushShortRangeAttackTask(taskPosition);
		}
		break;
		case TASK_SPAWN_VILLAGER:
		{
			CreateAndPushSpawnVillagerTask(taskPosition);
		}
		break;
		case TASK_SPAWN_CLASSA_WARRIOR:
		{
			CreateAndPushSpawnClassAArmyTask(taskPosition);
		}
		break;
		case TASK_SPAWN_CLASSB_WARRIOR:
		{
			CreateAndPushSpawnClassBArmyTask(taskPosition);
		}
		break;
		default:
			break;
	}
	return true;
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
/*DATE    : 2018/10/07
*@purpose : Checks if
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Entity::GetResourceEntityFromList(std::vector<Entity*> &list)
{
	std::vector<Entity*> entityList;
	for (size_t listIndex = 0; listIndex < list.size(); listIndex++)
	{
		if(list.at(listIndex)->m_type == RESOURCE_FOOD || list.at(listIndex)->m_type == RESOURCE_STONE || list.at(listIndex)->m_type == RESOURCE_WOOD)
		{
			entityList.push_back(list.at(listIndex));
		}
	}
	return entityList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : Get all town center entity from list given
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Entity::GetTownCenterEntityFromList(std::vector<Entity*> &list)
{
	std::vector<Entity*> entityList;
	for (size_t listIndex = 0; listIndex < list.size(); listIndex++)
	{
		if (list.at(listIndex)->m_type == TOWN_CENTER)
		{
			entityList.push_back(list.at(listIndex));
		}
	}
	return entityList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Entity::GetMyTownCenterEntityFromList(std::vector<Entity*> &list)
{
	std::vector<Entity*> entityList;
	for (size_t listIndex = 0; listIndex < list.size(); listIndex++)
	{
		if (list.at(listIndex)->m_type == TOWN_CENTER && m_teamID == list.at(listIndex)->m_teamID)
		{
			entityList.push_back(list.at(listIndex));
		}
	}
	return entityList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetBestNeighbour()
{
	return IntVector2::MINUS_ONE;
}

int Entity::GetMyFoodCount()
{
	return ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_food;
}

int Entity::GetMyStoneCount()
{
	return ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_stone;

}

int Entity::GetMyWoodCount()
{
	return ((TownCenter*)FindMyTownCenter())->m_resourceStat.m_wood;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : returns corresponding mini map value 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Entity::GetMiniMapValue()
{
	switch (m_type)
	{
	case CIVILIAN:
		return (static_cast<float>(m_teamID - 1) * 0.40f) + 0.30f;
		break;
	case SHORT_RANGE_ARMY:
		return (static_cast<float>(m_teamID - 1) * 0.40f) + 0.25f;
		break;
	case LONG_RANGE_ARMY:
		return (static_cast<float>(m_teamID - 1) * 0.40f) + 0.20f;
		break;
	case HOUSE:
		return (static_cast<float>(m_teamID - 1) * 0.40f) + 0.05f;
		break;
	case TOWN_CENTER:
		return (static_cast<float>(m_teamID - 1) * 0.40f) + 0.10f;
		break;
	case RESOURCE_FOOD:
		return 0.8f;
		break;
	case RESOURCE_STONE:
		return 0.85f;
		break;
	case RESOURCE_WOOD:
		return 0.9f;
		break;
	case ARMY_SPAWNER:
		return (static_cast<float>(m_teamID - 1) * 0.40f) + 0.15f;
		break;
	default:
		break;
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : retrives task output value
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Entity::GetTaskValueFromNNOutput(TaskType type)
{
	for(int index = 0;index < m_taskTypeSupported.size();index++)
	{
		if(type == m_taskTypeSupported.at(index))
		{
			return m_neuralNet.m_outputs->m_neurons.at(index).m_value;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Entity::GetTaskValueFromDesiredOutput(TaskType type)
{
	for (int index = 0; index < m_taskTypeSupported.size(); index++)
	{
		if (type == m_taskTypeSupported.at(index))
		{
			return m_desiredOuputs.at(index);
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType Entity::GetTaskFromNNOutput(double &max)
{
	UNUSED(max);
	return TASK_IDLE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Gets the position to execute from NN Output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetTaskPositonFromNNOutput(Vector2 prevPosition)
{
	int xPosition = m_map->GetCordinates(prevPosition).x;
	int yPosition = m_map->GetCordinates(prevPosition).y;

	int minX = xPosition - g_entityMiniMapMaxWidth / 2;
	int minY = yPosition - g_entityMiniMapMaxHeight / 2;

	int maxX = xPosition + g_entityMiniMapMaxWidth / 2;
	int maxY = yPosition + g_entityMiniMapMaxHeight / 2;

	if (minX < 0)
	{
		maxX += (0 - minX);
	}
	if (minY < 0)
	{
		maxY += (0 - minY);
	}
	if (maxX > m_map->m_maxWidth)
	{
		minX -= (maxX - m_map->m_maxWidth);
	}
	if (maxY > m_map->m_maxHeight)
	{
		minY -= (maxY - m_map->m_maxHeight);
	}

	int mapMinX = GetMax(minX, 0);
	int mapMinY = GetMax(minY, 0);

	int mapMaxX = GetMin(maxX, m_map->m_maxWidth - 1);
	int mapMaxY = GetMin(maxY, m_map->m_maxHeight - 1);


	double taskXPosition    = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
	double taskYPosition    = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;
	float xRangedValue = RangeMapFloat(static_cast<float>(taskXPosition), 0.f, 1.f, static_cast<float>(mapMinX), static_cast<float>(mapMaxX));
	float yRangedValue = RangeMapFloat(static_cast<float>(taskYPosition), 0.f, 1.f, static_cast<float>(mapMinY), static_cast<float>(mapMaxY));
	int xPos = ClampInt(RoundToNearestInt(xRangedValue), 0, m_map->m_maxWidth  - 1);
	int yPos = ClampInt(RoundToNearestInt(yRangedValue), 0, m_map->m_maxHeight - 1);
	return IntVector2(xPos,yPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/09
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetTaskPositonFromNNOutput(Vector2 prevPosition, int width, int height)
{
	int xPosition = m_map->GetCordinates(prevPosition).x;
	int yPosition = m_map->GetCordinates(prevPosition).y;

	int minX = xPosition - width / 2;
	int minY = yPosition - height / 2;

	int maxX = xPosition + width / 2;
	int maxY = yPosition + height / 2;

	if (minX < 0)
	{
		maxX += (0 - minX);
	}
	if (minY < 0)
	{
		maxY += (0 - minY);
	}
	if (maxX > m_map->m_maxWidth)
	{
		minX -= (maxX - m_map->m_maxWidth);
	}
	if (maxY > m_map->m_maxHeight)
	{
		minY -= (maxY - m_map->m_maxHeight);
	}

	double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
	double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

	float xRangedValue = RangeMapFloat(static_cast<float>(taskXPosition), 0.f, 1.f, static_cast<float>(minX), static_cast<float>(maxX));
	float yRangedValue = RangeMapFloat(static_cast<float>(taskYPosition), 0.f, 1.f, static_cast<float>(minY), static_cast<float>(maxY));
	int xPos = ClampInt(RoundToNearestInt(xRangedValue), 0, m_map->m_maxWidth - 1);
	int yPos = ClampInt(RoundToNearestInt(yRangedValue), 0, m_map->m_maxHeight - 1);
	return IntVector2(xPos, yPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetTaskPositonFromNNOutput(Vector2 prevPosition, Vector2 nnOutput, int width, int height)
{
	int xPosition = m_map->GetCordinates(prevPosition).x;
	int yPosition = m_map->GetCordinates(prevPosition).y;

	int minX = xPosition - width / 2;
	int minY = yPosition - height / 2;

	int maxX = xPosition + width / 2;
	int maxY = yPosition + height / 2;

	if (minX < 0)
	{
		maxX += (0 - minX);
	}
	if (minY < 0)
	{
		maxY += (0 - minY);
	}
	if (maxX > m_map->m_maxWidth)
	{
		minX -= (maxX - m_map->m_maxWidth);
	}
	if (maxY > m_map->m_maxHeight)
	{
		minY -= (maxY - m_map->m_maxHeight);
	}


	double taskXPosition = nnOutput.x;// m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
	double taskYPosition = nnOutput.y;// m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

	float xRangedValue = RangeMapFloat(static_cast<float>(taskXPosition), 0.f, 1.f, static_cast<float>(minX), static_cast<float>(maxX));
	float yRangedValue = RangeMapFloat(static_cast<float>(taskYPosition), 0.f, 1.f, static_cast<float>(minY), static_cast<float>(maxY));
	int xPos = ClampInt(RoundToNearestInt(xRangedValue), 0, m_map->m_maxWidth - 1);
	int yPos = ClampInt(RoundToNearestInt(yRangedValue), 0, m_map->m_maxHeight - 1);
	return IntVector2(xPos, yPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Entity::GetPredictedNNOutputFromMapPosition(IntVector2 entityCords,IntVector2 mapCords,int width,int height)
{
	int xPosition = entityCords.x;
	int yPosition = entityCords.y;

	int minX = xPosition - width / 2;
	int minY = yPosition - height / 2;

	int maxX = xPosition + width / 2;
	int maxY = yPosition + height / 2;

	int mapMinX = GetMax(minX, 0);
	int mapMinY = GetMax(minY, 0);

	int mapMaxX = GetMin(maxX, m_map->m_maxWidth  - 1);
	int mapMaxY = GetMin(maxY, m_map->m_maxHeight - 1);

	if(mapMinX <= mapCords.x && mapCords.x <= mapMaxX)
	{
		if(mapMinY <= mapCords.y && mapCords.y <= mapMaxY)
		{
			int distanceX = mapCords.x - mapMinX;
			int distanceY = mapCords.y - mapMinY;
			float rangedX = RangeMapInt(distanceX, mapMinX, mapMaxX, 0, 1);
			float rangedY = RangeMapInt(distanceY, mapMinY, mapMaxY, 0, 1);
			return Vector2(rangedX, rangedY);
		}
	}


	double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
	double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

	float xRangedValue = RangeMapFloat(static_cast<float>(taskXPosition), 0.f, 1.f, static_cast<float>(mapMinX) - 0.48f, static_cast<float>(mapMaxX) + 0.48f);
	float yRangedValue = RangeMapFloat(static_cast<float>(taskYPosition), 0.f, 1.f, static_cast<float>(mapMinY) - 0.48f, static_cast<float>(mapMaxY) + 0.48f);
	int xPos = ClampInt(RoundToNearestInt(xRangedValue), 0, m_map->m_maxWidth - 1);
	int yPos = ClampInt(RoundToNearestInt(yRangedValue), 0, m_map->m_maxHeight - 1);
	return Vector2(static_cast<float>(xPos), static_cast<float>(yPos));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetRelativeCellLocation(float x, float y)
{
	int retX = -1;
	int retY = -1;
	if (x > 0.66)
	{
		retX = 1;
	}
	else if(x > 0.33)
	{
		retX = 0;
	}
	
	if (y > 0.66)
	{
		retY = 1;
	}
	else if (y > 0.33)
	{
		retY = 0;
	}
	return IntVector2(retX, retY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : Retrieves a random safe area from the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetRandomSafeArea()
{
	//return IntVector2(39, 19);
	return IntVector2(GetRandomIntInRange(m_minSafeArea.x, m_maxSafeArea.x), GetRandomIntInRange(m_minSafeArea.y, m_maxSafeArea.y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : Retrives a random area in the teritary
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetRandomTeritaryArea()
{
	return IntVector2(GetRandomIntInRange(m_minTeritaryArea.x, m_maxTeritaryArea.x), GetRandomIntInRange(m_minTeritaryArea.y, m_maxTeritaryArea.y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMiniMapMins()
{
	int xCords = GetCordinates().x;
	int yCords = GetCordinates().y;

	xCords -= 4;
	yCords -= 4;

	xCords = ClampInt(xCords, 0, m_map->m_maxWidth - 1);
	yCords = ClampInt(yCords, 0, m_map->m_maxHeight - 1);
	return IntVector2(xCords, yCords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMiniMapMaxs()
{
	int xCords = GetCordinates().x;
	int yCords = GetCordinates().y;

	xCords += 4;
	yCords += 4;

	xCords = ClampInt(xCords, 0, m_map->m_maxWidth - 1);
	yCords = ClampInt(yCords, 0, m_map->m_maxHeight - 1);
	return IntVector2(xCords, yCords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Task * Entity::GetRandomTaskByType(TaskType type)
{
	IntVector2 randomNeighbour = m_map->GetRandomNeighbour(GetCordinates(), 4);
	Vector2 position = m_map->GetMapPosition(randomNeighbour);

	int     tileIndex1 = m_map->GetTileIndex(m_map->GetRandomNeighbour(GetCordinates(), 1));
	int		tileIndex2 = m_map->GetTileIndex(m_map->GetRandomNeighbour(GetCordinates(), 2));

	Task * task = nullptr;
	switch (type)
	{
	case TASK_ATTACK:
		task = new TaskAttack(m_map, this,m_attackRange);
		break;
	case TASK_EXPLORE:
		task = new TaskExplore(m_map, this);
		break;
	case TASK_FOLLOW:
		task = new TaskFollow(m_map, this);
		break;
	case TASK_PATROL:
		task = new TaskPatrol(m_map, this);
		break;
	case TASK_RETREAT:
		task = new TaskRetreat(m_map, this);
		break;
	case TASK_MOVE:
		task = new TaskMove(m_map, this, position);
		break;
	case TASK_GATHER_RESOURCE_FOOD:
		task = new TaskGatherResource(this, TASK_GATHER_RESOURCE_FOOD);
		break;
	case TASK_GATHER_RESOURCE_STONE:
		task = new TaskGatherResource(this, TASK_GATHER_RESOURCE_STONE);
		break;
	case TASK_GATHER_RESOURCE_WOOD:
		task = new TaskGatherResource(this, TASK_GATHER_RESOURCE_WOOD);
		break;
	case TASK_DROP_RESOURCE:
		task = new TaskDropResource(this, FindMyTownCenter());
		break;
	case TASK_BUILD_TOWNCENTER:
	{
		int randomNum = GetRandomIntLessThan(static_cast<int>(m_taskTypeSupported.size()));
		TaskType taskType = m_taskTypeSupported.at(randomNum);
		return GetRandomTaskByType(taskType);
	}
		break;
	case TASK_BUILD_HOUSE:
		task = new TaskBuildHouse(m_map, this, position);
		break;
	case TASK_BUILD_ARMY_SPAWNER:
		task = new TaskBuildArmySpawner(m_map, this, position);
		break;
	case TASK_LONG_ATTACK:
		task = new TaskLongRangeAttack(m_map, this, tileIndex2);
		break;
	case TASK_SHORT_ATTACK:
		task = new TaskShortRangeAttack(m_map, this, tileIndex1);
		break;
	case TASK_SPAWN_VILLAGER:
		task = new TaskBuildArmySpawner(m_map, this, position);
		break;
	case TASK_SPAWN_CLASSA_WARRIOR:
		task = new TaskSpawnClassAWarrior(m_map, this);
		break;
	case TASK_SPAWN_CLASSB_WARRIOR:
		task = new TaskSpawnClassBWarrior(m_map, this);
		break;
	case TASK_IDLE:
		task = new TaskIdle();
		break;
	case TASK_MOVEX:
	{
		int randomNum = GetRandomIntLessThan(static_cast<int>(m_taskTypeSupported.size()));
		TaskType taskType = m_taskTypeSupported.at(randomNum);
		return GetRandomTaskByType(taskType);
	}
		break;
	case TASK_MOVEY:
	{
		int randomNum = GetRandomIntLessThan(static_cast<int>(m_taskTypeSupported.size()));
		TaskType taskType = m_taskTypeSupported.at(randomNum);
		return GetRandomTaskByType(taskType);
	}
		break;
	default:
		task = new TaskIdle();
		break;
	}
	return task;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/09
*@purpose : Retrieves my current task
*@param   : NIL
*@return  : task type
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType Entity::GetMyCurrentTask()
{
	if(m_taskQueue.size() == 0 || m_taskQueue.front() == nullptr)
	{
		return TASK_INVALID;
	}
	return m_taskQueue.front()->m_taskType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetGlobalBestFilePath()
{
	if (m_map->m_personality == AGGRESSIVE)
	{
		return "Data\\NN\\BestGame\\Aggressive\\" + ToString(m_teamID) + "\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + ".txt";
	}
	if (m_map->m_personality == DEFENSIVE)
	{
		return "Data\\NN\\BestGame\\Defensive\\" + ToString(m_teamID) + "\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + ".txt";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetLocalBestFilePath()
{
	if (m_map->m_personality == AGGRESSIVE)
	{
		return "Data\\NN\\"+m_map->m_folder +"\\Aggressive\\"+ ToString(m_teamID)+"\\"+ GetEntityTypeAsString(m_type) +"_"+ ToString(m_teamID) + ".txt";
	}
	if (m_map->m_personality == DEFENSIVE)
	{
		return "Data\\NN\\" + m_map->m_folder + "\\Defensive\\" + ToString(m_teamID) + "\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + ".txt";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : returns best stat global file path
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetGlobalBestStatFilePath()
{
	if(m_map->m_personality == AGGRESSIVE)
	{
		return "Data\\NN\\BestGame\\Aggressive\\" + ToString(m_teamID) + "\\BestStats\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + "_STATS.txt";
	}
	if(m_map->m_personality == DEFENSIVE)
	{
		return "Data\\NN\\BestGame\\Defensive\\" + ToString(m_teamID) +"\\BestStats\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + "_STATS.txt";
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : returns best stat local file path
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetLocalBestStatFilePath()
{
	if (m_map->m_personality == AGGRESSIVE)
	{
		return "Data\\NN\\" + m_map->m_folder + "\\Aggressive" + ToString(m_teamID) + "\\BestStats\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + "_STATS.txt";
	}
	if (m_map->m_personality == DEFENSIVE)
	{
		return "Data\\NN\\" + m_map->m_folder +"\\Defensive"+ToString(m_teamID)+"\\BestStats\\"+GetEntityTypeAsString(m_type) + "_"+ToString(m_teamID) + "_STATS.txt";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetGlobalBestScore()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetLocalBestScore()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetGlobalBestScore(int globalScore)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetLocalBestScore(int globalScore)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NNInputs Entity::GetMyNNInputs()
{

	NNInputs inputs;
	TownCenter *myTownCenter          = (TownCenter*)FindMyTownCenter();
	TownCenter *enemyTownCenter		  = (TownCenter*)FindEnemyTownCenter();
		
	inputs.m_inRangeAllyArmySpawnerCount = 0;
	inputs.m_inRangeAllyCiviliansCount = 0;
	inputs.m_inRangeAllyHouseCount = 0;
	inputs.m_inRangeAllyLongRangeArmyCount = 0;
	inputs.m_inRangeAllyShortRangeArmyCount = 0;
	inputs.m_inRangeAllyTownCenterCount = 0;
	inputs.m_inRangeAttackingAllyHeatMapValue = 0;
	inputs.m_inRangeStationaryAllyHeatMapValue = 0;

	inputs.m_inRangeEnemyArmySpawnerCount = 0;
	inputs.m_inRangeEnemyCiviliansCount = 0;
	inputs.m_inRangeEnemyHouseCount = 0;
	inputs.m_inRangeEnemyLongRangeArmyCount = 0;
	inputs.m_inRangeEnemyShortRangeArmyCount = 0;
	inputs.m_inRangeEnemyTownCenterCount = 0;
	inputs.m_inRangeAttackingEnemyHeatMapValue = 0;
	inputs.m_inRangeStationaryEnemyHeatMapValue = 0;

	inputs.m_allyInAttackMode  = 0;
	inputs.m_allyInExploreMode = 0;
	inputs.m_allyInFollowMode  = 0;
	inputs.m_allyInPatrolMode  = 0;
	inputs.m_allyInRetreatMode = 0;


	inputs.m_allyArmySpawnerCount			= myTownCenter->m_resourceStat.m_armySpawners;
	inputs.m_allyCiviliansCount				= myTownCenter->m_resourceStat.m_civilians;
	inputs.m_allyHouseCount					= myTownCenter->m_resourceStat.m_houses;
	inputs.m_allyLongRangeArmyCount			= myTownCenter->m_resourceStat.m_longRangeArmies;
	inputs.m_allyShortRangeArmyCount		= myTownCenter->m_resourceStat.m_shortRangeArmies;
	inputs.m_allyTownCenterHealth			= myTownCenter->m_health;
	inputs.m_allyTownCenterHealthLastFrame	= m_townCentreHealthLastFrame;

									  
	inputs.m_enemyArmySpawnerCount			= enemyTownCenter->m_resourceStat.m_armySpawners;
	inputs.m_enemyCiviliansCount			= enemyTownCenter->m_resourceStat.m_civilians;
	inputs.m_enemyHouseCount				= enemyTownCenter->m_resourceStat.m_houses;
	inputs.m_enemyLongRangeArmyCount		= enemyTownCenter->m_resourceStat.m_longRangeArmies;
	inputs.m_enemyShortRangeArmyCount		= enemyTownCenter->m_resourceStat.m_shortRangeArmies;
	inputs.m_enemyTownCenterHealth			= enemyTownCenter->m_health;
	inputs.m_enemyTownCenterHealthLastFrame = enemyTownCenter->m_healthLastFrame;

	inputs.m_health							= m_health;
	inputs.m_healthLastFrame				= m_healthLastFrame;
	inputs.m_resourceFoodCount				= enemyTownCenter->m_resourceStat.m_food;
	inputs.m_resourceStoneCount				= enemyTownCenter->m_resourceStat.m_stone;
	inputs.m_resourceWoodCount				= enemyTownCenter->m_resourceStat.m_wood;
	inputs.m_resourceCarrying				= 0;

	if(dynamic_cast<Civilian*>(this) != nullptr)
	{
		Civilian * civilian = dynamic_cast<Civilian*>(this);
		if(civilian->m_resourceTypeCarrying != nullptr)
		{
			inputs.m_resourceCarrying = 1.f;
		}
	}

	inputs.m_allyArmySpawnerCount		= RangeMap(inputs.m_allyArmySpawnerCount     ,0, 5,  0, 1);
	inputs.m_allyCiviliansCount			= RangeMap(inputs.m_allyCiviliansCount       ,0, 10, 0, 1);
	inputs.m_allyHouseCount				= RangeMap(inputs.m_allyHouseCount           ,0, 5,  0, 1);
	inputs.m_allyLongRangeArmyCount		= RangeMap(inputs.m_allyLongRangeArmyCount   ,0, 10, 0, 1);
	inputs.m_allyShortRangeArmyCount	= RangeMap(inputs.m_allyShortRangeArmyCount  ,0, 10, 0, 1);	
	inputs.m_allyTownCenterHealth		= RangeMap(inputs.m_allyTownCenterHealth     ,0, 50, 0, 1);
	//inputs.m_allyTownCenterHealthLastFrame		= RangeMap(inputs.m_allyTownCenterHealthLastFrame, 0, 50, 0, 1);


	inputs.m_enemyArmySpawnerCount		= RangeMap(inputs.m_enemyArmySpawnerCount    ,0, 5,  0, 1);
	inputs.m_enemyCiviliansCount		= RangeMap(inputs.m_enemyCiviliansCount	     ,0, 10, 0, 1);
	inputs.m_enemyHouseCount			= RangeMap(inputs.m_enemyHouseCount		     ,0, 5,  0, 1);
	inputs.m_enemyLongRangeArmyCount	= RangeMap(inputs.m_enemyLongRangeArmyCount  ,0, 10, 0, 1);	
	inputs.m_enemyShortRangeArmyCount	= RangeMap(inputs.m_enemyShortRangeArmyCount ,0, 10, 0, 1);	
	inputs.m_enemyTownCenterHealth		= RangeMap(inputs.m_enemyTownCenterHealth	 ,0, 50, 0, 1);
	inputs.m_enemyTownCenterHealthLastFrame		= RangeMap(inputs.m_enemyTownCenterHealthLastFrame, 0, 50, 0, 1);

	inputs.m_health						= RangeMap(inputs.m_health					 ,0, 10, 0, 1);
	inputs.m_resourceFoodCount			= RangeMap(inputs.m_resourceFoodCount		 ,0, 30, 0, 1);
	inputs.m_resourceStoneCount			= RangeMap(inputs.m_resourceStoneCount		 ,0, 30, 0, 1);
	inputs.m_resourceWoodCount			= RangeMap(inputs.m_resourceWoodCount		 ,0, 30, 0, 1);

	


	int totalCount = 0;
	for (int indexShortRangeArmy = 0; indexShortRangeArmy < m_map->m_classAWarriors.size(); indexShortRangeArmy++)
	{
		Entity * entity = m_map->m_classAWarriors.at(indexShortRangeArmy);
		if(entity->m_teamID == m_teamID)
		{
			if(entity->m_taskQueue.size() == 0)
			{
				continue;
			}
			Task *task = entity->m_taskQueue.front();
			if( task != nullptr && task->m_taskType == TASK_ATTACK)
			{
				inputs.m_allyInAttackMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_EXPLORE)
			{
				inputs.m_allyInExploreMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_FOLLOW)
			{
				inputs.m_allyInFollowMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_PATROL)
			{
				inputs.m_allyInPatrolMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_RETREAT)
			{
				inputs.m_allyInRetreatMode++;
				totalCount++;
			}
		}
	}
	for (int indexLongRangeArmy  = 0; indexLongRangeArmy  < m_map->m_classBWarriors.size(); indexLongRangeArmy++)
	{
		Entity * entity = m_map->m_classBWarriors.at(indexLongRangeArmy);
		if (entity->m_teamID == m_teamID)
		{
			if (entity->m_taskQueue.size() == 0)
			{
				continue;
			}
			Task *task = entity->m_taskQueue.front();
			if (task != nullptr && task->m_taskType == TASK_ATTACK)
			{
				inputs.m_allyInAttackMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_EXPLORE)
			{
				inputs.m_allyInExploreMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_FOLLOW)
			{
				inputs.m_allyInFollowMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_PATROL)
			{
				inputs.m_allyInPatrolMode++;
				totalCount++;
			}
			if (task != nullptr && task->m_taskType == TASK_RETREAT)
			{
				inputs.m_allyInRetreatMode++;
				totalCount++;
			}
		}
	}

	inputs.m_allyInAttackMode  = RangeMap (inputs.m_allyInAttackMode  ,0,10,0,1);
	inputs.m_allyInExploreMode = RangeMap (inputs.m_allyInExploreMode ,0,10,0,1);
	inputs.m_allyInFollowMode  = RangeMap (inputs.m_allyInFollowMode  ,0,10,0,1);
	inputs.m_allyInPatrolMode  = RangeMap (inputs.m_allyInPatrolMode  ,0,10,0,1);
	inputs.m_allyInRetreatMode = RangeMap (inputs.m_allyInRetreatMode ,0,10,0,1);

	std::vector<IntVector2> m_neighbouringCords = m_map->GetAllNeighbouringCoordinates(GetCordinates(), 4);
	m_neighbouringCords.push_back(GetCordinates());
	for(size_t index = 0;index < m_neighbouringCords.size();index++)
	{
		Entity *entity = m_map->GetEntityFromPosition(m_map->GetMapPosition(m_neighbouringCords.at(index)));
		if(entity != nullptr)
		{
			switch (entity->m_type)
			{
			case ARMY_SPAWNER:
				if(entity->m_teamID == m_teamID)
				{
					inputs.m_inRangeAllyArmySpawnerCount++;
				}
				else
				{
					inputs.m_inRangeEnemyArmySpawnerCount++;
				}
				break;
			case CIVILIAN:
				if (entity->m_teamID == m_teamID)
				{
					inputs.m_inRangeAllyCiviliansCount++;
				}
				else
				{
					inputs.m_inRangeEnemyCiviliansCount++;
				}
				break;
			case SHORT_RANGE_ARMY:
				if (entity->m_teamID == m_teamID)
				{
					inputs.m_inRangeAllyShortRangeArmyCount++;
					inputs.m_inRangeAttackingAllyHeatMapValue += entity->m_health;
				}
				else
				{
					inputs.m_inRangeEnemyShortRangeArmyCount++;
					inputs.m_inRangeAttackingEnemyHeatMapValue += entity->m_health;
				}
				break;
			case LONG_RANGE_ARMY:
				if (entity->m_teamID == m_teamID)
				{
					inputs.m_inRangeAllyLongRangeArmyCount++;
					inputs.m_inRangeAttackingAllyHeatMapValue += entity->m_health;
				}
				else
				{
					inputs.m_inRangeEnemyLongRangeArmyCount++;
					inputs.m_inRangeAttackingEnemyHeatMapValue += entity->m_health;
				}
				break;
			case HOUSE:
				if (entity->m_teamID == m_teamID)
				{
					inputs.m_inRangeAllyHouseCount++;
				}
				else
				{
					inputs.m_inRangeEnemyHouseCount++;
				}
				break;
			case TOWN_CENTER:
				if (entity->m_teamID == m_teamID)
				{
					inputs.m_inRangeAllyTownCenterCount++;
					inputs.m_inRangeStationaryAllyHeatMapValue     += entity->m_health;
				}
				else
				{
					inputs.m_inRangeEnemyTownCenterCount++;
					inputs.m_inRangeStationaryEnemyHeatMapValue += entity->m_health;

				}
				break;
			default:
				break;
			}
		}
	}

	inputs.m_inRangeAllyArmySpawnerCount		= RangeMap(inputs.m_inRangeAllyArmySpawnerCount    ,0, 05, 0, 1);
	inputs.m_inRangeAllyCiviliansCount			= RangeMap(inputs.m_inRangeAllyCiviliansCount      ,0, 10, 0, 1);
	inputs.m_inRangeAllyHouseCount				= RangeMap(inputs.m_inRangeAllyHouseCount          ,0, 05, 0, 1);
	inputs.m_inRangeAllyLongRangeArmyCount		= RangeMap(inputs.m_inRangeAllyLongRangeArmyCount  ,0, 10, 0, 1);
	inputs.m_inRangeAllyShortRangeArmyCount		= RangeMap(inputs.m_inRangeAllyShortRangeArmyCount ,0, 10, 0, 1);
	inputs.m_inRangeAllyTownCenterCount			= RangeMap(inputs.m_inRangeAllyTownCenterCount     ,0, 01, 0, 1);
	inputs.m_inRangeAttackingAllyHeatMapValue   = RangeMap(inputs.m_inRangeAttackingAllyHeatMapValue,0, 50, 0, 1);
	inputs.m_inRangeStationaryAllyHeatMapValue  = RangeMap(inputs.m_inRangeStationaryAllyHeatMapValue, 0, 50, 0, 1);


	inputs.m_inRangeEnemyArmySpawnerCount		= RangeMap(inputs.m_inRangeEnemyArmySpawnerCount   ,0, 05, 0, 1);
	inputs.m_inRangeEnemyCiviliansCount			= RangeMap(inputs.m_inRangeEnemyCiviliansCount     ,0, 10, 0, 1);
	inputs.m_inRangeEnemyHouseCount				= RangeMap(inputs.m_inRangeEnemyHouseCount         ,0, 05, 0, 1);
	inputs.m_inRangeEnemyLongRangeArmyCount		= RangeMap(inputs.m_inRangeEnemyLongRangeArmyCount ,0, 10, 0, 1);
	inputs.m_inRangeEnemyShortRangeArmyCount	= RangeMap(inputs.m_inRangeEnemyShortRangeArmyCount,0, 10, 0, 1);
	inputs.m_inRangeEnemyTownCenterCount		= RangeMap(inputs.m_inRangeEnemyTownCenterCount    ,0, 01, 0, 1);
	inputs.m_inRangeAttackingEnemyHeatMapValue  = RangeMap(inputs.m_inRangeAttackingEnemyHeatMapValue,0, 50, 0, 1);
	inputs.m_inRangeStationaryEnemyHeatMapValue = RangeMap(inputs.m_inRangeStationaryEnemyHeatMapValue, 0, 50, 0, 1);

	return inputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : Check if the entity is stationary (not moving)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsStationaryEntity()
{
	if(m_type == HOUSE || m_type == ARMY_SPAWNER || m_type == TOWN_CENTER || m_type == RESOURCE_FOOD || m_type == RESOURCE_STONE || m_type == RESOURCE_WOOD)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::HasResource()
{
	if(m_resourceTypeCarrying == nullptr)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/10
*@purpose : Checks if the entity is under attack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsUnderAttack()
{
	if(m_healthLastFrame != m_health)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Creates idle task and push to queue
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushIdleTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	ClearTaskQueue();
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
	ClearTaskQueue();
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
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and pushes task to gather resources 
*@param   : Resource cords
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushGatherResourceTask(IntVector2 cordinate,TaskType taskType)
{
	UNUSED(cordinate);
	ClearTaskQueue();
	Task *task = new TaskGatherResource(this,taskType);
	m_taskQueue.push(task);	
	return false;
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
	ClearTaskQueue();
	Task *task = new TaskDropResource(this, FindMyTownCenter());
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and pushes building town center task in queue
*@param   : Town center build location
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushBuildTownCenterTask(IntVector2 cordinate)
{
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskBuildHouse(m_map, this, mapPosition);
	m_taskQueue.push(task);
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
	m_taskQueue.push(task);
	return true;

	/*Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskBuildArmySpawner(m_map, this, mapPosition);
	if (task->CheckAndReduceResources())
	{
		m_taskQueue.push(task);
		return true;
	}
	delete task;
	CreateAndPushIdleTask(IntVector2::ONE);
	return false;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Creates and pushes long range attack task in queue
*@param   : Attack position in map
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushLongRangeAttackTask(IntVector2 cordinate)
{
	ClearTaskQueue();
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskLongRangeAttack(m_map, this, m_map->GetTileIndex(mapPosition));
	m_taskQueue.push(task);
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
	ClearTaskQueue();
	Vector2 mapPosition = m_map->GetMapPosition(cordinate);
	Task *task = new TaskShortRangeAttack(m_map, this, m_map->GetTileIndex(mapPosition));
	m_taskQueue.push(task);
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
	m_taskQueue.push(task);
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
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushDefenseTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskDefense(m_map, this);
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Creates and pushes follow task to job queue
*@param   : Current cordinate
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushFollowTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskFollow(m_map, this);
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Creates and pushes retreat task to job queue
*@param   : Current cordinate
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushRetreatTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskRetreat(m_map, this);
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Creates and pushes patrol task to job queue
*@param   : Current cordinate
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushPatrolTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskPatrol(m_map, this);
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/05
*@purpose : Creates and pushes explore task to job queue
*@param   : Current cordinate
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushExploreTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskExplore(m_map, this);
	m_taskQueue.push(task);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CreateAndPushAttackTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	Task *task = new TaskAttack(m_map, this,m_attackRange);
	m_taskQueue.push(task);
	return true;
}





/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfNoEnemiesAliveToExplore(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs)
{
	if (inputs.m_enemyLongRangeArmyCount + inputs.m_enemyShortRangeArmyCount == 0)
	{
		if(inputs.m_inRangeEnemyTownCenterCount == 0)
		{
			SetDesiredStrategyAsOutputForNN(EXPLORE, 1);
			m_state.m_neuralNetPoints++;
			CopyDesiredStrategyValuesIntoDesiredNNOuputs();
			return true;
		}
		SetDesiredStrategyAsOutputForNN(ATTACK, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/03/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfNoEnemiesAliveToExplore(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs functionValues, bool value)
{
	// ATTACK
	NNInputVectors.at(NNInput_enemyLongRangeArmyCount)		 =  functionValues.m_enemyLongRangeArmyCount        * priority;
	NNInputVectors.at(NNInput_enemyShortRangeArmyCount)		 =  functionValues.m_enemyShortRangeArmyCount       * priority;
	// EXPLORE
	// FOLLOW
	// PATROL
	// RETREAT
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfTownCenterUnderAttack(std::vector<double> &NNInputVectors,Strategy strategy,double priority,NNInputs functionValues)
{
	switch (strategy)
	{
	case RETREAT:
		NNInputVectors.at(NNInput_allyTownCenterHealth) =            functionValues.m_allyTownCenterHealth           * priority;
		NNInputVectors.at(NNInput_allyTownCenterHealthLastFrame) =  -functionValues.m_allyTownCenterHealthLastFrame  * priority;
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount) =      functionValues.m_inRangeAllyTownCenterCount     * priority;
		priority--;
		break;
	case ATTACK:
		NNInputVectors.at(NNInput_allyTownCenterHealth)          =   functionValues.m_allyTownCenterHealth          * priority;
		NNInputVectors.at(NNInput_allyTownCenterHealthLastFrame) =  -functionValues.m_allyTownCenterHealthLastFrame * priority;
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)    =   functionValues.m_inRangeAllyTownCenterCount    * priority;
		break;
	}
	if(true)
	{
		//return;
	}
	TownCenter *townCenter = (TownCenter*)FindMyTownCenter();

	if(townCenter->IsUnderAttack())
	{
		if(functionValues.m_inRangeAllyTownCenterCount == 0)
		{
			SetDesiredStrategyAsOutputForNN(RETREAT, 1);
			m_state.m_neuralNetPoints++;
			CopyDesiredStrategyValuesIntoDesiredNNOuputs();
			return true;
		}
		if(functionValues.m_inRangeAllyTownCenterCount != 0)
		{
			SetDesiredStrategyAsOutputForNN(ATTACK, 1);
			m_state.m_neuralNetPoints++;
			CopyDesiredStrategyValuesIntoDesiredNNOuputs();
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/03/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfTownCenterUnderAttack(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value)
{

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/10
*@purpose : Checks if current entity is under attack and cannot overpower
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetAttackStrategyIfEntityUnderAttack(std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs)
{
	switch (strategy)
	{
	case ATTACK:
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)           = inputs.m_inRangeAllyTownCenterCount         * priority;
		NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue)    = inputs.m_inRangeAttackingEnemyHeatMapValue  * priority;
		priority--;
		break;
	case RETREAT:
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)           = inputs.m_inRangeAllyTownCenterCount        * priority;
		NNInputVectors.at(NNInput_health)                               = inputs.m_health - 0.4					     * priority;
		break;
	}
	if (true)
	{
		//return;
	}




	if (inputs.m_inRangeAllyTownCenterCount != 0)
	{
		if(inputs.m_inRangeAttackingEnemyHeatMapValue > 0)
		{
			SetDesiredStrategyAsOutputForNN(ATTACK, 1);
			m_state.m_neuralNetPoints++;
			CopyDesiredStrategyValuesIntoDesiredNNOuputs();
			return true;
		}
	}
	
	if(inputs.m_inRangeAllyTownCenterCount == 0)
	{
		if(inputs.m_health < 0.4f)
		{
			SetDesiredStrategyAsOutputForNN(RETREAT, 1);
			m_state.m_neuralNetPoints++;
			CopyDesiredStrategyValuesIntoDesiredNNOuputs();
			return true;
		}
	}
	
	return false;
}







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfEnemiesOutweighsAllies(std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs)
{
	switch (strategy)
	{
	case ATTACK:
		NNInputVectors.at(NNInput_inRangeAttackingAllyHeatMapValue)  =  inputs.m_inRangeAttackingAllyHeatMapValue   * priority;
		NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) = -inputs.m_inRangeAttackingEnemyHeatMapValue  * priority;
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)        =  inputs.m_inRangeAllyTownCenterCount         * priority;
		priority--;
		break;
	case RETREAT:
		NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) =  inputs.m_inRangeAttackingEnemyHeatMapValue  * priority;
		NNInputVectors.at(NNInput_inRangeAttackingAllyHeatMapValue)  = -inputs.m_inRangeAttackingAllyHeatMapValue   * priority;
		NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) =  inputs.m_inRangeAttackingEnemyHeatMapValue  * priority;
		break;
	}
	if (true)
	{
		//return;
	}

	if (inputs.m_inRangeAttackingAllyHeatMapValue > inputs.m_inRangeAttackingEnemyHeatMapValue && inputs.m_inRangeAttackingEnemyHeatMapValue > 0)
	{
		SetDesiredStrategyAsOutputForNN(ATTACK, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	
	else if(inputs.m_inRangeAttackingAllyHeatMapValue < inputs.m_inRangeAttackingEnemyHeatMapValue && inputs.m_inRangeAttackingEnemyHeatMapValue > 0)
	{
		SetDesiredStrategyAsOutputForNN(RETREAT, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	if (inputs.m_inRangeStationaryEnemyHeatMapValue > 0)
	{
		SetDesiredStrategyAsOutputForNN(ATTACK, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/03/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfEnemiesOutweighsAllies(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value)
{
	NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) = inputs.m_inRangeAttackingEnemyHeatMapValue   * priority;
	NNInputVectors.at(NNInput_inRangeAttackingAllyHeatMapValue)  = inputs.m_inRangeStationaryEnemyHeatMapValue  * priority;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyPatrol(std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs)
{
	switch (strategy)
	{
	case PATROL:
		NNInputVectors.at(NNInput_allyInPatrolMode )                 =  inputs.m_inRangeAttackingAllyHeatMapValue   * priority;
		NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) = -inputs.m_inRangeAttackingEnemyHeatMapValue  * priority;
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)        =  inputs.m_inRangeAllyTownCenterCount         * priority;
		break;
	}

	if(true)
	{
		//return true;
	}

	float enemyCount = inputs.m_enemyLongRangeArmyCount + inputs.m_enemyShortRangeArmyCount;
	float allyCount = inputs.m_allyLongRangeArmyCount + inputs.m_allyShortRangeArmyCount;
	if(allyCount > enemyCount)
	{
		return false;
	}

	if(inputs.m_allyInPatrolMode < 0.4)
	{
		SetDesiredStrategyAsOutputForNN(PATROL, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/03/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyPatrol(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value)
{
	NNInputVectors.at(NNInput_allyInPatrolMode )                 =  inputs.m_inRangeAttackingAllyHeatMapValue   * priority;
	NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) = -inputs.m_inRangeAttackingEnemyHeatMapValue  * priority;
	NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)        =  inputs.m_inRangeAllyTownCenterCount         * priority;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyExplore(std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs)
{
	switch (strategy)
	{
	case EXPLORE:
		NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue)  =  0.5 * inputs.m_inRangeAttackingEnemyHeatMapValue   * priority;
		NNInputVectors.at(NNInput_inRangeStationaryEnemyHeatMapValue) =  0.5 * inputs.m_inRangeStationaryEnemyHeatMapValue  * priority;
		break;
	}

	if (true)
	{
		//return true;
	}


	if (inputs.m_inRangeAttackingEnemyHeatMapValue == 0 && inputs.m_inRangeStationaryEnemyHeatMapValue == 0)
	{
		SetDesiredStrategyAsOutputForNN(EXPLORE, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/03/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyExplore(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value)
{
	NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue) =  inputs.m_inRangeAttackingEnemyHeatMapValue   * priority;
	NNInputVectors.at(NNInput_inRangeStationaryEnemyHeatMapValue) = inputs.m_inRangeStationaryEnemyHeatMapValue  * priority;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyAttack(std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs)
{
	if (inputs.m_inRangeStationaryEnemyHeatMapValue > 0)
	{
		SetDesiredStrategyAsOutputForNN(ATTACK, 1);
		m_state.m_neuralNetPoints++;
		CopyDesiredStrategyValuesIntoDesiredNNOuputs();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2019/03/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyAttack(std::vector<double> &NNInputVectors, Strategy strategy, double priority, NNInputs inputs, bool value)
{
	NNInputVectors.at(NNInput_inRangeAttackingEnemyHeatMapValue)  = inputs.m_inRangeAttackingEnemyHeatMapValue   * priority;
	NNInputVectors.at(NNInput_inRangeStationaryEnemyHeatMapValue) = inputs.m_inRangeStationaryEnemyHeatMapValue  * priority;
}*/

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
	case SHORT_RANGE_ARMY:
		return "SHORT_RANGE_ARMY";
		break;
	case LONG_RANGE_ARMY:
		return "LONG_RANGE_ARMY";
		break;
	case HOUSE:
		return "HOUSE";
		break;
	case TOWN_CENTER:
		return "TOWN_CENTER";
		break;
	case RESOURCE_FOOD:
		return "RESOURCE_FOOD";
		break;
	case RESOURCE_STONE:
		return "RESOURCE_STONE";
		break;
	case RESOURCE_WOOD:
		return "RESOURCE_WOOD";
		break;
	case ARMY_SPAWNER:
		return "ARMY_SPAWNER";
		break;
	default:
		break;
	}
	return "";
}

std::string Entity::GetStrategyAsString(Strategy strategy)
{
	if(true)
	{
		return "";
	}
	switch (strategy)
	{
	case ATTACK:
		return "Attack";
		break;
	default:
		break;
	}
	return "";
}

Strategy Entity::GetStrategyFromString(std::string strategyStr)
{
	if (strategyStr == "Attack")
	{
		return ATTACK;
	}
	return ATTACK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ClampAndExtendTo01(float value)
{
	if (value > 0)
	{
		value = 1;
		return value;
	}
	return 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ConvertValueToFunctionBool(float value)
{
	if (value > 0)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FunctionToCheckEquality(float value1, float value2)
{
	if(value2 == value1)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetAttackStrategyIfNoEnemiesAliveTo(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1 = inputs.m_inRangeEnemyTownCenterCount;
	float functionValue = ClampAndExtendTo01(variable1);
	strategy = ATTACK;
	return ConvertValueToFunctionBool(functionValue);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetExploreStrategyIfNoEnemiesAliveToExplore(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1 = inputs.m_enemyLongRangeArmyCount + inputs.m_enemyShortRangeArmyCount;
	variable1 = ClampAndExtendTo01(variable1);
	float functionValue = 1 - variable1;
	strategy = EXPLORE;
	return ConvertValueToFunctionBool(functionValue);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetStrategyRetreatIfTownCenterUnderAttack(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1		= (float)FunctionToCheckEquality(inputs.m_allyTownCenterHealthLastFrame,inputs.m_allyTownCenterHealth);
	variable1			= 1 - variable1;
	variable1			= ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy			= RETREAT;
	return ConvertValueToFunctionBool(functionValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetStrategyAttackIfTownCenterUnderAttack(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1     = (float)FunctionToCheckEquality(inputs.m_allyTownCenterHealthLastFrame, inputs.m_allyTownCenterHealth);
	variable1           = 1 - variable1;
	variable1           = ClampAndExtendTo01(variable1);

	float variable2     = inputs.m_inRangeAllyTownCenterCount;
	float functionValue = ClampAndExtendTo01(variable1 * variable2);

	strategy = ATTACK;
	return ConvertValueToFunctionBool(functionValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetAttackStrategyIfEntityUnderAttack(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1 = inputs.m_inRangeAllyTownCenterCount * inputs.m_inRangeAttackingEnemyHeatMapValue;
	variable1 = ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy = ATTACK;
	return ConvertValueToFunctionBool(functionValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetRetreatStrategyIfEntityUnderAttack(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1 = (1 - inputs.m_inRangeAllyTownCenterCount) * inputs.m_health;
	if (inputs.m_health < 0.4f)
	{
		variable1 = 0.5;
	}
	else
	{
		variable1 = 0.f;
	}
	variable1			= ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy			= RETREAT;
	return ConvertValueToFunctionBool(functionValue);
	/*if (inputs.m_inRangeAllyTownCenterCount == 0)
	{
	if (inputs.m_health < 0.4f)
	{
	SetDesiredStrategyAsOutputForNN(RETREAT, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	return true;
	}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetAttackStrategyIfEnemiesOutweighs(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1		= (inputs.m_inRangeAttackingAllyHeatMapValue - inputs.m_inRangeAttackingEnemyHeatMapValue)*inputs.m_inRangeAttackingEnemyHeatMapValue;
	variable1			= ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy			= ATTACK;
	return ConvertValueToFunctionBool(functionValue);
	/*if (inputs.m_inRangeAttackingAllyHeatMapValue > inputs.m_inRangeAttackingEnemyHeatMapValue && inputs.m_inRangeAttackingEnemyHeatMapValue > 0)
	{
	SetDesiredStrategyAsOutputForNN(ATTACK, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	return true;
	}*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetRetreatStrategyIfEnemiesOutweighs(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1		= (inputs.m_inRangeAttackingEnemyHeatMapValue - inputs.m_inRangeAttackingAllyHeatMapValue)*inputs.m_inRangeAttackingEnemyHeatMapValue;
	variable1			= ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy			= RETREAT;
	return ConvertValueToFunctionBool(functionValue);

	/*if (inputs.m_inRangeAttackingAllyHeatMapValue < inputs.m_inRangeAttackingEnemyHeatMapValue && inputs.m_inRangeAttackingEnemyHeatMapValue > 0)
	{
	SetDesiredStrategyAsOutputForNN(RETREAT, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	return true;
	}*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetStrategyPatrol(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1 = inputs.m_inRangeAttackingEnemyHeatMapValue + inputs.m_inRangeStationaryEnemyHeatMapValue;
	variable1 = ClampAndExtendTo01(variable1);
	variable1 = 1 - variable1;
	variable1 = ClampAndExtendTo01(variable1);
	if(inputs.m_allyInPatrolMode < 0.4) 
	{
		variable1 = 0.5;
	}
	else
	{
		variable1 = 0.f;
	}
	float functionValue = variable1;
	strategy = PATROL;
	return ConvertValueToFunctionBool(functionValue);

	/*if (inputs.m_allyInPatrolMode < 0.4)
	{
	SetDesiredStrategyAsOutputForNN(PATROL, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	return true;
	}
	return false;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetStrategyExplore(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1		= inputs.m_inRangeAttackingEnemyHeatMapValue + inputs.m_inRangeStationaryEnemyHeatMapValue;
	variable1			= ClampAndExtendTo01(variable1);
	variable1			= 1 - variable1;
	variable1			= ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy			= EXPLORE;
	return ConvertValueToFunctionBool(functionValue);

	/*if (inputs.m_inRangeAttackingEnemyHeatMapValue == 0 && inputs.m_inRangeStationaryEnemyHeatMapValue == 0)
	{
	SetDesiredStrategyAsOutputForNN(EXPLORE, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	return true;
	}
	return false;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/04/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CheckAndSetStrategyAttack(std::vector<double> &NNInputVectors, Strategy &strategy, NNInputs inputs)
{
	float variable1		= inputs.m_inRangeStationaryEnemyHeatMapValue;
	variable1			= ClampAndExtendTo01(variable1);
	float functionValue = variable1;
	strategy			= ATTACK;
	return ConvertValueToFunctionBool(functionValue);

	/*
	if (inputs.m_inRangeStationaryEnemyHeatMapValue > 0)
	{
	SetDesiredStrategyAsOutputForNN(ATTACK, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();
	return true;
	}
	return false;*/
}