#include "GameCommon.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Renderer*			g_theRenderer				= nullptr;
InputSystem*		g_theInput					= nullptr;
DevConsole*			g_theDevConsole				= nullptr;
Map*				g_theCurrentMap				= nullptr;
TimeSystem*			g_theTimeSystem				= nullptr;
Encounter*			g_theCurrentEncounter		= nullptr;
HumanController*    g_theHumanController		= nullptr;
AudioSystem*	    g_audio						= nullptr;
Clock*				g_theGameClock				= nullptr;
int					g_initialMarioCount			= 100;
int					g_maxMarioCountForNextGen   = 75;
bool				g_isQuitting				= false;
float				g_minYValueForDeath			= 115;



bool				g_controlMode = false;
std::string			g_neuralNetFilePath = "Data\\NeuralNet.txt";
std::string			g_marioFitnessFilePath = "Data\\MarioFitness.txt";

//NN
float				g_minNeuralNetValueForWalk  = 0.5f;
float				g_minNeuralNetValueForJump  = 0.5f;
bool				g_isCurrentlyTraining		= false;

//GA
float				g_fitnessJumpFactor			= 0.000000025f;
float				g_fitnessBonusScore			= 1000;


//INIT
Vector3				g_startPosition;


// MINIMAP
float				g_minimapWidth		 = 50.f;
float				g_minmapBrickValue   = 0.5f;
float				g_minmapPitValue	 = 0.75f;
float				g_minmapMarioValue   = 1.f;

void InitVariables()
{
	g_startPosition = Vector3(200, 200, 0);
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
