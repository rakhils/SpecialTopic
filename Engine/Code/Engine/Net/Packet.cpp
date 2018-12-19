#include "Engine/Net/Packet.hpp"

// CONSTRUCTOR
Packet::Packet()
{

}

// DESTRUCTOR
Packet::~Packet()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Retrives header size
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t Packet::GetHeaderSize()
{
	return 8;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::SetConnectionIndex(uint8_t index)
{
	m_header.m_connectionindex = index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes sent ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::SetNextSentAck(uint16_t sentAck)
{
	m_header.m_ack = sentAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes last recived ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::SetLastReceivedAck(uint16_t lastRecvAck)
{
	m_header.m_lastReceivedAck = lastRecvAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes previous received ack bit field
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::SetPreviousReceviedAckBitField(uint16_t prevRecvAck)
{
	m_header.m_previousReceivedAckBitfield = prevRecvAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes unrealible msg count
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::SetUnrealiableMsgCount(uint8_t msgCount)
{
	m_header.m_unrealiableCount = msgCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::WriteConnectionIndex(uint8_t index)
{
	m_header.m_connectionindex = index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes sent ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::WriteNextSentAck(uint16_t sentAck)
{
	m_header.m_ack = sentAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes last recived ack
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::WriteLastReceivedAck(uint16_t lastRecvAck)
{
	m_header.m_lastReceivedAck = lastRecvAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes previous received ack bit field
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::WritePreviousReceviedAckBitField(uint16_t prevRecvAck)
{
	m_header.m_previousReceivedAckBitfield = prevRecvAck;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes unrealible msg count
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Packet::WriteUnrealiableMsgCount(uint8_t msgCount)
{
	m_packet.m_currentWritePosition = 7;
	m_header.m_unrealiableCount = msgCount;
	m_packet.WriteBytes(1, (char*)&m_header.m_unrealiableCount);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes header
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Packet::WritePacketHeader(PacketHeader header)
{
	m_header = header;
	m_packet.m_currentWritePosition = 0;
	m_packet.WriteBytes(1, (char*)&m_header.m_connectionindex);
	m_packet.WriteBytes(2, (char*)&m_header.m_ack);
	m_packet.WriteBytes(2, (char*)&m_header.m_lastReceivedAck);
	m_packet.WriteBytes(2, (char*)&m_header.m_previousReceivedAckBitfield);
	m_packet.WriteBytes(1, (char*)&m_header.m_unrealiableCount);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Packet::WritePacketHeader()
{
	return WritePacketHeader(m_header);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Packet::WriteMsgHeader(MsgHeader header)
{
	m_packet.WriteBytes(1, (char*)&header.m_messageIndex);
	m_packet.WriteBytes(2, (char*)&header.m_relibaleID);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Writes payload
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Packet::WriteMsg(NetMessage *msg)
{
	size_t msgSize = msg->m_bufferSize + 1;
	/*if(msg->m_isReliable)
	{
		msgSize += 2;
		m_packet.WriteBytes(2, (void*)&(msgSize));
		m_packet.WriteBytes(1, (void *)&msg->m_definitionIndex);
		m_packet.WriteBytes(2, (void*)&msg->m_reliableID);
	}
	else*/
	{
		m_packet.WriteBytes(2, (void*)&(msgSize));
		m_packet.WriteBytes(1, (void *)&msg->m_definitionIndex);
	}
	if(msg->m_bufferSize > 0)
	{
		m_packet.WriteBytes(msg->m_bufferSize, msg->m_buffer);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Packet::WriteNextMsg(NetMessage* msg)
{
	return m_packet.WriteBytes(msg->m_bufferSize, msg->m_buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Checks if packet has room enough for a packet
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Packet::HasRoom(size_t size)
{
	if(m_packet.m_bufferSize + size < PACKET_MTU)
	{
		return true;
	}
	return false;
}
