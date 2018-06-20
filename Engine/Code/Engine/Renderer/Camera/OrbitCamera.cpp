#include "Engine/Renderer/Camera/OrbitCamera.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
OrbitCamera::OrbitCamera()
{

}

// DESTRUCTOR
OrbitCamera::~OrbitCamera()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Updates orbit camera
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OrbitCamera::Update(float deltaTime)
{
	UNUSED(deltaTime);
	SetSphericalCords(m_transform.GetWorldPosition());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/20
*@purpose : Sets target position
*@param   : world position
*@return  : NIL
*//////////////////////////////////////////////////////////////
void OrbitCamera::SetTargetPosition(Vector3 targetPosition)
{
	m_targetPosition = targetPosition;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/20
*@purpose : Sets projection matrix and model matrix of this camera
*@param   : Camera position
*@return  : NIL
*//////////////////////////////////////////////////////////////
void OrbitCamera::SetSphericalCords(Vector3 position)
{
	Matrix44 lookAtMatrix = LookAt(m_targetPosition - ConvertSphericalToCartesian(position),m_targetPosition,Vector3(0,1,0));
	SetModelMatrix(lookAtMatrix);
	lookAtMatrix.InvertFast();
	SetViewMatrix(lookAtMatrix);
/*
	lookAtMatrix.Tx = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetIAxis());
	lookAtMatrix.Ty = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetJAxis());
	lookAtMatrix.Tz = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetKAxis());
	SetModelMatrix(lookAtMatrix);*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OrbitCamera::SetCameraPositionInSpherical(Vector3 position)
{
	m_position = position;
}
