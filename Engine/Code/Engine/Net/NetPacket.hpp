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
	uint8_t senderConnectionIndex; // conn idx of the sender of this packer
	uint8_t unreliableCount;	   // number of unreliable messages in this container;
};
class NetPacket : public BytePacker
{

public:
	//Member_Variables
	char m_localBuffer[PACKET_MTU];
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