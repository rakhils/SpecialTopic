#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Net/UDP/PacketTracker.hpp"
// CONSTRUCTOR

NetConnection::NetConnection(int index, NetAddress netAddress)
{
	m_index     = index;
	m_address   = netAddress;
	m_udpSocket = new UDPSocket(netAddress);
	m_udpSocket->SetBlocking(false);
	InitTrakcer();
}

NetConnection::NetConnection(int listenPort)
{
	m_udpSocket = new UDPSocket(listenPort);
	m_address.m_port = listenPort;
	InitTrakcer();
}

// DESTRUCTOR
NetConnection::~NetConnection()
{
	delete m_udpSocket;
	m_udpSocket = nullptr;

	// delete tracker
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Inits tracker with null values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::InitTrakcer()
{
	for(uint16_t index = 0;index < m_trackerMaxCount;index++)
	{
		m_trackerMap[index] = nullptr;
	}
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
	/*m_header.m_unrealiableCount = static_cast<uint8_t>(1);
	m_header.m_connectionindex = static_cast<uint8_t>(m_index);
	m_header.m_lastReceivedAck = m_lastReceivedAck;
	m_header.m_previousReceivedAckBitfield = m_previousReceivedAckBitField;
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
	return sendCount;*/
	std::vector<NetMessage*> msgs;
	msgs.push_back(&msg);
	return SendImmediately(m_index, msgs);
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

	m_header.m_unrealiableCount			   = static_cast<uint8_t>(1);
	m_header.m_connectionindex			   = static_cast<uint8_t>(m_index);
	m_header.m_lastReceivedAck			   = m_lastReceivedAck;
	m_header.m_previousReceivedAckBitfield = m_previousReceivedAckBitField;
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

	m_header.m_unrealiableCount			   = static_cast<uint8_t>(actualSendCount);
	m_header.m_connectionindex			   = static_cast<uint8_t>(m_index);
	m_header.m_lastReceivedAck			   = m_lastReceivedAck;
	m_header.m_previousReceivedAckBitfield = m_previousReceivedAckBitField;
	m_packet.SetWriteHead(0);
	WriteHeader();

	size_t length = m_packet.m_bufferSize;
	size_t sendCount = m_session->m_channel->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length);
	m_lastSendTime = static_cast<float>(GetCurrentTimeSeconds());
	AddTracker(m_nextSentAck);
	IncrementSendAck();

	m_packet.Reset();
	return sendCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Appends netmsg to outbound queue
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Append(NetMessage *msg)
{
	m_outboundMsgs.push_back(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::AddTracker(uint16_t ack)
{
	PacketTracker *existingTracker = GetTracker(ack);
	if(existingTracker != nullptr)
	{
		delete existingTracker;
		existingTracker = nullptr;
	}
	PacketTracker *tracker = new PacketTracker(ack);
	m_trackerMap[ack] = tracker;
	if(ack % m_trackerMaxCount == 0 && ack != 0)
	{
		CalculateLossPercent();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Retrieves tracker
*@param   : ack id
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PacketTracker * NetConnection::GetTracker(uint16_t ack)
{
	std::map<uint16_t, PacketTracker*>::iterator it = m_trackerMap.find(ack);
	if(it == m_trackerMap.end())
	{
		return nullptr;
	}
	return it->second;
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
		m_header.m_unrealiableCount = static_cast<uint8_t>(1);
		m_header.m_connectionindex = static_cast<uint8_t>(m_index);
		m_header.m_lastReceivedAck = m_lastReceivedAck;
		m_header.m_ack			   = m_nextSentAck;
		m_header.m_previousReceivedAckBitfield = m_previousReceivedAckBitField;
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

		m_header.m_unrealiableCount = static_cast<uint8_t>(actualSendCount);
		m_header.m_connectionindex  = static_cast<uint8_t>(m_index);
		m_header.m_lastReceivedAck  = m_lastReceivedAck;
		m_header.m_ack				= m_nextSentAck;
		m_header.m_previousReceivedAckBitfield = m_previousReceivedAckBitField;
		WriteHeader();

		size_t length    = m_packet.m_bufferSize;
		size_t sendCount = m_session->m_channel->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length + 1);

		m_lastSendTime = static_cast<float>(GetCurrentTimeSeconds());
		AddTracker(m_nextSentAck);
		IncrementSendAck();
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
	size_t readSize    = m_udpSocket->ReceiveFrom(data, length,netAddress);
	if(readSize < GetHeaderSize())
	{
		return 0;
	}
	m_lastReceivedTime = static_cast<float>(GetCurrentTimeSeconds());
	UDPHeader header   = GetHeaderFromPacket(data);
	if(header.m_lastReceivedAck != INVALID_PACKET_ACK)
	{
		ConfirmPacketReceived(header.m_lastReceivedAck);
	}
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
/*DATE    : 2018/10/22
*@purpose : Increments the send ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::IncrementSendAck()
{
	m_nextSentAck++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Confirms packet ack
*@param   : Ack number
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::ConfirmPacketReceived(uint16_t ack)
{
	PacketTracker *tracker = GetTracker(ack);
	if(tracker == nullptr)
	{
		return;
	}
	float newrtt = static_cast<float>(GetCurrentTimeSeconds()) - static_cast<float>(tracker->m_sendTime);
	m_rtt = m_rtt * 0.9 * newrtt *0.1; // blend of 90 percent
	std::map<uint16_t, PacketTracker*>::iterator it = m_trackerMap.find(ack);
	delete tracker;
	tracker = nullptr;
	//m_trackerMap.erase(it);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Calculates loss percent
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::CalculateLossPercent()
{
	int totalLoss = 0;
	std::map<uint16_t, PacketTracker*>::iterator it = m_trackerMap.begin();
	for(;it != m_trackerMap.end();it++)
	{
		if(it->second != nullptr)
		{
			totalLoss++;
		}
	}
	m_loss = static_cast<float>(totalLoss) / static_cast<float>(m_trackerMaxCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Returns the header from total packet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UDPHeader NetConnection::GetHeaderFromPacket(void *data)
{
	//uint8_t m_connectionindex = static_cast<uint8_t>(-1);
	//uint8_t m_unrealiableCount = static_cast<uint8_t>(0U);
	//
	//uint16_t m_ack;
	//uint16_t m_lastReceivedAck;
	//uint16_t m_previousReceivedAckBitfield;
	BytePacker packer;
	packer.WriteBytes(GetHeaderSize(), data);
	UDPHeader header;
	header.m_connectionindex = packer.ReadBytes((void*)&header.m_connectionindex, 1);
	header.m_connectionindex = packer.ReadBytes((void*)&header.m_unrealiableCount, 1);
	header.m_connectionindex = packer.ReadBytes((void*)&header.m_ack, 2);
	header.m_connectionindex = packer.ReadBytes((void*)&header.m_lastReceivedAck, 2);
	header.m_connectionindex = packer.ReadBytes((void*)&header.m_previousReceivedAckBitfield, 2);
	return header;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/19
*@purpose : retrives the udp packet header size
*@param   : NIL
*@return  : header size
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NetConnection::GetHeaderSize()
{
	int size = static_cast<int>(sizeof(UDPHeader));
	return 8;
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
	m_packet.m_currentWritePosition = 6;
	m_packet.WriteBytes(1, (char*)&m_header.m_unrealiableCount);
	m_packet.m_currentWritePosition = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Writes next sent ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WriteNextSentAck()
{
	size_t temp = m_packet.m_currentWritePosition;
	m_packet.m_currentWritePosition = 1;
	m_packet.WriteBytes(2, (char*)&m_header.m_ack);
	m_packet.m_currentWritePosition = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Writes last recevied ack to header
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WriteLastReceivedAck()
{
	size_t temp = m_packet.m_currentWritePosition;
	m_packet.m_currentWritePosition = 3;
	m_packet.WriteBytes(2, (char*)&m_header.m_lastReceivedAck);
	m_packet.m_currentWritePosition = temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : Writes previous recvd ack in it field in header
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WritePreviousReceviedAckBitField()
{
	size_t temp = m_packet.m_currentWritePosition;
	m_packet.m_currentWritePosition = 5;
	m_packet.WriteBytes(2, (char*)&m_header.m_previousReceivedAckBitfield);
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
	WriteConnectionIndex();
	WriteUnrealiableMsgCount();
	WriteNextSentAck();
	WriteLastReceivedAck();
	WritePreviousReceviedAckBitField();
	m_packet.m_currentWritePosition = GetHeaderSize() - 1;
	std::string header = m_packet.GetBitString();
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
	//SendImmediately(*msg);
	Append(msg);
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
