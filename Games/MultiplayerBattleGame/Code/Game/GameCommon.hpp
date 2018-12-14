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

struct Resources
{
	int m_food = 0;
	int m_wood = 0;
	int m_stone = 0;
	int m_buildings = 0;
	int m_units = 0;
	int m_unitsKilled = 0;

};

extern Renderer*			g_theRenderer;
extern InputSystem*			g_theInput;
extern DevConsole*			g_theDevConsole;
extern TimeSystem*			g_theTimeSystem;
extern AudioSystem*			g_audio;
extern Clock*				g_theGameClock;
extern Entity*				g_currentSelectedEntity;
extern Entity*				g_previousSelectedEntity;
extern float				g_radius;
extern float				g_unitDistance;
extern int					g_mapMaxWidth;
extern int					g_mapMaxHeight;
extern bool					g_isQuitting;
extern float				g_counter;

extern float				g_playerWidth;
extern float				g_playerHeight;
extern float				g_bulletWidth;
extern float				g_bulletHeight;


void InitVariables();
void QuitApp(Command &cmd);
void UnreliableTest(Command &cmd);
void ReliableTest(Command &cmd);



