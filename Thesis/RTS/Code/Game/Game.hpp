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
	NETMSG_OBJECT_CREATE,  
	NETMSG_OBJECT_DESTROY, 
	NETMSG_OBJECT_UPDATE,
	NETMSG_UNRELIABLE_TEST			  = 125,
	NETMSG_RELIABLE_TEST			  = 126,
	NETMSG_RELIABLE_IN_ORDER_TEST     = 127,

};

class Player;
class Bullets;
class Game
{
public:
	bool						isQuitTriggered					= false;
	GameMode					m_gameMode;
	Map *						m_map							= nullptr;
	Camera *					m_camera						= nullptr;
	int							m_currentIndex					= 0;
	bool						m_init							= false;
	float						positionX						= 0.f;
	int							m_netMsgConnectionIndex			= 0;
	int							m_netMsgCount					= 0;
	int							m_netMsgMaxUnrealiableMsgCount  = 0;
	bool						m_reliableMsg					= false;
	float						m_netMsgSendDelay				= 0.01f;
	float						m_netMsgSendTime				= 0.f;
	std::vector<MainMenuItems>  m_mainMenuItems;

	std::map<uint8_t, Player*>  m_playerMap;
	std::map<uint8_t, Bullets*> m_bulletMap;
	uint8_t						m_largetstBulletID				= 10;
	// STATIC
	static Game *s_game;

	// FUNCIONS
	Game();
	~Game();

	Player*  CreatePlayer(uint8_t idx, char const *name, Rgba color);
	Bullets* CreateBullet(uint8_t idx,Vector2 position,float angle);
	uint8_t  GetUniqueBulletID();
	void     DestroyPlayer(uint8_t playerID);
	void     DestroyBullet(uint8_t bulletID);
	void     DestroyAllPlayers();
	void     DestroyAllBullets();
	void	 SetupNetwork();
	
	void Initialize();
	void InitMainMenuItems();
	void InitCamera();

	void InitMapAndStartGame();

	void Update(float deltaTime);
	void Render();
};
bool OnUnreliableTest		(NetMessage &netMsg, NetAddress &netAddress);
bool OnReliableTest			(NetMessage &netMsg, NetAddress &netAddress);

bool OnObjectCreated		(NetMessage &netMsg, NetAddress &netAddress);
bool OnObjectUpdate			(NetMessage &netMsg, NetAddress &netAddress);
bool OnObjectDestroyed		(NetMessage &netMsg, NetAddress &netAddress);

void OnConnectionJoin		(NetConnection *cp);
void OnConnectionLeave		(NetConnection *cp);

void SendPlayerCreate		(NetMessage *msg, void *objPtr);
void RecvPlayerCreate		(NetMessage *msg, void *objPtr);
void SendPlayerDestroy		(NetMessage *msg, void *objPtr);
void RecvPlayerDestroy		(NetMessage *msg, void *objPtr);
void PlayerGetSnapshot		(NetMessage *msg, void *objPtr);

void PlayerSendSnapshot		(NetMessage *msg, void *snapshotPtr);
void PlayerRecvSnapshot		(NetMessage *msg, void *snapshotPtr);
void PlayerApplySnapshot	(NetMessage *msg, void *snapshotPtr);


