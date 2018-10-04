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
	
	//Static_Member_Variables

	//Methods

	UDPSocket(NetAddress &address);
	UDPSocket(int port);
	~UDPSocket();


	bool   Bind(NetAddress &addr,uint16_t port_range = 0U);		  
	size_t SendTo(NetAddress &addr, void *data, size_t const byte_count);
	size_t ReceiveFrom(void *buffer, size_t max_read_size,NetAddress *addr);
	
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