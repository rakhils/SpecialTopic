#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
class Tiles;
class Map
{
public:
	bool							m_init		   = false;
	bool							m_gameFinished = false;
	Camera *						m_camera       = nullptr;
	std::vector<Tiles*>				m_tiles;

	Map();
	~Map();

	void							Initialize();
	void							InitCamera();
	void							InitBlocks();
	void							InitObstacles();

	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);

	void							Render();
	void							RenderGrids();
	void							RenderTiles();

};