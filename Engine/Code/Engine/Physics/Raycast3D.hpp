#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Plane.hpp"
/*\class  : Raycast3D
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   
* \author : Rakhil  
* \version: 1.
* \date   : 5/18/2018 1:04:54 
* \contact: srsrakhil@gmail.com
*/
class Collider;
class RigidBody3D;
class GameObject;
struct RaycastHit
{
	Collider*		m_collider;
	Vector3			m_position;
	bool			m_hit;
	//RigidBody3D*	m_rigidBody;
};
struct Ray
{
	Vector3 m_start;
	Vector3 m_direction;
	float   m_maxDistance = 500;
	float   m_stepSize	  = 0.5;
	Vector3 Evaluate(float delta)
	{
		return m_start + m_direction * delta;
	}
};
class Raycast3D
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	Raycast3D();
	~Raycast3D();
	
	static RaycastHit Raycast(GameObject *gameObject,Ray ray);
	static Collider*  GetColliderAtPosition(GameObject *currentGameobject,Vector3 position);
	static RaycastHit RaycastOnPlane(Ray ray, Plane plane);
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