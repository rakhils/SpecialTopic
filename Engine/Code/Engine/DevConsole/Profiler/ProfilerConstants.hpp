#pragma once
#define	PROFILER_ENABLED
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
static bool				g_profilerEnabled				= false;
static PROFILER_VIEW	g_profilerView;
static REPORT_SORT		g_profilerReportSortType;
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