#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Debug/DebugDraw.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"
// CONSTRUCTOR
BoxCollider2D::BoxCollider2D()
{
	m_normalX = Vector3(1, 0, 0);
	m_normalY = Vector3(0, 1, 0);
	m_normalZ = Vector3(0, 0, 1);
}

// DESTRUCTOR
BoxCollider2D::~BoxCollider2D()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/26
*@purpose : Set the normal
*@param   : New normal
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider2D::SetNormal(Vector3 normal)
{
	m_normal = normal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider2D::Update(float deltaTime)
{
	UNUSED(deltaTime);
	//m_normal = ConvertSphericalToCartesian(m_transform.GetWorldRotation());
	m_normalX = m_transform.GetWorldMatrix().GetIAxis();
	m_normalY = m_transform.GetWorldMatrix().GetJAxis();
	m_normalZ = m_transform.GetWorldMatrix().GetKAxis();

	Vector3 position = m_transform.GetWorldPosition();
	DebugRenderOptions options;
	options.m_lifeTime = 0.2f;
	TODO("Camera setting for debug draws");
	DebugDraw::GetInstance()->SetDebugRender3DCamera(Camera::GetGamePlayCamera());
	DebugDraw::GetInstance()->m_renderer = Renderer::GetInstance();
	DebugDraw::GetInstance()->DebugRenderLineSegment(position, Rgba::RED, position + m_normalX * 5, Rgba::RED, options);
	DebugDraw::GetInstance()->DebugRenderLineSegment(position, Rgba::RED, position + m_normalY * 5, Rgba::GREEN, options);
	DebugDraw::GetInstance()->DebugRenderLineSegment(position, Rgba::RED, position + m_normalZ * 5, Rgba::BLUE, options);



	std::map<std::string, GameObject*>::iterator itGameObject;
	for (itGameObject = GameObject::s_gameObjects.begin(); itGameObject != GameObject::s_gameObjects.end(); itGameObject++)
	{
		GameObject *gameObject = itGameObject->second;
		if (gameObject == m_gameObject)
		{
			continue;
		}
		std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
		for (it = gameObject->m_colliderComponents.begin(); it != gameObject->m_colliderComponents.end(); it++)
		{
			if (it->second == this)
			{
				continue;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Checks if point is inside box collider
*@param   : world position
*@return  : return true of point inside box2d collder else false
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool BoxCollider2D::IsPointInside(Vector3 point)
{
	Vector3 centrePosition = GetCenterPosition();

	Vector3 direction = point - centrePosition;
	//direction		  = direction.GetNormalized();

	Matrix44 worldMatrix = m_transform.GetWorldMatrix();
	worldMatrix.InvertFast();
	Vector3 localPosition = worldMatrix.Multiply(direction);

	Vector3 xMaxs = centrePosition + Vector3(1,0,0) * offsetX;
	Vector3 xMins = centrePosition - Vector3(1,0,0) * offsetX;

	Vector3 yMaxs = centrePosition + Vector3(0,1,0) * offsetY;
	Vector3 yMins = centrePosition - Vector3(0,1,0) * offsetY;

	Vector3 zMaxs = centrePosition + Vector3(0,0,1) * offsetZ;
	Vector3 zMins = centrePosition - Vector3(0,0,1) * offsetZ;


	//Vector2 xMinMax(centrePosition.x - offsetX,centrePosition.x + offsetX);
	//Vector2 yMinMax(centrePosition.y - offsetY,centrePosition.y + offsetY);
	//Vector2 zMinMax(centrePosition.z - offsetZ,centrePosition.z + offsetZ);
	point = localPosition;
	if(point.x > xMins.x && point.x < xMaxs.x)
	{
		if(point.y > yMins.y && point.y < yMaxs.y)
		{
			if(point.z > zMins.z && point.z < zMaxs.z)
			{
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/19
*@purpose : Adds normal to the collider
*@param   : normal
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider2D::AddNormals(Vector3 normalX,Vector3 normaly,Vector3 normalz)
{
	m_normalX = normalX;
	m_normalY = normaly;
	m_normalZ = normalz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : on collision starts with other collider
*@param   : other collider
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider2D::OnCollisionEnter(Collider *collider)
{
	UNUSED(collider);
}
