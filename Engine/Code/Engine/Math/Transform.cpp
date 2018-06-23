#include "Engine/Math/Transform.hpp"
#include "Engine/GameObject/GameObject.hpp"
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Returns the current transform matrix
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Transform_t::GetMatrix()
{
	if(m_isDirty)
	{
		Matrix44 translateMatrix = Matrix44::MakeTranslation3D(m_position);
		Matrix44 rotateMatrix    = Matrix44::MatrixFromEuler(m_euler);
		Matrix44 scaleMatrix	 = Matrix44::MakeScale3D(m_scale);

		Matrix44 modelMatrix;

		modelMatrix.MultiplyAndSet(translateMatrix);
		modelMatrix.MultiplyAndSet(rotateMatrix);
		modelMatrix.MultiplyAndSet(scaleMatrix);
		m_local = modelMatrix;
		
		m_isDirty = false;
	}
	return m_local;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::SetMatrix(Matrix44 &matrix)
{
	m_local    = matrix;

	m_euler    = matrix.GetEulerFromMatrix();
	m_position = matrix.GetTVector();
	m_scale    = Vector3(matrix.Ix, matrix.Jy, matrix.Kz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Sets the dirty flag
*
*@param   : value
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::SetDirty(bool value)
{
	m_isDirty = value;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Sets position of transform
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::SetPosition(Vector3 pos)
{
	m_position = pos;
	SetDirty(true);
}


//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Tranlsate given transform
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::Translate(Vector3 offset)
{
	m_position += offset;
	SetDirty(true);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Sets rotation euler
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::SetRotationEuler(Vector3 euler)
{
	m_euler = euler;
	SetDirty(true);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Rotates by euler 
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::RotateByEuler(Vector3 euler)
{
	m_euler +=euler;
	SetDirty(true);
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
void Transform_t::RotateByEuler(Vector3 euler, Vector3 clampMin, Vector3 clampMax)
{
	m_euler += euler;
	m_euler = ClampVector3(m_euler, clampMin, clampMax);
	SetDirty(true);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Scales transform by given scale factor
*
*@param   : Scale factor in each direction
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::Scale(Vector3 scaleFactor)
{
	m_scale += scaleFactor;
	SetDirty(true);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Sets scale
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform_t::SetScale(Vector3 scale)
{
	m_scale = scale;
	SetDirty(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

														//TRANSFORM CLASS//
														// CONSTRUCTOR
Transform::Transform()
{
	Matrix44 identity;
	identity.SetIdentity();
	m_localTransform.SetMatrix(identity);
}

// DESTRUCTOR
Transform::~Transform()
{
	if (m_gameObject != nullptr)
	{
		delete m_gameObject;
		m_gameObject = nullptr;
	}
	/*for (int childIndex = 0; childIndex < m_children.size(); childIndex++)
	{
		if (m_children.at(childIndex) != nullptr)
		{
			delete m_children.at(childIndex);
			m_children.at(childIndex) = nullptr;
		}
	}*/
	m_children.clear();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::SetLocalMatrix(Matrix44 &mat)
{
	m_localTransform.SetMatrix(mat);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::Translate(Vector3 offset)
{
	m_localTransform.Translate(offset);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::RotateLocalByEuler(Vector3 euler)
{
	m_localTransform.RotateByEuler(euler);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::RotateLocalByEuler(Vector3 euler, Vector3 clampMin, Vector3 clampMax)
{
	m_localTransform.RotateByEuler(euler, clampMin, clampMax);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::Scale(Vector3 scaleFactor)
{
	m_localTransform.Scale(scaleFactor);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::SetLocalPosition(Vector3 pos)
{
	m_localTransform.SetPosition(pos);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::SetLocalRotationEuler(Vector3 euler)
{
	m_localTransform.SetRotationEuler(euler);
	MakeChildrenDirty();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::SetLocalScale(Vector3 scale)
{
	m_localTransform.SetScale(scale);
	MakeChildrenDirty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : Transform local matrix to look at position
*@param   : local position to look at
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Transform::LocalLookAt(Vector3 localLookAtPosition, Vector3 localUp /*= Vector3::UP*/)
{
	Matrix44 lookAt = Matrix44::LookAt(GetLocalPosition(), localLookAtPosition, localUp);
	SetLocalMatrix(lookAt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : Transform matrix to look at position
*@param   : local position to look at
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Transform::WorldLookAt(Vector3 worldPosition, Vector3 worldUP /*= Vector3::UP*/)
{
	Matrix44 lookat = Matrix44::LookAt(GetWorldPosition(), worldPosition, worldUP);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Calculates world matrix and returns
*@param   : NIL
*@return  : returns world matrix
*//////////////////////////////////////////////////////////////
Matrix44 Transform::GetWorldMatrix()
{
	if (m_parent == nullptr)
	{
		return GetLocalMatrix();
	}
	Matrix44 tempParentWorldMatrix = m_parent->GetWorldMatrix();
	tempParentWorldMatrix.MultiplyAndSet(m_localTransform.GetMatrix());
	return tempParentWorldMatrix;
}

/*DATE    : 2018/05/16
*@purpose : Gets world position( not equal to local position if parent is not null)
*
*@param   : NIL
*
*@return  : Vector3 position in world
*/
//////////////////////////////////////////////////////////////
Vector3 Transform::GetWorldPosition()
{
	if (m_parent == nullptr)
	{
		return GetLocalPosition();
	}
	return GetWorldMatrix().GetTVector();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : fetch world rotation of transform
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Transform::GetWorldRotation()
{
	if(m_parent == nullptr)
	{
		return GetLocalEulerAngles();
	}
	Matrix44 tempMatrix = GetWorldMatrix();
	tempMatrix.MultiplyAndSet(GetLocalMatrix());
	return tempMatrix.GetEulerFromMatrix();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Gets world scale from matrix
*
*@param   : NIL
*
*@return  : Scale factor
*/
//////////////////////////////////////////////////////////////
Vector3 Transform::GetWorldScale()
{
	if(m_parent == nullptr)
	{
		return GetLocalScale();
	}
	Matrix44 tempMatrix = GetWorldMatrix();
	tempMatrix.MultiplyAndSet(GetLocalMatrix());
	return tempMatrix.GetDiagonalValues();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Makes all child transform dirty
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
void Transform::MakeChildrenDirty()
{
	for (size_t childIndex = 0; childIndex < m_children.size(); childIndex++)
	{
		m_children.at(childIndex)->m_localTransform.SetDirty(true);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : removes the current transform from hierarchy
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Transform::RemoveFromHierarcy()
{
	for(int childIndex = 0;childIndex < m_parent->m_children.size();childIndex++)
	{
		if(m_parent->m_children.at(childIndex) == this)
		{
			//delete this;
		}
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Adds child transforms
*@param   : child transform
*@return  : NIL
*/////////////////////////////////////////////////////////////
void Transform::AddChild(Transform *transform)
{
	transform->m_parent = this;
	m_children.push_back(transform);
}
