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
	/*Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "SA", g_fontSize / 2, GetTeamColor());
	delete textMaterial;*/
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
/*DATE    : 2018/09/30
*@purpose : Trains NN according to the expected output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::TrainNN(Task *task)
{
	Entity::TrainNN(task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the previous action
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::EvaluateNN(Task *task, EntityState previousState, IntVector2 cords)
{
	if (!g_isCurrentlyTraining)
	{
		return;
	}
	CopyDesiredOutputs();
	switch (task->m_taskType)
	{
	case TASK_MOVE:
		EvaluateMoveTask(previousState, cords);
		break;
	case TASK_LONG_ATTACK:
		cords = GetTaskPositonFromNNOutput(previousState.m_position, 4, 4);
		EvaluateLongAttackTask(previousState, cords);
		break;
	case TASK_IDLE:
		EvaluateIdleTask(previousState, cords);
		break;
	}
	std::vector<Entity*> neighbourEnemies = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 1);
	if (neighbourEnemies.size() > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0);
		SetDesiredOutputForTask(TASK_SHORT_ATTACK, 1);
		m_state.m_neuralNetPoints++;
		return;
	}
	neighbourEnemies.clear();
	neighbourEnemies = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 2);
	if (neighbourEnemies.size() > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0);
		SetDesiredOutputForTask(TASK_SHORT_ATTACK, 1);
		m_state.m_neuralNetPoints++;
		return;
	}
	bool found = false;
	std::vector<double> entityMiniMapInput = GetMyMiniMap();
	for (int index = 0; index < entityMiniMapInput.size(); index++)
	{
		if (entityMiniMapInput.at(index) == 1)
		{
			IntVector2 prevCords = m_map->GetCordinates(previousState.m_position);
			IntVector2 minimapMins = GetMiniMapMins(m_map->GetCordinates(previousState.m_position));
			int xPos = index % 8;
			int yPos = index / 8;

			float xPosition = RangeMapInt(xPos, 0, 8, 0, 1);
			float yPosition = RangeMapInt(yPos, 0, 8, 0, 1);
			xPosition += GetRandomFloatInRange(-0.15, 0.15);
			yPosition += GetRandomFloatInRange(-0.15, 0.15);
			SetDesiredOutputToMoveToNeighbour(Vector2(xPosition, yPosition));
			m_state.m_neuralNetPoints++;

			IntVector2 actualCords = minimapMins + IntVector2(xPos, yPos);
			Entity *entity = m_map->GetEntityFromPosition(actualCords);
			float currentTime = static_cast<float>(GetCurrentTimeSeconds());
			m_scoreBoard.m_bonusScore++;
			if (currentTime - m_lastAttackTime > 2)
			{
				//m_map->CreateExplosions(m_map->GetMapPosition(actualCords), Rgba::YELLOW);
				m_lastAttackTime = currentTime;
			}
			found = true;
		}
	}
	if (!found)
	{
		SetDesiredOutputToChooseRandomNeighbourLocation(8);
		m_state.m_neuralNetPoints++;
	}
	Entity::EvaluateNN(task, previousState, cords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::EvaluateMoveTask(EntityState previousState, IntVector2 cords)
{
	UNUSED(cords);

	IntVector2 prevCords = m_map->GetCordinates(previousState.m_position);
	if (m_map->GetCordinates(m_previousState.m_position) == cords)
	{
		SetDesiredOutputToMoveToNeighbour(previousState, 2);
		m_state.m_neuralNetPoints++;
		return;
	}

	std::vector<Entity*> entityList = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 1);
	/*if (entityList.size() > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0);
		m_state.m_neuralNetPoints++;
		return;
	}
	entityList.clear();*/
	//entityList = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 2);
	if (entityList.size() > 0)
	{
		SetDesiredOutputForTask(TASK_MOVE, 0);
		SetDesiredOutputForTask(TASK_LONG_ATTACK,1);
		m_state.m_neuralNetPoints++;
		return;
	}
	UpdateMostFavoredMoveTask(previousState, cords);
	float maxValue = -1;
	int index = GetMostFavoredMoveTask(&maxValue);

	if (previousState.m_favoredMoveTaskCount.at(index) < m_state.m_favoredMoveTaskCount.at(index))
	{
		if(m_map->GetEntityFromPosition(cords) != nullptr && m_map->GetEntityFromPosition(cords) != this)
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
	m_state.m_neuralNetPoints++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the short attack task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::EvaluateLongAttackTask(EntityState previousState, IntVector2 cords)
{
	if(m_state.m_enemiesAttacked > previousState.m_enemiesAttacked)
	{
		SetDesiredOutputForTask(TASK_LONG_ATTACK, 1);
		m_state.m_neuralNetPoints++;
		return;
	}

	std::vector<Entity*> entityList = m_map->GetAllEnemiesNearLocation(m_teamID, GetPosition(), 2);
	/*if (entityList.size() > 0)
	{
		SetDesiredOutputForTask(TASK_LONG_ATTACK, 1);
		m_state.m_neuralNetPoints++;
		Entity* enemy = m_map->GetEntityFromPosition(cords);
		if (enemy != nullptr && enemy->m_teamID != m_teamID)
		{
			return;
		}
		SetDesiredOutputToChooseRandomNeighbourLocation(2);
		return;
	}*/
	entityList.clear();
	entityList = m_map->GetAllEnemiesNearLocation(m_teamID, GetPosition(), 1);
	if (entityList.size() > 0)
	{
		SetDesiredOutputForTask(TASK_LONG_ATTACK, 1);
		m_state.m_neuralNetPoints++;
		Entity* enemy = m_map->GetEntityFromPosition(cords);
		if (enemy != nullptr && enemy->m_teamID != m_teamID && enemy->m_teamID != 0)
		{
			return;
		}
		SetDesiredOutputToChooseRandomNeighbourLocation(2);
		return;
	}
	m_state.m_neuralNetPoints++;
	SetDesiredOutputForTask(TASK_LONG_ATTACK, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Evaluates the idle task
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassBWarrior::EvaluateIdleTask(EntityState previousState, IntVector2 cords)
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
void ClassBWarrior::Render()
{
	if (m_health <= 0)
	{
		return;
	}
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "LA", g_fontSize / 2.f, GetTeamColor());
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

