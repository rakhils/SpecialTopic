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
	m_scoreBoard.m_entity = this;

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
*@purpose : Retrieves my minimap
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
	IntVector2 minimapMins = GetMyMiniMapMins();
	int mapMinX = minimapMins.x;
	int mapMinY = minimapMins.y;
	IntVector2 minimapMaxs = GetMyMiniMapMaxs();
	int mapMaxX = minimapMaxs.x;
	int mapMaxY = minimapMaxs.y;

	for(int indexY = mapMinY;indexY < mapMaxY;indexY++)
	{
		for(int indexX = mapMinX;indexX < mapMaxX;indexX++)
		{
			double minimapValue    = m_map->GetMiniMapValueAtPositionFromEntityType(indexX, indexY,m_teamID,m_type,this);
			IntVector2 miniMapCords(indexX - mapMinX, indexY - mapMinY);
			int miniMapIndex = miniMapCords.y * g_entityMiniMapMaxHeight + miniMapCords.x;
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
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMiniMapMins(IntVector2 cords)
{
	return GetMiniMapMins(cords, g_entityMiniMapMaxWidth, g_entityMiniMapMaxHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMiniMapMins(IntVector2 cords, int width,int height)
{
	int xPosition = cords.x;
	int yPosition = cords.y;

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

	int mapMinX = GetMax(minX, 0);
	int mapMinY = GetMax(minY, 0);

	//int mapMaxX = GetMin(maxX, m_map->m_maxWidth);
	//int mapMaxY = GetMin(maxY, m_map->m_maxHeight);
	return IntVector2(mapMinX, mapMinY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMiniMapMaxs(IntVector2 cords)
{
	return GetMiniMapMaxs(cords, g_entityMiniMapMaxWidth, g_entityMiniMapMaxHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMiniMapMaxs(IntVector2 cords, int width, int height)
{
	int xPosition = cords.x;
	int yPosition = cords.y;

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

	//int mapMinX = GetMax(minX, 0);
	//int mapMinY = GetMax(minY, 0);

	int mapMaxX = GetMin(maxX, m_map->m_maxWidth);
	int mapMaxY = GetMin(maxY, m_map->m_maxHeight);
	return IntVector2(mapMaxX, mapMaxY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMyMiniMapMins()
{
	return GetMiniMapMins(GetCordinates());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Entity::GetMyMiniMapMaxs()
{
	return GetMiniMapMaxs(GetCordinates());
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

	if(m_map->m_mapMode == MAP_MODE_TRAINING_NONE_PLAY_GREEN && m_teamID == 1)
	{
		return;
	}


	if (!g_isCurrentlyTraining)
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
		for (int index = 0; index < 5; index++)
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
	if (g_skipRendering)
	{
		return;
	}
	if(m_type == SHORT_RANGE_ARMY || m_type == LONG_RANGE_ARMY)
	{
		IntVector2 minimapMins = GetMiniMapMins(GetCordinates());
		IntVector2 minimapMaxs = GetMiniMapMaxs(GetCordinates());
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
	m_scoreBoard.m_totalSteps++;
	bool interimScoreSuccess = m_scoreBoard.CalculateInterimScore(false, m_prevScoreBoard.m_interimScore);
	if(interimScoreSuccess)
	{
		m_prevScoreBoard = m_scoreBoard;
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
	UNUSED(deltaTime);
	
	if(!m_map->HasTrainingEnabled(this))
	{
		return;
	}
	if (m_map->m_mapMode == MAP_MODE_TRAINING_NONE_PLAY_GREEN && m_teamID == 1)
	{
		return;
	}

	//PrintDebugNN();
	TownCenter *townCenter = m_map->m_townCenters.at(m_teamID - 1);
	if (townCenter == nullptr)
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


	if (true)
		return;


	int foodCount  = townCenter->m_resourceStat.m_food;
	int stoneCount = townCenter->m_resourceStat.m_stone;
	int woodCount  = townCenter->m_resourceStat.m_wood;
	float newfoodCount  = RangeMapFloat(static_cast<float>(foodCount) , 0.f, 30.f, 0.f, 1.f);
	float newstoneCount = RangeMapFloat(static_cast<float>(stoneCount), 0.f, 30.f, 0.f, 1.f);
	float newwoodCount  = RangeMapFloat(static_cast<float>(woodCount) , 0.f, 30.f, 0.f, 1.f);
	newfoodCount  =	ClampFloat(newfoodCount  ,0,1);
	newstoneCount =	ClampFloat(newstoneCount ,0,1);
	newwoodCount  = ClampFloat(newwoodCount  ,0,1);
	m_gameStats.push_back(static_cast<double>(newfoodCount));
	m_gameStats.push_back(static_cast<double>(newstoneCount));
	m_gameStats.push_back(static_cast<double>(newwoodCount));
	
	float civilianTeam1		  = RangeMapFloat(static_cast<float>(m_map->m_gameStats.m_numOfCiviliansTeam1),      0.f, 5.f, 0.f, 1.f);
	float shortRangeArmyTeam1 = RangeMapFloat(static_cast<float>(m_map->m_gameStats.m_numOfShortRangeArmyTeam1), 0.f, 5.f, 0.f, 1.f);
	float longRangeArmyTeam1  = RangeMapFloat(static_cast<float>(m_map->m_gameStats.m_numOfLongRangeArmyTeam1),  0.f, 5.f, 0.f, 1.f);

	float civilianTeam2		  = RangeMapFloat(static_cast<float>(m_map->m_gameStats.m_numOfCiviliansTeam2), 0.f, 5.f, 0.f, 1.f);
	float shortRangeArmyTeam2 = RangeMapFloat(static_cast<float>(m_map->m_gameStats.m_numOfShortRangeArmyTeam2), 0.f, 5.f, 0.f, 1.f);
	float longRangeArmyTeam2  = RangeMapFloat(static_cast<float>(m_map->m_gameStats.m_numOfLongRangeArmyTeam2), 0.f, 5.f, 0.f, 1.f);

	civilianTeam1        = ClampFloat(civilianTeam1, 0, 1);
	shortRangeArmyTeam1  = ClampFloat(shortRangeArmyTeam1, 0, 1);
	longRangeArmyTeam1   = ClampFloat(longRangeArmyTeam1, 0, 1);
	civilianTeam2		 = ClampFloat(civilianTeam2, 0, 1);
	shortRangeArmyTeam2  = ClampFloat(shortRangeArmyTeam2, 0, 1);
	longRangeArmyTeam2   = ClampFloat(longRangeArmyTeam2, 0, 1);


	if(m_teamID == 1)
	{
		m_gameStats.push_back(static_cast<double>(m_map->m_gameStats.m_numOfArmySpawnerTeam1));
		m_gameStats.push_back(static_cast<double>(civilianTeam1));
		m_gameStats.push_back(static_cast<double>(shortRangeArmyTeam1));
		m_gameStats.push_back(static_cast<double>(longRangeArmyTeam1));
		m_gameStats.push_back(static_cast<double>(m_map->m_gameStats.m_numOfHousesTeam1));
	}
	else
	{
		m_gameStats.push_back(static_cast<double>(m_map->m_gameStats.m_numOfArmySpawnerTeam2));
		m_gameStats.push_back(static_cast<double>(civilianTeam2));
		m_gameStats.push_back(static_cast<double>(shortRangeArmyTeam2));
		m_gameStats.push_back(static_cast<double>(longRangeArmyTeam2));
		m_gameStats.push_back(static_cast<double>(m_map->m_gameStats.m_numOfHousesTeam2));
	}
	int health = 0;
	if(m_type == TOWN_CENTER)
	{
		health = static_cast<int>(RangeMapFloat(m_health, 0.f, 50.f, 0.f, 1.f));
	}
	else
	{
		health = static_cast<int>(RangeMapFloat(m_health, 0.f, 10.f, 0.f, 1.f));
	}
	m_gameStats.push_back(health);


	if (HasResource())
	{
		m_gameStats.push_back((static_cast<double>(1)));
	}
	else
	{
		m_gameStats.push_back((static_cast<double>(0)));
	}

/*
	IntVector2 myPosition = m_map->GetCordinates(GetPosition());
	double minimapValue    = m_map->GetMiniMapValueAtPositionFromEntityType(myPosition.x, myPosition.y,m_teamID,m_type);
	m_map->SetMiniMapValues(myPosition.x, myPosition.y, this);*/

	std::vector<double> entityMiniMapInput = GetMyMiniMap();
	m_neuralNet.FeedForward(entityMiniMapInput, m_gameStats);
	//m_map->SetMiniMapValues(myPosition.x, myPosition.y, static_cast<float>(minimapValue));
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
		CreateAndPushMoveTask(taskPosition);
		}
		break;
	case TASK_GATHER_RESOURCE_FOOD:
	{
		CreateAndPushGatherResourceTask(taskPosition,TASK_GATHER_RESOURCE_FOOD);
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
	case TASK_RETREAT:
	{
		if(m_intendedStrategy == RETREAT)
		{
			m_scoreBoard.m_bonusScore++;
		}
		CreateAndPushRetreatTask(taskPosition);
	}
	break;
	case TASK_EXPLORE:
	{
		if (m_intendedStrategy == EXPLORE)
		{
			m_scoreBoard.m_bonusScore++;
		}
		CreateAndPushExploreTask(taskPosition);
	}
	break;
	case TASK_PATROL:
	{
		if (m_intendedStrategy == PATROL)
		{
			m_scoreBoard.m_bonusScore++;
		}
		CreateAndPushPatrolTask(taskPosition);
	}
	break;
	case TASK_FOLLOW:
	{
		if (m_intendedStrategy == FOLLOW)
		{
			m_scoreBoard.m_bonusScore++;
		}
		CreateAndPushFollowTask(taskPosition);
	}
	break;
	case TASK_ATTACK:
	{
		if (m_intendedStrategy == ATTACK)
		{
			m_scoreBoard.m_bonusScore++;
		}
		CreateAndPushAttackTask(taskPosition);
	}
	break;
	case TASK_LONG_ATTACK:
	{
		IntVector2 currentPosition = m_map->GetCordinates(m_previousState.m_position);

		//double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
		//double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

		taskPosition = GetTaskPositonFromNNOutput(m_previousState.m_position, 4, 4);
		//m_map->CreateExplosions(m_map->GetMapPosition(taskPosition), Rgba::RED);
		CreateAndPushLongRangeAttackTask(taskPosition);
	}
		break;
	case TASK_SHORT_ATTACK:
	{
		IntVector2 currentPosition = m_map->GetCordinates(m_previousState.m_position);

		//double taskXPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 2).m_value;
		//double taskYPosition = m_neuralNet.m_outputs->m_neurons.at(m_taskTypeSupported.size() - 1).m_value;

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
	m_scoreBoard.UpdateScoreResourceFoodCollected(count);
	m_map->GetMyScoreBoard(this).UpdateScoreResourceFoodCollected(count);
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
	m_scoreBoard.UpdateScoreResourceStoneCollected(count);
	m_map->GetMyScoreBoard(this).UpdateScoreResourceStoneCollected(count);
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
	m_scoreBoard.UpdateScoreResourceWoodCollected(count);
	m_map->GetMyScoreBoard(this).UpdateScoreResourceWoodCollected(count);
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
	m_scoreBoard.UpdateScoreArmySpawnersBuilt(count);
	m_map->GetMyScoreBoard(this).UpdateScoreArmySpawnersBuilt(count);
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
	m_scoreBoard.UpdateScoreHousesBuilt(count);
	m_map->GetMyScoreBoard(this).UpdateScoreHousesBuilt(count);
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
	m_scoreBoard.UpdateScoreShortRangeArmySpawned(count);
	m_map->GetMyScoreBoard(this).UpdateScoreShortRangeArmySpawned(count);
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
	m_scoreBoard.UpdateScoreLongRangeArmySpawned(count);
	m_map->GetMyScoreBoard(this).UpdateScoreLongRangeArmySpawned(count);
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
	m_state.m_civilianSpawned += count;
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
			//oldPositionSensoryValue -= m_map->GetCellDistance(cords,m_map->GetCordinates(prevState.m_position)) / m_map->m_classAWarriors.size();
			continue;
		}
		if (m_type == SHORT_RANGE_ARMY && (((FavoredMoveStats)index) == FAVORED_MOVETO_TEAM1_ARMY_SHORT_RANGE) && m_teamID == 1)
		{
			//oldPositionSensoryValue -= m_map->GetCellDistance(cords, m_map->GetCordinates(prevState.m_position)) / m_map->m_classAWarriors.size();
			continue;
		}

		if (m_type == LONG_RANGE_ARMY && (((FavoredMoveStats)index) == FAVORED_MOVETO_TEAM1_ARMY_LONG_RANGE) && m_teamID == 1)
		{
			//oldPositionSensoryValue -= m_map->GetCellDistance(cords, m_map->GetCordinates(prevState.m_position)) / m_map->m_classBWarriors.size();
			continue;
		}
		if (m_type == LONG_RANGE_ARMY && (((FavoredMoveStats)index) == FAVORED_MOVETO_TEAM2_ARMY_LONG_RANGE) && m_teamID == 2)
		{
			//oldPositionSensoryValue -= m_map->GetCellDistance(cords, m_map->GetCordinates(prevState.m_position)) / m_map->m_classBWarriors.size();
			continue;
		}

		if (m_type == CIVILIAN && (((FavoredMoveStats)index) == FAVORED_MOVETO_TEAM1_CIVILIAN) && m_teamID == 1)
		{
			//oldPositionSensoryValue -= m_map->GetCellDistance(cords, m_map->GetCordinates(prevState.m_position)) / m_map->m_classBWarriors.size();
			continue;
		}
		if (m_type == CIVILIAN && (((FavoredMoveStats)index) == FAVORED_MOVETO_TEAM2_CIVILIAN) && m_teamID == 2)
		{
			//oldPositionSensoryValue -= m_map->GetCellDistance(cords, m_map->GetCordinates(prevState.m_position)) / m_map->m_classBWarriors.size();
			continue;
		}

		if(m_type == SHORT_RANGE_ARMY || m_type == LONG_RANGE_ARMY)
		{
			if(((FavoredMoveStats)index) == FAVORED_MOVETO_RESOURCE_FOOD || ((FavoredMoveStats)index) == FAVORED_MOVETO_RESOURCE_WOOD || ((FavoredMoveStats)index) == FAVORED_MOVETO_RESOURCE_STONE)
			{
				continue;
			}
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
	if(m_state.m_favoredMoveTaskCount.size() == 0)
	{
		return 0;
	}
	*max = -1;
	int retIndex = 0;
	std::vector<int> indices;
	for(int index = 0;index < m_state.m_favoredMoveTaskCount.size();index++)
	{
		if(m_state.m_favoredMoveTaskCount.at(index) > *max)
		{
			*max = m_state.m_favoredMoveTaskCount.at(index);
			retIndex = index;
		}
	}
	for (int index = 0; index < m_state.m_favoredMoveTaskCount.size(); index++)
	{
		if(*max == m_state.m_favoredMoveTaskCount.at(index))
		{
			indices.push_back(index);
		}
	}

	size_t randomIndex = static_cast<size_t>(GetRandomIntLessThan(static_cast<int>(indices.size())));
	if(randomIndex < indices.size())
	{
		return indices.at(randomIndex);
	}
	return static_cast<int>(m_state.m_favoredMoveTaskCount.at(0));
	//return retIndex;
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
	RenderTaskType();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/09
*@purpose : Renders the task currently carrying on
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::RenderTaskType()
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	TaskType taksType = GetMyCurrentTask();
	std::string taskTypeStr = Task::GetTaskTypeAsShortString(taksType);
	g_theRenderer->DrawTextOn3DPoint(Vector3(GetPosition(),0) + Vector3(0,g_radius + g_fontSize, 0), Vector3::RIGHT, Vector3::UP, taskTypeStr, g_fontSize, Rgba::YELLOW);

	delete textMaterial;
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
		//m_map->CreateExplosions(m_map->GetMapPosition(neighbour),Rgba::WHITE);
	}
	//m_map->CreateExplosions(m_map->GetMapPosition(neighbour),Rgba::YELLOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetDesiredOutputToMoveToNeighbour(Vector2 position)
{
	SetDesiredOutputForTask(TASK_MOVE, 1);
	SetDesiredOutputForTask(TASK_MOVEX, position.x);
	SetDesiredOutputForTask(TASK_MOVEY, position.y);
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
	UNUSED(cellDistance);
	double xPosition = GetRandomDoubleInRange(0, 1);
	double yPosition = GetRandomDoubleInRange(0, 1);
	SetDesiredOutputForTask(TASK_MOVEX, static_cast<float>(xPosition));
	SetDesiredOutputForTask(TASK_MOVEY, static_cast<float>(yPosition));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::SetRandomTaskInQueue()
{
	if (m_map->m_mapMode == MAP_MODE_TRAINING_NONE_PLAY_GREEN && m_teamID == 1)
	{
		return;
	}

	if (m_map->m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN)
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
	m_intendedStrategy = strategy;
	m_desiredStrategyValues.at(strategy)+= incrementValue;
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
	return "Data\\NN\\BestGame\\"        + ToString(m_teamID)+"\\"+GetEntityTypeAsString(m_type) +"_"+ ToString(m_teamID) + ".txt";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetLocalBestFilePath()
{
	return "Data\\NN\\"+m_map->m_folder +"\\"+ ToString(m_teamID)+"\\"+ GetEntityTypeAsString(m_type) +"_"+ ToString(m_teamID) + ".txt";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : returns best stat global file path
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Entity::GetGlobalBestStatFilePath()
{
	return "Data\\NN\\BestGame\\"     + ToString(m_teamID) +"\\BestStats\\" + GetEntityTypeAsString(m_type) + "_" + ToString(m_teamID) + "_STATS.txt";
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

	inputs.m_allyInAttackMode = 0;
	inputs.m_allyInExploreMode = 0;
	inputs.m_allyInFollowMode = 0;
	inputs.m_allyInPatrolMode = 0;
	inputs.m_allyInRetreatMode = 0;


	inputs.m_allyArmySpawnerCount     = myTownCenter->m_resourceStat.m_armySpawners;
	inputs.m_allyCiviliansCount       = myTownCenter->m_resourceStat.m_civilians;
	inputs.m_allyHouseCount           = myTownCenter->m_resourceStat.m_houses;
	inputs.m_allyLongRangeArmyCount   = myTownCenter->m_resourceStat.m_longRangeArmies;
	inputs.m_allyShortRangeArmyCount  = myTownCenter->m_resourceStat.m_shortRangeArmies;
	inputs.m_allyTownCenterHealth     = myTownCenter->m_health;
									  
	inputs.m_enemyArmySpawnerCount    = enemyTownCenter->m_resourceStat.m_armySpawners;
	inputs.m_enemyCiviliansCount	  = enemyTownCenter->m_resourceStat.m_civilians;
	inputs.m_enemyHouseCount		  = enemyTownCenter->m_resourceStat.m_houses;
	inputs.m_enemyLongRangeArmyCount  = enemyTownCenter->m_resourceStat.m_longRangeArmies;
	inputs.m_enemyShortRangeArmyCount = enemyTownCenter->m_resourceStat.m_shortRangeArmies;
	inputs.m_enemyTownCenterHealth	  = enemyTownCenter->m_health;

	inputs.m_health					  = m_health;
	inputs.m_resourceFoodCount		  = enemyTownCenter->m_resourceStat.m_food;
	inputs.m_resourceStoneCount		  = enemyTownCenter->m_resourceStat.m_stone;
	inputs.m_resourceWoodCount		  = enemyTownCenter->m_resourceStat.m_wood;
	inputs.m_resourceCarrying		  = 0;

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
																					   
	inputs.m_enemyArmySpawnerCount		= RangeMap(inputs.m_enemyArmySpawnerCount    ,0, 5,  0, 1);
	inputs.m_enemyCiviliansCount		= RangeMap(inputs.m_enemyCiviliansCount	     ,0, 10, 0, 1);
	inputs.m_enemyHouseCount			= RangeMap(inputs.m_enemyHouseCount		     ,0, 5,  0, 1);
	inputs.m_enemyLongRangeArmyCount	= RangeMap(inputs.m_enemyLongRangeArmyCount  ,0, 10, 0, 1);	
	inputs.m_enemyShortRangeArmyCount	= RangeMap(inputs.m_enemyShortRangeArmyCount ,0, 10, 0, 1);	
	inputs.m_enemyTownCenterHealth		= RangeMap(inputs.m_enemyTownCenterHealth	 ,0, 50, 0, 1);
																					   
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfTownCenterUnderAttack(std::vector<double> &NNInputVectors,Strategy strategy,double priority,NNInputs inputs)
{
	switch (strategy)
	{
	case RETREAT:
		NNInputVectors.at(NNInput_allyTownCenterHealth) =            inputs.m_allyTownCenterHealth           * priority;
		NNInputVectors.at(NNInput_allyTownCenterHealthLastFrame) =  -inputs.m_allyTownCenterHealthLastFrame  * priority;
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount) =      inputs.m_inRangeAllyTownCenterCount     * priority;
		priority--;
		break;
	case ATTACK:
		NNInputVectors.at(NNInput_allyTownCenterHealth)          =   inputs.m_allyTownCenterHealth          * priority;
		NNInputVectors.at(NNInput_allyTownCenterHealthLastFrame) =  -inputs.m_allyTownCenterHealthLastFrame * priority;
		NNInputVectors.at(NNInput_inRangeAllyTownCenterCount)    =   inputs.m_inRangeAllyTownCenterCount    * priority;
		break;
	}
	if(true)
	{
		//return;
	}
	TownCenter *townCenter = (TownCenter*)FindMyTownCenter();

	if(townCenter->IsUnderAttack())
	{
		if(inputs.m_inRangeAllyTownCenterCount == 0)
		{
			SetDesiredStrategyAsOutputForNN(RETREAT, 1);
			m_state.m_neuralNetPoints++;
			CopyDesiredStrategyValuesIntoDesiredNNOuputs();
			return true;
		}
		if(inputs.m_inRangeAllyTownCenterCount != 0)
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
/*DATE    : 2019/02/10
*@purpose : Checks if current entity is under attack and cannot overpower
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Entity::CheckAndSetStrategyIfEntityUnderAttack(std::vector<double> &NNInputVectors, Strategy strategy,double priority,NNInputs inputs)
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
/*DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/*DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/*DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/*DATE    : 2019/02/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
