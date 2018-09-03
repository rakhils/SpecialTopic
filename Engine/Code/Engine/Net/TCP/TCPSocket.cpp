#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
/*

#include <string>*/
//#include <winsock.h>
// CONSTRUCTOR
TCPSocket::TCPSocket()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TCPSocket::TCPSocket(SOCKET socket)
{
	m_socket = socket;
}


// DESTRUCTOR
TCPSocket::~TCPSocket()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::Send(char *data, size_t size)
{
	int sent = ::send(m_socket, data, size, 0);
	if(sent == SOCKET_ERROR)
	{
		closesocket(m_socket);
		return 0U;
	}
	return sent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::Recv(char *outData)
{
	size_t recvd = ::recv(m_socket, outData, 256 - 1U, 0);
	outData[recvd] = NULL;
	return recvd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void TCPSocket::Connect(NetAddress &addr, char *msg )
{
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) 
	{
		return;
	}

	sockaddr_storage saddr;
	size_t		     addresslen;
	addr.ToSockAddr((sockaddr*)&saddr, &addresslen);

	int result = ::connect(m_socket, (sockaddr*)&saddr, (int)addresslen);
	if (result == SOCKET_ERROR)
	{
		::closesocket(m_socket);
		return;
	}

	/ *std::string str = msg;
	::send(sock, msg, str.length(), 0);

	char payload[256];
	size_t recvd = ::recv(sock, payload, 256 - 1U, 0);
	payload[recvd] = NULL;
	::closesocket(sock);* /
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : Closes the socket
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSocket::CloseSocket()
{
	::closesocket(m_socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::SendMessage(char *ip, char *port, char *msg, size_t msgLength)
{
	SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (sock == INVALID_SOCKET) 
	{
		return 0;
	}

	NetAddress *netaddress = new NetAddress();

	sockaddr_storage saddr;
	int addresslen;
	if (!NetAddress::GetRemoteAddress(netaddress,(sockaddr*)&saddr, &addresslen, ip, port))
	{
		return 0;
	}
	msg[msgLength] = NULL;

	/*sockaddr_storage saddr;
	size_t		     addresslength;*/
	
//	netaddress->ToSockAddr((sockaddr*)&saddr,&addresslength);

	int result = ::connect(sock, (sockaddr*)&saddr, addresslen);
	int error = WSAGetLastError();
	if (result == SOCKET_ERROR)
	{
		::closesocket(sock);
		return 0;
	}

	::send(sock, msg, msgLength, 0);
	int error1 = WSAGetLastError();

	char outData[256];
	int recvd = ::recv(sock, outData, 256 - 1U, 0);

	std::string str = outData;
	//DevConsole::GetInstance()->PushToOutputText("Invalid Command\n", Rgba::RED);
	DevConsole::GetInstance()->PushToOutputText(str, Rgba::YELLOW);

	::closesocket(sock);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : Receives the data and push to dev console
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSocket::RecvAndPushToDevConsole(void *data)
{
	while (true)
	{
		char outData[256];
		int recvd = ::recv(((TCPSocket*)(data))->m_socket, outData, 256 - 1U, 0);
		int error = WSAGetLastError();
		if(recvd == 0)
		{
			continue;
		}
		outData[recvd] = NULL;
		if (recvd == SOCKET_ERROR)
		{
			::closesocket(((TCPSocket*)(data))->m_socket);
			break;
		}

		std::string str = outData;
		//DevConsole::GetInstance()->PushToOutputText("Invalid Command\n", Rgba::RED);
		DevConsole::GetInstance()->PushToOutputText(str,Rgba::YELLOW);
		if (str == "rcquit")
		{
			::closesocket(((TCPSocket*)(data))->m_socket);
			break;
		}
	}
	delete ((TCPSocket*)(data));
}