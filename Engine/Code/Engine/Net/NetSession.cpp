#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
NetSession *NetSession::s_netSession = nullptr;
int			NetSession::s_defaultPort = 10085;
// CONSTRUCTOR
NetSession::NetSession(int port)
{
	m_channel = new NetConnection(port);
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
	RegisterMessage("ping",			OnPing,			"Pings connection");
	RegisterMessage("add",			OnAdd,			"Adds 2 numbers");
	RegisterMessage("add_response", OnAddResponse,  "Adds a response");
	RegisterMessage("pong",			OnPong,			"Send pong on a connection");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Creates a  new instance and return
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetSession* NetSession::GetInstance()
{
	if(s_netSession == nullptr)
	{
		s_netSession = new NetSession(s_defaultPort);
	}
	return s_netSession;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Update(float deltaTime)
{
	UNUSED(deltaTime);
	ProcessIncomingMessage();
	ProcessOutgoingMessage();
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
	char data[PACKET_MTU];
	size_t maxsize = PACKET_MTU;
	NetAddress netAddr;
	size_t recvd = m_channel->Recv(data, maxsize,&netAddr);
	if(recvd > 0)
	{
		ProcessMsg(ConstructMsgFromData(netAddr,recvd,data),&netAddr);
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
	for (it = m_remoteConnections.begin(); it != m_remoteConnections.end(); it++)
	{
		NetConnection * netConnection = it->second;
		char data[PACKET_MTU];
		size_t size = 0;
		//netConnection->Recv(data, size);
		if (size > 0)
		{
			int a = 1;
		}
		//ProcessMsg(ConstructNetMsgFromData(data));
	}

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
/*DATE    : 2018/09/23
*@purpose : Adds new connection 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::AddConnection(int index, NetAddress *netaddress)
{
	std::map<int, NetConnection*>::iterator it = m_remoteConnections.find(index);
	if(it == m_remoteConnections.end())
	{
		NetConnection *connection  = new NetConnection(index, *netaddress);
		connection->m_session = this;
		m_remoteConnections[index] = connection;
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
	NetAddress *address = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;
	NetAddress::GetRemoteAddress(address, (sockaddr*)&out, &out_addrlen, ip.c_str(), ToString(port).c_str());
	return AddConnection(index, address);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::GetConnection(int index)
{
	std::map<int, NetConnection*>::iterator it = m_remoteConnections.find(index);
	if(it != m_remoteConnections.end())
	{
		return it->second;
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
	for(it = m_remoteConnections.begin();it != m_remoteConnections.end();it++)
	{
		if(it->second->m_address == *netAddress)
		{
			return it->second;
		}
	}
	return nullptr;
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
	// - 01 01->packet header
	// - 07 00 02 [ 05 'h' 'e' 'l' 'l' 'o' ]
	// - 00000001 00000001 
	// - 00000010 00000000 00000010 00000000
	BytePacker recvdPacket;
	recvdPacket.WriteBytes(size, data);
	std::string str = recvdPacket.GetBitString();

	char connectionIndex;
	char unreliableCount;
	recvdPacket.ReadBytes(&connectionIndex, 1);
	recvdPacket.ReadBytes(&unreliableCount, 1);
	if(connectionIndex <0 || connectionIndex > m_remoteConnections.size())
	{
		DevConsole::GetInstance()->PushToOutputText("BAD MSG RECEVIED FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port)+" SIZE "+ToString(static_cast<int>(size)),Rgba::RED);
		return retmsgs;
	}
	for(int msgCount = 0;msgCount < static_cast<int>(unreliableCount);msgCount++)
	{
		uint16_t msgSize = recvdPacket.ReadSize2();
		char cmdIndex;
		recvdPacket.ReadBytes(&cmdIndex, 1);
		//char cmdSize;
		//recvdPacket.ReadBytes(&cmdSize, 1);
		//char msg[1000];
		//recvdPacket.ReadString(msg, cmdSize);
		//std::string msgstr (msg, cmdSize);
		//NetMessageDefinition * msgdef = GetMsgDefinition(static_cast<int>(cmdIndex));
		// (char*)m_buffer + m_currentReadPosition
		// 

		if(cmdIndex >=0 && cmdIndex < m_netMessageCmdDefinition.size())
		{
			NetMessage *netmsg = new NetMessage(GetMsgName(static_cast<int>(cmdIndex)));
			netmsg->WriteBytes(msgSize - 1, ((char*)recvdPacket.m_buffer + recvdPacket.m_currentReadPosition));
			recvdPacket.m_currentReadPosition += msgSize - 1;
			//(*(iter->second.m_callback))(newcommand);
			//(*msgdef->m_callback)();
			//NetMessage *netmsg = new NetMessage(GetMsgName(static_cast<int>(cmdIndex)));
			retmsgs.push_back(netmsg);
		}
		else
		{
			DevConsole::GetInstance()->PushToOutputText("BAD MSG RECEVIED FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port)+" SIZE "+ToString(static_cast<int>(size)),Rgba::RED);
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
void NetSession::ProcessMsg(std::vector<NetMessage *> netmsg,NetAddress *fromAddress)
{
	UNUSED(netmsg);
	for(int msgIndex = 0; msgIndex < netmsg.size(); msgIndex++)
	{
		NetMessageDefinition * msgdef = GetMsgDefinition(netmsg.at(msgIndex)->m_definitionName);
		(*msgdef->m_callback)(*netmsg.at(msgIndex), *fromAddress);
	}
	// check for the all function pointers stored in session and call back
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
	netConnection->Send(pongMsg);
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
	netConnection->Send(addResponse);
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
/*DATE    : 2018/10/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnBadMessage(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	DevConsole::GetInstance()->PushToOutputText("RECEIVED BAD MSG FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port), Rgba::YELLOW, true);
	return true;
}
