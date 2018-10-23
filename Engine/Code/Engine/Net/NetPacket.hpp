#pragma once
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Net/NetMessage.hpp"
/*\class  : NetPacket		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 9/23/2018 10:38:09 AM
* \contact: srsrakhil@gmail.com
*/
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

struct PacketHeader
{
	uint8_t m_senderConnectionIndex; // conn idx of the sender of this packer
	uint8_t m_unreliableCount;	   // number of unreliable messages in this container;

	uint16_t m_ack;
	uint16_t m_lastReceivedAck;
	uint16_t m_previousReceivedAckBitfield;
};
class NetPacket : public BytePacker
{

public:
	//Member_Variables
	char m_localBuffer[PACKET_MTU];

	/*uint16_t m_next_sent_ack = 0U;

	// receiving - updated during a process_packet
	uint16_t m_last_received_ack = INVALID_PACKET_ACK;
	uint16_t m_previous_received_ack_bitfield = 0;

	// Analytics
	//uint m_last_send_time_ms;
	//uint m_last_received_time_ms;

	// note these variables are unrelated to the debug sim on the session
	// but will end up reflecting those numbers.
	float m_loss = 0.0f;       // loss rate we perceive to this connection
	float m_rtt = 0.0f;      */  // latency perceived on this connection



	//Static_Member_Variables

	//Methods

	NetPacket();
	~NetPacket();

	bool WriteHeader(PacketHeader &msg);
	bool ReadHeader(PacketHeader &msg);

	bool WriteMessage(NetMessage &msg);
	bool ReadMessage(NetMessage *outmsg);


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