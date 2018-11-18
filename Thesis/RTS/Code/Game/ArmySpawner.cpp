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
	m_taskQueue.push(new TaskIdle());
	InitNeuralNet();
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
