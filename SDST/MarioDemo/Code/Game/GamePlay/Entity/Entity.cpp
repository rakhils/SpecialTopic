#include "Entity.hpp"
#include "Engine/Renderer/Materials/Material.hpp"

Entity::Entity(std::string name) : GameObject(name) 
{
	
}

Entity::Entity(std::string name,Vector2 position) : GameObject(name)
{
	m_transform.SetLocalPosition(Vector3(position, 0.f));
}

Vector2 Entity::GetForwardUnitVector()
{
	return m_transform.GetWorldMatrix().GetIAxis().GetXY();
}

Vector2 Entity::GetCenter()
{
	return m_transform.GetWorldPosition().GetXY();
}

void Entity::Update(float deltaTime)
{
	m_spriteAnimSet->update(deltaTime);
	GameObject::Update(deltaTime);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Entity::UpdateVelocity(float deltaTime)
{

}

void Entity::Render()
{
	Vector2 minPosition = m_spriteAnimSet->GetCurrentUVs().mins;
	Vector2 maxPosition = m_spriteAnimSet->GetCurrentUVs().maxs;
	Vector2 entityPosition = m_transform.GetWorldPosition().GetXY();
	AABB2 m_aabb2(entityPosition,GetDrawDimensions().x/2,GetDrawDimensions().y/2);

	g_theRenderer->DrawTexturedAABB(m_aabb2, m_texture, minPosition , maxPosition,Rgba::WHITE);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/04
*@purpose : Sets position of entity
*
*@param   : position
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Entity::SetPosition(Vector2 position)
{
	m_transform.SetLocalPosition(Vector3(position, 0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 Entity::GetDrawDimensions()
{
	return Vector2(m_definition->m_length, m_definition->m_height);
}

