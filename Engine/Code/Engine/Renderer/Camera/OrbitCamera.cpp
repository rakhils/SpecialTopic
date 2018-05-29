#include "Engine/Renderer/Camera/OrbitCamera.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/EngineCommon.hpp"
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
void OrbitCamera::SetSphericalCords(Vector3 cameraPosition)
{
	Matrix44 lookAtMatrix = LookAt(m_targetPosition - ConvertSphericalToCartesian(cameraPosition),m_targetPosition,Vector3(0,1,0));
	SetViewMatrix(lookAtMatrix);
	lookAtMatrix.InvertFast();
	SetModelMatrix(lookAtMatrix);
}
