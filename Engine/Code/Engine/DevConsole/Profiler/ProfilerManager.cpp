#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/Windows.hpp"

Profiler*					  ProfilerManager::s_profiler		  = nullptr;
std::vector<ProfilerReport*>   ProfilerManager::s_profilerReports;
ReportUI					  ProfilerManager::s_report;
Graph						  ProfilerManager::s_performanceGraph;
int							  ProfilerManager::s_maxSample = g_profilerMaxSamples;
double						  ProfilerManager::s_fps;
double					      ProfilerManager::s_frameTime;
double						  ProfilerManager::s_prevTime;
double					      ProfilerManager::s_currentTime;
bool						  ProfilerManager::s_isPaused = false;
bool						  ProfilerManager::s_showSpecificReport = false;
int							  ProfilerManager::s_showReportIndex	= 0;
// CONSTRUCTOR
ProfilerManager::ProfilerManager()
{
	StartUp();
}

// DESTRUCTOR
ProfilerManager::~ProfilerManager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::StartUp()
{
	s_profiler		   = new Profiler();
	s_profiler->SetMaxSamples(g_profilerMaxSamples);

	s_report.AddReportColumn("FUNCTION NAME", 50);
	s_report.AddReportColumn("CALLS"		, 50);
	s_report.AddReportColumn("TOTAL%"		, 50);
	s_report.AddReportColumn("TOTAL_TIME"	, 50);
	s_report.AddReportColumn("SELF%"		, 50);
	s_report.AddReportColumn("SELF_TIME"	, 50);
	
	s_performanceGraph.SetBounds(AABB2(g_profilerTimeGraphPosition, g_profilerTimeGraphWidth, g_profilerTimeGraphHeight));
	s_performanceGraph.SetMaxSamplesCount(g_profilerMaxSamples);
	s_performanceGraph.SetMaxValue(g_profilerGraphMaxValue);
	s_performanceGraph.PopulateValues();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Sets max samples
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::SetMaxSamples(int samples)
{
	s_maxSample = samples;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Updates profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::Update(float deltaTime)
{
	UNUSED(deltaTime);
	s_currentTime = Clock::g_theMasterClock->total.m_seconds;
	double delta  = s_currentTime - s_prevTime;
	s_prevTime	  = s_currentTime;
	s_fps		  = static_cast<double>(1) / delta;
	s_frameTime   = delta;

	if (InputSystem::GetInstance()->WasRButtonJustPressed())
	{
		s_isPaused = false;
	}

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_P))
	{
		s_isPaused = s_isPaused ? false : true;
	}
	if(s_isPaused)
	{
		return;
	}

	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_V))
	{
		if (g_profilerView == TREE)
		{
			g_profilerView = FLAT;
		}
		else
		{
			g_profilerView = TREE;
		}
	}
	if (InputSystem::GetInstance()->WasLButtonJustPressed())
	{
		Vector2 postion = InputSystem::GetInstance()->GetMousePosition();
		if (s_performanceGraph.m_bounds.IsPointInside(postion))
		{
			//float Xvalue = postion.x - s_performanceGraph.m_bounds.mins.x;
			//float index = RangeMapFloat(Xvalue, 0, s_performanceGraph.m_bounds.GetDimensions().x, 0, s_profilerReports.size());
			//s_showSpecificReport = true;
			//s_isPaused = true;
			//s_showReportIndex = static_cast<int>(index);
		}
	}
	

	if(s_profiler->m_measurements.size() >= 1)
	{
		ProfileMeasurement_t* measurement = s_profiler->m_measurements.at(s_profiler->m_measurements.size() - 1);
		GenerateReport(measurement);
		ProfilerReport *lastReport = s_profilerReports.at(s_profilerReports.size() - 1);
		if (lastReport != nullptr)
		{
			s_performanceGraph.UpdateGraphPoints((lastReport->m_root->m_totalTimeInSec));
		}
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_J))
	{
		s_performanceGraph.UpdateAllPointsWithNewMax(s_performanceGraph.m_maxValue, s_performanceGraph.m_maxValue/2.f);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Renders profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::RenderProfiler()
{
	DebugDraw::GetInstance()->DebugRenderLogf("RENDERPROFILE");
	RenderInfo();
	RenderFPSAndFrameRate();
	Vector2 startPosition =  g_profilerUIStartPos;
	RenderAttributes();
	startPosition.y       -= g_profilerUILineSpace;


	if(g_profilerView == TREE)
	{
		/*if (s_showSpecificReport)
		{
			ProfilerReport *lastReport = s_profilerReports.at(s_showReportIndex);
			RenderTreeView(lastReport->m_root, startPosition, 0);
		}
		else*/
		if (s_profilerReports.size() > 0)
		{
			ProfilerReport *lastReport = s_profilerReports.at(s_profilerReports.size() - 1);
			RenderTreeView(lastReport->m_root, startPosition, 0);
		}
	}
	if (g_profilerView == FLAT)
	{
		if (s_profilerReports.size() > 0)
		{
			ProfilerReport *lastReport = s_profilerReports.at(s_profilerReports.size() - 1);
			RenderFlatView(lastReport->m_root, startPosition);
		}
	}
	s_performanceGraph.Render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : renders all attributes
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::RenderAttributes()
{
	Vector2 startPosition = g_profilerUIStartPos;
	Renderer *renderer = Renderer::GetInstance();
	int indent = static_cast<int>(g_profilerUIParaSpace);
	std::string headings = Stringf("%*s%-*s %-10s %-10s %-14s %-10s %-14s ", indent, "",
		(3 * indent), s_report.m_attribs.at(0).c_str(),
		s_report.m_attribs.at(1).c_str(),
		s_report.m_attribs.at(2).c_str(),
		s_report.m_attribs.at(3).c_str(),
		s_report.m_attribs.at(4).c_str(),
		s_report.m_attribs.at(5).c_str());

	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	renderer->BindMaterial(material);
	renderer->DrawTextOn3DPoint(Vector3(startPosition, 0), Vector3::RIGHT, Vector3::UP, headings, g_profilerUIFontSize, Rgba::WHITE);
	delete material;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Renders tree view of profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::RenderTreeView(ProfilerReportEntry *entry,Vector3 position,int depth)
{
	Renderer *renderer = Renderer::GetInstance();
	int indent = static_cast<int>(g_profilerUIParaSpace);
	std::string entryString = Stringf("%*s%-*s %-10s %-10s %-14s %-10s %-14s ", indent + depth, "",
		3*indent, entry->m_id.c_str(),
		ToString(entry->m_callCount).c_str(),
		ToString(entry->m_totalPercentTimeInSec).c_str(),
		ToString(entry->m_totalTimeInSec).c_str(),
		ToString(entry->m_selfPercentTimeInSec).c_str(),
		ToString(entry->m_selfTimeInSec).c_str());
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	renderer->BindMaterial(material);
	renderer->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, entryString, g_profilerUIFontSize, Rgba::WHITE);
	delete material;
	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = entry->m_children.begin(); it != entry->m_children.end(); it++)
	{
		position.y -= g_profilerUILineSpace;
		RenderTreeView(it->second, position, depth + 1);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::RenderFlatView(ProfilerReportEntry* root,Vector3 startPos)
{
	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = root->m_children.begin(); it != root->m_children.end(); it++)
	{
		ProfilerReportEntry* children = it->second;
		Renderer *renderer = Renderer::GetInstance();
		int indent = static_cast<int>(g_profilerUIParaSpace);
		std::string entryString = Stringf("%*s%-*s %-10s %-10s %-14s %-10s %-14s ", indent, "",
			(3*indent), children->m_id.c_str(),
			ToString(children->m_callCount).c_str(),
			ToString(children->m_totalPercentTimeInSec).c_str(),
			ToString(children->m_totalTimeInSec).c_str(),
			ToString(children->m_selfPercentTimeInSec).c_str(),
			ToString(children->m_selfTimeInSec).c_str());
		Renderer::GetInstance()->BindShader(Shader::GetDefaultShader());
		Shader::SetCurrentShader(Shader::GetDefaultShader());
		renderer->DrawTextOn3DPoint(startPos, Vector3::RIGHT, Vector3::UP, entryString, g_profilerUIFontSize, Rgba::WHITE);
		startPos.y -= g_profilerUILineSpace;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : renders fps and frame rate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::RenderFPSAndFrameRate()
{
	Vector2 screenDimensions = Windows::GetInstance()->GetDimensions().GetAsVector2();
	Vector3 position(Vector3(screenDimensions) - Vector3(300, 200, 0));
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "FPS :", g_profilerUIFontSize, Rgba::WHITE);
	position += Vector3(10 * g_profilerUIFontSize, 0, 0);
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, ToString(static_cast<int>(s_fps)), g_profilerUIFontSize, Rgba::WHITE);
	position += Vector3(-10 *g_profilerUIFontSize ,10 * g_profilerUIFontSize, 0);

	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "FRAME RATE :", g_profilerUIFontSize, Rgba::WHITE);
	position += Vector3(10 * g_profilerUIFontSize,0, 0);

	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, ToString(s_frameTime), g_profilerUIFontSize, Rgba::WHITE);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Renders all informations
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::RenderInfo()
{
	Vector3 position(g_profilerInfoStartPos, 0);
	Material *material = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(material);
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "P : PAUSE", g_profilerUIFontSize, Rgba::WHITE);
	position.y -= g_profilerUILineSpace;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "V : TOGGLE VIEW", g_profilerUIFontSize, Rgba::WHITE);
	position.y -= g_profilerUILineSpace;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "L : CHANGE SORT MODE", g_profilerUIFontSize, Rgba::WHITE);
	position.y -= g_profilerUILineSpace;
	Renderer::GetInstance()->DrawTextOn3DPoint(position, Vector3::RIGHT, Vector3::UP, "M : ENABLES MOUSE", g_profilerUIFontSize, Rgba::WHITE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::CreateReportFromPreviousFrames()
{
	for(int index = 0;index < s_profiler->m_measurements.size();index++)
	{
		ProfileMeasurement_t* measurement = s_profiler->m_measurements.at(index);
		GenerateReportTree(measurement);
		s_performanceGraph.UpdateGraphPoints((measurement->m_elapsedTimeInSec));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Marks the start of frame
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::MarkFrame()
{
	s_profiler->MarkFrame();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Pushes new profiler to the tail
*@param   : function name
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::PushProfiler(std::string name)
{
	s_profiler->PushProfiler(name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : remove the current profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::PoPProfiler()
{
	s_profiler->PoPProfiler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Generates report for the current measurement
*@param   : Measurement
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::GenerateReport(ProfileMeasurement_t* root)
{
	switch (g_profilerView)
	{
	case TREE:
		GenerateReportTree(root);
		break;
	case FLAT:
		GenerateReportFlat(root);
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Generates report tree for the current frame
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::GenerateReportTree(ProfileMeasurement_t *root)
{
	ProfilerReport *report = new ProfilerReport();
	report->GenerateTreeReportFromFrame(root);
	s_profilerReports.push_back(report);
	if(s_profilerReports.size() >= s_maxSample)
	{
		ProfilerReport *lastReport = s_profilerReports.at(0);
		delete lastReport;
		s_profilerReports.erase(s_profilerReports.begin(), s_profilerReports.begin() + 1);
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Generates flat report
*@param   : Root node for preparing report
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerManager::GenerateReportFlat(ProfileMeasurement_t* root)
{
	UNUSED(root);
	ProfilerReport *report = new ProfilerReport();
	report->GenerateFlatReportFromFrame(root);
	s_profilerReports.push_back(report);
	if (s_profilerReports.size() >= s_maxSample)
	{
		ProfilerReport *lastReport = s_profilerReports.at(0);
		delete lastReport;
		s_profilerReports.erase(s_profilerReports.begin(), s_profilerReports.begin() + 1);
	}
}
