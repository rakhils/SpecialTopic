#include "Game.hpp"
#include "Engine/Net/Socket.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/StringUtils.hpp"
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
Player* Game::CreatePlayer(uint8_t idx, char const *name, Rgba color)
{
	DevConsole::GetInstance()->PushToOutputText("CREATING PLAYER ID " + ToString(idx));
	Player *player   = new Player();
	player->m_name   = name;
	player->m_index  = idx;
	player->m_color  = color;
	m_playerMap[idx] = player;

	NetObject* netObject = NetSession::GetInstance()->GetNetObjectSystem()->CreateNetObject(NETOBJ_PLAYER, idx);
	player->m_netObject = netObject;

	
	return player;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Bullets* Game::CreateBullet(uint8_t idx,Vector2 position,float angle)
{
	Bullets *bullet  = new Bullets();
	bullet->m_index  = idx;
	position = position + Vector2(CosDegrees(angle), SinDegrees(angle)) * 90;
	bullet->m_position = position;
	bullet->m_angle = angle;
	m_bulletMap[idx] = bullet;


	NetObject* netObject = NetSession::GetInstance()->GetNetObjectSystem()->CreateNetObject(NETOBJ_BULLET, idx);
	bullet->m_netObject = netObject;

	if(NetSession::GetInstance()->m_hostConnection->IsHost())
	{

		std::map<int, NetConnection*>::iterator it;
		for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
		{
			NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_BULLET, idx,it->second->m_index);
			msg->WriteBytes(4, (void*)&position.x);
			msg->WriteBytes(4, (void*)&position.y);
			msg->WriteBytes(4, (void*)&angle);

			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}
		DevConsole::GetInstance()->PushToOutputText("BULLET CREATED AT HOST IDX " + ToString(idx));
		return bullet;
	}
	DevConsole::GetInstance()->PushToOutputText("BULLET CREATED AT CLIENT IDX " + ToString(idx));
	return bullet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/12/04
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Game::GetUniqueBulletID()
{
	return m_largetstBulletID++;
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
	NetSession::GetInstance()->GetNetObjectSystem()->DestroyNetObject(it->second->m_netObject);
	delete it->second;
	m_playerMap.erase(it);
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
	playerType.m_snapshotSize	= sizeof(PlayerSnapShot_t);
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
	for(int index = 0;index < MAP_MODE_NUM_ITEMS;index++)
	{
		MainMenuItems mmitem;
		mmitem.m_index = index + 1;
		mmitem.m_menuItemString = Map::GetMapModeAsString((MapMode)index);
		m_mainMenuItems.push_back(mmitem);
	}
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
			
				std::map<int, NetConnection*>::iterator itcon;
				for (itcon = NetSession::GetInstance()->m_boundConnections.begin(); itcon != NetSession::GetInstance()->m_boundConnections.end(); itcon++)
				{
					NetMessage *msg = NetMessage::CreateObjectDestroyMsg(NETOBJ_BULLET, it->second->m_index, itcon->second->m_index);	
					msg->SetAddress(&itcon->second->m_address);
					itcon->second->Append(msg);
				}

				//NetSession::GetInstance()->BroadcastMsg(msg, CONNECTION_READY);
				delete it->second;
				m_bulletMap.erase(it);
				break;
			}
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
			Disc2 playerDisc(itPlayerMap->second->m_position, 50);
			Disc2 bulletDisc(itBulletMap->second->m_position, 20);

			if(DoDiscsOverlap(playerDisc,bulletDisc))
			{


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


				/*NetMessage *playerDestroyMsg = NetMessage::CreateObjectDestroyMsg(NETOBJ_PLAYER, itPlayerMap->second->m_index);
				NetMessage *bulletDestroyMsg = NetMessage::CreateObjectDestroyMsg(NETOBJ_BULLET, itBulletMap->second->m_index);
				
				NetSession::GetInstance()->BroadcastMsg(playerDestroyMsg, CONNECTION_READY);
				NetSession::GetInstance()->BroadcastMsg(bulletDestroyMsg, CONNECTION_READY);*/
				
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
		if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_E))
		{
			g_isCurrentlyTraining = g_isCurrentlyTraining ? false : true;
		}
		if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_T))
		{
			g_enableNeuralNet = g_enableNeuralNet ? false : true;
		}
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
	UNUSED(deltaTime);
	m_camera->SetOrthoProjection();
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_UP_ARROW))
	{
		m_currentIndex--;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_DOWN_ARROW))
	{
		m_currentIndex++;
	}
	if (g_theInput->wasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ENTER))
	{
		if (m_map == nullptr)
		{
			m_map = new Map();
			m_map->SetMapType(MapMode(m_currentIndex));
			m_map->Initialize();
			m_gameMode = GAME_PLAY;
		}
	}
	m_currentIndex = ClampInt(m_currentIndex, 0, static_cast<int>(m_mainMenuItems.size() - 1));
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
	Vector2 menuItemStartPosition(g_mainMenuStartX, g_mainMenuStartY);
	//Material *defaultMaterial = Material::AquireResource("default");
	//Renderer::GetInstance()->BindMaterial(defaultMaterial);
	//Renderer::GetInstance()->DrawAABB(AABB2(menuItemStartPosition + Vector2(-100, m_currentIndex*g_fontSize * 4),500,50),Rgba::WHITE);
	for(int menuItemIndex = 0;menuItemIndex < m_mainMenuItems.size();menuItemIndex++)
	{
		std::string menuItemString = ToString(menuItemIndex);
		menuItemString.append(" :: "+m_mainMenuItems.at(menuItemIndex).m_menuItemString);
		Rgba color = Rgba::WHITE;
		if(m_currentIndex == menuItemIndex)
		{
			color = Rgba::YELLOW;
		}
		//color = Rgba(GetRandomFloatInRange(0,255), GetRandomFloatInRange(0,255),GetRandomFloatInRange(0,255));

		g_theRenderer->DrawTextOn3DPoint(menuItemStartPosition, Vector3::RIGHT, Vector3::UP, menuItemString, g_fontSize, color);
		menuItemStartPosition.y -= g_fontSize*4;
		break;
	}
	delete textMaterial;
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
	//Material *defaultMaterial = Material::AquireResource("default");
	//Renderer::GetInstance()->BindMaterial(defaultMaterial);
	//Disc2 disc(Vector2(250, 250), 100);
	//g_theRenderer->DrawCircle(disc, Rgba::RED);
	//delete defaultMaterial;
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

void Game::InitSampleNN()
{
	NeuralNetwork neuralNet(2, 2, 2);
	neuralNet.m_inputs->m_neurons.at(0).m_value = 0.05f;
	neuralNet.m_inputs->m_neurons.at(1).m_value = 0.1f;
	neuralNet.m_inputs->m_bias.m_value = 1;

	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(0) = 0.15f;
	neuralNet.m_inputs->m_neurons.at(0).m_weights.at(1) = 0.25f;

	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(0) = 0.20f;
	neuralNet.m_inputs->m_neurons.at(1).m_weights.at(1) = 0.30f;

	neuralNet.m_inputs->m_bias.m_weights.at(0) = 0.35f;
	neuralNet.m_inputs->m_bias.m_weights.at(1) = 0.35f;

	///////////////////////////////////////////////////////////////

	/*neuralNet.m_hiddenLayers->m_neurons.at(0).m_value = 0.40;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_value = 0.1;*/
	neuralNet.m_hiddenLayers->m_bias.m_value = 1;

	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(0) = 0.40f;
	neuralNet.m_hiddenLayers->m_neurons.at(0).m_weights.at(1) = 0.50f;

	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(0) = 0.45f;
	neuralNet.m_hiddenLayers->m_neurons.at(1).m_weights.at(1) = 0.55f;

	neuralNet.m_hiddenLayers->m_bias.m_weights.at(0) = 0.60f;
	neuralNet.m_hiddenLayers->m_bias.m_weights.at(1) = 0.60f;

	neuralNet.FeedForwardNN();
	
	std::vector<double> inputs;
	inputs.push_back(.05);
	inputs.push_back(.1);

	std::vector<double> m_targetOuputs;
	m_targetOuputs.push_back(.5);
	m_targetOuputs.push_back(.5);
	for(int index = 0;index < 2000;index ++)
	{
		inputs.at(0) = GetRandomFloatZeroToOne();
		inputs.at(1) = GetRandomFloatZeroToOne();
		neuralNet.FeedForward(inputs);
		neuralNet.BackPropogateOuputToHiddenLayer(m_targetOuputs);
		neuralNet.BackPropogateHiddenToInputLayer(m_targetOuputs);
	}
	neuralNet.FeedForward(inputs);
	neuralNet;
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
	DevConsole::GetInstance()->PushToOutputText("ON OBJECT CREATED ");
	uint8_t objectType;
	uint8_t objectID;
	netMsg.m_currentReadPosition = 5;
	netMsg.ReadBytes((void*)&objectType, 1);
	if (objectType == NETOBJ_PLAYER)
	{
		netMsg.ReadBytes((void*)&objectID, 1);
		Rgba color;
		netMsg.ReadColor(&color);
		DevConsole::GetInstance()->PushToOutputText("CREATED PLAYER FOR CONN "+ToString(objectID));
		Game::GetInstance()->CreatePlayer(objectID, "", color);
	}

	if (objectType == NETOBJ_BULLET)
	{
		netMsg.ReadBytes((void*)&objectID, 1);
		Vector2 position;
		float   angle;
		netMsg.ReadBytes((void*)&position.x, 4);
		netMsg.ReadBytes((void*)&position.y, 4);
		netMsg.ReadBytes((void*)&angle,    4);

		Game::GetInstance()->CreateBullet(objectID, position,angle);
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
	float primary;

	netMsg.ReadBytes((void*)&objectType, 1);
	netMsg.ReadBytes((void*)&objectID,   1);

	size_t readPosition = netMsg.m_currentReadPosition;
	std::string bit = netMsg.GetBitString();
	netMsg.ReadBytes((void*)&position.x ,4);
	netMsg.ReadBytes((void*)&position.y ,4);
	netMsg.ReadBytes((void*)&angle		,4);
	netMsg.ReadBytes((void*)&primary    ,4);

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
		DevConsole::GetInstance()->PushToOutputText("OBJECT DESTROYED BULLET");
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
	Game::GetInstance()->CreatePlayer(cp->m_index, "", Rgba::GREEN);

	


	std::map<int, NetConnection*>::iterator it;
	for (it = NetSession::GetInstance()->m_boundConnections.begin(); it != NetSession::GetInstance()->m_boundConnections.end(); it++)
	{
		//if (state >= it->second->m_connectionState)
		{

			NetMessage *msg = NetMessage::CreateObjectCreateMsg(NETOBJ_PLAYER, cp->m_index,it->second->m_index);
			//NetMessage *copyMsg = new NetMessage(msg);
			msg->SetAddress(&it->second->m_address);
			it->second->Append(msg);
		}
	}
	




	//msg->SetAddress(&NetSession::GetInstance()->m_hostConnection->m_address);
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
	msg->WriteColor (player->m_color);
}

void RecvPlayerCreate(NetMessage *msg, void *objPtr)
{
	uint8_t		index;
	std::string name;
	Rgba		color;

	msg->ReadBytes((void*)&index,1);
	msg->ReadString((char*)&name, 5);
	msg->ReadColor(&color);

	Game *game		= Game::GetInstance();
	Player *player  = game->CreatePlayer(index, name.c_str(), color);

	objPtr = (void *)player;
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
