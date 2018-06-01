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
	//SetCameraFocusOnPosition();


	m_map = new Map();
	m_init = true;


	m_forwardRenderingPath = new ForwardRenderingPath(g_theRenderer);

	

	m_scene = SceneManager::CreateOrGetScene("level1");
	//m_scene->AddCamera(g_theCamera::GetGamePlayCamera());

	CreateShip();
	Camera::GetCurrentCamera()->m_transform.Translate(Vector3(0, 0, -6));

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
void Game::CreateShip()
{
	//g_theCamera::GetCurrentCamera() = g_theCamera::GetGamePlayCamera();
	//m_ship = new Ship();
	//m_ship->m_renderable->SetMesh(g_theRenderer->CreateOrGetMesh("Data\\Model\\scifi_fighter_mk6.obj"));
	//m_ship->m_renderable->SetMaterial(Material::CreateOrGetMaterial("Data\\Materials\\ship.mat", g_theRenderer));
	//m_ship->m_renderable->m_useLight = true;
	//m_ship->m_renderable->m_name = "ship";
	//m_ship->m_transform.RotateLocalByEuler(Vector3(1.f * 0.5f, 0.f, 0.f));
	//m_ship->AddChild(m_lightObj);
	//SphereCollider* collider = new SphereCollider();
	//collider->m_radius = 10;
	


	//Mesh *mesh = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0, 0, 0), 1, 15, 15, Rgba::BLUE, FILL_MODE_WIRE);
	//m_lightObj->m_renderable->SetMesh(mesh);
	//m_lightObj->m_renderable->SetMaterial(Material::CreateOrGetMaterial("default", g_theRenderer));
	//m_lightObj->m_transform.Translate(g_theRenderer->GetCurrentCameraForwardVector());
	//m_lightObj->m_renderable->m_sortOrder = 0;
	
	//m_ship->m_transform.AddChild(&m_lightObj->m_transform);
	//m_lightObj->m_transform.m_parent = &m_ship->m_transform;
	//m_lightObj->m_name = "shiplight";
	
	
	m_testObject = new GameObject("testobject",m_scene);
	Mesh *meshtest = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0, 0, 0), .10, 15, 15, Rgba::YELLOW, FILL_MODE_WIRE);
	m_testObject->m_renderable->SetMesh(meshtest);
	m_testObject->m_renderable->SetMaterial(Material::CreateOrGetMaterial("default"));
	m_testObject->AddRigidBody3DComponent();
	m_testObject->AddSphereCollider(Vector3::ZERO, 10.f);
	m_testObject->AddLightComponent(Vector3::ZERO, Rgba::RED);

	m_testObject->AddCameraComponent(Vector3(0, 0, -10));
	PerspectiveCamera *camera = (PerspectiveCamera*)(m_testObject->GetComponentByType(CAMERA));
	m_scene->AddCamera(camera);
	Camera::SetCurrentCamera(camera);
	//Camera::SetCurrentCamera(camera);


	RigidBody3D *rb = new RigidBody3D();
	rb->m_useGravity = true;
	rb->m_gravity = Vector3(0, -.25f, 0);
	rb->m_friction   = .05f;
	m_testObject->AddComponent(RIGID_BODY_3D,rb);


	m_test1Object = new GameObject("test1object",m_scene);
	RigidBody3D *rb1 = new RigidBody3D();
	rb1->m_useGravity = false;
	rb1->m_friction = .05f;
	m_test1Object->AddComponent(RIGID_BODY_3D, rb1);
	Mesh *plane = MeshBuilder::Create3DPlane<Vertex_3DPCUNTB>(Vector3::ZERO, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector2(50, 50), Rgba::RED,FILL_MODE_FILL);
	m_test1Object->AddBoxCollider2D(Vector3::ZERO, Vector3(2, 50, 50),Vector3(0,0,1));
	m_test1Object->m_renderable->SetMesh(plane);
	m_test1Object->m_transform.Translate(Vector3(0, -80, 0));
	m_test1Object->m_renderable->SetMaterial(Material::CreateOrGetMaterial("default", g_theRenderer));
	m_test1Object->AddAudioComponent("Data\\Audio\\GameplayMusic.mp3");
	m_test1Object->AddParticleComponent(Vector3::ZERO,g_theRenderer);

	m_scene->AddRenderable(m_testObject->m_renderable);
	//m_scene->AddRenderable(m_ship->m_renderable);
	//m_scene->AddRenderable(m_lightObj->m_renderable);
	m_scene->AddRenderable(m_test1Object->m_renderable);



	
}

void Game::Update(float deltaTime)
{		
	Init();
	if(g_theInput->wasKeyJustPressed(27))
	{
		isQuitTriggered = true;
	}

	float PI = 3.14f;
	Vector3 minRotationValue(-PI / 2, 0     , 0);
	Vector3 maxRotationValue( PI / 2, 2 * PI, 0);
	Vector2 delta = g_theInput->GetMouseDelta();
	delta.y += .5;
	if (delta.x != 0)
	{
		m_testObject->m_transform.RotateLocalByEuler(Vector3(0, 0.005f*delta.x, 0));
		Vector3 rotation = m_testObject->m_transform.GetLocalEulerAngles();
		rotation.y = fmodf(rotation.y, 2*PI);
		m_testObject->m_transform.SetLocalRotationEuler(rotation);
	}
	if (delta.y != 0)
	{
		float rotateXAngle = 0.005f*delta.y;
		m_testObject->m_transform.RotateLocalByEuler(Vector3(rotateXAngle, 0, 0));
		Vector3 rotation = m_testObject->m_transform.GetLocalEulerAngles();
		rotation.x = ClampFloat(rotation.x, -PI / 2, PI / 2);
		m_testObject->m_transform.SetLocalRotationEuler(rotation);
	}

	float movementFactor = 20.f;
	if(g_theInput->isKeyPressed(InputSystem::KEYBOARD_0))
	{
		m_testObject->GetRigidBody3DComponent()->ApplyForce(Vector3(0, 5, 0), deltaTime);
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_1))
	{
		m_test1Object->m_transform.RotateLocalByEuler(Vector3(0.05, 0, 0.0));
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		Matrix44 worldMatrix = m_testObject->m_transform.GetWorldMatrix();
		Vector3 rigthDirection  = worldMatrix.GetIAxis();
		m_testObject->m_transform.Translate(rigthDirection*(-1 * deltaTime));
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		Matrix44 worldMatrix = m_testObject->m_transform.GetWorldMatrix();
		Vector3 rigthDirection = worldMatrix.GetIAxis();
		m_testObject->m_transform.Translate(rigthDirection*(1 * deltaTime));
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		Matrix44 worldMatrix = m_testObject->m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_testObject->m_transform.Translate(forwardDirection*(1 * deltaTime));
		//LogManager::PushLog("Data//Logs//Sample.txt", "test2\n");

		DebugDraw::GetInstance()->DebugRenderLogf(0,"test");
		//((AudioComponent*)m_test1Object->GetComponentByType(AUDIO))->Play();
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		Matrix44 worldMatrix = m_testObject->m_transform.GetWorldMatrix();
		Vector3 forwardDirection = worldMatrix.GetKAxis();
		m_testObject->m_transform.Translate(forwardDirection*(-1 * deltaTime));
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		((ParticleEmitter*)m_test1Object->GetComponentByType(PARTICLE))->SpawnParticles(4);
		//((AudioComponent*)m_test1Object->GetComponentByType(AUDIO))->ResumeSound();
	}
	DebugRenderOptions options;
	DebugDraw::GetInstance()->m_renderer = g_theRenderer;
	DebugDraw::GetInstance()->m_camera   = Camera::GetCurrentCamera();
	DebugDraw::GetInstance()->DebugRenderTag(Vector3(0,10,0), "LOOK AT ME", 1, Rgba::WHITE,options);


	DebugDraw::GetInstance()->DebugRenderText(Vector3(0,20,0),Vector3(1,0,0),"TEXT 3D",Vector2(1,1),3,options);
	DebugDraw::GetInstance()->DebugRenderLogf("X = %f Y = %f", delta.x, delta.y);
	DebugDraw::GetInstance()->DebugRenderText2D(Vector3(100, 100, 0), Vector3(1, 0, 0), "TEXT2D", Vector2(1, 1), 10, Rgba::RED, options);

	m_testObject->Update(deltaTime);
	m_test1Object->Update(deltaTime);

	ParticleSystem::UpdateParticleSystem(deltaTime);
	DebugDraw::Update();
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::OnCollisionEnter(Collider *collider)
{
	int a = 1;
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

	

	std::string scoreText = "SCORE : ";
	std::string score =  std::to_string(m_score);
	scoreText.append(score);
	DebugRenderOptions options;
	options.m_lifeTime = 0;
	//debugDraw->DebugRenderText2D(Vector3(0, 200, 0), Vector3(0, 0, 0), scoreText, Vector2(0, 0), 10, Rgba::GREEN, options);
	GL_CHECK_ERROR();



	Mesh *grid = MeshBuilder::Create2DGrid<Vertex_3DPCU>(Vector3(0, 0, 0), Vector2(30, 30), Vector3(1, 0, 0), Vector3(0, 0,1), Rgba::WHITE);
	Renderable *renderable = new Renderable();
	renderable->m_name = "grid";
	renderable->SetMesh(grid);
	Transform transformTemp;
	transformTemp.SetLocalPosition(Vector3(0, 0, 0));
	renderable->m_modelMatrix = transformTemp.GetLocalMatrix();
	renderable->SetMaterial(Material::CreateOrGetMaterial("default",g_theRenderer));
	m_scene->AddRenderable(renderable);
	GL_CHECK_ERROR();

	Material *material1 = Material::CreateOrGetMaterial("Data//Materials//default_light.mat");

	Material *material2 = Material::AquireResource("Data//Materials//default_light.mat");
	Material *material3 = Material::AquireResource("Data//Materials//default_light.mat");
	material1->SetProperty("TINT1", Vector4(0, 0, 0, 1));
	Material *material4 = Material::CreateOrGetMaterial("Data//Materials//default_light.mat", g_theRenderer);
	float x = 10;
	Vector2 k(1, 1);
	Vector2 j = x * k;

	//delete material2;
	//delete material3;

	Mesh *cube = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3(0, 10, 0), Vector3(5, 5, 5), Rgba::WHITE, FILL_MODE_FILL);
	Renderable *renderable1 = new Renderable();
	renderable1->m_modelMatrix.SetIdentity();
	renderable1->m_name = "cube";
	renderable1->SetMesh(cube);
	renderable1->SetMaterial(Material::AquireResource("Data//Materials//default_light.mat"));
	m_scene->AddRenderable(renderable1);
	GL_CHECK_ERROR();
	m_forwardRenderingPath->m_renderer->BeginFrame();

	m_forwardRenderingPath->RenderScene(m_scene);
	GL_CHECK_ERROR();
	

	m_scene->RemoveRenderable(renderable);
	m_scene->RemoveRenderable(renderable1);
	delete renderable;

	delete renderable1;
	//m_map->Render();
	/*Vector3 direction = m_test1Object->GetBoxCollider2D()->m_normalX;
	Vector3 testpos   = m_test1Object->GetWorldPosition();
	debugDraw->DebugRenderLine(testpos, testpos + direction * 150,Rgba::RED,1);

	direction = m_test1Object->GetBoxCollider2D()->m_normalY;
	testpos = m_test1Object->GetWorldPosition();
	debugDraw->DebugRenderLine(testpos, testpos + direction * 150, Rgba::GREEN, 1);

	direction = m_test1Object->GetBoxCollider2D()->m_normalZ;
	testpos = m_test1Object->GetWorldPosition();
	debugDraw->DebugRenderLine(testpos, testpos + direction * 150, Rgba::BLUE, 1);*/
	
	DebugDraw::Render();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::RenderBullet()
{
	for each (Bullet *bullet in m_bullets)
	{
		bullet->Render();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/10
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::RenderEnemyBullet()
{
	for each (Bullet *bullet in m_enemyBullet)
	{
		bullet->Render();
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::RenderAsteroids()
{
	for each (Bullet *bullet in m_bullets)
	{
		bullet->Render();
	}
}

void Game::SetCameraFocusOnPosition()
{
	float PI = 3.14f;
	float aspect = Windows::GetInstance()->GetDimensions().x/Windows::GetInstance()->GetDimensions().y;
	//Camera::GetGamePlayCamera()->SetPerspectiveCamera(PI /6.f, 1/aspect, 0.1f, 300.f);
	//g_theRenderer->SetCamera(Camera::GetGamePlayCamera());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::CreateBullet(Vector3 position, Vector3 forward,bool isEnemy)
{
	SoundID testSound = g_audio->CreateOrGetSound("Data\\Audio\\PlayerHit.wav");
	g_audio->PlaySound(testSound);

	Bullet *bullet = new Bullet(Vector3::ZERO,forward);
	bullet->SetScene(m_scene);
	bullet->SetPosition(position);
	bullet->m_light = Light::CreateOrGetPointLight();
	if(bullet-> m_light != nullptr)
	{
		bullet->m_light->SetPointLightColor(Rgba::FADED_RED.GetAsFloats());
		bullet->m_light->SetPointLightDirection(bullet->m_forward);
		bullet->m_light->SetPointLightAttenutaion(Vector3(1, 0, 0));
		bullet->m_light->SetPointLigthSpecAttenuation(Vector3(1, 0, 0));
		bullet->m_light->SetPointLightAttenutaion(Vector3(1, 0, 0));
		bullet->m_light->SetPointLigthSpecAttenuation(Vector3(1, 0, 1));
	}
	bullet->m_forward = forward;
	//bullet->m_velocity = forward*10;
	bullet->m_renderable->SetMaterial(Material::CreateOrGetMaterial("default", g_theRenderer));
	//Mesh *mesh = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0, 0, 0), 0.5f, 10, 10, Rgba::RED, FILL_MODE_FILL);
	//bullet->m_renderable->SetMesh(mesh);
	bullet->m_renderable->m_name = "bullet";
	m_scene->AddRenderable(bullet->m_renderable);
	bullet->m_index = static_cast<int>(m_bullets.size());
	if(isEnemy)
	{
		m_enemyBullet.push_back(bullet);
		return;
	}
	m_bullets.push_back(bullet);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Game::CreateAsteroid(Vector3 position, Vector3 forward,float radius)
{
	Asteroid *asteroid = new Asteroid();
	asteroid->SetScene(m_scene);
	asteroid->SetPosition(position);
	asteroid->m_forward = forward;
	asteroid->m_name = "asteroid";
	asteroid->m_renderable->m_name = "asteroid";
	//asteroid->m_velocity = forward*0.25;
	asteroid->m_renderable->SetMaterial(Material::CreateOrGetMaterial("Data\\Materials\\Asteroids.mat", g_theRenderer));
	asteroid->m_radius = radius;
	asteroid->m_health = static_cast<float>(radius);
	Mesh *mesh = MeshBuilder::CreateDistortedUVSpehere<Vertex_3DPCUNTB>(Vector3(0, 0, 0), radius, 32, 16,radius/8, Rgba::WHITE, FILL_MODE_FILL);
	asteroid->m_renderable->SetMesh(mesh);
	m_scene->AddRenderable(asteroid->m_renderable);
	asteroid->m_index = static_cast<int>(m_asteroids.size());
	m_asteroids.push_back(asteroid);
	m_map->AddToMapObjects(asteroid);
}

void Game::Render1()
{
	g_theRenderer->ClearScreen();
	g_theRenderer->ClearDepth();
	//g_theRenderer->m_currentShader->SetDepth(COMPARE_LESS, true);

	DebugRenderOptions options;
	options.m_lifeTime = 0.f;
	options.m_startColor = Rgba::WHITE;
	options.m_endColor = Rgba::WHITE;
	/*
	Texture *ship = g_theRenderer->CreateOrGetTexture("Data\\Model\\SciFi_Fighter-MK6-diffuse.jpg");
	Texture *objTexture = g_theRenderer->CreateOrGetTexture("Data\\Images\\example_colour.png");
	Texture *objNormalTexture = g_theRenderer->CreateOrGetTexture("Data\\Images\\example_normal.png");

	g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("lighting"));
	g_theRenderer->SetTexture(ship);

	g_theRenderer->DrawMesh(g_theRenderer->CreateOrGetMesh("Data\\Model\\scifi_fighter_mk6.obj"));
	GL_CHECK_ERROR();

	g_theRenderer->SetUniform("CAMERA_POSITION", g_theCamera::GetCurrentCamera()->m_transform.GetLocalPosition());
	EnableLight();
	Mesh *sphere = MeshBuilder::CreateUVSpehere<Vertex_3DPCUNTB>(Vector3(0, -10, 0), 1.f, 50, 50, Rgba::WHITE,FILL_MODE_FILL);
	g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("lighting"));
	g_theRenderer->SetUniform("PLIGHT_POSITION", 8, ligthPosition);
	g_theRenderer->SetUniform("PLIGHT_COLOR", 8, ligthColor);
	g_theRenderer->SetTexture(objTexture);
	g_theRenderer->BindTexture(1, objNormalTexture);
	g_theRenderer->DrawMesh(sphere);

	Mesh* cube = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(Vector3(10, -10, 0), Vector3(1, 1, 1), Rgba::WHITE,FILL_MODE_FILL);
	g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("lighting"));
	g_theRenderer->m_currentShader->SetFillMode(FILLMODE_SOLID);
	g_theRenderer->SetTexture(objTexture);
	g_theRenderer->BindTexture(1, objNormalTexture);
	g_theRenderer->DrawMesh(cube);

	Mesh* plane = MeshBuilder::Create3DPlane<Vertex_3DPCUNTB>(Vector3(-10, -10, 0), Vector3(1, 0, 0), Vector3(0, 1, 0), Vector2(2.5, 2.5), Rgba::WHITE,FILL_MODE_FILL);
	g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("lighting"));
	g_theRenderer->SetTexture(objTexture);
	g_theRenderer->BindTexture(1, objNormalTexture);
	g_theRenderer->DrawMesh(plane);*/


	/*for (int lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++)
	{
		if (g_theRenderer->m_defaultLight->IsPointLightActive(lightIndex))
		{
			if (lightIndex == 7) continue;
			Vector3 camPosition = g_theRenderer->m_defaultLight->GetPointLightPosition(lightIndex);
			Vector3 direction = g_theRenderer->m_defaultLight->GetPointLightDirection(lightIndex);

			Vector3 cubePosition = camPosition - direction;
			if(m_cameraBodyVisible)
			{
				Mesh* cube1 = MeshBuilder::CreateCube<Vertex_3DPCUNTB>(cubePosition, Vector3(0.25, 0.25, 0.25), Rgba::WHITE,FILL_MODE_FILL);
				g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("lighting"));
				g_theRenderer->SetTexture(objTexture);
				g_theRenderer->BindTexture(1, objNormalTexture);
				g_theRenderer->DrawMesh(cube1);
				delete cube1;
			}
		}
	}

	delete plane;
	delete cube;
	delete sphere;*/

	//Vector3 spherePosition(0, 0, 0);
	/*
	PointLight *currentPointLight = g_theRenderer->GetActivePointLight();
	if(currentPointLight)
	{
		spherePosition = currentPointLight->GetPosition();
	}
	g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("debug"));*/
	//debugDraw->DebugRenderSphere(spherePosition, 1.0f, 25, 25, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	//RenderDebug();
	//DebugDraw::Render();
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
void Game::RenderDebug()
{
	Texture *objTexture = Texture::CreateOrGetTexture("Data\\Images\\terrain.png");
	DebugRenderOptions optionsDepth;
	optionsDepth.m_lifeTime = 11100.f;
	optionsDepth.m_startColor = Rgba::WHITE;
	optionsDepth.m_endColor = Rgba::WHITE;
	optionsDepth.m_mode = DEBUG_RENDER_USE_DEPTH;
	if (m_isDebugDrawn == false)
	{

		DebugRenderOptions options;
		options.m_lifeTime = 11000.f;
		options.m_startColor = Rgba::WHITE;
		options.m_endColor = Rgba::WHITE;

		DebugRenderOptions options1;
		options1.m_lifeTime = 20.f;
		options1.m_startColor = Rgba::RED;
		options1.m_endColor = Rgba::WHITE;

		DebugRenderOptions optionsIgnoreDepth;
		optionsIgnoreDepth.m_lifeTime = 11100.f;
		optionsIgnoreDepth.m_startColor = Rgba::WHITE;
		optionsIgnoreDepth.m_endColor = Rgba::WHITE;
		optionsIgnoreDepth.m_mode = DEBUG_RENDER_IGNORE_DEPTH;

		DebugRenderOptions optionsXray;
		optionsXray.m_lifeTime = 11100.f;
		optionsXray.m_startColor = Rgba::WHITE;
		optionsXray.m_endColor = Rgba::WHITE;
		optionsXray.m_mode = DEBUG_RENDER_XRAY;

		
		//debugDraw->DebugRenderText(Vector3(0, 20, 0), Vector3(0, 0, 0), "SWITCH ON LIGHTS", Vector2(0, 0), 5.f, optionsDepth);
		//debugDraw->DebugRenderText(Vector3(0, 30, 0), Vector3(0, 0, 0), "TURN AROUND TO SEE DEBUG DRAWS", Vector2(0, 0), 5.f, optionsDepth);
		//
		//debugDraw->DebugRenderGrid(Vector3(0, -15, 0), Vector2(40, 40), Vector3(1, 0, 0), Vector3(0, 0, 1), Rgba::FADED_WHITE, options.m_lifeTime);
		//m_isDebugDrawn = true;
		//
		//debugDraw->DebugRenderTag(Vector3(0, -20, -80), "EXAMPLE TAG TURN AROUND", 1.f, Rgba::YELLOW, optionsDepth);
		//
		//// WALL
		//debugDraw->DebugRenderCube(Vector3(0, 0, -90), Vector3(15, 5, 2), nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
		//
		//
		//
		//debugDraw->DebugRenderPlane(Vector3(0, 0, -100), Vector2(5, 5), Vector3(0, 3.14f, 0), objTexture, Rgba::WHITE, DEBUG_RENDER_FILL, optionsIgnoreDepth);
		//debugDraw->DebugRenderText(Vector3(0, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "IGNORE DEPTH", Vector2(0, 0), 2.f, optionsDepth);
		//
		//debugDraw->DebugRenderPlane(Vector3(15, 0, -100), Vector2(5, 5), Vector3(0, 3.14f, 0), objTexture, Rgba::WHITE, DEBUG_RENDER_FILL, optionsXray);
		//debugDraw->DebugRenderText(Vector3(15, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "XRAY DEPTH", Vector2(0, 0), 2.f, optionsDepth);
		//
		//debugDraw->DebugRenderPlane(Vector3(-15, 0, -100), Vector2(5, 5), Vector3(0, 3.14f, 0), objTexture, Rgba::WHITE, DEBUG_RENDER_FILL, optionsDepth);
		//debugDraw->DebugRenderText(Vector3(-15, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "NORMAL DEPTH", Vector2(0, 0), 2.f, optionsDepth);
		//
		//debugDraw->DebugRenderText(Vector3(30, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "WIRE AABB3", Vector2(0, 0), 2.f, optionsDepth);
		//debugDraw->DebugRenderWireAABB3(Vector3(30, 0, -100), Vector3(3, 3, 3), options, Rgba::YELLOW);
		//
		//debugDraw->DebugRenderText(Vector3(-30, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "COLOR CHANGE (AND DESTROY)", Vector2(0, 0), 2.f, optionsDepth);
		//debugDraw->DebugRenderCube(Vector3(-30, 0, -100), Vector3(5, 5, 5), nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options1);
		//
		//debugDraw->DebugRenderText(Vector3(-45, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "QUAD", Vector2(0, 0), 2.f, optionsDepth);
		//debugDraw->DebugRenderQuad(Vector3(-45, 0, -100), AABB2(Vector2(0, 0), 2.5f, 2.5f), Vector3(-1, 0, 0), Vector3(0, 1, 0), objTexture, Rgba::WHITE, //DEBUG_RENDER_FILL, options);
		//
		//debugDraw->DebugRenderText(Vector3(45, 10, -100), Vector3(0, 3.14f, 3.14f / 2.f), "GLYPH", Vector2(0, 0), 2.f, optionsDepth);
		//debugDraw->DebugRenderBasis(Vector3(0, -20, -80), Vector3(0, 0, 0), 11111, 10);

	}
	g_theRenderer->SetShader(ShaderDefinitions::GetShaderByDefinitionName("debug"));
	Shader *debugShader = Shader::GetCurrentShader();
	debugShader->ResetValues();	

	DebugRenderOptions options2;
	options2.m_lifeTime = 0.005f;
	options2.m_startColor = Rgba::WHITE;
	options2.m_endColor = Rgba::WHITE;
	options2.m_mode = DEBUG_RENDER_IGNORE_DEPTH;

	//debugDraw->DebugRenderPoint(g_theCamera::GetGamePlayCamera()->m_transform.GetLocalPosition(), 5, Rgba::WHITE, Rgba::RED);
	 
	//Vector3 rotationEuler = g_theCamera::GetGamePlayCamera()->m_transform.GetLocalEulerAngles();
	//debugDraw->DebugRenderLogf("rotationXYZ  %f %f %f ", rotationEuler.x, rotationEuler.y, rotationEuler.z);

	optionsDepth.m_lifeTime = 0.05f;
	/*debugDraw->DebugRenderGlyph(Vector3(45, 0, -100), Vector2(5, 5), objTexture, Rgba::WHITE, optionsDepth);
	 
	if(m_enable2D)
	{
		debugDraw->DebugRenderQuad2D(Vector3(300,300,0), AABB2(Vector2(0, 0), 30, 30), 20, objTexture, Rgba::WHITE, DEBUG_RENDER_FILL, options2);

		debugDraw->DebugRenderLine2D(Vector3(100, 100, 0), Vector3(100, 500, 0), Rgba::YELLOW, options2);

		debugDraw->DebugRenderText2D(Vector3(500, 100, 0),Vector3(0,0,45.f), "TEXT 2D ", Vector2(0, 0), 20.f, Rgba::RED, options2);

	}*/



}


