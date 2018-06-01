#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderDefinitions.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Physics/Collider/SphereCollider.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"   
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/SceneManagement/SceneManager.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleSystem.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Audio/AudioComponent.hpp"
#include "Engine/Logger/LogManager.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Entity/Bullet.hpp"
#include "Game/GamePlay/Entity/Asteroid.hpp"
#include "Game/GamePlay/Entity/Ship.hpp"

Game::Game()
{
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/10
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::Init()
{

	if(m_init)
	{
		return;
	}

	m_init = true;

	m_forwardRenderingPath = new ForwardRenderingPath(g_theRenderer);
	m_scene = SceneManager::CreateOrGetScene("level1");

	CreatePlayer();

	Light *light = Light::CreateAmbientLight();
	light->EnableAmbientLight(Rgba::FADED_WHITE.GetAsFloats());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/06
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::CreatePlayer()
{
	Mesh *sphere = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0, 0, 0), 1, 15, 15, Rgba::YELLOW, FILL_MODE_WIRE);
	m_tank = new Ship();
	m_tank->AddRigidBody3DComponent();
	m_tank->AddCameraComponent(Vector3(0, 0, -10));
	m_tank->m_renderable->SetMesh(sphere);
	m_tank->m_renderable->SetMaterial(Material::CreateOrGetMaterial("default"));

	PerspectiveCamera *camera = (PerspectiveCamera*)(m_tank->GetComponentByType(CAMERA));
	m_scene->AddCamera(camera);
	Camera::SetCurrentCamera(camera);

	m_scene->AddRenderable(m_tank->m_renderable);	
}

void Game::Update(float deltaTime)
{		
	Init();

	float PI = 3.14f;
	Vector3 minRotationValue(-PI / 2, 0     , 0);
	Vector3 maxRotationValue( PI / 2, 2 * PI, 0);
	Vector2 delta = g_theInput->GetMouseDelta();
	delta.y += .5;
	if (delta.x != 0)
	{
		m_tank->m_transform.RotateLocalByEuler(Vector3(0, 0.005f*delta.x, 0));
		Vector3 rotation = m_tank->m_transform.GetLocalEulerAngles();
		rotation.y = fmodf(rotation.y, 2*PI);
		m_tank->m_transform.SetLocalRotationEuler(rotation);
	}
	if (delta.y != 0)
	{
		float rotateXAngle = 0.005f*delta.y;
		m_tank->m_transform.RotateLocalByEuler(Vector3(rotateXAngle, 0, 0));
		Vector3 rotation = m_tank->m_transform.GetLocalEulerAngles();
		rotation.x = ClampFloat(rotation.x, -PI / 2, PI / 2);
		m_tank->m_transform.SetLocalRotationEuler(rotation);
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		Matrix44 worldMatrix = m_tank->m_transform.GetWorldMatrix();
		Vector3 rigthDirection  = worldMatrix.GetIAxis();
		m_tank->m_transform.Translate(rigthDirection*(-1 * deltaTime));
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		Matrix44 worldMatrix = m_tank->m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIAxis();
		m_tank->m_transform.Translate(rigthDirection*(1 * deltaTime));
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		Matrix44 worldMatrix = m_tank->m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_tank->m_transform.Translate(forwardDirection*(1 * deltaTime));
		//LogManager::PushLog("Data//Logs//Sample.txt", "test2\n");

		DebugDraw::GetInstance()->DebugRenderLogf(0,"test");
		//((AudioComponent*)m_test1Object->GetComponentByType(AUDIO))->Play();
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		Matrix44 worldMatrix = m_tank->m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_tank->m_transform.Translate(forwardDirection*(-1 * deltaTime));
	}
	m_tank->Update(deltaTime);

	ParticleSystem::UpdateParticleSystem(deltaTime);
	DebugDraw::Update();
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
void Game::Render()
{
	DebugRenderOptions options;
	options.m_lifeTime = 0;

	Mesh *grid = MeshBuilder::Create2DGrid<Vertex_3DPCU>(Vector3(0, 0, 0), Vector2(30, 30), Vector3(1, 0, 0), Vector3(0, 0,1), Rgba::WHITE);
	Renderable *renderable = new Renderable();
	renderable->m_name = "grid";
	renderable->SetMesh(grid);
	Transform transformTemp;
	transformTemp.SetLocalPosition(Vector3(0, 0, 0));
	renderable->m_modelMatrix = transformTemp.GetLocalMatrix();
	renderable->SetMaterial(Material::CreateOrGetMaterial("default",g_theRenderer));
	m_scene->AddRenderable(renderable);
	m_forwardRenderingPath->m_renderer->BeginFrame();
	m_forwardRenderingPath->RenderScene(m_scene);

	m_scene->RemoveRenderable(renderable);
	delete renderable;
	
	DebugDraw::Render();
}

void Game::SetCameraFocusOnPosition()
{
	float PI = 3.14f;
	float aspect = Windows::GetInstance()->GetDimensions().x/Windows::GetInstance()->GetDimensions().y;
	//Camera::GetGamePlayCamera()->SetPerspectiveCamera(PI /6.f, 1/aspect, 0.1f, 300.f);
	//g_theRenderer->SetCamera(Camera::GetGamePlayCamera());
}


