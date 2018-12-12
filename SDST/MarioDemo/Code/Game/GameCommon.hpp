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
extern int					g_maxMarioCountForNextGen;
extern bool					g_controlMode;
extern std::string			g_neuralNetFilePath;
extern std::string			g_marioFitnessFilePath;
extern float				g_minYValueForDeath;

//NN
extern float				g_minNeuralNetValueForWalk;
extern float				g_minNeuralNetValueForJump;
extern bool					g_isCurrentlyTraining;

//GA
extern float				g_fitnessJumpFactor;
extern float				g_fitnessBonusScore;

// INIT
extern Vector3				g_startPosition;

// MINIMAP
extern float				g_minimapWidth;
extern float				g_minmapMarioValue;
extern float				g_minmapPitValue;
extern float				g_minmapBrickValue;

extern bool g_isQuitting;

void InitVariables();
void QuitApp(Command &cmd);

