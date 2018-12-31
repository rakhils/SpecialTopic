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
#include "Engine/Net/NetObjectConnectionView.hpp"
#include "Engine/Net/NetObjectView.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR

NetConnection::NetConnection(int index, NetAddress netAddress)
{
	m_startTime = Clock::GetMasterClock()->total.m_seconds;
	m_index     = static_cast<uint8_t>(index);
	m_address   = netAddress;
	//m_udpSocket = new UDPSocket(netAddress);
	m_lastReceivedTime = static_cast<float>(GetCurrentTimeSeconds());
	m_lastSendTime     = static_cast<float>(GetCurrentTimeSeconds());
	m_netObjectConnectionView = new NetObjectConnectionView();
	InitTracker();
}

NetConnection::NetConnection(int listenPort)
{
	m_startTime				= Clock::GetMasterClock()->total.m_seconds;
	m_udpSocket				= new UDPSocket(listenPort);
	m_address.m_port		= listenPort;
	m_index = 0;
	m_lastReceivedTime = static_cast<float>(GetCurrentTimeSeconds());
	m_lastSendTime = static_cast<float>(GetCurrentTimeSeconds());

	m_netObjectConnectionView = new NetObjectConnectionView();
	InitTracker();
	m_connectionState = CONNECTION_READY;
}

NetConnection::NetConnection()
{
	m_lastReceivedTime = static_cast<float>(GetCurrentTimeSeconds());
	m_lastSendTime = static_cast<float>(GetCurrentTimeSeconds());

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
	/*for(uint16_t index = 0;index < m_trackerMaxCount;index++)
	{
		//m_trackerMap[index] = nullptr;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::BindConnection()
{
	m_udpSocket = new UDPSocket(m_address);
	m_udpSocket->SetBlocking(false);
	m_connectionState = CONNECTION_BOUND;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/30
*@purpose : Disconnects the connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Disconnect()
{
	if(this == NetSession::GetInstance()->m_myConnection && IsHost())
	{
		std::map<int, NetConnection*>::iterator it;
		for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end();it++)
		{
			NetMessage *msg = NetMessage::CreateHangUpMsg(this);
			msg->m_address = &it->second->m_address;
			it->second->Append(msg);
			it->second->FlushUnrealiables();
		}
		NetSession::GetInstance()->m_boundConnections.clear();
		m_udpSocket->Close();
		m_connectionState = CONNECTION_DISCONNECTED;
		return;
	}
	m_udpSocket->Close();
	m_connectionState = CONNECTION_DISCONNECTED;
	NetMessage *msg = NetMessage::CreateHangUpMsg(this);
	Append(msg);
	FlushUnrealiables();
	//Disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::CreateNetObjectConectionView()
{
	m_netObjectConnectionView = new NetObjectConnectionView();
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
	size_t sendCount = m_session->m_hostConnection->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length);
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
	if(msg->m_isReliable)
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
	PushNetMsgFromNetObjectViews();
	if(m_session->m_hostConnection == nullptr)
	{
		return 0;
	}
	float min = GetMinOf2(m_sendRate, m_session->m_sendRate);
	if (Clock::GetMasterClock()->total.m_seconds > (m_lastSendTime + 1/min))
	{
		size_t sendCount = 0;
		sendCount += FlushUnConfirmedReliables();
		sendCount += FlushUnSentRealiables();
		sendCount += FlushUnrealiables();
		if(sendCount > 0)
		{
			m_lastSendTime = Clock::GetMasterClock()->total.m_seconds;
		}
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

		if (CanSentUnconfirmedReliableMsg(msg))
		{
			if (packet.WriteMsg(msg))
			{
				sendCount += m_session->SendPacket(packet);
				msg->ResetAge();
				//PacketTracker *tracker = msg->m_tracker;
				PacketTracker *newTracker = AddTracker(m_nextSentAck);
				//DevConsole::GetInstance()->PushToOutputText("SENDING UNCONFIRMED RELIABLE " + ToString(msg->m_count) + " TRACKER "+ToString(static_cast<int>(m_nextSentAck)));
				//DevConsole::GetInstance()->PushToOutputText("SENDING UNCONFIRMED OLD " + ToString(msg->m_count) + " TRACKER " +
					//ToString(static_cast<int>(tracker->m_ackID)));
				
				newTracker->AddReliables(msg->m_reliableID);
				msg->m_tracker = newTracker;
				IncrementSendAck();
				//break;
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

		if (CanSendNewReliableMessage(msg->m_reliableID))
		{
			//msg->m_reliableID = GetNextRealiableIDToSend();
			if (packet.WriteMsg(msg))
			{
				sendCount += m_session->SendPacket(packet);
				PacketTracker *tracker = AddTracker(m_nextSentAck);
				msg->m_tracker = tracker;
				tracker->AddReliables(msg->m_reliableID);
				msg->ResetAge();
				m_unsentReliableMsgs.erase(m_unsentReliableMsgs.begin() + unsentReliableIndex, m_unsentReliableMsgs.begin() + unsentReliableIndex + 1);
				m_unconfirmedReliableMessages.push_back(msg);
				unsentReliableIndex--;
				//DevConsole::GetInstance()->PushToOutputText("SENDING UNSENT RELIABLE " + ToString(msg->m_reliableID) + " TRACKER "+ToString(static_cast<int>(m_nextSentAck)));
				std::string bit = msg->GetBitString();
				IncrementSendAck();
				//IncrementRealiableID();
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
			if(msg->m_definitionIndex != NETMSG_BLANK)
			{
				AddTracker(m_nextSentAck);
				IncrementSendAck();
			}
		}
		delete m_unsentUnreliableMsgs.at(unsentUnreliableIndex);
		m_unsentUnreliableMsgs.erase(m_unsentUnreliableMsgs.begin() + unsentUnreliableIndex, m_unsentUnreliableMsgs.begin() + unsentUnreliableIndex + 1);
		unsentUnreliableIndex--;
		break;
		//return sendCount;
	}
	if(m_unsentUnreliableMsgs.size() > 0)
	{
		FlushUnrealiables();
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
/*DATE    : 2018/11/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::PushToInboundMsgQueue(NetMessage* msg)
{
	if(msg->m_reliableID <= m_oldestOrderedReliableMsgID)
	{
		return;
	}
	m_inboundMsgQueue.push_back(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::DoProcessInboundMsgQueue()
{
	for(int index = 0;index < m_inboundMsgQueue.size();index++)
	{
		for (int index1 = 0; index1 < m_inboundMsgQueue.size(); index1++)
		{
			if((m_oldestOrderedReliableMsgID + 1) == m_inboundMsgQueue.at(index1)->GetReliableID())
			{
				NetMessage *msg = m_inboundMsgQueue.at(index1);
				//DevConsole::GetInstance()->PushToOutputText("CONFIRMING MSG REL ID " + ToString(m_oldestOrderedReliableMsgID + 1)+" TOTAL "+ToString(m_inboundMsgQueue.size()),Rgba::RED);
				//DevConsole::GetInstance()->PushToOutputText("MSG REL ID "+msg->m_definitionName, Rgba::RED);

				NetMessageDefinition * msgdef = NetSession::GetInstance()->GetMsgDefinition(msg->m_definitionName);
				(*msgdef->m_callback)(*msg, *msg->m_address);
				m_oldestOrderedReliableMsgID++;

				m_inboundMsgQueue.erase(m_inboundMsgQueue.begin() + index1, m_inboundMsgQueue.begin() + index1 + 1);
				index1--;
				index--;

				//DevConsole::GetInstance()->PushToOutputText(" INBOUND TOTAL SIZE " + ToString(m_inboundMsgQueue.size()), Rgba::RED);
			}
		}
	}
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
/*DATE    : 2018/11/23
*@purpose : Sets state and sends accroos all connection
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetState(EConnectionState state)
{
	UNUSED(state);
	if(NetSession::GetInstance()->m_hostConnection == this && m_connectionState == CONNECTION_CONNECTED)
	{
		std::map<int, NetConnection*>::iterator it = m_session->m_boundConnections.begin();
		for(;it != m_session->m_boundConnections.end();it++)
		{
			if(it->first == 0)
			{
				continue;
			}
			//NetMessage *msg = NetMessage::CreateUpdateConnState(it->second,state);
			//it->second->Append(msg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/30
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SetLastRecvTime(float time)
{
	m_lastReceivedTime = time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetConnection::IsHost()
{
	if(m_session->m_myConnection != nullptr && m_session->m_hostConnection != nullptr)
	{
		if(m_session->m_myConnection == m_session->m_hostConnection)
		{
			return true;
		}
	}
	return false;
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
void NetConnection::IncrementSentRealiableID()
{
	m_highestSentRealiableID++;
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
	bool ret = false;
	for (size_t index = 0; index < m_unconfirmedReliableMessages.size(); index++)
	{
		if (m_unconfirmedReliableMessages.at(index) == msg)
		{
			m_unconfirmedReliableMessages.erase(m_unconfirmedReliableMessages.begin() + index, m_unconfirmedReliableMessages.begin() + index + 1);
			m_sentReliableIDs.push_back(msg->m_reliableID);
			//DevConsole::GetInstance()->PushToOutputText("PUSHIN REL ID " + ToString(static_cast<int>(msg->m_reliableID)), Rgba::YELLOW);

			ret = true;
			break;
		}
	}

	if ((m_oldestUnconfirmedReliableID + 1) == msg->m_reliableID)
	{
		m_oldestUnconfirmedReliableID++;
		//DevConsole::GetInstance()->PushToOutputText("CONFIRMING MSG OUT " + ToString(static_cast<int>(m_oldestUnconfirmedReliableID)),Rgba::GREEN);
		for (int index = 0; index < m_sentReliableIDs.size(); index++)
		{
			for (int index1 = 0; index1 < m_sentReliableIDs.size(); index1++)
			{
				if ((m_oldestUnconfirmedReliableID + 1)== m_sentReliableIDs.at(index1))
				{
					m_oldestUnconfirmedReliableID++;
					//DevConsole::GetInstance()->PushToOutputText("CONFIRMING MSG IN " + ToString(static_cast<int>(m_oldestUnconfirmedReliableID)),Rgba::GREEN);
					break;
				}
			}
		}
	}
	for (int index1 = 0; index1 < m_sentReliableIDs.size(); index1++)
	{
		if (m_sentReliableIDs.at(index1) <= m_oldestUnconfirmedReliableID)
		{
			m_sentReliableIDs.erase(m_sentReliableIDs.begin() + index1, m_sentReliableIDs.begin() + index1 + 1);
			index1--;
		}
	}


	return ret;
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
void NetConnection::AddReceivedRealiableID(uint16_t reliableID)
{
	m_reliableIDs.push_back(reliableID);

	m_highestRecvRealiableID = GetMax(reliableID, m_highestRecvRealiableID);
	uint16_t minimumReliableID = m_highestRecvRealiableID - RELIABLE_WINDOW + 1;
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
	m_highestRecvRealiableID = GetMax(m_highestRecvRealiableID, reliableID);
	uint16_t minimumID = m_highestRecvRealiableID - RELIABLE_WINDOW + 1;
	for (size_t reliableIDIndex = 0; reliableIDIndex < m_reliableIDs.size(); reliableIDIndex++)
	{
		if (CycleLess(m_reliableIDs.at(reliableIDIndex), minimumID))
		{
			m_reliableIDs.erase(m_reliableIDs.begin() + reliableIDIndex, m_reliableIDs.begin() + reliableIDIndex + 1);
			reliableIDIndex--;
		}
	}
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
	m_reliableIDs.push_back(reliableID);

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetConnection::GetNextSentReliableID()
{
	return m_highestSentRealiableID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetConnection::GetOldestUnConfirmedRealiableID()
{
	return m_oldestUnconfirmedReliableID;
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
bool NetConnection::CanSentUnconfirmedReliableMsg(NetMessage *msg)
{
	if(msg->m_lastSentTime + 0.01 < static_cast<float>(GetCurrentTimeSeconds()))
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
bool NetConnection::CanSendNewReliableMessage(uint16_t nextReliableID)
{
	//uint16_t nextReliableID          = GetNextRealiableIDToSend();
	uint16_t oldestUnConfirmedRelID  = GetOldestUnConfirmedRealiableID();
	uint16_t diff = nextReliableID - oldestUnConfirmedRelID;
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
	if (tracker == nullptr)
	{
		return;
	}
	

	//DevConsole::GetInstance()->PushToOutputText("TRACKER RECV " + ToString(static_cast<int>(tracker->m_sentReliableIDs[0])));
	for(unsigned int reliableIndexInPacket = 0;reliableIndexInPacket < tracker->m_reliablesInPacket;reliableIndexInPacket++)
	{
		//DevConsole::GetInstance()->PushToOutputText("REL IDX " + ToString(static_cast<int>(tracker->m_sentReliableIDs[reliableIndexInPacket])), Rgba::YELLOW);
		NetMessage *msg = GetUnconfirmedReliableMsg(tracker->m_sentReliableIDs[reliableIndexInPacket]);
		if(msg != nullptr)
		{
			if (msg->m_definitionName == "join_accept")
			{
				int	a = 1;
			}
			//DevConsole::GetInstance()->PushToOutputText("MSG CONFIRMED " + ToString(static_cast<int>(tracker->m_sentReliableIDs[reliableIndexInPacket]))+
				//ToString(static_cast<int>(msg->m_reliableID)), Rgba::YELLOW);

			//DevConsole::GetInstance()->PushToOutputText("MSG CONFIRMED " + msg->m_definitionName +ToString(static_cast<int>(msg->m_reliableID)), Rgba::YELLOW);
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
		//DebugDraw::GetInstance()->DebugRenderLogf(2, "CLEARING LOGS");
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
	if(true)
	{
		return;
	}
	double lastHearBeatHPC = m_lastHeartbeatTime;
	double heartBeatFreq   = m_heartBeatFrequency;

	if (Clock::GetMasterClock()->total.m_seconds > lastHearBeatHPC + (1 / heartBeatFreq))
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
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::SortAndPushIntoNetObjectViews(NetObjectView *netObjectView)
{
	if(m_netObjectViews.size() == 0)
	{
		m_netObjectViews.push_back(netObjectView);
		return;
	}
	bool insertSuccess = false;
	for(int index = 0;index < m_netObjectViews.size();index++)
	{
		if(m_netObjectViews.at(index)->m_age < netObjectView->m_age)
		{
			insertSuccess = true;
			m_netObjectViews.insert(m_netObjectViews.begin()+index,netObjectView);
		}
	}
	if(!insertSuccess)
	{
		m_netObjectViews.push_back(netObjectView);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::PushNetMsgFromNetObjectViews()
{
	for(int index = 0;index < m_netObjectViews.size();index++)
	{
		NetMessage *msg = m_netObjectViews.at(index)->m_netMsg;
		Append(msg);
	}
	m_netObjectViews.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::SendDirect(NetAddress *address, void *data, size_t size)
{
	return m_udpSocket->SendTo(*address, data, size);
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
