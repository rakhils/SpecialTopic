#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"

// CONSTRUCTOR
ParticleEmitter::ParticleEmitter(Renderer *renderer, Scene *scene)
{
	m_renderer	  = renderer;
	m_scene		  = scene;
	m_camera	  = Camera::GetCurrentCamera();
	m_colorEnd	  = Rgba::WHITE;
	m_colorStart  = Rgba::WHITE;
}

// DESTRUCTOR
ParticleEmitter::~ParticleEmitter()
{
	for(int index = 0;index < m_renderables.size();index++)
	{
		if(m_renderables.at(index)!=nullptr)
		{
			delete m_renderables.at(index);
			m_renderables.erase(m_renderables.begin() + index);
			index--;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : Set position of emitter
*@param   : world position
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::SetPosition(Vector3 position)
{
	m_transform.SetLocalPosition(position);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Updates particle emission
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::Update(float deltaTime)
{
	Update(m_camera, deltaTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Updates particle with cameras direction
*@param   : Camera , delta
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::Update(Camera *camera, float deltaTime)
{
	for(int renderableIndex = 0;renderableIndex < m_renderables.size();renderableIndex++)
	{
		m_scene->RemoveRenderable(m_renderables.at(renderableIndex));
		if(m_renderables.at(renderableIndex)->m_mesh != nullptr)
		{
			delete m_renderables.at(renderableIndex)->m_mesh;
			m_renderables.at(renderableIndex)->m_mesh = nullptr;
		}
		if(m_renderables.at(renderableIndex) != nullptr)
		{
			m_renderables.at(renderableIndex)->m_material = nullptr;
			delete m_renderables.at(renderableIndex);
			m_renderables.at(renderableIndex)= nullptr;
		}
		m_renderables.erase(m_renderables.begin() + renderableIndex);
		m_renderables.shrink_to_fit();
		m_renderables.resize(m_renderables.size());
		renderableIndex--;
	}

	for (int particleIndex = 0; particleIndex < m_particles.size(); particleIndex++)
	{
		m_particles.at(particleIndex).m_force = Vector3(0, -3.0f, 0);
		m_particles.at(particleIndex).Update(deltaTime);

		if (!m_particles.at(particleIndex).isAlive(static_cast<float>(Clock::g_theMasterClock->total.m_seconds)))
		{
			m_particles.erase(m_particles.begin() + particleIndex);
			m_particles.shrink_to_fit();
			m_particles.resize(m_particles.size());
			particleIndex--;
		}
	}

	MeshBuilder mb;
	Vector3 cameraRight = camera->GetCameraRightVector();
	Vector3 cameraUp = camera->GetCameraUpVector();
	MeshBuilder meshBuilder;
	for (int particleIndex = 0; particleIndex < m_particles.size(); particleIndex++)
	{
		const Particle& particle = m_particles.at(particleIndex);
		float currentTime = static_cast<float>(Clock::g_theMasterClock->total.m_seconds);

		float factor = (currentTime - particle.m_startTime) / (particle.m_endTime - particle.m_startTime);
		factor    = ClampFloat(factor, 0, 1);
		Rgba color = Interpolate(m_colorStart, m_colorEnd, factor);
		MeshBuilder::Create3DPlane(meshBuilder,particle.m_position, cameraRight, cameraUp, Vector2(particle.m_size, particle.m_size), color, FILL_MODE_FILL);
	}
	if (m_particles.size() > 0)
	{
		Renderable *renderable = new Renderable();
		renderable->SetMaterial(Material::CreateOrGetMaterial1("Data\\Materials\\Particles.mat", m_renderer));
		Mesh *mesh = meshBuilder.CreateMesh();
		renderable->SetMesh(mesh);
		renderable->m_name = "particles";
		m_renderables.push_back(renderable);
	}
	AddRenderablesToScene();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Adds the renderables in the current scene
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::AddRenderablesToScene()
{
	for(int rednerableIndex = 0;rednerableIndex < m_renderables.size();rednerableIndex++)
	{
		m_scene->AddRenderable(m_renderables.at(rednerableIndex));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Spawn one particle
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::SpawnParticle()
{
	Particle particle;
	particle.m_position  = m_transform.GetWorldPosition();
	particle.m_velocity  = Vector3::GetRandomValueOnUnitSphere();
	particle.m_size		 = GetRandomFloatInRange(0.2f,0.6f);
	particle.m_lifeTime  = GetRandomFloatInRange(2.f, 3.f);

	particle.m_startTime = static_cast<float>(Clock::g_theMasterClock->total.m_seconds);
	particle.m_endTime   = particle.m_startTime + particle.m_lifeTime;
	particle.m_force     = Vector3::ONE;
	particle.m_mass		 = 1.f;
	m_particles.push_back(particle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Spawn n particles
*@param   : count (n)
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::SpawnParticles(int count)
{
	for(int index = 0;index < count;index++)
	{
		SpawnParticle();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : Sets color of particle
*@param   : Color
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::SetColor(Vector4 color)
{
	m_colorStart = color;
	m_colorEnd   = color;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : Sets camera
*@param   : camera pointer
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::SetCamera(Camera *camera)
{
	m_camera = camera;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Sets spawn rate
*@param   : number of particle per second
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::SetSpawnRate(float particlePerSecond)
{
	m_particleGeneratedPerSecond = particlePerSecond;
	if (m_particleGeneratedPerSecond == 0.0f) 
	{
		m_spawnOverTime = false;
	}
	else 
	{
		m_spawnOverTime = true;
		m_spawnInterval = 1.0f / m_particleGeneratedPerSecond;
	}
}
