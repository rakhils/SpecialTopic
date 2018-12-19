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
float				g_counter		= 0;

float				g_playerWidth   = 50;
float				g_playerHeight  = 50;
float				g_bulletWidth   = 10;
float				g_bulletHeight  = 10;


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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Sends set of unrealiable msgs to a connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UnreliableTest(Command &cmd)
{
	int index = -1;
	int maxCount = 0;
	cmd.GetNextInt(&index);
	cmd.GetNextInt(&maxCount);
	Game::GetInstance()->m_netMsgMaxUnrealiableMsgCount = maxCount;
	Game::GetInstance()->m_netMsgConnectionIndex = index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Sends set of unrealiable msgs to a connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ReliableTest(Command &cmd)
{
	int index = -1;
	int maxCount = 0;
	cmd.GetNextInt(&index);
	cmd.GetNextInt(&maxCount);
	Game::GetInstance()->m_netMsgMaxUnrealiableMsgCount = maxCount;
	Game::GetInstance()->m_netMsgConnectionIndex = index;
	Game::GetInstance()->m_reliableMsg = true;
}
