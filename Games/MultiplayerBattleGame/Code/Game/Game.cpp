#include "Game.hpp"
#include "Engine/Net/Socket.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetObjectType.hpp"
#include "Engine/Net/NetObjectSystem.hpp"
#include "Engine/Net/NetObject.hpp"
#include "Game/GamePlay/Entity/Player.hpp"
#include "Game/GamePlay/Entity/Bullets.hpp"
Game *Game::s_game = nullptr;

Game::Game()
{
	
}

Game::~Game()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : Creates player
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Player* Game::CreatePlayer(uint8_t idx, char const *name,Vector2 position,float angle)
{
	DevConsole::GetInstance()->PushToOutputText("CREATING PLAYER ID " + ToString(idx));
	Player *player   = new Player(Vector2(GetRandomFloatInRange(0,m_mapWidth),GetRandomFloatInRange(0,m_mapHeight)));
	player->m_name   = name;
	player->m_index  = idx;
	m_playerMap[idx] = player;
	player->SetPosition(position);
	player->SetDirection(angle);

	NetObject* netObject = NetSession::GetInstance()->GetNetObjectSystem()->CreateNetObject(NETOBJ_PLAYER, idx);
	player->m_netObject  = netObject;
	netObject->m_latestReceivedSnapshot = player->m_localEntitySnapshot;

	return player;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Bullets* Game::CreateBullet(uint8_t playerID,uint8_t idx,Vector2 position,float angle)
{
	Bullets *bullet  = new Bullets(position,angle,1);
	bullet->m_index  = idx;
	bullet->m_position = position;
	bullet->m_angle = angle;
	bullet->m_playerID = playerID;
	m_bulletMap[idx] = bullet;

	NetObject* netObject = NetSession::GetInstance()->GetNetObjectSystem()->CreateNetObject(NETOBJ_BULLET, idx);
	bullet->m_netObject = netObject;
	return bullet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Explosion* Game::CreateExplosion(uint8_t idx, Vector2 position)
{
	/*Explosion *explosion = new Explosion(position);
	m_explosions[idx] = explosion;

	NetObject* netObject = NetSession::GetInstance()->GetNetObjectSystem()->CreateNetObject(NETOBJ_EXPLOSION, idx);
	explosion->m_netObject = netObject;

	if (NetSession::GetInstance()->m_hostConnection->IsHost())
	{

		std::map<int, NetConnection*>::iterator it;
		for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
		{
			NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_EXPLOSION, idx, it->second->m_index);
			msg->WriteBytes(4, (void*)&position.x);
			msg->WriteBytes(4, (void*)&position.y);

			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}
		DevConsole::GetInstance()->PushToOutputText("BULLET CREATED AT HOST IDX " + ToString(idx));
		return explosion;
	}
	DevConsole::GetInstance()->PushToOutputText("BULLET CREATED AT CLIENT IDX " + ToString(idx));
	return explosion;*/
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::BroadcastCreatePlayerMsg(uint8_t idx, Vector2 position, float angle)
{
	if (NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		std::map<int, NetConnection*>::iterator it;
		for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
		{
			NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_PLAYER, idx, it->second->m_index);
			msg->WriteBytes(4, (void*)&position.x);
			msg->WriteBytes(4, (void*)&position.y);
			msg->WriteBytes(4, (void*)&angle);

			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}
		DevConsole::GetInstance()->PushToOutputText("PLAYER CREATED AT HOST IDX " + ToString(idx));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::BroadcastCreateBulletMsg(uint8_t playerID,Vector2 position,float angle)
{
	if (NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		uint8_t idx = GetUniqueNonPlayerID();
		std::map<int, NetConnection*>::iterator it;
		for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
		{
			NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_BULLET, idx, it->second->m_index);
			msg->WriteBytes(4, (void*)&position.x);
			msg->WriteBytes(4, (void*)&position.y);
			msg->WriteBytes(4, (void*)&angle);
			msg->WriteBytes(1, (void*)&playerID);

			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}
		//DevConsole::GetInstance()->PushToOutputText("BULLET CREATED AT HOST IDX AND BROADCASTED" + ToString(idx));
		Game::GetInstance()->CreateBullet(playerID,idx, position, angle);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::BroadcastDestroyPlayerMsg(uint8_t idx)
{
	std::map<uint8_t, Player*>::iterator it = m_playerMap.begin();
	for (; it != m_playerMap.end(); it++)
	{
		std::map<int, NetConnection*>::iterator itcon;
		for (itcon = NetSession::GetInstance()->m_boundConnections.begin(); itcon != NetSession::GetInstance()->m_boundConnections.end(); itcon++)
		{
			NetMessage *msg = NetMessage::CreateObjectDestroyMsg(NETOBJ_PLAYER, it->second->m_index, itcon->second->m_index);
			msg->SetAddress(&itcon->second->m_address);
			itcon->second->Append(msg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/13
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::BroadcastDestroyBulletMsg(uint8_t idx)
{
	std::map<int, NetConnection*>::iterator itcon;
	for (itcon = NetSession::GetInstance()->m_boundConnections.begin(); itcon != NetSession::GetInstance()->m_boundConnections.end(); itcon++)
	{
  		NetMessage *msg = NetMessage::CreateObjectDestroyMsg(NETOBJ_BULLET, idx, itcon->second->m_index);
		//DevConsole::GetInstance()->PushToOutputText("BROADCAST DESTR BULLET REL ID " + ToString(msg->m_reliableID), Rgba::GREEN);

		msg->SetAddress(&itcon->second->m_address);
		itcon->second->Append(msg);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/14
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Game::IsPositionOutOfBounds(Vector2 position)
{
	if(position .x < 0 || position .y < 0)
	{
		return true;
	}
	if(position.x > Windows::GetInstance()->GetDimensions().x || position.y > Windows::GetInstance()->GetDimensions().y)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Game::GetUniqueNonPlayerID()
{
	return m_largestNonPlayerID++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DestroyPlayer(uint8_t playerID)
{
	std::map<uint8_t, Player*>::iterator it = m_playerMap.find(playerID);
	if(it == m_playerMap.end())
	{
		return;
	}
	Vector2 playerPosition = it->second->m_position;
	NetSession::GetInstance()->GetNetObjectSystem()->DestroyNetObject(it->second->m_netObject);
	delete it->second;
	m_playerMap.erase(it);
	Explosion *explosion = new Explosion(playerPosition);
	m_explosions.push_back(explosion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DestroyBullet(uint8_t bulletID)
{
	std::map<uint8_t, Bullets*>::iterator it = m_bulletMap.find(bulletID);
	if(it == m_bulletMap.end())
	{
		return;
	}
	NetSession::GetInstance()->GetNetObjectSystem()->DestroyNetObject(it->second->m_netObject);
	delete it->second;
	m_bulletMap.erase(it);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DestroyAllPlayers()
{
	std::map<uint8_t, Player*>::iterator it = m_playerMap.begin();
	for(;it!= m_playerMap.end();it++)
	{
		NetSession::GetInstance()->GetNetObjectSystem()->DestroyNetObject(it->second->m_netObject);
		//delete it->second;
	}
	m_playerMap.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/06
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::DestroyAllBullets()
{
	std::map<uint8_t, Bullets*>::iterator it = m_bulletMap.begin();
	for (; it != m_bulletMap.end(); it++)
	{
		NetSession::GetInstance()->GetNetObjectSystem()->DestroyNetObject(it->second->m_netObject);
		//delete it->second;
	}
	m_bulletMap.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : Setups network
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::SetupNetwork()
{
	NetObjectType playerType;
	playerType.m_id = NETOBJ_PLAYER;
	playerType.m_sendCreate		= SendPlayerCreate;
	playerType.m_recvCreate		= RecvPlayerCreate;
	playerType.m_sendDestroy	= SendPlayerDestroy;
	playerType.m_recvDestroy	= RecvPlayerDestroy;

	// snapshotm_snapshotSize;
	playerType.m_snapshotSize	= sizeof(EntitySnapShot_t);
	playerType.m_getSnapshot	= PlayerGetSnapshot;
	playerType.m_sendSnapshot	= PlayerSendSnapshot;
	playerType.m_recvSnapshot	= PlayerRecvSnapshot;
	playerType.m_applySnapshot	= PlayerApplySnapshot;

	NetObjectSystem *nos = NetSession::GetInstance()->GetNetObjectSystem();
	nos->RegisterType(playerType);


	NetSession::GetInstance()->m_join.Subscibe (OnConnectionJoin);
	NetSession::GetInstance()->m_leave.Subscibe(OnConnectionLeave);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Registers game net message
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RegisterGameMessage()
{
	NetSession::GetInstance()->RegisterMessage((eNetCoreMessage)NETMSG_UNRELIABLE_TEST, "unreliable_test", OnUnreliableTest,
		"TEST AN UNRELIABLE MSGS");
	NetSession::GetInstance()->RegisterMessage((eNetCoreMessage)NETMSG_RELIABLE_TEST,   "reliable_test",   OnReliableTest,
		"TEST AN RELIABLE MSGS",NETMESSAGE_OPTION_RELIABLE);

	NetSession::GetInstance()->RegisterMessage((eNetCoreMessage)NETMSG_OBJECT_CREATE, "creates_game_object", OnObjectCreated,
		"TEST AN RELIABLE MSGS", NETMESSAGE_OPTION_RELIALBE_IN_ORDER);
	NetSession::GetInstance()->RegisterMessage((eNetCoreMessage)NETMSG_OBJECT_UPDATE, "updates_game_object", OnObjectUpdate,
		"TEST AN RELIABLE MSGS", NETMESSAGE_OPTION_UNRELIABLE);
	NetSession::GetInstance()->RegisterMessage((eNetCoreMessage)NETMSG_OBJECT_DESTROY, "destroys_game_object", OnObjectDestroyed,
		"TEST AN RELIABLE MSGS", NETMESSAGE_OPTION_RELIALBE_IN_ORDER);
	SetupNetwork();
}

// INIT GAME
void Game::Initialize()
{
	InitCamera();
	InitMainMenuItems();
	RegisterGameMessage();
	m_gameMode = MAIN_MENU;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Inits main menu items
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitMainMenuItems()
{
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Inits main camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_camera = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateGame(float deltaTime)
{
	UpdatePlayer(deltaTime);
	UpdateBullet(deltaTime);
	UpdateExplosions(deltaTime);
	UpdatePlayerBulletCollision();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdatePlayer(float deltaTime)
{
	std::map<uint8_t, Player*>::iterator it = m_playerMap.begin();
	for (; it != m_playerMap.end(); it++)
	{
		it->second->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateBullet(float deltaTime)
{
	std::map<uint8_t, Bullets*>::iterator it = m_bulletMap.begin();
	for (; it != m_bulletMap.end(); it++)
	{
		it->second->Update(deltaTime);
		if (NetSession::GetInstance()->m_hostConnection->IsHost())
		{
			if (it->second->m_spawnTime + it->second->m_lifeTime < static_cast<float>(GetCurrentTimeSeconds()))
			{
				it->second->m_markAsDead = true;
			
				Game::GetInstance()->BroadcastDestroyBulletMsg(it->second->m_index);

				//NetSession::GetInstance()->BroadcastMsg(msg, CONNECTION_READY);
				delete it->second;
				m_bulletMap.erase(it);
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateExplosions(float deltaTime)
{
	for (size_t explosionIndex = 0;explosionIndex< m_explosions.size();explosionIndex++)
	{
		m_explosions.at(explosionIndex)->Update(deltaTime);
		if(m_explosions.at(explosionIndex)->m_markForDelete)
		{
			m_explosions.erase(m_explosions.begin() + explosionIndex, m_explosions.begin() + explosionIndex + 1);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdatePlayerBulletCollision()
{
	if(NetSession::GetInstance()->m_hostConnection != nullptr && !NetSession::GetInstance()->m_hostConnection->IsHost())
	{
		return;
	}
	std::map<uint8_t, Player*>::iterator itPlayerMap  = m_playerMap.begin();
	std::map<uint8_t, Bullets*>::iterator itBulletMap = m_bulletMap.begin();
	bool doBreak = false;
	for (; itPlayerMap != m_playerMap.end(); itPlayerMap++)
	{
		for (itBulletMap = m_bulletMap.begin(); itBulletMap != m_bulletMap.end(); itBulletMap++)
		{
			if (itBulletMap->second->m_playerID == itPlayerMap->second->m_index)
			{
				//DevConsole::GetInstance()->PushToOutputText(" NOT HIT PLAYER ID "+ToString(itPlayerMap->second->m_index)+" BULET ID "+ToString(itBulletMap->second->m_playerID));
				continue;
			}
			Disc2 playerDisc(itPlayerMap->second->m_position, 50);
			Disc2 bulletDisc(itBulletMap->second->m_position, 20);
			//DevConsole::GetInstance()->PushToOutputText(" ## PLAYER ID " + ToString(itPlayerMap->second->m_index) + " BULET ID " + ToString(itBulletMap->second->m_playerID));
			//DevConsole::GetInstance()->PushToOutputText(" ## PLAYER POSITION " + ToString(itPlayerMap->second->m_position.x) + " , " + ToString(itPlayerMap->second->m_position.y));
			//DevConsole::GetInstance()->PushToOutputText(" ## BULLET POSITION " + ToString(itBulletMap->second->m_position.x) + " , " + ToString(itBulletMap->second->m_position.y));
			Vector2 distance = itPlayerMap->second->m_position - itBulletMap->second->m_position;
			//DevConsole::GetInstance()->PushToOutputText(" ## DISTANCE " + ToString(distance.GetLength()));

			if(DoDiscsOverlap(playerDisc,bulletDisc))
			{
				//DevConsole::GetInstance()->PushToOutputText("$$ HIT PLAYER ID " + ToString(itPlayerMap->second->m_index) + " BULET ID " + ToString(itBulletMap->second->m_playerID));

				std::map<int, NetConnection*>::iterator itcon;
				for (itcon = NetSession::GetInstance()->m_boundConnections.begin(); itcon != NetSession::GetInstance()->m_boundConnections.end(); itcon++)
				{

					NetMessage *playerDestroyMsg = NetMessage::CreateObjectDestroyMsg(NETOBJ_PLAYER, itPlayerMap->second->m_index, itcon->second->m_index);
					NetMessage *bulletDestroyMsg = NetMessage::CreateObjectDestroyMsg(NETOBJ_BULLET, itBulletMap->second->m_index, itcon->second->m_index);
					playerDestroyMsg->SetAddress(&itcon->second->m_address);
					bulletDestroyMsg->SetAddress(&itcon->second->m_address);

					itcon->second->Append(playerDestroyMsg);
					itcon->second->Append(bulletDestroyMsg);

				}
				
				DestroyBullet(itBulletMap->second->m_index);
				DestroyPlayer(itPlayerMap->second->m_index);

				doBreak = true;
				break;
			}
		}
		if(doBreak)
		{
			break;
		}
	}
}

void Game::Update(float deltaTime)
{
	if(!m_init)
	{
		Initialize();
		m_init = true;
	}
	
	m_camera->SetOrthoProjection();
	UpdateGame(deltaTime);
	/*switch (m_gameMode)
	{
	case MAIN_MENU:
		UpdateMainMenu(deltaTime);
		break;
	case GAME_PLAY:
		UpdateMap(deltaTime);
		break;
	default:
		break;
	}*/
	UpdateUnreliableMsgs(deltaTime);
	UpdateReliableMsgs(deltaTime);
}


void Game::Render()
{
	/*switch (m_gameMode)
	{
	case MAIN_MENU:
		RenderMainMenu();
		break;
	case GAME_PLAY:
		RenderMap();
		break;
	default:
		break;
	}*/
	RenderGame();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Updates map if exist
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateMap(float deltaTime)
{
	if(m_map != nullptr)
	{
		if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
		{
			isQuitTriggered = true;
		}
		
		m_map->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Updates mainmenu items
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateMainMenu(float deltaTime)
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateUnreliableMsgs(float deltaTime)
{
	if(true)
	{
		return;
	}
	if(m_netMsgCount <  m_netMsgMaxUnrealiableMsgCount)
	{
		m_netMsgSendTime += deltaTime;
		if(m_netMsgSendTime > m_netMsgSendDelay)
		{
			m_netMsgSendTime = 0.f;
			NetMessage *msg = NetMessage::CreateUnreliableTestMessage(NetSession::GetInstance()->GetConnection(m_netMsgConnectionIndex), 
				m_netMsgCount, m_netMsgMaxUnrealiableMsgCount);
			NetConnection *connection = NetSession::GetInstance()->GetConnection(m_netMsgConnectionIndex);
			connection->Append(msg);
			m_netMsgCount++;
			if(m_netMsgCount >= m_netMsgMaxUnrealiableMsgCount)
			{
				m_netMsgMaxUnrealiableMsgCount = 0;
				m_netMsgCount = 0;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::UpdateReliableMsgs(float deltaTime)
{
	if (m_netMsgCount < m_netMsgMaxUnrealiableMsgCount)
	{
		m_netMsgSendTime += deltaTime;
		if (m_netMsgSendTime > m_netMsgSendDelay)
		{
			m_netMsgSendTime = 0.f;
			NetMessage *msg = NetMessage::CreateReliableTestMessage(NetSession::GetInstance()->GetConnection(m_netMsgConnectionIndex),
				m_netMsgCount + 1, m_netMsgMaxUnrealiableMsgCount);
			NetSession::GetInstance()->GetConnection(m_netMsgConnectionIndex)->Append(msg);
			m_netMsgCount++;
			if (m_netMsgCount >= m_netMsgMaxUnrealiableMsgCount)
			{
				m_netMsgMaxUnrealiableMsgCount = 0;
				m_netMsgCount = 0;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Renders map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderMap()
{
	if(m_map != nullptr)
	{
		m_map->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/10/14
*@purpose : Render main menu
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderMainMenu()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	Material *textMaterial = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial);
	//Vector2 menuItemStartPosition(g_mainMenuStartX, g_mainMenuStartY);
	//Material *defaultMaterial = Material::AquireResource("default");
	//Renderer::GetInstance()->BindMaterial(defaultMaterial);
	//Renderer::GetInstance()->DrawAABB(AABB2(menuItemStartPosition + Vector2(-100, m_currentIndex*g_fontSize * 4),500,50),Rgba::WHITE);
	
	delete textMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/12
*@purpose : Renders explosions
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderExplosions()
{
	for (size_t explosionIndex = 0; explosionIndex < m_explosions.size(); explosionIndex++)
	{
		m_explosions.at(explosionIndex)->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderGame()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();
	RenderPlayers();
	RenderBullets();
	RenderExplosions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderPlayers()
{
	std::map<uint8_t, Player*>::iterator it = m_playerMap.begin();
	for (; it != m_playerMap.end(); it++)
	{
		it->second->Render();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/05
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::RenderBullets()
{
	std::map<uint8_t, Bullets*>::iterator it = m_bulletMap.begin();
	for (; it != m_bulletMap.end(); it++)
	{
		it->second->Render();
	}
}

Game* Game::GetInstance()
{
	if (s_game == nullptr)
	{
		s_game = new Game();
	}
	return s_game;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/04
*@purpose : Unreliable test msg
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnUnreliableTest(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	std::string str = netMsg.GetBitString();
	int value1 = 0;
	int value2 = 0;
	netMsg.m_currentReadPosition = 3;
	netMsg.ReadBytes(&value1, 4);
	netMsg.ReadBytes(&value2, 4);
	g_counter++;
	DevConsole::GetInstance()->PushToOutputText("UNRELIABLE TEST ( " + ToString(value1) + " , " + ToString(value2)+" )");
	if(g_counter == value2)
	{
		DevConsole::GetInstance()->PushToOutputText("COUNTER - " + ToString(g_counter));
		g_counter = 0;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/11/05
*@purpose : Once a reliable test msg is received
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnReliableTest(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	int value1 = 0;
	int value2 = 0;
	netMsg.ReadBytes(&value1, 4);
	netMsg.ReadBytes(&value2, 4);
	g_counter++;
	DevConsole::GetInstance()->PushToOutputText("RELIABLE TEST ( " + ToString(value1) + " , " + ToString(value2) + " )");
	if (g_counter == value2)
	{
		DevConsole::GetInstance()->PushToOutputText("COUNTER - " + ToString(g_counter));
		g_counter = 0;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnObjectCreated(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	//DevConsole::GetInstance()->PushToOutputText("ON OBJECT CREATED ");
	uint8_t objectType;
	uint8_t objectID;
	netMsg.m_currentReadPosition = 5;
	netMsg.ReadBytes((void*)&objectType, 1);
	if (objectType == NETOBJ_PLAYER)
	{
		netMsg.ReadBytes((void*)&objectID, 1);
		Vector2 position;
		float   angle;
		netMsg.ReadBytes((void*)&position.x, 4);
		netMsg.ReadBytes((void*)&position.y, 4);
		netMsg.ReadBytes((void*)&angle, 4);
		DevConsole::GetInstance()->PushToOutputText("CREATED PLAYER FOR CONN "+ToString(objectID) +" POS X = "+ToString(position.x)+" POS Y = "+ToString(position.y));
		Game::GetInstance()->CreatePlayer(objectID, "", position,angle);
	}

	if (objectType == NETOBJ_BULLET)
	{
		//DevConsole::GetInstance()->PushToOutputText("ON BULLET CREATED ");
		netMsg.ReadBytes((void*)&objectID, 1);
		Vector2 position;
		float   angle;
		float   health;
		uint8_t playerID;
		netMsg.ReadBytes((void*)&position.x, 4);
		netMsg.ReadBytes((void*)&position.y, 4);
		netMsg.ReadBytes((void*)&angle, 4);
		netMsg.ReadBytes((void*)&playerID, 1);

		if (NetSession::GetInstance()->m_hostConnection->IsHost())
		{
			Game::GetInstance()->BroadcastCreateBulletMsg(playerID,position, angle);
		}
		else
		{
			Game::GetInstance()->CreateBullet(playerID,objectID, position, angle);
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnObjectUpdate(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	//DevConsole::GetInstance()->PushToOutputText("ON OBJECT UPDATE ");
	uint8_t objectType;
	uint8_t objectID;
	netMsg.m_currentReadPosition = 3;

	Vector2 position;
	float angle;
	float health;

	netMsg.ReadBytes((void*)&objectType, 1);
	netMsg.ReadBytes((void*)&objectID,   1);

	size_t readPosition = netMsg.m_currentReadPosition;
	std::string bit = netMsg.GetBitString();
	netMsg.ReadBytes((void*)&position.x ,4);
	netMsg.ReadBytes((void*)&position.y ,4);
	netMsg.ReadBytes((void*)&angle		,4);
	netMsg.ReadBytes((void*)&health     ,4);

	netMsg.m_currentReadPosition = readPosition;

	if(!NetSession::GetInstance()->m_netObjectSystem->DoesObjectExist(objectID))
	{
		return true;
	}

	if (objectType == NETOBJ_PLAYER)
	{
		if(NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot == nullptr)
		{
			NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot = malloc(16);
		}
		netMsg.ReadBytes(NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot, 16);
		//std::string bit = ToBitString(NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot, 16);
		
		if(NetSession::GetInstance()->m_hostConnection->IsHost())
		{
			NetSession::GetInstance()->BroadcastMsg(&netMsg,CONNECTION_READY);
		}
		//DevConsole::GetInstance()->PushToOutputText("CREATED PLAYER FOR CONN " + ToString(objectID));
	}

	if (objectType == NETOBJ_BULLET)
	{
		if (NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot == nullptr)
		{
			NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot = malloc(16);
		}
		netMsg.ReadBytes(NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot, 16);
		//std::string bit = ToBitString(NetSession::GetInstance()->m_netObjectSystem->m_netObjectMap[objectID]->m_latestReceivedSnapshot, 16);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/02
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OnObjectDestroyed(NetMessage &netMsg, NetAddress &netAddress)
{
	UNUSED(netAddress);
	uint8_t objectType;
	uint8_t objectID;
	netMsg.m_currentReadPosition = 5;

	netMsg.ReadBytes((void*)&objectType, 1);
	netMsg.ReadBytes((void*)&objectID, 1);


	if (objectType == NETOBJ_PLAYER)
	{
		DevConsole::GetInstance()->PushToOutputText("OBJECT DESTROYED PLAYER");
		Game::GetInstance()->DestroyPlayer(objectID);
	}
	if (objectType == NETOBJ_BULLET)
	{
		//DevConsole::GetInstance()->PushToOutputText("OBJECT DESTROYED BULLET");
		Game::GetInstance()->DestroyBullet(objectID);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OnConnectionJoin(NetConnection *cp)
{
	Vector2 maxPositions = Windows::GetInstance()->GetDimensions().GetAsVector2();
	Vector2 position = Vector2(GetRandomFloatInRange(0, maxPositions.x), GetRandomFloatInRange(0, maxPositions.y));
	float   angle    = GetRandomFloatInRange(0, 360);
  	Game::GetInstance()->CreatePlayer(cp->m_index, "", position,angle);

	std::map<int, NetConnection*>::iterator it;
	for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
	{
		//if (state >= it->second->m_connectionState)
		{

			NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_PLAYER, cp->m_index,it->second->m_index);
			
			msg->WriteBytes(4, (void *)&position.x);
			msg->WriteBytes(4, (void *)&position.y);
			msg->WriteBytes(4, (void *)&angle);

			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}
	}
	DevConsole::GetInstance()->PushToOutputText("BROADCASTS PLAYER CREATE MSG ");
	//NetSession::GetInstance()->BroadcastMsg(msg,CONNECTION_READY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/03
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OnConnectionLeave(NetConnection *cp)
{
	Game::GetInstance()->DestroyPlayer(cp->m_index);
	if(cp == NetSession::GetInstance()->m_hostConnection) // client connections
	{
		DevConsole::GetInstance()->PushToOutputText("HOST LEFT ",Rgba::RED);
		NetSession::GetInstance()->m_hostConnection = nullptr;
		NetSession::GetInstance()->m_sessionState = SESSION_DISCONNECTED;
		
		Game::GetInstance()->DestroyAllBullets();
		Game::GetInstance()->DestroyAllPlayers();
		//g_isQuitting = true;
	}
}

void SendPlayerCreate(NetMessage *msg, void *objPtr)
{
	Player *player = (Player*)objPtr;
	msg->WriteBytes (1, (void*)&player->m_ownerConnectionIndex);
	msg->WriteString(player->m_name.c_str());
}

void RecvPlayerCreate(NetMessage *msg, void *objPtr)
{
	/*uint8_t		index;
	std::string name;
	Rgba		color;

	msg->ReadBytes((void*)&index,1);
	msg->ReadString((char*)&name, 5);
	msg->ReadColor(&color);

	Game *game		= Game::GetInstance();
	Player *player  = game->CreatePlayer(index, name.c_str(), color);

	objPtr = (void *)player;*/
}

void SendPlayerDestroy(NetMessage *msg, void *objPtr) 
{
	UNUSED(msg);
	UNUSED(objPtr);
}

void RecvPlayerDestroy(NetMessage *msg, void *objPtr) 
{
	UNUSED(msg);
	UNUSED(objPtr); 
	//Player *player = (Player*)objPtr;
	//Game *game = Game::GetInstance();
	//game->DestroyPlayer(player);
}

void PlayerGetSnapshot(NetMessage *msg, void *Obj) 
{
	UNUSED(msg);
	UNUSED(Obj);
}

void PlayerSendSnapshot(NetMessage *msg, void *snapshotPtr)
{
	UNUSED(msg);
	UNUSED(snapshotPtr);
}

void PlayerRecvSnapshot(NetMessage *msg, void *snapshotPtr)
{
	UNUSED(msg);
	UNUSED(snapshotPtr);
}

void PlayerApplySnapshot(NetMessage *msg, void *snapshotPtr) 
{
	UNUSED(msg);
	UNUSED(snapshotPtr);
}
