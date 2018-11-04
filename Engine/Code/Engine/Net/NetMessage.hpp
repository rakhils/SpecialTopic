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

class NetMessage : public BytePacker
{

public:
	//Member_Variables
	//char m_localBuffer[ETHERNET_MTU];
	//NetMessageDefinition* m_definition;
	uint8_t				  m_definitionIndex;
	std::string			  m_definitionName;
	int					  m_connectionIndex;
	NetAddress			  *m_address = nullptr;
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