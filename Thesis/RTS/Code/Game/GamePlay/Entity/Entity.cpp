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
/*DATE    : 2018/11/04
*@purpose : returns team's score board
*@param   : NIL
*@return  : score board
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
ScoreBoard Entity::GetMyTeamScoreBoard()
{
	if(m_teamID == 1)
	{
		return m_map->m_team1;
	}
	return m_map->m_team2;
}*/

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
*@purpose : Gets the cordinate and returs
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
	m_statsSupported.push_back(HEALTH);
	m_statsSupported.push_back(RESOURCE_COUNT);

	int inputCount = static_cast<int>( g_entityMiniMapMaxHeight * g_entityMiniMapMaxWidth);
//	m_neuralNet.CreateNeuralNetwork(m_map->m_maxHeight*m_map->m_maxWidth + g_extraNNInputs, g_hiddenLayerCount, static_cast<int>(m_taskTypeSupported.size()));
	m_neuralNet.CreateNeuralNetwork(inputCount + g_extraNNInputs, g_hiddenLayerCount, static_cast<int>(m_taskTypeSupported.size()));

	bool loadSuccess = m_neuralNet.LoadFromFile(GetGlobalBestFilePath().c_str());
	if(!loadSuccess)
	{
		m_neuralNet.SetRandomWeight();
	}
	// input + 6 -> for game stat && output + 2 for positions x,y

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
	m_state.m_health = m_health;
	m_state.m_position = GetPosition();
	m_previousState = m_state;
	m_minSafeArea = FindMyTownCenter()->GetCordinates() - IntVector2(3, 3);
	m_maxSafeArea = FindMyTownCenter()->GetCordinates() + IntVector2(3, 3);
	m_minSafeArea = m_map->ClampCoordinates(m_minSafeArea);
	m_maxSafeArea = m_map->ClampCoordinates(m_maxSafeArea);

	if(m_teamID > 0)
	{
		m_minTeritaryArea = IntVector2(m_map->m_maxWidth ,m_map->m_maxHeight) * static_cast<int>((static_cast<float>(m_teamID) - 1.f)/2.f);
		m_maxTeritaryArea = m_minTeritaryArea + IntVector2(m_map->m_maxWidth, m_map->m_maxHeight) * 2;
		m_minTeritaryArea = m_map->ClampCoordinates(m_minTeritaryArea);
		m_maxTeritaryArea = m_map->ClampCoordinates(m_maxTeritaryArea);
	}

	for (int index = 0; index < 15; index++)
	{
		m_state.m_favoredMoveTaskCount.push_back(0);
		m_previousState.m_favoredMoveTaskCount.push_back(0);
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
			
			m_neuralNet.StoreToFile(("Data\\NN\\"+GetEntityTypeAsString(m_type) + "_"+ToString(m_teamID)+".txt").c_str());
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
			if (g_currentSelectedEntity->m_type == SHORT_RANGE_ARMY || g_currentSelectedEntity->m_type == LONG_RANGE_ARMY)
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
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/30
*@purpose : Retrives my minimap
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<double> Entity::GetMyMiniMap()
{
	std::vector<double> minmapValues;
	for(int index = 0;index < g_entityMiniMapMaxHeight * g_entityMiniMapMaxWidth;index++)
	{
		minmapValues.push_back(0);
	}

	int xPosition = GetCordinates().x;
	int yPosition = GetCordinates().y;

	int minX = xPosition - g_entityMiniMapMaxWidth  / 2;
	int minY = yPosition - g_entityMiniMapMaxHeight / 2;

	int maxX = xPosition + g_entityMiniMapMaxWidth / 2;
	int maxY = yPosition + g_entityMiniMapMaxHeight / 2;

	if(minX < 0)
	{
		maxX += (0 - minX);
	}
	if(minY < 0)
	{
		maxY += (0 - minY);
	}
	if(maxX > m_map->m_maxWidth)
	{
		minX -= (maxX - m_map->m_maxWidth);
	}
	if(maxY > m_map->m_maxHeight)
	{
		minY -= (maxY - m_map->m_maxHeight);
	}



	int mapMinX = GetMax(minX, 0);
	int mapMinY = GetMax(minY, 0);

	int mapMaxX = GetMin(maxX, m_map->m_maxWidth);
	int mapMaxY = GetMin(maxY, m_map->m_maxHeight);

	for(int indexY = mapMinY;indexY < mapMaxY;indexY++)
	{
		for(int indexX = mapMinX;indexX < mapMaxX;indexX++)
		{
			double minimapValue    = m_map->GetMiniMapValueAtPosition(indexX, indexY);
			IntVector2 miniMapCords(indexX - mapMinX, indexY - mapMinY);
			int miniMapIndex = miniMapCords.y * g_entityMiniMapMaxHeight + miniMapCords.x;
			if(indexX == GetCordinates().x && indexY == GetCordinates().y)
			{
				minmapValues.at(miniMapIndex) = 1;
				continue;
			}
			minmapValues.at(miniMapIndex) = (minimapValue);
			if(m_miniMapDebug)
			{
				m_map->CreateDebugLetterEntity(m_map->GetMapPosition(IntVector2(indexX, indexY)), ToString(miniMapIndex), 8, Rgba::YELLOW);
				//m_map->CreateExplosions(m_map->GetMapPosition(IntVector2(indexX, indexY)));
			}
		}
	}
	return minmapValues;


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Trains NN with the snapshot input
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::TrainNN(Task *task)
{
	UNUSED(task);
	for(int index = 0;index < m_taskTypeSupported.size();index++)
	{
		if(m_neuralNet.m_outputs->m_neurons.at(index).m_sumOfPreviousLayer < -3.f)
		{
			m_desiredOuputs.at(index) = 1;
		}
		if (m_neuralNet.m_outputs->m_neurons.at(index).m_sumOfPreviousLayer > 3.f)
		{
			m_desiredOuputs.at(index) = 0;
		}
	}

	if (m_state.m_neuralNetPoints > m_previousState.m_neuralNetPoints)
	{
		for (int index = 0; index < 10; index++)
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
			m_taskQueue.pop();
			if(g_enableNeuralNet && m_taskQueue.size() == 0)
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
	if (!g_enableNeuralNet)
	{
		return;
	}
	
	if(!m_map->HasTrainingEnabled(this))
	{
		return;
	}

	PrintDebugNN();
	TownCenter *townCenter = m_map->m_townCenters.at(m_teamID - 1);
	if (townCenter == nullptr)
	{
		return;
	}

	std::vector<double> m_gameStats;
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(townCenter->m_resourceStat.m_food)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(townCenter->m_resourceStat.m_stone)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(townCenter->m_resourceStat.m_wood)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(townCenter->m_resourceStat.m_buildings)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(townCenter->m_resourceStat.m_units)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(townCenter->m_resourceStat.m_unitsKilled)));
	m_gameStats.push_back(m_neuralNet.GetSigmoidValue(static_cast<double>(m_health)));
	if (HasResource())
	{
		m_gameStats.push_back((static_cast<double>(1)));
	}
	else
	{
		m_gameStats.push_back((static_cast<double>(0)));
	}

	IntVector2 myPosition = m_map->GetCordinates(GetPosition());
	double minimapValue    = m_map->GetMiniMapValueAtPosition(myPosition.x, myPosition.y);
	m_map->SetMiniMapValues(myPosition.x, myPosition.y, 1);

	std::vector<double> entityMiniMapInput = GetMyMiniMap();

	m_neuralNet.FeedForward(entityMiniMapInput, m_gameStats);
	m_map->SetMiniMapValues(myPosition.x, myPosition.y, static_cast<float>(minimapValue));
}

void Entity::UpdateTaskFromNN(float deltaTime)
{
	if (!m_map->HasTrainingEnabled(this))
	{
		return;
	}
	double max = 0;
	TaskType task			= GetTaskFromNNOutput(max);
	IntVector2 taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position);
	m_previousState			= m_state;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float logTime = 3;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	deltaTime = logTime;
	switch (task)
	{
	case TASK_IDLE:
		{
			CreateAndPushIdleTask(taskPosition);
			break;
		}
	case TASK_MOVE:
		{
		
		std::vector<Entity*> entityList     = m_map->GetAllEntitiesNearLocation(m_map->GetMapPosition(taskPosition), 1);
		std::vector<Entity*> townCenterList = GetMyTownCenterEntityFromList(entityList);
		std::vector<Entity*> resourceList   = GetResourceEntityFromList(entityList);
		//if(townCenterList.size() > 0 || resourceList.size() > 0)
		{
			CreateAndPushMoveTask(taskPosition);
		}
		/*else
		{
			CreateAndPushIdleTask(taskPosition);
		}*/
			
		}
		break;
	case TASK_GATHER_RESOURCE:
	{
		CreateAndPushGatherResourceTask(taskPosition);
	}
		break;
	case TASK_DROP_RESOURCE:
	{
		logTime = deltaTime;
		CreateAndPushDropResourceTask(taskPosition);
	}
		break;
	case TASK_BUILD_TOWNCENTER:
	{
		logTime = deltaTime;
		CreateAndPushBuildTownCenterTask(taskPosition);
	}
		break;
	case TASK_BUILD_HOUSE:
	{
		logTime = deltaTime;
		CreateAndPushBuildHouseTask(taskPosition);
	}
		break;
	case TASK_BUILD_ARMY_SPAWNER:
	{
		logTime = deltaTime;
		CreateAndPushBuildArmySpawnerTask(taskPosition);
	}
		break;
	case TASK_LONG_ATTACK:
	{
		IntVector2 currentPosition = m_map->GetCordinates(m_previousState.m_position);

		double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
		double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

		taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position, 4, 4);
		//m_map->CreateExplosions(m_map->GetMapPosition(taskPosition), Rgba::RED);
		CreateAndPushLongRangeAttackTask(taskPosition);
	}
		break;
	case TASK_SHORT_ATTACK:
	{
		IntVector2 currentPosition = m_map->GetCordinates(m_previousState.m_position);

		double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
		double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

		taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position,2,2);
		
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
	if (g_isCurrentlyTraining == false && g_currentSelectedEntity == this && g_enableDebugPrints && static_cast<double>(m_lastDebug + m_debugPrintDelay) < GetCurrentTimeSeconds())
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
		int index = GetMostFavoredMoveTask(&maxValue);
		std::string taskStr = GetFavoredMoveToAsString((FavoredMoveStats)index);

		DebugDraw::GetInstance()->DebugRenderLogf(taskStr, m_debugPrintDelay, GetTeamColor(), GetTeamColor());
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
	if(m_state.m_resourceFoodDropped < g_maxResourceCountRequired)
	{
		m_scoreBoard.m_resourceFoodCollected++;
	}
	m_map->GetMyScoreBoard(this).m_resourceFoodCollected++;
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
	m_map->GetMyScoreBoard(this).m_resourceStoneCollected++;
	if(m_state.m_resourceStoneDropped < g_maxResourceCountRequired)
	{
		m_scoreBoard.m_resourceStoneCollected++;
	}
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
	m_map->GetMyScoreBoard(this).m_resourceWoodCollected++;
	if(m_state.m_resourceWoodDropped < g_maxResourceCountRequired)
	{
		m_scoreBoard.m_resourceWoodCollected++;
	}
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
	m_map->GetMyScoreBoard(this).m_armySpawnersBuilt++;
	if (m_state.m_numberOfArmySpawnerBuilt < g_maxArmySpawnerBuilt)
	{
		m_scoreBoard.m_armySpawnersBuilt++;
	}
	else
	{
		m_scoreBoard.m_armySpawnersBuilt--;
	}
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
	m_map->GetMyScoreBoard(this).m_housesBuilt++;
	if(m_state.m_numberOfHouseBuilt < g_maxHousesBuilt)
	{
		m_scoreBoard.m_housesBuilt--;
	}
	else
	{
		m_scoreBoard.m_housesBuilt++;
	}
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
	m_map->GetMyScoreBoard(this).m_shortRangeArmySpawned++;
	m_scoreBoard.m_shortRangeArmySpawned++;
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
	m_map->GetMyScoreBoard(this).m_longRangeArmySpawned++;
	m_scoreBoard.m_longRangeArmySpawned++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update with unit stat for enemies attacked
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForEnemiesAttacked(Entity* attackedEntity ,int count)
{
	m_state.m_enemiesAttacked += count;
	switch (attackedEntity->m_type)
	{
	case HOUSE:
		m_map->GetMyScoreBoard(this).m_housesAttacked++;
		m_scoreBoard.m_housesAttacked++;
		break;
	case ARMY_SPAWNER:
		m_map->GetMyScoreBoard(this).m_armySpawnerAttacked++;
		m_scoreBoard.m_armySpawnerAttacked++;
		break;
	case CIVILIAN:
		m_map->GetMyScoreBoard(this).m_civiilansAttacked++;
		m_scoreBoard.m_civiilansAttacked++;
		break;
	case SHORT_RANGE_ARMY:
		m_map->GetMyScoreBoard(this).m_shortRangeArmyAttacked++;
		m_scoreBoard.m_shortRangeArmyAttacked++;
		break;
	case LONG_RANGE_ARMY:
		m_map->GetMyScoreBoard(this).m_longRangeArmyAttacked++;
		m_scoreBoard.m_longRangeArmyAttacked++;
		break;
	case TOWN_CENTER:
		m_map->GetMyScoreBoard(this).m_townCenterAttacked++;
		m_scoreBoard.m_townCenterAttacked++;
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with enemies killed
*@param   : Update by count
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForEnemiesKilled(Entity* attackedEntity,int count)
{
	m_state.m_enemiesKilled += count;
	switch (attackedEntity->m_type)
	{
	case HOUSE:
		m_map->GetMyScoreBoard(this).m_housesDestroyed++;
		m_scoreBoard.m_housesDestroyed++;
		break;
	case ARMY_SPAWNER:
		m_map->GetMyScoreBoard(this).m_armySpawnerDestroyed++;
		m_scoreBoard.m_armySpawnerDestroyed++;
		break;
	case CIVILIAN:
		m_map->GetMyScoreBoard(this).m_civiliansKilled++;
		m_scoreBoard.m_civiliansKilled++;
		break;
	case SHORT_RANGE_ARMY:
		m_map->GetMyScoreBoard(this).m_shortRangeArmyKilled++;
		m_scoreBoard.m_shortRangeArmyKilled++;
		break;
	case LONG_RANGE_ARMY:
		m_map->GetMyScoreBoard(this).m_longRangeArmyKilled++;
		m_scoreBoard.m_longRangeArmyKilled++;
		break;
	case TOWN_CENTER:
		m_map->GetMyScoreBoard(this).m_townCenterDestroyed++;
		m_scoreBoard.m_townCenterDestroyed++;
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Update stat with villager spawned
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateUnitStatForVillagerSpawned(int count)
{
	m_state.m_villagerSpawned += count;
	m_map->GetMyScoreBoard(this).m_civiliansSpawned++;
	m_scoreBoard.m_civiliansSpawned++;
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
/*DATE    : 2018/10/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::UpdateMostFavoredMoveTask(EntityState prevState,IntVector2 cords)
{
	int oldIndex = m_map->GetTileIndex(prevState.m_position);
	int newIndex = m_map->GetTileIndex(cords);

	CellSensoryValues oldSensoryValue = m_map->m_cellSensoryValues.at(oldIndex);
	CellSensoryValues newSensoryValue = m_map->m_cellSensoryValues.at(newIndex);

	for(int index = 0;index < oldSensoryValue.m_entityNearness.size();index++)
	{
		float newPositionSensoryValue = newSensoryValue.m_entityNearness.at(index);
		float oldPositionSensoryValue = oldSensoryValue.m_entityNearness.at(index);
		if(m_type == SHORT_RANGE_ARMY && (((FavoredMoveStats)index) == FAVORED_MOVETO_TEAM2_ARMY_SHORT_RANGE) && m_teamID == 2)
		{
			oldPositionSensoryValue -= m_map->GetCellDistance(cords,m_map->GetCordinates(prevState.m_position)) / m_map->m_classAWarriors.size();
		}

		if(newPositionSensoryValue < oldPositionSensoryValue)
		{
			m_state.m_favoredMoveTaskCount.at(index)++;
		}
	}
	/*if(newSensoryValue.m_team1ArmyNearness > oldSensoryValue.m_team1ArmyNearness)
	{
		if(m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToAllyArmy++;
		}
		else if(m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToEnemyArmy++;
		}
	}

	if(newSensoryValue.m_team1BuildingNearness > oldSensoryValue.m_team1BuildingNearness)
	{
		if(m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToAllyBuilding++;
		}
		else if(m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToEnemyBuilding++;
		}
	}

	if(newSensoryValue.m_team1CivilianNearness > oldSensoryValue.m_team1CivilianNearness)
	{
		if(m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToAllyCivilian++;
		}
		else if(m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToEnemyCivilian++;
		}
	}

	if(newSensoryValue.m_team1TownCenterNearness > newSensoryValue.m_team1TownCenterNearness)
	{
		if(m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToAllyTownCenter++;
		}
		else if(m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToEnemyTownCenter++;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (newSensoryValue.m_team2ArmyNearness > oldSensoryValue.m_team2ArmyNearness)
	{
		if (m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToAllyArmy++;
		}
		else if (m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToEnemyArmy++;
		}
	}

	if (newSensoryValue.m_team2BuildingNearness > oldSensoryValue.m_team2BuildingNearness)
	{
		if (m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToAllyBuilding++;
		}
		else if (m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToEnemyBuilding++;
		}
	}

	if (newSensoryValue.m_team2CivilianNearness > oldSensoryValue.m_team2CivilianNearness)
	{
		if (m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToAllyCivilian++;
		}
		else if (m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToEnemyCivilian++;
		}
	}

	if (newSensoryValue.m_team2TownCenterNearness > newSensoryValue.m_team2TownCenterNearness)
	{
		if (m_teamID == 2)
		{
			m_state.m_numTimesFavoredMoveToAllyTownCenter++;
		}
		else if (m_teamID == 1)
		{
			m_state.m_numTimesFavoredMoveToEnemyTownCenter++;
		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Retrieves most favored entity task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Entity::GetMostFavoredMoveTask(float *max)
{
	*max = -1;
	int retIndex = 0;
	for(int index = 0;index < m_state.m_favoredMoveTaskCount.size();index++)
	{
		if(m_state.m_favoredMoveTaskCount.at(index) > *max)
		{
			*max = m_state.m_favoredMoveTaskCount.at(index);
			retIndex = index;
		}
	}
	return retIndex;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::IsCurrentMoveFavoredByPastMoveTask(EntityState prevState, IntVector2 cords)
{
/*
	float total = m_state.m_numTimesFavoredMoveToAllyArmy + m_state.m_numTimesFavoredMoveToAllyBuilding
	+ m_state.m_numTimesFavoredMoveToAllyCivilian + m_state.m_numTimesFavoredMoveToAllyTownCenter + m_state.m_numTimesFavoredMoveToEnemyArmy 
		+ m_state.m_numTimesFavoredMoveToEnemyBuilding + m_state.m_numTimesFavoredMoveToEnemyCivilian + m_state.m_numTimesFavoredMoveToEnemyTownCenter;*/

	int oldIndex = m_map->GetTileIndex(prevState.m_position);
	int newIndex = m_map->GetTileIndex(cords);

	CellSensoryValues oldSensoryValue = m_map->m_cellSensoryValues.at(oldIndex);
	CellSensoryValues newSensoryValue = m_map->m_cellSensoryValues.at(newIndex);

	/*float team1ArmyNearness		  = newSensoryValue.m_team1ArmyNearness       - oldSensoryValue.m_team1ArmyNearness;
	float team1BuildingNearness   = newSensoryValue.m_team1BuildingNearness   - oldSensoryValue.m_team1BuildingNearness;
	float team1CivilianNearness   = newSensoryValue.m_team1CivilianNearness   - oldSensoryValue.m_team1CivilianNearness;
	float team1TownCenterNearness = newSensoryValue.m_team1TownCenterNearness - newSensoryValue.m_team1TownCenterNearness;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	float team2ArmyNearness       = newSensoryValue.m_team2ArmyNearness       - oldSensoryValue.m_team2ArmyNearness;
	float team2BuildingNearness   = newSensoryValue.m_team2BuildingNearness   - oldSensoryValue.m_team2BuildingNearness;
	float team2CivilianNearness   = newSensoryValue.m_team2CivilianNearness   - oldSensoryValue.m_team2CivilianNearness;
	float team2TownCenterNearness = newSensoryValue.m_team2TownCenterNearness - newSensoryValue.m_team2TownCenterNearness;*/
	return true;
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
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredOutputToMoveToNeighbour(EntityState prevState,int distance)
{
	IntVector2 cords = m_map->GetCordinates(prevState.m_position);
	int xPosition = cords.x;
	int yPosition = cords.y;
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

	int mapMaxX = GetMin(maxX, m_map->m_maxWidth);
	int mapMaxY = GetMin(maxY, m_map->m_maxHeight);


	UNUSED(distance);
	IntVector2 neighbour = m_map->GetRandomNeighbour(m_map->GetCordinates(prevState.m_position), distance);
	float desiredXPosition = RangeMapFloat(static_cast<float>(neighbour.x),static_cast<float>(mapMinX), static_cast<float>(mapMaxX), 0.f, 1.f);
	float desiredYPosition = RangeMapFloat(static_cast<float>(neighbour.y),static_cast<float>(mapMinY), static_cast<float>(mapMaxY), 0.f, 1.f);

	desiredXPosition = GetRandomFloatZeroToOne();
	desiredYPosition = GetRandomFloatZeroToOne();

	SetDesiredOutputForTask(TASK_MOVE, 1);
	SetDesiredOutputForTask(TASK_MOVEX, desiredXPosition);
	SetDesiredOutputForTask(TASK_MOVEY, desiredYPosition);
	if(desiredXPosition > 1 || desiredYPosition > 1 || desiredXPosition < 0 || desiredYPosition < 0)
	{
		m_map->CreateExplosions(m_map->GetMapPosition(neighbour),Rgba::WHITE);
	}
	m_map->CreateExplosions(m_map->GetMapPosition(neighbour),Rgba::YELLOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredOutputToMoveToNeighbourOpposite(int cellDistance, IntVector2 cords)
{
	UNUSED(cellDistance);
	int xDistance = GetCordinates().x - cords.x;
	int yDistance = GetCordinates().y - cords.y;
	xDistance = ClampInt(xDistance,-1, 1);
	yDistance = ClampInt(yDistance,-1, 1);
	IntVector2 neighBour = GetCordinates() + IntVector2(xDistance, yDistance);
	if(m_map->IsValidCordinate(neighBour))
	{
		SetDesiredOutputForTask(TASK_MOVE, 1);
		SetDesiredOutputForTask(TASK_MOVEX, static_cast<float>(neighBour.x));
		SetDesiredOutputForTask(TASK_MOVEY, static_cast<float>(neighBour.y));
		//m_map->CreateExplosions(m_map->GetMapPosition(neighBour));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredOutputToMoveToPrevPosition()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredOutputToChooseRandomNeighbourLocation(int cellDistance)
{
	double xPosition = GetRandomDoubleInRange(0, 1);
	double yPosition = GetRandomDoubleInRange(0, 1);
	SetDesiredOutputForTask(TASK_MOVEX, xPosition);
	SetDesiredOutputForTask(TASK_MOVEY, yPosition);
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

/*void Entity::SetOuputsToTrainToMoveToPosition(IntVector2 cords)
{
	int xPosition = static_cast<int>(m_desiredOuputs.size() - 2);
	int yPosition = static_cast<int>(m_desiredOuputs.size() - 1);
	m_desiredOuputs.at(xPosition) = RangeMapFloat(static_cast<float>(cords.x), 0.f, static_cast<float>(m_map->m_maxWidth),  0.f, 1.f);
	m_desiredOuputs.at(yPosition) = RangeMapFloat(static_cast<float>(cords.y), 0.f, static_cast<float>(m_map->m_maxHeight), 0.f, 1.f);
	int taskIndex = GetIndexOfOutputTask(TASK_MOVE);
	m_desiredOuputs.at(taskIndex) = 1;
	//m_map->CreateExplosions(m_map->GetMapPosition(cords));
}*/


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
		return 0.80f;
		break;
	case RESOURCE_STONE:
		return 0.85f;
		break;
	case RESOURCE_WOOD:
		return 0.90f;
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
/*DATE    : 2018/09/22
*@purpose : Gets the task type from NN outputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType Entity::GetTaskFromNNOutput(double &max)
{
	TaskType type = m_taskTypeSupported.at(0);
	for (int outputIndex = 0; outputIndex < m_taskTypeSupported.size() - 2; outputIndex++)
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
	float xRangedValue = RangeMapFloat(static_cast<float>(taskXPosition), 0.f, 1.f, static_cast<float>(mapMinX) - 0.49, static_cast<float>(mapMaxX) + 0.49);
	float yRangedValue = RangeMapFloat(static_cast<float>(taskYPosition), 0.f, 1.f, static_cast<float>(mapMinY) - 0.49, static_cast<float>(mapMaxY) + 0.49);
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

	/*if (minX < 0)
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
	}*/



	int mapMinX = GetMax(minX, 0);
	int mapMinY = GetMax(minY, 0);

	int mapMaxX = GetMin(maxX, m_map->m_maxWidth  - 1);
	int mapMaxY = GetMin(maxY, m_map->m_maxHeight - 1);


	double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
	double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

	float xRangedValue = RangeMapFloat(static_cast<float>(taskXPosition), 0.f, 1.f, static_cast<float>(mapMinX) - 0.48, static_cast<float>(mapMaxX) + 0.48);
	float yRangedValue = RangeMapFloat(static_cast<float>(taskYPosition), 0.f, 1.f, static_cast<float>(mapMinY) - 0.48, static_cast<float>(mapMaxY) + 0.48);
	int xPos = ClampInt(RoundToNearestInt(xRangedValue), 0, m_map->m_maxWidth - 1);
	int yPos = ClampInt(RoundToNearestInt(yRangedValue), 0, m_map->m_maxHeight - 1);
	if(yPos == 4)
	{
		int a = 1;
	}
	return IntVector2(xPos, yPos);
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
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetGlobalBestFilePath()
{
	return "Data\\NN\\BestGame\\" +ToString(m_teamID)+"\\"+GetEntityTypeAsString(m_type) +"_"+ ToString(m_teamID) + ".txt";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetLocalBestFilePath()
{
	return "Data\\NN\\" +m_map->m_folder +"\\"+ ToString(m_teamID)+"\\" + GetEntityTypeAsString(m_type) +"_"+ ToString(m_teamID) + ".txt";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : returns best stat global file path
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetGlobalBestStatFilePath()
{
	return "Data\\NN\\BestGame\\" + ToString(m_teamID) + "\\BestStats\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + "_STATS.txt";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : returns best stat local file path
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetLocalBestStatFilePath()
{
	return "Data\\NN\\" + m_map->m_folder +"\\"+ToString(m_teamID)+"\\BestStats\\"+GetEntityTypeAsString(m_type) + "_"+ToString(m_teamID) + "_STATS.txt";
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
bool Entity::CreateAndPushGatherResourceTask(IntVector2 cordinate)
{
	UNUSED(cordinate);
	ClearTaskQueue();
	Task *task = new TaskGatherResource(this);
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
*@purpose : Creats and pushes building town center task in queue
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetFavoredMoveToAsString(FavoredMoveStats stats)
{
	switch (stats)
	{
	case FAVORED_MOVETO_RESOURCE_FOOD:
		return "FAVORED_MOVETO_RESOURCE";
		break;
	case FAVORED_MOVETO_TEAM1_ARMY_SHORT_RANGE:
		return "FAVORED_MOVETO_TEAM1_ARMY";
		break;
	case FAVORED_MOVETO_TEAM1_BUILDING:
		return "FAVORED_MOVETO_TEAM1_BUILDING";
		break;
	case FAVORED_MOVETO_TEAM1_CIVILIAN:
		return "FAVORED_MOVETO_TEAM1_CIVILIAN";
		break;
	case FAVORED_MOVETO_TEAM1_TOWNCENTER:
		return "FAVORED_MOVETO_TEAM1_TOWNCENTER";
		break;
	case FAVORED_MOVETO_TEAM2_ARMY_SHORT_RANGE:
		return "FAVORED_MOVETO_TEAM2_ARMY";
		break;
	case FAVORED_MOVETO_TEAM2_BUILDING:
		return "FAVORED_MOVETO_TEAM2_BUILDING";
		break;
	case FAVORED_MOVETO_TEAM2_CIVILIAN:
		return "FAVORED_MOVETO_TEAM2_CIVILIAN";
		break;
	case FAVORED_MOVETO_TEAM2_TOWNCENTER:
		return "FAVORED_MOVETO_TEAM2_TOWNCENTER";
		break;
	default:
		break;
	}
	return "INVALID";
}
