#include "Game/GamePlay/Entity/ClassBWarrior.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskLongRangeAttack.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
// CONSTRUCTOR
ClassBWarrior::ClassBWarrior()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ClassBWarrior::ClassBWarrior(Map *map, Vector2 position, int teamID)
{
	m_map = map;
	m_type = LONG_RANGE_ARMY;
	m_attackRange = 2;
	SetPosition(position);
	SetTeam(teamID);
	m_taskTypeSupported.push_back(TASK_LONG_ATTACK);
	m_taskTypeSupported.push_back(TASK_MOVE);
	m_taskTypeSupported.push_back(TASK_IDLE);
	m_taskTypeSupported.push_back(TASK_MOVEX);
	m_taskTypeSupported.push_back(TASK_MOVEY);
	InitStates();
	InitNeuralNet();
	SetRandomTaskInQueue();
}

// DESTRUCTOR
ClassBWarrior::~ClassBWarrior()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::ProcessInputs(float deltaTime)
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
				Task *task = new TaskLongRangeAttack(m_map,this, m_map->GetTileIndex(mapPosition));
				m_taskQueue.push(task);
			}
		}
	}
	Entity::ProcessInputs(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType ClassBWarrior::GetTaskFromNNOutput(double &max)
{
	TaskType type = m_taskTypeSupported.at(0);
	int subtractTaskCount = 2;
	for (int outputIndex = 0; outputIndex < m_taskTypeSupported.size() - subtractTaskCount; outputIndex++)
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ClassBWarrior::GetGlobalBestScore()
{
	if (m_teamID == 1)
	{
		return g_globalMaxScoreLongRangeArmy1;
	}
	if (m_teamID == 2)
	{
		return g_globalMaxScoreLongRangeArmy2;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ClassBWarrior::GetLocalBestScore()
{
	if (m_teamID == 1)
	{
		return g_localMaxScoreLongRangeArmy1;
	}
	if (m_teamID == 2)
	{
		return g_localMaxScoreLongRangeArmy2;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::SetGlobalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_globalMaxScoreLongRangeArmy1 = value;
	}
	if (m_teamID == 2)
	{
		g_globalMaxScoreLongRangeArmy2 = value;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::SetLocalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_localMaxScoreLongRangeArmy1 = value;
	}
	if (m_teamID == 2)
	{
		g_localMaxScoreLongRangeArmy2 = value;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Trains NN according to the expected output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::TrainNeuralNetwork(Task *task)
{
	Entity::TrainNeuralNetwork(task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the previous action
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::EvaluateNN(Task *task, EntityState previousState, IntVector2 cords)
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
void ClassBWarrior::Render()
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
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "LA", g_fontSize / 2.f, Rgba::BLACK);
	}
	else
	{
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "LA", g_fontSize / 2.f, Rgba::WHITE);
	}
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Checks if position in range
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ClassBWarrior::IsInRange(int tileIndex)
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
bool ClassBWarrior::IsInRange(Vector2 position)
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
bool ClassBWarrior::IsInRange(IntVector2 cords)
{
	int myTileIndex = m_map->GetTileIndex(GetPosition());
	IntVector2 myCords = m_map->GetCordinates(myTileIndex);

	int range = 2;
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

