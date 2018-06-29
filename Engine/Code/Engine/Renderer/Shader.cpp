#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderDefinitions.hpp"
// CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Shader  *Shader::s_defaultShader = nullptr;
Shader  *Shader::s_currentShader = nullptr;
Shader::Shader(Renderer *renderer)
{
	m_renderer = renderer;
	SetCullMode(CULLMODE_BACK);
	SetFillMode(FILLMODE_SOLID);
	SetFrontFace(WIND_COUNTER_CLOCKWISE);
	EnableColorBlending(COMPARE_ADD, BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
	EnableAlphaBlending(COMPARE_ADD, BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
	DisableDepth();
}

Shader::Shader(const Shader &shaderObj)
{
	m_sortOrder		 = shaderObj.m_sortOrder;
	m_renderQueue	 = shaderObj.m_renderQueue;
	m_state			 = shaderObj.m_state;
	m_defines		 = shaderObj.m_defines;
	m_renderer		 = shaderObj.m_renderer;	  // CAN BE SAME POINTER
	m_shaderDef		 = shaderObj.m_shaderDef;	  // CAN BE SAME POINTER 
	m_shaderProgram  = shaderObj.m_shaderProgram; // CAN BE SAME POINTER 
}

// DESTRUCTOR
Shader::~Shader()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::SetProgram(ShaderProgram *shaderProgram)
{
	m_shaderProgram = shaderProgram;
	//SetShaderProgram(shaderProgram);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::EnableColorBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst)
{
	m_state.m_color_blend_op = op;
	m_state.m_color_src_factor = src;
	m_state.m_color_dst_factor = dst;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::EnableAlphaBlending(eBlendOperation op, eBlendFactor src, eBlendFactor dst)
{
	m_state.m_alpha_blend_op = op;
	m_state.m_alpha_src_factor = src;
	m_state.m_alpha_dst_factor = dst;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::SetDepth(eCompare compare, bool write)
{
	m_state.m_depth_compare = compare;
	m_state.m_depth_write = write;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::SetCullMode(eCullMode cullMode)
{
	m_state.m_cull_mode = cullMode;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::SetFillMode(eFillMode fillMode)
{
	m_state.m_fill_mode = fillMode;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::SetFrontFace(eWindOrder windOrder)
{
	m_state.m_front_face = windOrder;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/09
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Shader::ResetValues()
{
	if(m_shaderDef != nullptr)
	{
		SetCullMode(CULLMODE_BACK);
		SetFillMode(FILLMODE_SOLID);
		SetFrontFace(WIND_COUNTER_CLOCKWISE);
		EnableColorBlending(COMPARE_ADD, BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
		EnableAlphaBlending(COMPARE_ADD, BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
		DisableDepth();
		m_shaderDef->SetShaderInitValues(this);
	}
}
