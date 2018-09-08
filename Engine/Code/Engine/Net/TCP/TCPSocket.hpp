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
class BytePacker;
class TCPSocket
{

public:
	//Member_Variables
	SOCKET					m_socket;
	BytePacker *			m_bytePacketer;
	bool					m_blocking       = true;
	bool					m_isListening	 = true;
	bool					m_isDisconnected = false;
	int						m_port;
	std::string				m_ipaddress;

	//Static_Member_Variables

	//Methods

	TCPSocket();
	TCPSocket(SOCKET socket,char *ip,bool blocking);
	~TCPSocket();

	void   InitiateBytePacker();
	void   SetSocket(SOCKET socket);

	size_t Send(char *data, size_t size);
	size_t Recv(char *outData);
	size_t Recv(char *outData, unsigned int maxSize);
	size_t Recv(size_t startSize, size_t endSize);

	bool   Connect(NetAddress &addr, char *msg);
	bool   Connect(char *ip, int port);
	bool   Connect(int port);

	void   CloseSocket();
	void   Disconnect();

	bool   IsDisconnected();

	bool   HasFatalError();

	std::string GetRemoteIp();
	int			GetRemotePort();

	static size_t SendMsg(char *ip, char *port, char *msg, size_t msgLength,char *out_msg);
	static void   RecvAndPushToDevConsole(void *data);
	void   RecvRCSCommands(void *data);

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

size_t SendTCPData(void *socket,std::string data);