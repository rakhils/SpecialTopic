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
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Shaders/ShaderProgramInfo.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
#include "ShaderDefinitions.hpp"
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
	ProfilerManager::PushProfiler("ForwardRenderingPath::RenderSceneFromCamera");

	Camera::SetCurrentCamera(camera);
	m_renderer->BeginFrame();

	for each(Light *light in scene->m_lights) 
	{
		if (light->m_type != AMBIENT_LIGHT && light->m_isShadowCasting)
		{
			RenderShadowCastingObjects(light, scene);
		}
	}

	Camera::SetCurrentCamera(camera);
	//m_renderer->BeginFrame();
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
				Vector3 distanceI = Camera::GetCurrentCamera()->m_transform.GetLocalPosition() - drawCallI.m_modelMatrix.GetTVector();
				Vector3 distanceJ = Camera::GetCurrentCamera()->m_transform.GetLocalPosition() - drawCallJ.m_modelMatrix.GetTVector();
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
		//Shader *shader = Shader::GetCurrentShader();
		Renderer::GetInstance()->BindTexture(9, m_shadowCamera->m_defaultFrameBuffer->m_depth_stencil_target);
		Renderer::GetInstance()->BindSampler(9, Sampler::CreateShadowSampler());
		std::vector<Light*> lights = scene->GetMostContributingLights(8, drawCalls.at(index).m_modelMatrix.GetTVector());
		Light::BindAllLightsToShader(m_renderer,lights);
		DrawCall dc = drawCalls.at(index);
		m_renderer->DrawMesh(dc.m_mesh,dc.m_modelMatrix);
	}
	ProfilerManager::PoPProfiler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/23
*@purpose : Renders shadow casting objects
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ForwardRenderingPath::RenderShadowCastingObjects(Light *light, Scene *scene)
{
	if(m_shadowCamera == nullptr)
	{
		m_shadowCamera = new OrthographicCamera();
		Texture* shadowTex = new Texture();
		Texture* colorTest = new Texture();
		colorTest->CreateRenderTarget(2048, 2048, TEXTURE_FORMAT_RGBA8);
		shadowTex->CreateRenderTarget(2048, 2048, TEXTURE_FORMAT_D24S8);
		light->m_shadowTexture = shadowTex;
		FrameBuffer *cFrameBuffer = new FrameBuffer();
		m_shadowCamera->m_defaultFrameBuffer = cFrameBuffer;
		m_shadowCamera->SetColorTarget(nullptr);
		m_shadowCamera->SetDepthStencilTarget(light->m_shadowTexture);
		light->m_shadowTexture->m_slot = 9;
		
	}
	m_shadowCamera->SetOrthoProjection(Vector2(-128,-128), Vector2(128,128), -100, 100.f);

	/*switch (light->m_type) 
	{
	case DIRECTIONAL:
		m_shadowCamera->SetOrthoProjection(Vector2::ZERO, Vector2(light->m_shadowPPU, light->m_shadowPPU), 0.0f, light->m_shadowDistance);
		break;
	}*/
	//Vector3 lightDirection = light->m_transform.GetWorldMatrix().GetKVector();

	Vector3 lightDirection	   = light->GetDirection();
	Vector3 lightWorldPosition = light->GetPosition();
	
	//lightDirection     = Vector3();
	//lightWorldPosition = Vector3();
	//Matrix44 lookAt = m_shadowCamera->LookAt(lightWorldPosition,lightWorldPosition + lightDirection, Vector3::UP);
	Matrix44 lookAt = m_shadowCamera->LookAt(lightWorldPosition, lightWorldPosition + lightDirection, Vector3::UP);

	m_shadowCamera->SetModelMatrix(lookAt);
	Matrix44 viewMat = lookAt;
	viewMat.Inverse();
	m_shadowCamera->SetViewMatrix(viewMat);

	Camera::SetCurrentCamera(m_shadowCamera);
	
	m_renderer->ClearDepth(1.f);
	light->m_vp = m_shadowCamera->GetViewProjection();
	light->SetViewProjection(light->m_vp);
	Shader *shader = ShaderDefinitions::GetShaderByDefinitionName("shadow");
	m_renderer->BindShader(shader);
	Shader::SetCurrentShader(shader);
	for each(Renderable *renderable in scene->m_renderables)
	{
		//Material *meshMaterial = renderable->GetMaterial();
		GL_CHECK_ERROR();

		//if (meshMaterial->m_isOpaque)
		{

			//meshMaterial->SetTexture2D(8, light->m_shadowTexture);
			//meshMaterial->SetSampler(8, Sampler::GetDefaultSampler());


			//Renderer::GetInstance()->BindMaterial(meshMaterial);
			m_renderer->DrawMesh(renderable->m_mesh,renderable->m_modelMatrix);
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
