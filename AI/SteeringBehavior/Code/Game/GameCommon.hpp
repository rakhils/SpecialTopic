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
extern const int			g_maxWidth;
extern const int			g_maxHeight;
extern const float 			g_unitDistance;
extern const float			g_randomAngle;
extern const float			g_fontSize;
extern const float			g_deltaChangeInForce;
extern const float			g_maxArriveForce;
extern const float			g_maxValue;

extern Vector2				g_demoPath1;
extern Vector2				g_demoPath2;
extern Vector2				g_demoPath3;
extern Vector2				g_demoPath4;
extern Vector2				g_demoPath5;
extern Vector2				g_demoPath6;
extern Vector2				g_demoPath7;
extern Vector2				g_demoPath8;
extern Vector2				g_demoPath9;
extern Vector2				g_demoPath10;
extern Vector2			    g_playerStart;

extern AABB2 g_cohesionNegativeBounds;
extern AABB2 g_cohesionPositiveBounds;
extern AABB2 g_cohesionValueBounds;
extern AABB2 g_cohesionStringValueBounds;

extern AABB2 g_seperationNegativeBounds;
extern AABB2 g_seperationPositiveBounds;
extern AABB2 g_seperationValueBounds;
extern AABB2 g_seperationStringValueBounds;

extern AABB2 g_alignmentNegativeBounds;
extern AABB2 g_alignmentPositiveBounds;
extern AABB2 g_alignmentValueBounds;
extern AABB2 g_alignmentStringValueBounds;


extern const std::string   g_textMaterialPath;
extern const std::string   g_defaultMaterialPath;

extern const std::string   COHESION;
extern const std::string   SEPARATION;
extern const std::string   ALIGNMENT;

void InitVariables();
void QuitApp(Command &cmd);



