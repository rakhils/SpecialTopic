#include "Engine/EngineSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Logger/LogManager.hpp"
// CONSTRUCTOR
EngineSystem::EngineSystem()
{

}

// DESTRUCTOR
EngineSystem::~EngineSystem()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Starts up the engine
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::StartUp()
{
	RendererStartUp();
	DevConsoleStartUp();
	LoggerStartUp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Steps all engine components
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::Update(float deltaTime)
{
	ProfilerManager::PushProfiler("EngineSystem::Update");
	if(InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_F1))
	{
		ProfilerManager::CreateReportFromPreviousFrames();
		ProfilerManager::s_profilerEnabled = ProfilerManager::s_profilerEnabled ? false : true;
	}
	UpdateDebugDraws(deltaTime);
	UpdateDevConsoleSystem(deltaTime);
	ProfilerManager::PoPProfiler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::Render()
{
	ProfilerManager::PushProfiler("EngineSystem::Render");
	RenderDevConsoleSystem();
	ProfilerManager::PoPProfiler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Shuts down all engine components
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::ShutDown()
{
	LogManager::LogSystemShutdown();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Starts up rendering system
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::RendererStartUp()
{
	Renderer::GetInstance()->RenderStartup();
	Renderer::GetInstance()->PostStartup();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Starts up dev console
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::DevConsoleStartUp()
{
	DevConsole::StartUp();
	CommandStartup();
	ProfilerManager::StartUp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::LoggerStartUp()
{
	LogManager::LogSystemStartup();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Steps in devconsole
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::UpdateDevConsoleSystem(float deltaTime)
{
	if (DevConsole::GetInstance()->IsDevConsoleOpen())
	{
		DevConsole::GetInstance()->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Updates the profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::UpdateProfiler(float deltaTime)
{
	ProfilerManager::PushProfiler("EngineSystem::UpdateProfiler");
#if defined( PROFILER_ENABLED )
	if(ProfilerManager::s_profilerEnabled)
	{
		ProfilerManager::Update(deltaTime);
	}
#endif
	ProfilerManager::PoPProfiler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : Updates all debug draws
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::UpdateDebugDraws(float deltaTime)
{
	UNUSED(deltaTime);
	DebugDraw::Update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/03
*@purpose : renders dev console
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::RenderDevConsoleSystem()
{
	if (DevConsole::GetInstance()->IsDevConsoleOpen())
	{
		DevConsole::GetInstance()->Render();
		//RenderProfiler();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Renders profiler 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineSystem::RenderProfiler()
{
	ProfilerManager::PushProfiler("EngineSystem::RenderProfiler");
#if defined( PROFILER_ENABLED )
	if (ProfilerManager::s_profilerEnabled)
	{
		ProfilerManager::RenderProfiler();
	}
#endif
	ProfilerManager::PoPProfiler();

}