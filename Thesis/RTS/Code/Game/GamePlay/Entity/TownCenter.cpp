#include "Game/GamePlay/Entity/TownCenter.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskSpawnVillager.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
TownCenter::TownCenter()
{

}

TownCenter::TownCenter(Map *map,Vector2 position, int teamId) 
{
	m_map   = map;
	m_type	= TOWN_CENTER;
	m_teamID = teamId;
	SetPosition(position);
	m_taskTypeSupported.push_back(TASK_SPAWN_VILLAGER);
	m_taskTypeSupported.push_back(TASK_IDLE);
	m_health = 50;
	m_healthLastFrame = 50;
	InitNeuralNet();
	SetRandomTaskInQueue();
}

// DESTRUCTOR
TownCenter::~TownCenter()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Recevies all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	if (g_currentSelectedEntity == this)
	{
		if(InputSystem::GetInstance()->WasLButtonJustPressed())
		{
			Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
			mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
			if (g_unitStatHUDFirstButton.IsPointInside(mousePosition))
			{
				Task *task = new TaskSpawnVillager(m_map, this);
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
/*DATE    : 2018/08/21
*@purpose : Updates TownCenter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::Update(float deltaTime)
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
TaskType TownCenter::GetTaskFromNNOutput(double &max)
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TownCenter::GetGlobalBestScore()
{
	if (m_teamID == 1)
	{
		return g_globalMaxScoreTownCenter1;
	}
	if (m_teamID == 2)
	{
		return g_globalMaxScoreTownCenter2;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			return g_globalAttackMaxScoreTownCenter1;
		}
		if (m_teamID == 2)
		{
			return g_globalAttackMaxScoreTownCenter2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_globalDefenseMaxScoreTownCenter1;
		}
		if (m_teamID == 2)
		{
			return g_globalDefenseMaxScoreTownCenter2;
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
int TownCenter::GetLocalBestScore()
{
	if (m_teamID == 1)
	{
		return g_localMaxScoreTownCenter1;
	}
	if (m_teamID == 2)
	{
		return g_localMaxScoreTownCenter2;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			return g_localAttackMaxScoreTownCenter1;
		}
		if (m_teamID == 2)
		{
			return g_localAttackMaxScoreTownCenter2;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			return g_localDefenseMaxScoreTownCenter1;
		}
		if (m_teamID == 2)
		{
			return g_localDefenseMaxScoreTownCenter2;
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
void TownCenter::SetGlobalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_globalMaxScoreTownCenter1 = value;
	}
	if (m_teamID == 2)
	{
		g_globalMaxScoreTownCenter2 = value;
	}

	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_globalAttackMaxScoreTownCenter1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalAttackMaxScoreTownCenter2 = value;
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_globalDefenseMaxScoreTownCenter1 = value;
		}
		if (m_teamID == 2)
		{
			g_globalDefenseMaxScoreTownCenter2 = value;
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
void TownCenter::SetLocalBestScore(int value)
{
	if (m_teamID == 1)
	{
		g_localMaxScoreTownCenter1 = value;
	}
	if (m_teamID == 2)
	{
		g_localMaxScoreTownCenter2 = value;
	}
	/*switch (m_strategy)
	{
	case ATTACK:
		if (m_teamID == 1)
		{
			g_localAttackMaxScoreTownCenter1 = value;
		}
		if (m_teamID == 2)
		{
			g_localAttackMaxScoreTownCenter2 = value;  
		}
		break;
	case DEFENSE:
		if (m_teamID == 1)
		{
			g_localDefenseMaxScoreTownCenter1 = value;
		}
		if (m_teamID == 2)
		{
			g_localDefenseMaxScoreTownCenter2 = value;
		}
		break;
	default:
		break;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::EvaluateNN(Task *task, EntityState previousState, IntVector2 cords)
{
	if (!g_isCurrentlyTraining)
	{
		return;
	}
	CopyDesiredOutputs();
	switch (task->m_taskType)
	{
	case TASK_SPAWN_VILLAGER:
		EvaluateSpawnCivilianTask(previousState, cords);
		break;
	case TASK_IDLE:
		EvaluateIdleTask(previousState, cords);
		break;
	}
	Entity::EvaluateNN(task, previousState, cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::EvaluateSpawnCivilianTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	m_state.m_neuralNetPoints++;
	if (m_state.m_civilianSpawned == previousState.m_civilianSpawned)
	{
		m_desiredOuputs.at(0) = 0;
		return;
	}

	if(m_state.m_civilianSpawned > previousState.m_civilianSpawned)
	{
		m_desiredOuputs.at(0) = 1;
		// should check below values
	}

	if(m_teamID == 1 && m_map->m_gameStats.m_numOfCiviliansTeam1 > g_maxCivilianCount)
	{
		m_desiredOuputs.at(0) = 0;
		return;
	}
	if (m_teamID == 2 && m_map->m_gameStats.m_numOfCiviliansTeam2 > g_maxCivilianCount)
	{
		m_desiredOuputs.at(0) = 0;
		return;
	}
	m_desiredOuputs.at(0) = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::EvaluateIdleTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);
	m_state.m_neuralNetPoints++;
	m_desiredOuputs.at(1) = 1;
	if (m_teamID == 1 && m_map->m_gameStats.m_numOfCiviliansTeam1 > g_maxCivilianCount)
	{
		m_desiredOuputs.at(1) = 1;
		return;
	}
	if (m_teamID == 2 && m_map->m_gameStats.m_numOfCiviliansTeam2 > g_maxCivilianCount)
	{
		m_desiredOuputs.at(1) = 1;
		return;
	}
	if (m_map->HasEnoughResourceToSpawnCivilian(m_teamID))
	{
		m_desiredOuputs.at(1) = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render TownCenter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::Render()
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
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "T", g_fontSize, Rgba::BLACK);
	}
	else
	{
		g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "T", g_fontSize, Rgba::WHITE);
	}
	delete textMaterial;
}
