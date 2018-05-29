#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
//-----------------------------------------------------------------------------------------------
#define GET_VARIABLE_NAME(Variable) (#Variable)

const std::string		 Stringf( const char* format, ... );
const std::string		 Stringf( const int maxLength, const char* format, ... );

std::vector<std::string> Split(std::string,char ch);
std::string				 ReplaceAllChars(std::string str,char originalChar,char newChar);

std::string				 ToLower(std::string str);
std::string				 ToUpper(std::string str);
std::string				 ReadLine(char *src);
std::string				 ReadLine(char *src, int index);
int						 GetNumberOfOccurance(std::string str,char ch);
						 
bool					 StartsWith(std::string original,std::string root);
						 
bool					 IsEqual(std::string str,char *ch);

std::string				 ToString(std::vector<std::string> Strings);
std::string				 ToString(float value);
std::string				 ToString(bool value);
std::string				 ToString(int value);
std::string				 ToString(Vector2 value);
std::string				 ToString(Vector3 value);
std::string				 ToString(Vector4 value);

std::string				 TrimString(std::string);

bool					 ToInt(std::string value,int *intvalue);
bool					 ToFloat(std::string value, float *floatvalue);
