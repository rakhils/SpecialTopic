#include "Engine/Renderer/Camera/PerspectiveCamera.hpp"
#include "Engine/Core/Windows.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
// CONSTRUCTOR
PerspectiveCamera::PerspectiveCamera()
{

}

// DESTRUCTOR
PerspectiveCamera::~PerspectiveCamera()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/21
*@purpose : Updates camera
*@param   : delta time
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PerspectiveCamera::Update(float deltaTime)
{
	UNUSED(deltaTime);
	float PI     = 3.14f;
	float width  = static_cast<float>(Windows::GetInstance()->GetDimensions().x);
	float heigth = static_cast<float>(Windows::GetInstance()->GetDimensions().y);
	float aspect =  width/heigth ;
	SetPerspective(PI / 6.f, aspect, 0.1f, 300.f);
}



