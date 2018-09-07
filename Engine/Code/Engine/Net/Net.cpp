#include "Engine/Net/Net.hpp"
#include "Engine/Core/StringUtils.hpp"
std::string Net::m_ipaddr = "";
//#include "Engine/Logger/LogManager.hpp"
// CONSTRUCTOR
Net::Net()
{

}

// DESTRUCTOR
Net::~Net()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Net::StartUp()
{
	m_ipaddr = "";
	WORD version = MAKEWORD(2, 2);

	WSADATA data;
	int error = ::WSAStartup(version, &data);
	//ASSERT(error == 0);

	return(error == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Net::ShutDown()
{
	::WSACleanup();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Net::GetHostAddress(sockaddr *out, int *out_addrlen, char const *hostname, char const *service /*= "123"*/)
{
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags  = AI_PASSIVE;
	//hints.ai_flags  = AI_NUMERICHOST;

	addrinfo *result = nullptr;
	int status = ::getaddrinfo(hostname, service, &hints, &result);
	if (status != 0)
	{
		// NET FAILED ::gai_strerror(status)
		return false;
	}

	bool foundOne = false;
	addrinfo *iter = result;
	while (iter != nullptr)
	{
		if (iter->ai_family == AF_INET)
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr);

			//char out[256];
			//::inet_ntop(ipv4->sin_family, &(ipv4->sin_addr), out, 256);
			memcpy(out, ipv4, sizeof(sockaddr_in));
			*out_addrlen = sizeof(sockaddr_in);
			foundOne = true;
			break;
			//LogManager::LogTaggedPrintf("devconsole", " ADDRESS :: %s", out);
		}
		iter = iter->ai_next;
	}
	::freeaddrinfo(result);
	return foundOne;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/20
*@purpose : Connects to an ip and port
*@param   : IPaddr and port
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Net::Connect(char *ip, char *port)
{
	sockaddr_storage saddr;
	int addrlen;
	if(!GetHostAddress((sockaddr*)&saddr,&addrlen,ip,port))
	{
		return;
	}

	SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		// could not create socket
		return;
	}

	int result = ::connect(sock, (sockaddr*)&saddr, (int)addrlen);
	if(result == SOCKET_ERROR)
	{
		// COULD NOT CONNECT
		::closesocket(sock);
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/20
*@purpose : Get own IP address
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Net::GetBindableAddress(sockaddr *out,int *out_addrlen,int port)
{
	char myName[256];
	if(SOCKET_ERROR == ::gethostname(myName,256))
	{
		return false;
	}

	char const *service = ToString(port).c_str();

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;
	//hints.ai_flags  = AI_NUMERICHOST;

	addrinfo *result = nullptr;
	int status = ::getaddrinfo(myName, service, &hints, &result);
	if(status != 0)
	{
		// NET FAILED ::gai_strerror(status)
		return false;
	}

	addrinfo *iter = result;
	while(iter != nullptr)
	{
		if(iter->ai_family == AF_INET)
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr);
			char outbuff[256];
			::inet_ntop(ipv4->sin_family, &(ipv4->sin_addr), outbuff, 256);
			m_ipaddr = outbuff;
			memcpy(out, ipv4, sizeof(sockaddr_in));
			*out_addrlen = sizeof(sockaddr_in);
			//LogManager::LogTaggedPrintf("devconsole", " ADDRESS :: %s", out);
		}
		iter = iter->ai_next;
	}

	::freeaddrinfo(result);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/25
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string Net::GetIP()
{
	if(m_ipaddr.size() != 0)
	{
		return m_ipaddr;
	}
	char myName[256];
	if (SOCKET_ERROR == ::gethostname(myName, 256))
	{
		return "";
	}

	char const *service = "80";

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//hints.ai_flags  = AI_NUMERICHOST;

	addrinfo *result = nullptr;
	int status = ::getaddrinfo(myName, service, &hints, &result);
	if (status != 0)
	{
		// NET FAILED ::gai_strerror(status)
		return "";
	}

	addrinfo *iter = result;
	while (iter != nullptr)
	{
		if (iter->ai_family == AF_INET)
		{
			sockaddr_in* ipv4 = (sockaddr_in*)(iter->ai_addr);
			char buff[256];
			::inet_ntop(ipv4->sin_family, &(ipv4->sin_addr), buff, 256);
			m_ipaddr = std::string(buff);
			
			/*memcpy(out, ipv4, sizeof(sockaddr_in));
			*out_addrlen = sizeof(sockaddr_in);*/
			//LogManager::LogTaggedPrintf("devconsole", " ADDRESS :: %s", out);
		}
		iter = iter->ai_next;
	}
	::freeaddrinfo(result);
	return m_ipaddr;
}
