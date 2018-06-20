#pragma once
#include <vector>
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Matrix44.hpp"
/*\class  : Transform		   
* \group  : <GroupName>	   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/29/2018 6:39:16 PM
* \contact: srsrakhil@gmail.com
*/
struct Transform_t
{
	Transform_t(): m_position(0.0f), m_euler(0.0f), m_scale(1.0f) {}

	Matrix44	   m_local;

	bool		   m_isDirty = false;
	Vector3		   m_position;
	Vector3		   m_euler;
	Vector3		   m_scale;


	Matrix44	   GetMatrix();
	void		   SetMatrix(Matrix44 &mat);
	
	void		   SetDirty(bool value);

	void		   Translate(Vector3 offset);
	void		   RotateByEuler(Vector3 euler);
	void		   RotateByEuler(Vector3 euler, Vector3 clampMin, Vector3 clampMax);
	void		   Scale(Vector3 scaleFactor);

	void		   SetPosition(Vector3 position);
	Vector3		   GetPosition() const     { return m_position;}

	void		   SetRotationEuler(Vector3 euler);
	Vector3		   GetEulerAngles() const   { return m_euler; };

	void		   SetScale(Vector3 scale);
	Vector3		   GetScale() const			{ return m_scale; };

	// STATICS
	static Transform_t const IDENTITY;
};
class GameObject;
class Transform
{

public:
	//Member_Variables
	std::vector<Transform*> m_children;
	Transform			    *m_parent     = nullptr;
	GameObject				*m_gameObject = nullptr;
	//Static_Member_Variables

	//Methods

	Transform();
	~Transform();
	
	Matrix44		GetLocalMatrix()														 { return m_localTransform.GetMatrix(); }
	void			SetLocalMatrix(Matrix44 &mat);
																				 
	void			Translate(Vector3 offset);
	void			RotateLocalByEuler(Vector3 euler);
	void			RotateLocalByEuler(Vector3 euler, Vector3 clampMin, Vector3 clampMax);
	void			Scale(Vector3 scaleFactor);

	void			SetLocalPosition(Vector3 pos);
	Vector3			GetLocalPosition() const { return m_localTransform.GetPosition(); }

	void			SetLocalRotationEuler(Vector3 euler);
	Vector3			GetLocalEulerAngles() const 											 { return m_localTransform.GetEulerAngles(); }
																				 
	void			SetLocalScale(Vector3 scale);
	Vector3			GetLocalScale() const													 { return m_localTransform.GetScale(); }

	void			LocalLookAt(Vector3 localPosition, Vector3 localUp = Vector3::UP);
	void			WorldLookAt(Vector3 worldPosition, Vector3 worldUP = Vector3::UP);

	Matrix44		GetWorldMatrix();
	Vector3			GetWorldPosition();
	Vector3			GetWorldRotation();
	Vector3	        GetWorldScale();

	void			MakeChildrenDirty();
	void			RemoveFromHierarcy();
	void			AddChild(Transform *transform);
	//Static_Methods

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables
	Transform_t m_localTransform; 
	//Static_Member_Variables

	//Methods

	//Static_Methods

};