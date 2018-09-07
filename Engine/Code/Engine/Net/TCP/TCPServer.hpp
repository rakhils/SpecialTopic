#pragma once
#include "Engine/Net/NetAddress.hpp"
/*\class  : TCPServer		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/24/2018 7:07:01 PM
* \contact: srsrakhil@gmail.com
*/
 
class TCPServer
{

public:
	//Member_Variables
	bool m_blocking = false;
	bool m_isListening = true;
	int  m_port = 0;
	bool m_isDisconnected = false;
	std::string m_ipaddress;
	SOCKET m_socket;
	//Static_Member_Variablesk

	//Methods

	TCPServer();
	TCPServer(int port);
	~TCPServer();

	void Listen(int port);
	void Listen();
	void Disconnect();
	bool   HasFatalError();
	std::string GetIp();
	//Static_Methods
	static void ListenOnThread(void *data);

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