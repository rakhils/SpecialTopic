#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/Socket.hpp"

class UDPTest
{
public:
	UDPTest();
	~UDPTest();

	bool start(int port);

	void stop();

	size_t send_to(NetAddress &addr, void const *buffer, int byte_count);

	void Update();

public:
	// if you have multiple address, you can use multiple sockets
	// but you have to be clear on which one you're sending from; 
	UDP1Socket m_socket;
};

size_t UDPSend(const char *ip, int port, const char* msg);
void UDPRecvOnConsole(char *ip, int port);
void UDPListen(void *data);