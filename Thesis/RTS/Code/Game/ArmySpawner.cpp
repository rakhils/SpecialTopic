#include "Game/ArmySpawner.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskSpawnClassAWarrior.hpp"
#include "Game/GamePlay/Task/TaskSpawnClassBWarrior.hpp"

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
/*DATE    : 2018/09/30
*@purpose : Trains NN according to the expected output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ArmySpawner::TrainNN()
{

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
