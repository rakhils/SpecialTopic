#pragma once
#include "Engine/DevConsole/Profiler/ProfileLogScoped.hpp"
#define	PROFILER_ENABLED
#define PROFILE_LOG_SCOPE(tag) ProfileLogScoped __FUNCTION__ ##__LINE__ ## (tag)
enum PROFILER_VIEW
{
	FLAT,
	TREE
};
enum REPORT_SORT
{
	TOTAL,
	SELF,
	NONE
};

static int				g_profilerMaxSamples			= 150;

//GRAPH CONSTANTS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static float			g_profilerTimeGraphWidth		= 500.f;
static float			g_profilerTimeGraphHeight       = 100.f;
static Vector2			g_profilerTimeGraphPosition(1000.f, 900.f);
static float			g_profilerGraphMinValue         = 0.f;
static float			g_profilerGraphMaxValue		    = 0.350f;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//REPORT UI ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Vector2			g_profilerUIStartPos(0.f, 800.f);
static Vector2			g_profilerInfoStartPos(100.f, 300.f);
static float			g_profilerUILineSpace			= 20;
static float			g_profilerUIParaSpace			= 10;
static float			g_profilerUIFontSize			= 8;