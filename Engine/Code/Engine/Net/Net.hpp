#pragma once
// can have engine WindowsCommon.h which includes all the below . 

#include <WinSock2.h> // before inculding windows.h
#include <WS2tcpip.h> // FOR IPV6
#include <Windows.h>
#include <string>
#pragma  comment(lib,"ws2_32.lib") // winsock libraries
/*\class  : Net		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 8/20/2018 2:44:42 PM
* \contact: srsrakhil@gmail.com
*/
 
class Net
{

public:
	//Member_Variables
	
	//Static_Member_Variables
	static std::string m_ipaddr;
	//Methods

	Net();
	~Net();
	
	//Static_Methods
	static bool StartUp();
	static bool ShutDown();

	static void Connect(char *ip,char *port);
	static bool GetHostAddress(sockaddr *out, int *out_addrlen, char const *hostname, char const *service = "123");
	static bool GetBindableAddress(sockaddr *out,int *out_addrlen,int port);
	static std::string GetIP();
	//static void SendData(char *ip,char *port,)

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