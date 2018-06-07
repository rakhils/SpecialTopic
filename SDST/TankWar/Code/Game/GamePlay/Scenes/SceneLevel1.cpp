#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Logger/LogManager.hpp"

#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GamePlay/Entity/Tank.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
// CONSTRUCTOR
SceneLevel1::SceneLevel1() : Scene()
{
	//InitScene();
}

SceneLevel1::SceneLevel1(std::string str) : Scene(str)
{
	//InitScene();
}

// DESTRUCTOR
SceneLevel1::~SceneLevel1()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Initialize scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::InitScene()
{
	if(m_init)
	{
		return;
	}
	m_init = true;
	m_forwardRenderingPath = new ForwardRenderingPath();
	Light *light = Light::CreateAmbientLight();
	Rgba color;
	color.SetAsFloats(1, 1, 1, GetRandomFloatInRange(0.5f, 1.0f));
	light->EnableAmbientLight(color.GetAsFloats());
	AddLight(light);
	
	Light *lightDirectional = Light::CreateOrGetPointLight();
	Rgba color1;
	color1.SetAsFloats(1, 1, 1, GetRandomFloatInRange(0.6f, 0.8f));
	lightDirectional->EnablePointLight(color1.GetAsFloats(),Vector3(0,70,100),Vector3(1,0,0));
	Vector3 lightDirection(1, -1, 0);
	lightDirectional->SetPointLightDirection(lightDirection.GetNormalized());
	lightDirectional->SetPointLightInnerOuterAngles(30.f, 60.f);
	AddLight(lightDirectional);
	// LOADING IT HERE
	Material::CreateOrGetMaterial1("Data\\Materials\\Skybox.mat", Renderer::GetInstance());
	
	//

	CreatePlayer();
	CreateMap();
	
	m_isSkyBoxEnabled = true;
	if(m_isSkyBoxEnabled)
	{
		m_texturecube = new TextureCube();
		m_texturecube->make_from_image("Data//Images//Galaxy.png");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Creates a player
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::CreatePlayer()
{
	Mesh *sphere = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0, 0, 0), .10f, 15, 15, Rgba::YELLOW, FILL_MODE_WIRE);
	m_playerTank = new Tank();
	m_playerTank->SetScene(this);
	m_playerTank->AddRigidBody3DComponent();
	m_playerTank->AddCameraComponent(Vector3(0, .10f, -2));
	m_playerTank->m_renderable->SetMesh(sphere);
	m_playerTank->AddRigidBody3DComponent();
	m_playerTank->GetRigidBody3DComponent()->m_gravity = Vector3(0,-1,0);
	m_playerTank->GetRigidBody3DComponent()->m_useGravity = false;
	m_playerTank->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	m_playerTank->m_transform.SetLocalPosition(Vector3(115, 30, 55));
	PerspectiveCamera *camera = (PerspectiveCamera*)(m_playerTank->GetComponentByType(CAMERA));
	AddCamera(camera);
	AddRenderable(m_playerTank->m_renderable);
	Camera::SetGameplayCamera(camera);
	/*Mesh *grid = MeshBuilder::Create2DGrid<Vertex_3DPCU>(Vector3(0, 0, 0), Vector2(30, 30), Vector3(1, 0, 0), Vector3(0, 0, 1), Rgba::WHITE);
	Renderable *renderable = new Renderable();
	renderable->m_name = "grid";
	renderable->SetMesh(grid);
	renderable->m_modelMatrix.SetIdentity();
	renderable->SetMaterial(Material::CreateOrGetMaterial("default", Renderer::GetInstance()));
	AddRenderable(renderable);*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::CreateMap()
{
	m_map = new Map();
	Image *terrainImage = new Image("Data//Images//m6.png");
	m_map->m_image = terrainImage;
	m_map->LoadFromImage(*terrainImage, AABB2(Vector2(0, 0), terrainImage->texture->getDimensions().GetAsVector2()), 1, 25, Vector2::ONE);

	Renderable *renderable = new Renderable();
	renderable->m_name = "terrain";
	renderable->SetMesh(m_map->m_terrain);
	renderable->m_modelMatrix.SetIdentity();
	renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	renderable->m_material->m_textures.at(0) = Texture::CreateOrGetTexture("Data//Images//terrain_grass.jpg");
	AddRenderable(renderable);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Updates scene
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::Update(float deltaTime)
{
	LogManager::PushLog("levelstart.txt", "SCENELEVEL1 UPDATE");
	if (IsEnteringScene(deltaTime))
	{
		UpdateEnteringTime(deltaTime);
	}
	if(isExitingScene(deltaTime))
	{
		UpdateExitingTime(deltaTime);
	}
	DebugRenderOptions options;
	options.m_lifeTime = 0;
	DebugDraw::GetInstance()->DebugRenderSphere(Vector3(0, 70, 100), 20, 32, 32, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL_WIRE, options);
	float PI = 3.14f;
	Vector3 minRotationValue(-PI / 2, 0, 0);
	Vector3 maxRotationValue(PI / 2, 2 * PI, 0);
	Vector2 delta = InputSystem::GetInstance()->GetMouseDelta();
	delta.y += .5;
	if (delta.x != 0)
	{
		m_playerTank->m_transform.RotateLocalByEuler(Vector3(0, 0.005f*delta.x, 0));
		Vector3 rotation = m_playerTank->m_transform.GetLocalEulerAngles();
		rotation.y = fmodf(rotation.y, 2 * PI);
		m_playerTank->m_transform.SetLocalRotationEuler(rotation);
	}
	/*if (delta.y != 0)
	{
		float rotateXAngle = 0.005f*delta.y;
		m_playerTank->m_transform.RotateLocalByEuler(Vector3(rotateXAngle, 0, 0));
		Vector3 rotation = m_playerTank->m_transform.GetLocalEulerAngles();
		rotation.x = ClampFloat(rotation.x, -PI / 2, PI / 2);
		m_playerTank->m_transform.SetLocalRotationEuler(rotation);
	}*/

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		Matrix44 worldMatrix = m_playerTank->m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIAxis();
		m_playerTank->m_transform.Translate(rigthDirection*(-1 * deltaTime * m_playerTank->m_velocity));
	}

	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		Matrix44 worldMatrix = m_playerTank->m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIAxis();
		m_playerTank->m_transform.Translate(rigthDirection*(1 * deltaTime* m_playerTank->m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		Matrix44 worldMatrix = m_playerTank->m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_playerTank->m_transform.Translate(forwardDirection*(1 * deltaTime * m_playerTank->m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		Matrix44 worldMatrix = m_playerTank->m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_playerTank->m_transform.Translate(forwardDirection*(-1 * deltaTime * m_playerTank->m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_SPACE))
	{
		Matrix44 worldMatrix = m_playerTank->m_transform.GetWorldMatrix();
		Vector3 upDirection = worldMatrix.GetJAxis();
		m_playerTank->m_transform.Translate(upDirection*(1 * deltaTime * m_playerTank->m_velocity));
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_ENTER))
	{
		Matrix44 worldMatrix = m_playerTank->m_transform.GetWorldMatrix();
		Vector3 upDirection = worldMatrix.GetJAxis();
		m_playerTank->m_transform.Translate(upDirection*(-1 * deltaTime * m_playerTank->m_velocity));
	}
	Vector3 position       = m_playerTank->m_transform.GetWorldPosition();
	float   terrainHeight  = m_map->GetHeight(position.GetXZ());
	//float   currentHeight  = Interpolate(position.y, terrainHeight + 2, deltaTime);
	m_playerTank->m_transform.SetLocalPosition(Vector3(position.x, terrainHeight + 2, position.z));

	m_playerTank->Update(deltaTime);
	Vector3 playerWorldPosition = m_playerTank->m_transform.GetWorldPosition();
	DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f , %f, %f", playerWorldPosition.x,playerWorldPosition.y,playerWorldPosition.z);
	//DebugDraw::GetInstance()->DebugRenderBasis(Vector3::ZERO, Vector3::ZERO, 0, 50);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::Render()
{
	m_forwardRenderingPath->RenderScene(this);
}
