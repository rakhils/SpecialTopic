#include "Engine/Renderer/Renderable.hpp"
#include "Game/GamePlay/Entity/Asteroid.hpp"
// CONSTRUCTOR
Asteroid::Asteroid() : GameObject("default")
{
	//m_speed = GetRandomFloatInRange(0, 0.5);
	//m_velocity *= m_speed;
}

// DESTRUCTOR
Asteroid::~Asteroid()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Asteroid::Update(float deltaTime)
{
	//m_transform.TranslateLocal(m_forward*(m_velocity*deltaTime));
	m_renderable->m_modelMatrix = m_transform.GetLocalMatrix();
	//if(m_transform.GetLocalPosition().GetLength() > 100)
	//{
	//	m_forward = m_forward * -1;
	//	m_velocity = m_forward;
	//}
//	m_velocity = ClampVector3(m_velocity, Vector3::ONE*-1,Vector3::ONE*1);

	GameObject::Update(deltaTime);
}
