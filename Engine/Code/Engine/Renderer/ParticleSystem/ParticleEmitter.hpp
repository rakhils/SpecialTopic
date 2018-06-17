#pragma once
#include <vector>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"
#include "Engine/Core/Component.hpp"

/*\class  : ParticleEmitter   
* \group  : <GroupName>	   
* \brief  :	Used to create particle effects in game   
* \TODO:  :	   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 5/4/2018 10:23:35 PM
* \contact: srsrakhil@gmail.com
*/
struct Particle
{
	Vector3 m_position;
	Vector3 m_velocity;
	Vector3 m_force = 5;
	float   m_size;
	float   m_mass;

	float m_startTime;
	float m_endTime;
	float m_lifeTime;

	void Update(float deltaTime)
	{
		Vector3 acceleration = m_force/ m_mass;
		m_velocity += acceleration * deltaTime;
		m_position += m_velocity * deltaTime;

		//m_force = Vector3::ZERO;
	}

	bool isAlive(float time) 
	{ 
		return time < m_endTime; 
	}

	float GetNormalizedAge(float time)
	{
		return (time - m_startTime / m_endTime - time);
	}
};
class Camera;
class Renderable;
class Scene;
class ParticleEmitter : public Component
{

public:
	//Member_Variables
	Renderer					*m_renderer						= nullptr;
	Scene					    *m_scene						= nullptr;
	Camera						*m_camera						= nullptr;
	Mesh						*m_mesh							= nullptr;
	std::vector<Renderable*>	m_renderables;
	std::vector<Mesh *>			m_meshes;
	//Transform				    m_transform;
	MeshBuilder					m_builder;
	std::vector<Particle>		m_particles;
	bool						m_spawnOverTime					= false;
	float						m_spawnInterval					= 5.f;
	float						m_size							= 5.f;
	float						m_particleGeneratedPerSecond	= 5.f;
	Rgba						m_colorStart;
	Rgba						m_colorEnd;
	//Static_Member_Variables

	//Methods
	ParticleEmitter(Renderer *renderer,Scene *scene);
	~ParticleEmitter();
	
	void SetPosition(Vector3 position);

	void Update(float deltaTime);
	void Update(Camera *cam, float dt);
	void AddRenderablesToScene();
	void SpawnParticle();
	void SpawnParticles(int count);
	void SetColor(Vector4 color);
	void SetCamera(Camera *camera);
	void SetSpawnRate(float particlePerSecond);

public:
	
	//TRange<uint> m_burst;
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