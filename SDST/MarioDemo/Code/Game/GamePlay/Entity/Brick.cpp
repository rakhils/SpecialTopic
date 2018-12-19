#include "Game/GamePlay/Entity/Brick.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
// CONSTRUCTOR
Brick::Brick(std::string name):Entity(name)
{

}

Brick::Brick(std::string name,EntityDefinition *definition) :Entity(name)
{
	m_definition = definition;
	m_spriteAnimSet = new SpriteAnimSet(definition->m_spriteAnimSetDef);
	m_spriteAnimSet->SetAnimation("IDLE");
}

// DESTRUCTOR
Brick::~Brick()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Brick::Update(float deltaTime)
{
	UNUSED(deltaTime);
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
	Material *defaultMaterial = Material::AquireResource("default");\
	//std::string filePath = 
	m_texture = Texture::CreateOrGetTexture("Data\\Images\\brick.jpg",true,true);
	defaultMaterial->m_textures.at(0) = m_texture;
	Texture::SetCurrentTexture(m_texture);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Entity::Render();
	/*Vector2 entityPosition = m_transform.GetWorldPosition().GetXY();
	AABB2 m_aabb2(entityPosition, GetDrawDimensions().x / 2, GetDrawDimensions().y / 2);
	Renderer::GetInstance()->DrawAABB(m_aabb2, Rgba::WHITE);*/
	delete defaultMaterial;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Brick::OnCollisionEnter(Collider *collider)
{
	UNUSED(collider);
}
