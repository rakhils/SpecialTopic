#include "Tiles.hpp"


Tiles::Tiles(std::string type)
{
	m_type = type;
}

void Tiles::Render()
{
	Texture *m_texture = Texture::CreateOrGetTexture("Data\\Images\\terrain.png");	
	if(spriteSheet==nullptr)
	{
		spriteSheet = new SpriteSheet(*m_texture,8,8);
	}

	Vector2 spriteCoords = TileDefinition::getDefinition(m_type)->m_spriteCords;
	Vector2 spriteLayout = TileDefinition::getDefinition(m_type)->m_spriteLayout;
	Vector2 minPosition  = Vector2(spriteCoords.x/spriteLayout.x,(spriteCoords.y + 1)/spriteLayout.y );//     TileDefinition::getDefinit;
	Vector2 maxPosition  = Vector2((spriteCoords.x + 1)/spriteLayout.x,spriteCoords.y/spriteLayout.y); //     Vector2(1,0);/ns(m_type)->maxposition;
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;
	float alphaVal = 1.0f;
	Rgba tint;
	tint.SetAsFloats(red,green,blue,alphaVal);
	AABB2 aabb2;
	aabb2.mins = Vector2(m_tileCoords.x - m_width/2,m_tileCoords.y - m_width/2);
	aabb2.maxs = Vector2(m_tileCoords.x + m_width/2,m_tileCoords.y + m_width/2);

	g_theRenderer->DrawTexturedAABB(aabb2, m_texture, minPosition , maxPosition,tint);
}

void Tiles::UpdateTileCoords(int x, int y)
{
	/*m_tileCoords.x = x;
	m_tileCoords.y = y;
	AABB2 tempaabb2(Vector2(x,y),1/2,1/2);
	m_aabb2 = tempaabb2;*/
	/*Vector2 positionVector(x,y);
	//positionVector += (1/2); 
	
	//m_centreX = positionVector.x+m_length/2;
	//m_centreY = positionVector.y+m_bredth/2;

	//positionVector += (m_width/2); 
	m_tileCoords = positionVector;
	m_centreX = positionVector.x;//+m_width/2;
	m_centreY = positionVector.y;//+m_width/2;
	//AABB2 tempaabb2(positionVector,m_width/2,m_width/2);
	//m_aabb2 = tempaabb2;
*/




	m_tileCoords.x = static_cast<float>(x);
	m_tileCoords.y = static_cast<float>(y);
	AABB2 tempaabb2(m_tileCoords,0.5f,0.5f);
	m_aabb2 = tempaabb2;
	
}

