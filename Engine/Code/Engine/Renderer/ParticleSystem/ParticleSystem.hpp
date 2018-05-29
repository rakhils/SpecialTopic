#pragma once
#include <map>

#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
/*\class  : ParticleSystem
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 5/4/2018 10:23:06 PM
*
* \contact: srsrakhil@gmail.com
*/
class ParticleSystem
{

public:
	//Member_Variables
	
	//Static_Member_Variables
	//static std::map<std::string,ParticleEmitter*> s_particleEmitters;
	static std::vector<ParticleEmitter*> s_particleEmitters;
	//Methods

	ParticleSystem();
	~ParticleSystem();

	//Static_Methods
	static ParticleEmitter*	CreateOrGetParticleEmiiter(std::string emitterName);
	static void				UpdateParticleSystem(float deltatime);
	static void			    DeleteParticleEmitter(ParticleEmitter* emitter);

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