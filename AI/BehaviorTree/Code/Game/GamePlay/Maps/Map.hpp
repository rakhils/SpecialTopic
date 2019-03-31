#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"

class Tiles;
class Player;
class Enemy;
class Bullets;
class Map
{
public:
	bool							m_init		    = false;
	bool							m_gameFinished  = false;
	Camera *						m_camera        = nullptr;
	Player *						m_player	    = nullptr;
	Enemy *							m_enemy			= nullptr;

	std::vector<Tiles*>				m_tiles;
	std::vector<Bullets*>			m_bullets;

	Map();
	~Map();

	void							Initialize();
	void							InitCamera();
	void							InitBlocks();
	void							InitObstacles();

	void							InitGameEntities();

	void							CreateBullet(Vector2 position, Vector2 direction);

	void							CheckBulletVsPlayerCollision();
	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);
	void							UpdatePlayer(float deltaTime);
	void							UpdateEnemy (float deltaTime);
	void							UpdateBullet(float deltaTime);

	void							Render();
	void							RenderGrids();
	void							RenderTiles();
	void							RenderPlayer();
	void							RenderEnemy();
	void							RenderBullet();

};