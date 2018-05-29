#pragma once
#include "Engine/Renderer/Camera.hpp"
/*\class  : OrbitCamera   
* \group  : <GroupName>		   
* \brief  :	Used to setup orbit camera in scene orbits around a point	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 2/20/2018 6:28:29 PM
* \contact: srsrakhil@gmail.com
*/
class OrbitCamera : public Camera
{

public:
	//Member_Variables
	//Static_Member_Variables
	//Methods

	OrbitCamera();
	~OrbitCamera();
	void Update(float deltaTime);
	void SetTargetPosition(Vector3 targetPosition);
	void SetSphericalCords(Vector3 cameraPosition);
	//Static_Methods

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