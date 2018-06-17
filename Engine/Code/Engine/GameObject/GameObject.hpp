#pragma once
#include <vector>
#include <map>
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/Component.hpp"
#include "Engine/Physics/Collider/Collider.hpp"
/*\class  : GameObject		   
* \group  : <GroupName>		   
* \brief  :	Gameobject is base of each entity in the game	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 4/29/2018 5:11:42 PM
* \contact: srsrakhil@gmail.com
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Render
class Renderer;
class Renderable;
class Scene;
class Light;
//COMPONENTS
class RigidBody3D;
class Collider;
// COLLIDER
class SphereCollider;
class BoxCollider2D;
class BoxCollider;
//////////////////////////////////////////////////////////////////////////
enum COMPONENT_TYPE
{
	RIGID_BODY_3D,RIGID_BODY_2D,AUDIO,PARTICLE,LIGHT,CAMERA
};

class GameObject
{

public:
	//Member_Variables
	std::string									   m_name;
	std::string									   m_tag;
	bool										   m_isHidden				= false;
	double										   m_creationTime			= 0.f;
	double										   m_lifeTime				= 0.f;
	Scene*										   m_scene					= nullptr;
	Renderable*									   m_renderable				= nullptr;
	Transform									   m_transform;
	Vector3										   m_forward;
	GameObject*									   m_parent					= nullptr;
	std::vector<GameObject*>					   m_childObjects;
	std::map<COMPONENT_TYPE, Component*>		   m_components;
	std::map<COMPONENT_COLLIDER_TYPE, Collider*>   m_colliderComponents;
	//Static_Member_Variables
	static std::map<std::string , GameObject*>     s_gameObjects;
	//Methods

	GameObject(std::string name);
	GameObject(std::string name, Scene *scene);
	~GameObject();

	virtual void						OnCollisionEnter(Collider *collider);

	Matrix44							GetWorldMatrix();
	Vector3								GetWorldPosition();
	Vector3								GetWorldScale();
	Vector3								GetWorldRotation();

	void								AddChild(GameObject *gameObject);
	void								AddComponent(COMPONENT_TYPE componentType,Component *component);
	void								AddRigidBody3DComponent();
	void								AddAudioComponent(std::string filename);
	void								Add3DAudioComponent(std::string filename,Vector3 localPosition);
	void								AddParticleComponent(Vector3 position, Renderer *renderer);
	void								AddLightComponent(Vector3 position, Rgba color);
	void								AddCameraComponent(Vector3 localPosition);
	
	void								AddColliderComponent(COMPONENT_COLLIDER_TYPE type, Collider *collider_t);
	void								AddSphereCollider(Vector3 localPosition, float radius);
	void								AddBoxCollider(Vector3 localPosition, Vector3 dimensions);
	void								AddBoxCollider2D(Vector3 localPosition, Vector3 dimensions,Vector3 normal);
	void								AddCircleCollider(Vector3 localPosition, float radius);

	Component*							GetComponentByType(COMPONENT_TYPE type);
	RigidBody3D*						GetRigidBody3DComponent();

	SphereCollider*						GetSphereCollider();
	BoxCollider2D *						GetBoxCollider2D();
	BoxCollider   *						GetBoxCollider();
	CircleCollider*						GetCircleCollider();

	void								SetPosition(Vector3 position);
	void								SetScene(Scene *scene);
										
	void								Update(float deltaTime);
	void								UpdateLifeTime(float deltaTime);
	void								UpdateComponents(float deltaTime);
	void								UpdateColliderComponents(float deltaTime);
	void								UpdateChildren(float deltaTime);

	void								Render();

	//Static_Methods
	static void							CreateGameObject(std::string name,GameObject *gameObject);
	static GameObject*					FindByName(std::string name);
	static std::vector<GameObject*>		FindByTag (std::string tag);


protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

};