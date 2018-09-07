
#include "Engine/Net/TCP/TCPServer.hpp"
#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/System/Thread/Thread.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Net/RCS.hpp"
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
	m_ipaddress = NetAddress::GetIP();
	NetAddress addr;

	NetAddress::GetBindableAddress(&addr, port);

	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	sockaddr_storage saddr;
	size_t			 addrlen;
	addr.ToSockAddr((sockaddr*)&saddr, &addrlen);

	int result = ::bind(m_socket, (sockaddr*)&saddr, static_cast<int>(addrlen));
	if (result == SOCKET_ERROR)
	{
		Disconnect();
		return;
	}

	u_long non_blocking = m_blocking ? 0 : 1;
	::ioctlsocket((SOCKET)m_socket, FIONBIO, &non_blocking);
	
	int maxQueued = 16;
	result = ::listen(m_socket, maxQueued);
	if (result == SOCKET_ERROR)
	{
		Disconnect();
		return;
	}
	if(HasFatalError())
	{
		Disconnect();
		return;
	}
	

	while (m_isListening)
	{
		sockaddr_storage remoteAddr;
		int remoteAddrLen = sizeof(sockaddr_storage);

		SOCKET remoteSock = ::accept(m_socket, (sockaddr*)&remoteAddr, &remoteAddrLen);
		

		/*remoteAddr.ss_family.

		sockaddr_in const *ipv4 = (sockaddr_in const*)remoteAddr;

		int ip   = ipv4->sin_addr.S_un.S_addr;
		int port = ::ntohs(ipv4->sin_port);* /*/

		if (remoteSock != INVALID_SOCKET)
		{
			if (port == RCS::GetInstance()->m_rcsPort)
			{
				RCS::GetInstance()->m_state = RCS_STATE_HOST;
				TCPSocket *tcpSocket = new TCPSocket(remoteSock,(char*)(NetAddress::GetIP()+":"+ToString(m_port)).c_str(),false);
				RCS::GetInstance()->PushNewConnection(tcpSocket);
			}
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
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::Disconnect()
{
	if(m_port == RCS::GetInstance()->m_rcsPort)
	{
		RCS::GetInstance()->FailedToHost();
	}
	m_isListening = false;
	m_isDisconnected = true;
	::closesocket(m_socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TCPServer::HasFatalError()
{
	int error = WSAGetLastError();
	if (error == WSAEWOULDBLOCK || error == WSAEMSGSIZE || error == WSAECONNRESET)
	{
		return false;
	}
	return error > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string TCPServer::GetIp()
{
	return m_ipaddress;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPServer::ListenOnThread(void *data)
{
	TCPServer *server = (TCPServer*)(data);
	int	 port		  = server->m_port;
	std::string ip    = NetAddress::GetIP();
	DevConsole::GetInstance()->PushToOutputText("Starting to listen on IP " + ip + " PORT "+ToString(port),Rgba::GREEN);
	server->Listen(port);
}

