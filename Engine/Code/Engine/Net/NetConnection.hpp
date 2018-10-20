#pragma once
#include <vector>
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetMessage.hpp"
/*\class  : NetConnection		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0		   
* \date   : 9/16/2018 7:37:15 PM
* \contact: srsrakhil@gmail.com
*/
class UDPSocket;
class NetSession;
struct UDPHeader
{
	uint8_t m_connectionindex  = static_cast<uint8_t>(-1);
	uint8_t m_unrealiableCount = static_cast<uint8_t>(0U);
};
class NetConnection
{

public:
	//Member_Variables
	UDPSocket *					m_udpSocket;
	NetAddress					m_address;
	int							m_index;
	std::vector<NetMessage*>	m_outboundMsgs;
	BytePacker					m_packet;
	UDPHeader					m_header;
	NetSession *				m_session;
	float						m_lastHeartbeatReceivedTime;
	float						m_lastHeartbeatHPC;
	float						m_heartBeatFrequency = .001;
	float						m_sendRate			 = 0;
	float						m_lastSendTime		 = 0;
	//Static_Member_Variables

	//Methods

	NetConnection(int index,NetAddress netAddress); // As Client
	NetConnection(int listenPort);					// As Server
	~NetConnection();

	void   SetLastHeartBeatReceivedTime(float time);
	void   SetHeartBeatFrequency(float freq);
	void   SetConnectionIndex(int index);
	void   SetUnrealiableMsgCount(int count);
	void   SetSendRate(float sendRate);

	int    GetHeaderSize();

	void   WriteConnectionIndex();
	void   WriteUnrealiableMsgCount();
	void   WriteHeader();
	bool   WritePayload(NetMessage *msg);

	void   SendHeartBeat();

	void Update(float deltaTime);

	size_t SendImmediately(int connectionIndex, std::vector<NetMessage*> &msgs);
	size_t SendImmediately(NetMessage msg);
	void   Append(NetMessage *msg);
	size_t FlushMsgs();
	size_t   Recv(char *data,size_t &maxlength,NetAddress *netAddress);
	
	std::string GetIPPortAsString();
	//Static_Methods

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