#pragma once
#include "Engine/Math/Vector3.hpp"
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
struct RaycastHit
{
	Collider*		m_collider;
	Vector3			m_position;
	bool			m_hit;
	//RigidBody3D*	m_rigidBody;
};
class Raycast3D
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	Raycast3D();
	~Raycast3D();
	
	static RaycastHit Raycast(Collider *collider,Vector3 startPosition, Vector3 direction, float maxDistance, float deltaLength);
	static Collider*  GetColliderAtPosition(Vector3 positiono);
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