#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetSession.hpp"
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

} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/25
*@purpose : Writes command index at position 2
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetMessage::WriteCommandIndex()
{
	//size_t tempIndex = m_currentWritePosition;
	//m_currentWritePosition = 2;
	WriteBytes(1, (char*)&m_definitionIndex);
	//m_currentWritePosition = tempIndex;
}

