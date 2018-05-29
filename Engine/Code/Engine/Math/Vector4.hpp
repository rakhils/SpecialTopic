#pragma once
#include "Engine/Math/Vector3.hpp"
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;
	
	Vector4() { x = y = z = w = 0;}
	Vector4(float xval,float yval,float zval,float wval);
	Vector4(Vector2 value);
	Vector4(Vector3 value);
	Vector4(Vector3 value,float wval);
	Vector4(std::string text);

	const Vector4	 operator - (const Vector4& vecToSubtract) const;
	const Vector4	 operator + (const Vector4& vectora) const;
	const Vector4	 operator * (float uniformScale);
	const Vector4	 operator / (float uniformScale);
	void			 operator -=(const Vector4& vecToSubtract);
	void			 operator +=(const Vector4& VectorToAdd);
	void			 operator *=(const float uniformScale);
	void			 operator /=(const float uniformScale);
	bool			 operator ==(const Vector4& vectorB);
	bool			 operator !=(const Vector4& vectorB);
	void	     	 operator = (const Vector4& vectorB);

	friend const Vector4 operator *(float unformscale, const Vector4& value);


	//float* GetAsArray();

	bool			SetFromText(const char* text);
	Vector3			XYZ();
	float			GetLength();
	void			Normalize();

	static Vector4	ZERO;
	static Vector4	ONE;
};