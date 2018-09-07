#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/Net.hpp"

// CONSTRUCTOR
NetAddress::NetAddress()
{

}
// DESTRUCTOR
NetAddress::~NetAddress()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetAddress::GetBindableAddress(NetAddress *netaddr,int port)
{
	//netaddr = new NetAddress();
	sockaddr_storage saddr;
	int addrlen;
	if (!Net::GetBindableAddress((sockaddr*)&saddr, &addrlen,port))
	{
		return false;
	}
	netaddr->FromSockAddr((sockaddr*)&saddr);
	netaddr->m_port = port;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/24
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetAddress::GetRemoteAddress(NetAddress *netaddr,sockaddr *out, int *out_addrlen, char const *hostname, char const *service)
{
	Net::GetHostAddress(out, out_addrlen, hostname, service);
	netaddr->FromSockAddr((sockaddr*)&out);
	netaddr->ToSockAddr((sockaddr*)&out,(size_t*)&out_addrlen);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string NetAddress::GetIP()
{
	std::string ip = Net::GetIP();
	return ip;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/23
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetAddress::FromSockAddr(sockaddr const *addr)
{
	if(addr->sa_family != AF_INET)
	{
		return false;
	}

	sockaddr_in const *ipv4 = (sockaddr_in const*)addr;

	int ip   = ipv4->sin_addr.S_un.S_addr;
	int port = ::ntohs(ipv4->sin_port);

	m_address = ip;
	m_port	  = port;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/23
*@purpose : Netaddress to sockaddress
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetAddress::ToSockAddr(sockaddr *addr, size_t *out_size)
{
	*out_size		  = sizeof(sockaddr_in);
	sockaddr_in *ipv4 = (sockaddr_in*)addr;

	memset(ipv4, 0, sizeof(sockaddr_in));

	ipv4->sin_family		   = AF_INET;
	ipv4->sin_addr.S_un.S_addr = m_address;
	ipv4->sin_port			   = static_cast<USHORT>(::htons(m_port));
	return true;

	// to string
	/*
	 uint8_t *array = (uint8_t*)&m_address;
	 printf("%u.%u.%u.%u.%u:%u",
	 array[0],
	 array[1],
	 array[2],
	 array[3],
	 m_port);
	 */
}
