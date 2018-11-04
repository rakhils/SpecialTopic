#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Net/UDP/PacketTracker.hpp"
#include "Engine/Debug/DebugDraw.hpp"
// CONSTRUCTOR

NetConnection::NetConnection(int index, NetAddress netAddress)
{
	m_startTime = Clock::GetMasterClock()->total.m_seconds;
	m_index     = index;
	m_address   = netAddress;
	m_udpSocket = new UDPSocket(netAddress);
	m_udpSocket->SetBlocking(false);
	InitTracker();
}

NetConnection::NetConnection(int listenPort)
{
	m_startTime = Clock::GetMasterClock()->total.m_seconds;
	m_udpSocket = new UDPSocket(listenPort);
	m_address.m_port = listenPort;
	m_address.m_address = (m_udpSocket->m_address.m_address);
	m_index = 0;
	InitTracker();
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
void NetConnection::InitTracker()
{
	for(uint16_t index = 0;index < m_trackerMaxCount;index++)
	{
		//m_trackerMap[index] = nullptr;
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
	NetAddress *address = msgs.at(0)->m_address;
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
		if(WritePayload(msgs.at(index),address))
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
	m_lastSendTime = Clock::GetMasterClock()->total.m_seconds - m_startTime;
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
	CalculateLoss();

	PacketTracker *existingTracker = GetTracker(ack);
	if(existingTracker != nullptr)
	{
		delete existingTracker;
		existingTracker = nullptr;
	}
	PacketTracker *tracker = new PacketTracker(ack);
	m_trackerMap[ack] = tracker;
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
	m_index = 255;
	float min = GetMinOf2(m_sendRate, m_session->m_sendRate);
	if (m_lastSendTime + 1 / min > Clock::GetMasterClock()->total.m_seconds)
	{
		if (m_outboundMsgs.size() <= 0)
		{
			return 0;
		}
		NetAddress *address       = m_outboundMsgs.at(0)->m_address;
		NetConnection *connection = m_session->GetConnection(address);
		if(connection == nullptr)
		{
			return 0;
		}

		int actualSendCount = 0;
		m_header.m_unrealiableCount = static_cast<uint8_t>(1);
		m_header.m_connectionindex = static_cast<uint8_t>(m_index);
		m_header.m_lastReceivedAck = connection->m_lastReceivedAck;
		m_header.m_ack			   = connection->m_nextSentAck;
		m_header.m_previousReceivedAckBitfield = connection->m_previousReceivedAckBitField;
		WriteHeader();
		if(address->m_port == 10085)
		{
			int a = 1;
		}
		if (address->m_port == 10084)
		{
			int a = 1;
		}
		for (int index = 0; index < m_outboundMsgs.size(); index++)
		{
			if (WritePayload(m_outboundMsgs.at(index),address))
			{
				delete m_outboundMsgs.at(index);
				actualSendCount++;
				m_outboundMsgs.erase(m_outboundMsgs.begin() + index, m_outboundMsgs.begin() + index + 1);
				continue;
			}
		}

		m_header.m_unrealiableCount = static_cast<uint8_t>(actualSendCount);
		m_header.m_connectionindex  = static_cast<uint8_t>(m_index);
		m_header.m_lastReceivedAck  = connection->m_lastReceivedAck;
		m_header.m_ack				= connection->m_nextSentAck;
		m_header.m_previousReceivedAckBitfield = connection->m_previousReceivedAckBitField;
		WriteHeader();
		DebugDraw::GetInstance()->DebugRenderLogf(2,"SENDING PACKET %d ", static_cast<int>(m_nextSentAck));
		std::string pack = m_packet.GetBitString();
		size_t length    = m_packet.m_bufferSize;
		connection->AddTracker(m_nextSentAck);
		size_t sendCount = m_udpSocket->SendTo(*address, (char *)m_packet.m_buffer, length);

		connection->m_lastSendTime = Clock::GetMasterClock()->total.m_seconds - m_startTime;
		//IncrementSendAck();
		connection->m_nextSentAck++;
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
	m_trackerMap;
	if(GetRandomFloatZeroToOne() < m_session->m_lossAmount)
	{
		return 0;
	}
	if(readSize < GetHeaderSize())
	{
		return 0;
	}
	
	NetConnection *connection = m_session->GetConnection(netAddress);
	if(connection == nullptr)
	{
		return 0;
	}
	connection->m_lastReceivedTime = Clock::GetMasterClock()->total.m_seconds - connection->m_startTime;
	UDPHeader header   = GetHeaderFromPacket(data);
	connection->m_lastReceivedAck = header.m_ack;
	

	if (header.m_lastReceivedAck != INVALID_PACKET_ACK)
	{
		connection->UpdateAckStatus(netAddress, header.m_lastReceivedAck);
		connection->ConfirmPacketReceived(header.m_lastReceivedAck);
	}
	return readSize;
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
	std::map<uint16_t, PacketTracker*>::iterator it = m_trackerMap.find(ack);
	if (it == m_trackerMap.end())
	{
		return;
	}
	PacketTracker *tracker = it->second;
	if(tracker == nullptr)
	{
		return;
	}
	float newrtt = Clock::GetMasterClock()->total.m_seconds - (tracker->m_sendTime);
	m_rtt = m_rtt * 0.9 + newrtt * 0.1; // blend of 90 percent
	delete it->second;
	it->second = nullptr;
	DebugDraw::GetInstance()->DebugRenderLogf(2,"CONFIRMING PACKET %d", static_cast<int>(ack));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/03
*@purpose : Calculates loss
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::CalculateLoss()
{
	if (m_trackerMap.size() >= m_trackerMaxCount)
	{
		std::map<uint16_t, PacketTracker*>::iterator it = m_trackerMap.begin();
		int numOfAcksNotReceived = 0;
		for (; it != m_trackerMap.end(); it++)
		{
			if (it->second != nullptr && it->second->m_valid)
			{
				numOfAcksNotReceived++;
			}
		}
		std::map<uint16_t, PacketTracker*>::reverse_iterator iter = m_trackerMap.rbegin();
		for (iter = m_trackerMap.rbegin(); iter != m_trackerMap.rend(); iter++)
		{
			if (iter->second != nullptr)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}
		m_trackerMap.erase(m_trackerMap.begin(), m_trackerMap.end());
		m_loss = static_cast<float>(numOfAcksNotReceived) / static_cast<float>(m_trackerMaxCount);
		DebugDraw::GetInstance()->DebugRenderLogf(2, "CLEARING LOGS");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/28
*@purpose : Pushes into prev ack bit field
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::PushToAckBitField(NetAddress *netAddress,uint16_t ack)
{
	if(ack >= m_AckBitfieldMinPosition && ack <= m_AckBitfieldMaxPosition)
	{
		uint16_t diff = ack - m_AckBitfieldMinPosition;
		SetBits(m_previousReceivedAckBitField, 1<<diff);
	}
	if(ack > m_AckBitfieldMaxPosition)
	{
		uint16_t diff = ack - m_AckBitfieldMaxPosition;
		RightShift(m_previousReceivedAckBitField, diff);
		m_AckBitfieldMinPosition += diff;
		m_AckBitfieldMaxPosition += diff;
		diff = ack - m_AckBitfieldMinPosition;
		SetBits(m_previousReceivedAckBitField, 1<<diff);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/29
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::UpdateAckStatus(NetAddress *netAddress,uint16_t ackReceived)
{
	PushToAckBitField(netAddress,ackReceived);
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
	packer.ReadBytes((void*)&header.m_connectionindex, 1);
	packer.ReadBytes((void*)&header.m_ack, 2);
	packer.ReadBytes((void*)&header.m_lastReceivedAck, 2);
	packer.ReadBytes((void*)&header.m_previousReceivedAckBitfield, 2);
	packer.ReadBytes((void*)&header.m_unrealiableCount, 1);
	std::string packetStr = packer.GetBitString();
	return header;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/28
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int NetConnection::GetCommandIndex(void *data,int size)
{
	int startPosition = GetHeaderSize();
	BytePacker packer;
	packer.WriteBytes(size, data);
	int cmdIndex = -1;
	packer.ReadBytes(&cmdIndex, 1);
	return cmdIndex;
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
	m_packet.m_currentWritePosition = 7;
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
	WriteNextSentAck();
	WriteLastReceivedAck();
	WritePreviousReceviedAckBitField();
	WriteUnrealiableMsgCount();
	m_packet.m_currentWritePosition = GetHeaderSize();
	std::string header = m_packet.GetBitString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::WritePayload(NetMessage *msg,NetAddress *address)
{
	if(!(*msg->m_address == *address))
	{
		return false;
	}

	if(m_packet.m_bufferSize + msg->m_bufferSize + GetHeaderSize() > ETHERNET_MTU)
	{
		return false;
	}
	if(msg->m_address == address)
	{
		m_packet.WriteBytes(msg->m_bufferSize, msg->m_buffer);
	}
	else
	{
		int a = 1;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Sends and flushes  a heartbeat
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SendHeartBeat(NetAddress *address)
{
	if(m_session->GetConnection(address) == nullptr)
	{
		return;
	}

	float lastHearBeatHPC = m_session->GetConnection(address)->m_lastHeartbeatHPC;
	float heartBeatFreq   = m_session->GetConnection(address)->m_heartBeatFrequency;

	if (lastHearBeatHPC + 1 / heartBeatFreq < Clock::GetMasterClock()->total.m_seconds)
	{
		NetMessage *msg = NetMessage::CreateHeartBeatMessage(address);
		Append(msg);
		m_session->GetConnection(address)->m_lastHeartbeatHPC = Clock::GetMasterClock()->total.m_seconds;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Updates 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Update(float deltaTime)
{
	FlushMsgs();
}
