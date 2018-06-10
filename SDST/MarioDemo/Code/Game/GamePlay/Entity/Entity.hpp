#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/GameObject/GameObject.hpp"

#include "Game/GamePlay/Entity/EntityDefinition.hpp"

class Map;
class Entity : public GameObject
{
public:
	float m_length = 1;
	float m_height = 1;

	float m_speed = 0.05f;
	bool             m_visible = true;
	Vector2 m_forwardVector;
	EntityDefinition *m_definition	    = nullptr;
	Texture			 *m_texture			= nullptr;
	SpriteAnimSet    *m_spriteAnimSet   = nullptr;
	Map			     *m_map				= nullptr;

	Entity(std::string name);
	Entity(std::string name,Vector2 position);
	void Update(float deltaTime);
	void UpdateVelocity(float deltaTime);
	void Render();

	void SetPosition(Vector2 position);
	Vector2 GetDrawDimensions();
	Vector2 GetCenter();
	Vector2 GetForwardUnitVector();
};