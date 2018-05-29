#pragma once
#include "Engine/Renderer/External/gl/glcorearb.h"
#include "Engine/Renderer/External/gl/glext.h"
#include "Engine/Renderer/External/gl/wglext.h"
#include "Engine/Renderer/GL/glfunctions.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/EngineCommon.hpp"
/*\class  : MaterialProperty
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 4/27/2018 2:50:52 PM
* \contact: srsrakhil@gmail.com
*/
class MaterialProperty
{

public:
	//Member_Variables
	std::string m_name;
	//Static_Member_Variables

	//Methods

	MaterialProperty();
	~MaterialProperty();
	virtual MaterialProperty *Clone() = 0;
	
	virtual void Bind(int programHandle) = 0;
	//Static_Methods
};

class MaterialPropertyFloat : public MaterialProperty
{
public:
	float m_value = 0.f;
	MaterialPropertyFloat(std::string name,float value)
	{
		m_name  = name;
		m_value = value;
	}
	MaterialPropertyFloat(const MaterialPropertyFloat &obj);
	~MaterialPropertyFloat(){}
	
	void Bind(int programHandle)
	{
		GLuint idx = glGetUniformLocation(programHandle, m_name.c_str());
		if (idx >= 0) 
		{
			glUniform1fv(idx, 1, (GLfloat*)&m_value);
		}
	}

	MaterialProperty * Clone()
	{
		return new MaterialPropertyFloat(*this);
	}
private:

};

class MaterialPropertyVector2 : public MaterialProperty
{
public:
	Vector2 m_value;
	MaterialPropertyVector2(std::string name, Vector2 value)
	{
		m_name = name;
		m_value = value;
	}
	MaterialPropertyVector2(const MaterialPropertyVector2 &obj);
	~MaterialPropertyVector2() {}

	void Bind(int programHandle)
	{
		GLuint idx = glGetUniformLocation(programHandle, m_name.c_str());
		if (idx >= 0)
		{
			//glUniform2fv(idx, 1, (GLfloat*)&m_value);
		}
	}
	MaterialProperty * Clone()
	{
		return new MaterialPropertyVector2(*this);
	}
};

class MaterialPropertyVector3 : public MaterialProperty
{
public:
	Vector3 m_value;
	MaterialPropertyVector3(std::string name, Vector3 value)
	{
		m_name = name;
		m_value = value;
	}
	MaterialPropertyVector3(const MaterialPropertyVector3 &obj);
	~MaterialPropertyVector3() {}

	void Bind(int programHandle)
	{
		GLuint idx = glGetUniformLocation(programHandle, m_name.c_str());
		if (idx >= 0)
		{
			glUniform3fv(idx, 1, (GLfloat*)&m_value);
		}
	}
	MaterialProperty * Clone()
	{
		return new MaterialPropertyVector3(*this);
	}
};

class MaterialPropertyVector4 : public MaterialProperty
{
public:
	Vector4 m_value;
	MaterialPropertyVector4(std::string name, Vector4 value)
	{
		m_name = name;
		m_value = value;
	}
	MaterialPropertyVector4(const MaterialPropertyVector4 &obj);
	~MaterialPropertyVector4() {}

	void Bind(int programHandle)
	{
		GLint uniformfloat = glGetUniformLocation(programHandle, m_name.c_str());
		if (uniformfloat >= 0)
		{
			glUniform4f(uniformfloat, (GLfloat)m_value.x, (GLfloat)m_value.y, (GLfloat)m_value.z, (GLfloat)m_value.w);
		}
	}
	MaterialProperty * Clone()
	{
		return new MaterialPropertyVector4(*this);
	}
};
