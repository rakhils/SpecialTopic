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
#include "Engine/Core/Time.hpp"

#include "Game/GamePlay/Task/TaskMove.hpp"
#include "Game/GamePlay/Task/TaskIdle.hpp"
#include "Game/GamePlay/Task/TaskShortRangeAttack.hpp"
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
	std::string str = GetCurrentTimeMilliSecAsString();
	m_gameTime = 0.f;
	InitCamera();
	g_mapCounter = 0;
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
	case MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN:
		InitTrainedVsRandomTraining();
		break;
	case MAP_MODE_TRAINED_RED_VS_RANDOM_GREEN:
		InitTrainedVsRandomGame();
		break;
	case MAP_MODE_TRAINING_RED_VS_HUMAN_GREEN:
		InitTrainingForPlayGreen();
		break;
	case MAP_MODE_TRAINING_RANDOM_MAP_GEN:
		InitTrainingForRandomGenMaps();
		break;
	case MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN:
	default:
		InitTrainedRedVsHumanGreen();
		break;
	}

	m_gameStats.Reset();
	m_gameFinished = false;
	m_isScoreBoardUpdated = false;
	ResetAllScores();
	if(!m_firstTime)
	{
		return;
	}
	InitTiles();
	CreateDirectoryForNN();
	InitAndStoreBestScoreFromFile();
	
	m_firstTime = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTiles()
{
	if(m_tiles.size() > 0)
	{
		return;
	}
	for(int index = 0;index < m_maxHeight * m_maxWidth;index++)
	{
		Tile *tile = new Tile();
		m_tiles.push_back(tile);
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
	m_folder = "RANDOM_MAP";

	if(ShouldSaveLocalResultInDirectory())
	{
		time_t rawtime;
		char buffer[80];
		time(&rawtime);

		struct tm info;
		localtime_s(&info, &rawtime);
		strftime(buffer, 50, "%Y-%m-%d  %H'%M'%S", &info);
		m_folder = std::string(buffer);
	}

	System::CreateDirectoryFromPath(("Data\\NN\\" + m_folder).c_str());


	System::CreateDirectoryFromPath(("Data\\NN\\" + m_folder + "\\1\\").c_str());
	System::CreateDirectoryFromPath(("Data\\NN\\" + m_folder + "\\2\\").c_str());

	System::CreateDirectoryFromPath(("Data\\NN\\" + m_folder + "\\1\\BestStats\\").c_str());
	System::CreateDirectoryFromPath(("Data\\NN\\" + m_folder + "\\2\\BestStats\\").c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/27
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ResetAllScores()
{
	ResetLocalScores();
	ResetGlobalScores();
	m_localBestCivilianScoreCardTeam1		.Reset();
	m_localBestCivilianScoreCardTeam2		.Reset();
	m_localBestShortRangeArmyScoreCardTeam1	.Reset();
	m_localBestShortRangeArmyScoreCardTeam2	.Reset();
	m_localBestLongRangeArmyScoreCardTeam1	.Reset();
	m_localBestLongRangeArmyScoreCardTeam2	.Reset();
	m_localBestTownCenterScoreCardTeam1		.Reset();
	m_localBestTownCenterScoreCardTeam2		.Reset();
	if (m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN || m_mapMode == MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN)
	{
		return;
	}
	m_localBestCivilianScoreCardTeam1.ReadFromFile("Data\\NN\\BestGame\\1\\BestStats\\CIVILIAN_1_STATS.txt");
	m_localBestCivilianScoreCardTeam2.ReadFromFile("Data\\NN\\BestGame\\2\\BestStats\\CIVILIAN_2_STATS.txt");
	m_localBestShortRangeArmyScoreCardTeam1 .ReadFromFile("Data\\NN\\BestGame\\1\\BestStats\\SHORT_RANGE_ARMY_1_STATS.txt");
	m_localBestShortRangeArmyScoreCardTeam2 .ReadFromFile("Data\\NN\\BestGame\\2\\BestStats\\SHORT_RANGE_ARMY_2_STATS.txt");
	m_localBestLongRangeArmyScoreCardTeam1  .ReadFromFile("Data\\NN\\BestGame\\1\\BestStats\\LONG_RANGE_ARMY_1_STATS.txt");
	m_localBestLongRangeArmyScoreCardTeam2  .ReadFromFile("Data\\NN\\BestGame\\2\\BestStats\\LONG_RANGE_ARMY_2_STATS.txt");
	m_localBestTownCenterScoreCardTeam1		.ReadFromFile("Data\\NN\\BestGame\\1\\BestStats\\TOWN_CENTER_1_STATS.txt");
	m_localBestTownCenterScoreCardTeam2		.ReadFromFile("Data\\NN\\BestGame\\2\\BestStats\\TOWN_CENTER_2_STATS.txt");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/06
*@purpose : reads best score from file and store
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitAndStoreBestScoreFromFile()
{
	/*std::string strategyTeam1	   = Entity::GetStrategyAsString(m_team1Strategy);
	std::string strategyTeam2	   = Entity::GetStrategyAsString(m_team2Strategy);

	std::string fileContentTeam1	= GetFileContentAsString(("Data\\NN\\BestGame\\1\\BestStats\\TEAM_1_STATS.txt"));
	std::string fileContentTeam2	= GetFileContentAsString(("Data\\NN\\BestGame\\2\\BestStats\\TEAM_2_STATS.txt"));
	std::string armySpawnerTeam1	= GetFileContentAsString(("Data\\NN\\BestGame\\1\\BestStats\\ARMY_SPAWNER_1_STATS.txt"));
	std::string armySpawnerTeam2	= GetFileContentAsString(("Data\\NN\\BestGame\\2\\BestStats\\ARMY_SPAWNER_2_STATS.txt"));
	std::string civilianTeam1		= GetFileContentAsString(("Data\\NN\\BestGame\\1\\BestStats\\CIVILIAN_1_STATS.txt"));
	std::string civilianTeam2		= GetFileContentAsString(("Data\\NN\\BestGame\\2\\BestStats\\CIVILIAN_2_STATS.txt"));
	std::string shortRangeArmyTeam1 = GetFileContentAsString(("Data\\NN\\BestGame\\1\\BestStats\\SHORT_RANGE_ARMY_1_STATS.txt"));
	std::string shortRangeArmyTeam2 = GetFileContentAsString(("Data\\NN\\BestGame\\2\\BestStats\\SHORT_RANGE_ARMY_2_STATS.txt"));
	std::string longRangeArmyTeam1  = GetFileContentAsString(("Data\\NN\\BestGame\\1\\BestStats\\LONG_RANGE_ARMY_1_STATS.txt"));
	std::string longRangeArmyTeam2  = GetFileContentAsString(("Data\\NN\\BestGame\\2\\BestStats\\LONG_RANGE_ARMY_2_STATS.txt"));
	std::string townCenterTeam1		= GetFileContentAsString(("Data\\NN\\BestGame\\1\\BestStats\\TOWN_CENTER_1_STATS.txt"));
	std::string townCenterTeam2		= GetFileContentAsString(("Data\\NN\\BestGame\\2\\BestStats\\TOWN_CENTER_2_STATS.txt"));

	ToInt(fileContentTeam1.substr	(0, fileContentTeam1.find('\n')), &g_globalMaxScoreTeam1);
	ToInt(fileContentTeam2.substr	(0, fileContentTeam2.find('\n')), &g_globalMaxScoreTeam2);
	ToInt(armySpawnerTeam1.substr	(0, armySpawnerTeam1.find('\n')), &g_globalMaxScoreArmySpawnerTeam1);
	ToInt(armySpawnerTeam2.substr	(0, armySpawnerTeam2.find('\n')), &g_globalMaxScoreArmySpawnerTeam2);
	ToInt(civilianTeam1.substr		(0, civilianTeam1.find('\n')),    &g_globalMaxScoreCivilianTeam1);
	ToInt(civilianTeam2.substr		(0, civilianTeam2.find('\n')),     &g_globalMaxScoreCivilianTeam2);
	ToInt(shortRangeArmyTeam1.substr(0, shortRangeArmyTeam1.find('\n')), &g_globalMaxScoreShortRangeArmy1);
	ToInt(shortRangeArmyTeam2.substr(0, shortRangeArmyTeam2.find('\n')), &g_globalMaxScoreShortRangeArmy2);
	ToInt(longRangeArmyTeam1.substr (0, longRangeArmyTeam1.find('\n')), &g_globalMaxScoreLongRangeArmy1);
	ToInt(longRangeArmyTeam2.substr (0, longRangeArmyTeam2.find('\n')), &g_globalMaxScoreLongRangeArmy2);
	ToInt(townCenterTeam1.substr	(0, townCenterTeam1.find('\n')), &g_globalMaxScoreTownCenter1);
	ToInt(townCenterTeam2.substr	(0, townCenterTeam2.find('\n')), &g_globalMaxScoreTownCenter2);*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Inits camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	if(m_camera != nullptr)
	{
		return;
	}
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
/*DATE    : 2018/11/09
*@purpose : Restarts the current map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RestartMap()
{
	for(int index = 0;index < m_armySpawners.size();index++)
	{
		delete m_armySpawners.at(index);
	}
	for (int index = 0; index < m_civilians.size(); index++)
	{
		delete m_civilians.at(index);
	}
	for (int index = 0; index < m_classAWarriors.size(); index++)
	{
		delete m_classAWarriors.at(index);
	}
	for (int index = 0; index < m_classBWarriors.size(); index++)
	{
		delete m_classBWarriors.at(index);
	}
	for (int index = 0; index < m_houses.size(); index++)
	{
		delete m_houses.at(index);
	}
	for (int index = 0; index < m_townCenters.size(); index++)
	{
		delete m_townCenters.at(index);
		m_townCenters.at(index) = nullptr;
	}
	for (int index = 0; index < m_explosions.size(); index++)
	{
		delete m_explosions.at(index);
	}
	for (int index = 0; index < m_resources.size(); index++)
	{
		delete m_resources.at(index);
	}
	for (int index = 0; index < m_debugEntities.size(); index++)
	{
		delete m_debugEntities.at(index);
	}
	m_armySpawners			.clear();
	m_civilians				.clear();
	m_classAWarriors		.clear();
	m_classBWarriors		.clear();
	m_houses				.clear();
	m_townCenters			.clear();
	m_resources				.clear();
	m_explosions			.clear();
	m_debugEntities			.clear();
	m_movableEntities		.clear();
	m_standAloneEntities	.clear();
	m_entitiesHavingTraning .clear();
	Initialize();
	g_currentSelectedEntity = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IntVector2 Map::GetRandomFreePosition(IntVector2 positionStart, IntVector2 positionEnd)
{
	int loopVariable = 0;
	IntVector2 randomPosition;
	while(loopVariable < 10)
	{
		randomPosition = IntVector2(GetRandomIntInRange(positionStart.x, positionEnd.x), GetRandomIntInRange(positionStart.y,positionEnd.y));
		if(GetEntityFromPosition(randomPosition) == nullptr)
		{
			return randomPosition;
		}
		loopVariable++;
	}
	return randomPosition;
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
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_maxWidth  = g_mapMaxWidth;
	m_maxHeight = g_mapMaxHeight;

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
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);

	CreateTownCenter(GetMapPosition(55), 1);
	CreateTownCenter(GetMapPosition(7), 2);
	CreateResources(GetMapPosition(9), RESOURCE_FOOD);
	CreateResources(GetMapPosition(57), RESOURCE_STONE);
	CreateResources(GetMapPosition(62), RESOURCE_WOOD);


	CreateCivilian(GetMapPosition(17), 2);
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
	m_entitiesHavingTraning.clear();
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
	m_entitiesHavingTraning.clear();
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
	m_entitiesHavingTraning.push_back(TOWN_CENTER);
	CreateTownCenter(GetMapPosition(7), 1);
	CreateResources(GetMapPosition(40), RESOURCE_FOOD);

	CreateTownCenter(GetMapPosition(61), 2);
	//CreateCivilian(GetMapPosition(42), 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains short range army to attack and move
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainedVsRandomTraining()
{
	//g_isCurrentlyTraining = false;
	SetRandomSRAND();
	m_maxWidth = 40;
	m_maxHeight = 20;
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(LONG_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(ARMY_SPAWNER);
	m_entitiesHavingTraning.push_back(TOWN_CENTER);

	int randomFoodCount = GetRandomIntInRange(1, 3);
	int randomWoodCount = GetRandomIntInRange(1, 3);
	int randomStoneCount = GetRandomIntInRange(1, 3);

	IntVector2 randomTownCenterPosition1(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	IntVector2 randomTownCenterPosition2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));


	//randomTownCenterPosition1 = IntVector2(15, 10);
	//randomTownCenterPosition2 = IntVector2(25, 10);


	CreateTownCenter(GetMapPosition(randomTownCenterPosition1), 1);
	CreateTownCenter(GetMapPosition(randomTownCenterPosition2), 2);


	IntVector2 randomPosition(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_FOOD);
	randomPosition = IntVector2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_FOOD);

	randomPosition = IntVector2(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_STONE);
	randomPosition = IntVector2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_STONE);

	randomPosition = IntVector2(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_WOOD);
	randomPosition = IntVector2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_WOOD);


	IntVector2 randomCivilianPosition1(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	IntVector2 randomCivilianPosition2(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));


	for (int index = 0; index < 5; index++)
	{
		CreateClassAWarrior(GetMapPosition(IntVector2(15 + index, 15)), 1);
		CreateClassAWarrior(GetMapPosition(IntVector2(25 + index, 15)), 2);
	}


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains long range army to attack and move
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainedVsRandomGame()
{
	g_isCurrentlyTraining = false;
	SetRandomSRAND();
	m_maxWidth = 40;
	m_maxHeight = 20;
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(LONG_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(ARMY_SPAWNER);
	m_entitiesHavingTraning.push_back(TOWN_CENTER);

	int randomFoodCount = GetRandomIntInRange(1, 3);
	int randomWoodCount = GetRandomIntInRange(1, 3);
	int randomStoneCount = GetRandomIntInRange(1, 3);

	IntVector2 randomTownCenterPosition1(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	IntVector2 randomTownCenterPosition2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));


	//randomTownCenterPosition1 = IntVector2(15, 10);
	//randomTownCenterPosition2 = IntVector2(25, 10);


	CreateTownCenter(GetMapPosition(randomTownCenterPosition1), 1);
	CreateTownCenter(GetMapPosition(randomTownCenterPosition2), 2);


	IntVector2 randomPosition(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_FOOD);
	randomPosition = IntVector2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_FOOD);

	randomPosition = IntVector2(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_STONE);
	randomPosition = IntVector2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_STONE);

	randomPosition = IntVector2(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_WOOD);
	randomPosition = IntVector2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_WOOD);


	IntVector2 randomCivilianPosition1(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	IntVector2 randomCivilianPosition2(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));


	for (int index = 0; index < 5; index++)
	{
		CreateClassAWarrior(GetMapPosition(IntVector2(15 + index, 15)), 1);
		CreateClassAWarrior(GetMapPosition(IntVector2(25 + index, 15)), 2);
	}


	//CreateCivilian(GetMapPosition(randomCivilianPosition1), 1);
	//CreateCivilian(GetMapPosition(randomCivilianPosition2), 2);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Trains army spawner to spawn long range and short range armies
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForPlayGreen()
{
	g_isCurrentlyTraining = false;
	SetRandomSRAND();
	m_maxWidth = 40;
	m_maxHeight = 20;
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(LONG_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(ARMY_SPAWNER);
	m_entitiesHavingTraning.push_back(TOWN_CENTER);

	int randomFoodCount = GetRandomIntInRange(1, 3);
	int randomWoodCount = GetRandomIntInRange(1, 3);
	int randomStoneCount = GetRandomIntInRange(1, 3);

	IntVector2 randomTownCenterPosition1(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	IntVector2 randomTownCenterPosition2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));


	randomTownCenterPosition1 = IntVector2(15, 13);
	randomTownCenterPosition2 = IntVector2(27, 10);


	CreateTownCenter(GetMapPosition(randomTownCenterPosition1), 1);
	CreateTownCenter(GetMapPosition(randomTownCenterPosition2), 2);


	IntVector2 randomPosition = GetRandomFreePosition(IntVector2(0, 0), IntVector2(m_maxWidth / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_FOOD);
	randomPosition			  = GetRandomFreePosition(IntVector2(m_maxWidth / 2, 0), IntVector2(m_maxWidth, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_FOOD);

	randomPosition = GetRandomFreePosition(IntVector2(0, 0), IntVector2(m_maxWidth / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_STONE);
	randomPosition = GetRandomFreePosition(IntVector2(m_maxWidth / 2, 0), IntVector2(m_maxWidth, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_STONE);

	randomPosition = GetRandomFreePosition(IntVector2(0, 0), IntVector2(m_maxWidth / 2, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_WOOD);
	randomPosition = GetRandomFreePosition(IntVector2(m_maxWidth / 2, 0), IntVector2(m_maxWidth, m_maxHeight));
	CreateResources(GetMapPosition(randomPosition), RESOURCE_WOOD);


	IntVector2 randomCivilianPosition1(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	IntVector2 randomCivilianPosition2(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	CreateCivilian(GetMapPosition(randomCivilianPosition1), 1);
	CreateCivilian(GetMapPosition(randomCivilianPosition2), 2);

	for (int index = 0; index < 5; index++)
	{
		CreateClassAWarrior(GetMapPosition(IntVector2(15 + index, 15)), 1);
		CreateClassAWarrior(GetMapPosition(IntVector2(25 + index, 15)), 2);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainingForRandomGenMaps()
{
	SetRandomSRAND();
	m_maxWidth  = 40;
	m_maxHeight = 20;
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(LONG_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(ARMY_SPAWNER);
	m_entitiesHavingTraning.push_back(TOWN_CENTER);

	int randomCivilian1       = GetRandomIntLessThan(5);
	int randomShortRangeArmy1 = GetRandomIntInRange(5,10);
	int randomLongRangeArmy1 =  GetRandomIntLessThan(5);
	int randomArmySpawner1    = GetRandomIntLessThan(5);
	int randomHouses1		  = GetRandomIntLessThan(5);
													 
	int randomCivilian2		  = GetRandomIntLessThan(5);
	int randomShortRangeArmy2 = GetRandomIntLessThan(5);
	int randomLongRangeArmy2 =  GetRandomIntLessThan(5);
	int randomArmySpawner2    = GetRandomIntLessThan(5);
	int randomHouses2         = GetRandomIntLessThan(5);
								
	int randomFoodCount       = GetRandomIntLessThan(4);
	int randomWoodCount		  = GetRandomIntLessThan(4);
	int randomStoneCount      = GetRandomIntLessThan(4);

	IntVector2 randomTownCenterPosition1(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	IntVector2 randomTownCenterPosition2(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));

	Entity *entityTC1 = CreateTownCenter(GetMapPosition(randomTownCenterPosition1), 1);
	Entity *entityTC2 = CreateTownCenter(GetMapPosition(randomTownCenterPosition2), 2);

	//entityTC1->SetPosition(GetMapPosition(IntVector2(5, 5)));
	//entityTC1->SetPosition(GetMapPosition(IntVector2(5, 7)));

	entityTC1->m_health = static_cast<float>(GetRandomIntLessThan(50));
	entityTC2->m_health = static_cast<float>(GetRandomIntLessThan(50));
	entityTC1->m_healthLastFrame = entityTC1->m_health;
	entityTC2->m_healthLastFrame = entityTC2->m_health;

	m_townCenters.at(0)->m_resourceStat.m_food  = GetRandomIntLessThan(30);
	m_townCenters.at(0)->m_resourceStat.m_stone = GetRandomIntLessThan(30);
	m_townCenters.at(0)->m_resourceStat.m_wood  = GetRandomIntLessThan(30);

	m_townCenters.at(1)->m_resourceStat.m_food  = GetRandomIntLessThan(30);
	m_townCenters.at(1)->m_resourceStat.m_stone = GetRandomIntLessThan(30);
	m_townCenters.at(1)->m_resourceStat.m_wood  = GetRandomIntLessThan(30);

	for(int randomFoodCountIndex = 0;randomFoodCountIndex < randomFoodCount;randomFoodCountIndex++)
	{
		IntVector2 randomFoodPosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		CreateResources(GetMapPosition(randomFoodPosition), RESOURCE_FOOD);
	}
	for (int randomStoneCountIndex = 0; randomStoneCountIndex < randomStoneCount; randomStoneCountIndex++)
	{
		IntVector2 randomStonePosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		CreateResources(GetMapPosition(randomStonePosition), RESOURCE_STONE);
	}
	for (int randomWoodCountIndex = 0; randomWoodCountIndex < randomWoodCount; randomWoodCountIndex++)
	{
		IntVector2 randomWoodPosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		CreateResources(GetMapPosition(randomWoodPosition), RESOURCE_WOOD);
	}

	for (size_t armySpawner1Index = 0; armySpawner1Index < randomArmySpawner1; armySpawner1Index++)
	{
		IntVector2 randomArmySpawnerPosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity = CreateArmySpawner(GetMapPosition(randomArmySpawnerPosition), 1);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));

	}
	for (size_t house1Index = 0; house1Index < randomHouses1; house1Index++)
	{
		IntVector2 randomHouse1Position(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity = CreateHouse(GetMapPosition(randomHouse1Position), 1);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
	}

	for(int civilianIndex1 = 0;civilianIndex1 < randomCivilian1;civilianIndex1++)
	{
		IntVector2 randomCivilianPosition1(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity =CreateCivilian(GetMapPosition(randomCivilianPosition1), 1);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
		if(GetRandomBoolean())
		{
			float randomFloat = GetRandomFloatZeroToOne();
			if (randomFloat < 0.33)
			{
				((Civilian*)entity)->m_resourceTypeCarrying = GetResourceByType(RESOURCE_FOOD);
			}
			else if (randomFloat < 0.66)
			{
				((Civilian*)entity)->m_resourceTypeCarrying = GetResourceByType(RESOURCE_STONE);
			}
			else
			{
				((Civilian*)entity)->m_resourceTypeCarrying = GetResourceByType(RESOURCE_WOOD);
			}
		}
	}

	for (size_t shortRangeArmy1Index = 0; shortRangeArmy1Index < randomShortRangeArmy1; shortRangeArmy1Index++)
	{
		IntVector2 randomShortRangeArmy1Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity =CreateClassAWarrior(GetMapPosition(randomShortRangeArmy1Cords), 1);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
	}

	for (size_t longRangeArmy1Index = 0 ; longRangeArmy1Index < randomLongRangeArmy1;longRangeArmy1Index++)
	{
		IntVector2 randomLongRangeArmy1Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity = CreateClassBWarrior(GetMapPosition(randomLongRangeArmy1Cords), 1);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
	}
	
	for (int civilianIndex2 = 0; civilianIndex2 < randomCivilian2; civilianIndex2++)
	{
		IntVector2 randomCivilian2Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity =CreateCivilian(GetMapPosition(randomCivilian2Cords), 2);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
		if (GetRandomBoolean())
		{
			float randomFloat = GetRandomFloatZeroToOne();
			if (randomFloat < 0.33)
			{
				((Civilian*)entity)->m_resourceTypeCarrying = GetResourceByType(RESOURCE_FOOD);
			}
			else if (randomFloat < 0.66)
			{
				((Civilian*)entity)->m_resourceTypeCarrying = GetResourceByType(RESOURCE_STONE);
			}
			else
			{
				((Civilian*)entity)->m_resourceTypeCarrying = GetResourceByType(RESOURCE_WOOD);
			}
		}
	}

	for (size_t shortRangeArmy2Index = 0; shortRangeArmy2Index < randomShortRangeArmy2; shortRangeArmy2Index++)
	{
		IntVector2 randomShortRangeArmy2Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity =CreateClassAWarrior(GetMapPosition(randomShortRangeArmy2Cords), 2);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
		//entity->SetPosition(GetMapPosition(IntVector2(5, 6)));
	}

	for (size_t longRangeArmy2Index = 0; longRangeArmy2Index < randomLongRangeArmy2; longRangeArmy2Index++)
	{
		IntVector2 randomLongRangeArmy2Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity =CreateClassBWarrior(GetMapPosition(randomLongRangeArmy2Cords), 2);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
	}

	for (size_t armySpawner2Index = 0; armySpawner2Index < randomArmySpawner2; armySpawner2Index++)
	{
		IntVector2 randomArmySpawner2Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity =CreateArmySpawner(GetMapPosition(randomArmySpawner2Cords), 2);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
	}

	for (size_t house2Index = 0; house2Index < randomHouses2; house2Index++)
	{
		IntVector2 randomHouse2Cords(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		Entity *entity   = CreateHouse(GetMapPosition(randomHouse2Cords), 2);
		entity->m_health = static_cast<float>(GetRandomIntLessThan(10));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Initialize non training mode
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitTrainedRedVsHumanGreen()
{
	if(true)
	{
		InitTrainedVsRandomGame();
		return;
	}
	SetRandomSRAND();
	m_maxWidth = 40;
	m_maxHeight = 20;
	m_entitiesHavingTraning.clear();
	m_entitiesHavingTraning.push_back(CIVILIAN);
	m_entitiesHavingTraning.push_back(SHORT_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(LONG_RANGE_ARMY);
	m_entitiesHavingTraning.push_back(ARMY_SPAWNER);
	m_entitiesHavingTraning.push_back(TOWN_CENTER);

	int randomFoodCount  = GetRandomIntInRange(1, 3);
	int randomWoodCount  = GetRandomIntInRange(1, 3);
	int randomStoneCount = GetRandomIntInRange(1, 3);

	IntVector2 randomTownCenterPosition1(GetRandomIntInRange(0, m_maxWidth / 2), GetRandomIntInRange(0, m_maxHeight / 2));
	IntVector2 randomTownCenterPosition2(GetRandomIntInRange(m_maxWidth / 2, m_maxWidth), GetRandomIntInRange(m_maxHeight / 2, m_maxHeight));


	randomTownCenterPosition1 = IntVector2(15, 10);
	randomTownCenterPosition2 = IntVector2(25, 10);


	CreateTownCenter(GetMapPosition(randomTownCenterPosition1), 1);
	CreateTownCenter(GetMapPosition(randomTownCenterPosition2), 2);

	for (int randomFoodCountIndex = 0; randomFoodCountIndex < randomFoodCount; randomFoodCountIndex++)
	{
		IntVector2 randomFoodPosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		CreateResources(GetMapPosition(randomFoodPosition), RESOURCE_FOOD);
	}
	for (int randomStoneCountIndex = 0; randomStoneCountIndex < randomStoneCount; randomStoneCountIndex++)
	{
		IntVector2 randomStonePosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		CreateResources(GetMapPosition(randomStonePosition), RESOURCE_STONE);
	}
	for (int randomWoodCountIndex = 0; randomWoodCountIndex < randomWoodCount; randomWoodCountIndex++)
	{
		IntVector2 randomWoodPosition(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
		CreateResources(GetMapPosition(randomWoodPosition), RESOURCE_WOOD);
	}
	IntVector2 randomCivilianPosition1(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	IntVector2 randomCivilianPosition2(GetRandomIntLessThan(m_maxWidth), GetRandomIntLessThan(m_maxHeight));
	CreateCivilian(GetMapPosition(randomCivilianPosition1), 1);
	CreateCivilian(GetMapPosition(randomCivilianPosition2), 2);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Check if the map mode is non training
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::IsNonTrainingMode()
{
	if(m_mapMode == MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN)
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
	TownCenter *townCenter = m_townCenters.at(entity->m_teamID - 1);
	if (townCenter == nullptr)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINING_RED_VS_HUMAN_GREEN  && entity->m_teamID == 1)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN && entity->m_teamID == 1)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN  && entity->m_teamID == 1)
	{
		return false;
	}
	if(m_mapMode ==  MAP_MODE_TRAINED_RED_VS_RANDOM_GREEN   && entity->m_teamID == 1)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_RANDOM_GREEN && entity->m_teamID == 2)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN && entity->m_teamID == 2)
	{
		return false;
	}
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
/*DATE    : 2019/02/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasFeedForwardEnabled(Entity *entity)
{
	if (m_mapMode == MAP_MODE_TRAINING_RED_VS_HUMAN_GREEN && entity->m_teamID == 1)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN && entity->m_teamID == 1)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN && entity->m_teamID == 1)
	{
		return false;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_RANDOM_GREEN && entity->m_teamID == 1)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasRandomBehaviorEnabled(Entity *entity)
{
	if (m_mapMode == MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN && entity->m_teamID == 1)
	{
		return true;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_RANDOM_GREEN && entity->m_teamID == 1)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::HasHumanBehaviorEnabled(Entity *entity)
{
	if (m_mapMode == MAP_MODE_TRAINING_RED_VS_HUMAN_GREEN && entity->m_teamID == 1)
	{
		return true;
	}
	if (m_mapMode == MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN && entity->m_teamID == 1)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Create a civilian at position and with team id
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateCivilian(Vector2 position, int teamID)
{
	Civilian *civilian = new Civilian(this,position,teamID);
	m_civilians.push_back(civilian);
	m_movableEntities.push_back(civilian);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_civilians++;
	if(teamID == 1)
	{
		m_gameStats.m_numOfCiviliansTeam1++;
	}
	if (teamID == 2)
	{
		m_gameStats.m_numOfCiviliansTeam2++;
	}
	return civilian;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateArmySpawner(Vector2 position, int teamID)
{
	ArmySpawner * armySpawner = new ArmySpawner(this,position,teamID);
	m_armySpawners.push_back(armySpawner);
	m_standAloneEntities.push_back(armySpawner);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_armySpawners++;
	if (teamID == 1)
	{
		m_gameStats.m_numOfArmySpawnerTeam1++;
	}
	if (teamID == 2)
	{
		m_gameStats.m_numOfArmySpawnerTeam2++;
	}
	return armySpawner;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a class Warrior index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateClassAWarrior(Vector2 position, int teamID)
{
	ClassAWarrior *classAWarrior = new ClassAWarrior(this,position,teamID);
	m_classAWarriors.push_back(classAWarrior);
	m_movableEntities.push_back(classAWarrior);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_shortRangeArmies++;
	if (teamID == 1)
	{
		m_gameStats.m_numOfShortRangeArmyTeam1++;
	}
	if (teamID == 2)
	{
		m_gameStats.m_numOfShortRangeArmyTeam2++;
	}
	return classAWarrior;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates classB Warriors
*@param   : Position and teamID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateClassBWarrior(Vector2 position, int teamID)
{
	ClassBWarrior *classBWarrior = new ClassBWarrior(this,position,teamID);
	m_classBWarriors.push_back(classBWarrior);
	m_movableEntities.push_back(classBWarrior);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_units++;
	m_townCenters.at(teamID - 1)->m_resourceStat.m_longRangeArmies++;
	if (teamID == 1)
	{
		m_gameStats.m_numOfLongRangeArmyTeam1++;
	}
	if (teamID == 2)
	{
		m_gameStats.m_numOfLongRangeArmyTeam2++;
	}
	return classBWarrior;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a House
*@param   : Position and teamID
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateHouse(Vector2 position, int teamID)
{
	House *house = new House(this,position,teamID);
	m_houses.push_back(house);
	m_standAloneEntities.push_back(house);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
	if (teamID == 1)
	{
		m_gameStats.m_numOfHousesTeam1++;
	}
	if (teamID == 2)
	{
		m_gameStats.m_numOfHousesTeam2++;
	}
	return house;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates town center
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateTownCenter(Vector2 position, int teamID)
{
	TownCenter *townCenter = new TownCenter(this,position,teamID);
	m_townCenters.push_back(townCenter);
	m_standAloneEntities.push_back(townCenter);
	m_townCenters.at(teamID - 1)->m_resourceStat.m_buildings++;
	return townCenter;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Creates a resource item in map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity * Map::CreateResources(Vector2 position,EntityType type)
{
	Resource *resource = new Resource(this,position,type);
	m_resources.push_back(resource);
	return resource;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/26
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Map::GetResourceByType(EntityType type)
{
	for (size_t index = 0; index < m_resources.size(); index++)
	{
		if(m_resources.at(index)->m_type == type)
		{
			return m_resources.at(index);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Creates explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CreateExplosions(Vector2 position,Rgba color)
{
	if(g_skipRendering)
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
/*DATE    : 2019/02/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::ShouldSaveLocalResultInDirectory()
{
	if (!(m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN || m_mapMode == MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN || m_mapMode == MAP_MODE_TRAINING_RED_VS_HUMAN_GREEN))
	{
		return true;
	}
	return false;
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
/*DATE    : 2019/01/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<IntVector2> Map::GetAllNeighbouringCoordinates(IntVector2 tileCords, int distance)
{
	std::vector<IntVector2> neighbours;
	tileCords = ClampCoordinates(tileCords);
	IntVector2 tileCordsE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y + 0));
	IntVector2 tileCordsN = ClampCoordinates(IntVector2(tileCords.x + 0, tileCords.y + distance));
	IntVector2 tileCordsW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y + 0));
	IntVector2 tileCordsS = ClampCoordinates(IntVector2(tileCords.x + 0, tileCords.y - distance));

	IntVector2 tileCordsNE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y + distance));
	IntVector2 tileCordsNW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y + distance));
	IntVector2 tileCordsSW = ClampCoordinates(IntVector2(tileCords.x - distance, tileCords.y - distance));
	IntVector2 tileCordsSE = ClampCoordinates(IntVector2(tileCords.x + distance, tileCords.y - distance));

	for(int indexY = tileCordsS.y; indexY <= tileCordsN.y;indexY++)
	{
		for(int indexX = tileCordsW.x;indexX <= tileCordsE.x;indexX++)
		{
			IntVector2 cords(indexX, indexY);
			if(cords == tileCords)
			{
				continue;
			}
			neighbours.push_back(cords);
		}
	}
	return neighbours;
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
/*DATE    : 2019/01/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Map::GetRelativePosition(IntVector2 cords, IntVector2 targetCords, int maxCoverage)
{
	//int xDiff = targetCords.x - cords.x;
	//int yDiff = targetCords.y - cords.y;

	int minX = cords.x - maxCoverage;
	int minY = cords.y - maxCoverage;
	int maxX = cords.x + maxCoverage;
	int maxY = cords.y + maxCoverage;

	minX = ClampInt(minX, 0, m_maxWidth);
	minY = ClampInt(minY, 0, m_maxHeight);
	maxX = ClampInt(maxX, 0, m_maxWidth);
	maxY = ClampInt(maxY, 0, m_maxHeight);

	int targetDiffX = targetCords.x - minX;
	int targetDiffY = targetCords.y - minY;

	float relX = RangeMapFloat(static_cast<float>(targetDiffX), 0.f, static_cast<float>(maxCoverage) * 2.f, 0.f, 1.f);
	float relY = RangeMapFloat(static_cast<float>(targetDiffY), 0.f, static_cast<float>(maxCoverage) * 2.f, 0.f, 1.f);
	
	return Vector2(relX, relY);
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

bool Map::HasEnoughResourceToSpawnCivilian(int teamID)
{
	TownCenter *townCenter = m_townCenters.at(teamID - 1);
	if(townCenter->m_resourceStat.m_food >= g_minFoodCountToSpawnCivilian && townCenter->m_resourceStat.m_wood >= g_minWoodCountToSpawnCivilian)
	{
		return true;
	}
	return false;
}

bool Map::HasEnoughResourceToBuildHouse(int teamID)
{
	TownCenter *townCenter = m_townCenters.at(teamID - 1);
	if (townCenter->m_resourceStat.m_stone >= g_minStoneCountToBuildHouse && townCenter->m_resourceStat.m_wood >= g_minWoodCountToBuildHouse)
	{
		return true;
	}
	return false;
}

bool Map::HasEnoughResourceToBuildArmySpawnCenter(int teamID)
{
	TownCenter *townCenter = m_townCenters.at(teamID - 1);
	if (townCenter->m_resourceStat.m_stone > g_minStoneCountToBuildArmyCenter && townCenter->m_resourceStat.m_wood > g_minWoodCountToBuildArmyCenter)
	{
		return true;
	}
	return false;
}

bool Map::HasEnoughResourceToSpawnShortRangeArmy(int teamID)
{
	TownCenter *townCenter = m_townCenters.at(teamID - 1);
	if (townCenter->m_resourceStat.m_food >= g_minFoodCountToSpawnShortRangeArmy && townCenter->m_resourceStat.m_wood >= g_minWoodCountToSpawnShortRangeArmy)
	{
		return true;
	}
	return false;
}

bool Map::HasEnoughResourceToSpawnLongRangeArmy(int teamID)
{
	TownCenter *townCenter = m_townCenters.at(teamID - 1);
	if (townCenter->m_resourceStat.m_food >= g_minFoodCountToSpawnLongRangeArmy && townCenter->m_resourceStat.m_wood >= g_minWoodCountToSpawnLongRangeArmy)
	{
		return true;
	}
	return false;
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
	std::vector<IntVector2> neighbourCords = GetAllNeighbouringCoordinates(GetCordinates(mapPosition), distance);
	std::vector<Entity*> returnList;

	for (int index = 0; index < neighbourCords.size(); index++)
	{
		Entity *entity = GetEntityFromPosition(neighbourCords.at(index));
		if (entity != nullptr && entity->m_teamID != teamID && entity->m_teamID != 0)
		{
			returnList.push_back(entity);
		}
	}
	return returnList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Entity*> Map::GetAllAlliesNearLocation(int teamID, Vector2 mapPosition, int distance)
{
	std::vector<IntVector2> neighbourCords = GetAllNeighbouringCoordinates(GetCordinates(mapPosition), distance);
	std::vector<Entity*> returnList;

	for(int index = 0;index < neighbourCords.size();index++)
	{
		Entity *entity = GetEntityFromPosition(neighbourCords.at(index));
		if(entity != nullptr && entity->m_teamID == teamID)
		{
			returnList.push_back(entity);
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
Entity* Map::AttackOnPosition(int tileIndex,float damagePoint,int teamID)
{
	Entity *entity = GetEntityFromPosition(tileIndex);
	if(entity == nullptr)
	{
		return nullptr;
	}
	if(entity->m_teamID == teamID)
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
Entity* Map::AttackOnPosition(IntVector2 cords, float damagePoint,int teamID)
{
	int tileIndex = GetTileIndex(cords);
	return AttackOnPosition(tileIndex,damagePoint,teamID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : Attacks on position
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Entity* Map::AttackOnPosition(Vector2 position, float damagePoint,int teamID)
{
	int tileIndex = GetTileIndex(position);
	return AttackOnPosition(tileIndex, damagePoint,teamID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/14
*@purpose : Check if 2 entities are enemies
*@param   : 2 entities
*@return  : true if teamId not matched
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Map::AreEnemies(Entity *entityOne, Entity *entityTwo)
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
	if (g_skipRendering)
	{
		return;
	}
	UNUSED(deltaTime);
	m_mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	m_mousePosition.y = Windows::GetInstance()->GetDimensions().y - m_mousePosition.y;
	m_currentTileIndex = GetTileIndex(m_mousePosition);

	if(InputSystem::GetInstance()->WasRButtonJustPressed())
	{
		g_currentSelectedEntity = nullptr;
		m_selectedEntities.clear();
	}
	if(InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_P))
	{
		g_enableDebugPrints = g_enableDebugPrints == true ? false : true;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_S))
	{
		g_mapBreakCounter++;
		m_gameFinished = true;
		/*if (g_skipRendering)
		{
			g_skipRendering = false;
		}*/
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_T))
	{
		g_isCurrentlyTraining = g_isCurrentlyTraining == true ? false : true;
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
void Map::CheckAndUpdateOnWinCondition(float deltaTime)
{
	if(m_gameFinished)
	{
		m_gameFinishedTime += deltaTime;
		//g_skipRendering = false;
		if(m_gameFinishedTime > 1 || m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN)
		{
			m_gameFinishedTime = 0;
			CheckAndSaveBestStats();
		}
		return;
	}
	for(size_t townCenterIndex = 0;townCenterIndex < m_townCenters.size();townCenterIndex++)
	{
		if(m_townCenters.at(townCenterIndex)->m_health <= 0)
		{
			m_gameFinished = true;
			g_numOfGameCounter++;
			/*if(g_skipRendering)
			{
				g_skipRendering = false;
			}*/
			return;
		}
	}
	if (g_mapCounter > 150000)
	{
		g_mapCounter = 0;
		g_numOfGameCounter++;
		g_mapBreakCounter++;
		m_gameFinished = true;
		/*if (g_skipRendering)
		{
			g_skipRendering = false;
		}*/
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/01/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::CheckAndUpdateOnRandomMapGen(float deltaTime)
{
	UNUSED(deltaTime);
	if(m_mapMode == MAP_MODE_TRAINING_RANDOM_MAP_GEN)
	{
		if(m_counter %2 == 0)
		{
			g_mapBreakCounter++;
			m_gameFinished = true;
			CheckAndSaveBestStats();
			/*if (g_skipRendering)
			{
				g_skipRendering = false;
			}*/
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
	//Renderer::GetInstance()->TakeScreenShotAndSave("Data\\NN\\" + m_folder + "\\");
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
	m_team1ScoreCard.CalculateTotalScore();
	m_team2ScoreCard.CalculateTotalScore();

	if(ShouldSaveLocalResultInDirectory())
	{
		m_team1ScoreCard.SaveToFile(("Data\\NN\\" + m_folder + "\\1\\TEAM_STATS.txt").c_str());
		m_team2ScoreCard.SaveToFile(("Data\\NN\\" + m_folder + "\\2\\TEAM_STATS.txt").c_str());
	}
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
	if(entity != nullptr)
	{
		SaveEntityBestGlobalScore(entity);
	}
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
			if (teamID == m_civilians.at(index)->m_teamID)
			{
				m_civilians.at(index)->m_scoreBoard.CalculateTotalScore();
				if (m_civilians.at(index)->GetGlobalBestScore() < m_civilians.at(index)->m_scoreBoard.m_totalScore)
				{
					m_civilians.at(index)->SetGlobalBestScore(m_civilians.at(index)->m_scoreBoard.m_totalScore);
					returnIndex = static_cast<int>(index);
				}
			}
		}
		if(returnIndex >= 0)
		{
			return m_civilians.at(static_cast<size_t>(returnIndex));
		}
		break;
	case SHORT_RANGE_ARMY:
		for (size_t index = 0; index < m_classAWarriors.size(); index++)
		{
			if (teamID == m_classAWarriors.at(index)->m_teamID)
			{
				m_classAWarriors.at(index)->m_scoreBoard.CalculateTotalScore();
				if (m_classAWarriors.at(index)->GetGlobalBestScore() < m_classAWarriors.at(index)->m_scoreBoard.m_totalScore)
				{
					m_classAWarriors.at(index)->SetGlobalBestScore(m_classAWarriors.at(index)->m_scoreBoard.m_totalScore);
					returnIndex = static_cast<int>(index);
				}
			}
		}
		if (returnIndex >= 0)
		{
			return m_classAWarriors.at(static_cast<size_t>(returnIndex));
		}
		break;
	case LONG_RANGE_ARMY:
		for (size_t index = 0; index < m_classBWarriors.size(); index++)
		{
			m_classBWarriors.at(index)->m_scoreBoard.CalculateTotalScore();
			if (m_classBWarriors.at(index)->GetGlobalBestScore() < m_classBWarriors.at(index)->m_scoreBoard.m_totalScore)
			{
				m_classBWarriors.at(index)->SetGlobalBestScore(m_classBWarriors.at(index)->m_scoreBoard.m_totalScore);
				returnIndex = static_cast<int>(index);
			}
		}
		if (returnIndex >= 0)
		{
			return m_classBWarriors.at(static_cast<size_t>(returnIndex));
		}
		break;
	case TOWN_CENTER:
		for (size_t index = 0; index < m_townCenters.size(); index++)
		{
			m_townCenters.at(index)->m_scoreBoard.CalculateTotalScore();
			if (m_townCenters.at(index)->GetGlobalBestScore() < m_townCenters.at(index)->m_scoreBoard.m_totalScore)
			{
				m_townCenters.at(index)->SetGlobalBestScore(m_townCenters.at(index)->m_scoreBoard.m_totalScore);
				returnIndex = static_cast<int>(index);
			}
		}
		if (returnIndex >= 0)
		{
			return m_townCenters.at(static_cast<size_t>(returnIndex));
		}
		break;
	case ARMY_SPAWNER:
		for (size_t index = 0; index < m_armySpawners.size(); index++)
		{
			m_armySpawners.at(index)->m_scoreBoard.CalculateTotalScore();
			if (m_armySpawners.at(index)->GetGlobalBestScore() < m_armySpawners.at(index)->m_scoreBoard.m_totalScore)
			{
				m_armySpawners.at(index)->SetGlobalBestScore(m_armySpawners.at(index)->m_scoreBoard.m_totalScore);
				returnIndex = static_cast<int>(index);
			}
		}
		if (returnIndex >= 0)
		{
			return m_armySpawners.at(static_cast<size_t>(returnIndex));
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
	entity->SetGlobalBestScore(entity->m_scoreBoard.m_totalScore);
	entity->m_scoreBoard.SaveStatsToFile(entity->GetGlobalBestStatFilePath().c_str());
	if(ShouldSaveLocalResultInDirectory())
	{
		entity->m_scoreBoard.SaveStatsToFile(entity->GetLocalBestStatFilePath().c_str());
		entity->m_neuralNet.StoreToFile(entity->GetLocalBestFilePath().c_str());
	}

	std::string filePath = entity->GetGlobalBestFilePath();
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
	//FileWrite(entity->GetLocalBestStatFilePath(), ToString(entity->m_scoreBoard.m_totalScore));
	entity->m_scoreBoard.SaveToFile(entity->GetLocalBestStatFilePath().c_str());
	std::string filePath = entity->GetLocalBestFilePath();
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/15
*@purpose : Process inputs for selecting multiple entities
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputForMultipleSelection(float deltaTime)
{
	if (InputSystem::GetInstance()->WasLButtonJustPressed() && m_selectedEntities.size() == 0)
	{
		Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
		mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
		m_multipleEntitySelectionJustStarted = true;
		m_multipleEntitySelectionJustEnded   = false;
		m_multipleEntitySelectionStartPoint  = mousePosition;
		m_selectedEntities.clear();
	}

	if (InputSystem::GetInstance()->WasLButtonJustReleased())
	{
		if(m_multipleEntitySelectionJustStarted)
		{
			Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
			mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;
			m_multipleEntitySelectionJustStarted = false;
			m_multipleEntitySelectionJustEnded   = true;
			m_multipleEntitySelectionEndPoint  = mousePosition;
			SelectAllEntitiesUnderMouseSelection();
		}
	}

	if (InputSystem::GetInstance()->WasRButtonJustPressed())
	{
		m_selectedEntities.clear();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/15
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::SelectAllEntitiesUnderMouseSelection()
{
	for(int index = 0;index < m_classAWarriors.size();index++)
	{
		if(m_classAWarriors.at(index)->m_teamID == 2)
		{
			continue;
		}
		if(m_classAWarriors.at(index)->GetPosition().x > m_multipleEntitySelectionStartPoint.x && m_classAWarriors.at(index)->GetPosition().y < m_multipleEntitySelectionStartPoint.y)
		{
			if (m_classAWarriors.at(index)->GetPosition().x < m_multipleEntitySelectionEndPoint.x && m_classAWarriors.at(index)->GetPosition().y > m_multipleEntitySelectionEndPoint.y)
			{
				m_selectedEntities.push_back(m_classAWarriors.at(index));
			}
		}
	}
}

void Map::Update(float deltaTime)
{
	m_gameTime += deltaTime;
	m_counter++;
	g_mapCounter++;

	CheckAndUpdateOnWinCondition(deltaTime);
	CheckAndClearEntityOverlap();
	if(m_gameFinished)
	{
		return;
	}
	CheckAndUpdateOnRandomMapGen(deltaTime);

	ProcessInputs(deltaTime);
	UpdateMultipleSelectionEntityTasks(deltaTime);
	ProcessInputForMultipleSelection(deltaTime);

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
	UpdateEntities(deltaTime);
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
	if (g_skipRendering)
	{
		return;
	}
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
			if (m_civilians.at(civiliansIndex)->m_teamID == 1)
			{
				m_gameStats.m_numOfCiviliansTeam1--;
			}
			if (m_civilians.at(civiliansIndex)->m_teamID == 2)
			{
				m_gameStats.m_numOfCiviliansTeam2--;
			}
			delete m_civilians.at(civiliansIndex);
			m_civilians.at(civiliansIndex) = nullptr;
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
			if (m_armySpawners.at(armySpawnerIndex)->m_teamID == 1)
			{
				m_gameStats.m_numOfArmySpawnerTeam1--;
			}
			if (m_armySpawners.at(armySpawnerIndex)->m_teamID == 2)
			{
				m_gameStats.m_numOfArmySpawnerTeam2--;
			}
			delete m_armySpawners.at(armySpawnerIndex);
			m_armySpawners.at(armySpawnerIndex) = nullptr;
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
			ClassAWarrior *warrior = m_classAWarriors.at(classAWarriorIndex);
			TownCenter *townCenter = (TownCenter*)m_classAWarriors.at(classAWarriorIndex)->FindMyTownCenter();
			townCenter->m_resourceStat.m_shortRangeArmies--;

			DeleteFromMovableEntityList(m_classAWarriors.at(classAWarriorIndex));
			
			if (m_classAWarriors.at(classAWarriorIndex)->m_teamID == 1)
			{
				m_gameStats.m_numOfShortRangeArmyTeam1--;
			}
			if (m_classAWarriors.at(classAWarriorIndex)->m_teamID == 2)
			{
				m_gameStats.m_numOfShortRangeArmyTeam2--;
			}
			delete m_classAWarriors.at(classAWarriorIndex);
			m_classAWarriors.at(classAWarriorIndex) = nullptr;
			warrior = nullptr;
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
			TownCenter *townCenter = (TownCenter*)m_classBWarriors.at(classBWarriorIndex)->FindMyTownCenter();
			townCenter->m_resourceStat.m_longRangeArmies--;
			if (m_classBWarriors.at(classBWarriorIndex)->m_teamID == 1)
			{
				m_gameStats.m_numOfLongRangeArmyTeam1--;
			}
			if (m_classBWarriors.at(classBWarriorIndex)->m_teamID == 2)
			{
				m_gameStats.m_numOfLongRangeArmyTeam2--;
			}
			DeleteFromMovableEntityList(m_classBWarriors.at(classBWarriorIndex));
			delete m_classBWarriors.at(classBWarriorIndex);
			m_classBWarriors.at(classBWarriorIndex) = nullptr;
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
		
			if (m_houses.at(houseIndex)->m_teamID == 1)
			{
				m_gameStats.m_numOfHousesTeam1--;
			}
			if (m_houses.at(houseIndex)->m_teamID == 2)
			{
				m_gameStats.m_numOfHousesTeam2--;
			}
			delete m_houses.at(houseIndex);
			m_houses.at(houseIndex) = nullptr;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateEntities(float deltaTime)
{
	for (size_t houseIndex = 0; houseIndex < m_houses.size(); houseIndex++)
	{
		m_houses.at(houseIndex)->m_healthLastFrame = m_houses.at(houseIndex)->m_health;
	}

	for (size_t townCenterIndex = 0; townCenterIndex < m_townCenters.size(); townCenterIndex++)
	{
		m_townCenters.at(townCenterIndex)->m_healthLastFrame = m_townCenters.at(townCenterIndex)->m_health;
	}

	for (size_t classAWarriorIndex = 0; classAWarriorIndex < m_classAWarriors.size(); classAWarriorIndex++)
	{
		m_classAWarriors.at(classAWarriorIndex)->m_healthLastFrame = m_classAWarriors.at(classAWarriorIndex)->m_health;
	}

	for (size_t classBWarriorIndex = 0; classBWarriorIndex < m_classBWarriors.size(); classBWarriorIndex++)
	{
		m_classBWarriors.at(classBWarriorIndex)->m_healthLastFrame = m_classBWarriors.at(classBWarriorIndex)->m_health;
	}

	for (size_t armySpawnerIndex = 0; armySpawnerIndex < m_armySpawners.size(); armySpawnerIndex++)
	{
		m_armySpawners.at(armySpawnerIndex)->m_healthLastFrame = m_armySpawners.at(armySpawnerIndex)->m_health;
	}

	for (size_t civiliansIndex = 0; civiliansIndex < m_civilians.size(); civiliansIndex++)
	{
		m_civilians.at(civiliansIndex)->m_healthLastFrame = m_civilians.at(civiliansIndex)->m_health;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2019/02/15
*@purpose : Update task for multiple entities
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateMultipleSelectionEntityTasks(float deltaTime)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		for (int index = 0; index < m_selectedEntities.size(); index++)
		{
			if(m_selectedEntities.at(index) == nullptr || m_selectedEntities.at(index)->m_health <= 0)
			{
				continue;
			}


			int tileIndex = GetTileIndex(mousePosition);
			Entity *entity = GetEntityFromPosition(tileIndex);
			if (entity == nullptr)
			{
				m_selectedEntities.at(index)->ClearTaskQueue();
				Vector2 mapPosition = GetMapPosition(tileIndex);
				Task *task = new TaskMove(this, m_selectedEntities.at(index), mapPosition);
				m_selectedEntities.at(index)->m_taskQueue.push(task);
			}
			else if (entity != nullptr && AreEnemies(entity, m_selectedEntities.at(index)))// && IsInRange(m_map->GetCordinates(entity->GetPosition())))
			{
				m_selectedEntities.at(index)->ClearTaskQueue();
				Vector2 mapPosition = GetMapPosition(tileIndex);
				Task *task = new TaskShortRangeAttack(this, m_selectedEntities.at(index), GetTileIndex(mapPosition));
				if (task->m_isValid)
				{
					m_selectedEntities.at(index)->m_taskQueue.push(task);
				}
				else
				{
					delete task;
				}
			}
		}
	}
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	//DebugDraw::GetInstance()->DebugRenderLogf("GAME COUNTER %d MAP COUNTER %d MAP BREAK COUNTER %d", App::s_gameCounter,g_numOfGameCounter,g_mapBreakCounter);

	RenderTiles();
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
	RenderMultipleSelectionBox();

	if (m_gameFinished)
	{
		RenderWinState();
	}

	if (g_skipRendering)
	{
		Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
		Renderer::GetInstance()->BindMaterial(textMaterial);
		g_theRenderer->DrawTextOn3DPoint(Vector3(600, 500, 0), Vector3::RIGHT, Vector3::UP, "SKIP RENDERING", 50.f, Rgba::YELLOW);
		delete textMaterial;
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
/*DATE    : 2018/11/21
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderTiles()
{
	Rgba lightRed;
	lightRed.SetAsFloats(0.5f, 0.f, 0.f, 1.f);
	Rgba lightGreen;
	lightGreen.SetAsFloats( 0.f, 0.5f,0.f, 1.f);

	Rgba lightBlue;
	lightBlue.SetAsFloats(0.f, 0.f, 0.5f, 1.f);

	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	for(int index = 0;index < m_maxWidth*m_maxHeight;index++)
	{
		Vector2 mapPosition = GetMapPosition(index);
		AABB2 aabb(mapPosition, g_radius, g_radius);
		if(m_tiles.at(index)->m_covered > 0 )
		{
			if(m_tiles.at(index)->m_covered == 1)
			{
				g_theRenderer->DrawAABB(aabb, lightGreen);
			}
			if (m_tiles.at(index)->m_covered == 2)
			{
				g_theRenderer->DrawAABB(aabb, lightRed);
			}
			if (m_tiles.at(index)->m_covered == 3)
			{
				g_theRenderer->DrawAABB(aabb, lightBlue);
			}
			m_tiles.at(index)->m_covered = 0;
		}
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
	if (g_currentSelectedEntity == nullptr || g_currentSelectedEntity->m_health <= 0)
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
/*DATE    : 2019/02/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderMultipleSelectionBox()
{
	if (m_multipleEntitySelectionJustStarted)
	{
		AABB2 selectionBox(m_multipleEntitySelectionStartPoint, Vector2::ZERO);
		Material *defaultMaterial = Material::AquireResource("default");
		Renderer::GetInstance()->BindMaterial(defaultMaterial);
		g_theRenderer->DrawLine(m_multipleEntitySelectionStartPoint, Vector2(m_mousePosition.x,m_multipleEntitySelectionStartPoint.y));
		g_theRenderer->DrawLine(m_multipleEntitySelectionStartPoint, Vector2(m_multipleEntitySelectionStartPoint.x, m_mousePosition.y));
		g_theRenderer->DrawLine(Vector2(m_multipleEntitySelectionStartPoint.x,m_mousePosition.y), m_mousePosition);
		g_theRenderer->DrawLine(Vector2(m_mousePosition.x,m_multipleEntitySelectionStartPoint.y), m_mousePosition);

		delete defaultMaterial;
	}
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
*@purpose : Render sensory values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::RenderCensoryValues()
{
	
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
	case MAP_MODE_TRAINING_RED_VS_RANDOM_GREEN:
		return "TRAINING_RED_VS_RANDOM_GREEN";
		break;
	case MAP_MODE_TRAINED_RED_VS_RANDOM_GREEN:
		return "TRAINED_RED_VS_RANDOM_GREEN";
		break;
	case MAP_MODE_TRAINING_RED_VS_HUMAN_GREEN:
		return "TRAINING_RED_VS_HUMAN_GREEN";
		break;
	case MAP_MODE_TRAINED_RED_VS_HUMAN_GREEN:
		return "TRAINED_RED_VS_HUMAN_GREEN";
		break;
	case MAP_MODE_TRAINING_RANDOM_MAP_GEN:
		return "TRAINING_RANDOM_MAP_GEN";
		break;
	case MAP_MODE_NUM_ITEMS:
		return "NUM_ITEMS";
		break;
	default:
		break;
	}
	return "INVALID";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameStats::Reset()
{
	m_numOfArmySpawnerTeam1 = 0;
	m_numOfArmySpawnerTeam2 = 0;
	m_numOfCiviliansTeam1 = 0;
	m_numOfCiviliansTeam2 = 0;
	m_numOfShortRangeArmyTeam1 = 0;
	m_numOfShortRangeArmyTeam2 = 0;
	m_numOfLongRangeArmyTeam1 = 0;
	m_numOfLongRangeArmyTeam2 = 0;
	m_numOfHousesTeam1 = 0;
	m_numOfHousesTeam2 = 0;
}
