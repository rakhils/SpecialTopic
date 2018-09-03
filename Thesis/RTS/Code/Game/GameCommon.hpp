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
extern Entity*				g_currentSelectedEntity;
extern Entity*				g_previousSelectedEntity;
extern float				g_radius;
extern float				g_unitDistance;
extern int					g_mapMaxWidth;
extern int					g_mapMaxHeight;
extern bool					g_isQuitting;

//HUD
extern float				g_fontSize;
extern Vector3				g_resourceFoodPositionPopulationA;
extern Vector3				g_resourceStonePositionPopulationA;
extern Vector3				g_resourceWoodPositionPopulationA;
extern Vector3				g_resourceFoodPositionPopulationB;
extern Vector3				g_resourceStonePositionPopulationB;
extern Vector3				g_resourceWoodPositionPopulationB;
extern Vector3				g_unitTypePosition;
extern Vector3				g_unitHealthPosition;
extern AABB2				g_resourceStatHUD;
extern AABB2				g_unitStatHUD;
extern Vector3				g_unitStatHUDHealthInfoPosition;
extern Vector3				g_unitStatHUDResourceInfoPosition;
extern AABB2				g_unitStatHUDFirstButton;
extern AABB2				g_unitStatHUDSecondButton;


void InitVariables();
void QuitApp(Command &cmd);

