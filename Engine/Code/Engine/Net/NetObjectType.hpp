#pragma once
#include <stdint.h>

/*\class  : NetObjectType		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/3/2018 12:21:57 AM
* \contact: srsrakhil@gmail.com
*/
class NetMessage;

typedef void (*GameEvents)(NetMessage *msg, void *objPtr);

class NetObjectType
{

public:
	//Member_Variables
	uint8_t m_id;
	size_t m_snapshotSize;
	GameEvents m_sendCreate;
	GameEvents m_recvCreate;
	GameEvents m_sendDestroy;
	GameEvents m_recvDestroy;
						 
	GameEvents m_getSnapshot;
	GameEvents m_sendSnapshot;
	GameEvents m_recvSnapshot;
	GameEvents m_applySnapshot;

	//Static_Member_Variables

	//Methods

	NetObjectType();
	void operator=(const NetObjectType &type);
	~NetObjectType();
	
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