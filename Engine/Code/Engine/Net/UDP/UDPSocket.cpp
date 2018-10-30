#include "Engine/Net/UDP/UDPSocket.hpp"
#include "Engine/Core/EngineCommon.hpp"
UDPSocket::UDPSocket(NetAddress &address)
{
	m_address = address;
	//NetAddress *addr = new NetAddress();
	/*sockaddr_storage out;
	int out_addrlen;

	std::string ipaddress(address);
	std::string ip   = ipaddress.substr(0, ipaddress.find(':'));
	std::string port = ipaddress.substr(ipaddress.find(':')+1,ipaddress.length());*/

	//NetAddress::GetRemoteAddress(&m_netaddr,(sockaddr*)&out, &out_addrlen, ip.c_str(), port.c_str());
	//Bind(address);
}

UDPSocket::UDPSocket(int port)
{
	NetAddress::GetBindableAddress(&m_address, port);
	Bind(m_address,0);
	SetBlocking(false);
}

UDPSocket::~UDPSocket()
{
	Close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UDPSocket::Bind(NetAddress &addr, uint16_t port_range /*= 0U*/)
{
	UNUSED(port_range);
	SOCKET my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_storage sock_addr;
	size_t			 sock_addr_len;

	addr.ToSockAddr((sockaddr*)&sock_addr, &sock_addr_len);

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
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t UDPSocket::SendTo(NetAddress &addr, void *data, size_t const byte_count)
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
	int sent = ::sendto(sock, (char *)data, static_cast<int>(byte_count), 0, (sockaddr*)&sockaddrr, static_cast<int>(addr_len));
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
/*DATE    : 2018/09/16
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t UDPSocket::ReceiveFrom(void *buffer, size_t max_read_size,NetAddress *addr)
{
	if (IsClosed())
	{
		return 0U;
	}

	sockaddr_storage fromaddr;
	int addr_len = sizeof(sockaddr_storage);
	SOCKET sock = (SOCKET)m_handle;

	int recvd = ::recvfrom(sock, (char*)buffer, (int)max_read_size, 0, (sockaddr*)&fromaddr, &addr_len);
	addr->FromSockAddr((sockaddr*)&fromaddr);
	//int error = WSAGetLastError();
	if (recvd > 0)
	{
		return recvd;
	}
	return 0;
}
