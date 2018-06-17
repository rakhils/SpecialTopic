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
#include "Engine/Physics/Raycast3D.hpp"
#include "Engine/Physics/Terrain.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Audio/AudioComponent.hpp"

#include "Game/Game.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Scenes/SceneLevel1.hpp"
#include "Game/GamePlay/Entity/Tank.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Entity/EnemyBase.hpp"
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
	
	Mesh *lightSphere = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0,70,100), 10.f, 15, 15, Rgba::WHITE, FILL_MODE_WIRE);
	Renderable *lightRenderable = new Renderable();
	lightRenderable->SetMesh(lightSphere);
	lightRenderable->SetMaterial(Material::AquireResource("default"));
	AddRenderable(lightRenderable);

	Light *lightDirectional = Light::CreateOrGetPointLight();
	Rgba color1;
	color1.SetAsFloats(1, 1, 1, GetRandomFloatInRange(0.6f, 0.8f));
	lightDirectional->EnablePointLight(color1.GetAsFloats(),Vector3(0,70,100),Vector3(1,0,0));
	Vector3 lightDirection(1, -1, 0);
	lightDirectional->SetPointLightDirection(lightDirection.GetNormalized());
	lightDirectional->SetPointLightInnerOuterAngles(30.f, 60.f);
	AddLight(lightDirectional);
	// LOADING IT HERE
	CreateMap();
	CreatePlayer();
	
	m_isSkyBoxEnabled = false;
	if(m_isSkyBoxEnabled)
	{
		Material::CreateOrGetMaterial1("Data\\Materials\\Skybox.mat", Renderer::GetInstance());
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
	Mesh *turretHead = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3::ZERO, .75f, 15, 15, Rgba::YELLOW, FILL_MODE_WIRE);
	Mesh *tankBody   = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(1.f, .25f, 2.f), Rgba::WHITE, FILL_MODE_FILL);
	Mesh *turretGun  = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(.1f, .1f, 2.f), Rgba::WHITE, FILL_MODE_FILL);
	GameObject *turretHeadGO = new GameObject("turrethead");
	GameObject *turretGunGO = new GameObject("turretgun");
	turretHeadGO->SetScene(this);
	turretHeadGO->m_renderable->SetMesh(turretHead);
	turretHeadGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	turretHeadGO->m_transform.SetLocalPosition(Vector3(0, 0.4f, 0));
	
	turretGunGO->SetScene(this);
	turretGunGO->m_renderable->SetMesh(turretGun);
	turretGunGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));
	turretGunGO->m_transform.SetLocalPosition(Vector3(0, .5, 2));


	m_playerTank = new Tank();
	m_playerTank->SetScene(this);
	m_playerTank->AddRigidBody3DComponent();
	m_playerTank->m_renderable->SetMesh(tankBody);
	m_playerTank->AddRigidBody3DComponent();
	m_playerTank->GetRigidBody3DComponent()->m_gravity = Vector3(0,-1,0);
	m_playerTank->GetRigidBody3DComponent()->m_useGravity = false;
	m_playerTank->AddParticleComponent(Vector3::ZERO, Renderer::GetInstance());
	m_playerTank->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\default_light.mat"));

	m_playerTank->AddChild(turretHeadGO);
	m_playerTank->AddChild(turretGunGO);

	m_playerTank->m_transform.Translate(Vector3(120, 20, 274));

	m_camera = (PerspectiveCamera*)Camera::CreateOrGetCamera("level1cam", PERSPECTIVE);
	m_camera->m_transform.SetLocalPosition(Vector3(59, 30, 174));
	m_camera->m_transform.RotateLocalByEuler(Vector3(0, 90, 0));
	AddCamera(m_camera);
	AddRenderable(m_playerTank->m_renderable);
	AddRenderable(turretGunGO->m_renderable);
	AddRenderable(turretHeadGO->m_renderable);
	m_playerTank->AddSphereCollider(Vector3::ZERO, 1.5f);

	float height = m_map->GetHeight(Vector2(10, 10));
	EnemyBase::AddEnemyBase("enemybase_1", Vector3(10, height + 5, 10), Vector3(10, 10, 10), this);

	height = m_map->GetHeight(Vector2(500, 10));
	EnemyBase::AddEnemyBase("enemybase_2", Vector3(500, height+ 5, 10), Vector3(10, 10, 10), this);
	
	height = m_map->GetHeight(Vector2(10, 500));
	EnemyBase::AddEnemyBase("enemybase_3", Vector3(10, height+ 5, 500), Vector3(10, 10, 10), this);

	height = m_map->GetHeight(Vector2(500, 500));
	EnemyBase::AddEnemyBase("enemybase_4", Vector3(500, height+ 5, 500), Vector3(10, 10, 10), this);

	Camera::SetGameplayCamera(m_camera);


	m_soundTestObj = new GameObject("soundtest");
	m_soundTestObj->SetScene(this);
	m_soundTestObj->SetPosition(Vector3(250, 20, 250));
	m_soundTestObj->Add3DAudioComponent("Data\\Audio\\GameplayMusic.mp3", Vector3::ZERO);
	AudioComponent* audio =  (AudioComponent*)m_soundTestObj->GetComponentByType(AUDIO);
	audio->Play();
	Mesh *body   = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(1, 1, 2), Rgba::WHITE, FILL_MODE_FILL);
	m_soundTestObj->m_renderable->SetMesh(body);
	m_soundTestObj->m_renderable->SetMaterial(Material::AquireResource("default"));
	AddRenderable(m_soundTestObj->m_renderable);
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
	m_map->LoadFromImage(this,terrainImage, AABB2(Vector2(0, 0),256,256), 1, 50, Vector2::ONE);

	Game::SetMinBounds(Vector3(0, 1, 0));
	Game::SetMaxBounds(Vector3(static_cast<float>(m_map->m_image->GetDimensions().x), 50, static_cast<float>(m_map->m_image->GetDimensions().y)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Updates scene
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::Update(float deltaTime)
{
	m_camera->Update(deltaTime);
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
	options.m_mode = DEBUG_RENDER_USE_DEPTH;
	DebugDraw::GetInstance()->DebugRenderSphere(Vector3(0, 70, 100), 20, 32, 32, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL_WIRE, options);

	
	if (InputSystem::GetInstance()->IsLButtonDown())
	{
		Vector2 screenXY = InputSystem::GetInstance()->GetMouseClientPosition();
		PickRay ray = Camera::GetGamePlayCamera()->GetPickRayFromScreenCords(screenXY);

		Ray raycast;
		raycast.m_direction = ray.m_direction;
		raycast.m_start = m_playerTank->m_transform.GetWorldPosition();

		RaycastHit result = m_map->m_terrain->Raycast(raycast);
		DebugDraw::GetInstance()->DebugRenderLogf("RAYCAST : POSITION :: %f,%f %f", result.m_position.x, result.m_position.y, result.m_position.z);
		DebugDraw::GetInstance()->DebugRenderLine(raycast.m_start, result.m_position, Rgba::YELLOW, 0.f, DEBUG_RENDER_IGNORE_DEPTH);
		DebugDraw::GetInstance()->DebugRenderLine(result.m_position,result.m_position + ray.m_direction*1000 ,Rgba::RED, 0.f, DEBUG_RENDER_IGNORE_DEPTH);

		DebugDraw::GetInstance()->DebugRenderSphere(result.m_position, 1, 16, 16, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	}

	m_playerTank->Update(deltaTime);
	m_soundTestObj->Update(deltaTime);
	Vector3 playerWorldPosition = m_playerTank->m_transform.GetWorldPosition();
	DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f ,%f, %f", playerWorldPosition.x,playerWorldPosition.y,playerWorldPosition.z);
	DebugDraw::GetInstance()->DebugRenderLogf("ENEMY BASE LEFT %d ",static_cast<int>(EnemyBase::s_enemyBases.size()));
	DebugDraw::GetInstance()->DebugRenderLogf("ENEMY TANK LEFT %d ", static_cast<int>(EnemyBase::s_enemyTanks.size()));

	EnemyBase::UpdateEnemyBase(deltaTime);
	
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


	AABB2 healthBound(Vector2(1600, 1000), Vector2(1900, 1050));
	AABB2 health(Vector2(1600, 1000), Vector2(1600 + m_playerTank->m_health*30, 1050));
	AABB2 statsBound(Vector2(1400, 900), Vector2(1950, 1070));

	Camera::SetCurrentCamera(Camera::GetUICamera());
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawRectangle(statsBound);
	Renderer::GetInstance()->DrawRectangle(healthBound);
	Renderer::GetInstance()->DrawAABB(health, Rgba::RED);

	Material *text = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(text);
	Renderer::GetInstance()->DrawText2D(Vector2(1450, 1025), "HEALTH : ", 10.f, Rgba::WHITE, 1, nullptr);
	delete defaultMaterial;
	delete text;


	if (EnemyBase::s_enemyBases.size() == 0 && EnemyBase::s_enemyTanks.size() == 0)
	{
		Material *def = Material::AquireResource("Data\\Materials\\text.mat");
		Renderer::GetInstance()->BindMaterial(def);
		Renderer::GetInstance()->DrawText2D(Vector2(1000, 500), "YOU WON", 50, Rgba::WHITE, 1, nullptr);
		delete def;
	}

	if(m_playerTank->m_markForDead)
	{
		Material *def = Material::AquireResource("Data\\Materials\\text.mat");
		Renderer::GetInstance()->BindMaterial(def);
		std::string text1 = ToString(m_playerTank->m_timeLeftToRespawn) + " SECONDS";
		Renderer::GetInstance()->DrawText2D(Vector2(300, 500), "YOU DIED WAIT FOR "+text1, 30, Rgba::WHITE, 1, nullptr);
		delete def;
	}
}
