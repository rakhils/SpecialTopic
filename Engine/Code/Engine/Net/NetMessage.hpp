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
enum eNetCoreMessage : uint8_t
{
	NETMSG_PING = 0,    // unreliable, connectionless
	NETMSG_PONG, 		// unreliable, connectionless
	NETMSG_HEARTBEAT,	// unreliable
	NETMSG_CORE_COUNT,
};

enum eNetMessageOption
{
	NETMESSAGE_OPTION_CONNECTION = 0,
	NETMESSAGE_OPTION_CONNECTIONLESS = BIT_FLAG(0),
	NETMESSAGE_OPTION_RELIABLE = BIT_FLAG(1),
	NETMESSAGE_OPTION_IN_ORDER = BIT_FLAG(2),
	NETMESSAGE_OPTION_RELIALBE_IN_ORDER = NETMESSAGE_OPTION_RELIABLE | NETMESSAGE_OPTION_IN_ORDER,
};
#define INVALID_PACKET_ACK (0xffff)
struct UDPHeader
{
	uint8_t  m_connectionindex = static_cast<uint8_t>(-1);
	uint16_t m_ack = static_cast<uint16_t>(0U);
	uint16_t m_lastReceivedAck = INVALID_PACKET_ACK;
	uint16_t m_previousReceivedAckBitfield = static_cast<uint8_t>(0U);
	uint8_t  m_unrealiableCount = static_cast<uint8_t>(0U);
	void operator=(const UDPHeader &copy);

};
class NetMessage : public BytePacker
{

public:
	//Member_Variables
	//char m_localBuffer[ETHERNET_MTU];
	//NetMessageDefinition* m_definition;
	UDPHeader			  m_header;
	uint8_t				  m_definitionIndex;
	std::string			  m_definitionName;
	int					  m_connectionIndex;
	NetAddress			  *m_address = nullptr;
	float				  m_lag = 0;
	//Static_Member_Variables

	//Methods

	NetMessage(std::string cmd);
	~NetMessage();

	void WriteCommandIndex();
	size_t				  GetSize();
	bool				  RequiresConnection();

	//Static_Methods
	static NetMessage * CreateAddMessage(float value1, float value2);
	static NetMessage * CreatePingMessage(std::string msg);
	static NetMessage * CreateHeartBeatMessage(NetAddress  *netaddress);
	static NetMessage * CreateHeartBeatMessage(NetConnection *connection);
	static NetMessage * CreateUnreliableTestMessage(NetConnection *connection,int count,int maxCount);
	static NetMessage * CreateReliableTestMessage(NetConnection *connection, int count, int maxCount);
	static NetMessage * CreateBadMessage();
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