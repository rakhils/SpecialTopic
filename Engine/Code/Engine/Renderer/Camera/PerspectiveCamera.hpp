#pragma once
#include "Engine/Renderer/Camera.hpp"
/*\class  : PerspectiveCamera	   
* \group  : <GroupName>		   
* \brief  :	Used to enable perspective vision camera in scene	   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 3/30/2018 1:42:34 PM
* \contact: srsrakhil@gmail.com
*/
class PerspectiveCamera : public Camera
{

public:
	//Member_Variables
	//Static_Member_Variables

	//Methods

	PerspectiveCamera();
	~PerspectiveCamera();
	void Update(float deltaTime);
	//void SetPerspectiveCamera(float fovDegrees, float aspect, float nearz, float farz);
	
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