#pragma once
#include <math.h>
#include <string>

#include "Engine/Math/Vector2.hpp"

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3() { x = y = z = 0.f;}
	Vector3(std::string value);
	Vector3(Vector2 value);
	Vector3(Vector2 value,float valuez);
	Vector3(float value) { x = y = z = value; }
	Vector3(float xval,float yval,float zval);

	const Vector3		 operator - (const Vector3& vecToSubtract) const;
	const Vector3		 operator + (const Vector3 vectora) const;
	const Vector3		 operator * (float uniformScale);
	const Vector3		 operator / (float uniformScale) const;
	void				 operator -=(const Vector3& vecToSubtract);
	void				 operator +=(const Vector3 vectorA);
	void				 operator *=(const float uniformScale);
	void				 operator /=(const float uniformScale);
	bool				 operator ==(const Vector3 vectorA);
	bool				 operator !=(const Vector3 vectorA);
	const Vector3		 operator = (const Vector2& vectorb) const;
	bool				 operator >=(const Vector3& vectorA) const;
	bool				 operator <=(const Vector3& vectorA) const;
	bool				 operator > (const Vector3& vectorA) const;
	bool				 operator < (const Vector3& vectorA) const;

	friend const Vector3 operator*(float uniformScale, const Vector3& vecToScale);


	float				GetLength();
	Vector3				GetNormalized();
	std::string			GetAsString();
	bool				SetFromText(const char *text);
	bool				IsNearZero();

	Vector2				GetXY();
	Vector2				GetXZ();
	Vector2				GetYZ();

	IntVector2			GetIntVector2XY();
	IntVector2			GetIntVector2XZ();
	IntVector2			GetIntVector2YZ();

	static Vector3		ZERO;
	static Vector3		ONE;

	static Vector3		UP;
	static Vector3      RIGHT;
	static Vector3      FORWARD;

	static Vector3		GetRandom(Vector3 mins, Vector3 max);
	static Vector3		GetRandomValueOnUnitSphere();
};