#include "Engine/Renderer/ParticleSystem/ParticleSystem.hpp"
std::vector<ParticleEmitter*> ParticleSystem::s_particleEmitters;
// CONSTRUCTOR
ParticleSystem::ParticleSystem()
{

}

// DESTRUCTOR
ParticleSystem::~ParticleSystem()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
ParticleEmitter * ParticleSystem::CreateOrGetParticleEmiiter(std::string emitterName)
{
	//ParticleEmitter *particleEmitter = new ParticleEmitter();
	/*if(s_particleEmitters[emitterName] == nullptr)
	{
		ParticleEmitter *particleEmitter = new ParticleEmitter();
		s_particleEmitters[emitterName] = particleEmitter;
	}*/
	//s_particleEmitters.push_back(particleEmitter);
	//return particleEmitter;re
	return nullptr;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void ParticleSystem::UpdateParticleSystem(float deltatime)
{
	for(int index = 0;index < s_particleEmitters.size();index++)
	{
		s_particleEmitters.at(index)->Update(s_particleEmitters.at(index)->m_camera,deltatime);
		if(s_particleEmitters.at(index)->m_particles.size() == 0)
		{
			delete s_particleEmitters.at(index);
			s_particleEmitters.erase(s_particleEmitters.begin() + index);
			index--;
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void ParticleSystem::DeleteParticleEmitter(ParticleEmitter* emitter)
{
	for (int index = 0; index < s_particleEmitters.size(); index++)
	{
		if(s_particleEmitters.at(index) == emitter)
		{
			delete s_particleEmitters.at(index);
			s_particleEmitters.erase(s_particleEmitters.begin() + index);
			index--;
		}
	}
}
