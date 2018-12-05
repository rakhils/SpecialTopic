#pragma once
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetObjectType.hpp"
/*\class  : NetObjectSystem		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 12/2/2018 7:05:59 PM
* \contact: srsrakhil@gmail.com
*/
class NetObject;
class NetObjectSystem
{

public:
	//Member_Variables
	uint8_t m_nextNetworkID = 0;
	std::vector<NetObject*>			m_netObjects; 				
	std::map<uint8_t, NetObject*>   m_netObjectMap;
	std::map<void*, NetObject*>		m_objectLookup;

	std::map<uint8_t, NetObjectType> m_netObjectTypeMap;
	//Static_Member_Variables

	//Methods

	NetObjectSystem();
	~NetObjectSystem();

	//void * GetObjectSnapshot(uint8_t objType, uint8_t objectID);

	void SyncObject(uint8_t objType, void *localPtr);
	void RegisterType(NetObjectType const &type);

	std::vector<NetMessage*> GetCreateObjectMsgForAllObjects();
	std::vector<NetMessage*> GetUpdateObjectMsgForAllObjects();
	NetMessage *			 GetObjectDestroyMessage(uint8_t id);
	NetObjectType			 FindByID(uint8_t id);
	NetObject *				 CreateNetObject(uint8_t type, uint8_t idx);

	void					 DestroyNetObject(NetObject *netObject);
	
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