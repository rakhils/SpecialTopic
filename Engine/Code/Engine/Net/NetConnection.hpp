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
	//Static_Member_Variables

	//Methods

	NetConnection(int index,NetAddress netAddress); // As Client
	NetConnection(int listenPort);					// As Server
	~NetConnection();

	void   SetConnectionIndex(int index);
	void   SetUnrealiableMsgCount(int count);
		   
	void   WriteConnectionIndex();
	void   WriteUnrealiableMsgCount();
	void   WriteHeader();
	void   WritePayload(NetMessage *msg);

	size_t Send(int connectionIndex, std::vector<NetMessage*> &msgs);
	size_t Send(NetMessage msg);
	void   Append(NetMessage *msg);
	size_t   Recv(char *data,size_t &maxlength,NetAddress *netAddress);
	
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