//#include "XMLUtilities.hpp"
#include <string>
#include <vector>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/TinyXml2/tinyxml2.h"

using namespace tinyxml2;

int ParseXmlAttribute(const XMLElement& element, const char* attributeName, int defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		const char* value = element.Attribute(attributeName);
		std::string valueStr;
		valueStr.append(value);
		return (int)stoi(valueStr);	
	}
	return defaultValue;
}

char ParseXmlAttribute(const XMLElement& element, const char* attributeName, char defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		const char* title = element.Attribute(attributeName);
		return title[0];
	}
	return defaultValue;
}

std::string ParseXmlAttribute(const XMLElement& element, const char* attributeName, const std::string& defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		const char* title = element.Attribute(attributeName);
		return title;
	}
	return defaultValue;
}

bool ParseXmlAttribute(const XMLElement& element, const char* attributeName, bool defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		const char* title = element.Attribute(attributeName);
		std::string value;
		value.append(title);
		if(value == ("true"))
		{
			return true;
		}
		return false;
	}
	return defaultValue;
}

float ParseXmlAttribute(const XMLElement& element, const char* attributeName, float defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		std::string value;
		value.append(element.Attribute(attributeName));
		return (float)stof(value);	
	}
	return defaultValue;
}

Rgba ParseXmlAttribute(const XMLElement& element, const char* attributeName, const Rgba& defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		Rgba rgba;
		std::string value;
		value.append(element.Attribute(attributeName));
		rgba.SetFromText(value.c_str());
		return rgba;
	}
	return defaultValue;
}

Vector2 ParseXmlAttribute(const XMLElement& element, const char* attributeName, const Vector2& defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		Vector2 v1;
		std::string value;
		value.append(element.Attribute(attributeName));
		v1.SetFromText(value.c_str());
		return v1;
	}
	return defaultValue;
}

IntRange ParseXmlAttribute(const XMLElement& element, const char* attributeName, const IntRange& defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		IntRange intRange;
		std::string value;
		value.append(element.Attribute(attributeName));
		intRange.SetFromText(value.c_str());
		return intRange;
	}
	return defaultValue;
}

FloatRange ParseXmlAttribute(const XMLElement& element, const char* attributeName, const FloatRange& defaultValue)
{
	if(element.Attribute(attributeName)!=nullptr)
	{
		FloatRange floatRange;
		std::string value;
		value.append(element.Attribute(attributeName));
		floatRange.SetFromText(value.c_str());
		return floatRange;
	}
	return defaultValue;
}

IntVector2 ParseXmlAttribute(const XMLElement& element, const char* attributeName, const IntVector2& defaultValue)
{
	 if(element.Attribute(attributeName)!=nullptr)
	{
		IntVector2 intVector2;
		std::string value;
		value.append(element.Attribute(attributeName));
		intVector2.SetFromText(value.c_str());
		return intVector2;
	}
	return defaultValue;
}

std::vector<int> ParseXmlAttribute(const XMLElement& element, const char* attributename, const std::vector<int>& defaultValue)
{
	if(element.Attribute(attributename)!=nullptr)
	{
		std::vector<int> value;
		std::string attribute;
		attribute.append(element.Attribute(attributename));
		size_t index = 0;

		for(;index<attribute.length();index++)
		{
			int start = static_cast<int>(index);
			int finalIndex = static_cast<int>(attribute.find_first_of(',',index));
			if(finalIndex == -1)
			{
				break;
			}
			index = finalIndex;
			std::string intValueInString = attribute.substr(start,finalIndex);
			value.push_back(stoi(intValueInString));
		}
		std::string intValueInString = attribute.substr(index,attribute.length());
		value.push_back(stoi(intValueInString));
		return value;
	}
	return defaultValue;
}

