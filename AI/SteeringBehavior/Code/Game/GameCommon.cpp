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

bool				g_isQuitting	= false;
const int			g_maxWidth		= 32;
const int			g_maxHeight		= 20;
const float			g_unitDistance  = 50;
const float 		g_randomAngle   = 45;
const float			g_fontSize		= 12.f;
const float			g_deltaChangeInForce = 0.005f;
const float			g_maxArriveForce = 100.f;
const float			g_maxValue = 9999;

const std::string   g_textMaterialPath	  = "Data\\Materials\\text.mat";
const std::string   g_defaultMaterialPath = "default";
const std::string   COHESION			  = "COHESION";
const std::string   SEPARATION			  = "SEPARATION";
const std::string   ALIGNMENT			  = "ALIGNMENT";

Vector2				g_playerStart(100,100);
Vector2				g_demoPath1(0, 100);
Vector2				g_demoPath2(200, 200);
Vector2				g_demoPath3(400, 400);
Vector2				g_demoPath4(600, 600);
Vector2				g_demoPath5(800, 600);
Vector2				g_demoPath6(900, 500);
Vector2				g_demoPath7(1200, 300);
Vector2				g_demoPath8(1400, 200);
Vector2				g_demoPath9(1600, 100);
Vector2				g_demoPath10(1800, 100);

AABB2 g_cohesionNegativeBounds(Vector2(300, 950), 25, 25);
AABB2 g_cohesionPositiveBounds(Vector2(500, 950), 25, 25);
AABB2 g_cohesionValueBounds(Vector2(350, 950), 25, 25);
AABB2 g_cohesionStringValueBounds(Vector2(50, 950), 25, 25);

AABB2 g_seperationNegativeBounds(Vector2(300, 890), 25, 25);
AABB2 g_seperationPositiveBounds(Vector2(500, 890), 25, 25);
AABB2 g_seperationValueBounds(Vector2(350, 890), 25, 25);
AABB2 g_seperationStringValueBounds(Vector2(50, 890), 25, 25);

AABB2 g_alignmentNegativeBounds(Vector2(300, 830), 25, 25);
AABB2 g_alignmentPositiveBounds(Vector2(500, 830), 25, 25);
AABB2 g_alignmentValueBounds(Vector2(350, 830), 25, 25);
AABB2 g_alignmentStringValueBounds(Vector2(50, 830), 25, 25);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Init all uninitilaized variables
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitVariables()
{
	
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