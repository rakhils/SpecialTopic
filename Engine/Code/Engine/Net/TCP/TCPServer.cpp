
#include "Engine/Net/TCP/TCPServer.hpp"
#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
//#include "Engine/Net/NetAddress.hpp"
// CONSTRUCTOR
TCPServer::TCPServer(int port)
{
	m_port = port;
	Listen(port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TCPServer::TCPServer()
{

}

// DESTRUCTOR
TCPServer::~TCPServer()
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/23
*@purpose : Listen in port
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::Listen(int port)
{
	NetAddress addr;

	int count = NetAddress::GetBindableAddress(&addr, port);
	SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_storage saddr;
	size_t			 addrlen;
	addr.ToSockAddr((sockaddr*)&saddr, &addrlen);

	int result = ::bind(sock, (sockaddr*)&saddr, addrlen);
	if (result == SOCKET_ERROR)
	{
		::closesocket(sock);
		return;
	}

	int maxQueued = 16;
	result = ::listen(sock, maxQueued);
	if (result == SOCKET_ERROR)
	{
		::closesocket(sock);
		return;
	}

	while (m_isListening)
	{
		sockaddr_storage remoteAddr;
		int remoteAddrLen = sizeof(sockaddr_storage);

		SOCKET remoteSock = ::accept(sock, (sockaddr*)&remoteAddr, &remoteAddrLen);
		int error		  =  WSAGetLastError();

		std::string pong = "pong";
		::send(remoteSock, pong.c_str(), pong.length(), 0);
		//int error1 = WSAGetLastError();


		if (remoteSock != INVALID_SOCKET)
		{
			TCPSocket* tcpSocket = new TCPSocket(remoteSock);
			Thread::ThreadCreateAndDetach("NetworkRecvToDevConsole", TCPSocket::RecvAndPushToDevConsole, tcpSocket);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::Listen()
{
	Listen(m_port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::ListenOnThread(void *data)
{
	int *value = (int*)(void*)&data;
	int	 port = *value;
	std::string ip = NetAddress::GetIP();
	DevConsole::GetInstance()->PushToOutputText("Starting to listen on IP " + ip + " PORT "+ToString(1),Rgba::GREEN);
	TCPServer *server = new TCPServer();
	server->Listen(12345);
}

