#pragma once
#include <vector>
#include <map>
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetMessage.hpp"

/*\class  : NetConnection		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :	   
* \author : Rakhil Soman	   
* \version: 1.0		   
* \date   : 9/16/2018 7:37:15 PM
* \contact: srsrakhil@gmail.com
*/
class UDPSocket;
class NetSession;
class PacketTracker;
constexpr uint16_t RELIABLE_WINDOW = 32;
#define MAX_ID_LENGTH 10
#define INVALID_SESSION_INDEX 255
enum EConnectionState
{
	CONNECTION_DISCONNECTED = 0,  // Session can be modified     
	CONNECTION_BOUND,             // Bound to a socket - can send and receive connectionless messages.  No connections exist
	CONNECTION_CONNECTING,        // Attempting to connecting - waiting for response from the host
	CONNECTION_JOINING,           // Has established a connection, waiting final setup information/join completion
	CONNECTION_READY,
	CONNECTION_CONNECTED
};
struct NetConnectionInfo
{
	uint8_t		m_sessionIndex = INVALID_SESSION_INDEX;
	NetAddress  m_address;
	char		m_id[MAX_ID_LENGTH];
	bool		m_isHost = false;
};
class NetObjectConnectionView;
class NetObjectView;
class NetConnection
{

public:
	//Member_Variables
	std::string					m_id;
	UDPSocket *					m_udpSocket;
	NetAddress					m_address;
	uint8_t						m_index;
	EConnectionState			m_connectionState;

	std::vector<NetMessage*>	m_unsentUnreliableMsgs;
	std::vector<NetMessage*>	m_unsentReliableMsgs;
	std::vector<NetMessage*>    m_unconfirmedReliableMessages;
	std::vector<NetMessage*>    m_laggedMsgs;
	std::vector<NetMessage*>    m_inboundMsgQueue;

	std::vector<NetObjectView*> m_netObjectViews;

	std::vector<uint16_t>       m_reliableIDs;
	std::vector<uint16_t>       m_sentReliableIDs;

	uint16_t					m_highestSentRealiableID = 0;
	uint16_t					m_highestRecvRealiableID = 0;

	uint16_t					m_oldestUnconfirmedReliableID = 0;
	uint16_t					m_oldestOrderedReliableMsgID = 0;
	bool						m_isReliable = false;
	BytePacker					m_packet;
	PacketHeader				m_header;
	NetSession *				m_session;
	double						m_lastHeartbeatReceivedTime = 0;
	double						m_lastHeartbeatTime  = 0;
	float						m_heartBeatFrequency = 0.5f;
	float						m_sendRate = 20;
	double					    m_startTime;

	double						m_joinInterval = .1;
	double						m_lastJoinReqstSentTime = 1;
	double						m_firstJoinReqstTime = 0;

	uint16_t					m_nextSentAck = 0U;

	uint16_t					m_lastReceivedAck = INVALID_PACKET_ACK;
	uint16_t					m_previousReceivedAckBitField = 0;

	float						m_loss				= 0.0f;
	float						m_rtt				= 0.0f;

	double						m_lastSendTime		= 0.f;
	double						m_lastReceivedTime  = 0.f;

	uint16_t					m_AckBitfieldMinPosition = 0;
	uint16_t					m_AckBitfieldMaxPosition = 15;

	std::map<uint16_t, PacketTracker*> m_trackerMap;
	uint16_t					m_trackerMaxCount = 128;
	int							m_trackerMinPosition = 0;
	int							m_trackerMaxPosition = m_trackerMaxCount;

	bool						m_isFirstHeartBeat;
	NetObjectConnectionView *   m_netObjectConnectionView = nullptr;
	//Static_Member_Variables

	//Methods

	NetConnection(int index,NetAddress netAddress); // As Client
	NetConnection(int listenPort);		
	NetConnection();					// As Server
	~NetConnection();

	void		 InitTracker();
	void		 BindConnection();
	void		 Disconnect();
	void		 HangUp();
	void		 CreateNetObjectConectionView();



	void		 SetLastHeartBeatReceivedTime(float time);
	void		 SetHeartBeatFrequency(float freq);
	void		 SetConnectionIndex(int index);
	void		 SetUnrealiableMsgCount(int count);
	void		 SetSendRate(float sendRate);
	void		 SetState(EConnectionState state);
	void		 SetLastRecvTime(float time);

	bool		 IsConnected()     { return m_connectionState >= CONNECTION_CONNECTED;}
	bool         IsDisconnected()  { return m_connectionState == CONNECTION_DISCONNECTED;}
	bool		 IsReady()		   { return m_connectionState == CONNECTION_READY;}
	bool         IsHost();
	bool		 IsMe();

	void		 IncrementSendAck();
	void		 IncrementSentRealiableID();
	// RELIABLE
	NetMessage * GetUnconfirmedReliableMsg(uint16_t reliableID);
	bool		 RemoveUnconfirmedReliableMsg(NetMessage *msg);
	void         DestroyNetMessage(NetMessage *msg);

	void		 AddReceivedRealiableID(uint16_t reliableID);
	bool		 HasReceivedReliableID(uint16_t reliableID);

	uint16_t	 GetNextSentReliableID();
	uint16_t	 GetOldestUnConfirmedRealiableID();

	// SENDING PART
	uint16_t	 GetNextPacketAck();
	bool		 CanSentUnconfirmedReliableMsg(NetMessage *msg);
	void		 ConfirmSentRealiable(uint16_t relID);
	bool		 CanSendNewReliableMessage(uint16_t nextReliableID);


	// ACK
	void		 ConfirmPacketReceived(uint16_t ack);
	void		 CalculateLoss();
	void		 PushToAckBitField(NetAddress *netaddress,uint16_t ack);
	void		 UpdateAckStatus(NetAddress *netConnection,uint16_t ackReceived);
	///////////////////////////////////////////////////////////////////
	//HEADER
	PacketHeader GetHeaderFromPacket(void *data);
	int			 GetCommandIndex(void *data,int size);
	int			 GetHeaderSize();
	
	void		 WriteConnectionIndex();
	void		 WriteReliableID();
	void		 WriteUnrealiableMsgCount();
	void		 WriteNextSentAck();
	void		 WriteLastReceivedAck();
	void		 WritePreviousReceviedAckBitField();
	void		 WriteHeader();
	///////////////////////////////////////////////////////////////////

	//PAYLOAD
	bool		 WritePayload(NetMessage *msg,NetAddress *address);
	///////////////////////////////////////////////////////////////////
	void		 SendHeartBeat(NetAddress *address);

	void		 Update(float deltaTime);
	void		 SortAndPushIntoNetObjectViews(NetObjectView *netObjectView);
	void         PushNetMsgFromNetObjectViews();
	size_t				SendDirect(NetAddress *address, void *data, size_t size);
	size_t				SendImmediately(int connectionIndex, std::vector<NetMessage*> &msgs);
	size_t				SendImmediately(NetMessage msg);
	void				Append(NetMessage *msg);
	PacketTracker *		AddTracker(uint16_t ack);
	PacketTracker *		GetTracker(uint16_t ack);
	size_t				FlushMsgs();
	size_t				FlushUnConfirmedReliables();
	size_t				FlushUnSentRealiables();
	size_t				FlushUnrealiables();
	size_t				Recv(char *data,size_t &maxlength,NetAddress *netAddress);
	
	std::string			GetIPPortAsString();

	void				PushToInboundMsgQueue(NetMessage* msg);
	void				DoProcessInboundMsgQueue();
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