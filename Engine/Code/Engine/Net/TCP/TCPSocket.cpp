#include "Engine/Net/TCP/TCPSocket.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Core/StringUtils.hpp"
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
TCPSocket::TCPSocket(SOCKET socket,char *ip,bool blocking)
{
	m_socket = socket;
	InitiateBytePacker();
	m_blocking = blocking;
	std::string ipaddress(ip);
	int colonIndex = ipaddress.find(':');
	m_ipaddress = ipaddress.substr(0, colonIndex);
	ToInt(ipaddress.substr(colonIndex + 1, ipaddress.length()),&m_port);
}


// DESTRUCTOR
TCPSocket::~TCPSocket()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSocket::InitiateBytePacker()
{
	m_bytePacketer = new BytePacker(LITTLE_ENDIAN);
	m_bytePacketer->m_bytepackerType = BYTEPACKER_CAN_GROW;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSocket::SetSocket(SOCKET socket)
{
	m_socket = socket;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::Send(char *data, size_t size)
{
	int sent = ::send(m_socket, data, static_cast<int>(size), 0);
	if(m_blocking)
	{
		if (sent == SOCKET_ERROR)
		{
			Disconnect();
			return 0U;
		}
	}
	else
	{
		if(sent == -1)
		{
			if(HasFatalError())
			{
				Disconnect();
				return 0U;
			}
		}
		if(sent == 0)
		{
			Disconnect();
			return 0U;
		}
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
	return Recv(outData, 256 - 1U);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::Recv(char *outData, unsigned int maxSize)
{
	size_t recvd = ::recv(m_socket, outData, maxSize, 0);
	if (m_blocking)
	{
		if (recvd == SOCKET_ERROR)
		{
			Disconnect();
			return 0U;
		}
	}
	else
	{
		if (recvd == -1)
		{
			if (HasFatalError())
			{
				Disconnect();
				return 0U;
			}
		}
		if (recvd == 0)
		{
			Disconnect();
			return 0U;
		}
	}
	outData[recvd] = NULL;
	return recvd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::Recv(size_t startSize, size_t endSize)
{
	char data[256];
	int recvd = ::recv(m_socket, data, static_cast<int>(endSize - startSize), 0);
	if (m_blocking)
	{
		if (recvd == SOCKET_ERROR)
		{
			Disconnect();
			return 0U;
		}
	}
	else
	{
		if (recvd == -1)
		{
			if (HasFatalError())
			{
				Disconnect();
				return 0U;
			}
		}
		if (recvd == 0)
		{
			Disconnect();
			return 0U;
		}
	}
	if(recvd > 0)
	{
		std::string strb = m_bytePacketer->GetBitString();
		std::string str  = m_bytePacketer->GetAsString();
		//data[recvd] = NULL;
		m_bytePacketer->WriteBytes(recvd, data);
	}
	return recvd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::HasFatalError()
{
	int error = WSAGetLastError();
	if(error == WSAEWOULDBLOCK || error == WSAEMSGSIZE || error == WSAECONNRESET)
	{
		return false;
	}
	return error > 0 ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::Connect(char *ip, int port)
{
	if(ip == NULL)
	{
		ip = (char *)NetAddress::GetIP().c_str();
	}
	
	
	m_ipaddress = ip;
	m_port = port;
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) 
	{
		return false;
	}

	NetAddress *netaddress = new NetAddress();

	sockaddr_storage saddr;
	int addresslen;
	if (!NetAddress::GetRemoteAddress(netaddress,(sockaddr*)&saddr, &addresslen, ip, ToString(port).c_str()))
	{
		return false;
	}
	
	int result = ::connect(m_socket, (sockaddr*)&saddr, addresslen);

	u_long non_blocking = m_blocking ? 0 : 1;
	::ioctlsocket((SOCKET)m_socket, FIONBIO, &non_blocking);

	if(result == INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : Closes the socket
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSocket::CloseSocket()
{
	Disconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TCPSocket::Disconnect()
{
	m_isDisconnected = true;
	closesocket(m_socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TCPSocket::IsDisconnected()
{
	if(m_isDisconnected)
	{
		return true;
	}
	/*if(HasFatalError())
	{
		return true;
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string TCPSocket::GetRemoteIp()
{
	return m_ipaddress;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/07
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TCPSocket::GetRemotePort()
{
	return m_port;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t TCPSocket::SendMsg(char *ip, char *port, char *msg, size_t msgLength,char *out_msg)
{
	if(ip == NULL)
	{
		ip = (char *)NetAddress::GetIP().c_str();
	}

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
	//int error = WSAGetLastError();
	if (result == SOCKET_ERROR)
	{
		::closesocket(sock);
		return 0;
	}

	::send(sock, msg, static_cast<int>(msgLength), 0);
	//int error = WSAGetLastError();

	int recvd = ::recv(sock, out_msg, 256 - 1U, 0);
	UNUSED(recvd);
	std::string str = out_msg;
	//DevConsole::GetInstance()->PushToOutputText("Invalid Command\n", Rgba::RED);
	//DevConsole::GetInstance()->PushToOutputText(str, Rgba::YELLOW);

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
		//int error = WSAGetLastError();
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
