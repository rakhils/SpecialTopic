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
	InitPersonality();
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
		return m_map->m_localBestShortRangeArmyScoreCardTeam1.m_totalScore;
	}
	if (m_teamID == 2)
	{
		return m_map->m_localBestShortRangeArmyScoreCardTeam2.m_totalScore;
	}
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/05/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::InitPersonality()
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
		m_strategies.push_back(CheckAndSetAttackStrategyIfEntityUnderAttack);
		m_strategies.push_back(CheckAndSetRetreatStrategyIfEnemiesOutweighs);
		m_strategies.push_back(CheckAndSetStrategyPatrol);
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Trains NN according to the expected output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::TrainNeuralNetwork(Task *task)
{
	Entity::TrainNeuralNetwork(task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the previous action
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateNNDefensive(Task *task, EntityState previousState, IntVector2 cords)
{
	if (!m_map->HasTrainingEnabled(this))
	{
		return;
	}

	ClearDesiredStrategyValues();
	NNInputs inputs  = GetMyNNInputsFromMap();

	Strategy strategy = ATTACK;
	std::vector<double> NNInputVectors;
	for (int inputIndex = 0; inputIndex < NNInput_InputMaxCount; inputIndex++)
	{
		NNInputVectors.push_back(0);
	}

	for(int index = 0; index < m_strategies.size();index++)
	{
		FunctionStrategy currentStrategy = m_strategies.at(index);
		bool result = (currentStrategy)(NNInputVectors, strategy, inputs);
		if(result)
		{
			SetDesiredStrategyValue(strategy, 1);
		}
	}

	m_state.m_neuralNetTrainingCount++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();

	Entity::EvaluateStrategies(task, previousState, cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/03/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::EvaluateStrategies(Task *task, EntityState previousState, IntVector2 cords)
{
	if (!m_map->HasTrainingEnabled(this))
	{
		return;
	}

	ClearDesiredStrategyValues();
	NNInputs inputs = GetMyNNInputsFromMap();

	Strategy strategy = ATTACK;
	std::vector<double> NNInputVectors;
	for (int inputIndex = 0; inputIndex < NNInput_InputMaxCount; inputIndex++)
	{
		NNInputVectors.push_back(0);
	}

	for (int index = 0; index < m_strategies.size(); index++)
	{
		FunctionStrategy currentStrategy = m_strategies.at(index);
		bool result = (currentStrategy)(NNInputVectors, strategy, inputs);
		if (result)
		{
			SetDesiredStrategyValue(strategy, 1);
		}
	}

	m_state.m_neuralNetTrainingCount++;
	CopyDesiredStrategyValuesIntoDesiredNNOuputs();

	Entity::EvaluateStrategies(task, previousState, cords);
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
	if(m_teamID == 1)
	{
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "SA", g_fontSize/2.f, Rgba::BLACK);
	}
	else
	{
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "SA", g_fontSize / 2.f, Rgba::WHITE);
	}
	std::string str = Entity::GetStrategyAsString(m_currentStr);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP + Vector3(0,20,0), str, g_fontSize / 2.f, Rgba::WHITE);
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
