#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/FileUtil/File.h"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/DevConsole/Profiler/ProfilerManager.hpp"
#include "Engine/DevConsole/Profiler/ProfilerConstants.hpp"
#include "Engine/Logger/LogManager.hpp"
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/Net/TCP/TCPServer.hpp"
#include "Engine/Net/RCS.hpp"
#include "Engine/Net/UDP/UDPTest.hpp"
#include "Engine/Net/NetSession.hpp"
//#include "Engine/Net/UDP/UDPSocket.hpp"
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
	bool hasDoubleQuotes = false;
	int startPosition = m_readIndex;
	for(;m_readIndex< m_command.length();m_readIndex++)
	{
		if(m_command[m_readIndex] == '\"' )
		{
			if(m_readIndex == startPosition)
			{
				isInsideDoubleQuotes = true;
				hasDoubleQuotes = true;
				continue;
			}
			if(m_readIndex > startPosition && m_command[m_readIndex - 1] != '\\')
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
			if(hasDoubleQuotes)
			{
				return m_command.substr(startPosition + 1, m_readIndex - startPosition - 3);
			}
			return m_command.substr(startPosition,m_readIndex-startPosition-1);
		}
	}
	if(hasDoubleQuotes)
	{
		return m_command.substr(startPosition+1,m_readIndex-startPosition - 2);
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
	//CommandRegister("debug_render_quad2d"   , DebugRenderQuad2D,			 "DRAWS DEBUG QUAD2D IN 2D SPACE");
	CommandRegister("debug_render"          ,			ToggleDebugRenderVisibility,		"TOGGLES DEBUG RENDER OBJECTS VISIBILITY");
	CommandRegister("debug_render_clear"    ,			ClearDebugRenderDraws,				"CLEARS ALL DEBUG RENDER OBJECTS");
	CommandRegister("debug_render_text2d"   ,			DebugRenderText2D,					"DRAWS TEXT IN 2D SPACE");
	CommandRegister("debug_render_point3d"  ,			DebugRenderPoint3D,					"DRAWS POINT IN CURRENT 3D SPACE");
	CommandRegister("debug_render_sphere"   ,			DebugRenderSphere,					"DRAWS SPHERE IN CURRENT 3D SPACE");
	CommandRegister("debug_render_text3d"	,			DebugRenderText3D,					"DRAWS TEXT IN CURRENT 3D SPACE");

	CommandRegister("profiler_resume",					ProfilerResume,						"Resumes Profiler");
	CommandRegister("profiler_pause" ,					ProfilerPause,						"Pauses Profiler");
	CommandRegister("show_mouse",						MouseShow,							"Shows  mouse ");
	CommandRegister("hide_mouse",						MouseHide,							"Hides  mouse ");
	CommandRegister("profiler_show",					ProfilerShow ,						"Shows profiler");
	CommandRegister("profiler_hide",					ProfilerHide,						"Hides Profiler");
	CommandRegister("profiler_tree_view",				ProfilerTreeView,					"Views profiler in tree style");
	CommandRegister("profiler_flat_view",				ProfilerFlatView,					"Views profiler in flat style");
	
	// THREAD
	CommandRegister("thread_test_thread",				NewThreadTest,						"DO THREAD TEST ON NEW THREAD");
	CommandRegister("thread_test_main"  ,				MainThreadTest,						"DO THREAD TEST ON MAIN THREAD");
	CommandRegister("log_flush_test",					LogFlushTest,						"DO THE LOGS FLUSH TEST");
	
	// LOGGER
	CommandRegister("log_read_write_test",				LogReadWriteTest,					"DO THE LOGS READ MULTIPLE WRITE ONE TEST");
	CommandRegister("attach_devconsole_tolog",			AttachDevConsoleLogging,			"ATTACHES DEVCONSOLE OUTPUT TO LOGGER");
	CommandRegister("detach_devconsole_tolog",			DetachDevConsoleLogging,			"DETACHES DEVCONSOLE OUTPUT TO LOGGER");

	CommandRegister("log_hide",							LogHideTag,							"ADDS LOG FILTER ");
	CommandRegister("log_show",							LogShowTag,							"REMOVES LOG FILTER");
	CommandRegister("log_show_devconsole",				LogShowDevConsoleTag,				"ADDS DEVCONSOLE LOG FILTER ");
	CommandRegister("log_hide_devconsole",			    LogHideDevConsoleTag,				"REMOVES DEVCONSOLE LOG FILTER");

	CommandRegister("log_show_all",						LogShowAllTag,						"SHOWS ALL TAGS");
	CommandRegister("log_hide_all",						LogHideAllTag,						"HIDES ALL TAGS");
	CommandRegister("log_show_all_devconsole",			LogShowAllDevConsoleTag,			"SHOWS ALL DEVCONSOLE TAGS");
	CommandRegister("log_hide_all_devconsole",			LogHideAllDevConsoleTag,			"HIDES ALL DEVCONSOLE TAGS");

	CommandRegister("a01_test_server", Listen, "Starts server and listen in a port");
	CommandRegister("a01_test_connect", Connect, "Connects to a port and server");

	CommandRegister("testrcs", TestRCSMsg, "TESTS RCS MSG");
	CommandRegister("rc", RCExecute, "Executes RC Commands");
	CommandRegister("rca", RCAExecute, "SEND COMMAND TO ALL CONNECTIONS AND EXECUTE LOCAL");
	CommandRegister("rcb", RCBExecute, "SEND COMMAND TO ALL CONNECTIONS AND NOT EXECUTE LOCAL");
	CommandRegister("rc_join", RCJoin, "TRIES TO JOIN TO A PARTICULAR IP PORT");
	CommandRegister("rc_host", RCHost, "HOST IN A PARTICULAR PORT");
	CommandRegister("rc_echo", RCEcho, "TOGGLES THE ECHO TO DEVCONSOLE");

	CommandRegister("udp_send", UDPPacketSend, "SENDS PACKET FROM UDP");
	CommandRegister("add_connection", AddUDPConnection, "ADDS A NEW UDP CONNECTION");
	CommandRegister("send", SendCommandOverUDP, "SENDS COMMANDS OVER UDP");
	CommandRegister("send_ping", SendPing, "SENDS PING COMMANDS OVER UDP");
	CommandRegister("send_add", SendAdd, "SENDS ADD COMMANDS OVER UDP");
	CommandRegister("send_combo", SendCombo, "SENDS COMBINATION OF MSGS");
	CommandRegister("send_bad", SendBad, "SENDS BAD MSG TO A CONNECTION");

	CommandRegister("net_set_session_send_rate", SetUDPSessionSendRate, "SETS UDP SESSION'S SEND RATE");
	CommandRegister("net_set_connection_send_rate", SetUDPConnectionSendRate, "SETS UDP CONNECTION'S SEND RATE");
	CommandRegister("net_sim_loss", SetUDPSessionLossRate, "SETS UDP SESSION LOSS RATE");
	CommandRegister("net_sim_latency", SetUDPSessionMinMaxLatency, "SETS UDP SESSION MIN AND MAX LATENCY");


	CommandRegister("setup_udp", SetupUDPConnections, "SETS UP CONNECTION FOR UDP");
	CommandRegister("add_host",  Host, "ADDS LOCAL CONNECTION TO LCOAL AND REMOTE");
	CommandRegister("add_remote", AddRemote, "ADDS REMOTE CONNECTION TO LOCAL AND REMOTE");
	CommandRegister("disconnect", Disconnect, "DISCONNECTS A CONNECTION");

	CommandRegister("listen_udp", ListenUDPPort, "LISTEN NET SESSION IN GIVEN PORT");
	CommandRegister("erase_queues", EraseAllQueues, "ERASES ALL MSGS IN QUEUES OF GIVEN CONN");

	CommandRegister("send_join", SendJoinRequest, "SENDS JOIN REQUEST");
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
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CommandRun(char const *command, char const *output)
{
	UNUSED(command);
	UNUSED(output);
	return true;
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
		DevConsole::GetInstance()->PushToOutputText(iter->first,Rgba::WHITE,false);
	}
	DevConsole::GetInstance()->PushToOutputText("", Rgba::WHITE, true);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Resumes profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerResume(Command &cmd)
{
	UNUSED(cmd);
	ProfilerManager::s_isPaused = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Pauess profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerPause(Command &cmd)
{
	UNUSED(cmd);
	ProfilerManager::s_isPaused = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Shows cursor
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MouseShow(Command &cmd)
{
	UNUSED(cmd);
	InputSystem::GetInstance()->ShowCursor(true);
	InputSystem::GetInstance()->MouseLockToScreen(false);
	InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_ABSOLUTE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Hides mouses
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MouseHide(Command &cmd)
{
	UNUSED(cmd);
	InputSystem::GetInstance()->m_mouse.m_mouseMode = MOUSEMODE_RELATIVE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Shows the profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerShow(Command &cmd)
{
	UNUSED(cmd);
	ProfilerManager::s_profilerEnabled = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : Hides profiler
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerHide(Command &cmd)
{
	UNUSED(cmd);
	ProfilerManager::s_profilerEnabled = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerFlatView(Command &cmd)
{
	UNUSED(cmd);
	ProfilerManager::s_profilerView = FLAT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfilerTreeView(Command &cmd)
{
	UNUSED(cmd);
	ProfilerManager::s_profilerView = TREE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainThreadTest(Command &cmd)
{
	UNUSED(cmd);
	Thread::TestOnMainThread();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Test on new thread
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NewThreadTest(Command &cmd)
{
	int number;
	if (!cmd.GetNextInt(&number))
	{
		DevConsole::GetInstance()->PushToOutputText("INVALID THREAD COUNT - thread_test_thread <count>", Rgba::RED);
		return;
	}
	Thread::TestOnThread(number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogFlushTest(Command &cmd)
{
	UNUSED(cmd);
	LogManager::GetInstance()->LogFlushTest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogReadWriteTest(Command &cmd)
{
	std::string filepath = cmd.GetNextString();
	int threadCount;
	cmd.GetNextInt(&threadCount);
	Thread::TestLogReadWrite(filepath.c_str(), threadCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AttachDevConsoleLogging(Command &cmd)
{
	std::string logForwardId = cmd.GetNextString();
	if(logForwardId == "")
	{
		logForwardId = "devconsole";
	}
	LogManager::GetInstance()->AttachLogForwardCallBacks(logForwardId,LogDevConsoleWriteCallBack);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/10
*@purpose : Detaches log forwarding
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DetachDevConsoleLogging(Command &cmd)
{
	std::string logForwardId = cmd.GetNextString();
	if (logForwardId == "")
	{
		logForwardId = "devconsole";
	}
	LogManager::GetInstance()->DetachLogForwardCallBacks(logForwardId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/14
*@purpose : Adds a new log filter
*@param   : Command
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogShowTag(Command &cmd)
{
	std::string filterTag = cmd.GetNextString();
	LogManager::GetInstance()->LogShowTag(filterTag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/14
*@purpose : Removes log filter
*@param   : Command
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogHideTag(Command &cmd)
{
	std::string filterTag = cmd.GetNextString();
	LogManager::GetInstance()->LogHideTag(filterTag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogShowDevConsoleTag(Command &cmd)
{
	std::string filterTag = cmd.GetNextString();
	LogManager::GetInstance()->LogShowDevConsoleTag(filterTag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogHideDevConsoleTag(Command &cmd)
{
	std::string filterTag = cmd.GetNextString();
	LogManager::GetInstance()->LogHideDevConsoleTag(filterTag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogShowAllDevConsoleTag(Command &cmd)
{
	UNUSED(cmd);
	LogManager::GetInstance()->LogShowAllDevConsoleTag();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogHideAllDevConsoleTag(Command &cmd)
{
	UNUSED(cmd);
	LogManager::GetInstance()->LogHideAllDevConsoleTag();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogShowAllTag(Command &cmd)
{
	UNUSED(cmd);
	LogManager::GetInstance()->LogShowAllTag();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/07/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogHideAllTag(Command &cmd)
{
	UNUSED(cmd);
	LogManager::GetInstance()->LogHideAllTag();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : Sends messages
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Connect(Command &cmd)
{
	std::string ip   = cmd.GetNextString();
	std::string port = cmd.GetNextString();
	std::string data = cmd.GetNextString();
	char *returnMsg = nullptr;
	TCPSocket::SendMsg((char*)ip.c_str(), (char*)port.c_str(), (char*)data.c_str(), data.length(),returnMsg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Listen(Command &cmd)
{
	int port;
	bool success  = cmd.GetNextInt(&port);
	if(success)
	{
		Thread::ThreadCreateAndDetach("net_listen", TCPServer::ListenOnThread, &port);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TestRCSMsg(Command &cmd)
{
	int connectionIndex;
	cmd.GetNextInt(&connectionIndex);
	std::string msg = cmd.GetNextString();
	RCS::GetInstance()->SendMsg(connectionIndex, false, msg.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCExecute(Command &cmd)
{
	std::string indexString = cmd.GetNextString();
	int index = -1;
	bool success = false;
	if(StartsWith(indexString,"idx"))
	{
		size_t equalIndex = indexString.find('=');
		std::string connectionIndexString = indexString.substr(equalIndex + 1, indexString.size());
		success = ToInt(connectionIndexString, &index);
		std::string rcmd = cmd.GetNextString();
		RCS::GetInstance()->SendMsg(index, false, rcmd.c_str());
		return;
	}
	RCS::GetInstance()->SendMsg(0, false, indexString.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : RCHost 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCHost(Command &cmd)
{
	std::string portString = cmd.GetNextString();
	size_t equalIndex = portString.find('=');
	std::string portnumString = portString.substr(equalIndex + 1, portString.size());
	int port = -1;
	bool success = ToInt(portnumString, &port);
	if(!success)
	{
		port = RCS::GetInstance()->m_rcsPort;
	}
	if (RCS::GetInstance()->m_tcpServer != nullptr)
	{
		RCS::GetInstance()->CleanUpHosting();
		RCS::GetInstance()->m_rcsPort = port;
		RCS::GetInstance()->Host();
	}
	else
	{
		RCS::GetInstance()->m_rcsPort = port;
		RCS::GetInstance()->Host();
	}
	//RCS::GetInstance()->m_state = RCS_STATE_CLIENT_FAILED;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : Joins new sessions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCJoin(Command &cmd)
{
	std::string addressString    = cmd.GetNextString();
	size_t colonIndex			 = addressString.find(':');
	std::string ipaddress        = addressString.substr(0, colonIndex);
	std::string portStr		     = addressString.substr(colonIndex+1,addressString.size());

	RCS::GetInstance()->DisconnectAndCleanUpAllConnections();
	RCS::GetInstance()->m_ipaddress  = ipaddress;
	int port = 0;
	if(ToInt(portStr, &port))
	{
		RCS::GetInstance()->m_rcsPort = port;
		RCS::GetInstance()->m_state = RCS_STATE_FORCE_JOIN;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : Executes command 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCBExecute(Command &cmd)
{
	std::string command    = cmd.GetNextString();
	RCS::GetInstance()->SendMsg(false, command.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCAExecute(Command &cmd)
{
	std::string command = cmd.GetNextString();
	RCS::GetInstance()->SendMsg(false, command.c_str());
	CommandRun(command.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RCEcho(Command &cmd)
{
	std::string toggle    = cmd.GetNextString();
	size_t colonIndex		  = toggle.find(':');
	std::string value     = toggle.substr(colonIndex+1, toggle.size());
	bool echoEnabled	  = false;
	if (value == "enabled")
	{
		echoEnabled = true;
	}
	RCS::GetInstance()->m_isHookedToDevConsole = echoEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/12
*@purpose : Sends a msg via UDP protocol
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UDPPacketSend(Command &cmd)
{
	UNUSED(cmd);
	std::string ip = "192.168.0.123";
	int port = 10084;
	std::string msg = "hello";
	/*std::string ip = cmd.GetNextString();
	int port = 0;
	if(!cmd.GetNextInt(&port))
	{
		return;
	}
	std::string msg = cmd.GetNextString();*/
	size_t writeSize = UDPSend(ip.c_str(), port, msg.c_str());
	DevConsole::GetInstance()->PushToOutputText("SEND " + ToString(writeSize) + " SUCCESSFULLY");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Adds a new UDP Connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AddUDPConnection(Command &cmd)
{
	int index = -1;
	cmd.GetNextInt(&index);
	if(index == -1)
	{
		return;
	}
	std::string ipaddress   = cmd.GetNextString();
	std::string ip			= ipaddress.substr(0, ipaddress.find(':'));
	std::string port		= ipaddress.substr(ipaddress.find(':')+1,ipaddress.length());
	int portNum = -1;
	if(ToInt(port,&portNum))
	{
		if(NetSession::GetInstance()->AddConnection(index,ip,portNum) != nullptr)
		{
			DevConsole::GetInstance()->PushToOutputText("CONNECTION SUCCEED");
		}
		else
		{
			DevConsole::GetInstance()->PushToOutputText("CONNECTION FAILED");
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/27
*@purpose : Sends ping to a UDP connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendPing(Command &cmd)
{
	int connectionIndex = -1;
	if (cmd.GetNextInt(&connectionIndex))
	{
		NetConnection* connection = NetSession::GetInstance()->GetConnection(connectionIndex);
		if (connection)
		{
			std::string pingString = cmd.GetNextString();
			DevConsole::GetInstance()->PushToOutputText("SENDING PING MSG -> "+ pingString+ " TO " + connection->GetIPPortAsString(),Rgba::YELLOW);
			NetMessage msg("ping");
			size_t msgSize = 0;
			// write temporarily 
			msg.WriteBytes(2, (char*)&msgSize);
			///////////////
			msg.WriteCommandIndex();
			msg.WriteString(pingString.c_str());
			msg.m_currentWritePosition = 0;
			msgSize = msg.m_bufferSize - 2;
			msg.WriteBytes(2, (char*)&(msgSize));
			std::string str = msg.GetBitString();
			connection->SendImmediately(msg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/27
*@purpose : Sends add msg via UDP connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendAdd(Command &cmd)
{
	int connectionIndex = -1;
	if (cmd.GetNextInt(&connectionIndex))
	{
		NetConnection* connection = NetSession::GetInstance()->GetConnection(connectionIndex);
		if (connection)
		{
			float value1 = 0;
			float value2 = 0;
			cmd.GetNextFloat(&value1);
			cmd.GetNextFloat(&value2);

			NetMessage msg("add");
			size_t msgSize = 0;
			// write temporarily 
			msg.WriteBytes(2, (char*)&msgSize);
			///////////////
			msg.WriteCommandIndex();
			msg.WriteBytes(4, (char*)&value1);
			msg.WriteBytes(4, (char*)&value2);
			DevConsole::GetInstance()->PushToOutputText("SENDING ADD MSG ADDING " + ToString(value1) + " & " + ToString(value2) + " TO " + connection->GetIPPortAsString(),Rgba::YELLOW);
			msg.m_currentWritePosition = 0;
			msgSize = msg.m_bufferSize - 2;
			msg.WriteBytes(2, (char*)&(msgSize));
			std::string str = msg.GetBitString();
			connection->SendImmediately(msg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendCombo(Command &cmd)
{
	int connectionIndex = 0;
	int maxCommands = 0;
	cmd.GetNextInt(&connectionIndex);
	cmd.GetNextInt(&maxCommands);
	NetConnection* connection = NetSession::GetInstance()->GetConnection(connectionIndex);
	std::vector<NetMessage*> msgs;
	for(int index = 0;index < maxCommands;index++)
	{
		float value = GetRandomFloatZeroToOne();
		NetMessage *netmsg = nullptr;
		if(value > 0.5)
		{
			DevConsole::GetInstance()->PushToOutputText("SENDING PING MSG -> helloping to " + connection->GetIPPortAsString());
			netmsg = NetMessage::CreatePingMessage("helloping");
		}
		else
		{
			float random1 = GetRandomFloatInRange(0, 100);
			float random2 = GetRandomFloatInRange(0, 100);
			DevConsole::GetInstance()->PushToOutputText("SENDING ADD MSG ADDING "+ToString(random1) +" & "+ToString(random2)+ " TO "+connection->GetIPPortAsString());
			netmsg = NetMessage::CreateAddMessage(random1,random2);
		}
		msgs.push_back(netmsg);
	}
	connection->SendImmediately(connectionIndex,msgs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/04
*@purpose : Send bad command
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendBad(Command &cmd)
{
	int connectionIndex = -1;
	int commandSize = 0;
	if (cmd.GetNextInt(&connectionIndex))
	{
		NetConnection* connection = NetSession::GetInstance()->GetConnection(connectionIndex);
		cmd.GetNextInt(&commandSize);
		//if (connection)
		{
			char badMsg[70000];
			NetMessage netmsg("");
			//int randomSize = GetRandomIntInRange(1, 1000);
			netmsg.WriteBytes(commandSize, badMsg);
			connection->SendImmediately(netmsg);
			DevConsole::GetInstance()->PushToOutputText("SENDING BAD MSG TO " +connection->GetIPPortAsString());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Sends command over UDP
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendCommandOverUDP(Command &cmd)
{
	//send add 0 24 32
	std::string netcmd  = cmd.GetNextString();
	int connectionIndex = -1;
	if(cmd.GetNextInt(&connectionIndex))
	{
		NetConnection* connection = NetSession::GetInstance()->GetConnection(connectionIndex);
		if(connection)
		{
			if(netcmd == "add")
			{
				return;
			}
			if(netcmd == "ping")
			{

			}

			NetMessage msg(netcmd);
			size_t size = 7;
			// write temporarily 
			msg.WriteBytes(2, (char*)&size);
			///////////////
			msg.WriteCommandIndex();
			msg.WriteString("hello");
			std::string str = msg.GetBitString();
			connection->SendImmediately(msg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : Sets udp session send rate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetUDPSessionSendRate(Command &cmd)
{
	//net_set_session_send_rate 20
	//net_set_connection_send_rate 0 10
	int sentRate = 0;
	cmd.GetNextInt(&sentRate);
	NetSession::GetInstance()->m_sendRate = static_cast<float>(sentRate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : Sets UDP Connections send rate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetUDPConnectionSendRate(Command &cmd)
{
	//net_set_connection_send_rate 0 10
	int connectionIndex = -1;
	int sendRate		= 0;
	cmd.GetNextInt(&connectionIndex);
	cmd.GetNextInt(&sendRate);
	NetSession::GetInstance()->GetConnection(connectionIndex)->m_sendRate = static_cast<float>(sendRate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/03
*@purpose : Sets udp session loss rate
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetUDPSessionLossRate(Command &cmd)
{
	float lossAmount = 0;
	cmd.GetNextFloat(&lossAmount);
	NetSession::GetInstance()->m_lossAmount = lossAmount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : Sets min and max latency  udp netsession
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetUDPSessionMinMaxLatency(Command &cmd)
{
	float minLatency = 0;
	float maxLatency = 0;
	cmd.GetNextFloat(&minLatency);
	cmd.GetNextFloat(&maxLatency);
	NetSession::GetInstance()->m_minLatency = minLatency;
	NetSession::GetInstance()->m_maxLatency = maxLatency;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Netsession recv port
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ListenUDPPort(Command &cmd)
{
	int port = -1;
	int index = -1;
	cmd.GetNextInt(&port);
	cmd.GetNextInt(&index);
	NetSession::GetInstance()->s_defaultPort = port;
	NetSession::GetInstance()->RestartInPort(index,port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetupUDPConnections(Command &cmd)
{
	int index = 0;
	cmd.GetNextInt(&index);

	std::string rcmd = "listen_udp 10085 1";
	RCS::GetInstance()->SendMsg(index, false, rcmd.c_str());
	std::string myIp = Net::GetIP();
	std::string addConnection0 = "add_connection 0 "+myIp+":10084";
	std::string addConnection1 = "add_connection 1 "+myIp+":10085";
	
	RCS::GetInstance()->SendMsg(index, false, addConnection0.c_str());
	RCS::GetInstance()->SendMsg(index, false, addConnection1.c_str());

	NetSession::GetInstance()->AddConnection(0, myIp, 10084);
	NetSession::GetInstance()->AddConnection(1, myIp, 10085);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetupUDPConnectionWithoutPortChangeInRemote(Command &cmd)
{
	int index = 0;
	cmd.GetNextInt(&index);

	std::string addConnection0 = "add_connection 0 192.168.0.123:10084";
	std::string addConnection1 = "add_connection 1 192.168.0.123:10085";
	RCS::GetInstance()->SendMsg(index, false, addConnection0.c_str());
	RCS::GetInstance()->SendMsg(index, false, addConnection1.c_str());

	NetSession::GetInstance()->AddConnection(0, "192.168.0.123", 10084);
	NetSession::GetInstance()->AddConnection(1, "192.168.0.123", 10085);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Adds local connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Host(Command &cmd)
{
	UNUSED(cmd);
	int port = 0;
	cmd.GetNextInt(&port);
	NetSession::s_defaultPort = port;
	NetSession::GetInstance()->Host("", static_cast<uint16_t>(port));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AddRemote(Command &cmd)
{
	UNUSED(cmd);
	std::string addConnection0 = "add_connection 1 192.168.0.123:10085";
	NetSession::GetInstance()->AddConnection(1,"192.168.0.123",10085);
	RCS::GetInstance()->SendMsg(0, false, addConnection0.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Disconnect(Command &cmd)
{
	int index = -1;
	cmd.GetNextInt(&index);
	NetConnection *connection = NetSession::GetInstance()->GetConnection(index);
	if(connection == nullptr)
	{
		return;
	}
	connection->Disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EraseAllQueues(Command &cmd)
{
	int connectionIndex = -1;
	cmd.GetNextInt(&connectionIndex);
	NetConnection *connection = NetSession::GetInstance()->GetConnection(connectionIndex);
	for(int index = 0;index < connection->m_unsentReliableMsgs.size();index++)
	{
		connection->m_unsentReliableMsgs.erase(connection->m_unsentReliableMsgs.begin() + index, connection->m_unsentReliableMsgs.begin() + index + 1);
		index--;
	}
	for (int index = 0; index < connection->m_unconfirmedReliableMessages.size(); index++)
	{
		connection->m_unconfirmedReliableMessages.erase(connection->m_unconfirmedReliableMessages.begin() + index, connection->m_unconfirmedReliableMessages.begin() + index + 1);
		index--;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : Sends a join request
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendJoinRequest(Command &cmd)
{

	std::string ipaddress = cmd.GetNextString();
	std::string portStr = "";
	std::string ip		= "";
	if(ipaddress == "")
	{
		ipaddress = "192.168.0.123";
		portStr   = "10084";
	}
	else
	{
		portStr = ipaddress.substr(ipaddress.find(':') + 1, ipaddress.length());
		ip      = ipaddress.substr(0, ipaddress.find(':'));
	}

	NetAddress *address = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;
	NetAddress::GetRemoteAddress(address, (sockaddr*)&out, &out_addrlen, ip.c_str(), portStr.c_str());
	
	NetConnectionInfo connectionInfo;
	connectionInfo.m_address = *address;

	NetSession::GetInstance()->Join("", connectionInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DoHost(Command &cmd)
{
	int port = 0;
	cmd.GetNextInt(&port);
	NetSession::s_defaultPort = port;

	std::string myIp = Net::GetIP();
	NetSession::GetInstance()->Host("", static_cast<uint16_t>(port));
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
	//CommandStartup();
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



