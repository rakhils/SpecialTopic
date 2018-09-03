#pragma once
/*
#include <WinSock2.h> // before inculding windows.h
#include <WS2tcpip.h> // FOR IPV6
#include <Windows.h>*/
#include "Engine/Net/NetAddress.hpp"
/*\class  : TCPSocket		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/23/2018 3:06:18 PM
* \contact: srsrakhil@gmail.com
*/

class TCPSocket
{

public:
	//Member_Variables
	SOCKET m_socket;
	bool   m_isListening = true;
	//Static_Member_Variables

	//Methods

	TCPSocket();
	TCPSocket(SOCKET socket);
	~TCPSocket();

	size_t Send(char *data, size_t size);
	size_t Recv(char *outData);
	void   Connect(NetAddress &addr, char *msg);
	void   CloseSocket();

	static size_t SendMessage(char *ip, char *port, char *msg, size_t msgLength);
	static void   RecvAndPushToDevConsole(void *data);

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