#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/StringUtils.hpp"
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
}

// DESTRUCTOR
NetConnection::~NetConnection()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/23
*@purpose : Sends msg in UDP Connection
*@param   : NetMsg (Data)
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetConnection::Send(NetMessage msg)
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
size_t NetConnection::Send(int connectionIndex, std::vector<NetMessage*> &msgs)
{
	int msgCount = msgs.size();
	m_header.m_unrealiableCount = static_cast<uint8_t>(msgCount);
	m_header.m_connectionindex = static_cast<uint8_t>(m_index);
	WriteHeader();
	for(int index = 0;index < msgCount;index++)
	{
		WritePayload(msgs.at(index));
		delete msgs.at(index);
	}
	msgs.clear();
	size_t length = m_packet.m_bufferSize;
	size_t sendCount = m_session->m_channel->m_udpSocket->SendTo(m_address, (char *)m_packet.m_buffer, length);
	m_packet.m_bufferSize = 0;
	delete m_packet.m_buffer;
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
void NetConnection::WritePayload(NetMessage *msg)
{
	m_packet.WriteBytes(msg->m_bufferSize, msg->m_buffer);
}
