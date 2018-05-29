#pragma once
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/wglext.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/Renderer.hpp"

/*\class  : Shader   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 4/4/2018 1:15:52 PM
* \contact: srsrakhil@gmail.com
*/
enum eRenderQueue
{
	RENDER_QUEUE_OPAQUE,
	RENDER_QUEUE_ALPHA,
};
class ShaderProgram;
class ShaderDefinitions;
class Shader
{

public:
	//Member_Variables
	int                m_sortOrder = 0;
	eRenderQueue       m_renderQueue; 
	ShaderDefinitions *m_shaderDef = nullptr;
	Renderer		  *m_renderer = nullptr;
	ShaderProgram	  *m_shaderProgram = nullptr;
	RenderState        m_state;
	std::string        m_defines;
	//Static_Member_Variables
	

	//Methods
	Shader(Renderer *renderer);
	Shader(const Shader &shaderObj);
	Shader();
	~Shader();

	void SetProgram(ShaderProgram *shaderProgram);
	ShaderProgram* GetShaderProgram() { return m_shaderProgram; }

	void EnableColorBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst);
	void EnableAlphaBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst);
	void DisableBlending();

	void SetDepth(eCompare compare, bool write);
	void DisableDepth() { SetDepth(COMPARE_ALWAYS, false); }

	void SetCullMode(eCullMode cullMode);
	void SetFillMode(eFillMode fillMode);
	void SetFrontFace(eWindOrder windOrder);

	void ResetValues();
	//Static_Methods

	static Shader *GetCurrentShader() { return s_currentShader;}
	static Shader *GetDefaultShader() { return s_defaultShader;}

	static void    SetCurrentShader(Shader *shader) { s_currentShader = shader;}
	static void    SetDefaultShader(Shader *shader) { s_defaultShader = shader;}

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables
	static Shader  *s_defaultShader;
	static Shader  *s_currentShader;
	//Static_Member_Variables

	//Methods

	//Static_Methods

};