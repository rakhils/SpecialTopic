#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Component.hpp"
/*\class  : RigidBody3D	   
* \group  : <GroupName>   
* \brief  :   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 5/17/2018 2:38:34 AM
* \contact: srsrakhil@gmail.com
*/
 
class RigidBody3D : public Component
{

public:
	//Member_Variables
	Transform					m_previousTransform;
	bool						m_useGravity			= false;
	bool						m_isKinematic			= false;
	float						m_speed;
	float						m_mass					= 1.f;
	float						m_friction				= 1.f;
	Vector3						m_acceleration;
	Vector3						m_velocity; 
	Vector3						m_gravity;
	//Static_Member_Variables

	//Methods

	RigidBody3D();
	~RigidBody3D();

	void Update(float deltaTime);
	void ApplyFriction(float deltaTime);
	void ApplyGravity(float deltaTime);
	void AddForce(Vector3 force, float deltaTime);
	void ApplyForce(Vector3 force, float deltaTime);

	Vector3 GetVelocity(float deltaTime);
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};