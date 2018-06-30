#include "Rgba.hpp"
#include <windows.h>			// #include this (massive, platform-specific) header in very few places	
#include <stdio.h>
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\MathUtil.hpp"
#include "Engine\Core\StringUtils.hpp"
#pragma comment( lib, "opengl32" )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/24
*@purpose : Returns random color with alpha 1
*@param   : NIL
*@return  : random color
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Rgba Rgba::GetRandomColor()
{
	float r = GetRandomFloatInRange(0, 1);
	float g = GetRandomFloatInRange(0, 1);
	float b = GetRandomFloatInRange(0, 1);
	float a = 1;
	Rgba rgb;
	rgb.SetAsFloats(r, g, b, a);
	return rgb;
}

Rgba Rgba::WHITE	   = Rgba(static_cast<unsigned char>(255),static_cast<unsigned char>(255),static_cast<unsigned char>(255));
Rgba Rgba::FADED_WHITE = Rgba(static_cast<unsigned char>(255),static_cast<unsigned char>(255),static_cast<unsigned char>(255),static_cast<unsigned char>(100));
Rgba Rgba::RED		   = Rgba(static_cast<unsigned char>(255),static_cast<unsigned char>(0),static_cast<unsigned char>(0));
Rgba Rgba::GREEN       = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(255),static_cast<unsigned char>(0));
Rgba Rgba::BLUE        = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(0),static_cast<unsigned char>(255));
Rgba Rgba::YELLOW      = Rgba(static_cast<unsigned char>(255),static_cast<unsigned char>(255),static_cast<unsigned char>(0));
Rgba Rgba::MAGENTA     = Rgba(static_cast<unsigned char>(255),static_cast<unsigned char>(0),static_cast<unsigned char>(255));
Rgba Rgba::CYAN        = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(255),static_cast<unsigned char>(255));
Rgba Rgba::BLACK	   = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(0),static_cast<unsigned char>(0));
Rgba Rgba::FADED_BLACK = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(0),static_cast<unsigned char>(0), static_cast<unsigned char>(100));
Rgba Rgba::FADED_RED   = Rgba(static_cast<unsigned char>(255),static_cast<unsigned char>(0),static_cast<unsigned char>(0), static_cast<unsigned char>(75));
Rgba Rgba::FADED_GREEN = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(255),static_cast<unsigned char>(0), static_cast<unsigned char>(75));
Rgba Rgba::FADED_BLUE  = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(0),static_cast<unsigned char>(255), static_cast<unsigned char>(75));
Rgba Rgba::CONSOLE_FADED_BLUE = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(0),static_cast<unsigned char>(255), static_cast<unsigned char>(75));
Rgba Rgba::CONSOLE_BLACK      = Rgba(static_cast<unsigned char>(0),static_cast<unsigned char>(0),static_cast<unsigned char>(0), static_cast<unsigned char>(150));


Rgba::Rgba() 
{
	SetAsFloats(1, 1, 1, 1);
}

Rgba::Rgba( unsigned char redByte, unsigned char greenByte,unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Rgba::Rgba(Vector4 color)
{
	r = static_cast<unsigned char> (RangeMapFloat(color.x, 0, 1, 0, 255));
	g = static_cast<unsigned char> (RangeMapFloat(color.y, 0, 1, 0, 255));
	b = static_cast<unsigned char> (RangeMapFloat(color.z, 0, 1, 0, 255));
	a = static_cast<unsigned char> (RangeMapFloat(color.w, 0, 1, 0, 255));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/24
*@purpose : Constructor from text
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Rgba::Rgba(std::string text)
{
	SetFromText(text.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Rgba::operator!=(const Rgba color)
{
	if (r == color.r && g == color.g && b == color.b && a == color.a)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Rgba::operator==(const Rgba color)
{
	if(r == color.r && g == color.g && b == color.b && a == color.a)
	{
		return true;
	}
	return false;
}

void Rgba::SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsFloats( float normalizedRed, float normalizedgreen,float normalizedblue, float normalizedAlpha)
{
	float red      = RangeMapFloat(normalizedRed,0,1,0,255);
	float green    = RangeMapFloat(normalizedgreen,0,1,0,255);
	float blue     = RangeMapFloat(normalizedblue,0,1,0,255);
	float alphaVal = RangeMapFloat(normalizedAlpha,0,1,0,255);

	r = static_cast<unsigned char> (red);
	g = static_cast<unsigned char> (green);
	b = static_cast<unsigned char> (blue);
	a = static_cast<unsigned char> (alphaVal);	
}

void Rgba::GetAsFloats( float& out_normalizedRed,float& out_normalizedGreen,float& out_normalizedBlue,float& out_normalizedAlpha ) const
{
	out_normalizedRed   = RangeMapFloat((float)r,0,255,0,1);
	out_normalizedGreen = RangeMapFloat((float)g,0,255,0,1);
	out_normalizedBlue  = RangeMapFloat((float)b,0,255,0,1);
	out_normalizedAlpha = RangeMapFloat((float)a,0,255,0,1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Rgba::GetAsFloatArray(float *arr)
{
	GetAsFloats(arr[0], arr[1], arr[2], arr[3]);
}

void Rgba::ScaleRGB( float rgbScale ) // Scales (and clamps) RGB components, but not A
{
	r = static_cast<unsigned char> (r*rgbScale);
	g = static_cast<unsigned char> (g*rgbScale);
	b = static_cast<unsigned char> (b*rgbScale);
	r = static_cast<unsigned char> (ClampZeroOrToOne(r));
	g = static_cast<unsigned char> (ClampZeroOrToOne(g));
	b = static_cast<unsigned char> (ClampZeroOrToOne(b));
}
void Rgba::ScaleAlpha( float alphaScale ) // Scales (and clamps) Alpha, RGB is untouched
{
	a = static_cast<unsigned char> (a*alphaScale);
	a = static_cast<unsigned char> (ClampZeroOrToOne(a));
}

void Rgba::SetFromText(const char* text)
{
	std::string value;
	value.append(text);
	size_t indexR = value.find_first_of(',',0);
	size_t indexG = value.find_first_of(',',indexR+1);
	size_t indexB = value.find_first_of(',',indexG+1);
	try
	{
		std::string rValue = value.substr(0, indexR);
		r = (unsigned char)stoi(rValue);

		std::string gValue = value.substr(indexR + 1, indexG);
		g = (unsigned char)stoi(gValue);

		if (indexB != std::string::npos)
		{
			std::string bValue = value.substr(indexG + 1, indexB);
			b = (unsigned char)stoi(bValue);

			std::string aValue = value.substr(indexB + 1, value.length());
			a = (unsigned char)stoi(aValue);
		}
		else
		{
			std::string bValue = value.substr(indexG + 1, value.length());
			b = (unsigned char)stoi(bValue);

		}
	}
	catch (const std::exception&)
	{
		SetAsFloats( 1.0f, 1.0f,1.0f,1.0f);
	}
}

std::string Rgba::GetAsString()
{
	std::string strValue;

	int red    = static_cast<int>(r);
	int green  = static_cast<int>(g);
	int blue   = static_cast<int>(b);
	int alphaV = static_cast<int>(a);
	strValue.append(ToString(red));
	strValue.append(",");
	strValue.append(ToString(green));
	strValue.append(",");
	strValue.append(ToString(blue));
	strValue.append(",");
	strValue.append(ToString(alphaV));

	return strValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/07
*@purpose : Gets color as vector4
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Vector4 Rgba::GetAsFloats()
{
	float _r,_g,_b,_a;
	GetAsFloats(_r, _g, _b, _a); // reference taken
	return Vector4(_r,_g,_b,_a);
}

