#pragma once
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetAddress.hpp"
/*\class  : Packet		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 11/7/2018 7:17:07 PM
* \contact: srsrakhil@gmail.com
*/
#define ETHERNET_MTU 1500 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 
class Packet
{

public:
	//Member_Variables
	PacketHeader m_header;
	BytePacker   m_packet;
	NetAddress   *m_address = nullptr;
	//Static_Member_Variables

	//Methods

	Packet();
	~Packet();

	size_t GetHeaderSize();

	void SetConnectionIndex					(uint8_t index);
	//void SetReliableID					(uint16_t reliableID);
	void SetNextSentAck						(uint16_t sentAck);
	void SetLastReceivedAck					(uint16_t lastRecvAck);
	void SetPreviousReceviedAckBitField		(uint16_t prevRecvAck);
	void SetUnrealiableMsgCount				(uint8_t msgCount);

	void WriteConnectionIndex				(uint8_t index);
	//void WriteReliableID					(uint16_t reliableID);
	void WriteNextSentAck					(uint16_t sentAck);
	void WriteLastReceivedAck				(uint16_t lastRecvAck);
	void WritePreviousReceviedAckBitField   (uint16_t prevRecvAck);
	void WriteUnrealiableMsgCount		    (uint8_t msgCount);

	bool WritePacketHeader();
	bool WritePacketHeader(PacketHeader header);
	bool WriteMsgHeader(MsgHeader header);

	bool WriteMsg(NetMessage *msg);
	bool WriteNextMsg(NetMessage *msg);

	bool HasRoom(size_t size);
	
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