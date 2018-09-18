#pragma once
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/Socket.hpp"
/*\class  : UDPSocket	   
* \group  : <GroupName>		   
* \brief  :		   
* TODO:  :		   
* \note   :	   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/10/2018 2:41:24 PM
* \contact: srsrakhil@gmail.com
*/

class UDPSocket : public Socket
{

public:
	//Member_Variables
	NetAddress m_netaddr;
	//Static_Member_Variables

	//Methods

	UDPSocket(char *address);
	UDPSocket(int port);
	~UDPSocket();


	bool   Bind(NetAddress &addr,uint16_t port_range = 0U);		  
	size_t SendTo(NetAddress &addr, void const *data, size_t const byte_count);
	size_t ReceiveFrom(NetAddress *out_addr, void *buffer, size_t max_read_size);
	
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};