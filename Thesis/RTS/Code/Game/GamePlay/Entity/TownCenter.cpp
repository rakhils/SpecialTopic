#include "Game/GamePlay/Entity/TownCenter.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskSpawnVillager.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
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
/*DATE    : 2018/09/30
*@purpose : Trains NN according to the expected output
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TownCenter::TrainNN()
{

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
