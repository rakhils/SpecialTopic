#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Net/NetObject.hpp"

NetSession *NetSession::s_netSession = nullptr;
int			NetSession::s_defaultPort = 10084;
// CONSTRUCTOR
NetSession::NetSession(int port)
{
	UNUSED(port);
	/*m_channel = new NetConnection(port);
	m_channel->m_session = this;*/
	Init();
}

// DESTRUCTOR
NetSession::~NetSession()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Inits basic msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Init()
{	
	RegisterMessage(NETMSG_PING,	  "ping", OnPing, "Pings connection",NETMESSAGE_OPTION_CONNECTIONLESS);
	RegisterMessage(NETMSG_PONG,	  "pong", OnPong, "Pings connection",NETMESSAGE_OPTION_CONNECTIONLESS);		
	RegisterMessage(NETMSG_HEARTBEAT, "heartbeat", OnHeartBeatMessage,"Update last heartbeat received",NETMESSAGE_OPTION_CONNECTIONLESS);
	RegisterMessage(NETMSG_BLANK,     "blank",     OnHeartBeatMessage, "Update last heartbeat received", NETMESSAGE_OPTION_CONNECTIONLESS);

	RegisterMessage(NETMSG_JOIN_REQUEST,		"join_request",		 OnJoinRequest,     "UPDATE RECV STATE",			    NETMESSAGE_OPTION_UNRELIABLE);
	RegisterMessage(NETMSG_JOIN_DENY,			"join_deny",		 OnJoinDeny   ,     "UPDATE STATE TO DROP", 			NETMESSAGE_OPTION_UNRELIABLE);
	RegisterMessage(NETMSG_JOIN_ACCEPT,			"join_accept",		 OnJoinAccept ,     "UPDATE STATE TO ACCEPT",			NETMESSAGE_OPTION_RELIALBE_IN_ORDER);
	RegisterMessage(NETMSG_NEW_CONNECTION,		"new_connection",	 OnNewConnection,   "UPDATE STATE TO NEW CON", 	     	NETMESSAGE_OPTION_RELIALBE_IN_ORDER);
	RegisterMessage(NETMSG_JOIN_FINISHED,		"join_finished",	 OnJoinFinished,    "UPDATE STATE TO FINISHED JOIN",	NETMESSAGE_OPTION_RELIALBE_IN_ORDER);
	RegisterMessage(NETMSG_HANGUP,				"msg_hangup",		 OnHangUp,			"HANGS UP THE CONNECTION",			NETMESSAGE_OPTION_UNRELIABLE);
	RegisterMessage(NETMSG_UPDATE_CONN_STATE,	"update_conn_state", OnUpdateConnState, "UPDATE CONN STATE",				NETMESSAGE_OPTION_RELIALBE_IN_ORDER);

	m_netObjectSystem = new NetObjectSystem();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : Updates netsession
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Update(float deltaTime)
{
	UNUSED(deltaTime);
	if(InputSystem::GetInstance()->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_F2))
	{
		m_sessionInfoVisible = m_sessionInfoVisible ? false : true;
	}
	uint64_t localDeltams = Clock::GetMasterClock()->frame.m_milliSeconds;
	uint64_t localNetworkDelta = 0;

	m_desiredClientTimems += localDeltams;

	if(m_hostConnection != nullptr && !m_hostConnection->IsHost() && m_currentClientTimems > 0)
	{
		Rgba color = Rgba::GREEN;
		Rgba color1 = Rgba::RED;
		
		if(m_currentClientTimems + localDeltams > m_desiredClientTimems)
		{
			localNetworkDelta =  static_cast<uint64_t>( static_cast<float>((1.f - MAX_NET_TIME_DILATION) * static_cast<float>(localDeltams)));
			//DebugDraw::GetInstance()->DebugRenderLogf(deltaTime,color,"NEGATIVE");
		}
		else if (m_currentClientTimems + localDeltams < m_desiredClientTimems)
		{
			localNetworkDelta =  static_cast<uint64_t>( static_cast<float>((1.f + MAX_NET_TIME_DILATION) * static_cast<float>(localDeltams)));
			//DebugDraw::GetInstance()->DebugRenderLogf(deltaTime,color1,"POSITIVE");
		}
		m_currentClientTimems += localNetworkDelta;
	}
	if(m_hostConnection != nullptr && m_hostConnection->IsHost())
	{
		m_currentClientTimems = Clock::GetMasterClock()->total.m_milliSeconds;
	}
	UpdateConnections(deltaTime);
	ProcessIncomingMessage();
	ProcessOutgoingMessage();
	UpdateSessionState();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::UpdateSessionState()
{
	switch(m_sessionState)
	{
		case SESSION_CONNECTING:
			if(m_hostConnection->IsConnected())
			{
				m_sessionState = SESSION_JOINING;
			}
			else if(GetCurrentTimeSeconds() > m_hostConnection->m_lastJoinReqstSentTime + m_hostConnection->m_joinInterval)
			{
				if(m_hostConnection->m_firstJoinReqstTime == 0)
				{
					m_hostConnection->m_firstJoinReqstTime = GetCurrentTimeSeconds();
				}
				if(m_hostConnection->m_firstJoinReqstTime + 10 < GetCurrentTimeSeconds())
				{
					//
				}
				NetMessage *msg = NetMessage::CreateJoinRequestMsg(&m_hostConnection->m_address);
				m_hostConnection->Append(msg);
				m_hostConnection->m_lastJoinReqstSentTime = GetCurrentTimeSeconds();
			}
			break;
		case SESSION_READY:
			{
				m_sessionState = SESSION_JOINING;
			}
			break;
		case SESSION_JOINING:
			if(m_myConnection->IsReady())
			{
				m_sessionState = SESSION_CONNECTED;
			}
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Updates all connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::UpdateConnections(float deltaTime)
{
	UNUSED(deltaTime);
	
	std::map<int, NetConnection*>::iterator it;
	for (it = m_boundConnections.begin(); it != m_boundConnections.end(); it++)
	{
		float time = static_cast<float>(GetCurrentTimeSeconds());
		if(time > (it->second->m_lastReceivedTime + DEFAULT_CONNECTION_TIMEOUT))
		{
			it->second->Disconnect();
			DestroyConnection(it->second);
			//RemoveConnections(it->second);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Restart netsession in given port
*@param   : Port
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RestartInPort(int index,int port)
{
	delete m_hostConnection;
	m_hostConnection = new NetConnection(port);
	m_boundConnections[index] = m_hostConnection;
	m_hostConnection->m_session = this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Host(char const *id, uint16_t port)
{
	if (m_sessionState != SESSION_DISCONNECTED || m_hostConnection != nullptr)
	{
		m_sessionError = SESSION_ERROR_USER;
		return;
	}

	s_defaultPort = static_cast<int>(port);
	m_hostConnection = new NetConnection(s_defaultPort);
	m_hostConnection->m_index = 0;
	m_hostConnection->m_session = this;
	m_myConnection = m_hostConnection;
	m_hostConnection->m_id = id;

	int wsaError = WSAGetLastError();
	if (wsaError != 0)
	{
		m_sessionError = SESSION_ERROR_INTERNAL;
		delete m_hostConnection;
		m_hostConnection = nullptr;
		m_myConnection = nullptr;
		return;
	}
	m_sessionState = SESSION_READY;
	m_sessionError = SESSION_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::Join(char const *name, NetConnectionInfo &connectionInfo)
{
	UNUSED(name);
	NetConnection *connection		= CreateConnection(connectionInfo);
	connection->BindConnection();
	connection->m_connectionState   = CONNECTION_CONNECTING;
	connection->m_index				= connectionInfo.m_sessionIndex;
	m_allConnections.push_back(connection);
	m_boundConnections[connectionInfo.m_sessionIndex] = connection;

	m_sessionState = SESSION_CONNECTING;
	m_hostConnection = connection;

	NetAddress *address = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;
	std::string port = "10085";
//	int portInt = 10085;
	NetAddress::GetRemoteAddress(address, (sockaddr*)&out, &out_addrlen, Net::GetIP().c_str(), ToString(s_defaultPort).c_str());

	NetConnectionInfo ownConnectionInfo;
	ownConnectionInfo.m_sessionIndex = INVALID_SESSION_INDEX;
	ownConnectionInfo.m_address		 = *address;
	ownConnectionInfo.m_isHost		 = true;
	m_myConnection					 = CreateHostConnection(ownConnectionInfo,s_defaultPort);
	m_myConnection->m_connectionState = CONNECTION_READY;
	return connection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Sets heartbeat for all connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::SetHeartBeatFrequency(float freq)
{
	std::map<int, NetConnection*>::iterator it;
	for(it = m_boundConnections.begin();it != m_boundConnections.end();it++)
	{
		it->second->SetHeartBeatFrequency(freq);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/15
*@purpose : Simulates loss amount(Simulation real world traffic)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::SetSimulateLoss(float lossAmount)
{
	m_lossAmount = lossAmount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/15
*@purpose : Sets min and max latency
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::SetSimulateLatency(float minLatency, float maxLatency)
{
	m_minLatency = minLatency;
	m_maxLatency = maxLatency;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ESessionError NetSession::GetLastSessionError()
{
	ESessionError tempError = m_sessionError;
	m_sessionError = SESSION_OK;
	return tempError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t NetSession::GetAndIncrementConnectionIndex()
{
	for(int index = 0;index < m_allConnections.size();index++)
	{
		//if(m_allConnections.at(index)-m>)	
		//if(m_)
	}
	return m_highestIndex++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/01
*@purpose : Removes connections from bounded connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RemoveConnections(NetConnection *connection)
{
	std::map<int, NetConnection*>::iterator it = m_boundConnections.find(connection->m_index);
	if(it != m_boundConnections.end())
	{
		m_boundConnections.erase(it);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : Checks if this is a host session
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetSession::IsHost()
{
	if(m_hostConnection == nullptr)
	{
		return false;
	}
	if(m_sessionState < SESSION_READY)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Reserves spaces for msg definition
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::InitMsgDefinition()
{
	for (size_t msgDefinitionIndex = 0; msgDefinitionIndex < static_cast<size_t>(NETMSG_CORE_COUNT); msgDefinitionIndex++)
	{
		NetMessageDefinition netMsgDefinition;
		m_netMessageCmdDefinition.push_back(netMsgDefinition);
	}
	//NetSession::GetInstance()->RegisterMessage((eNetCoreMessage)NETMSG_BLANK, "test_blank", OnBlankMsg,
		//"TEST AN BLANK MSGS");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Registers new callback msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RegisterMessage(std::string msgName, NetMessageCB netMsgCB, std::string description)
{
	NetMessageDefinition netMsgDefinition(msgName, netMsgCB, description);
	for (int index = 0; index < m_netMessageCmdDefinition.size(); index++)
	{
		NetMessageDefinition msgDef = m_netMessageCmdDefinition.at(index);
		if(Compare(msgDef.m_name,msgName) == 1)
		{
			m_netMessageCmdDefinition.insert(m_netMessageCmdDefinition.begin()+index,netMsgDefinition);
			return;
		}
	}
	m_netMessageCmdDefinition.push_back(netMsgDefinition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Registers core msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RegisterMessage(eNetCoreMessage coreMsg, std::string name, NetMessageCB cb, std::string desc, eNetMessageOption flag)
{
	if(!m_initMsgDefinition)
	{
		InitMsgDefinition();
		m_initMsgDefinition = true;
	}
	if(static_cast<size_t>(coreMsg) >= m_netMessageCmdDefinition.size())
	{
		for(size_t index = m_netMessageCmdDefinition.size();index <= static_cast<size_t>(coreMsg);index++)
		{
			NetMessageDefinition netMsgDefinition;
			m_netMessageCmdDefinition.push_back(netMsgDefinition);
		}
	}

	NetMessageDefinition &netMsgDefinition = m_netMessageCmdDefinition.at(static_cast<size_t>(coreMsg));
	netMsgDefinition.Init(coreMsg, name, cb, desc, flag);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : retrieves msg definitions
*@param   : String id
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessageDefinition * NetSession::GetMsgDefinition(std::string cmdName)
{
	for (int index = 0; index < m_netMessageCmdDefinition.size(); index++)
	{
		if(m_netMessageCmdDefinition.at(index).m_name == cmdName)
		{
			return &m_netMessageCmdDefinition.at(index);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : returns msg definition by index
*@param   : Msg index
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessageDefinition * NetSession::GetMsgDefinition(int index)
{
	if(index > m_netMessageCmdDefinition.size())
	{
		return nullptr;
	}
	return &m_netMessageCmdDefinition.at(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NetSession::GetMsgIndex(std::string cmdname)
{
	for(int index = 0;index < m_netMessageCmdDefinition.size();index++)
	{
		if(m_netMessageCmdDefinition.at(index).m_name == cmdname)
		{
			return index;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/26
*@purpose : returns msg name from index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string NetSession::GetMsgName(int index)
{
	if(index > m_netMessageCmdDefinition.size())
	{
		return "";
	}
	return m_netMessageCmdDefinition.at(index).m_name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetObjectSystem * NetSession::GetNetObjectSystem()
{
	return m_netObjectSystem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Verify msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetSession::VerifyMessage(std::string cmd,std::string msg)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessIncomingMessage()
{
	size_t recvd = 0;
	
	if(m_myConnection != nullptr)
	{
		do
		{
			char data[PACKET_MTU];
			size_t maxsize = PACKET_MTU;
			NetAddress netAddr;
			recvd = m_myConnection->Recv(data, maxsize, &netAddr);

			std::vector<NetMessage*> netMsgs = ConstructMsgFromData(netAddr, recvd, data);
			ProcessMsgs(netMsgs, &netAddr);
		} while (recvd > 0);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : Process all incoming msg from vector of connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessOutgoingMessage()
{
	std::map<int, NetConnection*>::iterator it;
	for (it = m_boundConnections.begin(); it != m_boundConnections.end(); it++)
	{
		it->second->SendHeartBeat(&it->second->m_address);
		it->second->FlushMsgs();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Sends packet to sender
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetSession::SendPacket(Packet packet)
{
	return m_myConnection->m_udpSocket->SendTo(*packet.m_address, packet.m_packet.m_buffer, packet.m_packet.m_bufferSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : Broadcast msg to all bounded connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::BroadcastMsg(NetMessage *msg,EConnectionState state)
{
	std::map<int, NetConnection*>::iterator it;
	for (it = m_boundConnections.begin(); it != m_boundConnections.end(); it++)
	{
		if(state >= it->second->m_connectionState)
		{
			NetMessage *copyMsg = new NetMessage(msg);
			copyMsg->SetAddress(&it->second->m_address);
			it->second->Append(copyMsg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/29
*@purpose : Check if connecection exist for an address
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetSession::DoesConnectionExist(NetAddress address)
{
	for (size_t index = 0; index < m_allConnections.size(); index++)
	{
		if(m_allConnections.at(index)->m_address == address)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::AddBinding(int port)
{
	UNUSED(port);
	//NetConnection *connection = new NetConnection(port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::CreateAndPushIntoAllConnection(NetConnectionInfo &connectionInfo)
{
	std::map<int, NetConnection*>::iterator it = m_boundConnections.begin();
	for(;it != m_boundConnections.end();it++)
	{
		if(it->second->m_address == connectionInfo.m_address)
		{
			return nullptr;
		}
	}
	// NOT EXIST IN ALREADY BOUNDED LIST
	for(size_t index = 0;index < m_allConnections.size();index++)
	{
		if(m_allConnections.at(index)->m_address == connectionInfo.m_address)
		{
			// EXIST ALREADY
			return nullptr;
		}
	}

	NetConnection *connection = new NetConnection(connectionInfo.m_address.m_port);
	connection->m_index       = connectionInfo.m_sessionIndex;
	connection->m_address     = connectionInfo.m_address;
	connection->m_session = this;

	m_allConnections.push_back(connection);
	return connection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::CreateHostConnection(NetConnectionInfo &connectionInfo,int port)
{
	for(int index = port;index < port + 10;index++)
	{
		int error = WSAGetLastError();
		UNUSED(error);
		s_defaultPort = index;
		NetConnection *connection = new NetConnection(s_defaultPort);
		int error1 = WSAGetLastError();
		if(error1 != 0)
		{
			delete connection;
			continue;
		}
		connection->m_index = connectionInfo.m_sessionIndex;
		connection->m_address = connectionInfo.m_address;
		connection->m_session = this;
		return connection;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::CreateConnection(NetConnectionInfo &connectionInfo)
{
	NetConnection *connection   = new NetConnection(connectionInfo.m_sessionIndex, connectionInfo.m_address);
	connection->m_index			= connectionInfo.m_sessionIndex;
	connection->m_session		= this;
	connection->CreateNetObjectConectionView();
	return connection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::DestroyConnection(NetConnection *connection)
{
	/*if(m_hostConnection == connection)
	{
		return;
	}*/

	uint8_t connectionIndex = connection->m_index;



	
	m_leave.Trigger(connection);





	if (m_hostConnection != nullptr && m_hostConnection->IsHost())
	{
		std::map<int, NetConnection*>::iterator it;
		for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
		{
			NetMessage *msg = NetMessage::CreateObjectDestroyMsg(NETOBJ_PLAYER, connectionIndex,it->second->m_index);
			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}

		//m_leave.Trigger(connection);
		//BroadcastMsg(msg, CONNECTION_READY);
	}

	std::map<int, NetConnection*>::iterator it = m_boundConnections.find(connection->m_index);
	if(it == m_boundConnections.end())
	{
		return;
	}
	DevConsole::GetInstance()->PushToOutputText("REMOVED CONNECTION TO " + connection->m_address.GetIP() + ":" + ToString(connection->m_address.m_port));
	delete m_boundConnections[connection->m_index];
	//m_boundConnections[connection->m_index] = nullptr;
	m_boundConnections.erase(it);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Adds new connection 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::AddConnection(int index, NetAddress *netaddress)
{
	std::map<int, NetConnection*>::iterator it = m_boundConnections.find(index);
	if(it == m_boundConnections.end())
	{
		NetConnection *connection = nullptr;
		if(netaddress->m_port == s_defaultPort)
		{
			connection  = new NetConnection(s_defaultPort);
			connection->BindConnection();
			m_hostConnection = connection;
		}
		else
		{
			connection = new NetConnection(index, *netaddress);
		}
		connection->m_session = this;
		m_boundConnections[index] = connection;
		return connection;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Adds a new connection from ip port
*@param   : Ip and port
*@return  : returns new connection obj
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::AddConnection(int index,std::string ip, int port)
{
	if (ip == Net::GetIP() && port == s_defaultPort)
	{
		if(m_hostConnection != nullptr)
		{
			return m_hostConnection;
		}
	}

	NetAddress *address = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;
	NetAddress::GetRemoteAddress(address, (sockaddr*)&out, &out_addrlen, ip.c_str(), ToString(port).c_str());
	NetConnection *connection = AddConnection(index, address);
	connection->m_index = static_cast<uint8_t>(index);
	connection->m_session = this;
	return connection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::GetConnection(int index)
{
	std::map<int, NetConnection*>::iterator it = m_boundConnections.find(index);
	if(it != m_boundConnections.end())
	{
		return it->second;
	}
	if(index == 0)
	{
		return m_myConnection;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/27
*@purpose : Gets connection from address
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::GetConnection(NetAddress *netAddress)
{
	std::map<int, NetConnection*>::iterator it;
	for(it = m_boundConnections.begin();it != m_boundConnections.end();it++)
	{
		if(it->second->m_address == *netAddress)
		{
			return it->second;
		}
	}
	if(m_hostConnection->m_address == *netAddress)
	{
		return m_hostConnection;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::GetConnectionFromAllConnections(NetAddress *address)
{
	for(size_t index = 0;index < m_allConnections.size();index++)
	{
		if(m_allConnections.at(index)->m_address == *address)
		{
			return m_allConnections.at(index);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetSession::RemoveFromAllConnections(NetAddress  address)
{
	for (size_t index = 0; index < m_allConnections.size(); index++)
	{
		if (m_allConnections.at(index)->m_address == address)
		{
			m_allConnections.erase(m_allConnections.begin()+index, m_allConnections.begin()+ index + 1);
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ReplaceConnectionWithNewIndex(uint8_t index,NetConnection *connection)
{
	std::map<int, NetConnection*>::iterator it = m_boundConnections.find(connection->m_index);
	if (it != m_boundConnections.end())
	{
		m_boundConnections.erase(it);
		m_boundConnections[index] = connection;
		connection->m_index = index;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Returns my udp connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::GetMyConnection()
{
	return m_hostConnection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Returns session index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NetSession::GetMySessionIndex()
{
	return m_hostConnection->m_index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Closes all connections related to this session
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::CloseAllConnections()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<NetMessage*> NetSession::ConstructMsgFromData(NetAddress &netAddress, size_t size, void *data)
{
	std::vector<NetMessage*> retmsgs;
	if (size < m_minHeaderSize)
	{
		return retmsgs;
	}
	//   00000000	00000000	00000000	00000000	00000000	00000000	00000000	00000000	00000000	00000000
	//   |cnindx|   |       relID	   |	|       ack		   |    |  last recv ack   |    |prevRecAckBitField|    |msgcnt|
	//
	// 
	
	if(size > 1000)
	{
		DevConsole::GetInstance()->PushToOutputText("BAD MSG " + netAddress.GetIP() + ":" + ToString(netAddress.m_port) + " SIZE " + ToString(static_cast<int>(size)), Rgba::RED);
		return retmsgs;
	}
	BytePacker recvdPacket;
	recvdPacket.WriteBytes(size, data);
	std::string str = recvdPacket.GetBitString();

	NetConnection* connection = GetConnection(&netAddress);
	int index = -1;
	if(connection != nullptr)
	{
		connection->SetLastRecvTime(static_cast<float>(GetCurrentTimeSeconds()));
		std::map<int, NetConnection*>::iterator it;
		for (it = m_boundConnections.begin(); it != m_boundConnections.end(); it++)
		{
			if (it->second->m_address == netAddress)
			{
				index = it->first;
			}
		}
	}
	else
	{
		DevConsole::GetInstance()->PushToOutputText("BAD CONNECTION INDEX FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port) + " SIZE " + ToString(static_cast<int>(size))+ "DATA "+str, Rgba::RED);
		//return retmsgs;

	}

	std::string recvData = recvdPacket.GetBitString();

	PacketHeader header = connection->GetHeaderFromPacket(data);

	//uint8_t connectionIndex = header.m_connectionindex;
	uint8_t unreliableCount = header.m_unrealiableCount;

	/*if(index == -1)
	{
		DevConsole::GetInstance()->PushToOutputText("NO INDEX PRESENT " + netAddress.GetIP() + ":" + ToString(netAddress.m_port) + " SIZE " + ToString(static_cast<int>(size)) + "DATA " + str, Rgba::RED);
		return retmsgs;
	}*/
	if(unreliableCount <= 0)
	{
		DevConsole::GetInstance()->PushToOutputText("BAD MSG RECEVIED UC = 0 FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port)+" SIZE "+ToString(static_cast<int>(size)),Rgba::RED,true);
		return retmsgs;
	}
	
	recvdPacket.AdvanceReadHead(connection->GetHeaderSize());
	for(int msgCount = 0;msgCount < static_cast<int>(unreliableCount);msgCount++)
	{
		uint16_t msgSize = recvdPacket.ReadSize2();
		uint8_t cmdIndex;
		recvdPacket.ReadBytes(&cmdIndex, 1);
		if(cmdIndex >=0 && cmdIndex < m_netMessageCmdDefinition.size())
		{
			//DevConsole::GetInstance()->PushToOutputText(m_netMessageCmdDefinition.at(cmdIndex).m_name +" MSG RECEVIED FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port)+" SIZE "+ToString(static_cast<int>(size)),Rgba::RED,true);
			NetMessage *netmsg = new NetMessage(GetMsgName(static_cast<int>(cmdIndex)));
			netmsg->m_packetHeader = header;
			netmsg->WriteBytes(msgSize - 1, ((char*)recvdPacket.m_buffer + recvdPacket.m_currentReadPosition));
			recvdPacket.m_currentReadPosition += msgSize - 1;
			retmsgs.push_back(netmsg);
			std::string pack = recvdPacket.GetBitString();
			std::string msgD = netmsg->GetBitString();
		}
		else
		{
			DevConsole::GetInstance()->PushToOutputText("BAD MSG RECEVIED FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port)+" SIZE "+ToString(static_cast<int>(size)),Rgba::RED,true);
			return retmsgs;
		}
	}
	return retmsgs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessMsgs(std::vector<NetMessage *> netmsg,NetAddress *fromAddress)
{
	float currSec = static_cast<float>(GetCurrentTimeSeconds());
	for (int msgIndex = 0; msgIndex < netmsg.size(); msgIndex++)
	{
		netmsg.at(msgIndex)->m_lag     = currSec + GetRandomFloatInRange(m_minLatency, m_maxLatency);
		netmsg.at(msgIndex)->m_address = new NetAddress(*fromAddress);
	}

	for (int laggedMsgindex = 0; laggedMsgindex < m_laggedMsgs.size(); laggedMsgindex++)
	{
		float currentTimeSec = static_cast<float>(GetCurrentTimeSeconds());
		if (m_laggedMsgs.at(laggedMsgindex)->m_lag > currentTimeSec)
		{
			continue;
		}
		NetAddress *fromAddr = m_laggedMsgs.at(laggedMsgindex)->m_address;
		ProcessMsg(m_laggedMsgs.at(laggedMsgindex), fromAddr);
		m_laggedMsgs.erase(m_laggedMsgs.begin() + laggedMsgindex, m_laggedMsgs.begin() + laggedMsgindex + 1);
		laggedMsgindex--;
	}

	for(int msgIndex = 0; msgIndex < netmsg.size(); msgIndex++)
	{
		if(netmsg.at(msgIndex)->m_lag > static_cast<float>(GetCurrentTimeSeconds()))
		{
			m_laggedMsgs.push_back(netmsg.at(msgIndex));
			netmsg.erase(netmsg.begin() + msgIndex, netmsg.begin() + msgIndex + 1);
			msgIndex --;
			continue;
		}
		std::string bitString = netmsg.at(0)->GetBitString();
		std::string name = netmsg.at(msgIndex)->m_definitionName;
		netmsg.at(msgIndex)->m_currentReadPosition = 3;
		ProcessMsg(netmsg.at(msgIndex), fromAddress);
	}

	// check for the all function pointers stored in session and call back
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Process single msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessMsg(NetMessage *msg, NetAddress *fromAddr)
{
	NetConnection *fromConnection = GetConnection(fromAddr);
	NetMessageDefinition * msgdef = GetMsgDefinition(msg->m_definitionName);
	PacketHeader header			  = msg->m_packetHeader;
	/*if (fromConnection == nullptr)
	{
		return;
	}*/
	if(fromConnection != nullptr)
	{
		fromConnection->m_lastReceivedTime = Clock::GetMasterClock()->total.m_seconds;// -fromConnection->m_startTime;
		fromConnection->m_lastReceivedAck = header.m_ack;
	}
	
	if (fromConnection != nullptr && msgdef != nullptr && (msgdef->m_options == NETMESSAGE_OPTION_RELIABLE || msgdef->m_options == NETMESSAGE_OPTION_RELIALBE_IN_ORDER))
	{
		//std::string bitString = msg->GetBitString();
		if (!fromConnection->HasReceivedReliableID(msg->GetReliableID()))
		{
			//DevConsole::GetInstance()->PushToOutputText("RECEVIED REL ID "+ ToString(static_cast<int>(relID)),Rgba::GREEN);
			msg->m_address = fromAddr;
			fromConnection->PushToInboundMsgQueue(msg);
			fromConnection->DoProcessInboundMsgQueue();
		}
		
		NetMessage *blankMsg = NetMessage::CreateBlankMessage(fromConnection);
		fromConnection->Append(blankMsg);
	}
	else
	{
		if ( msgdef != nullptr)
		{
			(*msgdef->m_callback)(*msg, *fromAddr);
		}
	}
	fromConnection = GetConnection(fromAddr);
	if(fromConnection != nullptr && header.m_lastReceivedAck != INVALID_PACKET_ACK)
	{
		//DevConsole::GetInstance()->PushToOutputText("### CONFIRMING PACKET RECV"+ToString(header.m_lastReceivedAck),Rgba::GREEN);
		fromConnection->UpdateAckStatus(fromAddr, header.m_lastReceivedAck);
		fromConnection->ConfirmPacketReceived(header.m_lastReceivedAck);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : Pushes outbound msgs to vector
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void NetSession::PushOutboundMsgs(NetAddress address,NetMessage *msg)
{
	OutBoundMSG outboudMsg;
	outboudMsg.m_address = address;
	outboudMsg.m_msg     = msg;
	m_outboudMsgs.push_back(outboudMsg);
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : Render netsession info
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Render()
{
	if(!m_sessionInfoVisible)
	{
		return;
	}
	
	RenderSessionDetails();
	Vector2 startPosition(100, 700);
	RenderConnectionColumnDetails(startPosition);
	if (m_hostConnection != nullptr)
	{
		startPosition.y += -50;
		RenderConnectionDetails(m_hostConnection,startPosition,false);
	}
	std::map<int, NetConnection*>::iterator it = m_boundConnections.begin();
	for(;it != m_boundConnections.end();it++)
	{
		if (it->second == m_hostConnection)
		{
			continue;
		}
		startPosition.y += -50;
		RenderConnectionDetails(it->second,startPosition,false);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/29
*@purpose : Renders session details
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RenderSessionDetails()
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	//Material *defaultMaterial = Material::AquireResource("default");

	float fontSizeBig = 15;
	float fontSize = 10;
	Vector2 startPosition(100, 900);
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOnPoint("SESSION INFO", startPosition, fontSizeBig, Rgba::WHITE);
	startPosition -= Vector2(0, 2 * fontSizeBig);
	Renderer::GetInstance()->DrawTextOnPoint("sim lag:", startPosition + Vector2(50, 0), fontSize, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOnPoint(ToString(m_minLatency) + " S  " + ToString(m_maxLatency) + " S", startPosition + Vector2(250, 0), fontSize, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOnPoint("sim loss:", startPosition + Vector2(900, 0), fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOnPoint(ToString(m_lossAmount) + "%", startPosition + Vector2(1100, 0), fontSize, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOnPoint("Time :", startPosition + Vector2(1300, 0), fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOnPoint(ToString(m_currentClientTimems), startPosition + Vector2(1500, 0), fontSize, Rgba::WHITE);
	
	startPosition -= Vector2(0, 2 * fontSizeBig);
	Renderer::GetInstance()->DrawTextOnPoint("Socket Address(es)...", startPosition + Vector2(100, 0), fontSize, Rgba::WHITE);
	startPosition -= Vector2(0, 2 * fontSizeBig);
	Renderer::GetInstance()->DrawTextOnPoint(Net::GetIP() + ":" + ToString(s_defaultPort), startPosition + Vector2(200, 0), fontSize, Rgba::WHITE);
	startPosition -= Vector2(0, 2 * fontSizeBig);
	Renderer::GetInstance()->DrawTextOnPoint("Connections...", startPosition + Vector2(150, 0), fontSize, Rgba::WHITE);
	startPosition -= Vector2(0, 2 * fontSizeBig);

	startPosition -= Vector2(0, 2 * fontSizeBig);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/03
*@purpose : Renders session columns
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RenderConnectionColumnDetails(Vector2 startPosition)
{
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	float fontSize = 10;

	int indent = 1;
	std::string entryString = Stringf("%*s%-*s %s %-22s %-7s %-7s %-7s %-7s %-7s %-7s %-7s", indent, "",
		(3 * indent), "",
		"idx", "address", "rtt", "loss", "lrcv(s)", "lsnt(s)", "sntack", "rcvack", "rcvbits");
	startPosition.x = 25;
	fontSize = 9;
	Renderer::GetInstance()->DrawTextOnPoint(entryString, startPosition, fontSize, Rgba::WHITE);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RenderConnectionDetails(NetConnection *connection,Vector2 startPosition,bool skipHeading)
{
	UNUSED(skipHeading);
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	
	float fontSize = 10;

	int indent = 1;
	startPosition.x = 25;
	fontSize = 9;

	int index = connection->m_index;
	std::string ip = connection->GetIPPortAsString();
	float rtt = connection->m_rtt;
	float loss = connection->m_loss;

	double lrcv = connection->m_lastReceivedTime;
	lrcv = Clock::GetMasterClock()->total.m_seconds - lrcv;
	
	double lsnt = connection->m_lastSendTime;
	lsnt = Clock::GetMasterClock()->total.m_seconds - lsnt;
	
	uint16_t sntack = connection->m_nextSentAck;
	uint16_t rcvack = connection->m_lastReceivedAck;
	uint16_t prevRcvBit = connection->m_previousReceivedAckBitField;
	//std::string entryString1;
	Renderer::GetInstance()->BindMaterial(textMaterial);
	std::string connectionDetailsStr;
	if (connection == m_hostConnection)
	{
		connectionDetailsStr = Stringf("%*s%-*s %s   %-22s %-7s %-7s %-7s %-7s %-7s %-7s %-7s", indent, "",
			(3 * indent), "L", ToString(index).c_str(), ip.c_str(), ToString(rtt, 2.f).c_str(), ToString(loss, 2.f).c_str(), ToString(lrcv, 2.f).c_str(), ToString(static_cast<float>(lsnt),2.f).c_str(), ToString(sntack).c_str(), ToString(rcvack).c_str(), ToBitString(prevRcvBit).c_str());
	}
	else
	{
		connectionDetailsStr = Stringf("%*s%-*s %s   %-22s %-7s %-7s %-7s %-7s %-7s %-7s %-7s", indent, "",
			(3 * indent), "", ToString(index).c_str(), ip.c_str(), ToString(rtt, 2.f).c_str(), ToString(loss, 2.f).c_str(), ToString(lrcv, 2.f).c_str(), ToString(static_cast<float>(lsnt),2.f).c_str(), ToString(sntack).c_str(), ToString(rcvack).c_str(), ToBitString(prevRcvBit).c_str());
	}
	Renderer::GetInstance()->DrawTextOnPoint(connectionDetailsStr, startPosition, fontSize, Rgba::WHITE);
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnPing(NetMessage &netMsg, NetAddress &netAddress)
{
	//   01 01->packet header
	// - 07 00 02 [ 05 'h' 'e' 'l' 'l' 'o' ]
	
	uint8_t pingSize;
	netMsg.ReadBytes(&pingSize, 1);
	char chararr[1000];
	netMsg.ReadString(chararr, static_cast<size_t>(pingSize));
	std::string strmsg(chararr, pingSize);
	DevConsole::GetInstance()->PushToOutputText("RECEIVED PING MSG  FROM "+netAddress.GetIP()+":"+ToString(netAddress.m_port)+" MSG-> "+strmsg, Rgba::YELLOW, true);
	NetMessage pongMsg("pong");

	//temp write
	uint16_t size = 0;// temporary write
	pongMsg.WriteBytes(2, (void*)&size);
	// ---------------

	pongMsg.WriteCommandIndex();
	uint8_t size2  = 0; // pong size
	pongMsg.WriteBytes(1, (void*)&size2);
	size_t msgSize = pongMsg.m_bufferSize - 2;
	pongMsg.m_currentWritePosition = 0;
	pongMsg.WriteBytes(2, (void*)&(msgSize));
	
	std::string pongMsgStr = pongMsg.GetBitString();

	NetConnection *netConnection = NetSession::GetInstance()->GetConnection(&netAddress);
	netConnection->SendImmediately(pongMsg);
	DevConsole::GetInstance()->PushToOutputText(" SENT PONG TO "+netAddress.GetIP()+":"+ToString(netAddress.m_port), Rgba::YELLOW, true);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnAdd(NetMessage &netMsg, NetAddress &netAddress)
{
	//07 00 02 [ 05 'h' 'e' 'l' 'l' 'o' ]
	//09 00 00 [ <float> <float> ]
	/*std::string bitString = netMsg.GetBitString();
	uint16_t rmsgSize = netMsg.ReadSize2();
	char cmdIndex;
	netMsg.ReadBytes(&cmdIndex, 1);*/
	float value1 = 0;
	float value2 = 0;
	netMsg.ReadBytes(&value1, 4);
	netMsg.ReadBytes(&value2, 4);
	float sum = value2 + value1;
	DevConsole::GetInstance()->PushToOutputText("RECEIVED ADD MSG FROM "+netAddress.GetIP()+":"+ToString(netAddress.m_port)+" ADDING "+ToString(value1)+ " AND "+ToString(value2) + " = "+ToString(sum) ,Rgba::YELLOW, true);
	NetMessage addResponse("add_response");

	//temp write
	uint16_t size = 0;// temporary write
	addResponse.WriteBytes(2, (void*)&size);
	// ---------------
	//09 00 00 [ <float> <float> ]
	addResponse.WriteCommandIndex(); 
	addResponse.WriteBytes(4, (void*)&value1);
	addResponse.WriteBytes(4, (void*)&value2);
	addResponse.WriteBytes(4, (void*)&(sum));

	size_t msgSize = addResponse.m_bufferSize - 2;
	addResponse.m_currentWritePosition = 0;
	addResponse.WriteBytes(2, (void*)&(msgSize));

	std::string pongMsgStr = addResponse.GetBitString();

	NetConnection *netConnection = NetSession::GetInstance()->GetConnection(&netAddress);
	DevConsole::GetInstance()->PushToOutputText(" SENDING ADD RESPONSE TO "+netAddress.GetIP()+":"+ToString(netAddress.m_port) , Rgba::YELLOW, true);
	netConnection->SendImmediately(addResponse);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnPong(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	UNUSED(netMsg);
	DevConsole::GetInstance()->PushToOutputText("RECEIVED PONG FROM "+netAddress.GetIP()+":"+ToString(netAddress.m_port) ,Rgba::YELLOW,true);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnAddResponse(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	float value1 = 0;
	float value2 = 0;
	float value3 = 0;
	netMsg.ReadBytes(&value1, 4);
	netMsg.ReadBytes(&value2, 4);
	netMsg.ReadBytes(&value3, 4);
	DevConsole::GetInstance()->PushToOutputText("RECEIVED ADD RESPONSE FROM "+netAddress.GetIP()+":"+ToString(netAddress.m_port) +" ADDED : "+ToString(value1)+" + "+ToString(value2)+" = "+ToString(value3),Rgba::YELLOW,true);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : On heart beat msg received
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnHeartBeatMessage(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	NetConnection *netConnection = NetSession::GetInstance()->GetConnection(&netAddress);
	netMsg.m_currentReadPosition = 3;
	if(netConnection != nullptr)
	{
		netConnection->SetLastHeartBeatReceivedTime(static_cast<float>(GetCurrentTimeSeconds()));
		//DevConsole::GetInstance()->PushToOutputText("RECEVIED HEART BEAT " + netAddress.GetIP() + ":" + ToString(netAddress.m_port));
	}

	uint64_t recvdTime = 0;
	netMsg.ReadBytes(&recvdTime, 8);

	//if(m_hostConnection != nullptr && !NetSession::GetInstance()->IsHost() && m_currentClientTimems > 0)

	if(NetSession::GetInstance()->m_hostConnection != nullptr && !NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		if(NetSession::GetInstance()->m_lastRecvdHostTimems < recvdTime)
		{
			uint64_t temp = recvdTime + (static_cast<uint64_t>((netConnection->m_rtt / 2.f) * 1000));  
			NetSession::GetInstance()->m_lastRecvdHostTimems = temp;

			NetSession::GetInstance()->m_desiredClientTimems = temp;// NetSession::GetInstance()->m_lastRecvdHostTimems;
		//	DebugDraw::GetInstance()->DebugRenderLogf(Clock::GetMasterClock()->frame.m_seconds * 25,Rgba::YELLOW ,"HEART BEAT RECV VALUE %d" , (static_cast<int>(recvdTime)));
			//DebugDraw::GetInstance()->DebugRenderLogf(deltaTime, color1, "POSITIVE");

			if(NetSession::GetInstance()->m_currentClientTimems == 0)
			{
				NetSession::GetInstance()->m_currentClientTimems = NetSession::GetInstance()->m_desiredClientTimems;
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnBadMessage(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg)
	UNUSED(netAddress);
	DevConsole::GetInstance()->PushToOutputText("RECEIVED BAD MSG FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port), Rgba::YELLOW, true);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnBlankMessage(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	UNUSED(netAddress);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : When recevied join request
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnJoinRequest(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	UNUSED(netAddress);
	DevConsole::GetInstance()->PushToOutputText("ON JOIN REQUEST ");
	if(!NetSession::GetInstance()->IsHost())
	{
		NetMessage *joinDeny = NetMessage::CreateJoinDeny(&netAddress);
		NetSession::GetInstance()->m_hostConnection->Append(joinDeny);
		return false;
	}
	if(NetSession::GetInstance()->m_highestIndex > MAX_CONN_ID_LENGTH)
	{
		NetMessage *joinDeny = NetMessage::CreateJoinDeny(&netAddress);
		NetSession::GetInstance()->m_hostConnection->Append(joinDeny);
		return false;
	}
	// CHECK ONLY IN ALL CONNECTION , CONNECTIONS EXISTING IN ALL CANNOT JOIN WITHOUT DISCONNECT 
	// MAY BE A BUG
	if (NetSession::GetInstance()->DoesConnectionExist(netAddress))
	{
		return false;
	}

	NetConnectionInfo connectionInfo;
	connectionInfo.m_address = netAddress;
	connectionInfo.m_sessionIndex = NetSession::GetInstance()->GetAndIncrementConnectionIndex();
	connectionInfo.m_isHost = false;

	//NetSession::GetInstance()->AddIntoAllConnection("", connectionInfo);

	NetConnection *connection =  NetSession::GetInstance()->CreateConnection(connectionInfo);
	connection->BindConnection();
	connection->m_connectionState = CONNECTION_CONNECTING;
	connection->m_index = connectionInfo.m_sessionIndex;
	NetSession::GetInstance()->m_allConnections.push_back(connection);
	NetSession::GetInstance()->m_boundConnections[connectionInfo.m_sessionIndex] = connection;

	//NetSession::GetInstance()->m_sessionState = SESSION_CONNECTING;

	NetMessage *joinAccept = NetMessage::CreateJoinAcceptMsg(&netAddress,connectionInfo.m_sessionIndex);
	connection->Append(joinAccept);

	//NetMessage *joinFinished = NetMessage::CreateJoinFinished(connection);
	//connection->Append(joinFinished);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnJoinDeny(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	UNUSED(netAddress);
	DevConsole::GetInstance()->PushToOutputText("ON JOIN DENY ");
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnJoinAccept(NetMessage &netMsg, NetAddress &netAddress)
{
	
	DevConsole::GetInstance()->PushToOutputText("ON JOIN ACCEPT ");
	netMsg.m_currentReadPosition = 5;
	uint8_t index = 0;
	netMsg.ReadBytes(&index, 1);

	NetConnection *connection	= NetSession::GetInstance()->GetConnectionFromAllConnections(&netAddress);
	NetSession::GetInstance()->ReplaceConnectionWithNewIndex(index, connection);
	connection->m_connectionState = CONNECTION_CONNECTING;
	connection->m_index = index;


	DevConsole::GetInstance()->PushToOutputText("ON JOIN FINISHED ");
	connection = NetSession::GetInstance()->GetConnection(&netAddress);
	connection->m_connectionState = CONNECTION_READY;
	NetSession::GetInstance()->m_sessionState = SESSION_READY;


	NetMessage *msg = NetMessage::CreateConnUpdate(connection);
	connection->Append(msg);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnNewConnection(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	UNUSED(netAddress);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnJoinFinished(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	DevConsole::GetInstance()->PushToOutputText("ON JOIN FINISHED ");
	NetConnection *connection = NetSession::GetInstance()->GetConnection(&netAddress);
	connection->m_connectionState = CONNECTION_READY;
	NetSession::GetInstance()->m_sessionState = SESSION_READY;

	NetMessage *msg = NetMessage::CreateConnUpdate(connection);
	connection->Append(msg);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnUpdateConnState(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netMsg);
	DevConsole::GetInstance()->PushToOutputText("ON CONN UPDATE ");

	NetConnection *connection = NetSession::GetInstance()->GetConnectionFromAllConnections(&netAddress);
	if(connection == nullptr)
	{
		return true;
	}
	connection->m_connectionState = CONNECTION_READY;

	// SENDING ALL ALREADY EXISTING NET OBJECTS
	for(int index = 0;index < NetSession::GetInstance()->m_netObjectSystem->m_netObjects.size();index++)
	{		
		NetObject *netObject = NetSession::GetInstance()->m_netObjectSystem->m_netObjects.at(index);
		NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_PLAYER, netObject->m_networkID,connection->m_index);
		msg->WriteBytes(12, NetSession::GetInstance()->m_netObjectSystem->m_netObjects.at(index)->m_latestReceivedSnapshot);

		size_t msgSize = msg->m_bufferSize - 2;
		msg->m_currentWritePosition = 0;
		msg->WriteBytes(2, (char*)&msgSize);

		msg->SetAddress(&connection->m_address);
		connection->Append(msg);
		
		/*NetMessage *updateMsg = NetMessage::CreateObjectUpdateMsg(NETOBJ_PLAYER, netObject->m_networkID);
		updateMsg->m_address = &connection->m_address;

		//float posX = memccpy()

		if (NetSession::GetInstance()->m_netObjectSystem->m_netObjects.at(index)->m_latestReceivedSnapshot == nullptr)
		{
			float x = 200;
			float y = 200;
			float angle = 0;
			float min = 0;
			updateMsg->WriteBytes(4, &x);
			updateMsg->WriteBytes(4, &y);
			updateMsg->WriteBytes(4, &angle);
			updateMsg->WriteBytes(4, &min);

		}
		else
		{
			updateMsg->WriteBytes(16, NetSession::GetInstance()->m_netObjectSystem->m_netObjects.at(index)->m_latestReceivedSnapshot);
		}
		
		size_t msgSize = updateMsg->m_bufferSize - 2;
		updateMsg->m_currentWritePosition = 0;
		updateMsg->WriteBytes(2, (char*)&msgSize);
		connection->Append(updateMsg);*/

	}
	NetSession::GetInstance()->m_join.Trigger(connection);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnHangUp(NetMessage &netMsg, NetAddress &netAddress)
{
	DevConsole::GetInstance()->PushToOutputText("ON HANG UP ");
	netMsg.m_currentReadPosition = 3;
	uint8_t index = 0;
	netMsg.ReadBytes(&index, 1);

	NetConnection *connection = NetSession::GetInstance()->GetConnectionFromAllConnections(&netAddress);
	if (connection == nullptr)
	{
		return true;
	}
	if(true)
	{
		return true;
	}
	NetSession::GetInstance()->DestroyConnection(connection);
	//NetSession::GetInstance()->RemoveConnections(connection);


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetSession * NetSession::GetInstance()
{
	if(s_netSession == nullptr)
	{
		s_netSession = new NetSession(s_defaultPort);
	}
	return s_netSession;
}
