#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR

NetConnection::NetConnection(int index, NetAddress netAddress)
{
	m_index     = index;
	m_address   = netAddress;
	m_udpSocket = new UDPSocket(netAddress);
	m_udpSocket->SetBlocking(false);
}

NetConnection::NetConnection(int listenPort)
{
	m_udpSocket = new UDPSocket(listenPort);
	m_address.m_port = listenPort;
}

// DESTRUCTOR
NetConnection::~NetConnection()
{
	delete m_udpSocket;
	m_udpSocket = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Updates last heart beat recv time
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetLastHeartBeatReceivedTime(float time)
{
	m_lastHeartbeatReceivedTime = time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Sets heartbeat freq
*@param   : Freq
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetHeartBeatFrequency(float freq)
{
	m_heartBeatFrequency = freq;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Sends msg in UDP Connection
*@param   : NetMsg (Data)
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::SendImmediately(NetMessage msg)
{
	m_header.m_unrealiableCount = static_cast<uint8_t>(1);
	m_header.m_connectionindex  = static_cast<uint8_t>(m_index);
	WriteHeader();
	std::string header = m_packet.GetBitString();
	WritePayload(&msg);
	//00 01
	//07 00 02 05 'hello'
	std::string finalMsg = m_packet.GetBitString();
	size_t length = m_packet.m_bufferSize;
	size_t sendCount = m_session->m_channel->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length);
	m_packet.m_bufferSize = 0;
	delete m_packet.m_buffer;
	return sendCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/03
*@purpose : Sends set of msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::SendImmediately(int connectionIndex, std::vector<NetMessage*> &msgs)
{
	if(msgs.size() <= 0)
	{
		return 0;
	}
	int msgCount = msgs.size();
	int actualSendCount = 0;

	m_header.m_unrealiableCount = actualSendCount;
	m_header.m_connectionindex = static_cast<uint8_t>(m_index);
	m_packet.SetWriteHead(0);
	WriteHeader();

	
	for(int index = 0;index < msgs.size();index++)
	{
		if(WritePayload(msgs.at(index)))
		{
			delete msgs.at(index);
			actualSendCount++;
			msgs.erase(msgs.begin() + index, msgs.begin() + index + 1);
			index--;
			continue;
		}
		m_outboundMsgs.push_back(msgs.at(index));
	}
	msgs.clear();

	m_header.m_unrealiableCount = actualSendCount;
	m_header.m_connectionindex = static_cast<uint8_t>(m_index);
	m_packet.SetWriteHead(0);
	WriteHeader();

	size_t length = m_packet.m_bufferSize;
	size_t sendCount = m_session->m_channel->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length);
	m_packet.Reset();
	return sendCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Append(NetMessage *msg)
{
	m_outboundMsgs.push_back(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::FlushMsgs()
{
	float min = GetMinOf2(m_sendRate, m_session->m_sendRate);
	if (m_lastSendTime + 1 / min > Clock::GetMasterClock()->total.m_milliSeconds)
	{
		m_lastSendTime = Clock::GetMasterClock()->total.m_milliSeconds;
		if (m_outboundMsgs.size() <= 0)
		{
			return 0;
		}
		int actualSendCount = 0;
		m_header.m_unrealiableCount = actualSendCount;
		m_header.m_connectionindex = static_cast<uint8_t>(m_index);
		m_packet.SetWriteHead(0);
		WriteHeader();

		for (int index = 0; index < m_outboundMsgs.size(); index++)
		{
			if (WritePayload(m_outboundMsgs.at(index)))
			{
				delete m_outboundMsgs.at(index);
				actualSendCount++;
				m_outboundMsgs.erase(m_outboundMsgs.begin() + index, m_outboundMsgs.begin() + index + 1);
				continue;
			}
		}

		m_header.m_unrealiableCount = actualSendCount;
		m_header.m_connectionindex = static_cast<uint8_t>(m_index);
		m_packet.SetWriteHead(0);
		WriteHeader();

		size_t length = m_packet.m_bufferSize;
		size_t sendCount = m_session->m_channel->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length);
		m_packet.Reset();
		return sendCount;
	}
	return 0;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/ *DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::Send(NetMessage netmsg)
{
	size_t startIndex = 0;
	size_t endIndex = ETHERNET_MTU;
	if (endIndex > netmsg.m_totalBytes) endIndex = netmsg.m_totalBytes;

	BytePacker bytePacker;
	bytePacker.SetEndianess(LITTLE_ENDIAN);

	int index = 0;
	//chunking all data
	for (; endIndex <= netmsg.m_totalBytes; startIndex += ETHERNET_MTU, endIndex += ETHERNET_MTU)
	{
		if (endIndex > netmsg.m_totalBytes)
		{
			endIndex = netmsg.m_totalBytes;
		}
		bytePacker.WriteSize(startIndex);
		float timeStamp = GetTimeStamp();
		bytePacker.WriteBytes(sizeof(float), (char*)&timeStamp);
		bytePacker.WriteBytes(sizeof(uint16_t), (char*)&netmsg.m_index);
		bytePacker.WriteBytes(endIndex - startIndex, ((char*)netmsg.m_bytePacker.m_buffer + startIndex));
	}
	uint16_t totalPacketCount = static_cast<uint16_t>(index);
	// sneding the index ofdata and how many total packets coming along
	// can do a handshake similar to tcp
	// Node1 : send request 
	// Node2 : send ack
	// Node1 : send all data followed by this


	Endianness::ToEndianness(sizeof(uint16_t), (void *)&netmsg.m_index, BIG_ENDIAN);
	Endianness::ToEndianness(sizeof(uint16_t), (void *)&totalPacketCount, BIG_ENDIAN);

	m_udpSocket->SendTo(m_address, (char*)&netmsg.m_index, 2);
	m_udpSocket->SendTo(m_address, (char*)&totalPacketCount, 2);

	bytePacker.ResetRead();

	while (bytePacker.m_currentReadPosition < bytePacker.m_bufferSize)
	{
		size_t currentPacketSize;
		bytePacker.ReadSize(&currentPacketSize);
		m_udpSocket->SendTo(m_address, ((char*)bytePacker.m_buffer + bytePacker.m_currentReadPosition), currentPacketSize);
		bytePacker.AdvanceReadHead(currentPacketSize);
	}
	return 0;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : Recv msg from UDP
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::Recv(char *data,size_t &length,NetAddress *netAddress)
{
	return m_udpSocket->ReceiveFrom(data, length,netAddress);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/03
*@purpose : returns ip and port as string
*@param   : NIL
*@return  : IP and Port 
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string NetConnection::GetIPPortAsString()
{
	return m_address.GetIP() + ":"+ToString(m_address.m_port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Sets the connection index info
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetConnectionIndex(int index)
{
	m_header.m_connectionindex = static_cast<uint8_t>(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Sets unrealiable msg count
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetUnrealiableMsgCount(int count)
{
	m_header.m_unrealiableCount = static_cast<uint8_t>(count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetSendRate(float sendRate)
{
	m_sendRate = sendRate;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : retrives the udp packet header size
*@param   : NIL
*@return  : header size
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NetConnection::GetHeaderSize()
{
	return sizeof(UDPHeader);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Writes connection index info into packet header
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WriteConnectionIndex()
{
	size_t temp = m_packet.m_currentWritePosition;
	m_packet.m_currentWritePosition = 0;
	m_packet.WriteBytes(1, (char*)&m_header.m_connectionindex);
	m_packet.m_currentWritePosition = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WriteUnrealiableMsgCount()
{
	size_t temp = m_packet.m_currentWritePosition;
	m_packet.m_currentWritePosition = 1;
	m_packet.WriteBytes(1, (char*)&m_header.m_unrealiableCount);
	m_packet.m_currentWritePosition = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Writes the entire header into the packet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WriteHeader()
{
	m_packet.m_currentWritePosition = 0;
	WriteConnectionIndex();
	m_packet.m_currentWritePosition = 1;
	WriteUnrealiableMsgCount();
	m_packet.m_currentWritePosition = 2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::WritePayload(NetMessage *msg)
{
	if(m_packet.m_bufferSize + msg->m_bufferSize + GetHeaderSize() > ETHERNET_MTU)
	{
		return false;
	}
	m_packet.WriteBytes(msg->m_bufferSize, msg->m_buffer);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Sends and flushes  a heartbeat
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SendHeartBeat()
{
	NetMessage *msg = NetMessage::CreateHeartBeatMessage();
	SendImmediately(*msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Updates 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Update(float deltaTime)
{
	if(m_lastHeartbeatHPC + 1/m_heartBeatFrequency < Clock::GetMasterClock()->total.m_seconds)
	{
		SendHeartBeat();
		m_lastHeartbeatHPC = Clock::GetMasterClock()->total.m_seconds;
	}
}
