#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Net/NetMessage.hpp"

enum GameMode
{
	MAIN_MENU,
	GAME_PLAY
};
struct MainMenuItems
{
	int m_index;
	std::string m_menuItemString;
};
enum eNetGameMessage : uint8_t
{
	NETMSG_TEST_GAME_MESSAGE = NETMSG_CORE_COUNT,
	NETMSG_UNRELIABLE_TEST   = 127,
	NETMSG_RELIABLE_TEST     = 128,
};
class Game
{
public:
	bool		 isQuitTriggered = false;
	GameMode     m_gameMode;
	Map *		 m_map			 = nullptr;
	Camera *     m_camera		 = nullptr;
	int			 m_currentIndex = 0;
	bool		 m_init			= false;

	int          m_netMsgConnectionIndex = 0;
	int          m_netMsgCount = 0;
	int		     m_netMsgMaxUnrealiableMsgCount = 0;
	bool		 m_reliableMsg = false;
	float        m_netMsgSendDelay   = .05f;
	float        m_netMsgSendTime = 0;
	std::vector<MainMenuItems> m_mainMenuItems;
	// STATIC
	static Game *s_game;

	// FUNCIONS
	Game();
	~Game();
	
	void RegisterGameMessage();

	void Initialize();
	void InitMainMenuItems();
	void InitCamera();
	void Update(float deltaTime);
	void Render();

	void UpdateMap(float deltaTime);
	void UpdateMainMenu(float deltaTime);

	void UpdateUnreliableMsgs(float deltaTime);
	void UpdateReliableMsgs(float deltaTime);

	void RenderMap();
	void RenderMainMenu();

	void InitSampleNN();
	static Game* GetInstance();

};
bool OnUnreliableTest(NetMessage &netMsg, NetAddress &netAddress);
bool OnReliableTest(NetMessage &netMsg, NetAddress &netAddress);
