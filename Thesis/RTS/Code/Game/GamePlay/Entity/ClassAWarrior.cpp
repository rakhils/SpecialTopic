#include "Game/GamePlay/Entity/ClassAWarrior.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
#include "Game/GamePlay/Task/TaskShortRangeAttack.hpp"
// CONSTRUCTOR
ClassAWarrior::ClassAWarrior()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ClassAWarrior::ClassAWarrior(Map *map,Vector2 position, int teamID)
{
	m_map			= map;
	m_type			= SHORT_RANGE_ARMY;
	m_attackRange	= 1;
	m_taskTypeSupported.push_back(TASK_ATTACK);
	m_taskTypeSupported.push_back(TASK_EXPLORE);
	m_taskTypeSupported.push_back(TASK_FOLLOW);
	m_taskTypeSupported.push_back(TASK_PATROL);
	m_taskTypeSupported.push_back(TASK_RETREAT);
	m_taskTypeSupported.push_back(TASK_IDLE);

	SetPosition(position);
	SetTeam(teamID);
	InitNeuralNet();
	InitStates();
	SetRandomTaskInQueue();
}

// DESTRUCTOR
ClassAWarrior::~ClassAWarrior()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Process inputs for ClassAWarrior
*@param   : Delta Time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::ProcessInputs(float deltaTime)
{
	if (g_currentSelectedEntity == this)
	{
		Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
		mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

		if (InputSystem::GetInstance()->WasLButtonJustPressed())
		{
			int tileIndex = m_map->GetTileIndex(mousePosition);
			Entity *entity = m_map->GetEntityFromPosition(tileIndex);	
			if (entity == nullptr)
			{
				ClearTaskQueue();
				Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskMove(m_map, this, mapPosition);
				m_taskQueue.push(task);
			}
			else if (entity != nullptr && m_map->AreEnemies(entity,this))// && IsInRange(m_map->GetCordinates(entity->GetPosition())))
			{
				ClearTaskQueue();
				Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskShortRangeAttack(m_map,this, m_map->GetTileIndex(mapPosition));
				if(task->m_isValid)
				{
					m_taskQueue.push(task);
				}
				else
				{
					delete task;
				}
			}
		
		}
	}
	Entity::ProcessInputs(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Update cycle of ClassAWarrior
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/28
*@purpose : Retrieves the best task associated with neural net.
*@param   : Value (returnining) associated with best task
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType ClassAWarrior::GetTaskFromNNOutput(double &max)
{
	TaskType type = m_taskTypeSupported.at(0);
	for (int outputIndex = 0; outputIndex < m_taskTypeSupported.size(); outputIndex++)
	{
		if (m_neuralNet.m_outputs->m_neurons.at(outputIndex).m_value > max)
		{
			type = m_taskTypeSupported.at(outputIndex);
			max = m_neuralNet.m_outputs->m_neurons.at(outputIndex).m_value;
		}
	}
	return type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : Retrives global best score
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ClassAWarrior::GetGlobalBestScore()
{
	if (m_teamID == 1)
	{
		return g_globalMaxScoreShortRangeArmy1;
	}
	if (m_teamID == 2)
	{
		return g_globalMaxScoreShortRangeArmy2;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			return g_globalAttackMaxScoreShortRangeArmy1;
		}
		if (m_teamID == 2)
		{
			return g_globalAttackMaxScoreShortRangeArmy2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_globalDefenseMaxScoreShortRangeArmy1;
		}
		if (m_teamID == 2)
		{
			return g_globalDefenseMaxScoreShortRangeArmy2;
		}
		break;
	default:
		break;
	}*/
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ClassAWarrior::GetLocalBestScore()
{
	if (m_teamID == 1)
	{
		return g_localMaxScoreShortRangeArmy1;
	}
	if (m_teamID == 2)
	{
		return g_localMaxScoreShortRangeArmy2;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			return g_localAttackMaxScoreShortRangeArmy1;
		}
		if (m_teamID == 2)
		{
			return g_localAttackMaxScoreShortRangeArmy2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_localDefenseMaxScoreShortRangeArmy1;
		}
		if (m_teamID == 2)
		{
			return g_localDefenseMaxScoreShortRangeArmy2;
		}
		break;
	default:
		break;
	}*/
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::SetGlobalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_globalMaxScoreShortRangeArmy1 = value;
	}
	if (m_teamID == 2)
	{
		g_globalMaxScoreShortRangeArmy2 = value;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_globalAttackMaxScoreShortRangeArmy1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalAttackMaxScoreShortRangeArmy2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_globalDefenseMaxScoreShortRangeArmy1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalDefenseMaxScoreShortRangeArmy2 = value;
		}
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::SetLocalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_localMaxScoreShortRangeArmy1 = value;
	}
	if (m_teamID == 2)
	{
		g_localMaxScoreShortRangeArmy2 = value;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_localAttackMaxScoreShortRangeArmy1 = value;
		}
		if (m_teamID == 2)
		{
			g_localAttackMaxScoreShortRangeArmy2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_localDefenseMaxScoreShortRangeArmy1 = value; 
		}
		if (m_teamID == 2)
		{
			g_localDefenseMaxScoreShortRangeArmy2 = value;
		}
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Trains NN according to the expected output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::TrainNN(Task *task)
{
	Entity::TrainNN(task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the previous action
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateNN(Task *task, EntityState previousState, IntVector2 cords)
{
	if (!g_isCurrentlyTraining)
	{
		return;
	}

	CopyDesiredOutputs();
	ClearDesiredStrategyValues();
	NNInputs inputs  = GetMyNNInputs();


	Strategy strategy = ATTACK;
	double priority = MAX_NUM_STRATEGY * NNInput_InputMaxCount;


	std::vector<double> strategyValues;
	for(int index = 0;index < MAX_NUM_STRATEGY;index++)
	{
		strategyValues.push_back(0);
	}
	std::vector<double> NNInputVectors;
	for (int inputIndex = 0; inputIndex < NNInput_InputMaxCount; inputIndex++)
	{
		NNInputVectors.push_back(0);
	}
	if(CheckAndSetStrategyIfTownCenterUnderAttack (NNInputVectors, strategy, priority, inputs))
	{
		return;
	}
	priority -= NNInput_InputMaxCount;
	if(CheckAndSetStrategyIfEntityUnderAttack     (NNInputVectors, strategy, priority, inputs))
	{
		return;
	}
	priority -= NNInput_InputMaxCount;
	if(CheckAndSetStrategyIfEnemiesOutweighsAllies(NNInputVectors, strategy, priority, inputs))
	{
		return;
	}
	priority -= NNInput_InputMaxCount;
	if(CheckAndSetStrategyPatrol                  (NNInputVectors, strategy, priority, inputs))
	{
		return;
	}
	priority -= NNInput_InputMaxCount;
	if(CheckAndSetStrategyExplore                 (NNInputVectors, strategy, priority, inputs))
	{
		return;
	}
	priority -= NNInput_InputMaxCount;
	if(CheckAndSetStrategyAttack                  (NNInputVectors, strategy, priority, inputs))
	{
		return;
	}
	priority -= NNInput_InputMaxCount;
	/*for(int strategyIndex = 0;strategyIndex < MAX_NUM_STRATEGY;strategyIndex++)
	{
		std::vector<double> NNInputVectors;
		for(int inputIndex = 0;inputIndex < NNInput_InputMaxCount;inputIndex++)
		{
			NNInputVectors.push_back(0);
		}
		
		
		strategy = (Strategy)(strategy + 1);

	}*/
	

	SetDesiredStrategyAsOutputForNN(IDLE, 1);
	m_state.m_neuralNetPoints++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();

	Entity::EvaluateNN(task, previousState, cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateDefenseTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	std::vector<Entity*> m_entityListAlliesNear = m_map->GetAllAlliesNearLocation(m_teamID, GetPosition(), 4);
	if(m_entityListAlliesNear.size() > 0)
	{
		IntVector2 freeCords = m_map->GetFreeNeighbourTile(m_entityListAlliesNear.at(0)->GetPosition(),2);
		Task *taskMove = new TaskMove(m_map, this, m_map->GetMapPosition(freeCords));
		m_taskQueue.push(taskMove);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateAttackTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	IntVector2 freeCords = m_map->GetFreeNeighbourTile(GetPosition(),8);
	Task *taskMove = new TaskMove(m_map, this, m_map->GetMapPosition(freeCords));
	m_taskQueue.push(taskMove);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateMoveTask(EntityState previousState, IntVector2 cords)
{
	SetDesiredOutputToMoveToNeighbour(previousState,2);
	if(true)
	{
		return;
	}
	/*IntVector2 freeCords = m_map->GetFreeNeighbourTile(GetPosition(), 8);
	Task *taskMove = new TaskMove(m_map, this, m_map->GetMapPosition(freeCords));
	m_taskQueue.push(taskMove);

	if (true)
	{
		return;
	}*/
		std::vector<Entity*> m_entityListAlliesNear1 = m_map->GetAllAlliesNearLocation(m_teamID, previousState.m_position, 4);
		if (m_entityListAlliesNear1.size() > 0)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1);
			Vector2 desiredLocation = m_map->GetRelativePosition(GetCordinates(), m_entityListAlliesNear1.at(0)->GetCordinates(), 4);
			SetDesiredOutputForTask(TASK_MOVEX, desiredLocation.x);
			SetDesiredOutputForTask(TASK_MOVEY, desiredLocation.y);

			Vector2 relativeX = GetPosition() - Vector2(1,1)*4*g_radius*2 + (desiredLocation * 8 ) * g_radius*2;
			m_map->CreateExplosions(relativeX, Rgba::WHITE);
			m_state.m_neuralNetPoints++;
			return;
		}
		/*std::vector<Entity*> m_entityListAllies = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 1);
		if (m_entityListAllies.size() > 0)
		{
			SetDesiredOutputForTask(TASK_MOVE, 0);
			m_state.m_neuralNetPoints++;
			return;
		}
		UpdateMostFavoredMoveTask(previousState, cords);
		float maxValue = -1;
		int index = GetMostFavoredMoveTask(&maxValue);

		if (previousState.m_favoredMoveTaskCount.at(index) < m_state.m_favoredMoveTaskCount.at(index))
		{
			if (m_map->GetEntityFromPosition(cords) != nullptr && m_map->GetEntityFromPosition(cords) != this)
			{
				SetDesiredOutputToMoveToNeighbour(previousState, 2);
				m_state.m_neuralNetPoints++;
				return;
			}
			SetDesiredOutputForTask(TASK_MOVE, 1);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputToMoveToNeighbour(previousState, 2);
		m_state.m_neuralNetPoints++;*/
	

	if(true)
	{
		return;
	}
	IntVector2 prevCords = m_map->GetCordinates(previousState.m_position);
	if(m_map->GetCordinates(m_previousState.m_position) == cords)
	{
		SetDesiredOutputToMoveToNeighbour(previousState,2);
		m_state.m_neuralNetPoints++;
		return;
	}
	
	std::vector<Entity*> m_entityList = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 1);
	if (m_entityList.size() > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0);
		SetDesiredOutputForTask(TASK_SHORT_ATTACK,1);
		m_state.m_neuralNetPoints++;
		return;
	}
	UpdateMostFavoredMoveTask(previousState, cords);
	float maxValue = -1;
	int index = GetMostFavoredMoveTask(&maxValue);

	if(previousState.m_favoredMoveTaskCount.at(index) < m_state.m_favoredMoveTaskCount.at(index))
	{
		if (m_map->GetEntityFromPosition(cords) != nullptr && m_map->GetEntityFromPosition(cords) != this)
		{
			SetDesiredOutputToMoveToNeighbour(previousState, 2);
			m_state.m_neuralNetPoints++;
			return;
		}
		SetDesiredOutputForTask(TASK_MOVE, 1);
		m_state.m_neuralNetPoints++;
		return;
	}
	SetDesiredOutputToMoveToNeighbour(previousState,2);
	m_state.m_neuralNetPoints++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the short attack task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateShortAttackTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	std::vector<Entity*> m_entityList = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 1);
	if(m_entityList.size() > 0)
	{
		m_state.m_neuralNetPoints++;
		IntVector2 prevCords = m_map->GetCordinates(previousState.m_position);
		IntVector2 minimapMins = GetMiniMapMins(m_map->GetCordinates(previousState.m_position),2,2);

		IntVector2 pos = m_entityList.at(0)->GetCordinates();
		IntVector2 diff = pos - minimapMins;

		float xOffSet = RangeMapInt(diff.x, 0, 2, 0, 1);
		float yOffSet = RangeMapInt(diff.y, 0, 2, 0, 1);
		SetDesiredOutputForTask(TASK_SHORT_ATTACK, 1);
		SetDesiredOutputForTask(TASK_MOVEX,xOffSet);
		SetDesiredOutputForTask(TASK_MOVEY,yOffSet);
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_SHORT_ATTACK, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evalutas the idle task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateIdleTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	SetDesiredOutputForTask(TASK_IDLE, 0);
	m_state.m_neuralNetPoints++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::Render()
{
	if (m_health <= 0)
	{
		return;
	}
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "SA", g_fontSize/2.f, GetTeamColor());
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Checks if position in range
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClassAWarrior::IsInRange(int tileIndex)
{
	IntVector2 tileCords = m_map->GetCordinates(tileIndex);
	return IsInRange(tileCords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClassAWarrior::IsInRange(Vector2 position)
{
	int tileIndex = m_map->GetTileIndex(position);
	return IsInRange(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Checks if the cords is in range
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClassAWarrior::IsInRange(IntVector2 cords)
{
	int myTileIndex	   = m_map->GetTileIndex(GetPosition());
	IntVector2 myCords = m_map->GetCordinates(myTileIndex);

	int range = 1;
	// NEIGHBOURS 
	if (myCords.x + range == cords.x && myCords.y == cords.y)
	{
		return true;
	}
	if (myCords.x == cords.x && myCords.y + range == cords.y)
	{
		return true;
	}
	if (myCords.x - range == cords.x && myCords.y == cords.y)
	{
		return true;
	}
	if (myCords.x == cords.x && myCords.y - range == cords.y)
	{
		return true;
	}


	if (myCords.x + range == cords.x && myCords.y + range == cords.y)
	{
		return true;
	}
	if (myCords.x - range == cords.x && myCords.y + range == cords.y)
	{
		return true;
	}
	if (myCords.x - range == cords.x && myCords.y - range == cords.y)
	{
		return true;
	}
	if (myCords.x + range == cords.x && myCords.y - range == cords.y)
	{
		return true;
	}

	return false;
}
