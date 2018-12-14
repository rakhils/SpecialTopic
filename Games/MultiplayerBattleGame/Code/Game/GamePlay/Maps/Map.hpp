#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
#include "Game/GamePlay/Utils/Explosion.hpp"

class Map
{
public:
	bool							m_init = false;
	bool							m_gameFinished = false;
	Camera *						m_camera = nullptr;
	std::vector<Explosion*>			m_explosions;


	Map();
	~Map();

	void							Initialize();
	void							InitCamera();

	void							CreateExplosions(Vector2 position);

	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);
	void							UpdateExplosions(float deltaTime);

	void							Render();
	void							RenderGrids();
	void							RenderExplosions();

	void							CreateAndBroadcastPlayerDieadMsg(uint8_t playerID);
	void							CreateAndBroadcastBulletDestroyedMsg(uint8_t bulletID);
	void							CreateAndBroadcastPlayerCreateMsg(uint8_t playerID);
	void							CreateAndBroadcastBulletCreateMsg(uint8_t bulletID);
};