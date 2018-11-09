#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Net/UDP/PacketTracker.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Net/Packet.hpp"
// CONSTRUCTOR

NetConnection::NetConnection(int index, NetAddress netAddress)
{
	m_startTime = Clock::GetMasterClock()->total.m_seconds;
	m_index     = static_cast<uint8_t>(index);
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
	UNUSED(connectionIndex);
	if(msgs.size() <= 0)
	{
		return 0;
	}
	NetAddress *address = msgs.at(0)->m_address;
	//int msgCount = msgs.size();
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
		m_unsentUnreliableMsgs.push_back(msgs.at(index));
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
	if(true)
	{
		m_unsentReliableMsgs.push_back(msg);
		return;
	}
	m_unsentUnreliableMsgs.push_back(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PacketTracker * NetConnection::AddTracker(uint16_t ack)
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
	return tracker;
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
	if(m_session->m_channel == nullptr)
	{
		return 0;
	}
	float min = GetMinOf2(m_sendRate, m_session->m_sendRate);
	if (m_lastSendTime + 1 / min > Clock::GetMasterClock()->total.m_seconds)
	{
		size_t sendCount = 0;
		sendCount += FlushUnConfirmedReliables();
		if(sendCount > 0)
		{
			return sendCount;
		}
		sendCount += FlushUnSentRealiables();
		if (sendCount > 0)
		{
			return sendCount;
		}
		sendCount += FlushUnrealiables();
		return sendCount;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Flushes unconfirmed realiable msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::FlushUnConfirmedReliables()
{
	size_t sendCount = 0;
	for (size_t unconfirmedMsgIndex = 0; unconfirmedMsgIndex < m_unconfirmedReliableMessages.size(); unconfirmedMsgIndex++)
	{
		NetMessage *msg = m_unconfirmedReliableMessages.at(unconfirmedMsgIndex);
		Packet packet;
		//PacketTracker *tracker = GetTracker(m_nextSentAck);
		packet.m_address = msg->m_address;
		packet.SetConnectionIndex			 (m_index);
		packet.SetNextSentAck				 (m_nextSentAck);
		packet.SetLastReceivedAck			 (m_lastReceivedAck);
		packet.SetPreviousReceviedAckBitField(m_previousReceivedAckBitField);
		packet.SetUnrealiableMsgCount(1);

		packet.WritePacketHeader();

		if (ShouldSentUnconfirmedReliableMsg(msg))
		{
			if (packet.WriteMsg(msg))
			{
				sendCount += m_session->SendPacket(packet);
				DevConsole::GetInstance()->PushToOutputText("SENDING UNCONFIRMED RELIABLE " + ToString(msg->m_count));
				msg->ResetAge();
				IncrementSendAck();
				m_lastSendTime = Clock::GetMasterClock()->total.m_seconds - m_startTime;
				return sendCount;
			}
		}
	}
	return sendCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Flushen unsent reliable msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::FlushUnSentRealiables()
{
	size_t sendCount = 0;
	for (size_t unsentReliableIndex = 0; unsentReliableIndex < m_unsentReliableMsgs.size(); unsentReliableIndex++)
	{
		NetMessage *msg = m_unsentReliableMsgs.at(unsentReliableIndex);
		Packet packet;
		packet.m_address = msg->m_address;
		packet.SetConnectionIndex			 (m_index);
		packet.SetNextSentAck				 (m_nextSentAck);
		packet.SetLastReceivedAck			 (m_lastReceivedAck);
		packet.SetPreviousReceviedAckBitField(m_previousReceivedAckBitField);
		packet.SetUnrealiableMsgCount(1);

		packet.WritePacketHeader();

		if (CanSendNewReliableMessage())
		{
			msg->m_reliableID = GetNextRealiableIDToSend();
			if (packet.WriteMsg(msg))
			{
				sendCount += m_session->SendPacket(packet);
				PacketTracker *tracker = AddTracker(m_nextSentAck);
				tracker->m_reliableID = m_highestRealiableID;
				msg->ResetAge();
				m_unsentReliableMsgs.erase(m_unsentReliableMsgs.begin() + unsentReliableIndex, m_unsentReliableMsgs.begin() + unsentReliableIndex + 1);
				m_unconfirmedReliableMessages.push_back(msg);
				unsentReliableIndex--;
				m_lastSendTime = Clock::GetMasterClock()->total.m_seconds - m_startTime;
				DevConsole::GetInstance()->PushToOutputText("SENDING UNSENT RELIABLE " + ToString(msg->m_count));
				IncrementSendAck();
				IncrementRealiableID();
				return sendCount;
			}
		}
	}
	return sendCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Flushes unreliable msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::FlushUnrealiables()
{
	size_t sendCount = 0;
	for (size_t unsentUnreliableIndex = 0; unsentUnreliableIndex < m_unsentUnreliableMsgs.size(); unsentUnreliableIndex++)
	{
		NetMessage *msg = m_unsentUnreliableMsgs.at(unsentUnreliableIndex);
		Packet packet;
		packet.m_address = msg->m_address;
		
		packet.SetConnectionIndex			 (m_index);
		packet.SetNextSentAck				 (m_nextSentAck);
		packet.SetLastReceivedAck			 (m_lastReceivedAck);
		packet.SetPreviousReceviedAckBitField(m_previousReceivedAckBitField);
		packet.SetUnrealiableMsgCount(1);
		packet.WritePacketHeader();

		if (packet.WriteMsg(msg))
		{
			sendCount += m_session->SendPacket(packet);
			PacketTracker *tracker = AddTracker(m_nextSentAck);
			IncrementSendAck();
			m_lastSendTime = Clock::GetMasterClock()->total.m_seconds - m_startTime;
		}
		delete m_unsentUnreliableMsgs.at(unsentUnreliableIndex);
		m_unsentUnreliableMsgs.erase(m_unsentUnreliableMsgs.begin() + unsentUnreliableIndex, m_unsentUnreliableMsgs.begin() + unsentUnreliableIndex + 1);
		unsentUnreliableIndex--;
		return sendCount;
	}
	return sendCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : Recv msg from UDP
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::Recv(char *data,size_t &length,NetAddress *netAddress)
{
	size_t readSize    = m_udpSocket->ReceiveFrom(data, length,netAddress);
	if(GetRandomFloatZeroToOne() < m_session->m_lossAmount)
	{
		return 0;
	}
	if(readSize < GetHeaderSize())
	{
		return 0;
	}
	/*NetConnection *connection = m_session->GetConnection(netAddress);
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
	}*/
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
/*DATE    : 2018/11/07
*@purpose : Increments realiable ids
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::IncrementRealiableID()
{
	m_highestRealiableID++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Retrives the unconfirmed reliable ID
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetConnection::GetUnconfirmedReliableMsg(uint16_t reliableID)
{
	for (size_t index = 0; index < m_unconfirmedReliableMessages.size(); index++)
	{
		if(m_unconfirmedReliableMessages.at(index)->m_reliableID == reliableID)
		{
			return m_unconfirmedReliableMessages.at(index);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : removes uncnofirmed reliable msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::RemoveUnconfirmedReliableMsg(NetMessage *msg)
{
	for (size_t index = 0; index < m_unconfirmedReliableMessages.size(); index++)
	{
		if (m_unconfirmedReliableMessages.at(index) == msg)
		{
			m_unconfirmedReliableMessages.erase(m_unconfirmedReliableMessages.begin() + index, m_unconfirmedReliableMessages.begin() + index + 1);
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Delets msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::DestroyNetMessage(NetMessage *msg)
{
	delete msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::ConfirmReceivedReliableID(uint16_t reliableID)
{
	m_reliableIDs.push_back(reliableID);
	m_highestRealiableID = GetMax(reliableID, m_highestRealiableID);
	uint16_t minimumReliableID = m_highestRealiableID - RELIABLE_WINDOW + 1;
	for (size_t reliableIDIndex = 0; reliableIDIndex < m_reliableIDs.size(); reliableIDIndex++)
	{
		if(CycleLess(m_reliableIDs.at(reliableIDIndex),minimumReliableID))
		{
			m_reliableIDs.erase(m_reliableIDs.begin() + reliableIDIndex, m_reliableIDs.begin() + reliableIDIndex + 1);
			reliableIDIndex--;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Has already received relible ID
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::HasReceivedReliableID(uint16_t reliableID)
{
	uint16_t minimumID = m_highestRealiableID - RELIABLE_WINDOW + 1;
	if(CycleLess(reliableID,minimumID))
	{
		return true;
	}
	for (size_t reliableIDIndex = 0; reliableIDIndex < m_reliableIDs.size(); reliableIDIndex++)
	{
		if(m_reliableIDs.at(reliableIDIndex) == reliableID)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetConnection::GetNextRealiableIDToSend()
{
	return m_highestRealiableID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetConnection::GetOldestUnconfirmedRealiableID()
{
	if(m_reliableIDs.size() == 0)
	{
		return 0;
	}
	return m_reliableIDs.at(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Returns the next packet ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetConnection::GetNextPacketAck()
{
	return m_nextSentAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Checks if the reliable msg should be sent (time out and last ack response)
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::ShouldSentUnconfirmedReliableMsg(NetMessage *msg)
{
	if(msg->m_lastSentTime + 0.1 < static_cast<float>(GetCurrentTimeSeconds()))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::ConfirmSentRealiable(uint16_t relID)
{
	UNUSED(relID);

	// go through unconfirmer list and delete it
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::CanSendNewReliableMessage()
{
	uint16_t nextReliableID        = GetNextRealiableIDToSend();
	uint16_t oldestConfirmedRelID  = GetOldestUnconfirmedRealiableID();
	uint16_t diff = nextReliableID - oldestConfirmedRelID;
	if(diff > RELIABLE_WINDOW)
	{
		return false;
	}
	return true;
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

	for(unsigned int reliableIndexInPacket = 0;reliableIndexInPacket < tracker->m_reliablesInPacket;reliableIndexInPacket++)
	{
		NetMessage *msg = GetUnconfirmedReliableMsg(tracker->m_sentReliableIDs[reliableIndexInPacket]);
		if(msg != nullptr)
		{
			RemoveUnconfirmedReliableMsg(msg);
			DestroyNetMessage(msg);
		}
	}

	double newrtt = Clock::GetMasterClock()->total.m_seconds - (tracker->m_sendTime);
	m_rtt = static_cast<float>(m_rtt * 0.9 + newrtt * 0.1); // blend of 90 percent
	delete it->second;
	it->second = nullptr;

	//DebugDraw::GetInstance()->DebugRenderLogf(2,"CONFIRMING PACKET %d", static_cast<int>(ack));
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
	UNUSED(netAddress);
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
PacketHeader NetConnection::GetHeaderFromPacket(void *data)
{
	//uint8_t m_connectionindex = static_cast<uint8_t>(-1);
	//uint8_t m_unrealiableCount = static_cast<uint8_t>(0U);
	//
	//uint16_t m_ack;
	//uint16_t m_lastReceivedAck;
	//uint16_t m_previousReceivedAckBitfield;
	BytePacker packer;
	packer.WriteBytes(GetHeaderSize(), data);
	PacketHeader header;
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
	//int startPosition = GetHeaderSize();
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
//	int size = static_cast<int>(sizeof(PacketHeader));
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
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::WriteReliableID()
{

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
	WriteReliableID();
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
	double lastHearBeatHPC = m_lastHeartbeatTime;
	double heartBeatFreq   = m_heartBeatFrequency;

	if (lastHearBeatHPC + 1 / heartBeatFreq < Clock::GetMasterClock()->total.m_seconds)
	{
		NetMessage *msg = NetMessage::CreateHeartBeatMessage(address);
		Append(msg);
		m_lastHeartbeatTime = Clock::GetMasterClock()->total.m_seconds;
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
	UNUSED(deltaTime);
	FlushMsgs();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PacketHeader::operator=(const PacketHeader &copy)
{
	m_connectionindex = copy.m_connectionindex;
	m_ack = copy.m_ack;
	m_lastReceivedAck = copy.m_lastReceivedAck;
	m_previousReceivedAckBitfield = copy.m_previousReceivedAckBitfield;
	m_unrealiableCount = copy.m_unrealiableCount;
}
