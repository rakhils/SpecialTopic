#pragma once
#include "Engine/Renderer/Camera.hpp"
/*\class  : OrthographicCamera	   
* \group  : <GroupName>
* \brief  :
* \TODO:  :
* \note   :
* \author : Rakhil Soman
* \version: 1.0
* \date   : 5/21/2018 8:53:27 PM
* \contact: srsrakhil@gmail.com
*/

class OrthographicCamera : public Camera
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	OrthographicCamera();
	~OrthographicCamera();
	void Update(float deltaTime);
	void ZoomIn(float deltaTime);
	void ZoomOut(float deltaTime);
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