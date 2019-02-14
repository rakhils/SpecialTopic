#include "Game/ArmySpawner.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskSpawnClassAWarrior.hpp"
#include "Game/GamePlay/Task/TaskSpawnClassBWarrior.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"

// CONSTRUCTOR
ArmySpawner::ArmySpawner()
{

}

ArmySpawner::ArmySpawner(Map *map, Vector2 position, int teamID)
{
	m_type = ARMY_SPAWNER;
	m_map = map;
	SetPosition(position);
	SetTeam(teamID);
	m_taskTypeSupported.push_back(TASK_SPAWN_CLASSA_WARRIOR);
	m_taskTypeSupported.push_back(TASK_SPAWN_CLASSB_WARRIOR);
	m_taskTypeSupported.push_back(TASK_IDLE);
	InitNeuralNet();
	SetRandomTaskInQueue();
}

// DESTRUCTOR
ArmySpawner::~ArmySpawner()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::ProcessInput(float deltaTime)
{
	UNUSED(deltaTime);
	if (g_currentSelectedEntity == this)
	{
		if (InputSystem::GetInstance()->WasLButtonJustPressed())
		{
			Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
			mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

			if (g_unitStatHUDFirstButton.IsPointInside(mousePosition))
			{
				Task *task = new TaskSpawnClassAWarrior(m_map, this);
				if (task->CheckAndReduceResources())
				{
					m_taskQueue.push(task);
					return;
				}
				delete task;
			}
			
			if (g_unitStatHUDSecondButton.IsPointInside(mousePosition))
			{
				Task *task = new TaskSpawnClassBWarrior(m_map, this);
				if (task->CheckAndReduceResources())
				{
					m_taskQueue.push(task);
					return;
				}
				delete task;
			}
		}
	}
	Entity::ProcessInputs(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : Evaluates the current task based on states
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::EvaluateNN(Task *task, EntityState previousState, IntVector2 cords)
{
	CopyDesiredOutputs();
	switch (task->m_taskType)
	{
	case TASK_SPAWN_CLASSA_WARRIOR:
		EvaluateShortRangeArmySpawnTask(previousState, cords);
		break;
	case TASK_SPAWN_CLASSB_WARRIOR:
		EvaluateLongRangeArmySpawnTask(previousState, cords);
		break;
	case TASK_IDLE:
		EvaluateIdleTask(previousState, cords);
		break;
	}
	Entity::EvaluateNN(task,previousState,cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::EvaluateShortRangeArmySpawnTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	m_state.m_neuralNetPoints++;
	if (previousState.m_shortRangeArmySpawned == m_state.m_shortRangeArmySpawned)
	{
		m_desiredOuputs.at(0) = 0;
		return;
	}
	if (previousState.m_shortRangeArmySpawned < m_state.m_shortRangeArmySpawned)
	{
		if(m_teamID == 1)
		{
			if(m_map->m_gameStats.m_numOfShortRangeArmyTeam1 >= g_maxShortRangeArmyCount)
			{
				m_desiredOuputs.at(0) = 0;
			}
			else
			{
				m_desiredOuputs.at(0) = 1;
			}
		}
		if (m_teamID == 2)
		{
			if (m_map->m_gameStats.m_numOfShortRangeArmyTeam2 >= g_maxShortRangeArmyCount)
			{
				m_desiredOuputs.at(0) = 0;
			}
			else
			{
				m_desiredOuputs.at(0) = 1;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::EvaluateLongRangeArmySpawnTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	m_state.m_neuralNetPoints++;
	if (previousState.m_longRangeArmySpawned == m_state.m_longRangeArmySpawned)
	{
		m_desiredOuputs.at(1) = 0;
		return;
	}
	if (previousState.m_longRangeArmySpawned < m_state.m_longRangeArmySpawned)
	{
		if (m_teamID == 1)
		{
			if (m_map->m_gameStats.m_numOfLongRangeArmyTeam1 >= g_maxLongRangeArmyCount)
			{
				m_desiredOuputs.at(1) = 0;
			}
			else
			{
				m_desiredOuputs.at(1) = 1;
			}
		}
		if (m_teamID == 2)
		{
			if (m_map->m_gameStats.m_numOfLongRangeArmyTeam2 >= g_maxLongRangeArmyCount)
			{
				m_desiredOuputs.at(1) = 0;
			}
			else
			{
				m_desiredOuputs.at(1) = 1;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::EvaluateIdleTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	m_state.m_neuralNetPoints++;
	if (m_teamID == 1 && m_map->m_gameStats.m_numOfLongRangeArmyTeam1 >= g_maxLongRangeArmyCount && m_map->m_gameStats.m_numOfShortRangeArmyTeam1 >= g_maxShortRangeArmyCount)
	{
		m_desiredOuputs.at(2) = 1;
		return;
	}
	if (m_teamID == 2 && m_map->m_gameStats.m_numOfLongRangeArmyTeam2 >= g_maxLongRangeArmyCount && m_map->m_gameStats.m_numOfShortRangeArmyTeam2 >= g_maxShortRangeArmyCount)
	{
		m_desiredOuputs.at(2) = 1;
		return;
	}

	if(m_map->HasEnoughResourceToSpawnLongRangeArmy(m_teamID) || m_map->HasEnoughResourceToSpawnShortRangeArmy(m_teamID))
	{
		m_desiredOuputs.at(2) = 0;
		return;
	}
	
	m_desiredOuputs.at(2) = 1;
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ArmySpawner::GetGlobalBestScore()
{
	if (m_teamID == 1)
	{
		return g_globalMaxScoreArmySpawnerTeam1;
	}
	if (m_teamID == 2)
	{
		return g_globalMaxScoreArmySpawnerTeam2;
	}
	return 0;
/*switch (m_strategy)
	{
	case ATTACK:
		if(m_teamID == 1)
		{
			return g_globalAttackMaxScoreArmySpawnerTeam1;
		}
		if (m_teamID == 2)
		{
			return g_globalAttackMaxScoreArmySpawnerTeam2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_globalDefenseMaxScoreArmySpawnerTeam1;
		}
		if (m_teamID == 2)
		{
			return g_globalDefenseMaxScoreArmySpawnerTeam2;
		}
		break;
	default:
		break;
	}
	return 0;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ArmySpawner::GetLocalBestScore()
{
	if (m_teamID == 1)
	{
		return g_localMaxScoreArmySpawnerTeam1;
	}
	if (m_teamID == 2)
	{
		return g_localMaxScoreArmySpawnerTeam2;
	}
	return 0;
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			return g_localAttackMaxScoreArmySpawnerTeam1;
		}
		if (m_teamID == 2)
		{
			return g_localAttackMaxScoreArmySpawnerTeam2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_localDefenseMaxScoreArmySpawnerTeam1;
		}
		if (m_teamID == 2)
		{
			return g_localDefenseMaxScoreArmySpawnerTeam2;
		}
		break;
	default:
		break;
	}
	return 0;*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::SetGlobalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_globalMaxScoreArmySpawnerTeam1 = value;
	}
	if (m_teamID == 2)
	{
		g_globalMaxScoreArmySpawnerTeam2 = value;
	}

/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_globalAttackMaxScoreArmySpawnerTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalAttackMaxScoreArmySpawnerTeam2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_globalDefenseMaxScoreArmySpawnerTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalDefenseMaxScoreArmySpawnerTeam2 = value;
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
void ArmySpawner::SetLocalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_localMaxScoreArmySpawnerTeam1 = value;
	}
	if (m_teamID == 2)
	{
		g_localMaxScoreArmySpawnerTeam2 = value;
	}

	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_localAttackMaxScoreArmySpawnerTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_localAttackMaxScoreArmySpawnerTeam2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_localDefenseMaxScoreArmySpawnerTeam1 = value;
		}
		if (m_teamID == 2)
		{
			g_localDefenseMaxScoreArmySpawnerTeam2 = value;
		}
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType ArmySpawner::GetTaskFromNNOutput(double &max)
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
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::Update(float deltaTime)
{
	ProcessInput(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Renders army spawner
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::Render()
{
	if(m_health <= 0)
	{
		return;
	}
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "A", g_fontSize, Rgba::WHITE);
	delete textMaterial;
}
