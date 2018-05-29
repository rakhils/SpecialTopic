#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/GameObject/GameObject.hpp"
// CONSTRUCTOR
RigidBody3D::RigidBody3D()
{
	m_gravity = Vector3(0.f, 9.8f, 0.f);
}

// DESTRUCTOR
RigidBody3D::~RigidBody3D()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
void RigidBody3D::Update(float deltaTime)
{
	ApplyGravity(deltaTime);
	ApplyFriction(deltaTime);
	m_velocity				= m_acceleration * deltaTime;
	Vector3 displacement	=  m_velocity     * deltaTime;
	Vector3 currentPosition = m_gameObject->m_transform.GetLocalPosition();
	m_gameObject->m_transform.SetLocalPosition(currentPosition + displacement);
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Applies friction force 
*@param   : deltatime
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RigidBody3D::ApplyFriction(float deltaTime)
{
	if(m_velocity == Vector3::ZERO)
	{
		return;
	}
	Vector3 frictionForce = m_velocity * (-1 * m_friction * m_mass * m_gravity.GetLength());
	ApplyForce(frictionForce, deltaTime);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Applies gravity to the object
*@param   : delta time
*@return  : NIL
*/////////////////////////////////////////////////////////////
void RigidBody3D::ApplyGravity(float deltaTime)
{
	if(m_useGravity)
	{
		ApplyForce(m_gravity, deltaTime);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Applies force in a direction
*@param   : force,deltatime
*@return  : NIL
*/////////////////////////////////////////////////////////////
void RigidBody3D::ApplyForce(Vector3 forceDirection, float deltaTime)
{
	/*
	Vector3 acceleration = forceDirection * (forceMagnitude / m_mass);
	m_velocity           += acceleration*deltaTime;
	m_speed              = m_velocity.GetLength();
	m_velocity           = m_velocity.GetNormalized*/

	m_acceleration += deltaTime *(forceDirection / m_mass );
}
