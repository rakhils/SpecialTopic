#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Net/NetMessage.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"

enum GameMode
{
	MAIN_MENU,
	GAME_PLAY
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
	bool						m_init							= false;


	float						m_mapWidth						= 500;
	float						m_mapHeight						= 500;

	int							m_netMsgConnectionIndex			= 0;
	int							m_netMsgCount					= 0;
	int							m_netMsgMaxUnrealiableMsgCount  = 0;

	bool						m_reliableMsg					= false;
	float						m_netMsgSendDelay				= 0.01f;
	float						m_netMsgSendTime				= 0.f;

	std::map<uint8_t, Player*>		  m_playerMap;
	std::map<uint8_t, Bullets*>		  m_bulletMap;
	std::vector<Explosion*>			  m_explosions;
	uint8_t							  m_largestNonPlayerID				= 10;
	// STATIC
	static Game *s_game;

	// FUNCIONS
	Game();
	~Game();

	Player*						CreatePlayer(uint8_t idx, char const *name,Vector2 position,float angle);
	Bullets*					CreateBullet(uint8_t playerID,uint8_t idx,Vector2 position,float angle);
	Explosion*					CreateExplosion(uint8_t idx, Vector2 position);

	void						BroadcastCreatePlayerMsg(uint8_t idx, Vector2 position, float angle);
	void						BroadcastCreateBulletMsg(uint8_t playerID,Vector2 position,float angle);
	void						BroadcastDestroyPlayerMsg(uint8_t idx);
	void						BroadcastDestroyBulletMsg(uint8_t idx);

	bool						IsPositionOutOfBounds(Vector2 position);

	uint8_t						GetUniqueNonPlayerID();
	void						DestroyPlayer(uint8_t playerID);
	void						DestroyBullet(uint8_t bulletID);
	void						DestroyAllPlayers();
	void						DestroyAllBullets();
	void						SetupNetwork();
	
	void						RegisterGameMessage();
								
	void						Initialize();
	void						InitMainMenuItems();
	void						InitCamera();
								
	void						UpdateGame(float deltaTime);
	void						UpdatePlayer(float deltaTime);
	void						UpdateBullet(float deltaTime);
	void						UpdateExplosions(float deltaTime);
	void						UpdatePlayerBulletCollision();
								
	void						Update(float deltaTime);
								
	void						Render();
								
	void						UpdateMap(float deltaTime);
	void						UpdateMainMenu(float deltaTime);
								
	void						UpdateUnreliableMsgs(float deltaTime);
	void						UpdateReliableMsgs(float deltaTime);
								
	void						RenderMap();
	void						RenderMainMenu();
	void						RenderExplosions();
								
	void						RenderGame();
	void						RenderPlayers();
	void						RenderBullets();

	void						InitSampleNN();
	static Game*				GetInstance();

};
bool	OnUnreliableTest		(NetMessage &netMsg, NetAddress &netAddress);
bool	OnReliableTest			(NetMessage &netMsg, NetAddress &netAddress);

bool	OnObjectCreated			(NetMessage &netMsg, NetAddress &netAddress);
bool	OnObjectUpdate			(NetMessage &netMsg, NetAddress &netAddress);
bool	OnObjectDestroyed		(NetMessage &netMsg, NetAddress &netAddress);

void	OnConnectionJoin		(NetConnection *cp);
void	OnConnectionLeave		(NetConnection *cp);

void	SendPlayerCreate		(NetMessage *msg, void *objPtr);
void	RecvPlayerCreate		(NetMessage *msg, void *objPtr);
void	SendPlayerDestroy		(NetMessage *msg, void *objPtr);
void	RecvPlayerDestroy		(NetMessage *msg, void *objPtr);
void	PlayerGetSnapshot		(NetMessage *msg, void *objPtr);

void	PlayerSendSnapshot		(NetMessage *msg, void *snapshotPtr);
void	PlayerRecvSnapshot		(NetMessage *msg, void *snapshotPtr);
void	PlayerApplySnapshot		(NetMessage *msg, void *snapshotPtr);


