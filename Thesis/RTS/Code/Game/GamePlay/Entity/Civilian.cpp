#include "Game/GamePlay/Entity/Civilian.hpp"

#include "Engine/Renderer/Materials/Material.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Task/TaskGatherResource.hpp"
#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskBuildArmySpawner.hpp"
#include "Game/GamePlay/Task/TaskBuildHouse.hpp"
#include "Game/GamePlay/Task/TaskDropResource.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
Civilian::Civilian()
{

}

Civilian::Civilian(Map *map,Vector2 position, int teamID)
{
	m_map		  = map;
	m_teamID	  = teamID;
	SetPosition(position);
	m_taskTypeSupported.push_back(TASK_GATHER_RESOURCE);
	m_taskTypeSupported.push_back(TASK_DROP_RESOURCE);
	m_taskTypeSupported.push_back(TASK_BUILD_ARMY_SPAWNER);
	m_taskTypeSupported.push_back(TASK_BUILD_HOUSE);
	m_taskTypeSupported.push_back(TASK_MOVE);
	m_taskTypeSupported.push_back(TASK_IDLE);

	InitNeuralNet();
	m_taskQueue.push(new TaskIdle());
}

// DESTRUCTOR
Civilian::~Civilian()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::ProcessInputs(float deltaTime)
{
	if(g_currentSelectedEntity == this)
	{
		Vector2 mousePosition   = InputSystem::GetInstance()->GetMouseClientPosition();
		mousePosition.y			= Windows::GetInstance()->GetDimensions().y - mousePosition.y;

		if (InputSystem::GetInstance()->WasLButtonJustPressed() && m_map->IsPositionInsideMap(mousePosition))
		{
			int tileIndex = m_map->GetTileIndex(mousePosition);
			Entity *entity = m_map->GetEntityFromPosition(tileIndex);
			if (m_pendingTask == BUILD_ARMYCENTER)
			{
				Task *task = new TaskBuildArmySpawner(m_map, this, mousePosition);
				if(task->CheckAndReduceResources())
				{
					m_taskQueue.push(task);
					m_pendingTask = NOTASK;
					return;
				}
				delete task;
			}
			if (m_pendingTask == BUILD_HOUSE)
			{
				Task *task = new TaskBuildHouse(m_map, this, mousePosition);
				if(task->CheckAndReduceResources())
				{
					m_taskQueue.push(task);
					m_pendingTask = NOTASK;
					return;
				}
				delete task;
			}
			if(entity == nullptr)
			{
				EmptyTaskQueue();
				Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskMove(m_map,this,mapPosition);
				m_taskQueue.push(task);
			}
			else if(entity != nullptr && (entity->m_type == RESOURCE_FOOD || entity->m_type == RESOURCE_STONE|| entity->m_type == RESOURCE_WOOD))
			{
				EmptyTaskQueue();
				//Vector2 mapPosition = m_map->GetMapPosition(tileIndex);
				Task *task = new TaskGatherResource(this, entity, FindMyTownCenter());
				m_taskQueue.push(task);
			}
			else if(entity != nullptr && (entity->m_type == TOWN_CENTER))
			{
				EmptyTaskQueue();
				Task *task = new TaskDropResource(this,FindMyTownCenter());
				m_taskQueue.push(task);
			}
		}
		if (g_unitStatHUDFirstButton.IsPointInside(mousePosition))
		{
			m_pendingTask = BUILD_ARMYCENTER;
		}
		if (g_unitStatHUDSecondButton.IsPointInside(mousePosition))
		{
			m_pendingTask = BUILD_HOUSE;
		}
	}
	Entity::ProcessInputs(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates civilian
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	Entity::Update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : trains NeuralNet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::TrainNN()
{
	std::vector<float> m_outputs;
	IntVector2 townCenterPosition = m_map->GetCordinates(FindMyTownCenter()->GetPosition());
	for(int outputIndex = 0;outputIndex < m_taskTypeSupported.size();outputIndex++)
	{
		m_outputs.push_back(0.f);
	}
	m_outputs.push_back(m_neuralNet.GetSigmoidValue(townCenterPosition.x));
	m_outputs.push_back(m_neuralNet.GetSigmoidValue(townCenterPosition.y));

	std::vector<Entity*> entityList = GetAllEntitiesNearMe(1);
	bool isResourceNearMe = false;
	bool isTownCenterNearMe = false;
	/*for (size_t entityIndex = 0; entityIndex < entityList.size(); entityIndex++)
	{
		Entity *entity = entityList.at(entityIndex);
		if(entity->m_type == RESOURCE_FOOD || entity->m_type == RESOURCE_STONE || entity->m_type == RESOURCE_WOOD)
		{
			int resourceGatherIndex = GetIndexOfOutputTask(TASK_GATHER_RESOURCE);
			m_outputs.at(resourceGatherIndex) = 1.f;
			isResourceNearMe = true;
		}
		if (entity->m_type == TOWN_CENTER)
		{
			int resourceGatherIndex = GetIndexOfOutputTask(TASK_DROP_RESOURCE);
			m_outputs.at(resourceGatherIndex) = 1.f;
			isTownCenterNearMe = true;
		}
	}*/
	//if(!isResourceNearMe && !isTownCenterNearMe)
	{
		int resourceGatherIndex = GetIndexOfOutputTask(TASK_MOVE);
		m_outputs.at(resourceGatherIndex) = 1.f;
	}
	m_neuralNet.DoBackPropogation(m_outputs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::Render()
{
	if (m_health <= 0)
	{
		return;
	}
	Entity::Render();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	g_theRenderer->DrawTextOn3DPoint(GetPosition(), Vector3::RIGHT, Vector3::UP, "C", g_fontSize, GetTeamColor());
	
	
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Start a task of gather resource
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Civilian::GatherResource(Vector2 resourcePosition)
{
	/*Task *task = new TaskGatherResource(this);
	((TaskGatherResource*)task)->SetResourcePosition(resourcePosition);
	((TaskGatherResource*)task)->SetStoragePosition(m_map->m_townCenters.at(0)->GetPosition());
	m_taskQueue.push(task);*/
}
