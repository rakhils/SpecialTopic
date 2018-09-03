#pragma once
#include "Engine/Net/Net.hpp"
/*
#include <WinSock2.h> // before including windows.h
#include <WS2tcpip.h> // FOR IPV6
#include <Windows.h>*/
/*\class  : NetAddress		   
* \group  : <GroupName>	   
* \brief  :	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/23/2018 2:14:19 PM
* \contact: srsrakhil@gmail.com
*/
class NetAddress
{

public:
	//Member_Variables
	int m_address;
	int m_port;
	//Static_Member_Variables

	//Methods

	NetAddress();
	NetAddress(char *ip, char* port);
	~NetAddress();
	
	bool FromSockAddr(sockaddr const *addr);
	bool ToSockAddr(sockaddr *addr, size_t *out_size);

	//Static_Methods
	static bool GetBindableAddress(NetAddress *netaddr,int port);
	static bool GetRemoteAddress(NetAddress *netaddr,sockaddr *out, int *out_addrlen, char const *hostname, char const *service);
	static std::string GetIP();

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