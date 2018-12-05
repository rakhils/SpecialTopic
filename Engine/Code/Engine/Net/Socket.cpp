#include "Engine/Net/Socket.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
Socket::Socket()
{

}

// DESTRUCTOR
Socket::~Socket()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Socket::Start()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Socket::Stop()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Socket::SetBlocking(bool blocking)
{
	u_long non_blocking = blocking ? 1 : 0;
	::ioctlsocket((SOCKET)m_handle, FIONBIO, &non_blocking);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Socket::Close()
{
	closesocket((SOCKET)m_handle);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Socket::IsClosed() const
{
	if(m_handle == (socket_t)INVALID_SOCKET)
	{
		return true;
	}
	return false;
}

bool UDP1Socket::bind(NetAddress &addr, uint16_t port_range /*= 0U*/)
{
	UNUSED(port_range);
	// create the socket 
	SOCKET my_socket = socket(AF_INET,	// IPv4 to send...
		SOCK_DGRAM,							// ...Datagrams... 
		IPPROTO_UDP);						// ...using UDP.

											//ASSERT_RETURN_VALUE(my_socket != INVALID_SOCKET, false);

											// TODO, try to bind all ports within the range.  
											// Shown - just trying one; 
	sockaddr_storage sock_addr;
	size_t			 sock_addr_len;


	addr.ToSockAddr((sockaddr*)&sock_addr, &sock_addr_len);

	// try to bind - if it succeeds - great.  If not, try the next port in the range.
	int result = ::bind(my_socket, (sockaddr*)&sock_addr, (int)sock_addr_len);
	if (result == 0)
	{
		m_handle = (socket_t)my_socket;
		m_address = addr;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t UDP1Socket::send_to(NetAddress &addr, void const *data, size_t const byte_count)
{
	if(IsClosed())
	{
		return false;
	}

	sockaddr_storage sockaddrr;
	size_t addr_len;

	addr.ToSockAddr((sockaddr*)&sockaddrr, &addr_len);

	SOCKET sock = (SOCKET)m_handle;
	//int error = WSAGetLastError();
	int sent = ::sendto(sock, (char const *)&data, static_cast<int>(byte_count), 0, (sockaddr*)&sockaddrr, static_cast<int>(addr_len));
	int error1 = WSAGetLastError();
	if(sent > 0)
	{
		return (size_t)sent;
	}
	/*else if(has_fatal_error())
	{
		close();
	}*/
	return 0U;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t UDP1Socket::receive_from(NetAddress *out_addr, void *buffer, size_t  max_read_size)
{
	UNUSED(out_addr);

	if(IsClosed())
	{
		return 0U;
	}

	sockaddr_storage fromaddr;
	int addr_len = sizeof(sockaddr_storage);
	SOCKET sock = (SOCKET)m_handle;


	int recvd = ::recvfrom(sock, (char*)buffer, (int)max_read_size, 0, (sockaddr*)&fromaddr, &addr_len);
	//int error = WSAGetLastError();
	if(recvd > 0)
	{
		return recvd;
	}
	else 
	{
		//close();
	}
	return 0;
}

