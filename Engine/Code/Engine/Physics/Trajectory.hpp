#pragma once
#include <math.h>
#include <vector>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"

/*\class  : Trajectory
*		   
* \group  : <GroupName>
*		   
* \brief  :
*		   
* \TODO:  :
*		   
* \note   :
*		   
* \author : Rakhil Soman
*		   
* \version: 1.0
*		   
* \date   : 3/8/2018 8:15:24 PM
*
* \contact: srsrakhil@gmail.com
*/
 
class Trajectory
{

public:
	//Member_Variables
	
	//Static_Member_Variables

	//Methods

	Trajectory();
	~Trajectory();

	static float GetMiniumLaunchVelocity(float GRAVITY,float maxDistance);
	static std::vector<Vector3> Trajectory::GetTrajectoryPosition(Vector3 startPosition, Vector3 forwardDirection, float initialVelocity, float GRAVITY, float angle, int samples);
	static Vector3 GetTrajectoryPositionAtTime(Vector3 startPosition, Vector3 forwardDirection, float initialVelocity, float GRAVITY, float angle, float time);

	static Vector2 Evaluate( float gravity, Vector2 launchVelocity, float time );
	static Vector2 Evaluate( float gravity, float launch_speed, float launch_angle, float time );

	static float   GetMinimumLaunchSpeed( float gravity, float distance );
	static bool    GetLaunchAngles(Vector2 *out, float gravity, float launch_speed, float distance, float height = 0.0f);
	static float   GetMaxHeight(float gravity, float launch_speed, float distance);
	static Vector2 GetLaunchVelocity(float gravity,float apex_height,float distance,float height);         


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