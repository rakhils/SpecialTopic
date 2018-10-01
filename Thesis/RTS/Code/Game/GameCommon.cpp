#include "GameCommon.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"

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
int					g_mapMaxWidth	= 40;
int					g_mapMaxHeight	= 20;
bool				g_enableNeuralNet = true;
bool				g_enableDebugPrints = false;
int					g_extraNNInputs = 6;
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
