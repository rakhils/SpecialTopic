#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/31
*@purpose : Splites String based on character
*
*@param   : Original String and the character delimited
*
*@return  : string pointer
*/
//////////////////////////////////////////////////////////////
void Split(std::vector<std::string> &Strings,std::string str, char ch)
{
	//std::vector<std::string> Strings;
	int index = 0;int startpos = 0;
	for(;index < str.length();index++)
	{
		if(str[index] == ch)
		{
			Strings.push_back(str.substr(startpos,index - startpos));
			startpos = index + 1;
		}
	}
	Strings.push_back(str.substr(startpos,index));
}

std::string ReplaceAllChars(std::string str,char originalChar,char newChar)
{
	for(int index = 0;index < str.length();index++)
	{
		if(str[index] == originalChar)
		{
			str[index] = newChar;
		}
	}
	return str;
} 

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/31
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string ToString(std::vector<std::string> Strings)
{
	std::string returnString;
	for(int index = 0;index < Strings.size();index++)
	{
		returnString.append(Strings.at(index));
	}
	return returnString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Converts vector2 to string
*@param   : input values
*@return  : output vec2 string
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToString(Vector2 value)
{
	std::string returnString = ToString(value.x);
	returnString.append(",");
	returnString.append(ToString(value.y));
	return returnString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToString(Vector3 value)
{
	std::string returnString = ToString(value.x);
	returnString.append(",");
	returnString.append(ToString(value.y));
	returnString.append(",");
	returnString.append(ToString(value.z));
	return returnString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToString(Vector4 value)
{
	std::string returnString = ToString(value.x);
	returnString.append(",");
	returnString.append(ToString(value.y));
	returnString.append(",");
	returnString.append(ToString(value.z));
	returnString.append(",");
	returnString.append(ToString(value.w));
	return returnString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToString(size_t value)
{
	int intValue = static_cast<int>(value);
	return ToString(intValue);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/04
*@purpose : Converts double to string
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToString(double value)
{
	std::string doubleValue = "";
	try
	{
		doubleValue = std::to_string(value);
	}
	catch (const std::exception&)
	{

	}
	return doubleValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Converts string to lowercast string
*
*@param   : NIL
*
*@return  : lower cast string equivalent
*/
//////////////////////////////////////////////////////////////
std::string ToLower(std::string str)
{
	for(int stringIndex = 0;stringIndex < str.length();stringIndex++)
	{
		if(str[stringIndex] >= 65 && str[stringIndex] <= 90 )
		{
			str[stringIndex]+=32;
		}
	}
	return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Converts all alphabet to upper case
*@param   : input string
*@return  : output string of uppercase
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToUpper(std::string str)
{
	for (int stringIndex = 0; stringIndex < str.length(); stringIndex++)
	{
		if (str[stringIndex] >= 97 && str[stringIndex] <= 122)
		{
			str[stringIndex] -= 32;
		}
	}
	return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/26
*@purpose : reads next line of char pointer
*@param   : char pointer ,index
*@return  : Next String
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ReadLine(char *src)
{
	int index = 0;
	for (; *(src + index) != NULL; index++)
	{
		if (*(src + index) == '\n')
		{
			std::string oneLine(src, index);

			return oneLine;
		}
	}
	std::string oneLine(src, index);
	return oneLine;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/26
*@purpose : reads line from the index of char pointer
*@param   : char pointer ,index
*@return  : Next String 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ReadLine(char *src, int index)
{
	int originalIndex = index;
	for (; *(src + index) != NULL; index++)
	{
		if (*(src + index) == '\n')
		{
			std::string oneLine(src, originalIndex, index - originalIndex);
			return oneLine;
		}
	}
	if (originalIndex == index)
	{
		return "";
	}
	std::string oneLine(src, originalIndex, index - originalIndex);
	return oneLine;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/05
*@purpose : Get number of occurance of a character in string
*
*@param   : NIL
*
*@return  : Number of occurance of ch in str
*/
//////////////////////////////////////////////////////////////
int GetNumberOfOccurance(std::string str, char ch)
{
	int count = 0;
	for(int index = 0;index<str.length();index++)
	{
		if(str[index] == ch)
		{
			count++;
		}
	}
	return count;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Check if a string starts with another string
*
*@param   : Original string and root string to compare
*
*@return  : true of root starts with original
*/
//////////////////////////////////////////////////////////////
bool StartsWith(std::string original, std::string root)
{
	int length = static_cast<int>(root.length());
	if(original.length() < static_cast<int>(length))
	{
		return false;
	}
	std::string substring = original.substr(0,static_cast<int>(length));
	if(substring == root)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/01/31
*@purpose : Compares string and ch array
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool IsEqual(std::string str, char *ch)
{
	std::string newStr(ch);
	if(str == newStr)
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string ToString(float value)
{
	std::string floatValue = "";
	try
	{
		floatValue = std::to_string(value);
	}
	catch (const std::exception&)
	{

	}
	return floatValue;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string ToString(bool value)
{
	if(value)
	{
		return "TRUE";
	}
	return "FALSE";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Converts integer to string
*@param   : Integer value
*@return  : String
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ToString(int value)
{
	std::string integer = "";
	try
	{
		integer = std::to_string(value);
	}
	catch (const std::exception&)
	{

	}
	return integer;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/05
*@purpose : Trims spaces out
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
std::string TrimString(std::string text)
{
	if(text.length() == 0)
	{
		return text;
	}
	int trimStartIndex = 0;
	for(int index = 0;index < text.length();index++)
	{
		if(text.at(index) == ' ')
		{
			trimStartIndex++;
		}
		else
		{
			break;
		}
	}
	int trimEndIndex = static_cast<int>(text.length()) - 1;
	for(int index = static_cast<int>(text.length()) - 1;index >= 0; index--)
	{
		if (text.at(index) == ' ')
		{
			trimEndIndex--;
		}
		else
		{
			break;
		}
	}
	return text.substr(trimStartIndex, trimEndIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Converts to Integer from string value
*@param   : string value and int value to be returned
*@return  : Integer value
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ToInt(std::string value,int *intvalue)
{
	try
	{
		*intvalue = std::stoi(value);
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Converts to float from string value
*@param   : string value and float value to be returned
*@return  : bool if converts successful
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ToFloat(std::string value, float *floatvalue)
{
	try
	{
		*floatvalue = std::stof(value);
		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}

