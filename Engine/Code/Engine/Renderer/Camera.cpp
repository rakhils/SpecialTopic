#include "Engine/Renderer/Camera.hpp"
#include "Engine/ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/Camera/OrbitCamera.hpp"
#include "Engine/Renderer/Camera/OrthographicCamera.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Math/MathUtil.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Camera*		Camera::s_defaultCamera		= nullptr;
Camera*		Camera::s_currentCamera		= nullptr;
Camera*		Camera::s_gamePlayCamera	= nullptr;
Camera*		Camera::s_effectCamera		= nullptr;
Camera*		Camera::s_uiCamera			= nullptr;

std::map<std::string, Camera*> Camera::s_cameras;
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

	Vector3 Up = CrossProduct(forward,right);
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
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float height = static_cast<float>(Windows::GetInstance()->GetDimensions().y);

	width -= m_xZoomIn;
	height -= m_yZoomIn;

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
/*DATE    : 2018/03/30
*@purpose : Set perspective view. Sets model and projection matrix of camera
*@param   : FovDegrees, aspect, nearZ and farZ
*@return  : NIL
*//////////////////////////////////////////////////////////////
void Camera::SetPerspective(float fovDegrees, float aspect, float nearz, float farz)
{
	Matrix44 modelMatrix = m_transform.GetWorldMatrix();
	m_model = modelMatrix;
	modelMatrix.Tx = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetIVector());
	modelMatrix.Ty = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetJVector());
	modelMatrix.Tz = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetKVector());

	modelMatrix.InvertFast();
	SetViewMatrix(modelMatrix);

	Matrix44 perspectiveMatrix = Matrix44::MakePerspectiveMatrix(fovDegrees, aspect, nearz, farz);
	SetProjection(perspectiveMatrix);
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
	//m_transform.SetLocalMatrix(model);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/07
*@purpose : Converts screen cords to world cords with respect to current camera
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
Vector3 Camera::ScreenToWorld(Vector2 screen_xy, float ndc_depth)
{
	Vector2 resolution = Windows::GetInstance()->GetDimensions().GetAsVector2();

	Vector2 NDC_XY = RangeMap(screen_xy, Vector2(0, 0), resolution, Vector2(-1.f, 1.f), Vector2(1.f, -1.f));

	Vector3 NDC    = Vector3(NDC_XY, ndc_depth);

	Vector4 homogenious_world = Vector4(NDC, 1);
	Matrix44 viewProjection = GetViewProjection();

	// INverse then dot
	viewProjection.Inverse();

	//viewProjection.Tx = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetIAxis());
	//viewProjection.Ty = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetJAxis());
	//viewProjection.Tz = DotProduct(m_transform.GetWorldPosition(), m_transform.GetWorldMatrix().GetKAxis());


	//viewProjection.InvertFast();
	homogenious_world = Matrix44::Multiply(viewProjection,homogenious_world);
	
	Vector3 worldPos = homogenious_world.XYZ() / homogenious_world.w;
	//worldPos.z = worldPos.z*-1;
	return worldPos;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PickRay Camera::GetPickRayFromScreenCords(Vector2 screenXY)
{
	Vector3 start = ScreenToWorld(screenXY, -1);
	Vector3 end   = ScreenToWorld(screenXY,  1);
	Vector3 direction = end - start;
	PickRay ray(start, direction.GetNormalized());
	return ray;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix44 Camera::GetViewProjection()
{
	Matrix44 result;
	result.MultiplyAndSet(m_projection);
	result.MultiplyAndSet(m_view);
	return result;
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
	Vector3 rigthDirection = localTransform.GetIVector();

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
	Vector3 rigthDirection = localTransform.GetIVector();
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
	Vector3 upDirection = localTransform.GetJVector();

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
	Vector3 upDirection = localTransform.GetJVector();

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
	Vector3 forwardDirection = localTransform.GetKVector();

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
	Vector3 forwardDirection = localTransform.GetKVector();
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Get camera's frustum object
*@param   : NIL
*@return  : Frustum
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Frustum Camera::GetFrustum()
{
	Frustum f;
	return f;
	//return Frustum::FromMatrix(m_projection*m_view);
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
/*DATE    : 2018/06/12
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Camera* Camera::CreateOrGetCamera(std::string name,CAMERA_TYPE type)
{
	if (s_cameras[name] != nullptr)
	{
		return s_cameras[name];
	}
	Camera *camera;
	switch (type)
	{
	case ORTHOGRAPHIC:
		camera = new OrthographicCamera();
		break;
	case ORBIT:
		camera = new OrbitCamera();
		break;
	case PERSPECTIVE:
		camera = new PerspectiveCamera();
		break;
	default:
		camera = new PerspectiveCamera();
		break;
	}
	FrameBuffer *cFrameBuffer = new FrameBuffer();
	camera->m_defaultFrameBuffer = cFrameBuffer;
	camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	return camera;
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
