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
	m_taskQueue.push(new TaskIdle());
	m_health = 50;
	InitNeuralNet();
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
/*DATE    : 2018/11/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::EvaluateNN(Task *task, EntityState previousState, IntVector2 cords)
{
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
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "T", g_fontSize, Rgba::WHITE);
	delete textMaterial;
}
