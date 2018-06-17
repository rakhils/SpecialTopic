#pragma once
#pragma warning (disable : 4820 4619 4668 4047 4024 4204 4100 4201)
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtil.hpp"
/*\class  : Quaternion	   
* \group  : <GroupName>		   
* \brief  :		   
* \TODO:  :		   
* \note   :		   
* \author : Rakhil Soman		   
* \version: 1.0		   
* \date   : 5/24/2018 4:45:25 PM
* \contact: srsrakhil@gmail.com
*/
class Quaternion
{
public:
	union {
		struct {
			Vector3 i;
			float   r;
		};
		Vector4 data;
	};
	Quaternion()
		: i(0.0f)
		, r(1.0f) {}
	Quaternion(float _x, float _y, float _z, float _w);
	Quaternion(float _r, Vector3 const &_i);

	/************************************************************************/
	/* OPERATORS                                                            */
	/************************************************************************/
	//------------------------------------------------------------------------
	Quaternion operator-(Quaternion const &a)
	{
		return Quaternion(-a.r, Vector3(a.i.x * -1.f,a.i.y * -1.f,a.i.z * -1.f));
	}

	//------------------------------------------------------------------------
	friend Quaternion operator*(Quaternion const &a, Quaternion const &b)
	{
		// Taken from 3D Math Primer - it's the reverse definition since
		// my engine also uses Row Major matrices by default, and I want to 
		// keep with that.
		Quaternion ret;
		ret.r = a.r * b.r - DotProduct(a.i, b.i);
		ret.i = a.r * b.i + b.r * a.i + CrossProduct(b.i, a.i);

		return ret;
	}

	//------------------------------------------------------------------------
	friend Vector3 operator*(Vector3 const &v, Quaternion const &q)
	{
		Quaternion p = Quaternion(0.0f, v);
		Quaternion result = q.GetInverse() * p * q;

		return result.i;
	}

	//------------------------------------------------------------------------
	friend Vector4 operator*(Vector4 const &v, Quaternion const &q)
	{
		/*Vector3 result = v.XYZ() * q;
		return Vector4(result, v.w);*/
		return Vector4::ZERO;
	}

	//------------------------------------------------------------------------
	friend bool operator==(Quaternion const &a, Quaternion const &b)
	{
		/*return (FlEqual(a.r, b.r) && (a.i == b.i))
		|| (FlEqual(a.r, -b.r) && (a.i == -b.i));*/
	}

	void operator=(Quaternion const &a)
	{	
		r = a.r;
		i = a.i;
	}

	//------------------------------------------------------------------------
	friend bool operator!=(Quaternion const &a, Quaternion const &b)
	{
		return !(a == b);
	}

	void       Normalize();
	bool       IsUnit() const;

	float      GetMagnitude() const;
	float      GetMagnitude2() const;

	Quaternion GetConjugate() const;
	Quaternion GetInverse() const;

	Vector3    GetEuler() const;
	//mat33    get_mat3() const;
	Matrix44   GetMatrix44() const;

	inline void invert() { *this = GetInverse(); }

	//inline Vector3 GetRightVector()   const { return vec3::RIGHT * (*this); }
	//inline Vector3 GetUpVector()      const { return vec3::UP * (*this); }
	//inline Vector3 GetForwardVector() const { return vec3::FORWARD * (*this); }

	//------------------------------------------------------------------------
	// STATIC
	//------------------------------------------------------------------------
	static Quaternion IDENTITY;

	//static Quaternion FromMatrix(mat33 const &mat);
	static Quaternion FromMatrix(Matrix44  &mat);

	static        Quaternion Around   (Vector3 const &axis, float const angle_radians);
	static        Quaternion FromEuler(Vector3 const &euler);
	static inline Quaternion FromEuler(float x, float y, float z) { return FromEuler(Vector3(x, y, z)); }
	//static		  Quaternion FromEuler(Vector3 const &euler, eRotationOrder const order);

	static Quaternion LookAt(Vector3 const forward);

Quaternion QuaternionGetDifference(Quaternion const &a, Quaternion const &b);
float	   QuaternionAngleInRadians(Quaternion const &a, Quaternion const &b);

float	   QuaternionDot(Quaternion const &a, Quaternion const &b);

// Works on normalized quaternion - returns a non-normalized quaternion
Quaternion QuaternionLog(Quaternion const &q);

// Works on quaternions of the form [0, a * i]
Quaternion QuaternionExp(Quaternion const &q);

Quaternion QuaternionScale(Quaternion const &q, float s);
Quaternion QuaternionPow(Quaternion const &q, float e);
Quaternion Slerp(Quaternion const &a, Quaternion const &b, float const t);
Quaternion QuaternionRotateTorward(Quaternion const &start, Quaternion const &end, float max_angle_radians);

Quaternion Lerp(Quaternion const &a, Quaternion const &b, float const &t);

bool	   NearEqual(Quaternion const &a, Quaternion const &b);

};
