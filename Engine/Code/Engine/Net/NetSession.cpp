#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Time/Clock.hpp"
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
	//RegisterMessage("ping",			OnPing,			"Pings connection");
	//RegisterMessage("add",			OnAdd,			"Adds 2 numbers");
	//RegisterMessage("add_response", OnAddResponse,  "Adds a response");
	//RegisterMessage("pong",			OnPong,			"Pings connection");
	//RegisterMessage("heartbeat",	OnHeartBeatMessage, "Update last heartbeat received");
	
	RegisterMessage(NETMSG_PING,	  "ping", OnPing, "Pings connection",NETMESSAGE_OPTION_CONNECTIONLESS);
	RegisterMessage(NETMSG_PONG,	  "pong", OnPong, "Pings connection",NETMESSAGE_OPTION_CONNECTIONLESS);		
	RegisterMessage(NETMSG_HEARTBEAT, "heartbeat", OnHeartBeatMessage,"Update last heartbeat received",NETMESSAGE_OPTION_CONNECTIONLESS);
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


	UpdateConnections(deltaTime);
	ProcessIncomingMessage();
	ProcessOutgoingMessage();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Updates all connections
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::UpdateConnections(float deltaTime)
{
	/*if(m_channel != nullptr)
	{
		m_channel->SendHeartBeat(&m_channel->m_address);
		std::map<int, NetConnection*>::iterator it = m_remoteConnections.begin();
		for(;it != m_remoteConnections.end();it++)
		{
			it->second->SendHeartBeat(&(it->second->m_address));
		}
		m_channel->Update(deltaTime);
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Restart netsession in given port
*@param   : Port
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::RestartInPort(int index,int port)
{
	delete m_channel;
	m_channel = new NetConnection(port);
	m_remoteConnections[index] = m_channel;
	m_channel->m_session = this;
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
	for(it = m_remoteConnections.begin();it != m_remoteConnections.end();it++)
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

	if(m_channel != nullptr)
	{
		size_t recvd = m_channel->Recv(data, maxsize,&netAddr);
		std::vector<NetMessage*> netMsgs = ConstructMsgFromData(netAddr, recvd, data);
		ProcessMsgs(netMsgs,&netAddr);
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
	/*m_channel->SendHeartBeat(&m_channel->m_address);
	std::map<int, NetConnection*>::iterator it = m_remoteConnections.begin();
	for (; it != m_remoteConnections.end(); it++)
	{
		it->second->SendHeartBeat(&(it->second->m_address));
	}
	m_channel->Update(deltaTime);*/

	std::map<int, NetConnection*>::iterator it;
	for (it = m_remoteConnections.begin(); it != m_remoteConnections.end(); it++)
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
	return m_channel->m_udpSocket->SendTo(*packet.m_address, packet.m_packet.m_buffer, packet.m_packet.m_bufferSize);
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
		NetConnection *connection = nullptr;
		if(netaddress->m_port == s_defaultPort)
		{
			connection  = new NetConnection(s_defaultPort);
		}
		else
		{
			connection = new NetConnection(index, *netaddress);
		}
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
	if (ip == Net::GetIP() && port == s_defaultPort)
	{
		if(m_channel != nullptr)
		{
			return m_channel;
		}
	}

	NetAddress *address = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;
	NetAddress::GetRemoteAddress(address, (sockaddr*)&out, &out_addrlen, ip.c_str(), ToString(port).c_str());
	NetConnection *connection = AddConnection(index, address);
	connection->m_index = static_cast<uint8_t>(index);
	connection->m_session = this;
	if(ip == Net::GetIP() && port == s_defaultPort)
	{
		m_channel = connection;
	}
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
	if(m_channel->m_address == *netAddress)
	{
		return m_channel;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Returns my udp connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::GetMyConnection()
{
	return m_channel;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Returns session index
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NetSession::GetMySessionIndex()
{
	return m_channel->m_index;
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
		std::map<int, NetConnection*>::iterator it;
		for (it = m_remoteConnections.begin(); it != m_remoteConnections.end(); it++)
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
		return retmsgs;

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
			DevConsole::GetInstance()->PushToOutputText(m_netMessageCmdDefinition.at(cmdIndex).m_name +" MSG RECEVIED FROM " + netAddress.GetIP() + ":" + ToString(netAddress.m_port)+" SIZE "+ToString(static_cast<int>(size)),Rgba::RED,true);
			NetMessage *netmsg = new NetMessage(GetMsgName(static_cast<int>(cmdIndex)));
			netmsg->m_packetHeader = header;
			netmsg->WriteBytes(msgSize - 1, ((char*)recvdPacket.m_buffer + recvdPacket.m_currentReadPosition));
			recvdPacket.m_currentReadPosition += msgSize - 1;
			retmsgs.push_back(netmsg);
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
		netmsg.at(msgIndex)->m_lag = currSec + GetRandomFloatInRange(m_minLatency, m_maxLatency);
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
		NetMessageDefinition * msgdef = GetMsgDefinition(m_laggedMsgs.at(laggedMsgindex)->m_definitionName);

		//if(m_laggedMsgs.at(laggedMsgindex)->RequiresConnection() && GetConnection(fromAddress)!= nullptr)
		{
			//(*msgdef->m_callback)(*m_laggedMsgs.at(laggedMsgindex), *fromAddr);
		}
		float value1 = -1;
		float value2 = -1;
		std::string str = m_laggedMsgs.at(laggedMsgindex)->m_definitionName;
		m_laggedMsgs.at(laggedMsgindex)->m_currentReadPosition = 3;
		m_laggedMsgs.at(laggedMsgindex)->ReadBytes(&value1, 4);
		m_laggedMsgs.at(laggedMsgindex)->ReadBytes(&value2, 4);

		ProcessMsg(m_laggedMsgs.at(laggedMsgindex), fromAddr);
		m_laggedMsgs.erase(m_laggedMsgs.begin() + laggedMsgindex, m_laggedMsgs.begin() + laggedMsgindex + 1);
		laggedMsgindex--;
	}

	for(int msgIndex = 0; msgIndex < netmsg.size(); msgIndex++)
	{
		if(netmsg.at(msgIndex)->m_lag > static_cast<float>(GetCurrentTimeSeconds()))
		{
			m_laggedMsgs.push_back(netmsg.at(msgIndex));
			//netmsg.erase(netmsg.begin() + msgIndex, netmsg.begin() + msgIndex + 1);
			//msgIndex --;
			continue;
		}
		float value1 = -1;
		float value2 = -1;
		std::string name = netmsg.at(msgIndex)->m_definitionName;
		netmsg.at(msgIndex)->m_currentReadPosition = 3;
		netmsg.at(msgIndex)->ReadBytes(&value1, 4);
		netmsg.at(msgIndex)->ReadBytes(&value2, 4);
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
	if (fromConnection == nullptr)
	{
		return;
	}

	NetMessageDefinition * msgdef = GetMsgDefinition(msg->m_definitionName);
	

	fromConnection->m_lastReceivedTime = Clock::GetMasterClock()->total.m_seconds - fromConnection->m_startTime;
	PacketHeader header = msg->m_packetHeader;
	fromConnection->m_lastReceivedAck = header.m_ack;

	if (true)
	{
		if (!fromConnection->HasReceivedReliableID(msg->GetReliableID()))
		{
			if (msg->RequiresConnection())
			{
				(*msgdef->m_callback)(*msg, *fromAddr);
			}
			fromConnection->ConfirmReceivedReliableID(msg->m_reliableID);
		}
	}
	else
	{
		if (msg->RequiresConnection())
		{
				(*msgdef->m_callback)(*msg, *fromAddr);
		}
	}


	if(header.m_lastReceivedAck != INVALID_PACKET_ACK)
	{
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
	Vector2 startPosition(100, 800);
	RenderConnectionColumnDetails(startPosition);
	if (m_channel != nullptr)
	{
		startPosition.y += -50;
		RenderConnectionDetails(m_channel,startPosition,false);
	}
	std::map<int, NetConnection*>::iterator it = m_remoteConnections.begin();
	for(;it != m_remoteConnections.end();it++)
	{
		if (it->second == m_channel)
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
	Vector2 startPosition(100, 1000);
	Renderer::GetInstance()->BindMaterial(textMaterial);
	Renderer::GetInstance()->DrawTextOnPoint("SESSION INFO", startPosition, fontSizeBig, Rgba::WHITE);
	startPosition -= Vector2(0, 2 * fontSizeBig);
	Renderer::GetInstance()->DrawTextOnPoint("sim lag:", startPosition + Vector2(50, 0), fontSize, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOnPoint(ToString(m_minLatency) + " ms  " + ToString(m_maxLatency) + " ms", startPosition + Vector2(250, 0), fontSize, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOnPoint("sim loss:", startPosition + Vector2(900, 0), fontSize, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOnPoint(ToString(m_lossAmount) + "%", startPosition + Vector2(1100, 0), fontSize, Rgba::WHITE);

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
	lrcv = Clock::GetMasterClock()->total.m_seconds - connection->m_startTime - lrcv;
	double lsnt = connection->m_lastSendTime;
	lsnt = Clock::GetMasterClock()->total.m_seconds - connection->m_startTime - lsnt;
	uint16_t sntack = connection->m_nextSentAck;
	uint16_t rcvack = connection->m_lastReceivedAck;
	uint16_t prevRcvBit = connection->m_previousReceivedAckBitField;
	//std::string entryString1;
	Renderer::GetInstance()->BindMaterial(textMaterial);
	std::string connectionDetailsStr;
	if (connection == m_channel)
	{
		connectionDetailsStr = Stringf("%*s%-*s %s   %-22s %-7s %-7s %-7s %-7s %-7s %-7s %-7s", indent, "",
			(3 * indent), "L", ToString(index).c_str(), ip.c_str(), ToString(rtt, 2).c_str(), ToString(loss, 2).c_str(), ToString(lrcv, 2).c_str(), ToString(lsnt,2).c_str(), ToString(sntack).c_str(), ToString(rcvack).c_str(), ToBitString(prevRcvBit).c_str());
	}
	else
	{
		connectionDetailsStr = Stringf("%*s%-*s %s   %-22s %-7s %-7s %-7s %-7s %-7s %-7s %-7s", indent, "",
			(3 * indent), "", ToString(index).c_str(), ip.c_str(), ToString(rtt, 2).c_str(), ToString(loss, 2).c_str(), ToString(lrcv, 2).c_str(), ToString(lsnt,2).c_str(), ToString(sntack).c_str(), ToString(rcvack).c_str(), ToBitString(prevRcvBit).c_str());
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
	if(netConnection != nullptr)
	{
		netConnection->SetLastHeartBeatReceivedTime(static_cast<float>(GetCurrentTimeSeconds()));
		//DevConsole::GetInstance()->PushToOutputText("RECEVIED HEART BEAT " + netAddress.GetIP() + ":" + ToString(netAddress.m_port));
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
