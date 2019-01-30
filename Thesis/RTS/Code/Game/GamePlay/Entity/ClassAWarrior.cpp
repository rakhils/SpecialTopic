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
	m_map		  = map;
	m_type		  = SHORT_RANGE_ARMY;
	SetPosition(position);
	SetTeam(teamID);
	m_taskTypeSupported.push_back(TASK_SHORT_ATTACK);
	m_taskTypeSupported.push_back(TASK_MOVE);
	m_taskTypeSupported.push_back(TASK_IDLE);
	m_taskTypeSupported.push_back(TASK_MOVEX);
	m_taskTypeSupported.push_back(TASK_MOVEY);
	InitNeuralNet();
	InitStates();
	SetRandomTaskInQueue();
}

// DESTRUCTOR
ClassAWarrior::~ClassAWarrior()
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
*@purpose : NIL
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
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TaskType ClassAWarrior::GetTaskFromNNOutput(double &max)
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
/*DATE    : 2019/01/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClassAWarrior::InitDefenseNNModel()
{
	
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
	/*Entity *targetPlaceEntity = m_map->GetEntityFromPosition(GetTaskPositonFromNNOutput());
	if (targetPlaceEntity != nullptr && targetPlaceEntity != this)
	{
		m_state.m_neuralNetPoints++;
		SetDesiredOutputToMoveToNeighbour(previousState,2);
		return;
	}*/
	if(previousState.m_health > m_state.m_health && m_state.m_health < 5)
	{
		SetDesiredOutputToChooseRandomNeighbourLocation(8);
		m_state.m_neuralNetPoints++;
		return;
	}
	switch (task->m_taskType)
	{
		case TASK_MOVE:
			EvaluateMoveTask(previousState, cords);
			break;
		case TASK_SHORT_ATTACK:
			cords = GetTaskPositonFromNNOutput(previousState.m_position, 2, 2);
			EvaluateShortAttackTask(previousState, cords);
			break;
		case TASK_IDLE:
			EvaluateIdleTask(previousState, cords);
			break;
	}

	std::vector<Entity*> neighbourEnemies = m_map->GetAllEnemiesNearLocation(m_teamID,previousState.m_position, 1);
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
void ClassAWarrior::EvaluateMoveTask(EntityState previousState, IntVector2 cords)
{
	if(true)
	{
		/*std::vector<Entity*> m_entityList = m_map->GetAllAlliesNearLocation(m_teamID, previousState.m_position, 1);
		if (m_entityList.size() > 0)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1);
			m_state.m_neuralNetPoints++;
			return;
		}*/
		std::vector<Entity*> m_entityListAlliesNear1 = m_map->GetAllAlliesNearLocation(m_teamID, previousState.m_position, 4);
		if (m_entityListAlliesNear1.size() > 0)
		{
			SetDesiredOutputForTask(TASK_MOVE, 1);
			Vector2 desiredLocation = m_map->GetRelativePosition(GetCordinates(), m_entityListAlliesNear1.at(0)->GetCordinates(), 4);
			SetDesiredOutputForTask(TASK_MOVEX, desiredLocation.x);
			SetDesiredOutputForTask(TASK_MOVEY, desiredLocation.y);
			m_state.m_neuralNetPoints++;
			return;
		}
		std::vector<Entity*> m_entityListAllies = m_map->GetAllEnemiesNearLocation(m_teamID, previousState.m_position, 1);
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
		m_state.m_neuralNetPoints++;
	}

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
