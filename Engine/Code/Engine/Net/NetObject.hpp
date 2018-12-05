#pragma once
#include "Engine/Net/NetObjectType.hpp"
#include "Engine/Math/Vector2.hpp"
/*\class  : NetObject	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/2/2018 7:08:16 PM
* \contact: srsrakhil@gmail.com
*/
class NetMessage;
class NetObject
{

public:
	//Member_Variables
	uint8_t		    m_objectID;
	uint8_t		    m_networkID = 0;
	NetObjectType   m_netObjectType;
	//NetMessage *    m_netMsg				 = nullptr;
	void *		    m_localSnapShotPointer   = nullptr; // pointer to player
	void *		    m_latestReceivedSnapshot = nullptr; //pointer to latest recvd snapshot of player
	//Static_Member_Variables

	//Methods

	NetObject();
	NetObject(size_t size);
	~NetObject();
	
	void *		GetMyLocalSnapShot();		 // host side
	void *		GetLatestReceivedSnapshot(); // client side
	void		UpdateAgeInAllNetObjectViews();

	//void		UpdatePlayerSnapshot(void *)
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