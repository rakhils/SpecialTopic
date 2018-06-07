#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/DrawCall.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include <Engine/Renderer/Shader.hpp>
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
ForwardRenderingPath::ForwardRenderingPath()
{
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
ForwardRenderingPath::ForwardRenderingPath(Renderer *renderer)
{
	m_renderer = renderer;
}

// DESTRUCTOR
ForwardRenderingPath::~ForwardRenderingPath()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void ForwardRenderingPath::RenderSceneForCamera(Camera *camera,Scene *scene)
{
	Camera::SetCurrentCamera(camera);
	m_renderer->BeginFrame();

	if(scene->m_isSkyBoxEnabled)
	{
		Mesh *m_cube = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3(0, 0, 0), Vector3(1, 1, 1), Rgba::WHITE, FILL_MODE_FILL);
		Material *skyboxMaterial = Material::CreateOrGetMaterial1("Data\\Materials\\Skybox.mat", Renderer::GetInstance());
		Renderable *renderableSkyBox = new Renderable();
		renderableSkyBox->SetMaterial(skyboxMaterial);
		renderableSkyBox->SetMesh(m_cube);
		Renderer::GetInstance()->BindShader(skyboxMaterial->GetShader());
		Shader::SetCurrentShader(skyboxMaterial->GetShader());
		//m_renderer->BindMaterial(skyboxMaterial);
		Renderer::GetInstance()->BindCubeMapTexture(scene->m_texturecube, 8);
		Renderer::GetInstance()->DrawMesh(m_cube, renderableSkyBox->m_modelMatrix);
		delete renderableSkyBox;
	}

	std::vector<DrawCall> drawCalls;
	
	//Light::BindAllLightsToShader(m_renderer);
	for (int index = 0; index < scene->m_renderables.size(); index++)
	{
		DrawCall drawCall;
		drawCall.m_name		   = scene->m_renderables.at(index)->m_name;	
		drawCall.m_mesh        = scene->m_renderables.at(index)->m_mesh;
		drawCall.m_material    = scene->m_renderables.at(index)->m_material;
		drawCall.m_modelMatrix = scene->m_renderables.at(index)->m_modelMatrix;
		drawCall.m_sortOrder   = scene->m_renderables.at(index)->m_sortOrder;
		drawCalls.push_back(drawCall);
	}

	for (int indexi = 0; indexi < drawCalls.size(); indexi++)
	{
		for (int indexj = indexi + 1; indexj < drawCalls.size(); indexj++)
		{
			int sortOrderI = drawCalls.at(indexi).m_sortOrder;
			int sortOrderJ = drawCalls.at(indexj).m_sortOrder;
			if (sortOrderJ > sortOrderI)
			{
				DrawCall drawCallTemp = drawCalls.at(indexj);
				drawCalls.at(indexj) = drawCalls.at(indexi);
				drawCalls.at(indexi) = drawCallTemp;
			}
		}
	}

	for(int indexi = 0;indexi < drawCalls.size(); indexi++)
	{
		for(int	indexj = indexi + 1;indexj < drawCalls.size();indexj++)
		{
			if(drawCalls.at(indexi).m_sortOrder == drawCalls.at(indexj).m_sortOrder)
			{
				DrawCall drawCallI = drawCalls.at(indexi);
				DrawCall drawCallJ = drawCalls.at(indexj);
				if(drawCallI.m_material->m_shader->m_renderQueue != drawCallJ.m_material->m_shader->m_renderQueue)
				{
					if(drawCallJ.m_material->m_shader->m_renderQueue == RENDER_QUEUE_ALPHA)
					{
						DrawCall drawCallTemp = drawCalls.at(indexj);
						drawCalls.at(indexj) = drawCalls.at(indexi);
						drawCalls.at(indexi) = drawCallTemp;
					}
				}
			}
		}
	}

	for (int indexi = 0; indexi < drawCalls.size(); indexi++)
	{
		for (int indexj = indexi + 1; indexj < drawCalls.size(); indexj++)
		{
			DrawCall drawCallI = drawCalls.at(indexi);
			DrawCall drawCallJ = drawCalls.at(indexj);
			if (drawCallI.m_material->m_shader->m_renderQueue == drawCallJ.m_material->m_shader->m_renderQueue)
			{
				Vector3 distanceI = Camera::GetCurrentCamera()->m_transform.GetLocalPosition() - drawCallI.m_modelMatrix.GetTAxis();
				Vector3 distanceJ = Camera::GetCurrentCamera()->m_transform.GetLocalPosition() - drawCallJ.m_modelMatrix.GetTAxis();
				float depthZI = distanceI.GetLength();
				float depthZJ = distanceJ.GetLength();

				if (depthZJ > depthZI)
				{
					DrawCall drawCallTemp = drawCalls.at(indexj);
					drawCalls.at(indexj) = drawCalls.at(indexi);
					drawCalls.at(indexi) = drawCallTemp;
				}
			}
		}
	}

	// alpha opaque sort
	// z depth sort

	for (int index = 0; index < drawCalls.size(); index++)
	{
		m_renderer->BindMaterial(drawCalls.at(index).m_material);
		std::vector<Light*> lights = scene->GetMostContributingLights(8, drawCalls.at(index).m_modelMatrix.GetTAxis());//but not using exactly
		Light::BindAllLightsToShader(m_renderer,lights);
		DrawCall dc = drawCalls.at(index);
		if(drawCalls.at(index).m_mesh == nullptr)
		{
			Vector3 position = dc.m_modelMatrix.GetTAxis();
			m_renderer->DrawText2D(position.GetXY(), dc.m_name, 20, Rgba::WHITE, 1, nullptr);
		}
		else
		{
			m_renderer->DrawMesh(dc.m_mesh,dc.m_modelMatrix);
			GL_CHECK_ERROR();
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/25
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void ForwardRenderingPath::RenderScene(Scene *scene)
{
	for each (Camera *camera in scene->m_cameras)
	{
		RenderSceneForCamera(camera, scene);
	}
}
