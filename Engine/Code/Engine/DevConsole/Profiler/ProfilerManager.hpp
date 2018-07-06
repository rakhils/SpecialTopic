#pragma once
#include <queue>
#include "Engine/DevConsole/Profiler/Profiler.hpp"
#include "Engine/DevConsole/Profiler/ProfilerReport.hpp"
#include "Engine/DevConsole/Profiler/ProfilerConstants.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
/*\class  : ProfilerManager	   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 7/3/2018 1:20:57 AM
* \contact: srsrakhil@gmail.com
*/
struct Graph
{
	int			 m_maxNumSample		    = g_profilerMaxSamples;
	double       m_maxValue			    = g_profilerGraphMaxValue;
	double       m_minValue			    = g_profilerGraphMinValue;
	MeshBuilder* m_meshBuilder		    = nullptr;
	double       m_lastPointValue       = 0;
	float		 m_scaleFactor			= 1.f;
	AABB2        m_bounds;
	Matrix44	 m_modelMatrix;
	Graph()
	{
		m_meshBuilder	= new MeshBuilder();
		m_bounds		= AABB2(Vector2::ZERO, Vector2::ONE);
	}
	Graph(AABB2 bounds, int maxSamples)
	{
		m_bounds		= bounds;
		m_maxNumSample  = maxSamples;
		m_meshBuilder   = new MeshBuilder();
		m_modelMatrix.Translate3D(Vector3(bounds.GetCenter(), 0));
	}
	~Graph()
	{
		if(m_meshBuilder != nullptr)
		{
			delete m_meshBuilder;
		}
	}
	void Update(float detaTime)
	{
		UNUSED(detaTime);
	}
	void SetBounds(AABB2 bounds)
	{
		m_bounds = bounds;
		Vector3 position(m_bounds.GetCenter(), 0);
		position.x -= bounds.GetDimensions().x;
		m_modelMatrix.Translate3D(position);
	}
	void SetMaxSamplesCount(int count)
	{
		m_maxNumSample = count;
	}
	void SetMaxValue(float maxValue)
	{
		m_maxValue = maxValue;
	}
	void PopulateValues()
	{
		float Xposition = 0;//m_bounds.mins.x;
		float minY      = 0;//m_bounds.mins.y;
		float deltaXVal = m_bounds.GetDimensions().x / m_maxNumSample;
		m_meshBuilder->Begin(PRIMITIVE_TRIANGES, true);
		for (int index = 0; index < m_maxNumSample; index++)
		{
			float value1 = 0;
			float value2 = 0;
			Vector3 position1(Xposition, minY, 0);
			Vector3 position2(Xposition + deltaXVal, minY, 0);
			Vector3 position3(Xposition, value1, 0);
			Vector3 position4(Xposition + deltaXVal, value2, 0);

			Xposition += deltaXVal;
			int qindex = static_cast<int>(m_meshBuilder->m_vertices.size());
			MeshBuilder::CreateRect(*m_meshBuilder, position1, position2, position4, position3, Rgba::YELLOW);
			m_meshBuilder->AddQuadIndex(qindex, qindex + 1, qindex + 3, qindex + 2);
		}
		m_meshBuilder->End();
	}

	void UpdatePosition()
	{
		float deltaXVal = m_bounds.GetDimensions().x / m_maxNumSample;
		for (int vertexIndex = 0; vertexIndex < m_meshBuilder->m_vertices.size(); vertexIndex++)
		{
			m_meshBuilder->m_vertices.at(vertexIndex).m_position -= Vector3(deltaXVal, 0, 0);
		}
	}
	void UpdateGraphPoints(double pointX1)
	{
		UpdateGraphPoints(m_lastPointValue, pointX1);
		m_lastPointValue = pointX1;
	}
	void UpdateGraphPoints(double pointX1, double pointX2)
	{
		//UpdateScaleFactor(pointX2);
		pointX1			= RangeMap(pointX1, m_minValue, m_maxValue, 0, static_cast<double>(m_bounds.GetDimensions().y));
		pointX2			= RangeMap(pointX2, m_minValue, m_maxValue, 0, static_cast<double>(m_bounds.GetDimensions().y));
		pointX1			= ClampDouble(pointX1,0, static_cast<double>(m_bounds.GetDimensions().y));
		pointX2			= ClampDouble(pointX2,0, static_cast<double>(m_bounds.GetDimensions().y));

		float value1    = static_cast<float>(pointX1);
		float value2    = static_cast<float>(pointX2);
		float Xposition = m_bounds.maxs.x;
		float deltaXVal = m_bounds.GetDimensions().x / m_maxNumSample;
		DebugDraw::GetInstance()->DebugRenderLogf("GRAPH VALUE %f ",value2);
		Vector3 position1(Xposition,			 0,   0);
		Vector3 position2(Xposition + deltaXVal, 0,   0);
		Vector3 position3(Xposition,			 value1			,   0);
		Vector3 position4(Xposition + deltaXVal, value2			,   0);

		MeshBuilder::CreateRect(*m_meshBuilder, position1, position2, position4, position3, Rgba::YELLOW);

		m_meshBuilder->End();
		m_meshBuilder->RemoveVerticesFromBegin(4);
		UpdatePosition();
		//CheckForMaxAndResetScale();
	}
	
	void UpdateScaleFactor(double value)
	{
		if(value > m_maxValue)
		{
			UpdateAllPointsWithNewMax(m_maxValue, value);
		}
	}

	void UpdateAllPointsWithNewMax(double oldMax,double newMax)
	{
		/*for (int vertexIndex = 0; vertexIndex < m_meshBuilder->m_vertices.size(); vertexIndex++)
		{
			float value              = m_meshBuilder->m_vertices.at(vertexIndex).m_position.y;
			value				     = value - m_bounds.mins.y;
			float origValue          = RangeMap(value, 0, static_cast<double>(m_bounds.GetDimensions().y),m_minValue,oldMax);

			float newValue           = RangeMap(origValue, m_minValue, newMax, 0, static_cast<double>(m_bounds.GetDimensions().y));
			float value2			 = static_cast<float>(newValue) + m_bounds.mins.y;

			m_meshBuilder->m_vertices.at(vertexIndex).m_position.y = value2;
		}*/
		float currentScaleFactor = m_modelMatrix.Jy;
		float factor		     = static_cast<float>(oldMax / newMax);
		m_scaleFactor	         = currentScaleFactor*factor;
		//m_scaleFactor			 = ClampFloat(m_scaleFactor,0,1);
		m_modelMatrix.Jy         = m_scaleFactor;
		m_maxValue				 = newMax;

	}

	void CheckForMaxAndResetScale()
	{
		float maxValue = 0;
		for (int vertexIndex = 0; vertexIndex < m_meshBuilder->m_vertices.size(); vertexIndex++)
		{
			float value              = m_meshBuilder->m_vertices.at(vertexIndex).m_position.y;
			float origValue			 = RangeMapFloat(value, 0, m_bounds.GetDimensions().y, static_cast<float>(m_minValue), static_cast<float>(m_maxValue));
			origValue				 = origValue*m_scaleFactor;
			if(origValue > maxValue)
			{
				maxValue = origValue;
			}
		}
		if(maxValue == 0)
		{
			return;
		}
		if(m_maxValue > maxValue)
		{
			//DebugDraw::GetInstance()->DebugRenderLogf(1, "SCALING UP %f ",maxValue);
		}
		UpdateAllPointsWithNewMax(m_maxValue, maxValue);
	}

	void Render()
	{
		Mesh *mesh = m_meshBuilder->CreateMesh();
		Material *material = Material::AquireResource("default");
		Renderer::GetInstance()->BindMaterial(material);
		AABB2 bounds = m_bounds;
		bounds.mins.y += bounds.GetDimensions().y / 2;
		bounds.maxs.y += bounds.GetDimensions().y / 2;
		Renderer::GetInstance()->DrawRectangle(bounds);
		Renderer::GetInstance()->DrawMesh(mesh, m_modelMatrix);
		delete material;
		delete mesh;
	}
};
struct ReportUI
{
	std::vector<std::string> m_attribs;
	std::vector<float>		 m_tabSpace;
	void AddReportColumn(std::string name, float space)
	{
		m_attribs.push_back(name);
		m_tabSpace.push_back(space);
	}
};
class ProfilerManager
{

public:
	//Member_Variables
	static Profiler*					 s_profiler;
	static std::vector<ProfilerReport*>  s_profilerReports;
	static ReportUI					     s_report;
	static Graph						 s_performanceGraph;
	static int							 s_maxSample;
	static double						 s_fps;
	static double					     s_frameTime;
	static double						 s_currentTime;
	static double						 s_prevTime;
	static bool							 s_isPaused;

	static bool							 s_showSpecificReport;
	static int							 s_showReportIndex;
	//Static_Member_Variables

	//Methods

	ProfilerManager();
	~ProfilerManager();
	
	static void		StartUp();
	static void     SetMaxSamples(int samples);
	static void     Update(float deltaTime);
	static void     RenderProfiler();
	static void		RenderAttributes();
	static void     RenderTreeView(ProfilerReportEntry *entry,Vector3 position,int depth);
	static void     RenderFlatView(ProfilerReportEntry* root,Vector3 startPos);
	static void     RenderFPSAndFrameRate();
	static void		RenderInfo();
	static void     CreateReportFromPreviousFrames();
	
	static void		MarkFrame();
	static void		PushProfiler(std::string name);
	static void		PoPProfiler();
	static void     GenerateReport(ProfileMeasurement_t* root);
	static void		GenerateReportTree(ProfileMeasurement_t* root);
	static void     GenerateReportFlat(ProfileMeasurement_t* root);

	
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