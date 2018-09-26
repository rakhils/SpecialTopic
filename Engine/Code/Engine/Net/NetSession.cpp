#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
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
	if(msgName == "pong")
	{
		//int  a = 1;
	}
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
	char data[ETHERNET_MTU];
	size_t size = 0;
	m_channel->Recv(data, size);
	if(size > 0)
	{
		int a = 1;
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
		netConnection->Recv(data, size);
		if (size > 0)
		{
			int a = 1;
		}
		ProcessMsg(ConstructNetMsgFromData(data));
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
NetMessage * NetSession::ConstructNetMsgFromData(void *data)
{
	UNUSED(data);
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessMsg(NetMessage *netmsg)
{
	if(netmsg == nullptr)
	{
		return;
	}
	UNUSED(netmsg);
	// check for the all function pointers stored in session and call back
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2018/09/23
*@purpose : Add msg
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnAdd(NetMessage &netMsg, NetAddress &netAddress)
{

}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnPing(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	/*std::string str;
	msg.read_string(&str);

	ConsoleInfof("Received ping from %s: %s",
		from.connection->get_address().to_string().c_str(),
		str.c_str());

	// ping responds with pong
	NetMessage pong("pong");
	from.cconnection->send(pong);

	// all messages serve double duty
	// do some work, and also validate
	// if a message ends up being malformed, we return false
	// to notify the session we may want to kick this connection; 
	return true;*/

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
	return true;
}
