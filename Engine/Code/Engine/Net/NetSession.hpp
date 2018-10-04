#pragma once
#include <map>
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Core/BytePacker.hpp"
/*\class  : NetSession	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/16/2018 6:52:56 PM
* \contact: srsrakhil@gmail.com
*/
typedef bool (*NetMessageCB)(NetMessage &msg, NetAddress &from);
struct NetMessageDefinition
{
	NetMessageDefinition(std::string name, NetMessageCB callback, std::string description)
	{
		m_name = name;
		m_callback = callback;
		m_description = description;
	}
	std::string m_name;
	NetMessageCB m_callback;
	std::string  m_description;
};

class NetSession
{

public:
	//Member_Variables
	std::map<int, NetConnection*>					m_remoteConnections;
	NetConnection *									m_channel;
	std::vector<NetMessageDefinition>				m_netMessageCmdDefinition;
	int												m_minHeaderSize = 2;

	//Static_Member_Variables
	static NetSession *s_netSession;
	static int		   s_defaultPort;

	//Methods
	NetSession(int listenPort);
	~NetSession();

	void					  Init();
	void					  Update(float deltaTime);
							  
	void					  RegisterMessage(std::string id, NetMessageCB netMsgCB,std::string description);
	NetMessageDefinition *    GetMsgDefinition(std::string cmdid);
	NetMessageDefinition *    GetMsgDefinition(int index);
	int						  GetMsgIndex(std::string cmdname);
	std::string				  GetMsgName(int index);
	bool					  VerifyMessage(std::string cmd,std::string msg);
							  
	void					  ProcessIncomingMessage();
	void					  ProcessOutgoingMessage();
							  
	//virtual void			   Send(uint16_t idx, char *data, size_t length);
	void					  AddBinding(int port);
	NetConnection*			  AddConnection(int index,std::string ip, int port);
	NetConnection*			  AddConnection(int index,NetAddress *netaddress);
	NetConnection*			  GetConnection(int index);
	NetConnection*			  GetConnection(NetAddress *netAddress);
							  
	void					  CloseAllConnections();
							  
	std::vector<NetMessage*>  ConstructMsgFromData(NetAddress &netAddress,size_t size,void *data);
	void					  ProcessMsg(std::vector<NetMessage *> netmsg,NetAddress *address);

	//Static_Methods
	static NetSession*		  GetInstance();

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

bool OnPing(NetMessage &netMsg, NetAddress &netAddress);
bool OnAdd(NetMessage  &netMsg, NetAddress &netAddress);
bool OnPong(NetMessage  &netMsg, NetAddress &netAddress);
bool OnAddResponse(NetMessage  &netMsg, NetAddress &netAddress);
bool OnBadMessage(NetMessage  &netMsg, NetAddress &netAddress);