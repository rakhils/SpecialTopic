#pragma once
#include <map>
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Math/MathUtil.hpp"
/*\class  : NetSession	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/16/2018 6:52:56 PM
* \contact: srsrakhil@gmail.com
*/

typedef bool (*NetMessageCB)(NetMessage &msg, NetAddress &from);
struct NetMessageDefinition
{
	NetMessageDefinition()
	{

	}
	NetMessageDefinition(std::string name, NetMessageCB callback, std::string description)
	{
		m_name = name;
		m_callback = callback;
		m_description = description;

	}
	NetMessageDefinition(eNetCoreMessage coreMsg, std::string name, NetMessageCB callback, std::string description, eNetMessageOption flag)
	{
		Init(coreMsg, name, callback, description, flag);
	}

	void Init(eNetCoreMessage coreMsg, std::string name, NetMessageCB callback, std::string description, eNetMessageOption flag)
	{
		m_name = name;
		m_callback = callback;
		m_description = description;
		m_coreMsg = coreMsg;
		m_options = flag;
	}
	std::string     m_name;
	NetMessageCB    m_callback;
	std::string     m_description;
	eNetCoreMessage m_coreMsg;
	eNetMessageOption m_options;
};
class NetSession
{

public:
	//Member_Variables
	std::map<int, NetConnection*>					m_remoteConnections;
	NetConnection *									m_channel      = nullptr;
												
	std::vector<NetMessageDefinition>				m_netMessageCmdDefinition;
	bool											m_initMsgDefinition = false;

	std::vector<NetMessage*>						m_laggedMsgs;
	int												m_minHeaderSize = 2;
	float											m_lossAmount = 0.f;
	float											m_minLatency = 0.f;
	float											m_maxLatency = 0.f;
	float											m_sendRate   = 0.f;
	bool											m_sessionInfoVisible = false;
	//Static_Member_Variables
	static NetSession *s_netSession;
	static int		   s_defaultPort;

	//Methods
	NetSession(int listenPort);
	~NetSession();

	void					  Init();
	void					  Update(float deltaTime);
	void					  UpdateConnections(float deltaTime);

	void					  RestartInPort(int port);

	void					  SetHeartBeatFrequency(float time);
	void					  SetSimulateLoss(float lossAmount);
	void					  SetSimulateLatency(float m_minLatency, float m_maxLatency);

	void					  InitMsgDefinition();
	void					  RegisterMessage(std::string id, NetMessageCB netMsgCB,std::string description);
	void					  RegisterMessage(eNetCoreMessage coreMsg, std::string name, NetMessageCB callback, std::string desc, eNetMessageOption flag = (eNetMessageOption)0);
	NetMessageDefinition *    GetMsgDefinition(std::string cmdid);
	NetMessageDefinition *    GetMsgDefinition(int index);
	int						  GetMsgIndex(std::string cmdname);
	std::string				  GetMsgName(int index);
	bool					  VerifyMessage(std::string cmd,std::string msg);
							  
	void					  ProcessIncomingMessage();
	void					  ProcessOutgoingMessage();
							  
	//virtual void			   Send(uint16_t idx, char *data, size_t length);
	void					  AddBinding(int port);
	NetConnection*			  AddConnection(int index,std::string ip, int port);
	NetConnection*			  AddConnection(int index,NetAddress *netaddress);
	NetConnection*			  GetConnection(int index);
	NetConnection*			  GetConnection(NetAddress *netAddress);
							  
	NetConnection*			  GetMyConnection();
	int						  GetMySessionIndex();
	void					  CloseAllConnections();
							  
	std::vector<NetMessage*>  ConstructMsgFromData(NetAddress &netAddress,size_t size,void *data);
	void					  ProcessMsg(std::vector<NetMessage *> netmsg,NetAddress *address);

	//void					  PushOutboundMsgs(NetAddress address,NetMessage *msg);

	void					  Render();
	void					  RenderSessionDetails();
	void					  RenderConnectionColumnDetails(Vector2 startPosition);
	void					  RenderConnectionDetails(NetConnection *connection,Vector2 startPOsitino,bool skipHeading);
	//Static_Methods

	static NetSession*		  GetInstance();

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

bool OnPing(NetMessage &netMsg, NetAddress &netAddress);
bool OnAdd(NetMessage  &netMsg, NetAddress &netAddress);
bool OnPong(NetMessage  &netMsg, NetAddress &netAddress);
bool OnAddResponse(NetMessage  &netMsg, NetAddress &netAddress);
bool OnHeartBeatMessage(NetMessage  &netMsg, NetAddress &netAddress);
bool OnBadMessage(NetMessage  &netMsg, NetAddress &netAddress);

