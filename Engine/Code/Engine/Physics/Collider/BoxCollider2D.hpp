#pragma once
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : BoxCollider2D		   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman	   
* \version: 1.0	   
* \date   : 5/18/2018 12:47:38 AM
* \contact: srsrakhil@gmail.com
*/
class BoxCollider2D : public Collider
{

public:
	//Member_Variables
	float   offsetX			= 1.f;
	float	offsetY			= 1.f;
	float	offsetZ			= 1.f;
	Vector3 m_normalX;
	Vector3 m_normalY;
	Vector3 m_normalZ;
	Vector3 m_normal;
	//Static_Member_Variables

	//Methods

	BoxCollider2D();
	~BoxCollider2D();
	
	void SetNormal(Vector3 normal);
	void Update(float deltaTime);
	bool IsPointInside(Vector3 point);
	void AddNormals(Vector3 normalX,Vector3 normaly,Vector3 normalz);
	void OnCollisionEnter(Collider *collider);
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