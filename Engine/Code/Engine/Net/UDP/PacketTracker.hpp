#pragma once
#include "Engine/Net/NetSession.hpp"
/*\class  : PacketTracker	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 10/22/2018 6:51:27 PM
* \contact: srsrakhil@gmail.com
*/
#define MAX_RELIALBES_PER_PACKET 1
class PacketTracker
{

public:
	//Member_Variables
	double m_sendTime;
	uint16_t m_ackID;
	bool     m_valid = false;
	uint16_t m_reliableID;
	uint16_t m_sentReliableIDs[MAX_RELIALBES_PER_PACKET];
	unsigned int m_reliablesInPacket = 1;
	//Static_Member_Variables

	//Methods

	PacketTracker(uint16_t ack);
	~PacketTracker();
	
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