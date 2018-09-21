#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
// CONSTRUCTOR

NetConnection::NetConnection(char *address)
{
	m_udpSocket = new UDPSocket(address);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection::NetConnection(int port)
{
	m_udpSocket = new UDPSocket(port);
}

// DESTRUCTOR
NetConnection::~NetConnection()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Bind(int port)
{
	NetAddress::GetBindableAddress(&m_address, port);
	m_udpSocket->Bind(m_address,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConnection::Recv(char *data,size_t &length)
{
	m_udpSocket->ReceiveFrom(&m_address, data, length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NetConnection::GetTimeStamp()
{
	return 0.f;
}
