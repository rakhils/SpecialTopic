#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
// CONSTRUCTOR
// 0 -> add
// 1 -> add_response
// 2 -> ping
// 3 -> pong
// 
// PING" and an "ADD" connection 0
// 
//   00 02    -> packet header
// - 07 00 02 [ 05 'h' 'e' 'l' 'l' 'o' ]
// - 09 00 00 [ <float> <float> ]
// 
// A packet header is..
//
// [uint8_t sender_conn_idx]
// [uint8_t unreliable_count]
// 
// Followed by all unreliable, each message having the format
// 
// [uint16_t message_and_header_length]
// [uint8_t  message_index]
// [byte_t*  message_payload]
// will be message_and_header_length - 1U long for now

NetMessage::NetMessage(std::string msg)
{
	m_definitionName			= msg;
	m_definitionIndex			= static_cast<uint8_t>(NetSession::GetInstance()->GetMsgIndex(msg));
}

// DESTRUCTOR
NetMessage::~NetMessage()
{
	//delete m_address;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetMessage::SetAddress(NetAddress *address)
{
	m_address = new NetAddress(*address);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Writes command index at position 2
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetMessage::WriteCommandIndex()
{
	WriteBytes(1, (char*)&m_definitionIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/28
*@purpose : Returns the size of msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetMessage::GetSize()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Check if the msg requires a connection 
*@param   : NIL
*@return  : true if connection bit is set else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetMessage::RequiresConnection()
{
     if(m_definitionIndex < 0)
	 {
		 return false;
	 }
	if(NetSession::GetInstance()->m_netMessageCmdDefinition.at(m_definitionIndex).m_options == NETMESSAGE_OPTION_CONNECTION)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Returns header size
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t NetMessage::GetHeaderSize()
{
	size_t headerSize = 8;
	if(m_isReliable)
	{
		headerSize += 2;
	}
	return headerSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/08
*@purpose : retrieves reliable ID
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetMessage::GetReliableID()
{
	m_currentReadPosition = 3;
	uint16_t relID = 0;
	ReadBytes(&relID, 2);
	m_reliableID = relID;
	return relID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : Resets the age
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetMessage::ResetAge()
{
	m_lastSentTime = static_cast<float>(GetCurrentTimeSeconds());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/03
*@purpose : Creates add msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateAddMessage(float value1, float value2)
{
	NetMessage *msg = new NetMessage("add");
	size_t msgSize = 0;
	// write temporarily 
	msg->WriteBytes(2, (char*)&msgSize);
	///////////////
	msg->WriteCommandIndex();
	msg->WriteBytes(4, (char*)&value1);
	msg->WriteBytes(4, (char*)&value2);
	msg->m_currentWritePosition = 0;
	msgSize = msg->m_bufferSize - 2;
	msg->WriteBytes(2, (char*)&(msgSize));
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/03
*@purpose : Creates ping msgs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreatePingMessage(std::string msg)
{
	NetMessage *netMsg = new NetMessage("ping");
	size_t msgSize = 0;
	// write temporarily 
	netMsg->WriteBytes(2, (char*)&msgSize);
	///////////////
	netMsg->WriteCommandIndex();
	netMsg->WriteString(msg.c_str());
	netMsg->m_currentWritePosition = 0;
	msgSize = netMsg->m_bufferSize - 2;
	netMsg->WriteBytes(2, (char*)&(msgSize));
	return netMsg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/18
*@purpose : Creates a hear beat msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateHeartBeatMessage(NetAddress *netaddress)
{
	NetMessage *netMsg = new NetMessage("heartbeat");
	netMsg->m_isReliable = false;
	netMsg->m_address = netaddress;
	size_t msgSize = 0;
	// write temporarily 
	netMsg->WriteBytes(2, (char*)&msgSize);
	///////////////
	netMsg->WriteCommandIndex();
	netMsg->m_currentWritePosition = 0;
	msgSize = netMsg->m_bufferSize - 2;
	netMsg->WriteBytes(2, (char*)&(msgSize));
	std::string pp = netMsg->GetBitString();
	return netMsg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/23
*@purpose : Creates a heartbeat msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateHeartBeatMessage(NetConnection *connection)
{
	UNUSED(connection);
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Creates a unreliable test msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateUnreliableTestMessage(NetConnection *connection,int count,int maxCount)
{
	NetMessage *msg = new NetMessage("unreliable_test");
	msg->m_address = &connection->m_address;
	size_t msgSize = 0;
	// write temporarily 
	msg->WriteBytes(2, (char*)&msgSize);
	///////////////
	msg->WriteCommandIndex();
	msg->WriteBytes(4, (char*)&count);
	msg->WriteBytes(4, (char*)&maxCount);
	msg->m_currentWritePosition = 0;
	msgSize = msg->m_bufferSize - 2;
	msg->WriteBytes(2, (char*)&(msgSize));
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateReliableTestMessage(NetConnection *connection, int count, int maxCount)
{
	NetMessage *msg = new NetMessage("reliable_test");
	msg->m_count = count;
	msg->m_isReliable = true;
	msg->m_address = &connection->m_address;
	size_t msgSize = 0;
	// write temporarily 
	msg->WriteBytes(2, (char*)&msgSize);
	///////////////
	msg->WriteCommandIndex();
	std::string bit = msg->GetBitString();
	connection->IncrementRealiableID();
	uint16_t relID = connection->GetNextRealiableIDToSend();
	bit = msg->GetBitString();
	msg->WriteBytes(2, (void*)&relID);
	bit = msg->GetBitString();
	msg->m_reliableID = relID;

	msg->WriteBytes(4, (char*)&count);
	msg->WriteBytes(4, (char*)&maxCount);
	bit = msg->GetBitString();
	msg->m_currentWritePosition = 0;
	msgSize = msg->m_bufferSize - 2;
	msg->WriteBytes(2, (char*)&(msgSize));
	bit = msg->GetBitString();
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateBlankMessage(NetConnection *connection)
{
	NetMessage *msg = new NetMessage("blank");
	msg->m_isReliable = false;
	msg->m_address = &connection->m_address;
	size_t msgSize = 0;
	// write temporarily 
	msg->WriteBytes(2, (char*)&msgSize);
	///////////////
	msg->WriteCommandIndex();

	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateJoinRequestMsg(NetAddress *address)
{
	NetMessage *msg = new NetMessage("join_request");
	msg->m_address  = address;
	size_t msgSize = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->WriteCommandIndex();
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateJoinDeny(NetAddress *address)
{
	NetMessage *msg = new NetMessage("join_deny");
	msg->m_address = address;
	size_t msgSize = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->WriteCommandIndex();
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateJoinAcceptMsg(NetAddress *address,int indexInt)
{
	NetMessage *msg = new NetMessage("join_accept");
	msg->SetAddress(address);
	size_t msgSize = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	
	msg->WriteCommandIndex();

	uint8_t index = static_cast<uint8_t>(indexInt);
	msg->WriteBytes(1, (char*)&index);

	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateNewConnection(NetConnection *connection)
{
	NetMessage *msg = new NetMessage("new_connection");
	msg->m_address = &connection->m_address;
	size_t msgSize = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->WriteCommandIndex();
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateJoinFinished(NetConnection *connection)
{
	NetMessage *msg = new NetMessage("join_finished");
	msg->m_address = &connection->m_address;
	size_t msgSize = 0;

	msg->WriteBytes(2, (char*)&msgSize);
	msg->WriteCommandIndex();
	/*msg->WriteBytes(1, (char*)&index);

	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msg->m_bufferSize);*/
	return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/30
*@purpose : Creates an update conn stats msg with disconnect code
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetMessage * NetMessage::CreateDisconnectUpdateMsg(NetConnection *connection)
{
	NetMessage *msg = new NetMessage("update_conn_state");
	msg->m_address = &connection->m_address;

	size_t msgSize = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->WriteCommandIndex();

	uint8_t index = static_cast<uint8_t>(connection->m_index);
	msg->WriteBytes(1, (char*)&index);
	uint8_t code = static_cast<uint8_t>(1);
	msg->WriteBytes(1, (char*)&code);

	msgSize = msg->m_bufferSize - 2;
	msg->WriteBytes(2, (char*)&msgSize);
	return msg;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/22
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
NetMessage * NetMessage::CreateUpdateConnState(NetConnection *connection,EConnectionState state)
{
	NetMessage *msg = new NetMessage("update_conn_state");
	msg->m_address = &connection->m_address;
	size_t msgSize = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	msg->WriteCommandIndex();
	uint8_t stateUint = static_cast<uint8_t>(state);
	msg->WriteBytes(1, (char*)&stateUint);

	msg->m_currentWritePosition = 0;
	msg->WriteBytes(2, (char*)&msgSize);
	return msg;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MsgHeader::operator=(const MsgHeader &copy)
{
	m_messageIndex = copy.m_messageIndex;
	m_relibaleID = copy.m_relibaleID;
}
