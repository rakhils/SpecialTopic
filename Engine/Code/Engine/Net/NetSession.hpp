#pragma once
#include <map>
#include "Engine/Net/Packet.hpp"
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
typedef void (*NetEventCB)(NetConnection *connection);
struct NetEvents 
{
	NetEventCB m_eventCB;
	void Subscibe(NetEventCB eventCB)
	{
		m_eventCB = eventCB;
	}

	void Trigger(NetConnection *connection)
	{
		(*m_eventCB)(connection);
	}
};
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
enum ESessionError
{
	SESSION_OK,
	SESSION_ERROR_USER,                 // user disconnected
	SESSION_ERROR_INTERNAL,             // socket error; 

	SESSION_ERROR_JOIN_DENIED,          // generic deny error (release)
	SESSION_ERROR_JOIN_DENIED_NOT_HOST, // debug - tried to join someone who isn't hosting
	SESSION_ERROR_JOIN_DENIED_CLOSED,   // debug - not in a listen state
	SESSION_ERROR_JOIN_DENIED_FULL,     // debug - session was full 
};
enum ESessionState
{
	SESSION_DISCONNECTED = 0,  // Session can be modified     
	SESSION_BOUND,             // Bound to a socket - can send and receive connectionless messages.  No connections exist
	SESSION_CONNECTING,        // Attempting to connecting - waiting for response from the host
	SESSION_JOINING,           // Has established a connection, waiting final setup information/join completion
	SESSION_READY,
	SESSION_CONNECTED
};
#define MAX_CONN_ID_LENGTH 16
#define DEFAULT_CONNECTION_TIMEOUT 5000
#define MAX_NET_TIME_DILATION (0.1f)

#define NETOBJ_PLAYER 		(10)
#define NETOBJ_BULLET 		(11)
#define NETOBJ_EXPLOSION 	(12)


class NetObjectSystem;
class NetSession
{

public:
	//Member_Variables
	int												m_blankCount = 0;
	std::vector<NetConnection*>						m_allConnections;
	std::map<int, NetConnection*>					m_boundConnections;
	std::vector<NetMessageDefinition>				m_netMessageCmdDefinition;
	std::vector<NetMessage*>						m_laggedMsgs;

	ESessionError									m_sessionError;
	ESessionState									m_sessionState;
	
	NetConnection *									m_hostConnection				= nullptr;
	NetConnection *									m_myConnection					= nullptr;
	NetObjectSystem	*								m_netObjectSystem				= nullptr;

	uint8_t											m_highestIndex					= 1;											
	bool											m_initMsgDefinition				= false;
	int												m_minHeaderSize					= 2;
	float											m_lossAmount					= 0.f;
	float											m_minLatency					= 0.f;
	float											m_maxLatency					= 0.f;
	float											m_sendRate						= 60.f;
	bool											m_sessionInfoVisible			= false;
	uint64_t										m_lastRecvdHostTimems			= 0;				// this is the time we received from the host + (RTT / 2)
	uint64_t										m_desiredClientTimems			= 0;				// Time we want the the client to eventually be
	uint64_t										m_currentClientTimems			= 0;				// what the client will actually report return
	float											m_dtp							= 0;

	NetEvents										m_join;
	NetEvents										m_leave;

	//Static_Member_Variables
	static NetSession *s_netSession;
	static int		   s_defaultPort;

	//Methods
	NetSession(int listenPort);
	~NetSession();

	void					  Init();
	void					  Update(float deltaTime);
	void					  UpdateSessionState();
	void					  UpdateConnections(float deltaTime);

	void					  RestartInPort(int index,int port);

	void					  Host(char const *id, uint16_t port);
	NetConnection*			  Join(char const *id, NetConnectionInfo &host_info);

	void					  AddIntoAllConnection(char const *id, NetConnectionInfo connectionInfo);
	void					  SetHeartBeatFrequency(float time);
	void					  SetSimulateLoss(float lossAmount);
	void					  SetSimulateLatency(float m_minLatency, float m_maxLatency);
	void					  SetError(ESessionError error) { m_sessionError = error; }
	void					  ClearSessionError()			{ m_sessionError = SESSION_OK; }
	ESessionError			  GetLastSessionError();
	uint8_t					  GetAndIncrementConnectionIndex();
	void					  RemoveConnections(NetConnection *connection);
	bool					  IsHost();
	uint64_t				  GetNetTimems() { return m_currentClientTimems;}

	void					  InitMsgDefinition();
	void					  RegisterMessage(std::string id, NetMessageCB netMsgCB,std::string description);
	void					  RegisterMessage(eNetCoreMessage coreMsg, std::string name, NetMessageCB callback, std::string desc, eNetMessageOption flag = (eNetMessageOption)0);
	NetMessageDefinition *    GetMsgDefinition(std::string cmdid);
	NetMessageDefinition *    GetMsgDefinition(int index);
	int						  GetMsgIndex(std::string cmdname);
	std::string				  GetMsgName(int index);
	NetObjectSystem *		  GetNetObjectSystem();
	bool					  VerifyMessage(std::string cmd,std::string msg);
							  
	void					  ProcessIncomingMessage();
	void					  ProcessOutgoingMessage();

	size_t					  SendPacket(Packet packet);
	void					  BroadcastMsg(NetMessage *msg,EConnectionState state);
	//virtual void			   Send(uint16_t idx, char *data, size_t length);
	bool					  DoesConnectionExist(NetAddress address);
	void					  AddBinding(int port);
	NetConnection*			  CreateAndPushIntoAllConnection(NetConnectionInfo &connectionInfo);
	NetConnection*			  CreateHostConnection(NetConnectionInfo &connectioninfo,int port);
	NetConnection*			  CreateConnection(NetConnectionInfo &connectionInfo);
	void					  DestroyConnection(NetConnection *connection);

	NetConnection*			  AddConnection(int index,std::string ip, int port);
	NetConnection*			  AddConnection(int index,NetAddress *netaddress);
	NetConnection*			  GetConnection(int index);
	NetConnection*			  GetConnection(NetAddress *netAddress);
	NetConnection*			  GetConnectionFromAllConnections(NetAddress *address);
	bool					  RemoveFromAllConnections(NetAddress  m_address);
	void					  ReplaceConnectionWithNewIndex(uint8_t index,NetConnection *connection);
							  
	NetConnection*			  GetMyConnection();
	int						  GetMySessionIndex();
	void					  CloseAllConnections();
							  
	std::vector<NetMessage*>  ConstructMsgFromData(NetAddress &netAddress,size_t size,void *data);
	void					  ProcessMsgs(std::vector<NetMessage *> netmsg,NetAddress *address);
	void					  ProcessMsg(NetMessage *msg, NetAddress *address);

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

bool OnPing				(NetMessage  &netMsg, NetAddress &netAddress);
bool OnAdd				(NetMessage  &netMsg, NetAddress &netAddress);
bool OnPong				(NetMessage  &netMsg, NetAddress &netAddress);
bool OnAddResponse		(NetMessage  &netMsg, NetAddress &netAddress);
bool OnHeartBeatMessage (NetMessage  &netMsg, NetAddress &netAddress);
bool OnBadMessage		(NetMessage  &netMsg, NetAddress &netAddress);
bool OnBlankMessage		(NetMessage  &netMsg, NetAddress &netAddress);

// A07
bool OnJoinRequest		(NetMessage  &netMsg, NetAddress &netAddress);
bool OnJoinDeny			(NetMessage  &netMsg, NetAddress &netAddress);
bool OnJoinAccept		(NetMessage  &netMsg, NetAddress &netAddress);
bool OnNewConnection	(NetMessage  &netMsg, NetAddress &netAddress);
bool OnJoinFinished		(NetMessage  &netMsg, NetAddress &netAddress);
bool OnUpdateConnState  (NetMessage  &netMsg, NetAddress &netAddress);
bool OnHangUp			(NetMessage  &netMsg, NetAddress &netAddress);


