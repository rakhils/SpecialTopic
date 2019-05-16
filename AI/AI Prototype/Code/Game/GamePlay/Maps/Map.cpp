#include "Game/GamePlay/Maps/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GamePlay/Tiles/Tiles.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Materials/Material.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Debug/DebugDraw.hpp"

Map::Map()
{
	Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Init the map
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::Initialize()
{
	InitCamera();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/21
*@purpose : Inits camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::InitCamera()
{
	int width  = Windows::GetInstance()->GetDimensions().x;
	int height = Windows::GetInstance()->GetDimensions().y;
	m_cameraPosition = Vector3::ZERO;
	m_camera   = new PerspectiveCamera();
	FrameBuffer *frameBuffer = new FrameBuffer();
	m_camera->m_defaultFrameBuffer = frameBuffer;
	m_camera->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	m_camera->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	m_camera->m_transform.SetLocalPosition(Vector3(static_cast<float>(width / 2), static_cast<float>(height / 2), 0));
	Camera::SetGameplayCamera(m_camera);
	Camera::SetCurrentCamera(m_camera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Process all inputs
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::ProcessInputs(float deltaTime)
{
	UNUSED(deltaTime);
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouseClientPosition();
	mousePosition.y = Windows::GetInstance()->GetDimensions().y - mousePosition.y;

	Vector2 delta = g_theInput->GetMouseDelta();
	if (delta.x != 0)
	{
		m_rotationY -= delta.x / 2.5f;
		m_rotationY = fmodf(m_rotationY, 360.f);
	}

	if (delta.y != 0)
	{
		m_rotationX -= delta.y / 2.5f;
		m_rotationX = ClampFloat(m_rotationX, -90.f, 90.f);
	}

	

	m_cameraMatrix.SetIdentity();

	m_cameraMatrix.Append(Matrix44::MakeRotationDegreesInY(m_rotationY));
	m_cameraMatrix.Append(Matrix44::MakeRotationDegreesInX(m_rotationX));
	m_cameraMatrix.Append(Matrix44::MakeRotationDegreesInZ(m_rotationZ));

	

	Vector3 xyPlaneForward ( CosDegrees(m_rotationY),  SinDegrees(m_rotationY), 0);
	Vector3 xyPlaneRight   (-SinDegrees(m_rotationY), CosDegrees(m_rotationY), 0);

	

	Vector3 forward = m_cameraMatrix.GetKVector();
	Vector3 right   = m_cameraMatrix.GetJVector();
	

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_A))
	{
		m_cameraPosition += xyPlaneRight * (-m_cameraSpeed * deltaTime);
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_D))
	{
		m_cameraPosition += xyPlaneRight * (m_cameraSpeed * deltaTime);
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_W))
	{
		m_cameraPosition += (forward*(m_cameraSpeed * deltaTime));
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_S))
	{
		m_cameraPosition += (forward*(-m_cameraSpeed * deltaTime));
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_Q))
	{
		m_cameraPosition.z += deltaTime * m_cameraSpeed;
	}
	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_E))
	{
		m_cameraPosition.z -= deltaTime * m_cameraSpeed;
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_E))
	{
		m_cameraPosition.z -= deltaTime * m_cameraSpeed;
	}

	if (g_theInput->isKeyPressed(InputSystem::KEYBOARD_R))
	{
		m_cameraPosition = Vector3::ZERO;
		m_rotationX = 0;
		m_rotationY = 0;
		m_rotationZ = 0;
	}

	m_cameraMatrix.Translate3D(m_cameraPosition);
	m_camera->m_transform.SetLocalMatrix(m_cameraMatrix);



	Matrix44 cameraMatrix;
	cameraMatrix.SetIdentity();

	cameraMatrix.Append(Matrix44::MakeRotationDegreesInX(-m_rotationX));
	cameraMatrix.Append(Matrix44::MakeRotationDegreesInY(-m_rotationY));
	cameraMatrix.Append(Matrix44::MakeRotationDegreesInZ(-m_rotationZ));

	cameraMatrix.Translate3D(-1*m_cameraPosition);

	m_camera->m_cameraMatrix = cameraMatrix;

	m_camera->Update(deltaTime);
}

void Map::Update(float deltaTime)
{
	ProcessInputs(deltaTime);
	UpdateCamera(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/08/31
*@purpose : Updates camera
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::UpdateCamera(float deltaTime)
{
	UNUSED(deltaTime);
	//m_camera->SetOrthoProjection();
}

void Map::Render()
{
	Camera::SetCurrentCamera(m_camera);
	Renderer::GetInstance()->BeginFrame();	

	Vector3 arrow(10, 0, 0);
	Material *textMaterial = Material::AquireResource("default");
	Renderer::GetInstance()->BindMaterial(textMaterial);

	//DebugDraw::GetInstance()->DebugRenderLine(Vector3::ZERO, Vector3(10, 0, 0), Rgba::RED,0.f,DEBUG_RENDER_USE_DEPTH);
	//DebugDraw::GetInstance()->DebugRenderLine(Vector3::ZERO, Vector3(0, 10, 0), Rgba::GREEN, 0.f, DEBUG_RENDER_USE_DEPTH);
	//DebugDraw::GetInstance()->DebugRenderLine(Vector3::ZERO, Vector3(0,  0, 10), Rgba::BLUE, 0.f, DEBUG_RENDER_USE_DEPTH);

	Renderer::GetInstance()->DrawLine(Vector3::ZERO, Vector3(10, 0, 0), Rgba::RED);
	Renderer::GetInstance()->DrawLine(Vector3::ZERO, Vector3(0, 10, 0), Rgba::GREEN);
	Renderer::GetInstance()->DrawLine(Vector3::ZERO, Vector3(0, 0, 10), Rgba::BLUE);


	for (int index = -10; index < 10; index++)
	{
		Renderer::GetInstance()->DrawLine(Vector3(-10,0,index), Vector3(10, 0, index), Rgba::YELLOW);
	}
	for (int index = -10; index < 10; index++)
	{
		Renderer::GetInstance()->DrawLine(Vector3(index,0,-10), Vector3(index,0,10), Rgba::YELLOW);
	}


	//DebugDraw::GetInstance()->DebugRenderBasis(Vector3::ZERO, Vector3::ZERO, 0.f, 10);

	Renderer::GetInstance()->DrawCube(Vector3(20,0,0), 5, Rgba::WHITE);

	delete textMaterial;

	Material* textMaterial1 = Material::AquireResource("Data\\Materials\\text.mat");
	Renderer::GetInstance()->BindMaterial(textMaterial1);
	Camera::SetCurrentCamera(Camera::GetUICamera());

	Vector3 cameraPosition = m_cameraPosition;


	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(50, 220, 0), Vector3::RIGHT, Vector3::UP, "CAMERA POS " + ToString(cameraPosition.x) + "," + ToString(cameraPosition.y) + "," + ToString(cameraPosition.z), 8, Rgba::WHITE);

	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(50, 200, 0), Vector3::RIGHT, Vector3::UP, "CAMERA ROT " + ToString(m_rotationX) + "," + ToString(m_rotationY) + "," + ToString(m_rotationZ), 8, Rgba::WHITE);

	Vector3 Ivector = m_cameraMatrix.GetIVector();
	Vector3 Jvector = m_cameraMatrix.GetJVector();
	Vector3 Kvector = m_cameraMatrix.GetKVector();

	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(50, 180, 0), Vector3::RIGHT, Vector3::UP, "CAMERA I " + ToString(Ivector), 8, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(50, 160, 0), Vector3::RIGHT, Vector3::UP, "CAMERA J " + ToString(Jvector), 8, Rgba::WHITE);
	Renderer::GetInstance()->DrawTextOn3DPoint(Vector3(50, 140, 0), Vector3::RIGHT, Vector3::UP, "CAMERA K " + ToString(Kvector), 8, Rgba::WHITE);


	delete textMaterial1;
}
