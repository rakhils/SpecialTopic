#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"

#include "Engine/Core/Windows.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/System/System.hpp"
Map::Map()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Map::~Map()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Init the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Initialize()
{
	InitCamera();
	switch (m_mapMode)
	{
	case MAP_MODE_TRAINING_CIVILIAN_GATHER_FOOD:
		InitTrainingForCivilianGatherFood();
		break;
	case MAP_MODE_TRAINING_CIVILIAN_GATHER_ALL_RESOURCES:
		InitTrainingForCivilianGatherAllResources();
		break;
	case MAP_MODE_TRAINING_CIVILIAN_BUILD:
		InitTrainingForCivilianBuildAll();
		break;
	case MAP_MODE_TRAINING_CIVILIAN:
		InitTrainingForCivilian();
		break;
	case MAP_MODE_TRAINING_TOWNCENTER:
		InitTrainingForTownCenter();
		break;
	case MAP_MODE_TRAINING_SHORTRANGE_ARMY:
		InitTrainingForShortRangeArmy();
		break;
	case MAP_MODE_TRAINING_LONGRANGE_ARMY:
		InitTrainingForLongRangeArmy();
		break;
	case MAP_MODE_TRAINING_ARMYSPAWNER:
		InitTrainingForArmySpawner();
		break;
	case MAP_MODE_TRAINING_NONE:
	default:
		InitNonTrainingMode();
		break;
	}
	InitMiniMap();
	InitCellSensoryValues();
	CreateDirectoryForNN();
	InitAndStoreBestScoreFromFile();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Initialize sensory values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCellSensoryValues()
{
	for(int index = 0;index < m_maxWidth*m_maxHeight;index++)
	{
		CellSensoryValues cellValue;
		cellValue.m_entityNearness.at(FAVORED_MOVETO_RESOURCE_FOOD)  = GetHeatMapDistanceFromEntity(GetCordinates(index), RESOURCE_FOOD, 0);
		cellValue.m_entityNearness.at(FAVORED_MOVETO_RESOURCE_STONE) = GetHeatMapDistanceFromEntity(GetCordinates(index), RESOURCE_STONE, 0);
		cellValue.m_entityNearness.at(FAVORED_MOVETO_RESOURCE_WOOD)  = GetHeatMapDistanceFromEntity(GetCordinates(index), RESOURCE_WOOD, 0);

		cellValue.m_entityNearness.at(FAVORED_MOVETO_TEAM1_TOWNCENTER) = GetHeatMapDistanceFromEntity(GetCordinates(index), TOWN_CENTER, 1);
		cellValue.m_entityNearness.at(FAVORED_MOVETO_TEAM2_TOWNCENTER) = GetHeatMapDistanceFromEntity(GetCordinates(index), TOWN_CENTER, 2);
		m_cellSensoryValues.push_back(cellValue);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : Creates directory to store all results of game
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateDirectoryForNN()
{
	time_t rawtime;
	char buffer[80];
	time(&rawtime);

	struct tm info;
	localtime_s(&info, &rawtime);
	strftime(buffer, 50, "%Y-%m-%d  %H'%M'%S", &info);

	System::CreateDirectoryFromPath(("Data\\NN\\" + std::string(buffer)).c_str());
	System::CreateDirectoryFromPath(("Data\\NN\\" + std::string(buffer) + "\\1\\").c_str());
	System::CreateDirectoryFromPath(("Data\\NN\\" + std::string(buffer) + "\\2\\").c_str());
	System::CreateDirectoryFromPath(("Data\\NN\\" + std::string(buffer) + "\\1\\BestStats\\").c_str());
	System::CreateDirectoryFromPath(("Data\\NN\\" + std::string(buffer) + "\\2\\BestStats\\").c_str());

	m_folder = std::string(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : reads best score from file and store
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitAndStoreBestScoreFromFile()
{
	std::string fileContentTeam1    = GetFileContentAsString("Data\\NN\\BestGame\\1\\BestStats\\TEAM_1_STATS.txt");
	std::string fileContentTeam2    = GetFileContentAsString("Data\\NN\\BestGame\\2\\BestStats\\TEAM_2_STATS.txt");
	std::string armySpawnerTeam1    = GetFileContentAsString("Data\\NN\\BestGame\\1\\BestStats\\ARMY_SPAWNER_1_STATS.txt");
	std::string armySpawnerTeam2    = GetFileContentAsString("Data\\NN\\BestGame\\2\\BestStats\\ARMY_SPAWNER_2_STATS.txt");
	std::string civilianTeam1       = GetFileContentAsString("Data\\NN\\BestGame\\1\\BestStats\\CIVILIAN_1_STATS.txt");
	std::string civilianTeam2       = GetFileContentAsString("Data\\NN\\BestGame\\2\\BestStats\\CIVILIAN_2_STATS.txt");
	std::string shortRangeArmyTeam1 = GetFileContentAsString("Data\\NN\\BestGame\\1\\BestStats\\SHORT_RANGE_ARMY_1_STATS.txt");
	std::string shortRangeArmyTeam2 = GetFileContentAsString("Data\\NN\\BestGame\\2\\BestStats\\SHORT_RANGE_ARMY_2_STATS.txt");
	std::string longRangeArmyTeam1  = GetFileContentAsString("Data\\NN\\BestGame\\1\\BestStats\\LONG_RANGE_ARMY_1_STATS.txt");
	std::string longRangeArmyTeam2  = GetFileContentAsString("Data\\NN\\BestGame\\2\\BestStats\\LONG_RANGE_ARMY_2_STATS.txt");
	std::string townCenterTeam1     = GetFileContentAsString("Data\\NN\\BestGame\\1\\BestStats\\TOWN_CENTER_1_STATS.txt");
	std::string townCenterTeam2     = GetFileContentAsString("Data\\NN\\BestGame\\2\\BestStats\\TOWN_CENTER_2_STATS.txt");
	
	ToInt(fileContentTeam1.substr(0,	fileContentTeam1.find('\n')),       &g_globalMaxScoreTeam1);
	ToInt(fileContentTeam2.substr(0,	fileContentTeam2.find('\n')),       &g_globalMaxScoreTeam2);
	ToInt(armySpawnerTeam1.substr(0,	armySpawnerTeam1.find('\n')),		&g_globalMaxScoreArmySpawnerTeam1);
	ToInt(armySpawnerTeam2.substr(0,	armySpawnerTeam2.find('\n')),		&g_globalMaxScoreArmySpawnerTeam2);
	ToInt(civilianTeam1.substr(0,		civilianTeam1.find('\n')),			&g_globalMaxScoreCivilianTeam1);
	ToInt(civilianTeam2.substr(0,		civilianTeam2.find('\n')),			&g_globalMaxScoreCivilianTeam2);
	ToInt(shortRangeArmyTeam1.substr(0, shortRangeArmyTeam1.find('\n')),	&g_globalMaxScoreShortRangeArmy1);
	ToInt(shortRangeArmyTeam2.substr(0, shortRangeArmyTeam2.find('\n')),	&g_globalMaxScoreShortRangeArmy2);
	ToInt(longRangeArmyTeam1.substr(0,  longRangeArmyTeam1.find('\n')),		&g_globalMaxScoreLongRangeArmy1);
	ToInt(longRangeArmyTeam2.substr(0,  longRangeArmyTeam2.find('\n')),		&g_globalMaxScoreLongRangeArmy2);
	ToInt(townCenterTeam1.substr(0,		townCenterTeam1.find('\n')),		&g_globalMaxScoreTownCenter1);
	ToInt(townCenterTeam2.substr(0,		townCenterTeam2.find('\n')),		&g_globalMaxScoreTownCenter2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : Calculate and retrieves heatmap distance from enity of type
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::GetHeatMapDistanceFromEntity(IntVector2 cellposition, EntityType type, int teamID)
{
	float avgCellDistance = 0.f;
	int   entityCount = 0;
	if(type == RESOURCE_FOOD || type == RESOURCE_STONE || type == RESOURCE_WOOD)
	{
		for(int resourceIndex = 0;resourceIndex < m_resources.size();resourceIndex++)
		{
			if(m_resources.at(resourceIndex)->m_type == type)
			{
				int cellDistance = GetCellDistance(m_resources.at(resourceIndex)->GetCordinates(), cellposition);
				entityCount++;
				if(cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if(entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}
	if(type == TOWN_CENTER)
	{
		for (int entityIndex = 0; entityIndex < m_townCenters.size(); entityIndex++)
		{
			Entity *entity = m_townCenters.at(entityIndex);
			if (entity->m_type == type && entity->m_teamID == teamID)
			{
				int cellDistance = GetCellDistance(entity->GetCordinates(), cellposition);
				entityCount++;
				if (cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if (entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}

	if (type == CIVILIAN)
	{
		for (int entityIndex = 0; entityIndex < m_civilians.size(); entityIndex++)
		{
			Entity *entity = m_civilians.at(entityIndex);
			if (entity->m_type == type && entity->m_teamID == teamID)
			{
				int cellDistance = GetCellDistance(entity->GetCordinates(), cellposition);
				entityCount++;
				if (cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if (entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}
	if(type == ARMY_SPAWNER)
	{
		for (int entityIndex = 0; entityIndex < m_armySpawners.size(); entityIndex++)
		{
			Entity *entity = m_armySpawners.at(entityIndex);
			if (entity->m_type == type && entity->m_teamID == teamID)
			{
				int cellDistance = GetCellDistance(entity->GetCordinates(), cellposition);
				entityCount++;
				if (cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if (entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}
	if (type == SHORT_RANGE_ARMY)
	{
		for (int entityIndex = 0; entityIndex < m_classAWarriors.size(); entityIndex++)
		{
			Entity *entity = m_classAWarriors.at(entityIndex);
			if (entity->m_type == type && entity->m_teamID == teamID)
			{
				int cellDistance = GetCellDistance(entity->GetCordinates(), cellposition);
				entityCount++;
				if (cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if (entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}
	if (type == LONG_RANGE_ARMY)
	{
		for (int entityIndex = 0; entityIndex < m_classBWarriors.size(); entityIndex++)
		{
			Entity *entity = m_classBWarriors.at(entityIndex);
			if (entity->m_type == type && entity->m_teamID == teamID)
			{
				int cellDistance = GetCellDistance(entity->GetCordinates(), cellposition);
				entityCount++;
				if (cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if (entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}
	if (type == HOUSE)
	{
		for (int entityIndex = 0; entityIndex < m_houses.size(); entityIndex++)
		{
			Entity *entity = m_houses.at(entityIndex);
			if (entity->m_type == type && entity->m_teamID == teamID)
			{
				int cellDistance = GetCellDistance(entity->GetCordinates(), cellposition);
				entityCount++;
				if (cellDistance == 1 || cellDistance == 2)
				{
					return static_cast<float>(cellDistance);
				}
				avgCellDistance += cellDistance;
			}
		}
		if (entityCount > 0)
		{
			return 2 + avgCellDistance / entityCount;
		}
		return 0;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Inits camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_camera   = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Sets the map type
*@param   : Map type
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SetMapType(MapMode type)
{
	m_mapMode = type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForCivilianGatherFood()
{
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_maxWidth  = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;
	g_hiddenLayerCount = 50;

	CreateTownCenter(GetMapPosition(6), 1);
	CreateResources(GetMapPosition(10),  RESOURCE_FOOD);

	CreateTownCenter(GetMapPosition(61), 2);
	CreateCivilian(GetMapPosition(42), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Inits training of civilian with all resources
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForCivilianGatherAllResources()
{
	m_maxWidth  = 8; //g_mapMaxWidth;
	m_maxHeight = 8;// g_mapMaxHeight;
	g_hiddenLayerCount = 50;
	m_entitiesHavingTraning.push_back(CIVILIAN);

	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(9), RESOURCE_FOOD);
	CreateResources(GetMapPosition(57), RESOURCE_STONE);
	CreateResources(GetMapPosition(62), RESOURCE_WOOD);


	CreateTownCenter(GetMapPosition(35), 2);
	CreateCivilian(GetMapPosition(17), 2);


	/*m_maxWidth  = 8; //g_mapMaxWidth;
	m_maxHeight = 8;// g_mapMaxHeight;

	CreateTownCenter(GetMapPosition(8), 1);
	CreateResources(GetMapPosition(1), RESOURCE_STONE);
	CreateResources(GetMapPosition(57), RESOURCE_FOOD);
	CreateResources(GetMapPosition(35), RESOURCE_WOOD);


	CreateTownCenter(GetMapPosition(62), 2);
	CreateCivilian(GetMapPosition(34), 2);*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains civilian in building House,ArmySpawner,
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForCivilianBuildAll()
{
	m_maxWidth = 8;
	m_maxHeight = 8;
	m_entitiesHavingTraning.push_back(CIVILIAN);

	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(9), RESOURCE_FOOD);
	CreateResources(GetMapPosition(57), RESOURCE_STONE);
	CreateResources(GetMapPosition(62), RESOURCE_WOOD);


	CreateTownCenter(GetMapPosition(61), 2);
	CreateCivilian(GetMapPosition(42), 2);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains civilan for all tasks
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForCivilian()
{
	m_maxWidth = 8;
	m_maxHeight = 8;
	m_entitiesHavingTraning.push_back(CIVILIAN);

	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(9), RESOURCE_FOOD);
	CreateResources(GetMapPosition(57), RESOURCE_STONE);
	CreateResources(GetMapPosition(62), RESOURCE_WOOD);


	CreateTownCenter(GetMapPosition(34), 2);
	CreateCivilian(GetMapPosition(33), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains town center to spawn villagers
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForTownCenter()
{
	m_maxWidth = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;

	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(40), RESOURCE_FOOD);

	CreateTownCenter(GetMapPosition(61), 2);
	CreateCivilian(GetMapPosition(42), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains short range army to attack and move
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForShortRangeArmy()
{
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);

	m_maxWidth = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;
	g_hiddenLayerCount = 50;

	CreateTownCenter(GetMapPosition(57), 1);
	CreateResources(GetMapPosition(40), RESOURCE_FOOD);

	CreateTownCenter(GetMapPosition(62), 2);
	CreateCivilian(GetMapPosition(14), 1);
	CreateClassAWarrior(GetMapPosition(20),2);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains long range army to attack and move
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForLongRangeArmy()
{
	m_maxWidth = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;

	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(40), RESOURCE_FOOD);

	CreateTownCenter(GetMapPosition(61), 2);
	CreateCivilian(GetMapPosition(42), 2);
	CreateClassBWarrior(GetMapPosition(32), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains army spawner to spawn long range and short range armies
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForArmySpawner()
{
	m_maxWidth = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;

	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(40), RESOURCE_FOOD);

	CreateTownCenter(GetMapPosition(61), 2);
	CreateCivilian(GetMapPosition(42), 2);
	CreateArmySpawner(GetMapPosition(33), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Initialize non training mode
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitNonTrainingMode()
{
	/*
	CreateResources     (GetMapPosition(0), RESOURCE_FOOD);
	CreateResources     (GetMapPosition(1), RESOURCE_WOOD);
	CreateResources     (GetMapPosition(2), RESOURCE_STONE);
	CreateTownCenter    (GetMapPosition(3), 1);
	CreateTownCenter    (GetMapPosition(4), 2);
	CreateCivilian      (GetMapPosition(5), 1);
	CreateCivilian      (GetMapPosition(6), 2);
	CreateClassAWarrior (GetMapPosition(7), 1);
	CreateClassAWarrior (GetMapPosition(8), 2);
	CreateClassBWarrior (GetMapPosition(9), 1);
	CreateClassBWarrior (GetMapPosition(10), 2);
	CreateHouse			(GetMapPosition(11), 1);
	CreateHouse			(GetMapPosition(12), 2);
	CreateArmySpawner   (GetMapPosition(13), 1);
	CreateArmySpawner   (GetMapPosition(14), 2);*/

	m_maxWidth  = 40;
	m_maxHeight = 20;
	g_hiddenLayerCount = 100;
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(LONG_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(ARMY_SPAWNER);
	m_entitiesHavingTraning.push_back(TOWN_CENTER);

	CreateTownCenter(GetMapPosition(722), 1);
	CreateResources(GetMapPosition(402), RESOURCE_FOOD);
	CreateResources(GetMapPosition(435), RESOURCE_FOOD);

	CreateResources(GetMapPosition(670), RESOURCE_STONE);
	CreateResources(GetMapPosition(610), RESOURCE_STONE);

	CreateResources(GetMapPosition(105), RESOURCE_WOOD);
	CreateResources(GetMapPosition(126), RESOURCE_WOOD);


	CreateTownCenter(GetMapPosition(75), 2);

	CreateCivilian      (GetMapPosition(76), 2);
	CreateCivilian      (GetMapPosition(723), 1);
	//CreateArmySpawner   (GetMapPosition(733), 1);
	//CreateClassAWarrior(GetMapPosition(42), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Check if the map mode is non training
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsNonTrainingMode()
{
	if(m_mapMode == MAP_MODE_TRAINING_NONE)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Checks if training is enabled for this entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasTrainingEnabled(Entity *entity)
{
	for(int index = 0;index < m_entitiesHavingTraning.size();index++)
	{
		if(entity->m_type == m_entitiesHavingTraning.at(index))
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Create a civilian at position and with team id
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateCivilian(Vector2 position, int teamID)
{
	Civilian *civilian = new Civilian(this,position,teamID);

	/*civilian->m_resourceTypeCarrying = m_resources.at(0);
	civilian->m_state.m_hasResource = true;*/

	m_civilians.push_back(civilian);
	m_movableEntities.push_back(civilian);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_civilians++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateArmySpawner(Vector2 position, int teamID)
{
	ArmySpawner * armySpawner = new ArmySpawner(this,position,teamID);
	m_armySpawners.push_back(armySpawner);
	m_standAloneEntities.push_back(armySpawner);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_armySpawners++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a class Warrior index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateClassAWarrior(Vector2 position, int teamID)
{
	ClassAWarrior *classAWarrior = new ClassAWarrior(this,position,teamID);
	m_classAWarriors.push_back(classAWarrior);
	m_movableEntities.push_back(classAWarrior);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_shortRangeArmies++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates classB Warriors
*@param   : Position and teamID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateClassBWarrior(Vector2 position, int teamID)
{
	ClassBWarrior *classBWarrior = new ClassBWarrior(this,position,teamID);
	m_classBWarriors.push_back(classBWarrior);
	m_movableEntities.push_back(classBWarrior);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_longRangeArmies++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a House
*@param   : Position and teamID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateHouse(Vector2 position, int teamID)
{
	House *house = new House(this,position,teamID);
	m_houses.push_back(house);
	m_standAloneEntities.push_back(house);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates town center
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateTownCenter(Vector2 position, int teamID)
{
	TownCenter *townCenter = new TownCenter(this,position,teamID);
	m_townCenters.push_back(townCenter);
	m_standAloneEntities.push_back(townCenter);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a resource item in map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateResources(Vector2 position,EntityType type)
{
	Resource *resource = new Resource(this,position,type);
	m_resources.push_back(resource);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Creates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateExplosions(Vector2 position,Rgba color)
{
	if(true)
	{
		return;
	}
	Explosion *explosion = new Explosion(position);
	explosion->m_color = color;
	m_explosions.push_back(explosion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/30
*@purpose : Createas a debug entity push to vector
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateDebugLetterEntity(Vector2 position, std::string str, float fontsize, Rgba color)
{
	DebugEntity *entity = new DebugEntity();
	entity->m_debugString = str;
	entity->m_fontSize = fontsize;
	entity->color = color;
	entity->m_disc.center = position;
	m_debugEntities.push_back(entity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Cheks if a position is inside map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsPositionInsideMap(Vector2 position)
{
	IntVector2 cords = GetCordinates(position);
	if(cords.x == -1 || cords.y ==-1)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsFreeTile(IntVector2 position)
{
	Entity *entity = GetEntityFromPosition(position);
	if(entity == nullptr)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Check if 2 position are neighbours
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsNeighbours(IntVector2 position1, IntVector2 position2)
{
	return IsNeighbours(position1, position2, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsNeighbours(IntVector2 position1, IntVector2 position2, int distance)
{
	IntVector2 displacement = position1 - position2;
	int neighbourDistance = distance;
	if (position1 == position2)
	{
		return false;
	}
	if (displacement.x <= neighbourDistance && displacement.x >= -neighbourDistance && displacement.y <= neighbourDistance && displacement.y >= -neighbourDistance)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/11
*@purpose : Check if the entity is resource
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsResource(Entity * entity)
{
	if(entity == nullptr)
	{
		return false;
	}
	if(entity->m_type == RESOURCE_FOOD || entity->m_type == RESOURCE_STONE || entity->m_type == RESOURCE_WOOD)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Init all mini map values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitMiniMap()
{
	for(int indexY = 0;indexY < m_maxHeight;indexY++)
	{
		for(int indexX = 0;indexX < m_maxWidth;indexX++)
		{
			m_minimapValue.push_back(0);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Updates mini map object with current entities
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateMiniMap()
{
	int count = 0;
	for (int indexY = 0; indexY < m_maxHeight; indexY++)
	{
		for (int indexX = 0; indexX < m_maxWidth; indexX++)
		{
			int tileIndex = indexY * m_maxWidth + indexX;
			Entity *entity = GetEntityFromPosition(tileIndex);
			if (entity != nullptr)
			{
				count++;
				SetMiniMapValues(indexX, indexY, entity->GetMiniMapValue());
				continue;
			}
			SetMiniMapValues(indexX, indexY, 0.f);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Retrives values from minimap object
*@param   : Row and column index
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Map::GetMiniMapValueAtPosition(int x, int y)
{
	int tileIndex = y * m_maxWidth + x;
	if (tileIndex >= 0 && tileIndex < m_maxHeight * m_maxWidth)
	{
		return m_minimapValue.at(tileIndex);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/22
*@purpose : Sets the values of minimap 
*@param   : Row,column and minimap value
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SetMiniMapValues(int x, int y, float minimapValue)
{
	int tileIndex = y * m_maxWidth + x;
	if(tileIndex >= 0 && tileIndex < m_maxHeight * m_maxWidth)
	{
		m_minimapValue.at(tileIndex) = static_cast<double>(minimapValue);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Checks the clossness to townCenter
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Map::NearnessValueToTownCenter(IntVector2 coords, int teamID)
{
	UNUSED(teamID);
	CellSensoryValues cellValue = m_cellSensoryValues.at(GetTileIndex(coords));
	/*if(teamID == 1)
	{
		return cellValue.m_team1TownCenterNearness;
	}
	if(teamID == 2)
	{
		return cellValue.m_team2TownCenterNearness;
	}*/
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Updates cell sensory values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCellSensoryValues()
{
	for(int index = 0;index < m_maxHeight*m_maxWidth;index++)
	{
		/*float shortRangeArmyTeam1NearnessValue	 = 0.f;
		float shortRangeArmyTeam1Count			 = 0.f;
		float shortRangeArmyTeam2NearnessValue	 = 0.f;
		float shortRangeArmyTeam2Count			 = 0.f;

		float longRangeArmyTeam1NearnessValue = 0.f;
		float longRangeArmyTeam1Count = 0.f;
		float longRangeArmyTeam2NearnessValue = 0.f;
		float longRangeArmyTeam2Count = 0.f;

		float buildingTeam1NearnessValue = 0.f;
		float buildingTeam1Count		 = 0.f;
		float buildingTeam2NearnessValue = 0.f;
		float buildingTeam2Count		 = 0.f;

		float civilianTeam1NearnessValue = 0.f;
		float civilianTeam1Count		 = 0.f;
		float civilianTeam2NearnessValue = 0.f;
		float civilianTeam2Count		 = 0.f;

		float townCenterTeam1NearnessValue = 0.f;
		float townCenterTeam1Count		   = 0.f;
		float townCenterTeam2NearnessValue = 0.f;
		float townCenterTeam2Count		   = 0.f;


		for(int civilianIndex = 0;civilianIndex < m_civilians.size();civilianIndex++)
		{
			if(m_civilians.at(civilianIndex)->m_teamID == 1)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_civilians.at(civilianIndex)->GetCordinates());
				civilianTeam1NearnessValue += cellDistance;
				civilianTeam1Count++;
			}
			if(m_civilians.at(civilianIndex)->m_teamID == 2)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_civilians.at(civilianIndex)->GetCordinates());
				civilianTeam2NearnessValue += cellDistance;
				civilianTeam2Count++;
			}
		}

		for(int houseIndex = 0;houseIndex < m_houses.size();houseIndex++)
		{
			if (m_houses.at(houseIndex)->m_teamID == 1)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_houses.at(houseIndex)->GetCordinates());
				buildingTeam1NearnessValue += cellDistance;
				buildingTeam1Count++;
			}
			if (m_houses.at(houseIndex)->m_teamID == 2)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_houses.at(houseIndex)->GetCordinates());
				buildingTeam2NearnessValue += cellDistance;
				buildingTeam2Count++;
			}
		}

		for (int armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
		{
			if (m_armySpawners.at(armySpawnerIndex)->m_teamID == 1)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_armySpawners.at(armySpawnerIndex)->GetCordinates());
				buildingTeam1NearnessValue += cellDistance;
				buildingTeam1Count++;
			}
			if (m_armySpawners.at(armySpawnerIndex)->m_teamID == 2)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_armySpawners.at(armySpawnerIndex)->GetCordinates());
				buildingTeam2NearnessValue += cellDistance;
				buildingTeam2Count++;
			}
		}


		for (int armyUnitIndex = 0; armyUnitIndex < m_classAWarriors.size(); armyUnitIndex++)
		{
			if (m_classAWarriors.at(armyUnitIndex)->m_teamID == 1)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_classAWarriors.at(armyUnitIndex)->GetCordinates());
				shortRangeArmyTeam1NearnessValue += cellDistance;
				shortRangeArmyTeam1Count++;
			}
			if (m_classAWarriors.at(armyUnitIndex)->m_teamID == 2)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_classAWarriors.at(armyUnitIndex)->GetCordinates());
				shortRangeArmyTeam2NearnessValue += cellDistance;
				shortRangeArmyTeam2Count++;
			}
		}

		for (int armyUnitIndex = 0; armyUnitIndex < m_classBWarriors.size(); armyUnitIndex++)
		{
			if (m_classBWarriors.at(armyUnitIndex)->m_teamID == 1)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_classBWarriors.at(armyUnitIndex)->GetCordinates());
				longRangeArmyTeam1NearnessValue += cellDistance;
				longRangeArmyTeam1Count++;
			}
			if (m_classBWarriors.at(armyUnitIndex)->m_teamID == 2)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_classBWarriors.at(armyUnitIndex)->GetCordinates());
				longRangeArmyTeam2NearnessValue += cellDistance;
				longRangeArmyTeam2Count++;
			}
		}

		for (int townCenterIndex = 0; townCenterIndex < m_townCenters.size(); townCenterIndex++)
		{
			if (m_townCenters.at(townCenterIndex)->m_teamID == 1)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_townCenters.at(townCenterIndex)->GetCordinates());
				townCenterTeam1NearnessValue += cellDistance;
				townCenterTeam1Count++;
			}
			if (m_townCenters.at(townCenterIndex)->m_teamID == 2)
			{
				int cellDistance = GetCellDistance(GetCordinates(index), m_townCenters.at(townCenterIndex)->GetCordinates());
				townCenterTeam2NearnessValue += cellDistance;
				townCenterTeam2Count++;
			}
		}

		//armyTeam1NearnessValue = RangeMap(armyTeam1NearnessValue, 0, GetMax(m_maxHeight, m_maxWidth), 0, 1);



		if (shortRangeArmyTeam1Count == 0)
		{
			shortRangeArmyTeam1Count = 1;
		}
		if (shortRangeArmyTeam2Count == 0)
		{
			shortRangeArmyTeam2Count = 1;
		}
		if (longRangeArmyTeam1Count == 0)
		{
			longRangeArmyTeam1Count = 1;
		}
		if (longRangeArmyTeam2Count == 0)
		{
			longRangeArmyTeam2Count = 1;
		}
		if (buildingTeam1Count == 0)
		{
			buildingTeam1Count = 1;
		}
		if (buildingTeam2Count == 0)
		{
			buildingTeam2Count = 1;
		}
		if (civilianTeam1Count == 0)
		{
			civilianTeam1Count = 1;
		}
		if (civilianTeam2Count == 0)
		{
			civilianTeam2Count = 1;
		}*/


		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM1_ARMY_SHORT_RANGE) 
			= GetHeatMapDistanceFromEntity(GetCordinates(index),SHORT_RANGE_ARMY,1);
		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM2_ARMY_SHORT_RANGE) 
			= GetHeatMapDistanceFromEntity(GetCordinates(index),SHORT_RANGE_ARMY,2);

		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM1_ARMY_LONG_RANGE) 
			= GetHeatMapDistanceFromEntity(GetCordinates(index),LONG_RANGE_ARMY,1);
		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM2_ARMY_LONG_RANGE) 
			= GetHeatMapDistanceFromEntity(GetCordinates(index),LONG_RANGE_ARMY,2);

		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM1_BUILDING)   
			= GetHeatMapDistanceFromEntity(GetCordinates(index),HOUSE,1);
		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM2_BUILDING)   
			= GetHeatMapDistanceFromEntity(GetCordinates(index),HOUSE,2);
		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM1_ARMYSPAWNER)
			= GetHeatMapDistanceFromEntity(GetCordinates(index), ARMY_SPAWNER, 1);
		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM2_ARMYSPAWNER)
			= GetHeatMapDistanceFromEntity(GetCordinates(index), ARMY_SPAWNER, 2);

		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM1_CIVILIAN)  
			= GetHeatMapDistanceFromEntity(GetCordinates(index),CIVILIAN,1);
		m_cellSensoryValues.at(index).m_entityNearness.at(FAVORED_MOVETO_TEAM2_CIVILIAN)  
			= GetHeatMapDistanceFromEntity(GetCordinates(index),CIVILIAN,2);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : returns my score board
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ScoreBoard & Map::GetMyScoreBoard(Entity *entity)
{
	if(entity->m_teamID == 1)
	{
		return m_team1;
	}
	return m_team2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : Gets a free neighbour tile
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetFreeNeighbourTile(Vector2 position)
{
	IntVector2 freeNeighbourTile = GetFreeNeighbourTile(position, 1);
	if(freeNeighbourTile == IntVector2(-1,-1))
	{
		freeNeighbourTile = GetFreeNeighbourTile(position, 2);
		if (freeNeighbourTile == IntVector2(-1, -1))
		{
			return GetFreeNeighbourTile(position, 3);
		}
		return freeNeighbourTile;
	}
	return freeNeighbourTile;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetFreeNeighbourTile(Vector2 position, int distance)
{
	int tileIndex = GetTileIndex(position);
	IntVector2 tileCords = GetCordinates(tileIndex);

	IntVector2 tileCordsE = IntVector2(tileCords.x + distance, tileCords.y + 0);
	IntVector2 tileCordsN = IntVector2(tileCords.x + 0, tileCords.y + distance);
	IntVector2 tileCordsW = IntVector2(tileCords.x - distance, tileCords.y + 0);
	IntVector2 tileCordsS = IntVector2(tileCords.x + 0, tileCords.y - distance);

	IntVector2 tileCordsNE = IntVector2(tileCords.x + distance, tileCords.y + distance);
	IntVector2 tileCordsNW = IntVector2(tileCords.x - distance, tileCords.y + distance);
	IntVector2 tileCordsSW = IntVector2(tileCords.x - distance, tileCords.y - distance);
	IntVector2 tileCordsSE = IntVector2(tileCords.x + distance, tileCords.y - distance);

	if (IsValidCordinate(tileCordsE))
	{
		if (!HasAnyEntityInTile(tileCordsE))
		{
			return tileCordsE;
		}
	}
	if (IsValidCordinate(tileCordsN))
	{
		if (!HasAnyEntityInTile(tileCordsN))
		{
			return tileCordsN;
		}
	}
	if (IsValidCordinate(tileCordsW))
	{
		if (!HasAnyEntityInTile(tileCordsW))
		{
			return tileCordsW;
		}
	}
	if (IsValidCordinate(tileCordsS))
	{
		if (!HasAnyEntityInTile(tileCordsS))
		{
			return tileCordsS;
		}
	}
	if (IsValidCordinate(tileCordsNE))
	{
		if (!HasAnyEntityInTile(tileCordsNE))
		{
			return tileCordsNE;
		}
	}
	if (IsValidCordinate(tileCordsNW))
	{
		if (!HasAnyEntityInTile(tileCordsNW))
		{
			return tileCordsNW;
		}
	}
	if (IsValidCordinate(tileCordsSW))
	{
		if (!HasAnyEntityInTile(tileCordsSW))
		{
			return tileCordsSW;
		}
	}
	if (IsValidCordinate(tileCordsSE))
	{
		if (!HasAnyEntityInTile(tileCordsSE))
		{
			return tileCordsSE;
		}
	}
	return IntVector2(-1, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Retrieves tile position from map position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetTilePosition(Vector2 position)
{
	int x = static_cast<int>(position.x / g_radius);
	int y = static_cast<int>(position.y / g_radius);
	return IntVector2(x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetTilePosition(int tilePosition)
{
	UNUSED(tilePosition);
	return IntVector2::ONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetRandomNeighbour(IntVector2 tileCords, int distance)
{
	tileCords			   = ClampCoordinates(tileCords);
	IntVector2 tileCordsE  = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y + 0));
	IntVector2 tileCordsN  = ClampCoordinates(IntVector2(tileCords.x + 0, tileCords.y + distance));
	IntVector2 tileCordsW  = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y + 0));
	IntVector2 tileCordsS  = ClampCoordinates(IntVector2(tileCords.x + 0, tileCords.y - distance));
	
	IntVector2 tileCordsNE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y + distance));
	IntVector2 tileCordsNW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y + distance));
	IntVector2 tileCordsSW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y - distance));
	IntVector2 tileCordsSE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y - distance));

	std::vector<IntVector2> cords;
	for(int index = tileCordsW.x;index <= tileCordsE.x;index++)
	{
		IntVector2 ncords(index, tileCordsW.y);
		if(ncords == tileCords)
		{
			continue;
		}
		cords.push_back(ncords);
	}
	for (int index = tileCordsNW.x; index <= tileCordsNE.x; index++)
	{
		IntVector2 ncords(index, tileCordsNW.y);
		if (ncords == tileCords)
		{
			continue;
		}
		cords.push_back(ncords);
	}
	for (int index = tileCordsSW.x; index <= tileCordsSE.x; index++)
	{
		IntVector2 ncords(index, tileCordsSE.y);
		if (ncords == tileCords)
		{
			continue;
		}
		cords.push_back(ncords);
	}
	if(cords.size() == 0)
	{
		return IntVector2::ONE*-1;
	}
	return cords.at(GetRandomIntLessThan(static_cast<int>(cords.size())));
	/*if (IsValidCordinate(tileCordsE))
	{
		if (!HasAnyEntityInTile(tileCordsE))
		{
			return tileCordsE;
		}
	}
	if (IsValidCordinate(tileCordsN))
	{
		if (!HasAnyEntityInTile(tileCordsN))
		{
			return tileCordsN;
		}
	}
	if (IsValidCordinate(tileCordsW))
	{
		if (!HasAnyEntityInTile(tileCordsW))
		{
			return tileCordsW;
		}
	}
	if (IsValidCordinate(tileCordsS))
	{
		if (!HasAnyEntityInTile(tileCordsS))
		{
			return tileCordsS;
		}
	}
	if (IsValidCordinate(tileCordsNE))
	{
		if (!HasAnyEntityInTile(tileCordsNE))
		{
			return tileCordsNE;
		}
	}
	if (IsValidCordinate(tileCordsNW))
	{
		if (!HasAnyEntityInTile(tileCordsNW))
		{
			return tileCordsNW;
		}
	}
	if (IsValidCordinate(tileCordsSW))
	{
		if (!HasAnyEntityInTile(tileCordsSW))
		{
			return tileCordsSW;
		}
	}
	if (IsValidCordinate(tileCordsSE))
	{
		if (!HasAnyEntityInTile(tileCordsSE))
		{
			return tileCordsSE;
		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/13
*@purpose : Retrieves all neighbor coordinates
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<IntVector2> Map::GetAllNeighbourCoordinates(IntVector2 tileCords,int distance)
{
	tileCords = ClampCoordinates(tileCords);
	IntVector2 tileCordsE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y + 0));
	IntVector2 tileCordsN = ClampCoordinates(IntVector2(tileCords.x + 0, tileCords.y + distance));
	IntVector2 tileCordsW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y + 0));
	IntVector2 tileCordsS = ClampCoordinates(IntVector2(tileCords.x + 0, tileCords.y - distance));

	IntVector2 tileCordsNE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y + distance));
	IntVector2 tileCordsNW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y + distance));
	IntVector2 tileCordsSW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y - distance));
	IntVector2 tileCordsSE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y - distance));

	std::vector<IntVector2> neighbourCords;
	neighbourCords.reserve(8);
	
	for(int index = 0;index < 8;index++)
	{
		bool hasCordinate = false;
		IntVector2 currentTileCords;
		if(index == 0)
		{
			currentTileCords = tileCordsE;
		}
		if (index == 1)
		{
			currentTileCords = tileCordsN;
		}
		if (index == 2)
		{
			currentTileCords = tileCordsW;
		}
		if (index == 3)
		{
			currentTileCords = tileCordsS;
		}
		if (index == 4)
		{
			currentTileCords = tileCordsNE;
		}
		if (index == 5)
		{
			currentTileCords = tileCordsNW;
		}
		if (index == 6)
		{
			currentTileCords = tileCordsSW;
		}
		if (index == 7)
		{
			currentTileCords = tileCordsSE;
		}
		if(currentTileCords == tileCords)
		{
			continue;
		}
		for(int coordsIndex = 0;coordsIndex < neighbourCords.size();coordsIndex++)
		{
			if(neighbourCords.at(coordsIndex) == currentTileCords)
			{
				hasCordinate = true;
			}
		}
		if(hasCordinate == false)
		{
			neighbourCords.push_back(currentTileCords);
		}
	}
	return neighbourCords;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : Clamp cords to the map cords
*@param   : Cords
*@return  : Clamped cords
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::ClampCoordinates(IntVector2 cords)
{
	cords.x = ClampInt(cords.x, 0, m_maxWidth - 1);
	cords.y = ClampInt(cords.y, 0, m_maxHeight - 1);
	return cords;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetTileIndex(Vector2 mapPosition)
{
	//mapPosition.y = Windows::GetInstance()->GetDimensions().y - mapPosition.y;
	//DebugDraw::GetInstance()->DebugRenderLogf("MousePOs %f ", mapPosition.y);
	int x = static_cast<int>((mapPosition.x - m_xOffset) / g_unitDistance);
	int y = static_cast<int>((mapPosition.y - m_yOffset) / g_unitDistance);
	if(x > m_maxWidth || y > m_maxHeight || x < 0 || y < 0)
	{
		return -1;
	}
	return GetTileIndex(IntVector2(x, y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetTileIndex(IntVector2 position)
{
	return (position.x) + (position.y) * m_maxWidth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : Returns the cell distance
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetCellDistance(Vector2 position, Vector2 position2)
{
	IntVector2 cords1 = GetCordinates(position);
	IntVector2 cords2 = GetCordinates(position2);
	return GetCellDistance(cords1, cords2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Map::GetCellDistance(IntVector2 cords1, IntVector2 cords2)
{
	IntVector2 distance = cords1 - cords2;
	if (GetAbsolute(distance.x) > GetAbsolute(distance.y))
	{
		return GetAbsolute(distance.x);
	}
	return GetAbsolute(distance.y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Check for all entities in a grid
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasAnyEntityInTile(int index)
{
	for(int armySpawnerIndex = 0;armySpawnerIndex < m_armySpawners.size();armySpawnerIndex++)
	{
		Vector2 position = m_armySpawners.at(armySpawnerIndex)->GetPosition();
		int armySpawnerTileIndex = GetTileIndex(position);
		if(armySpawnerTileIndex == index)
		{
			return true;
		}
	}
	for (int civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
	{
		Vector2 position = m_civilians.at(civilianIndex)->GetPosition();
		int civilianTileIndex = GetTileIndex(position);
		if (civilianTileIndex == index)
		{
			return true;
		}
	}
	for (int classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		Vector2 position = m_classAWarriors.at(classAWarriorIndex)->GetPosition();
		int classAWarriorPositionIndex = GetTileIndex(position);
		if (classAWarriorPositionIndex == index)
		{
			return true;
		}
	}
	for (int classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		Vector2 position = m_classBWarriors.at(classBWarriorIndex)->GetPosition();
		int classBWarriorPositionIndex = GetTileIndex(position);
		if (classBWarriorPositionIndex == index)
		{
			return true;
		}
	}
	for (int houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		Vector2 position = m_houses.at(houseIndex)->GetPosition();
		int housePositionIndex = GetTileIndex(position);
		if (housePositionIndex == index)
		{
			return true;
		}
	}
	for (int resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		Vector2 position = m_resources.at(resourceIndex)->GetPosition();
		int resourceTileIndex = GetTileIndex(position);
		if (resourceTileIndex == index)
		{
			return true;
		}
	}
	for (int towncenterIndex = 0; towncenterIndex < m_townCenters.size(); towncenterIndex++)
	{
		Vector2 position = m_townCenters.at(towncenterIndex)->GetPosition();
		int townCenterPositionIndex = GetTileIndex(position);
		if (townCenterPositionIndex == index)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Check for all entities in a grid
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasAnyEntityInTile(IntVector2 Cords)
{
	int index = GetTileIndex(Cords);
	if(index == -1)
	{
		return false;
	}
	return HasAnyEntityInTile(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Check for all entities in a grid
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasAnyEntityInTile(Vector2 mapPosition)
{
	int index = GetTileIndex(mapPosition);
	if(index == -1)
	{
		return false;
	}
	return HasAnyEntityInTile(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::GetEntityFromPosition(int index)
{
	if(index < 0 && index > m_maxHeight*m_maxWidth)
	{
		return nullptr;
	}

	for (int armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		Vector2 position = m_armySpawners.at(armySpawnerIndex)->GetPosition();
		int armySpawnerTileIndex = GetTileIndex(position);
		if (armySpawnerTileIndex == index)
		{
			return m_armySpawners.at(armySpawnerIndex);
		}
	}
	for (int civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
	{
		Vector2 position = m_civilians.at(civilianIndex)->GetPosition();
		int civilianTileIndex = GetTileIndex(position);
		if (civilianTileIndex == index)
		{
			return m_civilians.at(civilianIndex);
		}
	}
	for (int classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		Vector2 position = m_classAWarriors.at(classAWarriorIndex)->GetPosition();
		int classAWarriorPositionIndex = GetTileIndex(position);
		if (classAWarriorPositionIndex == index)
		{
			return m_classAWarriors.at(classAWarriorIndex);
		}
	}
	for (int classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		Vector2 position = m_classBWarriors.at(classBWarriorIndex)->GetPosition();
		int classBWarriorPositionIndex = GetTileIndex(position);
		if (classBWarriorPositionIndex == index)
		{
			return m_classBWarriors.at(classBWarriorIndex);
		}
	}
	for (int houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		Vector2 position = m_houses.at(houseIndex)->GetPosition();
		int housePositionIndex = GetTileIndex(position);
		if (housePositionIndex == index)
		{
			return m_houses.at(houseIndex);
		}
	}
	for (int resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		Vector2 position = m_resources.at(resourceIndex)->GetPosition();
		int resourceTileIndex = GetTileIndex(position);
		if (resourceTileIndex == index)
		{
			return m_resources.at(resourceIndex);
		}
	}
	for (int towncenterIndex = 0; towncenterIndex < m_townCenters.size(); towncenterIndex++)
	{
		Vector2 position = m_townCenters.at(towncenterIndex)->GetPosition();
		int townCenterPositionIndex = GetTileIndex(position);
		if (townCenterPositionIndex == index)
		{
			return m_townCenters.at(towncenterIndex);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::GetEntityFromPosition(IntVector2 cords)
{
	int tileIndex = GetTileIndex(cords);
	return GetEntityFromPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::GetEntityFromPosition(Vector2 position)
{
	int tileIndex = GetTileIndex(position);
	return GetEntityFromPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Map::GetAllEntitiesNearLocation(Vector2 mapPosition, int cellDistance)
{
	std::vector<Entity*> entityList;
	IntVector2 cords = GetCordinates(mapPosition);
	Entity *entity   = GetEntityFromPosition(cords + IntVector2(cellDistance, 0));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(cellDistance, cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(0, cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(-cellDistance, cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(-cellDistance, 0));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(-cellDistance, -cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(0, -cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	entity = GetEntityFromPosition(cords + IntVector2(cellDistance, -cellDistance));
	if (entity != nullptr)
	{
		entityList.push_back(entity);
	}
	return entityList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Map::GetAllResourcesNearLocation(Vector2 mapPosition, int distance)
{
	std::vector<Entity*> entityList = GetAllEntitiesNearLocation(mapPosition, distance);
	std::vector<Entity*> resourceList;
	for (int entityIndex = 0; entityIndex < entityList.size(); entityIndex++)
	{	
		if(entityList.at(entityIndex)->m_type == RESOURCE_FOOD || entityList.at(entityIndex)->m_type == RESOURCE_STONE ||entityList.at(entityIndex)->m_type == RESOURCE_WOOD)
		{
			resourceList.push_back(entityList.at(entityIndex));
		}
	}
	return resourceList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Map::GetAllTownCentersNearLocation(Vector2 mapPosition, int distance)
{
	std::vector<Entity*> entityList = GetAllEntitiesNearLocation(mapPosition, distance);
	std::vector<Entity*> resourceList;
	for (int entityIndex = 0; entityIndex < entityList.size(); entityIndex++)
	{
		if (entityList.at(entityIndex)->m_type == TOWN_CENTER)
		{
			resourceList.push_back(entityList.at(entityIndex));
		}
	}
	return resourceList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/21
*@purpose : Get all enemies near location
*@param   : current team id , maps position ,cell distance
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Map::GetAllEnemiesNearLocation(int teamID, Vector2 mapPosition, int distance)
{
	std::vector<Entity*> entityList = GetAllEntitiesNearLocation(mapPosition, distance);
	std::vector<Entity*> returnList;
	for(int index = 0;index < entityList.size();index++)
	{
		if(entityList.at(index)->m_teamID != teamID && entityList.at(index)->m_teamID != 0)
		{
			returnList.push_back(entityList.at(index));
		}
	}
	return returnList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Is a valid map cordinate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsValidCordinate(IntVector2 cords)
{
	if(cords.x >= 0 && cords.x < m_maxWidth && cords.y >=0 && cords.y < m_maxHeight)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on tile
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Map::AttackOnPosition(int tileIndex,float damagePoint)
{
	Entity *entity = GetEntityFromPosition(tileIndex);
	if(entity == nullptr)
	{
		return nullptr;
	}
	entity->TakeDamage(damagePoint);
	return entity;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on cords
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Map::AttackOnPosition(IntVector2 cords, float damagePoint)
{
	int tileIndex = GetTileIndex(cords);
	return AttackOnPosition(tileIndex,damagePoint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Map::AttackOnPosition(Vector2 position, float damagePoint)
{
	int tileIndex = GetTileIndex(position);
	return AttackOnPosition(tileIndex, damagePoint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : Check if 2 entities are enemies
*@param   : 2 entities
*@return  : true if teamId not matched
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsEnemies(Entity *entityOne, Entity *entityTwo)
{
	if(entityOne->m_teamID != entityTwo->m_teamID)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Destroys entity
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::DestroyEntity(Entity *entity)
{
	switch (entity->m_type)
	{
	case CIVILIAN:
		for (int civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
		{
			if (m_civilians.at(civilianIndex) == entity)
			{
				delete m_civilians.at(civilianIndex);
				m_civilians.erase(m_civilians.begin() + civilianIndex, m_civilians.begin() + civilianIndex + 1);
				return;
			}
		}
		break;
	case SHORT_RANGE_ARMY:
		for (int warriorShortRangeIndex = 0; warriorShortRangeIndex < m_classAWarriors.size(); warriorShortRangeIndex++)
		{
			if (m_classAWarriors.at(warriorShortRangeIndex) == entity)
			{
				delete m_classAWarriors.at(warriorShortRangeIndex);
				m_classAWarriors.erase(m_classAWarriors.begin() + warriorShortRangeIndex, m_classAWarriors.begin() + warriorShortRangeIndex + 1);
				return;
			}
		}
		break;
	case LONG_RANGE_ARMY:
		for (int warriorLongRangeIndex = 0; warriorLongRangeIndex < m_classBWarriors.size(); warriorLongRangeIndex++)
		{
			if (m_classBWarriors.at(warriorLongRangeIndex) == entity)
			{
				delete m_classBWarriors.at(warriorLongRangeIndex);
				m_classBWarriors.erase(m_classBWarriors.begin() + warriorLongRangeIndex, m_classBWarriors.begin() + warriorLongRangeIndex + 1);
				return;
			}
		}
		break;
	case HOUSE:
		for (int houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
		{
			if (m_houses.at(houseIndex) == entity)
			{
				delete m_houses.at(houseIndex);
				m_houses.erase(m_houses.begin() + houseIndex, m_houses.begin() + houseIndex + 1);
				return;
			}
		}
		break;
	case TOWN_CENTER:
		for (int townCenterIndex = 0; townCenterIndex < m_houses.size(); townCenterIndex++)
		{
			if (m_houses.at(townCenterIndex) == entity)
			{
				delete m_houses.at(townCenterIndex);
				m_houses.erase(m_houses.begin() + townCenterIndex, m_houses.begin() + townCenterIndex + 1);
				return;
			}
		}
		break;
	case RESOURCE_FOOD:
		for (int resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
		{
			if (m_resources.at(resourceIndex) == entity)
			{
				delete m_resources.at(resourceIndex);
				m_resources.erase(m_resources.begin() + resourceIndex, m_resources.begin() + resourceIndex + 1);
				return;
			}
		}
		break;
	case ARMY_SPAWNER:
		for (int armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
		{
			if (m_armySpawners.at(armySpawnerIndex) == entity)
			{
				delete m_armySpawners.at(armySpawnerIndex);
				m_armySpawners.erase(m_armySpawners.begin() + armySpawnerIndex, m_armySpawners.begin() + armySpawnerIndex + 1);
				return;
			}
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Changes to map position from tile index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Map::GetMapPosition(int tileIndex)
{
	float x = static_cast<float>(tileIndex % m_maxWidth);
	float y = static_cast<float>(tileIndex / m_maxWidth);
	
	return Vector2(x * g_unitDistance + m_xOffset + g_radius, y * g_unitDistance + m_yOffset + g_radius);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Map position from cordinate position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Map::GetMapPosition(IntVector2 tilecords)
{
	if(!IsValidCordinate(tilecords))
	{
		return Vector2::ONE*-1;
	}
	int tileIndex = GetTileIndex(tilecords);
	return GetMapPosition(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetCordinates(int tileIndex)
{
	int x = tileIndex % m_maxWidth;
	int y = tileIndex / m_maxWidth;
	return IntVector2(x, y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Gets position as cords
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetCordinates(Vector2 mapPosition)
{
	int tileIndex = GetTileIndex(mapPosition);
	return GetCordinates(tileIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	m_mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	m_mousePosition.y = Windows::GetInstance()->GetDimensions().y - m_mousePosition.y;
	m_currentTileIndex = GetTileIndex(m_mousePosition);

	if(InputSystem::GetInstance()->WasRButtonJustPressed())
	{
		g_currentSelectedEntity = nullptr;
	}
	if(InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_P))
	{
		g_enableDebugPrints = g_enableDebugPrints == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_F))
	{
		m_displaySensoryFoodValue = m_displaySensoryFoodValue == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_S))
	{
		m_displaySensoryStoneValue = m_displaySensoryStoneValue == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_W))
	{
		m_displaySensoryWoodValue = m_displaySensoryWoodValue == true ? false : true;
		Renderer::GetInstance()->TakeScreenShotAndSave("Data\\NN\\BestGame\\");
	}


	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_Z))
	{
		m_displaySensoryArmySpawner1Value = m_displaySensoryArmySpawner1Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_X))
	{
		m_displaySensoryArmySpawner2Value = m_displaySensoryArmySpawner2Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_C))
	{
		m_displaySensoryCivilian1Value = m_displaySensoryCivilian1Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_V))
	{
		m_displaySensoryCivilian2Value = m_displaySensoryCivilian2Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_B))
	{
		m_displaySensoryBuilding1Value = m_displaySensoryBuilding1Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_N))
	{
		m_displaySensoryBuilding2Value = m_displaySensoryBuilding2Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_M))
	{
		m_displaySensoryLongRangeArmy1Value = m_displaySensoryLongRangeArmy1Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_G))
	{
		m_displaySensoryLongRangeArmy2Value = m_displaySensoryLongRangeArmy2Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_H))
	{
		m_displaySensoryShortRangeArmy1Value = m_displaySensoryShortRangeArmy1Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_J))
	{
		m_displaySensoryShortRangeArmy2Value = m_displaySensoryShortRangeArmy2Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_K))
	{
		m_displaySensoryTC1Value = m_displaySensoryTC1Value == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_L))
	{
		m_displaySensoryTC2Value = m_displaySensoryTC2Value == true ? false : true;
	}

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_1))
	{
		m_townCenters.at(0)->m_resourceStat.m_food++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_2))
	{
		m_townCenters.at(0)->m_resourceStat.m_stone++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_3))
	{
		m_townCenters.at(0)->m_resourceStat.m_wood++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_4))
	{
		m_townCenters.at(1)->m_resourceStat.m_food++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_5))
	{
		m_townCenters.at(1)->m_resourceStat.m_stone++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_6))
	{
		m_townCenters.at(1)->m_resourceStat.m_wood++;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_7))
	{
		CreateClassAWarrior(m_mousePosition, 1);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_8))
	{
		CreateClassBWarrior(m_mousePosition, 1);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_9))
	{
		CreateClassAWarrior(m_mousePosition, 2);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_0))
	{
		CreateClassBWarrior(m_mousePosition, 2);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_U))
	{
		CreateArmySpawner(m_mousePosition, 1);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_I))
	{
		CreateArmySpawner(m_mousePosition, 2);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_O))
	{
		CreateHouse(m_mousePosition, 1);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_P))
	{
		//CreateHouse(m_mousePosition, 2);
	}

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_Y))
	{
		CreateResources(m_mousePosition,RESOURCE_FOOD);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_U))
	{
		CreateResources(m_mousePosition, RESOURCE_STONE);
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_I))
	{
		CreateResources(m_mousePosition, RESOURCE_WOOD);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndUpdateOnWinCondition()
{
	if(m_gameFinished)
	{
		CheckAndSaveBestStats();
		return;
	}
	for(size_t townCenterIndex = 0;townCenterIndex < m_townCenters.size();townCenterIndex++)
	{
		if(m_townCenters.at(townCenterIndex)->m_health <= 0)
		{
			m_gameFinished = true;
			g_isCurrentlyTraining = false;
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndSaveBestStats()
{
	if (m_isScoreBoardUpdated)
	{
		return;
	}
	m_isScoreBoardUpdated = true;
	CheckAndSaveBestTeamStats();
	CheckAndSaveBestEntities();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Check and compare the current best stat and overwrite if the current one is best
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndSaveBestTeamStats()
{
	m_team1.CalculateTotalScore();
	m_team2.CalculateTotalScore();
	if(m_team1.m_totalScore > g_globalMaxScoreTeam1)
	{
		m_team1.SaveToFile("Data\\NN\\BestGame\\1\\BestStats\\TEAM_1_STATS.txt");
	}
	if (m_team2.m_totalScore > g_globalMaxScoreTeam1)
	{
		m_team2.SaveToFile("Data\\NN\\BestGame\\2\\BestStats\\TEAM_2_STATS.txt");
	}
	m_team2.SaveToFile(("Data\\NN\\" + m_folder+"\\1\\BestStats\\" + "TEAM_1_STATS.txt").c_str());
	m_team2.SaveToFile(("Data\\NN\\" + m_folder+"\\2\\BestStats\\" + "TEAM_2_STATS.txt").c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndSaveBestEntities()
{
	CheckAndSaveBestEntityNN(CIVILIAN, 1);
	CheckAndSaveBestEntityNN(CIVILIAN, 2);
	CheckAndSaveBestEntityNN(SHORT_RANGE_ARMY, 1);
	CheckAndSaveBestEntityNN(SHORT_RANGE_ARMY, 2);
	CheckAndSaveBestEntityNN(LONG_RANGE_ARMY, 1);
	CheckAndSaveBestEntityNN(LONG_RANGE_ARMY, 2);
	CheckAndSaveBestEntityNN(TOWN_CENTER, 1);
	CheckAndSaveBestEntityNN(TOWN_CENTER, 2);
	CheckAndSaveBestEntityNN(ARMY_SPAWNER, 1);
	CheckAndSaveBestEntityNN(ARMY_SPAWNER, 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : Checks the best entity for current game and save stats off
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndSaveBestEntityNN(EntityType type,int teamID)
{
	Entity *entity = FindLocalBestByEntity(type, teamID);
	if(entity == nullptr)
	{
		return;
	}
	switch (type)
	{
	case CIVILIAN:
		if(teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreCivilianTeam1)
		{
			SaveEntityBestGlobalScore(entity);
		}
		if (teamID == 2 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreCivilianTeam2)
		{
			SaveEntityBestGlobalScore(entity);
		}
		break;
	case SHORT_RANGE_ARMY:
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreShortRangeArmy1)
		{
			SaveEntityBestGlobalScore(entity);
		}
		if (teamID == 2 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreShortRangeArmy2)
		{
			SaveEntityBestGlobalScore(entity);
		}
		break;
	case LONG_RANGE_ARMY:
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreLongRangeArmy1)
		{
			SaveEntityBestGlobalScore(entity);
		}
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreLongRangeArmy1)
		{
			SaveEntityBestGlobalScore(entity);
		}
		break;
	case TOWN_CENTER:
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreTownCenter1)
		{
			SaveEntityBestGlobalScore(entity);
		}
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreTownCenter2)
		{
			SaveEntityBestGlobalScore(entity);
		}
		break;
	case ARMY_SPAWNER:
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreArmySpawnerTeam1)
		{
			SaveEntityBestGlobalScore(entity);
		}
		if (teamID == 1 && entity->m_scoreBoard.m_totalScore > g_globalMaxScoreArmySpawnerTeam2)
		{
			SaveEntityBestGlobalScore(entity);
		}
		break;
	default:
		break;
	}
	SaveEntityBestLocalScore(entity);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : Finds current games personal best
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Map::FindLocalBestByEntity(EntityType type,int teamID)
{
	int returnIndex = -1;
	switch (type)
	{
	case CIVILIAN:
		for (size_t index = 0; index < m_civilians.size(); index++)
		{
			if(teamID == m_civilians.at(index)->m_teamID && teamID == 1)
			{
				m_civilians.at(index)->m_scoreBoard.CalculateTotalScore();
				if(g_localMaxScoreCivilianTeam1 < m_civilians.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreCivilianTeam1 = m_civilians.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
			if (teamID == m_civilians.at(index)->m_teamID && teamID == 2)
			{
				m_civilians.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreCivilianTeam2 < m_civilians.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreCivilianTeam2 = m_civilians.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
		}
		if(returnIndex >= 0)
		{
			return m_civilians.at(returnIndex);
		}
		break;
	case SHORT_RANGE_ARMY:
		for (size_t index = 0; index < m_classAWarriors.size(); index++)
		{
			if (teamID == m_classAWarriors.at(index)->m_teamID && teamID == 1)
			{
				m_classAWarriors.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreShortRangeArmy1 < m_classAWarriors.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreShortRangeArmy1 = m_classAWarriors.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
			if (teamID == m_classAWarriors.at(index)->m_teamID && teamID == 2)
			{
				m_classAWarriors.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreShortRangeArmy2 < m_classAWarriors.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreShortRangeArmy2 = m_classAWarriors.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
		}
		break;
	case LONG_RANGE_ARMY:
		for (size_t index = 0; index < m_classBWarriors.size(); index++)
		{
			if (teamID == m_classBWarriors.at(index)->m_teamID && teamID == 1)
			{
				m_classBWarriors.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreLongRangeArmy1 < m_classBWarriors.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreLongRangeArmy1 = m_classBWarriors.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
			if (teamID == m_classBWarriors.at(index)->m_teamID && teamID == 2)
			{
				m_classBWarriors.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreLongRangeArmy2 < m_classBWarriors.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreLongRangeArmy2 = m_classBWarriors.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
		}
		break;
	case TOWN_CENTER:
		for (size_t index = 0; index < m_townCenters.size(); index++)
		{
			if (teamID == m_townCenters.at(index)->m_teamID && teamID == 1)
			{
				m_townCenters.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreTownCenter1 < m_townCenters.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreTownCenter1 = m_townCenters.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
			if (teamID == m_townCenters.at(index)->m_teamID && teamID == 2)
			{
				m_townCenters.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreTownCenter2 < m_townCenters.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreTownCenter2 = m_townCenters.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
		}
		break;
	case ARMY_SPAWNER:
		for (size_t index = 0; index < m_armySpawners.size(); index++)
		{
			if (teamID == m_armySpawners.at(index)->m_teamID && teamID == 1)
			{
				m_armySpawners.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreArmySpawnerTeam1 < m_armySpawners.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreArmySpawnerTeam1 = m_armySpawners.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
			if (teamID == m_armySpawners.at(index)->m_teamID && teamID == 2)
			{
				m_armySpawners.at(index)->m_scoreBoard.CalculateTotalScore();
				if (g_localMaxScoreArmySpawnerTeam2 < m_armySpawners.at(index)->m_scoreBoard.m_totalScore)
				{
					g_localMaxScoreArmySpawnerTeam2 = m_armySpawners.at(index)->m_scoreBoard.m_totalScore;
					returnIndex = index;
				}
			}
		}
		break;
	default:
		break;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : Saves off entity best score
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SaveEntityBestGlobalScore(Entity *entity)
{
	FileWrite(entity->GetGlobalBestStatFilePath(), ToString(entity->m_scoreBoard.m_totalScore));

	std::string filePath = entity->GetGlobalBestFilePath();
	size_t size = filePath.length();
	FileRemove(filePath.c_str());
	entity->m_neuralNet.StoreToFile(filePath.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SaveEntityBestLocalScore(Entity *entity)
{
	FileWrite(entity->GetLocalBestStatFilePath(), ToString(entity->m_scoreBoard.m_totalScore));
	std::string filePath = entity->GetLocalBestFilePath();
	remove(filePath.c_str());
	entity->m_neuralNet.StoreToFile(filePath.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : If two entities overlap make the distant
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndClearEntityOverlap()
{
	for (size_t entityOneIndex = 0; entityOneIndex < m_movableEntities.size(); entityOneIndex++)
	{
		for (size_t entityTwoIndex = entityOneIndex + 1; entityTwoIndex < m_movableEntities.size(); entityTwoIndex++)
		{
			if(m_movableEntities.at(entityOneIndex)->m_taskQueue.size() == 0 && m_movableEntities.at(entityTwoIndex)->m_taskQueue.size() == 0)
			{
				if (m_movableEntities.at(entityOneIndex)->GetTileIndex() == m_movableEntities.at(entityTwoIndex)->GetTileIndex())
				{
					IntVector2 freeCords = GetFreeNeighbourTile(m_movableEntities.at(entityOneIndex)->GetPosition());
					if (freeCords == IntVector2(-1, -1))
					{
						freeCords = GetCordinates(m_movableEntities.at(entityOneIndex)->GetPosition());
						freeCords = freeCords + IntVector2(GetRandomIntInRange(-1,2), GetRandomIntInRange(-1,2));
					}
					m_movableEntities.at(entityOneIndex)->SetPosition(GetMapPosition(freeCords));
				}
			}
		}
	}

	for (size_t entityOneIndex = 0; entityOneIndex < m_standAloneEntities.size(); entityOneIndex++)
	{
		for (size_t entityTwoIndex = 0; entityTwoIndex < m_movableEntities.size(); entityTwoIndex++)
		{
			if (m_movableEntities.at(entityTwoIndex)->m_taskQueue.size() == 0)
			{
				if (m_movableEntities.at(entityTwoIndex)->GetTileIndex() == m_standAloneEntities.at(entityOneIndex)->GetTileIndex())
				{
					IntVector2 freeCords = GetFreeNeighbourTile(m_movableEntities.at(entityTwoIndex)->GetPosition());
					if (freeCords == IntVector2(-1, -1))
					{
						freeCords = GetCordinates(m_movableEntities.at(entityTwoIndex)->GetPosition());
						freeCords = freeCords + IntVector2(GetRandomIntInRange(-1, 2), GetRandomIntInRange(-1, 2));
					}
					m_movableEntities.at(entityTwoIndex)->SetPosition(GetMapPosition(freeCords));
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Deletes entity from movable entity list
*@param   : Entity to be deleted
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::DeleteFromMovableEntityList(Entity *entity)
{
	for (size_t movableEntityIndex = 0; movableEntityIndex < m_movableEntities.size(); movableEntityIndex++)
	{
		if(m_movableEntities.at(movableEntityIndex) == entity)
		{
			m_movableEntities.erase(m_movableEntities.begin() + movableEntityIndex, m_movableEntities.begin() + movableEntityIndex + 1);
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Deletes entity from standalone list
*@param   : Entity to be deleted
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::DeleteFromStandAlonEntityList(Entity *entity)
{
	for (size_t standAlonEntityIndex = 0; standAlonEntityIndex < m_standAloneEntities.size(); standAlonEntityIndex++)
	{
		if (m_standAloneEntities.at(standAlonEntityIndex) == entity)
		{
			m_standAloneEntities.erase(m_standAloneEntities.begin() + standAlonEntityIndex, m_standAloneEntities.begin() + standAlonEntityIndex + 1);
			return;
		}
	}
}

void Map::Update(float deltaTime)
{
	CheckAndUpdateOnWinCondition();
	CheckAndClearEntityOverlap();
	if(m_gameFinished)
	{
		return;
	}
	ProcessInputs(deltaTime);
	UpdateMiniMap();
	UpdateCellSensoryValues();

	UpdateCamera(deltaTime);
	UpdateResources(deltaTime);
	UpdateCivilans(deltaTime);
	UpdateArmySpawners(deltaTime);
	UpdateClassAWarriors(deltaTime);
	UpdateClassBWarriors(deltaTime);
	UpdateHouses(deltaTime);
	UpdateTownCenters(deltaTime);
	UpdateExplosions(deltaTime);
	UpdateDebugEntities(deltaTime);

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_E))
	{
		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Updates camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCivilans(float deltaTime)
{
	for (size_t civiliansIndex = 0; civiliansIndex < m_civilians.size(); civiliansIndex++)
	{
		m_civilians.at(civiliansIndex)->Update(deltaTime);
		if(m_civilians.at(civiliansIndex)->m_health <= 0)
		{
			DeleteFromMovableEntityList(m_civilians.at(civiliansIndex));
			delete m_civilians.at(civiliansIndex);
			m_civilians.erase(m_civilians.begin() + civiliansIndex, m_civilians.begin() + civiliansIndex + 1);
			civiliansIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateArmySpawners(float deltaTime)
{
	for (size_t armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		m_armySpawners.at(armySpawnerIndex)->Update(deltaTime);
		if (m_armySpawners.at(armySpawnerIndex)->m_health <= 0)
		{
			DeleteFromStandAlonEntityList(m_armySpawners.at(armySpawnerIndex));
			delete m_armySpawners.at(armySpawnerIndex);
			m_armySpawners.erase(m_armySpawners.begin() + armySpawnerIndex, m_armySpawners.begin() + armySpawnerIndex + 1);
			armySpawnerIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates classA Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateClassAWarriors(float deltaTime)
{
	for (size_t classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		m_classAWarriors.at(classAWarriorIndex)->Update(deltaTime);
		if (m_classAWarriors.at(classAWarriorIndex)->m_health <= 0)
		{
			DeleteFromMovableEntityList(m_classAWarriors.at(classAWarriorIndex));
			delete m_classAWarriors.at(classAWarriorIndex);
			m_classAWarriors.erase(m_classAWarriors.begin() + classAWarriorIndex, m_classAWarriors.begin() + classAWarriorIndex + 1);
			classAWarriorIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates classB Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateClassBWarriors(float deltaTime)
{
	for (size_t classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		m_classBWarriors.at(classBWarriorIndex)->Update(deltaTime);
		if (m_classBWarriors.at(classBWarriorIndex)->m_health <= 0)
		{
			DeleteFromMovableEntityList(m_classBWarriors.at(classBWarriorIndex));
			delete m_classBWarriors.at(classBWarriorIndex);
			m_classBWarriors.erase(m_classBWarriors.begin() + classBWarriorIndex, m_classBWarriors.begin() + classBWarriorIndex + 1);
			classBWarriorIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates all houses
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateHouses(float deltaTime)
{
	for (size_t houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		m_houses.at(houseIndex)->Update(deltaTime);
		if (m_houses.at(houseIndex)->m_health <= 0)
		{
			DeleteFromStandAlonEntityList(m_houses.at(houseIndex));
			delete m_houses.at(houseIndex);
			m_houses.erase(m_houses.begin() + houseIndex, m_houses.begin() + houseIndex + 1);
			houseIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates all town centers
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateTownCenters(float deltaTime)
{
	for (size_t townCenterIndex = 0; townCenterIndex < m_townCenters.size(); townCenterIndex++)
	{
		m_townCenters.at(townCenterIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Updates resources
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateResources(float deltaTime)
{
	for (size_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		m_resources.at(resourceIndex)->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Updates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateExplosions(float deltaTime)
{
	for (int explosionIndex = 0; explosionIndex < m_explosions.size(); explosionIndex++)
	{
		Explosion *explosion = m_explosions.at(explosionIndex);
		explosion->Update(deltaTime);
		if(explosion->m_completed)
		{
			delete explosion;
			m_explosions.erase(m_explosions.begin() + explosionIndex, m_explosions.begin() + explosionIndex + 1);
			explosionIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/30
*@purpose : Updates debug entities
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateDebugEntities(float deltaTime)
{
	for (int explosionIndex = 0; explosionIndex < m_debugEntities.size(); explosionIndex++)
	{
		DebugEntity *explosion = m_debugEntities.at(explosionIndex);
		explosion->Update(deltaTime);
		if (explosion->m_isCompleted)
		{
			delete explosion;
			m_debugEntities.erase(m_debugEntities.begin() + explosionIndex, m_debugEntities.begin() + explosionIndex + 1);
			explosionIndex--;
		}
	}
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	DebugDraw::GetInstance()->DebugRenderLogf("GAME COUNTER %d", App::s_gameCounter);

	if(g_isCurrentlyTraining)
	{
		Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(Vector3(600,500,0), Vector3::RIGHT, Vector3::UP, "TRAINING ON", 50.f, Rgba::YELLOW);
		delete textMaterial;
	}

	RenderCivilians();
	RenderClassAWarriors();
	RenderClassBWarriors();
	RenderArmySpawners();
	RenderHouses();
	RenderTownCenters();
	RenderResources();
	RenderGrids();
	RenderExplosions();
	RenderDebugEntites();
	RenderHUDGameStat();
	RenderHUDUnitStat();
	RenderUnitTask();

	RenderCensoryValues();

	RenderMousePosition();

	if (m_gameFinished)
	{
		RenderWinState();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render Civilians
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderCivilians()
{
	for (size_t civilianIndex = 0; civilianIndex < m_civilians.size(); civilianIndex++)
	{
		m_civilians.at(civilianIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderArmySpawners()
{
	for (size_t armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		m_armySpawners.at(armySpawnerIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders classA Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderClassAWarriors()
{
	for (size_t classAWarriorsIndex = 0; classAWarriorsIndex < m_classAWarriors.size(); classAWarriorsIndex++)
	{
		m_classAWarriors.at(classAWarriorsIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders classB Warriors
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderClassBWarriors()
{
	for (size_t classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		m_classBWarriors.at(classBWarriorIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render houses
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderHouses()
{
	for (size_t houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		m_houses.at(houseIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Renders towncenters
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderTownCenters()
{
	for (size_t townCenterIndex = 0; townCenterIndex < m_townCenters.size(); townCenterIndex++)
	{
		m_townCenters.at(townCenterIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Render resources
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderResources()
{
	for (size_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		m_resources.at(resourceIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/22
*@purpose : render grids
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderGrids()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(float x = 0; x < (static_cast<float>(m_maxWidth) + 1)*g_unitDistance ;x += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(x, 0.f, 0.f), Vector3(x, (static_cast<float>(m_maxHeight) + 2)*g_unitDistance, 0));
	}
	for (float y = 0; y < (static_cast<float>(m_maxHeight) + 2)* g_unitDistance; y += g_unitDistance)
	{
		g_theRenderer->DrawLine(Vector3(0.f, y, 0.f), Vector3((m_maxWidth + 1.f)* g_unitDistance,y,0.f));
	}
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderExplosions()
{
	for(int explosionIndex = 0;explosionIndex < m_explosions.size();explosionIndex++)
	{
		m_explosions.at(explosionIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/30
*@purpose : Renders debug string
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderDebugEntites()
{
	for (int explosionIndex = 0; explosionIndex < m_debugEntities.size(); explosionIndex++)
	{
		m_debugEntities.at(explosionIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderHUDGameStat()
{
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawAABB(g_resourceStatHUD, Rgba::BLUE);

	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	TownCenter *populationATownCenter = m_townCenters.at(0);
	TownCenter *populationBTownCenter = nullptr;
	
	if(m_townCenters.size() > 1)
	{
		populationBTownCenter = m_townCenters.at(1);
	}

	g_theRenderer->DrawTextOn3DPoint(g_resourceFoodPositionPopulationA, Vector3::RIGHT, Vector3::UP , "FOOD  : " + ToString(populationATownCenter->m_resourceStat.m_food),  g_fontSize, Rgba::GREEN);																																   
	g_theRenderer->DrawTextOn3DPoint(g_resourceStonePositionPopulationA, Vector3::RIGHT, Vector3::UP, "STONE : " + ToString(populationATownCenter->m_resourceStat.m_stone), g_fontSize, Rgba::GREEN);																																   
	g_theRenderer->DrawTextOn3DPoint(g_resourceWoodPositionPopulationA, Vector3::RIGHT, Vector3::UP , "WOOD  : " + ToString(populationATownCenter->m_resourceStat.m_wood),  g_fontSize, Rgba::GREEN);

	g_theRenderer->DrawTextOn3DPoint(g_unitsCountPositionPopulationA, Vector3::RIGHT, Vector3::UP ,       "UNITS         : " + ToString(populationATownCenter->m_resourceStat.m_units),  g_fontSize, Rgba::GREEN);

	g_theRenderer->DrawTextOn3DPoint(g_unitsKilledCountPositionPopulationA, Vector3::RIGHT, Vector3::UP , "UNITS KILLED  : " + ToString(populationATownCenter->m_resourceStat.m_unitsKilled),  g_fontSize, Rgba::GREEN);

	g_theRenderer->DrawTextOn3DPoint(g_buildingCountPositionPopulationA, Vector3::RIGHT, Vector3::UP ,    "BUILDINGS     : " + ToString(populationATownCenter->m_resourceStat.m_buildings),  g_fontSize, Rgba::GREEN);

	if(populationBTownCenter != nullptr)
	{
		g_theRenderer->DrawTextOn3DPoint(g_resourceFoodPositionPopulationB, Vector3::RIGHT, Vector3::UP, "FOOD  : " + ToString(populationBTownCenter->m_resourceStat.m_food), g_fontSize, Rgba::RED);
		g_theRenderer->DrawTextOn3DPoint(g_resourceStonePositionPopulationB, Vector3::RIGHT, Vector3::UP, "STONE : " + ToString(populationBTownCenter->m_resourceStat.m_stone), g_fontSize, Rgba::RED);
		g_theRenderer->DrawTextOn3DPoint(g_resourceWoodPositionPopulationB, Vector3::RIGHT, Vector3::UP, "WOOD  : " + ToString(populationBTownCenter->m_resourceStat.m_wood), g_fontSize, Rgba::RED);
		g_theRenderer->DrawTextOn3DPoint(g_unitsCountPositionPopulationB, Vector3::RIGHT, Vector3::UP,       "UNITS         : " + ToString(populationBTownCenter->m_resourceStat.m_units), g_fontSize, Rgba::RED);

		g_theRenderer->DrawTextOn3DPoint(g_unitsKilledCountPositionPopulationB, Vector3::RIGHT, Vector3::UP, "UNITS KILLED  : " + ToString(populationBTownCenter->m_resourceStat.m_unitsKilled), g_fontSize, Rgba::RED);

		g_theRenderer->DrawTextOn3DPoint(g_buildingCountPositionPopulationB, Vector3::RIGHT, Vector3::UP,    "BUILDINGS     : " + ToString(populationBTownCenter->m_resourceStat.m_buildings), g_fontSize, Rgba::RED);

	}

	delete textMaterial;
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Render HUD
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderHUDUnitStat()
{
	if(g_currentSelectedEntity == nullptr)
	{
		return;
	}
	Material *defaultMaterial = Material::AquireResource("default");
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawAABB(g_unitStatHUD, Rgba::BLUE);

	
	switch (g_currentSelectedEntity->m_type)
	{
	case TOWN_CENTER:
		g_theRenderer->DrawAABB(g_unitStatHUDFirstButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDFirstButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "CREATE CIVILIAN ", g_fontSize, Rgba::YELLOW);


	case SHORT_RANGE_ARMY:
	case LONG_RANGE_ARMY:
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDHealthInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "HEALTH : " + ToString(g_currentSelectedEntity->m_health), g_fontSize, Rgba::YELLOW);
		break;
	case CIVILIAN:
	{
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDHealthInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "HEALTH : " + ToString(g_currentSelectedEntity->m_health), g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDFirstButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDFirstButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "BUILD ARMY UNIT ", g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDSecondButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDSecondButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "BUILD HOUSE ", g_fontSize, Rgba::YELLOW);


		Renderer::GetInstance()->BindMaterial(textMaterial);
		Entity *resource = ((Civilian*)g_currentSelectedEntity)->m_resourceTypeCarrying;
		if(resource == nullptr)
		{
			g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDResourceInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "NO RESOURCE", g_fontSize, Rgba::YELLOW);
		}
		else
		{
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDResourceInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, Entity::GetEntityTypeAsString(resource->m_type), g_fontSize, Rgba::YELLOW);
		}


		if (((Civilian*)g_currentSelectedEntity)->m_resourceTypeCarrying != nullptr)
		{
			g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDResourceInfoPosition, Vector3::RIGHT, Vector3::UP, Entity::GetEntityTypeAsString(((Civilian*)g_currentSelectedEntity)->m_resourceTypeCarrying->m_type), g_fontSize, Rgba::YELLOW);
		}
	}
		break;
	case HOUSE:
	case RESOURCE_FOOD:
	case RESOURCE_STONE:
	case RESOURCE_WOOD:
		break;
	case ARMY_SPAWNER:
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDHealthInfoPosition - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "HEALTH : "+ToString(g_currentSelectedEntity->m_health), g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDFirstButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDFirstButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "CREATE SHORT ARMY ", g_fontSize, Rgba::YELLOW);

		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawAABB(g_unitStatHUDSecondButton, Rgba::WHITE);
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDSecondButton.GetCenter() - Vector2(g_unitStatHUD.GetDimensions().x / 2.f - g_fontSize / 2.f, 0), Vector3::RIGHT, Vector3::UP, "CREATE LONG ARMY ", g_fontSize, Rgba::YELLOW);
		
		break;
	default:
		break;
	}

	
	delete textMaterial;
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/30
*@purpose : Renders task info of each unit
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderUnitTask()
{
	if (g_currentSelectedEntity == nullptr)
	{
		return;
	}
	Material *defaultMaterial = Material::AquireResource("default");
	Material *textMaterial    = Material::AquireResource("Data\\Materials\\text.mat");
	if (g_currentSelectedEntity->m_taskQueue.size() > 0)
	{
		Task *task          = g_currentSelectedEntity->m_taskQueue.front();
		IntVector2 cords	= GetCordinates(task->m_targetPosition);
		std::string taskStr = Task::GetTaskTypeAsString(task->m_taskType);
		std::string taskPos = (" POS X , Y " + ToString(cords.x) + " " + ToString(cords.y));
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDTaskInfoPosition - Vector3(g_unitStatHUD.GetDimensions().x, 0, 0), Vector3::RIGHT, Vector3::UP, taskStr, g_fontSize, Rgba::YELLOW);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDTaskInfoPosition - Vector3(g_unitStatHUD.GetDimensions().x, 100, 0), Vector3::RIGHT, Vector3::UP, taskPos, g_fontSize, Rgba::YELLOW);
	}
	else
	{
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(g_unitStatHUDTaskInfoPosition - Vector3(g_unitStatHUD.GetDimensions().x, 0, 0), Vector3::RIGHT, Vector3::UP, "NO TASK", g_fontSize, Rgba::YELLOW);
	}
	delete textMaterial;
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Renders the current mouse position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMousePosition()
{
	Vector2 mapPosition = GetMapPosition(m_currentTileIndex);
	//DebugDraw::GetInstance()->DebugRenderLogf("CurrentTile %d ", m_currentTileIndex);
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	g_theRenderer->DrawAABB(AABB2(mapPosition,g_radius,g_radius), Rgba::FADED_BLUE);
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : Renders win condition
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderWinState()
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	if(m_townCenters.at(0)->m_health <= 0)
	{
		g_theRenderer->DrawTextOn3DPoint(Windows::GetInstance()->GetDimensions().GetAsVector2()/2.f, Vector3::RIGHT, Vector3::UP, "TEAM 2 WINS ", g_fontSize, Rgba::YELLOW);
		return;
	}
	g_theRenderer->DrawTextOn3DPoint(Windows::GetInstance()->GetDimensions().GetAsVector2()/2.f, Vector3::RIGHT, Vector3::UP, "TEAM 1 WINS ", g_fontSize, Rgba::YELLOW);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/17
*@purpose : Render Censory values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderCensoryValues()
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	
	if (m_displaySensoryFoodValue)
	{
	}

	int tileIndex = GetTileIndex(m_mousePosition);
	if(tileIndex < 0 || tileIndex >= m_maxHeight*m_maxWidth)
	{
		return;
	}
	float resourceNearnessCurrentForFood  =  m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_FOOD);
	float resourceNearnessCurrentForStone =  m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_STONE);
	float resourceNearnessCurrentForWood  =  m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_RESOURCE_WOOD);

	float army1NearnessValue		      = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM1_ARMY_SHORT_RANGE);
	float army2NearnessValue			  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM2_ARMY_SHORT_RANGE);

	float longArmy1NearnessValue          = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM1_ARMY_LONG_RANGE);
	float longArmy2NearnessValue          = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM2_ARMY_LONG_RANGE);

	float building1NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM1_BUILDING);
	float building2NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM2_BUILDING);

	float armySpawner1NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM1_ARMYSPAWNER);
	float armySpawner2NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM2_ARMYSPAWNER);
										  
	float civilian1NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM1_CIVILIAN);
	float civilian2NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM2_CIVILIAN);
										  
	float townCenter1NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM1_TOWNCENTER);
	float townCenter2NearnessValue		  = m_cellSensoryValues.at(tileIndex).m_entityNearness.at((FavoredMoveStats)FAVORED_MOVETO_TEAM2_TOWNCENTER);



	if(m_displaySensoryFoodValue)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(resourceNearnessCurrentForFood), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryStoneValue)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(resourceNearnessCurrentForStone), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryWoodValue)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(resourceNearnessCurrentForWood), 5.f, Rgba::YELLOW);
	}
	// SENSORY VALUES
	if (m_displaySensoryShortRangeArmy1Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(army1NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryShortRangeArmy2Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(army2NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryLongRangeArmy1Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(longArmy1NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryLongRangeArmy2Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(longArmy2NearnessValue), 5.f, Rgba::YELLOW);
	}

	if (m_displaySensoryBuilding1Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(building1NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryBuilding2Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(building2NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryArmySpawner1Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(armySpawner1NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryArmySpawner2Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(armySpawner2NearnessValue), 5.f, Rgba::YELLOW);
	}

	if (m_displaySensoryCivilian1Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(civilian1NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryCivilian2Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(civilian2NearnessValue), 5.f, Rgba::YELLOW);
	}

	if (m_displaySensoryTC1Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(townCenter1NearnessValue), 5.f, Rgba::YELLOW);
	}
	if (m_displaySensoryTC2Value)
	{
		g_theRenderer->DrawTextOn3DPoint(GetMapPosition(tileIndex), Vector3::RIGHT, Vector3::UP, ToString(townCenter2NearnessValue), 5.f, Rgba::YELLOW);
	}
	///////////////////////////////////////////////////////////////
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Returns map mode as string
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Map::GetMapModeAsString(MapMode mode)
{
	switch (mode)
	{
	case MAP_MODE_TRAINING_CIVILIAN_GATHER_FOOD:
		return "TRAINING_CIVILIAN_GATHER_FOOD";
		break;
	case MAP_MODE_TRAINING_CIVILIAN_GATHER_ALL_RESOURCES:
		return "TRAINING_CIVILIAN_GATHER_ALL_RESOURCES";
		break;
	case MAP_MODE_TRAINING_CIVILIAN_BUILD:
		return "TRAINING_CIVILIAN_BUILD";
		break;
	case MAP_MODE_TRAINING_CIVILIAN:
		return "TRAINING_CIVILIAN";
		break;
	case MAP_MODE_TRAINING_TOWNCENTER:
		return "TRAINING_TOWNCENTER";
		break;
	case MAP_MODE_TRAINING_SHORTRANGE_ARMY:
		return "TRAINING_SHORTRANGE_ARMY";
		break;
	case MAP_MODE_TRAINING_LONGRANGE_ARMY:
		return "TRAINING_LONGRANGE_ARMY";
		break;
	case MAP_MODE_TRAINING_ARMYSPAWNER:
		return "TRAINING_ARMYSPAWNER";
		break;
	case MAP_MODE_TRAINING_NONE:
		return "TRAINING_NONE";
		break;
	case MAP_MODE_NUM_ITEMS:
		return "NUM_ITEMS";
		break;
	default:
		break;
	}
	return "INVALID";
}

