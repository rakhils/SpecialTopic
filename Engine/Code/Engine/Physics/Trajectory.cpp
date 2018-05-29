#include "Engine/Physics/Trajectory.hpp"

// CONSTRUCTOR
Trajectory::Trajectory()
{

}

// DESTRUCTOR
Trajectory::~Trajectory()
{

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Return min launch velocity to reach distance . optimum angle 45 degrees
*
*@param   : Gravity, Distance
*
*@return  : Launch velcoity
*/
//////////////////////////////////////////////////////////////
float Trajectory::GetMiniumLaunchVelocity(float GRAVITY, float maxDistance)
{
	return sqrt(GRAVITY*maxDistance);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Returns trajectory positions incremented time by .05;
*
*@param   : Start position,forward direction, velocity at time start, Gravity , angle of laucnh, total number of samples til the position calculated
*
*@return  : Vector3 Positions of trajectory at time inceremtned by .05
*/
//////////////////////////////////////////////////////////////
std::vector<Vector3> Trajectory::GetTrajectoryPosition(Vector3 startPosition,Vector3 forwardDirection,float initialVelocity,float GRAVITY,float angle,int samples)
{
	startPosition.y -= .5f;
	std::vector<Vector3> positions;
	float time = 0.f;
	for (int posindex = 0; posindex < samples; posindex++)
	{
		positions.push_back(GetTrajectoryPositionAtTime(startPosition,forwardDirection,initialVelocity,GRAVITY,angle,time));
		time += 0.05f;
	}
	return positions;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Returns trajectory position at time t
*
*@param   : Start position,forward direction, velocity at time start, Gravity , angle of laucnh, time 
*
*@return  : Position of trajectory at time t
*/
//////////////////////////////////////////////////////////////
Vector3 Trajectory::GetTrajectoryPositionAtTime(Vector3 startPosition,Vector3 forwardDirection,float initialVelocity,float GRAVITY,float angle, float time)
{
	Vector2 XZVector(forwardDirection.x, forwardDirection.z);
	XZVector = XZVector.GetNormalized();
	float YTheta = Atan2Degrees(XZVector.y, XZVector.x);

	Vector2 XYVector(forwardDirection.x, forwardDirection.y);
	XYVector = XYVector.GetNormalized();
	float ZTheta = Atan2Degrees(XYVector.y, XYVector.x);

	if(ZTheta == 180)
	{
		ZTheta = 0.f;
	}
	float angleInDegrees = ConvertRadiansToDegrees(angle);
	ZTheta = angleInDegrees;
	ZTheta = ClampFloat(ZTheta, -90, 90);

	Vector3 finalVector = ConvertSphericalToCartesian(Vector3(1, YTheta, ZTheta));
	Vector3 gravityVector(0, GRAVITY, 0);
	
	return startPosition + finalVector*(initialVelocity*time) + gravityVector * (0.5f*time*time);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Evaluate trajectory path
*
*@param   : gravity,Launch velocity , time
*
*@return  : Position at given time 
*/
//////////////////////////////////////////////////////////////
Vector2 Trajectory::Evaluate(float gravity, Vector2 launchVelocity, float time)
{
	Vector2 gravityVector(0, -1 * gravity);
	return  (launchVelocity*time + 0.5f * gravityVector *time*time);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector2 Trajectory::Evaluate(float gravity, float launch_speed, float launch_angle, float time)
{
	Vector2 direction(CosDegrees(launch_angle), SinDegrees(launch_angle));
	return Evaluate(gravity, direction*launch_speed, time);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Gets the minimum launch speed to reach the distance
*
*@param   : Gravity,distance
*
*@return  : min launch speed to reach distance
*/
//////////////////////////////////////////////////////////////
float Trajectory::GetMinimumLaunchSpeed(float gravity, float distance)
{
	return sqrt(gravity*distance);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Angles needed to reach at distance with laucnh speed
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
bool Trajectory::GetLaunchAngles(Vector2 *out, float gravity, float launch_speed, float distance, float height /*= 0.0f*/)
{
	float launchSpeedSquare = launch_speed * launch_speed;
	float distanceSquare    = distance     * distance;
	float determinant = launchSpeedSquare * launchSpeedSquare - gravity * (gravity*distanceSquare + 2 * height*launchSpeedSquare);
	if(determinant < 0)
	{
		return false;
	}
	out->x = (launchSpeedSquare + sqrt(determinant)) / (gravity * distance);
	out->y = (launchSpeedSquare - sqrt(determinant)) / (gravity * distance);
	return true;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Returns the max heigth which can be reached with laucnh speed and distance
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
float Trajectory::GetMaxHeight(float gravity, float launch_speed, float distance)
{
	Vector2 *outputVector = new Vector2();
	if(GetLaunchAngles(outputVector, gravity, launch_speed, distance, 0))
	{
		float heigth1 = launch_speed * launch_speed*SinDegrees(outputVector->x)*SinDegrees(outputVector->x) / 2 * gravity;
		float heigth2 = launch_speed * launch_speed*SinDegrees(outputVector->y)*SinDegrees(outputVector->y) / 2 * gravity;

		if(heigth1 > heigth2)
		{
			return outputVector->x;
		}
		return outputVector->y;
	}
	return 0.f;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/08
*@purpose : Returns launch velocity with max height and distance 
*
*@param   : gravity, max height to be reached ,distance to be reached , height diff at start position and end distance
*
*@return  : Velocity
*/
//////////////////////////////////////////////////////////////
Vector2 Trajectory::GetLaunchVelocity(float gravity, float apex_height, float distance, float height)
{
	// From equation ->		V*Cos(theta)*time = distance								(1)
	//						V*Sin(theta)*time - 0.5*g*t*t = h    (heigth difference);	(2)
	//We know max height =  V*V*Sin(theta)*Sin(theta)/2*g = Hmax (apex Heigth)			(3)
	//				So      V*Sin(theta)  = sqrt(2*g*apex_heigth)						(4)
	//						Substituting in Equation 2
	//						0.5*g*t*t - sqrt(2*g*apexheigth)*t + heigth = 0				(5)
	//						Solving quadratic equation will get total time;(total time)
	//						Dividing V*Sin(theta)*t/V*Cos(theta)*t gives 
	//						Tan(theta) = (heigth +0.5*g*t*t)/d
	//						Tan Inverse gives Theta
	//						With Theta and Total time solving (1) and (2) We get Velocity
	//						

	float a = 0.5f*gravity;
	float b = -1 * sqrt(2 * gravity*apex_height);
	float c = height;

	Vector2 *solution = new Vector2();;
	Quadratic(solution, a, b, c);

	float time = solution->x;
	if (solution->x > solution->y)
	{
		time = solution->y;
	}

	float tanTheta = (height + 0.5f*gravity*time*time) / distance;

	float theta = atan2f(tanTheta, 1);

	float speed = distance / time * cosf(theta);

	float Vx = speed * cosf(theta);
	float Vy = speed * sinf(theta);

	return Vector2(Vx, Vy);
}
