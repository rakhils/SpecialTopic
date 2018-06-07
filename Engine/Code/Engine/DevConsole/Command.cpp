#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Command::Command()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Constructs command
*@param   : One line commands with atribs
*@return  : NIL
*//////////////////////////////////////////////////////////////
Command::Command(char const *str)
{
	m_command = (std::string)(str);
}

// DESTRUCTOR
Command::~Command()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Returns the command name
*@param   : NIL
*@return  : command name
*//////////////////////////////////////////////////////////////
std::string Command::GetName()
{
	return m_name;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Gets next string from read index (string till next space char)
*@param   : NIL
*@return  : Next string
*//////////////////////////////////////////////////////////////
std::string Command::GetNextString()
{
	bool isInsideDoubleQuotes = false;
	int startPosition = m_readIndex;
	for(;m_readIndex< m_command.length();m_readIndex++)
	{
		if(m_command[m_readIndex] == '\"' )
		{
			if(m_readIndex == 0)
			{
				isInsideDoubleQuotes = true;
				continue;
			}
			if(m_readIndex > 0 && m_command[m_readIndex - 1] != '\\')
			{
				if (isInsideDoubleQuotes)
				{
					isInsideDoubleQuotes = false;
				}
				else
				{
					isInsideDoubleQuotes = true;
				}
			}
		}
		if(m_command[m_readIndex] == ' ' && !isInsideDoubleQuotes)
		{
			m_readIndex++;
			return m_command.substr(startPosition,m_readIndex-startPosition-1);
		}
	}
	return m_command.substr(startPosition,m_readIndex-startPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Reads the next string and convert to string
*@param   : Int value to be returned as pointer
*@return  : true if next string can be converted to int else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Command::GetNextInt(int *out_val)
{
	int readIndex    = m_readIndex;
	std::string text = GetNextString();
	bool success     = ToInt(text, out_val);
	if(!success)
	{
		m_readIndex = readIndex;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Retrieves next float values
*@param   : Float value to be returned
*@return  : true of success else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Command::GetNextFloat(float *out_val)
{
	int readIndex = m_readIndex;
	std::string text = GetNextString();
	bool success = ToFloat(text, out_val);
	if (!success)
	{
		m_readIndex = readIndex;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Retrieves the next color value same as vec4 float
*@param   : Color to be fetched
*@return  : true if success
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Command::GetNextColor(Rgba *out_val)
{
	Vector4 value;
	if(GetNextVector4(&value))
	{
		out_val->GetAsFloats(value.x, value.y, value.z, value.w);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Retrieves vector2 from the string
*@param   : Vec2 to be returned
*@return  : true if success
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Command::GetNextVector2(Vector2 *out_val)
{
	int readIndex    = m_readIndex;
	std::string text = GetNextString();
	if (!out_val->SetFromText(text.c_str()))
	{
		m_readIndex = readIndex;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Retrieves vector3 from the string
*@param   : Vec3 to be returned
*@return  : true if success
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Command::GetNextVector3(Vector3 *out_val)
{
	int readIndex = m_readIndex;
	std::string text = GetNextString();
	if (!out_val->SetFromText(text.c_str()))
	{
		m_readIndex = readIndex;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Retrieves vector4 from the string
*@param   : Vec4 to be returned
*@return  : true if success
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Command::GetNextVector4(Vector4 *out_val)
{
	int readIndex = m_readIndex;
	std::string text = GetNextString();
	if (!out_val->SetFromText(text.c_str()))
	{
		m_readIndex = readIndex;
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Command Setup and cleaning history
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void CommandStartup()
{
	CommandRegister("test", TestCommand, "TEST COMMAND");
	CommandRegister("help", HelpCommand, "SHOWS ALL COMMAND AVAILABE");
	CommandRegister("clear", ClearCommand, "CLEARS OUTPUT SCREEN");
	CommandRegister("save_log", SaveLog, "SAVES COMMAND");
	CommandRegister("echo_with_color", EchoColor, "ECHO THE INPUT COLOR");

	//DEBUG RENDER COMMANDS
	CommandRegister("debug_render"          , ToggleDebugRenderVisibility,   "TOGGLES DEBUG RENDER OBJECTS VISIBILITY");
	CommandRegister("debug_render_clear"    , ClearDebugRenderDraws,		 "CLEARS ALL DEBUG RENDER OBJECTS");
	//CommandRegister("debug_render_quad2d"   , DebugRenderQuad2D,			 "DRAWS DEBUG QUAD2D IN 2D SPACE");
	CommandRegister("debug_render_text2d"   , DebugRenderText2D,			 "DRAWS TEXT IN 2D SPACE");
	CommandRegister("debug_render_point3d"  , DebugRenderPoint3D,			 "DRAWS POINT IN CURRENT 3D SPACE");
	CommandRegister("debug_render_sphere"   , DebugRenderSphere,			 "DRAWS SPHERE IN CURRENT 3D SPACE");
	CommandRegister("debug_render_text3d"	, DebugRenderText3D,			 "DRAWS TEXT IN CURRENT 3D SPACE");
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Registers a command in the registry for future use
*
*@param   : command name and actual function pointer
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void CommandRegister(char const *name, command_cb cmdCB,std::string helpText)
{
	std::string nameStr(name);
	CommandDefinition cmdDef;
	nameStr						= ToLower(nameStr);
	cmdDef.m_name				= nameStr;
	cmdDef.m_callback			= cmdCB;
	cmdDef.m_description		= helpText;
	commandRegistery[nameStr]	= cmdDef;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Retrieves all commands from registery
*@param   : NIL
*@return  : vector of command string
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<CommandDefinition> RetrieveAllCommands()
{
	std::map<std::string, CommandDefinition>::iterator it = commandRegistery.begin();
	std::vector<CommandDefinition> m_commands;
	for (; it != commandRegistery.end(); it++)
	{
		m_commands.push_back(it->second);
	}
	return m_commands;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Calls an command call back
*@param   : command name
*@return  : NIL
*//////////////////////////////////////////////////////////////
bool CommandRun(char const *command)
{
	Command newcommand(command);
	std::string cmd = newcommand.GetNextString();
	std::map<std::string, CommandDefinition>::iterator iter = commandRegistery.find(cmd);
	//InsertIntoCommandHistory(command);
	if(iter != commandRegistery.end())
	{
		(*(iter->second.m_callback))(newcommand);
		return true;
	}
	DevConsole::GetInstance()->PushToOutputText("Invalid Command\n",Rgba::RED);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/23
*@purpose : Commands to test the dev console feature
*@param   : Command
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TestCommand(Command &cmd)
{
	Vector3 value;
	if(cmd.GetNextVector3(&value))
	{
		DevConsole::GetInstance()->PushToOutputText("Command Success "+ ToString(value)+"\n",Rgba::RED);
	}
	else
	{
		DevConsole::GetInstance()->PushToOutputText("Command Failure "+ ToString(value) + "\n", Rgba::RED);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Insert recent command to command history vector
*@param   : command
*@return  : NIL
*//////////////////////////////////////////////////////////////
/*
void InsertIntoCommandHistory(std::string cmd)
{
	for(int historyIndex = 0;historyIndex<commandHistory.size();historyIndex++)	
	{
		//if(commandHistory.at(historyIndex) == cmd)
		//{
		//	commandHistory.erase(commandHistory.begin()+historyIndex,commandHistory.begin()+historyIndex+1);
		//	commandHistory.push_back(cmd);
		//	return;
		//}
	}
	commandHistory.push_back(cmd);
}*/

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Help Command
*
*@param   : Command
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void HelpCommand(Command &cmd)
{
	UNUSED(cmd);
	std::map<std::string,CommandDefinition>::iterator iter = commandRegistery.begin();
	for(;iter != commandRegistery.end();iter++)
	{
		DevConsole::GetInstance()->PushToOutputText(iter->first);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Clears all output text
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void ClearCommand(Command &cmd)
{
	UNUSED(cmd);
	DevConsole::GetInstance()->ClearOutputScreen();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Save command log to file
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void SaveLog(Command &cmd)
{
	std::string fileName = cmd.GetNextString();
	std::string outputBoxContent = "";
	for(int index = 0;index < DevConsole::GetInstance()->m_outputString.size();index++)
	{
		outputBoxContent += DevConsole::GetInstance()->m_outputString.at(index)->m_string;
	}
	bool success = FileWrite(fileName+".txt",(char*)outputBoxContent.c_str(),static_cast<int>(outputBoxContent.length()));
	if(success)
	{
		DevConsole::GetInstance()->PushToOutputText("File Write Success : FileName "+fileName+"\n");
		return;
	}
	DevConsole::GetInstance()->PushToOutputText("File Write Failure\n");
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/06
*@purpose : Prints a text in a color
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void EchoColor(Command &cmd)
{
	std::string color = cmd.GetNextString();
	Rgba rgba;
	rgba.SetFromText(color.c_str());
	std::string str = cmd.GetNextString();
	DevConsole::GetInstance()->PushToOutputText(str,rgba);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Toggles debug render visibility
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void ToggleDebugRenderVisibility(Command &cmd)
{
	std::string value = cmd.GetNextString();
	if (value == "true")
	{
		DebugDraw::s_debugRender = true;
	}
	else if (value == "false")
	{
		DebugDraw::s_debugRender = false;
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void ClearDebugRenderDraws(Command &cmd)
{
	std::string value = cmd.GetNextString();
	if (value == "true")
	{
		DebugDraw::ClearAllDebugRenderDraws();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugRenderQuad2D(Command &cmd)
{
	DebugRenderOptions options;
	Vector3 position;
	Vector2 dimension;
	float lifetime = 0;
	if(!cmd.GetNextVector3(&position))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID POSITION", Rgba::RED);
		return;
	}
	if(!cmd.GetNextVector2(&dimension))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID DIMENSIONS", Rgba::RED);
		return;
	}
	if(!cmd.GetNextFloat(&lifetime))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID LIFETIME", Rgba::RED);
		return;
	}
	options.m_lifeTime = lifetime;
	DebugDraw::GetInstance()->DebugRenderQuad2D(position, AABB2(Vector2(0, 0), dimension.x, dimension.y), 0.f, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	DevConsole::GetInstance()->PushToOutputText("SUCCESS", Rgba::GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Draws text in 2d space
*@param   : Command obj
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugRenderText2D(Command &cmd)
{
	DebugRenderOptions options;
	Vector3 position;
	float   fontsize = 1;
	float   lifetime = 0;
	if (!cmd.GetNextVector3(&position))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID POSITION SYNTAX - POSITION,FONTSIZE,LIFETIME,STRING", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&fontsize))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID FONT SIZE SYNTAX - POSITION,FONTSIZE,LIFETIME,STRING", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&lifetime))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID LIFETIME SYNTAX - POSITION,FONTSIZE,LIFETIME,STRING", Rgba::RED);
		return;
	}
	std::string str = cmd.GetNextString();
	options.m_lifeTime = lifetime;
	DebugDraw::GetInstance()->DebugRenderText2D(position, Vector3::ZERO, str, Vector2::ONE, fontsize, Rgba::WHITE, options);
	DevConsole::GetInstance()->PushToOutputText("SUCCESS", Rgba::GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Renders point in current camera space
*@param   : Command obj
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugRenderPoint3D(Command &cmd)
{
	DebugRenderOptions options;
	Vector3 position;
	Rgba    colorStart;
	Rgba    colorEnd;
	float   lifetime = 0;
	if (!cmd.GetNextVector3(&position))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID POSITION SYNTAX - POSITION,COLOR,COLOR,LIFETIME", Rgba::RED);
		return;
	}
	if (!cmd.GetNextColor(&colorStart))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID START COLOR SYNTAX - POSITION,COLOR,COLOR,LIFETIME", Rgba::RED);
		return;
	}
	if (!cmd.GetNextColor(&colorEnd))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID END COLOR SYNTAX - POSITION,COLOR,COLOR,LIFETIME", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&lifetime))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID LIFE TIME SYNTAX - POSITION,COLOR,COLOR,LIFETIME", Rgba::RED);
		return;
	}
	options.m_startColor = colorStart;
	options.m_endColor = colorEnd;
	options.m_lifeTime = lifetime;
	DebugDraw::GetInstance()->DebugRenderCube(position, Vector3::ONE*0.1f, nullptr, Rgba::WHITE, DEBUG_RENDER_FILL, options);
	DevConsole::GetInstance()->PushToOutputText("SUCCESS", Rgba::GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Renders sphere in current camera 3d space
*@param   : Command obj
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugRenderSphere(Command &cmd)
{
	DebugRenderOptions options;
	Vector3 position;
	float   radius;
	float   lifetime = 0;
	if (!cmd.GetNextVector3(&position))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID POSITION SYNTAX -   POSITION,RADIUS,LIFE TIME", Rgba::RED);
		return;																			   
	}																					   
	if (!cmd.GetNextFloat(&radius))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID RADIUS SYNTAX -   POSITION,RADIUS,LIFE TIME", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&lifetime))													   
	{																					   
		DevConsole::GetInstance()->PushToOutputText("INVALID LIFE TIME SYNTAX -   POSITION,RADIUS,LIFE TIME", Rgba::RED);
		return;
	}
	options.m_lifeTime = lifetime;
	DebugDraw::GetInstance()->DebugRenderSphere(position,radius,32,32,nullptr,Rgba::WHITE,DEBUG_RENDER_FILL,options);
	DevConsole::GetInstance()->PushToOutputText("SUCCESS", Rgba::GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Renders wire sphere in 3d space
*@param   : Command obj
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugRenderWiredSphere(Command &cmd)
{
	DebugRenderOptions options;
	Vector3 position;
	float   radius;
	float   lifetime = 0;
	if (!cmd.GetNextVector3(&position))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID POSITION SYNTAX -   POSITION,RADIUS,LIFE TIME", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&radius))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID RADIUS SYNTAX -   POSITION,RADIUS,LIFE TIME", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&lifetime))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID LIFE TIME SYNTAX -   POSITION,RADIUS,LIFE TIME", Rgba::RED);
		return;
	}
	options.m_lifeTime = lifetime;
	DebugDraw::GetInstance()->DebugRenderSphere(position, radius, 32, 32, nullptr, Rgba::WHITE, DEBUG_RENDER_WIRE, options);
	DevConsole::GetInstance()->PushToOutputText("SUCCESS", Rgba::GREEN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/01
*@purpose : Renders text in current 3d space
*@param   : Command obj
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugRenderText3D(Command &cmd)
{
	DebugRenderOptions options;
	Vector3 position;
	float   fontsize = 1;
	float   lifetime = 0;
	if (!cmd.GetNextVector3(&position))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID POSITION SYNTAX - POSITION,FONTSIZE,LIFETIME,STRING", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&fontsize))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID FONT SIZE SYNTAX - POSITION,FONTSIZE,LIFETIME,STRING", Rgba::RED);
		return;
	}
	if (!cmd.GetNextFloat(&lifetime))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID LIFETIME SYNTAX - POSITION,FONTSIZE,LIFETIME,STRING", Rgba::RED);
		return;
	}
	std::string str = cmd.GetNextString();
	options.m_lifeTime = lifetime;
	DebugDraw::GetInstance()->DebugRenderText(str,position, Vector3::ZERO, Vector2::ONE, lifetime,fontsize, Rgba::WHITE);
	DevConsole::GetInstance()->PushToOutputText("SUCCESS", Rgba::GREEN);
}

/*
//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetAttenuation(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	std::string attenuationConstStr = cmd.GetNextString();
	Vector3 attenuationConst;
	attenuationConst.SetFromText(attenuationConstStr.c_str());
	UNUSED(index);
	//Renderer::GetDefaultLight()->SetPointLightAttenutaion(attenuationConst);
}

//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/18
*@purpose : Sets the position of given light
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetPosition(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	UNUSED(index);

	std::string lightPositionStr = cmd.GetNextString();
	Vector3 lightPosition;
	lightPosition.SetFromText(lightPositionStr.c_str());
	//Renderer::GetDefaultLight()->SetPointLightPosition(lightPosition);
}

//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/18
*@purpose : Sets the color of given light
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetColor(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	UNUSED(index);

	std::string attenuationConstStr = cmd.GetNextString();
	Vector4 Color;
	Color.SetFromText(attenuationConstStr.c_str());

	//Renderer::GetDefaultLight()->SetPointLightColor(Color);
}*/

/*
//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/23
*@purpose : Sets the light direction
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetDirection(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	UNUSED(index);

	std::string directionConstStr = cmd.GetNextString();
	Vector3 direction;
	direction.SetFromText(directionConstStr.c_str());

	//Renderer::GetDefaultLight()->SetPointLightDirection(index, direction);
}

//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/23
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetInnerAndOuterAngle(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	UNUSED(index);
	std::string innerAngleStr = cmd.GetNextString();
	std::string outerAngleStr = cmd.GetNextString();
	float innerAngle = ToFloat(innerAngleStr.c_str());
	float outerAngle = ToFloat(outerAngleStr.c_str());
	UNUSED(innerAngle);
	UNUSED(outerAngle);
	//Renderer::GetDefaultLight()->SetPointLightInnerOuterAngles(index, innerAngle, outerAngle);
}

//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/23
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetSpecConstants(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	UNUSED(index);

	std::string specPowerStr = cmd.GetNextString();
	std::string specFactorStr = cmd.GetNextString();
	float specPower = ToFloat(specPowerStr.c_str());
	float specFactor = ToFloat(specFactorStr.c_str());
	UNUSED(specFactor);
	UNUSED(specPower);
	//Renderer::GetDefaultLight()->SetSpecularConstants(index, specPower, specFactor);
}

//////////////////////////////////////////////////////////////
/ *DATE    : 2018/04/23
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
* /
//////////////////////////////////////////////////////////////
void LightSetSpecAttenuation(Command &cmd)
{
	std::string lightIndex = cmd.GetNextString();
	int index = ToInt(lightIndex);
	UNUSED(index);
	Vector3 specAttenuation;
	std::string specAttenuationStr = cmd.GetNextString();
	specAttenuation.SetFromText(specAttenuationStr.c_str());
	//Renderer::GetDefaultLight()->SetPointLigthSpecAttenuation(index, specAttenuation);
}*/

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Gets the command history saved vector
*
*@param   : NIL
*
*@return  : command history vector
*/
//////////////////////////////////////////////////////////////
/*
std::vector<std::string> GetCommandHistory()
{
	return commandHistory;
}*/

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/04
*@purpose : Runs a "script", or multiple lines of commands (each line is treated as a command).
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void CommandRunScript(char const *script)
{
	CommandStartup();
	UNUSED(script);
	
	//CommandRegister("light_set_attenuation", LightSetAttenuation);
	//CommandRegister("light_set_color", LightSetColor);
	//CommandRegister("light_set_position", LightSetPosition);
	//CommandRegister("light_set_direction", LightSetDirection);
	//CommandRegister("light_set_angles", LightSetInnerAndOuterAngle);
	//CommandRegister("light_set_spec_const", LightSetSpecConstants);
	//CommandRegister("ligth_set_spec_attenuation", LightSetSpecAttenuation);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/02
*@purpose : Check for the existing command which matches the root words
*
*@param   : NIL
*
*@return  : return the matching commands for the root word
*/
//////////////////////////////////////////////////////////////
/*
std::vector<std::string> CommandAutoComplete(char const *root)
{
	//EXTRA TO BE IMPLEMENTED
	std::string rootcmd(root);
	std::vector<std::string> returnStrings;
	//for(int index = 0; index < gDefinitionCount;index++)
	//{
	//	std::string cmd = gDefinition[index].m_name;
	//	if(StartsWith(cmd,root))
	//	{
	//		returnStrings.push_back(cmd);
	//	}
	//}
	return returnStrings;
}
*/



