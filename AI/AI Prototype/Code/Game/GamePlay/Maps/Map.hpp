#pragma once
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Mesh/MeshBuilder.hpp"

#include "Game/GamePlay/Entity/Entity.hpp"
class Tiles;
struct Triangle
{
	Vector2 position1;
	Vector2 position2;
	Vector2 position3;
};
class Map
{
public:
	bool							m_init		   = false;
	Camera *						m_camera       = nullptr;
	std::vector<Tiles*>				m_tiles;
	
	Map();
	~Map();

	void							Initialize();
	void							InitCamera();

	void							ProcessInputs(float deltaTime);

	void							Update(float deltaTime);
	void							UpdateCamera(float deltaTime);

	void							Render();

};