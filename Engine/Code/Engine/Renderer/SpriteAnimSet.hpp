#pragma once
#include <map>
#include <string>

#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Renderer.hpp"

class SpriteAnimSet
{
public:

	SpriteAnimation* m_currentAnimation = nullptr;
	SpriteAnimSetDefinition *m_spirteAnimSetDef = nullptr;
	std::map<std::string,SpriteAnimation*> m_spriteAnimation;
	
	SpriteAnimSet();
	SpriteAnimSet(SpriteAnimSetDefinition *spriteAnimationSetDef);
	~SpriteAnimSet();

	void			update(float deltaTime);

	void			SetAnimation(std::string animation);
	AABB2 			GetCurrentUVs() const;

};