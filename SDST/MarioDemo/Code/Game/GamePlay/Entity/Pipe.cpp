#include "Game/GamePlay/Entity/Pipe.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
Pipe::Pipe(std::string name) :Entity(name)
{

}

Pipe::Pipe(std::string name, EntityDefinition *definition) : Entity(name)
{
	m_definition = definition;
	m_spriteAnimSet = new SpriteAnimSet(definition->m_spriteAnimSetDef);
	m_spriteAnimSet->SetAnimation("IDLE");
}

// DESTRUCTOR
Pipe::~Pipe()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pipe::Update(float deltaTime)
{
	Entity::Update(deltaTime);
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
void Pipe::Render()
{
	/*Material *defaultMaterial = Material::AquireResource("default");
	m_texture = Texture::CreateOrGetTexture("Data\\Images\\pipe.jpg", true, true);
	defaultMaterial->m_textures.at(0) = m_texture;
	Texture::SetCurrentTexture(m_texture);
	Renderer::GetInstance()->BindMaterial(defaultMaterial);
	Entity::Render();
	Vector2 entityPosition = m_transform.GetWorldPosition().GetXY();
	AABB2 m_aabb2(entityPosition, GetDrawDimensions().x / 2, GetDrawDimensions().y / 2);
	Renderer::GetInstance()->DrawAABB(m_aabb2, Rgba::WHITE);
	delete defaultMaterial;*/
}
