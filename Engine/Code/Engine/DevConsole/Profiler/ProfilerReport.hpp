#pragma once
#include <vector>
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/DevConsole/Profiler/Profiler.hpp"
#include "Engine/DevConsole/Profiler/ProfilerReportEntry.hpp"
/*\class  : ProfilerReporter
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 7/2/2018 7:31:35 PM
* \contact: srsrakhil@gmail.com
*/

class ProfilerReport
{

public:
	//Member_Variables
	ProfilerReportEntry*				 m_root = nullptr;
	
	//Static_Member_Variables

	//Methods

	ProfilerReport();
	~ProfilerReport();

	void	  GenerateTreeReportFromFrame(ProfileMeasurement_t *root);
	void	  GenerateFlatReportFromFrame(ProfileMeasurement_t *root);
	void	  Init();
	void	  Update(float deltaTime);
	void	  Render();

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