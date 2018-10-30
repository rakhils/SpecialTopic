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
 
class PacketTracker
{

public:
	//Member_Variables
	uint64_t m_sendTime;
	uint16_t m_ackID;
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