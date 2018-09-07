#pragma once

/*\class  : EngineSystem		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :	   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 7/3/2018 8:35:28 PM
* \contact: srsrakhil@gmail.com
*/
 
class EngineSystem
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	EngineSystem();
	~EngineSystem();
	
	static void StartUp();
	static void Update(float deltaTime);
	static void Render();
	static void ShutDown();

	static void RendererStartUp();
	static void DevConsoleStartUp();
	static void RCSStartUp();
	static void CommandSystemStartUp();
	static void LoggerStartUp();

	static void RCSShutDown();

	static void UpdateDevConsoleSystem(float deltaTime);
	static void UpdateProfiler(float deltaTime);
	static void UpdateDebugDraws(float deltaTime);

	static void RenderDevConsoleSystem();
	static void RenderProfiler();
	static void RenderDebugRenderSystem();
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};