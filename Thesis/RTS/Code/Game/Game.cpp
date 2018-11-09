#include "Game.hpp"
#include "Engine/Net/Socket.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Net/NetSession.hpp"

Game *Game::s_game = nullptr;

Game::Game()
{
	
}

Game::~Game()
{

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

void Game::Update(float deltaTime)
{
	if(!m_init)
	{
		Initialize();
		m_init = true;
	}
	switch (m_gameMode)
	{
	case MAIN_MENU:
		UpdateMainMenu(deltaTime);
		break;
	case GAME_PLAY:
		UpdateMap(deltaTime);
		break;
	default:
		break;
	}
	//UpdateUnreliableMsgs(deltaTime);
	UpdateReliableMsgs(deltaTime);
}


void Game::Render()
{
	switch (m_gameMode)
	{
	case MAIN_MENU:
		RenderMainMenu();
		break;
	case GAME_PLAY:
		RenderMap();
		break;
	default:
		break;
	}
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
				m_netMsgCount, m_netMsgMaxUnrealiableMsgCount);
			NetSession::GetInstance()->m_channel->Append(msg);
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
