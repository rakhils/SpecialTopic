#pragma once
#include <string>
#include "Engine/Core/BytePacker.hpp"
/*\class  : NetMessage	   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0	   
* \date   : 9/16/2018 7:32:44 PM
* \contact: srsrakhil@gmail.com
*/
class NetConnection;
class NetAddress;
class PacketTracker;
enum eNetCoreMessage : uint8_t
{
	NETMSG_PING = 0,    // unreliable, connectionless
	NETMSG_PONG, 		// unreliable, connectionless
	NETMSG_HEARTBEAT,	// unreliable
	NETMSG_BLANK,
	NETMSG_JOIN_REQUEST,
	NETMSG_JOIN_DENY,
	NETMSG_JOIN_ACCEPT,
	NETMSG_NEW_CONNECTION,
	NETMSG_JOIN_FINISHED,
	NETMSG_HANGUP,
	NETMSG_UPDATE_CONN_STATE,
	NETMSG_CORE_COUNT,
};

enum eNetMessageOption
{
	NETMESSAGE_OPTION_CONNECTION = 0,
	NETMESSAGE_OPTION_CONNECTIONLESS = BIT_FLAG(0),
	NETMESSAGE_OPTION_RELIABLE = BIT_FLAG(1),
	NETMESSAGE_OPTION_IN_ORDER = BIT_FLAG(2),
	NETMESSAGE_OPTION_UNRELIABLE, 
	NETMESSAGE_OPTION_RELIALBE_IN_ORDER = NETMESSAGE_OPTION_RELIABLE | NETMESSAGE_OPTION_IN_ORDER,
};
#define INVALID_PACKET_ACK (0xffff)
#define INVALID_PACKET_RELIABLE (0xffff)
struct PacketHeader
{
	uint8_t  m_connectionindex				= static_cast<uint8_t>(-1);
	//uint16_t m_reliableID					= static_cast<uint16_t>(0U);
	uint16_t m_ack							= static_cast<uint16_t>(0U);
	uint16_t m_lastReceivedAck				= INVALID_PACKET_ACK;
	uint16_t m_previousReceivedAckBitfield  = static_cast<uint8_t>(0U);
	uint8_t  m_unrealiableCount				= static_cast<uint8_t>(0U);
	void operator=(const PacketHeader &copy);
};
struct MsgHeader
{
	uint8_t  m_messageIndex;
	uint16_t m_relibaleID;
	void operator=(const MsgHeader &copy);	
};
class NetMessage : public BytePacker
{

public:
	//Member_Variables
	PacketHeader		  m_packetHeader;
	MsgHeader			  m_msgHeader;
	uint8_t				  m_definitionIndex;
	std::string			  m_definitionName;
	int					  m_connectionIndex;
	NetAddress			  *m_address = nullptr;
	float				  m_lag = 0;
	bool				  m_isReliable = false;
	uint16_t			  m_reliableID = 0;
	float				  m_lastSentTime = 0.f;
	PacketTracker		 *m_tracker = nullptr;

	int					  m_count = 0;
	//Static_Member_Variables

	//Methods

	NetMessage(std::string cmd);
	NetMessage(NetMessage *msg);
	~NetMessage();

	void				SetAddress(NetAddress *address);
	void				WriteCommandIndex();
	size_t				GetSize();
	bool				RequiresConnection();
	size_t				GetHeaderSize();
	uint16_t			GetReliableID();
	void				ResetAge();
	//Static_Methods

/*
	NETMSG_JOIN_REQUEST`, unreliable
		[] `NETMSG_JOIN_DENY`, unreliable
		[] `NETMSG_JOIN_ACCEPT`, reliable in - order
		[] `NETMSG_NEW_CONNECTION`, reliable in - order
		[] `NETMSG_JOIN_FINISHED`, relibale in - order
		[] `NETMSG_UPDATE_CONN_STATE`, reliable in - order*/

	static NetMessage * CreateAddMessage(float value1, float value2);
	static NetMessage * CreatePingMessage(std::string msg);
	static NetMessage * CreateHeartBeatMessage(NetAddress  *netaddress);
	static NetMessage * CreateHeartBeatMessage(NetConnection *connection);
	static NetMessage * CreateUnreliableTestMessage(NetConnection *connection,int count,int maxCount);
	static NetMessage * CreateReliableTestMessage(NetConnection *connection, int count, int maxCount);
	static NetMessage * CreateBlankMessage(NetConnection *connection);

	static NetMessage * CreateJoinRequestMsg(NetAddress *address);
	static NetMessage * CreateJoinDeny(NetAddress *address);
	static NetMessage * CreateJoinAcceptMsg(NetAddress *address,int index);
	static NetMessage * CreateNewConnection(NetConnection *connection);
	static NetMessage * CreateJoinFinished(NetConnection *connection);
	static NetMessage * CreateConnUpdate(NetConnection *connection);
	static NetMessage * CreateDisconnectUpdateMsg(NetConnection *connection);
	static NetMessage * CreateHangUpMsg(NetConnection *connection);
	static NetMessage * CreateObjectCreateMsg (uint8_t objectType, uint8_t objectID);
	static NetMessage * CreateObjectUpdateMsg (uint8_t objectType, uint8_t objectID);
	static NetMessage * CreateObjectDestroyMsg(uint8_t objectType, uint8_t objectID);


	//static NetMessage * CreateUpdateConnState(NetConnection *connection,EConnectionState state);
protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};