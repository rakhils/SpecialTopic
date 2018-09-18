#pragma once
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
enum ConnectionStates
{
	IDLE,
	START_DATA_SEND,
	START_DATA_RECV,
	ACK_DATA_SEND,
	ACK_DATA_RECV,
	DATA_TRASMISSION
};
class NetConnection
{

public:
	//Member_Variables
	UDPSocket *m_udpSocket;
	NetAddress m_address;
	ConnectionStates m_state;
	//Static_Member_Variables

	//Methods

	NetConnection(char *address);
	NetConnection(int port);
	~NetConnection();

	void   Bind(int port);

	size_t Send(NetMessage msg);
	void   Recv(char *data,size_t &length);
	
	float GetTimeStamp();
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