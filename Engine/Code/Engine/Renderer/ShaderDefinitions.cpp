#include "Engine/Renderer/ShaderDefinitions.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"

std::map<std::string, Shader*> ShaderDefinitions::s_shaders;
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/09
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void ShaderDefinitions::LoadDefintions(std::string xmlPath,Renderer *renderer)
{
	TinyXMLDocument doc;
	doc.LoadFile(xmlPath.c_str());
	XMLElement *xmlRootElement = doc.FirstChildElement("ShaderDefinitions");
	XMLElement *xmlChildElement = xmlRootElement->FirstChildElement("Shaders");
	for (; xmlChildElement != nullptr; xmlChildElement = xmlChildElement->NextSiblingElement())
	{
		ShaderDefinitions *shaderDef = new ShaderDefinitions(*xmlChildElement);
		

		Shader *shader = shaderDef->GetShaderFromDefinition(renderer);
		shader->m_shaderDef = shaderDef;
	}
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
Shader* ShaderDefinitions::GetShaderByDefinitionName(std::string name)
{
	return s_shaders[name];
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
Shader* ShaderDefinitions::GetShaderFromDefinition(Renderer *renderer)
{
	if(s_shaders[m_name] == nullptr)
	{
		Shader *shaderNew = new Shader(renderer);
		SetShaderInitValues(shaderNew);
		s_shaders[m_name] = shaderNew;

		ShaderProgram *program = new ShaderProgram();
		program->CreateShadersFrom(m_vertexShaderFileName.c_str(), m_fragmentShaderFileName.c_str(), GL_FRAGMENT_SHADER, m_defines.c_str());
		shaderNew->m_shaderProgram = program;
	}
	return s_shaders[m_name];
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
void ShaderDefinitions::SetShaderInitValues(Shader *shader)
{
	shader->m_state.m_color_blend_op   = Renderer::GetBlendOperationFromString(m_colorOperation);
	shader->m_state.m_color_src_factor = Renderer::GetBlendFactorFromString(m_colorSrc);
	shader->m_state.m_color_dst_factor = Renderer::GetBlendFactorFromString(m_colorDst);

	shader->m_state.m_alpha_blend_op   = Renderer::GetBlendOperationFromString(m_alphaOperation);
	shader->m_state.m_alpha_src_factor = Renderer::GetBlendFactorFromString(m_alphaSrc);
	shader->m_state.m_alpha_dst_factor = Renderer::GetBlendFactorFromString(m_alphaDst);

	shader->m_state.m_cull_mode		   = Renderer::GetCullModeFromString(m_cull_mode);
	shader->m_state.m_depth_compare    = Renderer::GetDepthCompareFromString(m_depth_compare);
	shader->m_state.m_fill_mode        = Renderer::GetFillModeFromString(m_fill_mode);
	shader->m_state.m_front_face       = Renderer::GetWindOrderFromString(m_front_face);
	shader->m_state.m_depth_write      = m_depthWrite;
	shader->m_defines				   = m_defines; 
}

// CONSTRUCTOR
ShaderDefinitions::ShaderDefinitions(XMLElement& xmlElement)
{
	/*
	SAMPLE
	<ShaderDefinitions>
	<Shaders name="lit_alpha">
	<program define = "USE_DIFFUSE;USE_SPECULAR">
	<vertex   file = "shader/glsl/mvp_lit.vs" / >
	<fragment file = "shader/glsl/lit.fs" / >
	< / program>

	<blend>
	<color operation = "add" src = "src_alpha" dest = "inv_src_alpha" / >
	< / blend>

	<depth write = "false" / >

	< / Shaders>
	< / ShaderDefinitions>
	*/
	m_name = ParseXmlAttribute(xmlElement, "name", m_name);
	XMLElement *childElement = xmlElement.FirstChildElement();
	for (; childElement != nullptr; childElement = childElement->NextSiblingElement())
	{
		const char* childValue = childElement->Value();
		std::string childValueStr; childValueStr.append(childValue);

		if (childValueStr == "program")
		{
			XMLElement *xmlProgramElement = xmlElement.FirstChildElement("program");
			m_defines = ParseXmlAttribute(*xmlProgramElement, "define", m_defines);
	
			XMLElement *xmlVertexElement = xmlProgramElement->FirstChildElement("vertex");
			XMLElement *xmlFragmentElement = xmlProgramElement->FirstChildElement("fragment");

			m_vertexShaderFileName = ParseXmlAttribute(*xmlVertexElement, "file", m_vertexShaderFileName);
			m_fragmentShaderFileName = ParseXmlAttribute(*xmlFragmentElement, "file", m_fragmentShaderFileName);
		}

		if (childValueStr == "blend")
		{
			XMLElement *xmlBlendElement = xmlElement.FirstChildElement("blend");
			XMLElement *xmlColorElement = xmlBlendElement->FirstChildElement("color");
			XMLElement *xmlAlphaElement = xmlBlendElement->FirstChildElement("alpha");

			if (xmlColorElement != nullptr)
			{
				m_colorOperation = ParseXmlAttribute(*xmlColorElement, "operation", m_colorOperation);
				m_colorSrc = ParseXmlAttribute(*xmlColorElement, "src", m_colorSrc);
				m_colorDst = ParseXmlAttribute(*xmlColorElement, "dest", m_colorDst);
			}

			if (xmlAlphaElement != nullptr)
			{
				m_alphaOperation = ParseXmlAttribute(*xmlAlphaElement, "operation", m_alphaOperation);
				m_alphaSrc = ParseXmlAttribute(*xmlAlphaElement, "src", m_alphaSrc);
				m_alphaDst = ParseXmlAttribute(*xmlAlphaElement, "dest", m_alphaDst);
			}
		}

		if (childValueStr == "depth")
		{
			XMLElement *xmlDepthElement = xmlElement.FirstChildElement("depth");
			m_depthWrite = ParseXmlAttribute(*xmlDepthElement, "write", m_depthWrite);
			m_depth_compare = ParseXmlAttribute(*xmlDepthElement, "compare", m_depth_compare);
		}

		if (childValueStr == "cull")
		{
			XMLElement *xmlCullElement = xmlElement.FirstChildElement("cull");
			m_cull_mode = ParseXmlAttribute(*xmlCullElement, "mode", m_cull_mode);
		}

		if (childValueStr == "fill")
		{
			XMLElement *xmlFillElement = xmlElement.FirstChildElement("fill");
			m_fill_mode = ParseXmlAttribute(*xmlFillElement, "mode", m_fill_mode);
		}

		if (childValueStr == "windorder")
		{
			XMLElement *xmlFrontElement = xmlElement.FirstChildElement("windorder");
			m_front_face = ParseXmlAttribute(*xmlFrontElement, "order", m_front_face);
		}
	}
}

// DESTRUCTOR
ShaderDefinitions::~ShaderDefinitions()
{

}
