#include "GameCommon.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"

Renderer*			g_theRenderer	= nullptr;
InputSystem*		g_theInput		= nullptr;
DevConsole*			g_theDevConsole = nullptr;
TimeSystem*			g_theTimeSystem = nullptr;
AudioSystem*	    g_audio			= nullptr;
Clock*				g_theGameClock  = nullptr;
Entity*				g_currentSelectedEntity = nullptr;
float				g_radius        = 20.f;
float				g_unitDistance  = g_radius * 2;
bool				g_isQuitting	= false;
int					g_mapMaxWidth	= 8;
int					g_mapMaxHeight	= 8;
int					g_hiddenLayerCount = 80;
bool				g_skipRendering = false;
bool				g_isCurrentlyTraining = true;
bool			    g_isGlobalyCurrentlyTraining = false;
float				g_lastTrainingStopTime = 0.f;
bool				g_enableNeuralNet = true;
bool				g_enableDebugPrints = false;
int					g_extraNNInputs = 10;
int					g_mapCounter = 0;
int					g_mapBreakCounter = 0;
int 				g_entityMiniMapMaxWidth  = 8;
int 				g_entityMiniMapMaxHeight = 8;

float				g_mapXOffset;
float				g_mapYOffset;

// MAIN MENU

float				g_mainMenuStartX = 300.f;
float				g_mainMenuStartY = 500.f;

// GAME PLAY

int				    g_maxResourceCountRequired = 15;
int				    g_maxHousesBuilt		   = 2;
int				    g_maxArmySpawnerBuilt	   = 2;
int				    g_maxUnits				   = 20;
int					g_maxCivilianCount		   = 5;
int					g_maxShortRangeArmyCount   = 5;
int					g_maxLongRangeArmyCount	   = 5;

int					g_minFoodCountToSpawnCivilian = 10;
int					g_minWoodCountToSpawnCivilian = 10;

int					g_minStoneCountToBuildArmyCenter = 8;
int					g_minWoodCountToBuildArmyCenter  = 8;

int					g_minStoneCountToBuildHouse = 5;
int					g_minWoodCountToBuildHouse  = 5;

int					g_minFoodCountToSpawnShortRangeArmy = 10;
int					g_minWoodCountToSpawnShortRangeArmy = 10;

int					g_minFoodCountToSpawnLongRangeArmy = 10;
int					g_minWoodCountToSpawnLongRangeArmy = 10;

// NN 
int					g_globalMaxScoreTeam1 = -1;
int					g_globalMaxScoreTeam2 = -1;
int					g_globalMaxScoreArmySpawnerTeam1 = -1;
int					g_globalMaxScoreArmySpawnerTeam2 = -1;
int					g_globalMaxScoreCivilianTeam1 = -1;
int					g_globalMaxScoreCivilianTeam2 = -1;
int					g_globalMaxScoreShortRangeArmy1 = -1;
int					g_globalMaxScoreShortRangeArmy2 = -1;
int					g_globalMaxScoreLongRangeArmy1 = -1;
int					g_globalMaxScoreLongRangeArmy2 = -1;
int					g_globalMaxScoreTownCenter1 = -1;
int					g_globalMaxScoreTownCenter2 = -1;

int					g_localMaxScoreTeam1			= -1;
int					g_localMaxScoreTeam2			= -1;
int					g_localMaxScoreArmySpawnerTeam1 = -1;
int					g_localMaxScoreArmySpawnerTeam2 = -1;
int					g_localMaxScoreCivilianTeam1	= -1;
int					g_localMaxScoreCivilianTeam2	= -1;
int					g_localMaxScoreShortRangeArmy1	= -1;
int					g_localMaxScoreShortRangeArmy2	= -1;
int					g_localMaxScoreLongRangeArmy1	= -1;
int					g_localMaxScoreLongRangeArmy2	= -1;
int					g_localMaxScoreTownCenter1		= -1;
int					g_localMaxScoreTownCenter2		= -1;

float				g_desiredOutputForSameRandomPosition        = 0.5f;
float				g_desiredOutputForRandomPosition            = 0.5f;

float				g_desiredOutputForSameCriticalPosition      = 0.5f;
float				g_desiredOutputForCriticalPosition	        = 0.75f;

float				g_desiredOutputForSameVeryCriticalPosition  = 0.f;
float				g_desiredOutputForVeryCriticalPosition      = 1.f;

int					g_minCellDistanceToStartCounting			= 5;

//HUD
float				g_fontSize							 = 10;
Vector3				g_resourceFoodPositionPopulationA	 (100, 1050, 0);
Vector3				g_resourceStonePositionPopulationA	 (100, 1025, 0);
Vector3				g_resourceWoodPositionPopulationA	 (100, 1000, 0);
Vector3				g_buildingCountPositionPopulationA   (400, 1050, 0);
Vector3             g_unitsCountPositionPopulationA      (400, 1025, 0);
Vector3             g_unitsKilledCountPositionPopulationA(400, 1000, 0);

Vector3				g_resourceFoodPositionPopulationB	 (800, 1050, 0);
Vector3				g_resourceStonePositionPopulationB	 (800, 1025, 0);
Vector3				g_resourceWoodPositionPopulationB	 (800, 1000, 0);
Vector3				g_buildingCountPositionPopulationB   (1100, 1050, 0);
Vector3             g_unitsCountPositionPopulationB      (1100, 1025, 0);
Vector3             g_unitsKilledCountPositionPopulationB(1100, 1000, 0);

Vector3				g_unitTypePosition					  (100, 100, 0);
Vector3				g_unitHealthPosition				  (400, 100, 0);
AABB2				g_resourceStatHUD(Vector2::ONE,Vector2::ONE);
AABB2				g_unitStatHUD(Vector2::ONE, Vector2::ONE);
Vector3				g_unitStatHUDHealthInfoPosition(Vector3::ONE);
Vector3				g_unitStatHUDResourceInfoPosition(Vector3::ONE);
Vector3				g_unitStatHUDTaskInfoPosition(Vector3::ONE);
AABB2				g_unitStatHUDFirstButton(Vector2::ONE ,Vector2::ONE);
AABB2				g_unitStatHUDSecondButton(Vector2::ONE,Vector2::ONE);

void ResetLocalScores()
{
	g_localMaxScoreTeam1 = -1;
	g_localMaxScoreTeam2 = -1;
	g_localMaxScoreArmySpawnerTeam1 = -1;
	g_localMaxScoreArmySpawnerTeam2 = -1;
	g_localMaxScoreCivilianTeam1 = -1;
	g_localMaxScoreCivilianTeam2 = -1;
	g_localMaxScoreShortRangeArmy1 = -1;
	g_localMaxScoreShortRangeArmy2 = -1;
	g_localMaxScoreLongRangeArmy1 = -1;
	g_localMaxScoreLongRangeArmy2 = -1;
	g_localMaxScoreTownCenter1 = -1;
	g_localMaxScoreTownCenter2 = -1;
}

void ResetGlobalScores()
{
	g_globalMaxScoreTeam1 = -1;
	g_globalMaxScoreTeam2 = -1;
	g_globalMaxScoreArmySpawnerTeam1 = -1;
	g_globalMaxScoreArmySpawnerTeam2 = -1;
	g_globalMaxScoreCivilianTeam1 = -1;
	g_globalMaxScoreCivilianTeam2 = -1;
	g_globalMaxScoreShortRangeArmy1 = -1;
	g_globalMaxScoreShortRangeArmy2 = -1;
	g_globalMaxScoreLongRangeArmy1 = -1;
	g_globalMaxScoreLongRangeArmy2 = -1;
	g_globalMaxScoreTownCenter1 = -1;
	g_globalMaxScoreTownCenter2 = -1;
}

int					g_counter = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Init all uninitilaized variables
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitVariables()
{
	float windowsSizeX					= Windows::GetInstance()->GetDimensions().GetAsVector2().x;
	float windowsSizeY					= Windows::GetInstance()->GetDimensions().GetAsVector2().y;
	g_mapXOffset					    = 0.f;
	g_mapYOffset						= 2 * g_radius;

	g_resourceStatHUD					= AABB2(Vector2(windowsSizeX / 2.f,windowsSizeY - 50), windowsSizeX / 2.f,50);

	g_unitStatHUD						= AABB2(Vector2(windowsSizeX - 160, windowsSizeY / 2.f), 160, windowsSizeY/2.f);
	g_unitStatHUDHealthInfoPosition		= Vector3(windowsSizeX - 160, windowsSizeY - 100, 0);
	g_unitStatHUDResourceInfoPosition   = Vector3(windowsSizeX - 150, windowsSizeY - 200, 0);
	g_unitStatHUDTaskInfoPosition		= Vector3(windowsSizeX - 150, windowsSizeY - 300,0);
	g_unitStatHUDFirstButton			= AABB2(Vector2(windowsSizeX - 160, windowsSizeY - 500),g_unitDistance*4,g_unitDistance);
	g_unitStatHUDSecondButton			= AABB2(Vector2(windowsSizeX - 160, windowsSizeY - 750),g_unitDistance*4,g_unitDistance);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Quit Application
*@param   : Command from console
*@return  : NIL
*//////////////////////////////////////////////////////////////
void QuitApp(Command &cmd)
{
	UNUSED(cmd);
	g_isQuitting = true;
}