#pragma once
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\TinyXml2\tinyxml2.h"
#include "Engine\DevConsole\DevConsole.hpp"
#include "Engine\DevConsole\Command.hpp"
#include "Engine\Time\TimeSystem.hpp"
#include "Engine\Audio\AudioSystem.hpp"
class Clock;
class Entity;
#define UNUSED(x) (void)(x);
using namespace tinyxml2;

extern Renderer*			g_theRenderer;
extern InputSystem*			g_theInput;
extern DevConsole*			g_theDevConsole;
extern TimeSystem*			g_theTimeSystem;
extern AudioSystem*			g_audio;
extern Clock*				g_theGameClock;

extern bool					g_isQuitting;
extern int					g_maxWidth;
extern int					g_maxHeight;
extern int					g_unitDistance;

void InitVariables();
void QuitApp(Command &cmd);



