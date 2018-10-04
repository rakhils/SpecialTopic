#include "Engine//Net/NetAddress.hpp"
#include <string>
#pragma  comment(lib,"ws2_32.lib") // winsock libraries
class socket_type;
typedef socket_type* socket_t;
//#include "Engine/DevConsole/DevConsole.hpp"
/*
#define BIT_FLAG(f)   (1U << (f))
enum eSocketOptionBit : int
{
	SOCKET_OPTION_BLOCKING = BIT_FLAG(0),

	// some other options that may be useful to support
	// SOCKET_OPTION_BROADCAST - socket can broadcast messages (if supported by network)
	// SOCKET_OPTION_LINGER - wait on close to make sure all data is sent
	// SOCKET_OPTION_NO_DELAY - disable nagle's algorithm
};
typedef int eSocketOptions;*/

// Rip out things from TCPSocket and put it to a base leve; 
class Socket
{
public:
	Socket();
	virtual ~Socket(); // closes

	void Start();
	void Stop();

	void SetBlocking(bool blocking);

	bool Close(); // closesocket and set m_handle = INVALID_SOCKET
	bool IsClosed() const; // m_handle == (socket_t)INVALID_SOCKET; 

	NetAddress const& get_address() const;

protected:


public:
	NetAddress m_address; // address assocated with this socket; 
	socket_t   m_handle; // initialized to INVALID_SOCKET

					   // used if you want to set options while closed, 
					   // used to keep track so you can apply_options() after the socket
					   // gets created; 
	//eSocketOptions m_options;
};

// fatal errors cause the socket to close; 
bool IsFatalSocketError(int errorCode);

// Make appropriate changes to TCPSocket if you want
// though nothing *required* in class will use TCPSocket again.

// net/udpsocket.hpp

// when searching for addresses - you are no longer looking for AF_INET
// you look for 

class UDP1Socket : public Socket
{
public:

	bool bind(NetAddress &addr, // address I want to listen for traffic on
		uint16_t port_range = 0U);		  // how many additional ports to bind on (so you bind from [addr.port,addr.port + port_range])

										  // return 0 on failure, otherwise how large is the next datagram
										  // TODO in class
	size_t send_to(NetAddress &addr, void const *data, size_t const byte_count);
	size_t receive_from(NetAddress *out_addr, void *buffer, size_t max_read_size);
};




// class test
#define GAME_PORT 10084
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Header Size: 20B-60B
// UDP Header Size: 8B 
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)

