#pragma once
#include "Engine/Renderer/External/gl/glcorearb.h"
/*\class  : Sampler
*		   
* \group  : <GroupName>
*		   
* \brief  : Sampler class to define how a texture should be used
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 2/3/2018 6:27:56 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class Sampler
{

public:
	//Member_Variables
	GLuint m_sampler_handle = NULL;
	//Static_Member_Variables
	static Sampler *s_defaultSampler;
	static Sampler *s_currentSampler;
	static Sampler *s_effectSampler; 
	float    m_nearValue = -1000;
	float	   m_farValue  = 1000;
	//Methods

	Sampler();
	~Sampler();
	bool Create();
	void Destroy();
	GLuint GetHandle();
	//Static_Methods
	static void     SetDefaultSampler(Sampler *sampler) { s_defaultSampler = sampler; }
	static void     SetCurrentSampler(Sampler *sampler) { s_currentSampler = sampler; }
	static void     SetEffectSampler (Sampler *sampler) { s_effectSampler  = sampler; }

	static Sampler* GetDefaultSampler() { return s_defaultSampler; }
	static Sampler* GetCurrentSampler() { return s_currentSampler; }
	static Sampler* GetEffectSampler()  { return s_effectSampler;  }

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