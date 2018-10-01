#pragma once
#include <map>
#include <vector>
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
typedef void (*ExeFunction_cb)(NetMessage const &msg, NetConnection const &from); 

class NetSession
{

public:
	//Member_Variables
	std::map<int, NetConnection*>		  m_remoteConnections;
	NetConnection*						  m_connection;
	std::map<std::string, ExeFunction_cb> m_exeFunction;

	std::vector<NetMessage*> m_udpSendData;
	std::vector<NetMessage*> m_udpRecvData;
	BytePacker bytePacker;
	//Static_Member_Variables
	static NetSession* s_instance;
	//Methods
	NetSession();
	~NetSession();

	void Bind(int port);
	void Update(float deltaTime);
	void ProcessIncomingMessage();
	void ProcessOutgoingMessage();

	//virtual void   Send(uint16_t idx, char *data, size_t length);
	virtual size_t Recv();

	void		   AddBinding(int port);
	NetConnection* AddConnection(int index, char *address);
	NetConnection* GetConnection(int index);

	void ProcessMsg(NetMessage *netmsg);

	float			GetTimeStamp();
	
	//Static_Methods
	static NetSession * GetInstance();
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