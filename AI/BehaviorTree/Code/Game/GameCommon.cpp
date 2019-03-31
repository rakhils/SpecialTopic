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

float				g_attackRange   = 150;
float				g_alertRange    = 300;
float				g_retreatRange	= 300;

std::string			BTREE_PLAYER_LOCATION	= "PLAYER_LOCATION";
std::string		    BTREE_SPAWN_LOCATION	= "SPAWN_LOCATION";
std::string		    BTREE_ENEMY_LOCATION	= "ENEMY_LOCATION";
std::string		    BTREE_PURSUIT_LOCATION  = "PURSUIT_LOCATION";
std::string		    BTREE_WANDER_LOCATION	= "WANDER_LOCATION";
std::string		    BTREE_ATTACK_LOCATION   = "ATTACK_LOCATION";
std::string		    BTREE_ENEMY_STATE		= "ENEMY_STATE";
std::string		    BTREE_ATTACK_NODE_NAME  = "ATTACK";
std::string		    BTREE_MOVE_NODE_NAME    = "MOVE";
std::string			BTREE_SET_ATTACK_LOC_NODE_NAME = "SET_ATTACK_LOC";
std::string			BTREE_SET_WANDER_LOC_NODE_NAME = "SET_WANDER_LOC";

AABB2				g_gameMapExtends(Vector2::ZERO, Vector2(1920, 300));
float				g_attackInterval		= 2.5f;

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