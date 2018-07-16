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
#include "Engine/Renderer/Camera/OrbitCamera.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"

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

	Vector3 lightPosition(20, 50, 50);
	Vector3 lightDirection = Vector3(0.f, -1.f, 1.f);

	Light *lightDirectional = Light::CreateOrGetPointLight();
	Rgba color1;
	color1.SetAsFloats(1, 1, 1, GetRandomFloatInRange(0.6f, 0.8f));
	lightDirectional->EnablePointLight(color1.GetAsFloats(),lightPosition,Vector3(1,0,0));
	DebugRenderOptions options;
	options.m_lifeTime = 1000;
	DebugDraw::GetInstance()->DebugRenderSphere(lightPosition, 5, 32, 32, nullptr, Rgba::YELLOW, DEBUG_RENDER_FILL, options);
	DebugDraw::GetInstance()->DebugRenderLine(lightPosition, lightPosition + (lightDirection * 10), Rgba::RED, 1000, DEBUG_RENDER_IGNORE_DEPTH);
	
	lightDirectional->EnableShadow();
	lightDirectional->SetLightDirection(lightDirection.GetNormalized());
	lightDirectional->SetPointLightInnerOuterAngles(30.f, 60.f);
	//lightDirectional->
	


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
	Mesh *tankBody   = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(1.f, .5f, 2.f), Rgba::WHITE, FILL_MODE_FILL);
	Mesh *turretGun  = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(.1f, .1f, 1.5f), Rgba::WHITE, FILL_MODE_FILL);
	GameObject *turretHeadGO = new GameObject("turrethead");
	GameObject *turretGunGO = new GameObject("turretgun");
	turretHeadGO->SetScene(this);
	turretHeadGO->m_renderable->SetMesh(turretHead);
	turretHeadGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\Tank.mat"));
	turretHeadGO->m_transform.SetLocalPosition(Vector3(0, 1, 0));
	
	turretGunGO->SetScene(this);
	turretGunGO->m_renderable->SetMesh(turretGun);
	turretGunGO->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\Tank.mat"));
	turretGunGO->m_transform.SetLocalPosition(Vector3(0, 1, .5f));


	m_playerTank = new Tank();
	m_playerTank->SetScene(this);
	m_playerTank->AddRigidBody3DComponent();
	m_playerTank->m_renderable->SetMesh(tankBody);
	m_playerTank->AddRigidBody3DComponent();
	m_playerTank->AddParticleComponent(Vector3::ZERO, Renderer::GetInstance());
	m_playerTank->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\Tank.mat"));
	m_playerTank->m_turret = turretGunGO;
	m_playerTank->m_turretHead = turretHeadGO;
	m_playerTank->AddChild(turretHeadGO);
	m_playerTank->AddChild(turretGunGO);
	m_playerTank->AddParticleComponent(Vector3(0,2,-1), Renderer::GetInstance());
	//m_playerTank->m_transform.Translate(Vector3(120, 20, 274));
	m_playerTank->m_transform.Translate(Vector3(30, 0, 30));
/*
	m_camera = (OrbitCamera*)Camera::CreateOrGetCamera("level1cam", ORBIT);
	m_camera->m_transform.SetLocalPosition(Vector3(0, 50, -10));
	AddCamera(m_camera);
	Camera::SetGameplayCamera(m_camera);*/

	m_ocamera = (OrbitCamera*)Camera::CreateOrGetCamera("level1cam", ORBIT);
	//m_ocamera->m_transform.SetLocalPosition(Vector3(20, 60, 0));
	//m_ocamera->m_transform.RotateLocalByEuler(Vector3(0, 0, 0));
	//float PI     = 3.14f;
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float heigth = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	float aspect = width / heigth;
	Matrix44 perspectiveMatrix = Matrix44::MakePerspectiveMatrix(60.f, aspect, 0.1f, 300.f);
	m_ocamera->SetProjection(perspectiveMatrix);
	

	AddCamera(m_ocamera);
	Camera::SetGameplayCamera(m_ocamera);

	AddRenderable(m_playerTank->m_renderable);
	AddRenderable(turretGunGO->m_renderable);
	AddRenderable(turretHeadGO->m_renderable);
	m_playerTank->AddSphereCollider(Vector3::ZERO, 1.5f);

	float height = m_map->GetHeight(Vector2(10, 10));
	EnemyBase::AddEnemyBase("enemybase_1", Vector3(10, height + 5, 10), Vector3(10, 10, 10), this);

	height = m_map->GetHeight(Vector2(500, 10));
	EnemyBase::AddEnemyBase("enemybase_2", Vector3(200, height+ 5, 10), Vector3(10, 10, 10), this);
	
	height = m_map->GetHeight(Vector2(10, 500));
	EnemyBase::AddEnemyBase("enemybase_3", Vector3(10, height+ 5, 200), Vector3(10, 10, 10), this);

	height = m_map->GetHeight(Vector2(500, 500));
	EnemyBase::AddEnemyBase("enemybase_4", Vector3(200, height+ 5, 200), Vector3(10, 10, 10), this);



	m_soundTestObj = new GameObject("soundtest");
	m_soundTestObj->SetScene(this);
	m_soundTestObj->SetPosition(Vector3(100, 20, 100));
	m_soundTestObj->Add3DAudioComponent("Data\\Audio\\GameplayMusic.mp3", Vector3::ZERO);
	AudioComponent* audio3D =  (AudioComponent*)m_soundTestObj->GetComponentByType(AUDIO);
	audio3D->Play();
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
	m_map->LoadFromImage(this,terrainImage, AABB2(Vector2(0, 0),256,256), 1, 50, Vector2::ONE*4);
	CreateWater();
	Game::SetMinBounds(Vector3(0, 1, 0));
	Game::SetMaxBounds(Vector3(static_cast<float>(m_map->m_image->GetDimensions().x), 50, static_cast<float>(m_map->m_image->GetDimensions().y)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::CreateWater()
{
	GameObject *water = new GameObject("water");
	Rgba color(Vector4(1, 1, 1,0.75f));
	Mesh *mesh = MeshBuilder::Create3DPlaneChunk<Vertex_3DPCUNTB>(Vector3(0, 20, 0), Vector3::RIGHT, Vector3::FORWARD, Vector2(50, 50),Vector2::ONE ,color,FILL_MODE_FILL);
	//Mesh *mesh1 = MeshBuilder::Create3DPlane<Vertex_3DPCUNTB>(Vector3(0, 20, 0), Vector3::RIGHT, Vector3::FORWARD, Vector2(20, 20) ,color,FILL_MODE_FILL);
	water->m_renderable->m_name = "water";
	water->m_renderable->SetMesh(mesh);
	water->m_renderable->SetMaterial(Material::AquireResource("Data\\Materials\\Water.mat"));
	water->SetScene(this);
	AddRenderable(water->m_renderable);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Updates scene
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::Update(float deltaTime)
{
	ProfilerManager::PushProfiler("SceneLevel1::Update");
	
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
	
	if(m_playerTank->m_health <= 0)
	{
		m_levelState = LOSE;
	}
	if(EnemyBase::s_enemyBases.size() == 0 && EnemyBase::s_enemyTanks.size() == 0)
	{
		m_levelState = WIN;
	}
	if(m_levelState == LOSE)
	{
		m_playerTank->m_timeLeftToRespawn -= deltaTime;
		if(m_playerTank->m_timeLeftToRespawn <=0)
		{
			if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_ENTER))
			{
				m_playerTank->Respawn(0,10);
				m_levelState = PLAY;
			}
		}
		ProfilerManager::PoPProfiler();
		return;
	}
	if(m_levelState == WIN)
	{
		if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_ENTER))
		{
			
		}
		//return;
	}
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_B))
	{
		if(m_playerTank->m_isArcherBullet)
		{
			m_playerTank->m_isArcherBullet = false;
		}
		else
		{
			m_playerTank->m_isArcherBullet = true;
		}
	}
	if(InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_Z))
	{
		m_cameraTheta += 50*deltaTime;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_X))
	{
		m_cameraTheta -= 50*deltaTime;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_C))
	{
		m_cameraPhi += 50*deltaTime;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_V))
	{
		m_cameraPhi -= 50*deltaTime;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_T))
	{
		m_cameraRadius += 50*deltaTime;
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_R))
	{
		m_cameraRadius -= 50*deltaTime;
	}
	
	if (InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::KEYBOARD_M))
	{
		if(!m_mode)
		{
			m_mode = true;
			InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_ABSOLUTE;
			InputSystem::GetInstance()->ShowCursor(true);
			InputSystem::GetInstance()->MouseLockToScreen(false);
		}
		else
		{
			m_mode = false;
			InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_RELATIVE;
			InputSystem::GetInstance()->ShowCursor(false);
			InputSystem::GetInstance()->MouseLockToScreen(true);
			ProfilerManager::s_profilerEnabled = false;
			ProfilerManager::s_isPaused = false;
			
		}
	}
	if (InputSystem::GetInstance()->isKeyPressed(InputSystem::KEYBOARD_L))
	{
		Sampler::GetCurrentSampler()->m_nearValue+=10;
	}

	Vector3 position(-m_cameraRadius, m_cameraPhi, m_cameraTheta);

	((OrbitCamera*)m_ocamera)->SetTargetPosition(m_playerTank->m_transform.GetWorldPosition() + Vector3(0,5,0));
	((OrbitCamera*)m_ocamera)->SetSphericalCords(position);

	Vector3 cameraPosition = m_ocamera->m_transform.GetWorldPosition();
	Vector3 cameraAngle	   = m_ocamera->m_transform.GetWorldRotation();

	//float PI = 3.14f;
	Vector2 delta = g_theInput->GetMouseDelta();
	delta.y += .5;
	if(m_mode)
	{
		delta = Vector2::ZERO;
		//delta.y += .5;
	}
	if (delta.x != 0)
	{
		m_cameraPhi  -= delta.x/5.f;
		m_cameraPhi  = fmodf(m_cameraPhi, 360);
	}
	if (delta.y != 0)
	{
		m_cameraTheta += delta.y / 5.f;
		m_cameraTheta = ClampFloat(m_cameraTheta, -90, 90);
	}

	//DebugDraw::GetInstance()->DebugRenderSphere(Vector3::ZERO, 5, 32, 32, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	m_playerTank->Update(deltaTime);
	m_soundTestObj->Update(deltaTime);
	Vector3 playerWorldPosition = m_playerTank->m_transform.GetWorldPosition();
	DebugDraw::GetInstance()->DebugRenderLogf("POSITION %f ,%f, %f", playerWorldPosition.x,playerWorldPosition.y,playerWorldPosition.z);
	DebugDraw::GetInstance()->DebugRenderLogf("ENEMY BASE LEFT %d ",static_cast<int>(EnemyBase::s_enemyBases.size()));
	DebugDraw::GetInstance()->DebugRenderLogf("ENEMY TANK LEFT %d ", static_cast<int>(EnemyBase::s_enemyTanks.size()));

	EnemyBase::UpdateEnemyBase(deltaTime);
	ProfilerManager::PoPProfiler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLevel1::Render()
{
	ProfilerManager::PushProfiler("SceneLevel1::Render");
	m_forwardRenderingPath->RenderScene(this);


	AABB2 healthBound(Vector2(1600, 1000), Vector2(1900, 1050));
	AABB2 health(Vector2(1600, 1000), Vector2(1600 + m_playerTank->m_health*30, 1050));
	AABB2 statsBound(Vector2(1315, 900), Vector2(1950, 1070));

	Camera::SetCurrentCamera(Camera::GetUICamera());
	Material *defaultMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Renderer::GetInstance()->DrawRectangle(statsBound);
	Renderer::GetInstance()->DrawRectangle(healthBound);
	Renderer::GetInstance()->DrawAABB(health, Rgba::RED);

	Material *text = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(text);
	Renderer::GetInstance()->DrawText2D(Vector2(1450, 1025), "HEALTH : ", 10.f, Rgba::WHITE, 1, nullptr);
	if(m_playerTank->m_isArcherBullet)
	{
		Renderer::GetInstance()->DrawText2D(Vector2(1325, 950), "BULLET TYPE ARCHER PRESS B TO CHANGE",10.f, Rgba::YELLOW, 1, nullptr);
	}
	else
	{
		Renderer::GetInstance()->DrawText2D(Vector2(1325, 950), "BULLET TYPE NORMAL PRESS B TO CHANGE",10.f, Rgba::YELLOW, 1, nullptr);
	}
	delete defaultMaterial;
	delete text;

	if (m_levelState == LOSE)
	{

		Material *def = Material::AquireResource("Data\\Materials\\text.mat");
		Renderer::GetInstance()->BindMaterial(def);
		std::string text1 = ToString(m_playerTank->m_timeLeftToRespawn) + " SECONDS";
		if (m_playerTank->m_timeLeftToRespawn <= 0)
		{
			Renderer::GetInstance()->DrawText2D(Vector2(300, 500), "PRESS ENTER TO RESPAWN ", 30, Rgba::WHITE, 1, nullptr);
		}
		else
		{
			Renderer::GetInstance()->DrawText2D(Vector2(300, 500), "YOU DIED WAIT FOR " + text1, 30, Rgba::WHITE, 1, nullptr);
		}
		delete def;
		ProfilerManager::PoPProfiler();

		return;
	}
	if (m_levelState == WIN)
	{
		Material *def = Material::AquireResource("Data\\Materials\\text.mat");
		Renderer::GetInstance()->BindMaterial(def);
		Renderer::GetInstance()->DrawText2D(Vector2(1000, 500), "YOU WON", 50, Rgba::WHITE, 1, nullptr);
		delete def;
	}
	ProfilerManager::PoPProfiler();

}
