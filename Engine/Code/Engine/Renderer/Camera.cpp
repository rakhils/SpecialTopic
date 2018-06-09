#include "Engine/Renderer/Camera.hpp"
#include "Engine/ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Camera/OrbitCamera.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/Windows.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Camera*		Camera::s_defaultCamera		= nullptr;
Camera*		Camera::s_currentCamera		= nullptr;
Camera*		Camera::s_gamePlayCamera	= nullptr;
Camera*		Camera::s_effectCamera		= nullptr;
Camera*		Camera::s_uiCamera			= nullptr;

// CONSTRUCTOR
Camera::Camera()
{
	m_model.SetIdentity();
}

// DESTRUCTOR
Camera::~Camera()
{
	delete m_defaultFrameBuffer;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Looks at a particlar target from a pos position
*
*@param   : Camera position,Target position,World up vector(reference)
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Camera::LookAt(Vector3 pos, Vector3 target, Vector3 worldUP)
{
	Vector3 dir = target - pos;
	Vector3 forward = dir.GetNormalized();

	Vector3 right =  CrossProduct(worldUP,forward);
	right = right.GetNormalized();

	Vector3 Up = CrossProduct(forward, right);
	Up = Up.GetNormalized();

	Matrix44 LookAtMatrix = Matrix44(right, Up, forward);
	
	LookAtMatrix.Tx = 1 * DotProduct(pos, right);
	LookAtMatrix.Ty = 1 * DotProduct(pos, Up);
	LookAtMatrix.Tz = 1 * DotProduct(pos, forward);
	LookAtMatrix.Tw = 1;
	
	return LookAtMatrix;
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/18
*@purpose : Set projection matrix
*
*@param   : Matrix to be set
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::SetProjection(Matrix44 const &proj)
{
	m_projection = proj;
}
void Camera::SetOrthoProjection()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;

	Vector3 cameraPosition = m_transform.GetWorldPosition();
	Vector3 minCameraPosition = cameraPosition - Vector3(width / 2, height / 2, 0);
	Vector3 maxCameraPosition = cameraPosition + Vector3(width / 2, height / 2, 0);

	SetOrthoProjection(minCameraPosition.GetXY(),maxCameraPosition.GetXY(), 0.f, 100.f);
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/18
*@purpose : Set ortho projection
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::SetOrthoProjection(Vector2 bottomLeft,Vector2 topRigth, float nearZ, float farZ)
{
	float horizontalDistance = topRigth.x - bottomLeft.x;
	float verticalDistance   = topRigth.y - bottomLeft.y;
	float zDistance = farZ - nearZ;

	m_projection.SetIdentity();

	m_projection.Tx = -1 * (topRigth.x + bottomLeft.x) / (topRigth.x - bottomLeft.x);
	m_projection.Ty = -1 * (topRigth.y + bottomLeft.y) / (topRigth.y - bottomLeft.y);
	m_projection.Tz = -1 * (farZ + nearZ) / (farZ - nearZ);


	m_projection.Ix = 2/horizontalDistance;
	m_projection.Jy = 2/verticalDistance,
	m_projection.Kz = 2/zDistance;
	
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : Sets view matrix
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::SetViewMatrix(Matrix44 view)
{
	m_view = view;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::SetModelMatrix(Matrix44 model)
{
	m_model = model;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Camera::ScreenToWorld(Vector2 xy, float ndc_depth)
{
	UNUSED(ndc_depth);
	/*Vector3 NDC_XY = RangeMap(screen_xy, Vector2(0, 0), Vector2(resoulution_x(), resolution_y()), Vector2(-1, 1), Vector2(1, -1);

	Vector3 NDC = Vector3(ndc_xy, ndc_depth);

	Vector4 homogenious_world = Vector4(ndc, 1)*GetViewProjection().inverse();

	Vector3 worldPos = homogenius_world.xyz() / homogenuous_world.w;

	return world_pos;*/
	return Vector3::ONE;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::SetColorTarget(Texture *colorTarget)
{
	m_defaultFrameBuffer->SetColorTarget(colorTarget);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::SetDepthStencilTarget(Texture *stencilTarget)
{
	m_defaultFrameBuffer->SetDepthStencilTarget(stencilTarget);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
GLint Camera::GetFrameBufferHandle()
{
	return m_defaultFrameBuffer->GetHandle();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
IntVector2 Camera::GetDimensions()
{
	return m_defaultFrameBuffer->GetDimensions();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/17
*@purpose : Get camera left vector
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Camera::GetCameraRightVector()
{
	//Vector3 leftVector(m_view.Ix,m_view.Iy,m_view.Iz);
	Vector3 leftVector(m_view.Ix,m_view.Jx,m_view.Kx);
	return leftVector;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/17
*@purpose : returns Forward vector
*
*@param   : NIL
*
*@return  : camera forward vector
*/
//////////////////////////////////////////////////////////////
Vector3 Camera::GetCameraForwardVector()
{
	//Vector3 forwardVector(m_view.Kx, m_view.Ky, m_view.Kz);
	Vector3 forwardVector(m_view.Iz, m_view.Jz, m_view.Kz);

	return forwardVector;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Get camera's up vector
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Camera::GetCameraUpVector()
{
	//Vector3 forwardVector(m_view.Jx, m_view.Jy, m_view.Jz);
	Vector3 forwardVector(m_view.Iy, m_view.Jy, m_view.Ky);
	return forwardVector;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::MoveRigth(float deltaTime)
{
	Matrix44 localTransform = m_transform.GetLocalMatrix();
	Vector3 rigthDirection = localTransform.GetIAxis();

	m_transform.Translate(rigthDirection*deltaTime);
	SetModelMatrix(m_transform.GetLocalMatrix());
	m_targetPosition += rigthDirection;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::MoveLeft(float deltaTime)
{
	Matrix44 localTransform = m_transform.GetLocalMatrix();
	Vector3 rigthDirection = localTransform.GetIAxis();
	deltaTime = -1 * deltaTime;
	m_transform.Translate(rigthDirection*deltaTime);
	SetModelMatrix(m_transform.GetLocalMatrix());
	//m_transform.SetLocalPosition() -= rigthDirection;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::MoveUp(float deltaTime)
{
	UNUSED(deltaTime);
	Matrix44 localTransform = m_transform.GetLocalMatrix();
	Vector3 upDirection = localTransform.GetJAxis();

	m_transform.Translate(upDirection);
	SetModelMatrix(m_transform.GetLocalMatrix());
	m_targetPosition += upDirection;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::MoveDown(float deltaTime)
{
	UNUSED(deltaTime);
	Matrix44 localTransform = m_transform.GetLocalMatrix();
	Vector3 upDirection = localTransform.GetJAxis();

	m_transform.Translate(upDirection*-1);
	SetModelMatrix(m_transform.GetLocalMatrix());
	m_targetPosition -= upDirection;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/31
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::MoveForward(float deltaTime)
{
	Matrix44 localTransform  = m_transform.GetLocalMatrix();
	Vector3 forwardDirection = localTransform.GetKAxis();

	m_transform.Translate(forwardDirection*deltaTime);
	SetModelMatrix(m_transform.GetLocalMatrix());
	m_targetPosition += forwardDirection;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/31
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::MoveBackward(float deltaTime)
{
	Matrix44 localTransform = m_transform.GetLocalMatrix();
	Vector3 forwardDirection = localTransform.GetKAxis();
	deltaTime = -1 * deltaTime;
	m_transform.Translate(forwardDirection*deltaTime);
	SetModelMatrix(m_transform.GetLocalMatrix());
	m_targetPosition -= forwardDirection;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::RotateX(float angle)
{
	m_transform.RotateLocalByEuler(Vector3(angle, 0, 0));
	//SetModelMatrix(m_transform.GetLocalMatrix());
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::RotateY(float angle)
{
	m_transform.RotateLocalByEuler(Vector3(0, angle, 0));
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::RotateZ(float angle)
{
	m_transform.RotateLocalByEuler(Vector3(0, 0, angle));
	//SetModelMatrix(m_transform.GetLocalMatrix());
}


//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Camera::Finalize()
{
	m_defaultFrameBuffer->Finalize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Initlizes all types of camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Camera::InitCamera()
{
	int window_width  = Windows::GetInstance()->GetDimensions().x;
	int window_height = Windows::GetInstance()->GetDimensions().y;

	s_defaultCamera = new OrthographicCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	s_defaultCamera->m_defaultFrameBuffer = frameBuffer;
	s_defaultCamera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	s_defaultCamera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	s_defaultCamera->m_name = "default_camera";

	s_gamePlayCamera = new OrthographicCamera();
	FrameBuffer *gameplayFrameBuffer = new FrameBuffer();
	s_gamePlayCamera->m_defaultFrameBuffer = gameplayFrameBuffer;
	s_gamePlayCamera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	s_gamePlayCamera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	s_gamePlayCamera->m_name = "gameplay_camera";

	s_effectCamera = new OrbitCamera();
	FrameBuffer *effectCameraFrameBuffer = new FrameBuffer();
	s_effectCamera->m_defaultFrameBuffer = effectCameraFrameBuffer;
	s_effectCamera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	s_effectCamera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	s_effectCamera->m_name = "effect_camera";

	s_uiCamera = new OrthographicCamera();
	FrameBuffer *uiFrameBuffer = new FrameBuffer();
	s_uiCamera->m_defaultFrameBuffer = uiFrameBuffer;
	s_uiCamera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	s_uiCamera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	s_uiCamera->SetOrthoProjection(Vector2(0.f, 0.f), Vector2(static_cast<float>(window_width), static_cast<float>(window_height)), -100.f, 100.f);
	s_uiCamera->m_name = "ui_camera";

	//SetCurrentCamera(GetGamePlayCamera());
	SetCurrentCamera(GetUICamera());
	SetCurrentCamera(GetDefaultCamera());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/22
*@purpose : Sets camera as current camera
*@param   : Camera pointer
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void Camera::SetCamera(Camera* camera)
{
	m_currentCamera = camera;
}*/
