#include "Game/GamePlay/Entity/Brick.hpp"

// CONSTRUCTOR
Brick::Brick():Entity("brick")
{

}

Brick::Brick(EntityDefinition *definition) :Entity("brick")
{
	m_spriteAnimSet = new SpriteAnimSet(definition->m_spriteAnimSetDef);
	m_spriteAnimSet->SetAnimation("IDLE");
}

// DESTRUCTOR
Brick::~Brick()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/04
*@purpose : Renders bricks
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Brick::Render()
{
	m_texture = Texture::CreateOrGetTexture("Data\\Images\\brick_block.png");
	Entity::Render();
}
