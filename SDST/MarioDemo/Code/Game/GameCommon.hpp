#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\TinyXml2\tinyxml2.h"
#include "Engine\DevConsole\DevConsole.hpp"
#include "Engine\DevConsole\Command.hpp"
#include "Engine\Time\TimeSystem.hpp"
#include "Engine\Audio\AudioSystem.hpp"
class Map;
class Encounter;
class HumanController;
class Clock;
class Menu;
#define UNUSED(x) (void)(x);
using namespace tinyxml2;
extern Renderer*			g_theRenderer;
extern InputSystem*			g_theInput;
extern DevConsole*			g_theDevConsole;
extern Map*					g_theCurrentMap;
extern Encounter*			g_theCurrentEncounter;
extern TimeSystem*			g_theTimeSystem;
extern HumanController*		g_theHumanController;
extern Menu*				g_theCurrentMenu;
extern AudioSystem*			g_audio;
extern Clock*				g_theGameClock;
extern int					g_initialMarioCount;
extern int					g_maxMarioCountedForNextGen;
extern bool					g_controlMode;

extern bool g_isQuitting;

void QuitApp(Command &cmd);

