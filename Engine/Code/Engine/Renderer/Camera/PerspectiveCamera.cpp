#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
PerspectiveCamera::PerspectiveCamera()
{

}

// DESTRUCTOR
PerspectiveCamera::~PerspectiveCamera()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Updates camera
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PerspectiveCamera::Update(float deltaTime)
{
	UNUSED(deltaTime);
	float PI     = 3.14f;
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float heigth = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	float aspect =  width/heigth ;
	SetPerspectiveCamera(PI / 6.f, aspect, 0.1f, 300.f);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Set perspective view. Sets model and projection matrix of camera
*@param   : FovDegrees, aspect, nearZ and farZ 
*@return  : NIL
*//////////////////////////////////////////////////////////////
void PerspectiveCamera::SetPerspectiveCamera(float fovDegrees, float aspect, float nearz, float farz)
{
	Matrix44 modelMatrix = m_transform.GetWorldMatrix();
	m_model = modelMatrix;
	modelMatrix.Tx = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetIAxis());
	modelMatrix.Ty = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetJAxis());
	modelMatrix.Tz = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetKAxis());

	modelMatrix.InvertFast();
	SetViewMatrix(modelMatrix);
	
	Matrix44 perspectiveMatrix = Matrix44::MakePerspectiveMatrix(fovDegrees, aspect,nearz, farz);
	SetProjection(perspectiveMatrix);
}

