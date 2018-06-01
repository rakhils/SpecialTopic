#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"

#include "Game/GamePlay/Maps/MapDefinition.hpp"
#include "Game/GamePlay/Tiles/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlay/Maps/Map.hpp"

struct ObjectProperties
{
	Vector4 m_tint;

};

class ForwardRenderingPath;
class Renderable;
class Scene;
class Ship;
class Game
{
public:
	bool				   m_init = false;


	Ship				   *m_tank;
	Scene				   *m_scene;
	ForwardRenderingPath   *m_forwardRenderingPath = nullptr;

	Game();
	void Init();
	void CreatePlayer();
	void SetCameraFocusOnPosition();
	void Update(float deltaTime);
	void Render();
};