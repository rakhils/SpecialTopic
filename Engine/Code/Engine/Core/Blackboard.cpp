#include "Blackboard.hpp"

void Blackboard::PopulateFromXmlElementAttributes(const XMLElement& element)
{
	std::string name = element.GetText();
}

//////////////////////////////////////////////////////////////////////////
//@purpose : Inserting value into map
void Blackboard::SetValue(const std::string& keyName, const std::string& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

bool Blackboard::GetValue(const std::string& keyName, bool defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		if(value.compare("true"))
		{
			return true;
		}
		return false;
	}
	return defaultValue;
}

int Blackboard::GetValue(const std::string& keyName, int defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		return stoi(value);
	}
	return defaultValue;
}

IntRange Blackboard::GetValue(const std::string& keyName, const IntRange& defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		IntRange intR;
		intR.SetFromText(value.c_str());
		return intR;
	}
	return defaultValue;
}

FloatRange Blackboard::GetValue(const std::string& keyName, const FloatRange& defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		FloatRange flR;
		flR.SetFromText(value.c_str());
		return flR;
	}
	return defaultValue;
}

IntVector2 Blackboard::GetValue(const std::string& keyName, const IntVector2& defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		IntVector2 intV;
		intV.SetFromText(value.c_str());
		return intV;
	}
	return defaultValue;
}

float Blackboard::GetValue(const std::string& keyName, float defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		return stof(value);
	}
	return defaultValue;
}

std::string Blackboard::GetValue(const std::string& keyName, std::string defaultValue) const
{

	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		return value;
	}
	return defaultValue;
}

std::string Blackboard::GetValue(const std::string& keyName, const char* defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->first;
		return value;
	}
	std::string returnValue;
	returnValue.append(defaultValue);
	return returnValue;
}

Rgba Blackboard::GetValue(const std::string& keyName, const Rgba& defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		Rgba rgba;
		rgba.SetFromText(value.c_str());
		return rgba;
	}
	return defaultValue;
}

Vector2 Blackboard::GetValue(const std::string& keyName, const Vector2& defaultValue) const
{
	std::map<std::string,std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if(iter!=m_keyValuePairs.end())
	{
		std::string value = iter->second;
		Vector2 vector2;
		vector2.SetFromText(value.c_str());
		return vector2;
	}
	return defaultValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Blackboard::GetValue(const std::string& keyName, const Vector3& defaultValue) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.end())
	{
		std::string value = iter->second;
		Vector3 vector3;
		vector3.SetFromText(value.c_str());
		return vector3;
	}
	return defaultValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Blackboard::GetValue(const std::string& keyName, const Vector4& defaultValue) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter != m_keyValuePairs.end())
	{
		std::string value = iter->second;
		Vector4 vector4;
		vector4.SetFromText(value.c_str());
		return vector4;
	}
	return defaultValue;
}
