#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Net/NetObject.hpp"
#include "Engine/Net/NetObjectView.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetObjectConnectionView.hpp"
// CONSTRUCTOR
NetObjectSystem::NetObjectSystem()
{

}

// DESTRUCTOR
NetObjectSystem::~NetObjectSystem()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetObjectSystem::SyncObject(uint8_t objType, void *localPtr)
{
	NetObjectType type = FindByID(objType);

	NetObject *netObject				= new NetObject();
	netObject->m_networkID				= m_nextNetworkID;
	netObject->m_netObjectType			= type;
	netObject->m_localSnapShotPointer   = localPtr;

	m_netObjectMap[m_nextNetworkID] = netObject;
	m_netObjects.push_back(netObject);
	m_objectLookup[localPtr] = netObject;
	
	/*NetMessage * msg = NetMessage::CreateObjectCreateMsg(nullptr,objType,m_nextNetworkID);  

	NetSession::GetInstance()->BroadcastMsg(msg, CONNECTION_READY);*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetObjectSystem::RegisterType(NetObjectType const &type)
{
	m_netObjectTypeMap[type.m_id] = type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<NetMessage*> NetObjectSystem::GetCreateObjectMsgForAllObjects()
{
	std::vector<NetMessage*> netMsgs;
	/*for(size_t index = 0;index < m_netObjects.size();index++)
	{
		NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_PLAYER, m_netObjects.at(index)->m_networkID);
		netMsgs.push_back(msg);
	}*/
	return netMsgs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<NetMessage*> NetObjectSystem::GetUpdateObjectMsgForAllObjects()
{
	std::vector<NetMessage*> netMsgs;
	return netMsgs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetObjectType NetObjectSystem::FindByID(uint8_t id)
{
	return m_netObjectTypeMap[id];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetObject * NetObjectSystem::CreateNetObject(uint8_t objType, uint8_t idx)
{
	NetObjectType type					= FindByID(objType);

	NetObject *netObject				= new NetObject();
	netObject->m_networkID				= idx;
	netObject->m_netObjectType			= type;
	
	CreateAndPushNetObjectViews(netObject);

	m_netObjectMap[idx] = netObject;
	m_netObjects.push_back(netObject);

	return netObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetObjectSystem::CreateAndPushNetObjectViews(NetObject *netObject)
{
	std::map<int, NetConnection*>::iterator it = NetSession::GetInstance()->m_boundConnections.begin();
	for(;it != NetSession::GetInstance()->m_boundConnections.end();it++)
	{
		NetObjectView *netObjectView = new NetObjectView(netObject->m_objectID,netObject->m_networkID);
		it->second->m_netObjectConnectionView->PushNetObjectView(netObjectView);
		netObjectView->m_netObject = netObject;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetObjectSystem::RemoveNetObjectViews(NetObject *netObject)
{
	std::map<int, NetConnection*>::iterator it = NetSession::GetInstance()->m_boundConnections.begin();
	for (; it != NetSession::GetInstance()->m_boundConnections.end(); it++)
	{
		it->second->m_netObjectConnectionView->RemoveNetObjectView(netObject->m_objectID);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetObjectSystem::DestroyNetObject(NetObject *netObject)
{
	for(size_t index = 0;index < m_netObjects.size();index++)
	{
		if(m_netObjects.at(index) == netObject)
		{
			m_netObjects.erase(m_netObjects.begin() + index, m_netObjects.begin() + index + 1);
			break;
		}
	}
	RemoveNetObjectViews(netObject);
	std::map<uint8_t, NetObject*>::iterator it = m_netObjectMap.find(netObject->m_networkID);
	m_netObjectMap.erase(it);
	delete netObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool NetObjectSystem::DoesObjectExist(uint8_t id)
{
	std::map<uint8_t, NetObject*>::iterator it = m_netObjectMap.find(id);
	if(it == m_netObjectMap.end())
	{
		return false;
	}
	return true;
}
