#include "Game/GamePlay/Utils/Explosion.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
// CONSTRUCTOR
Explosion::Explosion(Vector2 position)
{
	m_position = position;

	
}

// DESTRUCTOR
Explosion::~Explosion()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Explosion::Update(float deltaTime)
{

	Texture *explosionTexture = Texture::CreateOrGetTexture("Data\\Images\\explosion.png");
	if (m_explosionSpriteSheet == nullptr)
	{
		m_explosionSpriteSheet = new SpriteSheet(*explosionTexture, 5, 5);
	}

	if (m_explosionAnimation == nullptr)
	{
		int totalSprites = 5 * 5;
		m_explosionAnimation = new SpriteAnimation(m_explosionSpriteSheet, m_duration, SPRITE_ANIM_MODE_PLAY_TO_END, 0, totalSprites - 1);
	}
	if(m_markForDelete)
	{
		return;
	}
	if (m_explosionAnimation != nullptr)
	{
		m_explosionAnimation->Update(deltaTime);
		if (m_explosionAnimation->m_isFinished)
		{
			m_markForDelete = true;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/09/01
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Explosion::Render()
{
	if (m_markForDelete)
	{
		return;
	}
	if (m_explosionAnimation == nullptr)
	{
		return;
	}
	Texture *explosionTexture = Texture::CreateOrGetTexture("Data\\Images\\explosion.png");

	AABB2 aabb2(m_position, m_radius / 2, m_radius / 2);// g_explosionAnim->GetCurrentTexCoords();
	Vector2 minPosition = m_explosionAnimation->GetCurrentTexCoords().mins;
	Vector2 maxPosition = m_explosionAnimation->GetCurrentTexCoords().maxs;
	Rgba tint((unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255);

	Material *defaultMaterial = Material::AquireResource("default");
	defaultMaterial->m_textures.at(0) = explosionTexture;
	Texture::SetCurrentTexture(explosionTexture);
	g_theRenderer->BindMaterial(defaultMaterial);

	g_theRenderer->DrawTexturedAABB(aabb2, explosionTexture, minPosition, maxPosition, tint);
	delete defaultMaterial;
}
