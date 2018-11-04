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
#define INVALID_PACKET_ACK (0xffff)
class UDPSocket;
class NetSession;
class PacketTracker;
struct UDPHeader
{
	uint8_t m_connectionindex  = static_cast<uint8_t>(-1);
	uint16_t m_ack			   = static_cast<uint16_t>(0U);
	uint16_t m_lastReceivedAck = INVALID_PACKET_ACK;
	uint16_t m_previousReceivedAckBitfield = static_cast<uint8_t>(0U);
	uint8_t m_unrealiableCount = static_cast<uint8_t>(0U);

};
class NetConnection
{

public:
	//Member_Variables
	UDPSocket *					m_udpSocket;
	NetAddress					m_address;
	int							m_index;
	std::vector<NetMessage*>	m_outboundMsgs;
	BytePacker					m_packet;
	UDPHeader					m_header;
	NetSession *				m_session;
	float						m_lastHeartbeatReceivedTime;
	float						m_lastHeartbeatHPC;
	float						m_heartBeatFrequency = 0.25;
	float						m_sendRate			 = 20;
	uint64_t					m_startTime;

	uint16_t					m_nextSentAck = 0U;

	uint16_t					m_lastReceivedAck = INVALID_PACKET_ACK;
	uint16_t					m_previousReceivedAckBitField = 0;

	float						m_loss				= 0.0f;
	float						m_rtt				= 0.0f;

	float						m_lastSendTime		= 0.f;
	float						m_lastReceivedTime  = 0.f;

	float						m_AckBitfieldMinPosition = 0;
	float						m_AckBitfieldMaxPosition = 15;

	std::map<uint16_t, PacketTracker*> m_trackerMap;
	uint16_t					m_trackerMaxCount = 16;
	int							m_trackerMinPosition = 0;
	int							m_trackerMaxPosition = m_trackerMaxCount;



	//Static_Member_Variables

	//Methods

	NetConnection(int index,NetAddress netAddress); // As Client
	NetConnection(int listenPort);					// As Server
	~NetConnection();

	void InitTracker();


	void  SetLastHeartBeatReceivedTime(float time);
	void  SetHeartBeatFrequency(float freq);
	void  SetConnectionIndex(int index);
	void  SetUnrealiableMsgCount(int count);
	void  SetSendRate(float sendRate);

	void IncrementSendAck();

	// ACK
	void  ConfirmPacketReceived(uint16_t ack);
	void  CalculateLoss();
	void  PushToAckBitField(NetAddress *netaddress,uint16_t ack);
	void  UpdateAckStatus(NetAddress *netConnection,uint16_t ackReceived);
	///////////////////////////////////////////////////////////////////
	//HEADER
	UDPHeader GetHeaderFromPacket(void *data);
	int       GetCommandIndex(void *data,int size);
	int    GetHeaderSize();
	void   WriteConnectionIndex();
	void   WriteUnrealiableMsgCount();
	void   WriteNextSentAck();
	void   WriteLastReceivedAck();
	void   WritePreviousReceviedAckBitField();
	void   WriteHeader();
	///////////////////////////////////////////////////////////////////

	//PAYLOAD
	bool   WritePayload(NetMessage *msg,NetAddress *address);
	///////////////////////////////////////////////////////////////////
	void   SendHeartBeat(NetAddress *address);

	void   Update(float deltaTime);

	size_t SendImmediately(int connectionIndex, std::vector<NetMessage*> &msgs);
	size_t SendImmediately(NetMessage msg);
	void   Append(NetMessage *msg);
	void   AddTracker(uint16_t ack);
	PacketTracker * GetTracker(uint16_t ack);
	size_t FlushMsgs();
	size_t Recv(char *data,size_t &maxlength,NetAddress *netAddress);
	
	std::string GetIPPortAsString();
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