#pragma once
#include <string>
#include <map>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
/*\class  : Command  
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   : Command class for dev console		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 2/2/2018 3:09:59 PM
* \contact: srsrakhil@gmail.com
*/
 
// A command is a single submitted commmand
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" );
class Command;
typedef void (*command_cb)(Command &cmd);

struct CommandDefinition
{
public:
	CommandDefinition(std::string name, command_cb callback, std::string description)
	{
		m_name        = name;
		m_callback    = callback;
		m_description = description;
	}
	std::string m_name;
	command_cb m_callback;
	std::string m_description;
	CommandDefinition() {};
};

class Command
{
public:
	std::string m_name;
	std::string m_atrib;
	std::string m_command;
	int m_readIndex = 0;
	Command();
	Command(char const *str);
	~Command();

	std::string GetName(); // would return "echo_with_color"

	std::string GetNextString();   

	bool GetNextInt(int *out_val);
	bool GetNextFloat(float *out_val);
	bool GetNextColor(Rgba *out_val);
	bool GetNextVector2(Vector2 *out_val);
	bool GetNextVector3(Vector3 *out_val);
	bool GetNextVector4(Vector4 *out_val);
};

// Command callbacks take a Command.
static std::vector<std::string> commandHistory;
static int commandHistoryIndex = 0;
static bool isDevConsoleOutputActive = true;

void CommandStartup();
void CommandShutdown();

static std::map<std::string, CommandDefinition> commandRegistery;
void CommandRegister(char const *name, command_cb cmdCB,std::string helpText);
std::vector<CommandDefinition> RetrieveAllCommands();

bool CommandRun(char const *command);
bool CommandRun(char const *command, char const *output);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//COMMANDS
void TestCommand(Command &cmd);
void HelpCommand(Command &cmd);
void ClearCommand(Command &cmd);
void SaveLog(Command &cmd);
void EchoColor(Command &cmd);
void ToggleDebugRenderVisibility(Command &cmd);
void ClearDebugRenderDraws(Command &cmd);

//DEBUG RENDER COMMANDS
void DebugRenderQuad2D(Command &cmd);
void DebugRenderText2D(Command &cmd);
void DebugRenderPoint3D(Command &cmd);
void DebugRenderSphere(Command &cmd);
void DebugRenderWiredSphere(Command &cmd);
void DebugRenderText3D(Command &cmd);

// PROFILER COMMMANDS
void ProfilerResume(Command &cmd);
void ProfilerPause(Command &cmd);
void MouseShow(Command &cmd);
void MouseHide(Command &cmd);
void ProfilerShow(Command &cmd);
void ProfilerHide(Command &cmd);
void ProfilerFlatView(Command &cmd);
void ProfilerTreeView(Command &cmd);

// THREAD TEST COMMANDS
void MainThreadTest(Command &cmd);
void NewThreadTest (Command &cmd);
// LOG TEST
void LogFlushTest(Command &cmd);
void LogReadWriteTest(Command &cmd);

void AttachDevConsoleLogging(Command &cmd);
void DetachDevConsoleLogging(Command &cmd);

void LogShowTag(Command &cmd);
void LogHideTag(Command &cmd);

void LogShowDevConsoleTag(Command &cmd);
void LogHideDevConsoleTag(Command &cmd);

void LogShowAllDevConsoleTag(Command &cmd);
void LogHideAllDevConsoleTag(Command &cmd);

void LogShowAllTag(Command &cmd);
void LogHideAllTag(Command &cmd);


void Connect(Command &cmd);// char *ip, char *port, char* data, int dataLength);
void Listen(Command &cmd);

// RC COMMANDS
void TestRCSMsg(Command &cmd);
void RCExecute(Command &cmd);
void RCHost(Command &cmd);
void RCJoin(Command &cmd);
void RCBExecute(Command &cmd);
void RCAExecute(Command &cmd);
void RCEcho(Command &cmd);

void UDPPacketSend(Command &cmd);
void AddUDPConnection(Command &cmd);
void SendPing(Command &cmd);
void SendAdd(Command &cmd);
void SendCombo(Command &cmd);
void SendBad(Command &cmd);
void SendCommandOverUDP(Command &cmd);

void SetUDPSessionSendRate(Command &cmd);
void SetUDPConnectionSendRate(Command &cmd);
void SetUDPSessionLossRate(Command &cmd);
void SetUDPSessionMinMaxLatency(Command &cmd);

void ListenUDPPort(Command &cmd);

void SetupUDPConnections(Command &cmd);
void SetupUDPConnectionWithoutPortChangeInRemote(Command &cmd);
void Host(Command &cmd);
void AddRemote(Command &cmd);
void Disconnect(Command &cmd);
void EraseAllQueues(Command &cmd);

void SendJoinRequest(Command &cmd);
void DoHost(Command &cmd);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::vector<std::string> GetCommandHistory();

void CommandRunScript(char const *script);
void CommandRunScriptFile(char const *filename);
