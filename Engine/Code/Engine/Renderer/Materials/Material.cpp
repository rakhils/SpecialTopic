#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Materials/MaterialProperty.hpp"
#include "Engine/Renderer/Shaders/ShaderProgramInfo.hpp"
#include "Engine/Renderer/ShaderDefinitions.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/PropertyBlockInfo.hpp"
#include "Engine/Renderer/Texture.hpp"
// CONSTRUCTOR

std::map<std::string, Material*> Material::s_materials;

Material::Material()
{
	m_propertyNames.push_back("TINT");
}

// DESTRUCTOR
Material::~Material()
{
	std::map<std::string, MaterialProperty*>::iterator it = m_properties.begin();
	for (; it != m_properties.end(); it++)
	{
		delete it->second;
	}
	delete m_shader;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Material* Material::CreateOrGetMaterial1(std::string filepath)
{
	return CreateOrGetMaterial1(filepath, false);
}

Material* Material::CreateOrGetMaterial1(std::string filepath,bool isNewResource)

{
	/*<material id = "ship">
	<shader id = "lit" / >

	<texture bind = "0" src = "image/ship.png" / >
	<texture bind = "1" src = "image/ship.normal.png" / >

	<float bind = "SPEC_FACTOR" value = "1.0f" / > 
	<float bind = "SPEC_POWER" value = "16.0f" / >
	<color bind = "TINT" value = ".8, .8, 1.0f, 1.0" / >
	< / material>*/
	if(s_materials[filepath] != nullptr)
	{
		return s_materials[filepath];
	}
	if(filepath == "default")
	{
		Material *defaultMaterial = new Material();
		defaultMaterial->SetTexture2D(0U,Texture::GetDefaultTexture());
		defaultMaterial->SetSampler(0U,  Sampler::GetDefaultSampler());
		defaultMaterial->SetShader(      Shader::GetDefaultShader());
		s_materials[filepath] = defaultMaterial;
		return defaultMaterial;
	}
	Material *material = new Material();
	TinyXMLDocument doc;
	doc.LoadFile(filepath.c_str());
	XMLElement *xmlRootElement = doc.FirstChildElement("material");
	std::string materialId = xmlRootElement->Attribute("id");

	XMLElement *childElement = xmlRootElement->FirstChildElement();
	for (; childElement != nullptr; childElement = childElement->NextSiblingElement())
	{
		const char* childValue = childElement->Value();
		std::string childValueStr; childValueStr.append(childValue);

		if (childValueStr == "shader")
		{
			std::string id     = childElement->Attribute("id");
			if(isNewResource)
			{
				material->m_shader = new Shader(*ShaderDefinitions::GetShaderByDefinitionName(id));
			}
			else
			{
				material->m_shader = ShaderDefinitions::GetShaderByDefinitionName(id);
			}
		}

		if (childValueStr == "opaque")
		{
			material->m_isOpaque = ParseXmlAttribute(*childElement, "value", false); 
		}

		if (childValueStr == "texture")
		{
			const int bindId     = ParseXmlAttribute(*childElement, "bind", 0);
			std::string srcImage = childElement->Attribute("src");
			Texture *texture     = Texture::CreateOrGetTexture(srcImage.c_str());
			texture->m_slot = bindId;
			material->SetTexture2D(bindId,texture);
			material->SetSampler(bindId, Sampler::GetDefaultSampler());
		}

		if (childValueStr == "float")
		{
			std::string bindName = childElement->Attribute("bind");
			float value	         = ParseXmlAttribute(*childElement, "value", 0.0f);
			material->SetProperty((char*)bindName.c_str(), value,sizeof(float));
		}

		if(childValueStr == "vector4")
		{
			std::string bindName = childElement->Attribute("bind");
			Rgba value		     = ParseXmlAttribute(*childElement,"value",Rgba::WHITE);
			material->SetProperty(bindName.c_str(), value.GetAsFloats(),sizeof(Vector4));
		}
	}
	s_materials[filepath] = material;
	return material;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Creates new instance of material
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Material* Material::AquireResource(std::string filepath)
{
	Material *material = CreateOrGetMaterial1(filepath,true);
	return material->Clone();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Material::IsValid()
{
	if(m_shader == nullptr)
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/01
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::BindAll(int programHandle)
{
	std::map<std::string, MaterialProperty*>::iterator it = m_properties.begin();
	for (; it != m_properties.end(); it++)
	{
		Bind(m_properties[it->first]->m_name, programHandle);
	}

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/01
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::Bind(std::string propertyName,int programHandle)
{
	if(m_properties[propertyName] == nullptr)
	{
		Vector4 value(0, 0, 0, 0);
		GLuint idx = glGetUniformLocation(programHandle, propertyName.c_str());
		if (idx >= 0)
		{
			glUniform4f(idx, (GLfloat)value.x, (GLfloat)value.y, (GLfloat)value.z, (GLfloat)value.w);
			//glUniform4fv(idx, 4, (GLfloat*)value.GetAsArray());
		}
		return;
	}
	m_properties[propertyName]->Bind(programHandle);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::SetShader(Shader *shader)
{
	m_shader = shader;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Shader* Material::GetShader()
{
	return m_shader;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::SetTexture2D(int bindId, Texture *texture)
{
	UNUSED(bindId);
	m_textures.push_back(texture);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::SetSampler(int bindId, Sampler *sampler)
{
	UNUSED(bindId);
	m_samplers.push_back(sampler);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::RemoveProperty(std::string name)
{
	if(m_properties[(name)] != nullptr)
	{
		delete m_properties[(name)];
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/27
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::SetProperty(std::string name, float value,size_t data_size)
{
	UNUSED(data_size);
	RemoveProperty(name);

	MaterialPropertyFloat *materialProperty = new MaterialPropertyFloat(name, value);
	m_properties[std::string(name)] = materialProperty;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::SetProperty(std::string name, Vector3 value,size_t data_size)
{
	UNUSED(data_size);
	RemoveProperty(name);

	MaterialPropertyVector3 *materialProperty = new MaterialPropertyVector3(name, value);
	m_properties[std::string(name)] = materialProperty;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/01
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Material::SetProperty(std::string name, Vector4 value, size_t data_size)
{
	UNUSED(data_size);
	RemoveProperty(name);

	MaterialPropertyVector4 *materialProperty = new MaterialPropertyVector4(name, value);
	m_properties[std::string(name)] = materialProperty;
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
void Material::SetProperty(std::string name, Rgba value, size_t data_size)
{
	UNUSED(data_size);
	UNUSED(value);
	RemoveProperty(name);
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Material * Material::Clone()
{
	Material *materialClone = new Material();
	materialClone->m_shader = new Shader(*m_shader);
	materialClone->m_textures = m_textures;
	materialClone->m_samplers = m_samplers;
	materialClone->m_isOpaque = m_isOpaque;

	std::map<std::string, MaterialProperty*>::iterator it = m_properties.begin();
	for (; it != m_properties.end(); it++)
	{
		MaterialProperty *property = m_properties[it->first]->Clone();
		materialClone->m_properties[it->first] = property;
	}

	//materialClone->m_properties = m_properties;

	materialClone->m_propertyNames = m_propertyNames;
	return materialClone;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
/*
void Material::SetProperty(char const *name, void const *data, size_t data_size)
{
	ShaderProgramInfo *shaderInfo  = m_shader->GetShaderProgram()->GetInfo();
	PropertyInfo      *info        = shaderInfo->GetPropertyInfo(name);
	PropertyBlockInfo *block_info  = info->m_owningBlock;
	PropertyBlock     *block       = CreateOrGetBlock(block_info);

	block->SetGPUBuffer(info->m_offset,data_size,data);
}*/
//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/28
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
/*
PropertyBlock* Material::CreateOrGetBlock(PropertyBlockInfo *block_info)
{
	// vector/map/some container on material
	PropertyBlock *block = m_properties[block_info->m_name];
	if (block == nullptr) 
	{
		block = new PropertyBlock();
		block->set_size(block_info.size); // set cpu blob size, marks as dirty
		block->m_info = block_info;

		m_prop_blocks.add(block_info.name, block);
	}

	return block;
}*/


