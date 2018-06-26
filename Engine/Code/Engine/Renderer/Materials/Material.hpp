#pragma once
#include <vector>
#include <map>

#include "Engine/Core/Rgba.hpp"
#include "Engine/TinyXml2/tinyxml2.h"
/*\class  : Materials
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
* \date   : 4/24/2018 5:03:20 PM
*
* \contact: srsrakhil@gmail.com
*/
class Shader;
class Texture;
class Sampler;
class MaterialProperty;
class PropertyBlockInfo;
class PropertyBlock;
class Renderer;
using namespace tinyxml2;

class Material
{

public:
	//Member_Variables
	Shader * m_shader = nullptr;
	bool     m_isOpaque = false;
	std::vector<Texture*> m_textures;
	std::vector<Sampler*> m_samplers;
	std::map<std::string,MaterialProperty*> m_properties;
	std::vector<std::string> m_propertyNames;
	//Static_Member_Variables
	static std::map<std::string, Material*> s_materials;
	//Methods

	Material();
	Material(Material &material);
	~Material();
	
	template <typename T>
	void SetProperty(char *name, T v)
	{
		SetProperty(name, v, sizeof(T));
	}

	bool            IsValid();
	void			BindAll(int programHandle);
	void			Bind(std::string propertyName,int programHandle);
	void            SetShader(Shader *shader);
	Shader*         GetShader();
	PropertyBlock*  CreateOrGetBlock(PropertyBlockInfo *block_info);
			  
			  
	void			SetTexture2D(int bindId,Texture *texture);
	void			SetSampler(int bindId, Sampler *sampler);
	Sampler*		GetSampler(int index);
	Texture*		GetTexture(int index);
					
	void			RemoveProperty(std::string name);
	void			SetProperty(std::string name, float value     , size_t data_size);
	void			SetProperty(std::string name, int   value     , size_t data_size);
	void			SetProperty(std::string name, Rgba  value     , size_t data_size);
	void			SetProperty(std::string name, Vector3 value   , size_t data_size);
	void			SetProperty(std::string name, Vector4 value   , size_t data_size);
	void			SetProperty(std::string name, void const *data, size_t data_size);
	Material *		Clone();
	//Static_Methodsk
	static Material* CreateOrGetMaterial1(std::string filepath);
	static Material* CreateOrGetMaterial1(std::string filepath, bool isNewResource);
	static Material* AquireResource(std::string filepath);
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