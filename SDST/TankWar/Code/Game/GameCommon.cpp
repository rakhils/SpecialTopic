#include "GameCommon.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Renderer*			g_theRenderer = nullptr;
InputSystem*		g_theInput = nullptr;
DevConsole*			g_theDevConsole = nullptr;
Map*				g_theCurrentMap = nullptr;
TimeSystem*			g_theTimeSystem = nullptr;
Encounter*			g_theCurrentEncounter = nullptr;
HumanController*    g_theHumanController = nullptr;
AudioSystem*	    g_audio = nullptr;
//Clock*				g_theMasterClock		= nullptr;
Clock*				g_theGameClock = nullptr;

bool g_isQuitting = false;

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
