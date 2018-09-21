#include "Engine/Net/UDP/UDPTest.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
UDPTest::UDPTest()
{

}

// DESTRUCTOR
UDPTest::~UDPTest()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t UDPTest::send_to(NetAddress &addr, void const *buffer, int byte_count)
{
	return m_socket.send_to(addr, buffer, byte_count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UDPTest::Update()
{
	char *buffer;// [PACKET_MTU];
	buffer = new char[PACKET_MTU];
	NetAddress from_addr;
	size_t read = m_socket.receive_from(&from_addr, buffer, PACKET_MTU);

	if (read > 0) 
	{
		int max_bytes;
		if(read < 128)
		{
			max_bytes = static_cast<int>(read);
		}
		else
		{
			max_bytes = 128;
		}

		int string_size = max_bytes * 13U + 3U;
		buffer = new char[string_size];
		sprintf_s(buffer, 3U, "0x");

		/*std::string output = "0x";
		output.reserve(max_bytes * 2U + 3U);
		*/

		char *iter = buffer;
		iter += 2U; 

		for (int i = 0; i < read; ++i) 
		{
			sprintf_s(iter, 4U,"%08X", buffer[i]);
			iter += 3U;
		}
		*iter = NULL;
		//output.recalculate_sizes();

		DevConsole::GetInstance()->PushToOutputText(buffer);
		delete buffer;
		//ConsolePrintf("Received from %s;\n%s", from_addr.to_string().c_str(),output.c_str());
	}
}

bool UDPTest::start(int port)
{
	// get an address to use; 
	NetAddress addr;
	bool count = addr.GetBindableAddress(&addr, port);
	if (count == false)
	{
		//ConsolePrintf("No addresses");
		return false;
	}

	if (!m_socket.bind(addr, 10)) 
	{
		//int error = WSAGetLastError();
		return false;
	}
	else 
	{
		m_socket.set_blocking(false);
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
void UDPTest::stop()
{
	m_socket.close();
	//int error = WSAGetLastError();
	//int a = 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/12
*@purpose : Sends a msg in udp to given ip and port 
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t UDPSend(const char *ip, int port, const char* msg)
{
	UDPTest udptest;
	udptest.start(port+1);

	NetAddress *addr = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;

	port = 10088;
	NetAddress::GetRemoteAddress(addr, (sockaddr*)&out, &out_addrlen, ip, ToString(port).c_str());
	std::string str = msg;

	size_t sendSize = udptest.send_to(*addr, msg, static_cast<int>(str.length()));
	udptest.stop();
	return sendSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/12
*@purpose : recv udp and prints to console
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UDPRecvOnConsole(char *ip, int port)
{
	UNUSED(ip);
	UDPTest udptest;
	udptest.start(port);

	NetAddress *addr = new NetAddress();
	sockaddr_storage out;
	int out_addrlen;

	NetAddress::GetRemoteAddress(addr, (sockaddr*)&out, &out_addrlen, "10.8.128.15", "10084");
	std::string str = "hello";
	//char const *chararr = str.c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UDPListen(void *data)
{
	UNUSED(data);
}
