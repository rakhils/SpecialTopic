#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/SceneManagement/Scene.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Physics/RigidBody3D.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
#include "Engine/Physics/Collider/SphereCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider.hpp"
#include "Engine/Physics/Collider/BoxCollider2D.hpp"
#include "Engine/Physics/Collider/CircleCollider.hpp"
#include "Engine/Audio/AudioComponent.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::map<std::string, GameObject*>     GameObject::s_gameObjects;
// CONSTRUCTOR
GameObject::GameObject(std::string name)
{
	m_name = name;
	m_renderable   = new Renderable();
	m_creationTime = static_cast<float>(Clock::g_theMasterClock->total.m_seconds);
	CreateGameObject(name,this);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Creates gameobject with scene
*@param   : name and scene pointer
*@return  : NIL
*/////////////////////////////////////////////////////////////
GameObject::GameObject(std::string name, Scene *scene)
{
	m_name  = name;
	m_scene = scene;
	m_renderable = new Renderable();
	m_creationTime = static_cast<float>(Clock::g_theMasterClock->total.m_seconds);
	CreateGameObject(name,this);
}

// DESTRUCTOR
GameObject::~GameObject()
{
	m_scene->RemoveRenderable(m_renderable);
	if (m_renderable != nullptr)
	{
		delete m_renderable;
		m_renderable = nullptr;
	}
	std::map<COMPONENT_COLLIDER_TYPE, Collider*>::iterator it;
	for (it = m_colliderComponents.begin(); it != m_colliderComponents.end();it++)
	{
		delete it->second;
		it->second = nullptr;
	}
	m_colliderComponents.clear();
	m_transform;
	std::map<COMPONENT_TYPE, Component*>::iterator itcomponent;
	for (itcomponent = m_components.begin(); itcomponent != m_components.end(); itcomponent++)
	{
		if(itcomponent->first == LIGHT)
		{
			((Light*)itcomponent->second)->DisablePointLight();
			continue;
		}
		delete itcomponent->second;
		itcomponent->second = nullptr;
	}
	m_components.clear();

	for(int index = m_childObjects.size() - 1;index >=0;index--)
	{
		delete m_childObjects.at(index);
		m_childObjects.at(index) = nullptr;
	}
	m_childObjects.clear();
	std::map<std::string , GameObject*>::iterator gmit;
	for (gmit = s_gameObjects.begin(); gmit != s_gameObjects.end(); gmit++)
	{
		if(gmit->second == this)
		{
			s_gameObjects[gmit->first] = nullptr;
			s_gameObjects.erase(gmit);
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Triggers when collider components collides
*@param   : Collider Component type,collider
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::OnCollisionEnter(Collider *collider)
{
	UNUSED(collider);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : retrieves world position from transform
*@param   : NIL
*@return  : WORLD xyz
*/////////////////////////////////////////////////////////////
Vector3 GameObject::GetWorldPosition()
{
	return m_transform.GetWorldPosition();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : retrieves world scale from transform
*@param   : NIL
*@return  : Scale xyz
*/////////////////////////////////////////////////////////////
Vector3 GameObject::GetWorldScale()
{
	return m_transform.GetWorldScale();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Retrieves local to world matrix
*@param   : NIL
*@return  : World matrix
*/////////////////////////////////////////////////////////////
Matrix44 GameObject::GetWorldMatrix()
{
	return m_transform.GetWorldMatrix();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : retrieves world rotation from transform
*@param   : NIL
*@return  : Euler angles world rotation
*/////////////////////////////////////////////////////////////
Vector3 GameObject::GetWorldRotation()
{
	return m_transform.GetWorldRotation();
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/06
*@purpose : Add Child Game object to the current object
*@param   : Child game object
*@return  : NIL
*//////////////////////////////////////////////////////////////
void GameObject::AddChild(GameObject *gameObject)
{
	gameObject->m_transform.m_parent = &(this->m_transform);
	m_childObjects.push_back(gameObject);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Adds component to game object
*@param   : Component
*@return  : NIL
*/////////////////////////////////////////////////////////////
void GameObject::AddComponent(COMPONENT_TYPE type,Component *component)
{
	component->m_gameObject = this;
	m_components[type]		= component;
	m_transform.AddChild(&component->m_transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Adds rigid body 3d collider
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddRigidBody3DComponent()
{
	RigidBody3D *rigidBody = new RigidBody3D();
	AddComponent(RIGID_BODY_3D, rigidBody);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/20
*@purpose : Adds a sound component to the game object
*@param   : filename
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddAudioComponent(std::string filename)
{
	AudioComponent *audioComponent = new AudioComponent(filename);
	AddComponent(AUDIO, audioComponent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Adds 3D audio sound to the component
*@param   : Sound file name, Local position w.r.t gameobject
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::Add3DAudioComponent(std::string filename,Vector3 localPosition)
{
	AudioComponent *audioComponent = new AudioComponent(filename);
	audioComponent->m_is3DSound = true;
	audioComponent->m_position = localPosition;
	AddComponent(AUDIO, audioComponent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Adds a new collider component
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddColliderComponent(COMPONENT_COLLIDER_TYPE type, Collider *coliider_t)
{
	coliider_t->m_gameObject    = this;
	m_colliderComponents[type]  = coliider_t;
	m_transform.AddChild(&coliider_t->m_transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Adds sphere collider
*@param   : relative position,radius
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddSphereCollider(Vector3 localPosition, float radius)
{
	SphereCollider *collider = new SphereCollider();
	collider->m_radius = radius;
	collider->SetPosition(localPosition);
	AddColliderComponent(SPHERE_COLLIDER, collider);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Adds a box collider
*@param   : relative position, dimensions in x,y,z
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddBoxCollider(Vector3 localPosition, Vector3 dimensions)
{
	BoxCollider *collider = new BoxCollider();
	//collider->m_offsetX = dimensions.x;
	//collider->m_offsetY = dimensions.y;
	//collider->m_offsetZ = dimensions.z;
	collider->SetPosition(localPosition);
	AddColliderComponent(BOX_COLLIDER, collider);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Adds new box2d collider
*@param   : local position and dimensions xyz
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddBoxCollider2D(Vector3 localPosition, Vector3 dimensions,Vector3 normal)
{
	BoxCollider2D *collider = new BoxCollider2D();
	collider->SetPosition(localPosition);
	Vector2 worldPosition = GetWorldPosition().GetXY() + localPosition.GetXY();
	collider->m_aabb2 = AABB2(worldPosition, dimensions.x, dimensions.y);
	/*collider->offsetX = dimensions.x;
	collider->offsetY = dimensions.y;
	collider->offsetZ = dimensions.z;
	collider->SetNormal(normal);*/
	//collider->m_transform.RotateLocalByEuler(ConvertCartesianToSpherical(normal));
	AddColliderComponent(BOX_COLLIDER2D, collider);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : Adds circle collider
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddCircleCollider(Vector3 localPosition, float radius)
{
	CircleCollider *circleCollider = new CircleCollider();
	circleCollider->SetPosition(localPosition);
	Vector2 worldPosition = GetWorldPosition().GetXY() + localPosition.GetXY();
	circleCollider->m_transform.SetLocalPosition(localPosition);
	circleCollider->m_disc.center = worldPosition;
	circleCollider->m_disc.radius = radius;
	AddColliderComponent(CIRCLE_COLLIDER, circleCollider);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Adds particle component to game object
*@param   : position
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddParticleComponent(Vector3 position,Renderer *renderer)
{
	ParticleEmitter *emitter = new ParticleEmitter(renderer, m_scene);
	emitter->SetPosition(position);
	AddComponent(PARTICLE,emitter);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Adds a light component to the game object
*@param   : Position , color
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddLightComponent(Vector3 position, Rgba color)
{
	Light *light = Light::CreateOrGetPointLight();
	if(light == nullptr)
	{
		return;
	}
	light->m_transform.SetLocalPosition(position);
	light->EnablePointLight(color.GetAsFloats(), position, Vector3(1, 0, 0));
	light->SetPointLightDirection(Vector3(0,1,0));
	//light->SetPointLightInnerOuterAngles(30.f, 60.f);
	AddComponent(LIGHT, light);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Adds camera component to the gameobject
*@param   : local position
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::AddCameraComponent(Vector3 localPosition)
{
	Camera *perspective				  = new PerspectiveCamera();
	FrameBuffer *fBuffer		      = new FrameBuffer();
	perspective->m_defaultFrameBuffer = fBuffer;
	perspective->SetColorTarget(Texture::GetDefaultColorTargetTexture());
	perspective->SetDepthStencilTarget(Texture::GetDefaultDepthTargetTexture());
	perspective->m_transform.Translate(localPosition);
	m_scene->AddCamera(perspective);
	AddComponent(CAMERA, perspective);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : retrieves sphere collider attached
*@param   : NIL
*@return  : Sphere collider
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SphereCollider* GameObject::GetSphereCollider()
{
	return (SphereCollider*)m_colliderComponents[SPHERE_COLLIDER];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : Retrieves box2d Collider
*@param   : NIL
*@return  : Box Collider2d
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BoxCollider2D * GameObject::GetBoxCollider2D()
{
	return (BoxCollider2D*)m_colliderComponents[BOX_COLLIDER2D];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/18
*@purpose : retrieves and return box collider
*@param   : NIL
*@return  : box collider
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BoxCollider * GameObject::GetBoxCollider()
{
	return (BoxCollider*)m_colliderComponents[BOX_COLLIDER];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/08
*@purpose : returns the circle collider
*@param   : NIL
*@return  : circle collider
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CircleCollider* GameObject::GetCircleCollider()
{
	return (CircleCollider*)m_colliderComponents[CIRCLE_COLLIDER];
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/04
*@purpose : Sets the position of game object in world
*@param   : position
*@return  : NIL
*//////////////////////////////////////////////////////////////
void GameObject::SetPosition(Vector3 position)
{
	m_transform.SetLocalPosition(position);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void GameObject::SetScene(Scene *scene)
{
	m_scene = scene;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/05
*@purpose : Updates gameobject its components and its children
*@param   : NIL
*@return  : NIL
*//////////////////////////////////////////////////////////////
void GameObject::Update(float deltaTime)
{
	m_renderable->m_modelMatrix = m_transform.GetWorldMatrix();
	UpdateComponents(deltaTime);
	UpdateColliderComponents(deltaTime);
	UpdateChildren(deltaTime);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Updates the current life
*@param   : deltatime
*@return  : NIL
*/////////////////////////////////////////////////////////////
void GameObject::UpdateLifeTime(float deltaTime)
{
	m_lifeTime += static_cast<double>(deltaTime);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
void GameObject::UpdateComponents(float deltaTime)
{
	std::map<COMPONENT_TYPE, Component*>::iterator it;
	for (it = m_components.begin(); it != m_components.end(); it++)
	{
		it->second->Update(deltaTime);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Updates all collider components
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameObject::UpdateColliderComponents(float deltaTime)
{
	std::map<COMPONENT_COLLIDER_TYPE,Collider*>::iterator it;
	for (it = m_colliderComponents.begin(); it != m_colliderComponents.end(); it++)
	{
		it->second->Update(deltaTime);
	}
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/06
*@purpose : Update all children associated
*@param   : deltatime
*@return  : NIL
*//////////////////////////////////////////////////////////////
void GameObject::UpdateChildren(float deltaTime)
{
	for(int childIndex =0;childIndex< m_childObjects.size();childIndex++)
	{
		m_childObjects.at(childIndex)->Update(deltaTime);
	}
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Retrieves component by name
*@param   : NIL
*@return  : component name
*/////////////////////////////////////////////////////////////
Component* GameObject::GetComponentByType(COMPONENT_TYPE type)
{
	std::map<COMPONENT_TYPE, Component*>::iterator it = m_components.find(type);
	if(it != m_components.end())
	{
		return m_components[type];
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : returns rigid body component
*@param   : NIL
*@return  : rigid body component
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RigidBody3D* GameObject::GetRigidBody3DComponent()
{
	return static_cast<RigidBody3D*>(m_components[RIGID_BODY_3D]);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Creates new game object by name
*@param   : name
*@return  : NIL
*/////////////////////////////////////////////////////////////
void GameObject::CreateGameObject(std::string name,GameObject *gameObject)
{
	s_gameObjects[name] = gameObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : Search and returns gameobject by name
*@param   : name
*@return  : Gameobject
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameObject* GameObject::FindByName(std::string name)
{
	return s_gameObjects[name];
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/17
*@purpose : retrieves gameobject by tag name
*@param   : NIL
*@return  : NIL
*/////////////////////////////////////////////////////////////
std::vector<GameObject*> GameObject::FindByTag(std::string tag)
{
	std::map<std::string, GameObject*>::iterator it;
	std::vector<GameObject*> gameObjects;
	for(it = s_gameObjects.begin();it != s_gameObjects.end();it++)
	{
		if(it->second->m_tag == tag)
		{
			gameObjects.push_back(it->second);
		}
	}
	return gameObjects;
}
