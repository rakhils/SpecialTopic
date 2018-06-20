#pragma once
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/Component.hpp"
#include "Engine/Math/Frustum.hpp"
/*\class  : Camera		  
* \group  : <GroupName>		   
* \brief  : Camera class to view screen		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 2/10/2018 4:32:51 PM
* \contact: srsrakhil@gmail.com
*/
enum CAMERA_TYPE
{
	ORTHOGRAPHIC,
	ORBIT,
	PERSPECTIVE
};
struct PickRay
{
	Vector3 m_position;
	Vector3 m_direction;
	PickRay(Vector3 pos, Vector3 dir)
	{
		m_position = pos;
		m_direction = dir;
	}
};
class Camera : public Component
{

public:
	//Member_Variables

	Vector3		m_targetPosition;
	std::string m_name;
	Matrix44	m_projection;
	Matrix44	m_view;
	Matrix44	m_model;

	float		m_xZoomIn;
	float       m_yZoomIn;

	FrameBuffer *m_defaultFrameBuffer = nullptr;
	
	//Transform m_transform;

	//Static_Member_Variables
	static std::map<std::string, Camera*> s_cameras;
	//Methods

	Camera();
	~Camera();
	
	GLint		GetFrameBufferHandle();
	IntVector2  GetDimensions();
	Vector3		GetCameraRightVector();
	Vector3		GetCameraForwardVector();
	Vector3		GetCameraUpVector();
	void		SetProjection(Matrix44 const &proj);
	void		SetViewMatrix(Matrix44 view);
	void		SetModelMatrix(Matrix44 model);
	Vector3		ScreenToWorld(Vector2 xy, float ndc_depth);
	void	    SetOrthoProjection();
	void		SetOrthoProjection(Vector2 bottomLeft,Vector2 topRigth, float nearZ, float farZ);
	void		SetColorTarget(Texture *texture);
	void		SetDepthStencilTarget(Texture *texture);
	PickRay     GetPickRayFromScreenCords(Vector2 screenXY);
	Matrix44    GetViewProjection();
	Matrix44	LookAt(Vector3 pos, Vector3 tartget, Vector3 worldUP);
	void		SetPerspective(float fovDegrees, float aspect, float nearz, float farz);
	//void		Update(float deltaTime);

	void		MoveRigth    (float deltaTime);
	void		MoveLeft     (float deltaTime);
	void		MoveUp       (float deltaTime);
	void		MoveDown     (float deltaTime);
	void		MoveForward  (float deltaTime);
	void		MoveBackward (float deltaTime);

	void		RotateX(float angle);
	void		RotateY(float angle);
	void		RotateZ(float angle);
	Frustum		GetFrustum();
	void		Finalize();
	//Static_Methods
	static void	InitCamera();
	static Camera* CreateOrGetCamera(std::string name,CAMERA_TYPE type = ORTHOGRAPHIC);
	static void SetDefaultCamera (Camera *camera) { s_defaultCamera  = camera;}
	static void SetCurrentCamera (Camera *camera) 
	{ 
		camera->Finalize(); 
		s_currentCamera = camera; 
	}
	static void SetGameplayCamera(Camera *camera) { s_gamePlayCamera = camera;}
	static void SetEffectCamera  (Camera *camera) { s_effectCamera   = camera;}
	static void SetUICamera      (Camera *camera) { s_uiCamera       = camera;}

	static Camera* GetDefaultCamera()  { return s_defaultCamera; }
	static Camera* GetCurrentCamera()  { return s_currentCamera; }
	static Camera* GetGamePlayCamera() { return s_gamePlayCamera;}
	static Camera* GetEffectCamera()   { return s_effectCamera;	 }
	static Camera* GetUICamera()       { return s_uiCamera;		 }

protected:
	//Member_Variables

	//Static_Member_Variables

	//Methods

	//Static_Methods

private:
	//Member_Variables

	//Static_Member_Variables
public:
	static Camera  *s_defaultCamera;
	static Camera  *s_currentCamera;
	static Camera  *s_gamePlayCamera;
	static Camera  *s_effectCamera;
	static Camera  *s_uiCamera;
	//Methods

	//Static_Methods

};