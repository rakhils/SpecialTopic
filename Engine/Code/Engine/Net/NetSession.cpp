#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"
// CONSTRUCTOR
NetSession::NetSession()
{

}

// DESTRUCTOR
NetSession::~NetSession()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Bind(int port)
{
	m_connection->Bind(port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::Update(float deltaTime)
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessIncomingMessage()
{
	// 2 or 3 way handshake
	/*for(all connections in array)
	{
		if (connection->m_state == IDLE)
		{
			char *data = new char[INIT_DATA_SIZE];
			connection->Recv(data, INIT_DATA_SIZE);
			if (IsStartOfData(data))
			{
				m_connection->SendAck();
				m_connection->m_state = DATA_TRASMISSION;
				break;
			}
		}
		if(connection->m_state == DATA_TRASMISSION)
		{
			Recv();
			UpdatePacketIndex();
		}
	}*/
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessOutgoingMessage()
{
	// for loop through all connections send data
	// check all acks for the transimtted packets arrives then cleanup the queus
	/*for(int index = 0;index < m_remoteConnections.size();index++)
	{
		if(m_remoteConnections.at(index).hasStartDataNotRecvd())
		{
			m_remoteConnections.sendAck();
			m_remoteConnections.updateStateToDataTransmission();
		}

		if(m_remoteConnections.at(index).hasReceivedAcks())
		{
			m_remoteConnections.send();
		}

		if(m_remoteConnections.hasRecvdAllAck() || m_remoteConnections.hasTimedOut())
		{
			delete m_remoteConnections;
		}

		if(m_remoteConnections.hasDataRecvTimedOut())
		{
			for(int index = 0;index < m_remoteConnections.LeftOutDatas;index++)
			{
				m_remoteConnections.sendRetransmitData(m_remoteConnections.leftOutData.index);
			}
		}
	}*/

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetSession::Recv()
{
	/*char *data;
	size_t length;
	m_connection->Recv(data, length);
	BytePacker bytePacker;
	bytePacker.WriteBytes(length, data);

	// WRITTING ORDER
	/ *bytePacker.WriteSize(4 + 2 + endIndex - startIndex);
	float timeStamp = GetTimeStamp();
	bytePacker.WriteBytes(4, (char*)&timeStamp);
	bytePacker.WriteBytes(2, (char*)&idx);
	bytePacker.WriteBytes(endIndex - startIndex, ((char*)data + startIndex));* /

	size_t udpPacketDataIndex;
	bytePacker.ReadSize(&udpPacketDataIndex);
	float timeStamp;
	uint16_t index;
	bytePacker.ReadBytes(&timeStamp, sizeof(float));
	bytePacker.ReadBytes(&index, sizeof(uint16_t));

	char *udpData = new char[length - sizeof(float) - sizeof(uint16_t)];
	bytePacker.ReadBytes(udpData,length - sizeof(float) - sizeof(uint16_t));


	bool udpDataFound = false;
	for(int udpPacketDataIndex = 0;udpPacketDataIndex < m_udpRecvData.size();index++)
	{
		if(m_udpRecvData.at(udpPacketDataIndex)->m_index == index)
		{
			BytePacker bytePacker = m_udpRecvData.at(udpPacketDataIndex)->m_bytePacker;
			bytePacker.m_currentWritePosition = udpPacketDataIndex;
			bytePacker.WriteBytes(length - sizeof(float) - sizeof(uint16_t), udpData);
			m_udpRecvData.at(udpPacketDataIndex)->m_bytesRead += length - length - sizeof(float) - sizeof(uint16_t);
			udpDataFound = true;

			if(m_udpRecvData.at(udpPacketDataIndex)->m_bytesRead == m_udpRecvData.at(udpPacketDataIndex)->m_totalBytes)
			{
				ProcessMsg(m_udpRecvData.at(udpPacketDataIndex));
			}
			break;
		}
	}*/
	return 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::AddBinding(int port)
{
	NetConnection *connection = new NetConnection(port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetConnection* NetSession::AddConnection(int index, char *address)
{
	NetConnection *connection = new NetConnection(address);
	m_remoteConnections.at(index) = connection;
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
	return m_remoteConnections.at(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetSession::ProcessMsg(NetMessage *netmsg)
{
	// check for the all function pointers stored in session and call back
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float NetSession::GetTimeStamp()
{
	return 0.f;
}
