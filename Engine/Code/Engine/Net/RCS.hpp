#pragma once
#include <map>
#include <vector>
#include <string>
/*\class  : RCS	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/4/2018 5:16:54 PM
* \contact: srsrakhil@gmail.com
*/
class TCPSocket;
class BytePacker;
class TCPServer;
enum RCS_State
{
	RCS_STATE_CLIENT,
	RCS_STATE_CLIENT_FAILED,
	RCS_STATE_CLIENT_FAILED_SUPPLIED_ADDRESS,
	RCS_STATE_HOST,
	RCS_STATE_HOST_FAILED
};
class RCS
{

public:
	//Member_Variables
	std::string							m_ipaddress;
	int									m_rcsPort  = 29283;
	RCS_State							m_state;
	std::vector<TCPSocket*>				m_tcpSocketArray;
	TCPServer*							m_tcpServer		= nullptr;
	float								m_maxDelay		= 5.f;
	float								m_currentDelay  = 0.f;
	bool								m_isHookedToDevConsole = true;
//	TCPSocket*							m_tcpListenSocket;

	//Static_Member_Variables
	static RCS * s_theRCS;
	//Methods
	RCS();
	~RCS();
	
	//Static_Methods
	void Initialize();
	bool Join();
	bool Host();
	void Update(float deltaTime);
	void DisconnectAndCleanUpAllConnections();

	void FailedToHost();
	void ResetDelay();

	void ConstructBytePacketer(BytePacker *bp,void *data,size_t size);
	void RecvMsg();
	bool SendMsg(int idx, bool isEcho, char const *str);
	bool SendMsg(bool isEcho, char const *str);
	void ProcessConnection(TCPSocket* tcp);
	bool ProcessMessage(TCPSocket *socket, BytePacker *data);
	//void ProcessRCSRecvMsg(TCPSocket *socket, bool echo, std::string str);
	
	void         PushNewConnection(TCPSocket *socket);
	TCPSocket *  GetConnectionByIndex(int index);

	static RCS * GetInstance();

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