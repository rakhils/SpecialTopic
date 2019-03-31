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
int					g_maxWidth		= 16;
int					g_maxHeight		= 16;
int					g_unitDistance  = 50;

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