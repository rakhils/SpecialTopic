#include "Engine/Math/Quaternion.hpp"

Quaternion Quaternion::IDENTITY = Quaternion( 1.0f, 0.0f, 0.0f, 0.0f );


Quaternion::Quaternion( float r, float ix, float iy, float iz ) 
{
	data = Vector4( ix, iy, iz, r );
}

//------------------------------------------------------------------------
Quaternion::Quaternion( float _r, Vector3 const &_i )
{
	r = _r;
	i = _i;
}

//------------------------------------------------------------------------
float Quaternion::GetMagnitude() const
{
	return sqrtf( GetMagnitude2() );
}

//------------------------------------------------------------------------
float Quaternion::GetMagnitude2() const 
{
	return DotProduct( data, data );
}

//------------------------------------------------------------------------
void Quaternion::Normalize() 
{
	float const len_squared = GetMagnitude2();

	// Already normalized.
	/*if (FlEqual(len_squared, 1.0f)) 
	{
		return;
	}*/

/*
	if (r < .9999f) {
		float s = sqrtf( 1.0f - r * r );
		Vector3 vector = i / s;
		Normalize( &vector );

		i = s * vector;
	} else {
		r = 1.0f;
		i = Vector3(0.0f);
	}*/
}

//------------------------------------------------------------------------
bool Quaternion::IsUnit() const
{
	return true;
	//return QuaternionDot(*this, *this) == 1.0f;
}

//------------------------------------------------------------------------
Quaternion Quaternion::GetConjugate() const
{
	return Quaternion( r, -1*i );
}

//------------------------------------------------------------------------
Quaternion Quaternion::GetInverse() const
{
	// We only deal with unit Quaternions in this engine!
	// ASSERT( is_unit() );
	return GetConjugate();
}

//------------------------------------------------------------------------
// Look at the matrix of a rotation around a vector.  
// Notices the values in it - those are the components 
// of a Quaternion, or some combination thereof - so we can 
// refigure it into it; 
/*
Mat33 Quaternion::get_mat3() const
{
	// imaginary part
	float const x = i.x;
	float const y = i.y;
	float const z = i.z;

	// cache off some squares
	float const x2 = x * x;
	float const y2 = y * y;
	float const z2 = z * z;

	Vector3 row0 = Vector3( 1.0f - 2.0f * y2 - 2.0f * z2, 
		2.0f * x * y + 2.0f * r * z, 
		2.0f * x * z - 2.0f * r * y );
	Vector3 row1 = Vector3( 2 * x * y - 2.0f * r * z, 
		1.0f - 2.0f * x2 - 2.0f * z2, 
		2.0f * y * z + 2.0f * r * x );
	Vector3 row2 = Vector3( 2.0f * x * z + 2.0f * r * y, 
		2.0f * y * z - 2.0f * r * x, 
		1.0f - 2.0f * x2 - 2.0f * y2 );

	mat33 ret;

	ret.set_basis( row0, row1, row2 );

	return ret;
}*/

//------------------------------------------------------------------------
Matrix44 Quaternion::GetMatrix44() const
{
	// imaginary part
	float const x = i.x;
	float const y = i.y;
	float const z = i.z;

	// cache off some squares
	float const x2 = x * x;
	float const y2 = y * y;
	float const z2 = z * z;

	Vector3 row0 = Vector3(1.0f - 2.0f * y2 - 2.0f * z2,  2.0f * x * y + 2.0f * r * z,   2.0f * x * z - 2.0f * r * y);
	Vector3 row1 = Vector3(2 * x * y - 2.0f * r * z,      1.0f - 2.0f * x2 - 2.0f * z2,  2.0f * y * z + 2.0f * r * x);
	Vector3 row2 = Vector3(2.0f * x * z + 2.0f * r * y,   2.0f * y * z - 2.0f * r * x,   1.0f - 2.0f * x2 - 2.0f * y2);

	Matrix44 ret(Vector4(row0,0), Vector4(row1,0), Vector4(row2,0),Vector4(0,0,0,1));

	return ret;
}

//------------------------------------------------------------------------
Vector3 Quaternion::GetEuler() const 
{
	//mat33 mat = get_mat3();
	return GetMatrix44().GetEulerFromMatrix();
}

//------------------------------------------------------------------------
Quaternion Quaternion::FromMatrix( Matrix44 &mat ) 
{
	float m00 = mat.Get(0, 0);
	float m11 = mat.Get(1, 1);
	float m22 = mat.Get(2, 2);
	float tr = m00 + m11 + m22;

	float m21 = mat.Get(2, 1);
	float m12 = mat.Get(1, 2);
	float m02 = mat.Get(0, 2);
	float m20 = mat.Get(2, 0);
	float m10 = mat.Get(1, 0);
	float m01 = mat.Get(0, 1);

	Quaternion q;
	if (tr >= 0.0f) {
		float s = sqrtf(tr + 1.0f) * 2.0f;
		float is = 1.0f / s;
		q.r = .25f * s;
		q.i.x = (m21 - m12) * is;
		q.i.y = (m02 - m20) * is;
		q.i.z = (m10 - m01) * is;
	} else if ((m00 > m11) & (m00 > m22)) {
		float s = sqrtf( 1.0f + m00 - m11 - m22 ) * 2.0f;
		float is = 1.0f / s;
		q.r = (m21 - m12) * is;
		q.i.x = .25f * s;
		q.i.y = (m01 + m10) * is;
		q.i.z = (m02 + m20) * is;
	} else if (m11 > m22) {
		float s = sqrtf( 1.0f + m11 - m00 - m22 ) * 2.0f;
		float is = 1.0f / s;
		q.r = (m02 - m20) * is;
		q.i.x = (m01 + m10) * is;
		q.i.y = .25f * s;
		q.i.z = (m12 + m21) * is;
	} else {
		float s = sqrtf( 1.0f + m22 - m00 - m11 ) * 2.0f;
		float is = 1.0f / s;
		q.r = (m10 - m01) * is;
		q.i.x = (m02 + m20) * is;
		q.i.y = (m12 + m21) * is;
		q.i.z = .25f * s;
	}

	q.Normalize();
	return q;
}

//------------------------------------------------------------------------
/*
Quaternion Quaternion::FromMatrix( Matrix44 const &mat )
{
	return Quaternion::FromMatrix( mat.to_mat3() );
}*/

//------------------------------------------------------------------------
Quaternion Quaternion::Around( Vector3 const &axis, float const angle_radians ) 
{
	float const half_angle = .5f * angle_radians;
	float const c = cosf(half_angle);
	float const s = sinf(half_angle);

	return Quaternion( c, s * axis );
}

//------------------------------------------------------------------------
/*
Quaternion Quaternion::FromEuler( Vector3 const &euler)//, eRotationOrder const rot_order ) 
{
	return Quaternion::FromEuler(euler);
	/ *if (rot_order == ROTATE_DEFAULT) {
		return Quaternion::FromEuler(euler);
	} else {
		Quaternion rotx = Quaternion::Around( Vector3::RIGHT, euler.x * .5f );
		Quaternion roty = Quaternion::Around( Vector3::UP, euler.y * .5f );
		Quaternion rotz = Quaternion::Around( Vector3::FORWARD, euler.z * .5f );

		switch (rot_order) {
		case ROTATE_XYZ:
			return rotx * roty * rotz;
		case ROTATE_XZY:
			return rotx * rotz * roty;
		case ROTATE_YXZ:
			return roty * rotx * rotz;
		case ROTATE_YZX:
			return roty * rotz * rotx;
		case ROTATE_ZYX:
		default:
			return rotz * roty * rotx;
		}
	}* /
}*/

//------------------------------------------------------------------------
Quaternion Quaternion::FromEuler( Vector3 const &euler ) 
{
	// If this changes - this method is no longer valid
	//ASSERT( ROTATE_DEFAULT == ROTATE_ZXY );

	Vector3 const he = .5f * euler;
	float cx = cosf( he.x );
	float sx = sinf( he.x );

	float cy = cosf( he.y );
	float sy = sinf( he.y );

	float cz = cosf( he.z );
	float sz = sinf( he.z );

	float r =   cx*cy*cz + sx*sy*sz;
	float ix =  sx*cy*cz + cx*sy*sz;
	float iy =  cx*sy*cz - sx*cy*sz;
	float iz =  cx*cy*sz - sx*sy*cz;


	Quaternion q = Quaternion( r, ix, iy, iz );
	q.Normalize();

	// ASSERT( q.is_unit() );
	return q;
}

//------------------------------------------------------------------------
Quaternion Quaternion::LookAt( Vector3 const forward )
{
	Matrix44 lookat = Matrix44::LookAt(forward,Vector3::UP);
	return FromMatrix( lookat );
}

//------------------------------------------------------------------------
Quaternion QuaternionGetDifference( Quaternion const &a, Quaternion const &b ) 
{
	return a.GetInverse() * b;
}

//------------------------------------------------------------------------
float QuaternionAngleInRadians( Quaternion const &a, Quaternion const &b )
{
	// so just trying to get real part of the difference
	// inverse of a real part is the real part, so that doesn't change
	// but the imaginary part negates.
	// so need to cmpute that.
	float new_real = a.r * b.r - DotProduct(-1*a.i, b.i);
	// new_real = cosf(half_angle);
	// half_angle = acosf(new_real);
	// angle = 2 * half_angle;

	float angle = 2.0f * acosf(new_real);
	return angle;
}

//------------------------------------------------------------------------
float QuaternionDot( Quaternion const &a, Quaternion const &b ) 
{
	return DotProduct( a.data, b.data );
}


//------------------------------------------------------------------------
// Works on normalized Quaternion - returns a non-normalized Quaternion
Quaternion QuaternionLog( Quaternion const &q ) 
{
	// Purely just the real part
	if (q.r >= .9999f) {
		return Quaternion( 0, 0, 0, 0 );
	}

	float half_angle = acosf(q.r);
	float s = sinf( half_angle );

	Vector3 n =  Vector3(q.i.x / s,q.i.y/s,q.i.z/s);
	return Quaternion( 0.0f, half_angle * n );
}

//------------------------------------------------------------------------
// Works on Quaternions of the form [0, a * i]
/*
Quaternion QuaternionExp( Quaternion const &q ) 
{
	float half_angle = q.i.GetLength();// Magnitude(q.i);
	Vector3 vec = Vector3(0.0f);
	float r = cosf(half_angle);
	if (half_angle > 0.00001f) {
		Vector3 n = q.i / half_angle;
		vec = sinf(half_angle) * n;
	}

	return Quaternion( r, vec );
}*/

//------------------------------------------------------------------------
Quaternion QuaternionScale( Quaternion const &q, float s ) 
{
	return Quaternion( s * q.r, s * q.i );
}

//------------------------------------------------------------------------
/*
Quaternion QuaternionPow( Quaternion const &q, float e ) 
{
	Quaternion ret = QuaternionExp( QuaternionScale( QuaternionLog( q ), e ) );
	ret.normalize();
	return ret;
}*/

//------------------------------------------------------------------------
/*
Quaternion Slerp( Quaternion const &a, Quaternion const &b, float const t_val ) 
{
	float time = ClampFloatZeroToOne(t_val);
	float cos_angle = QuaternionDot( a, b );


	Quaternion start;
	if (cos_angle < 0.0f) {
		// If it's negative - it means it's going the long way
		// flip it.
		start = (-1*a);
		cos_angle = -cos_angle;
	} else {
		start = a;
	}

	float f0, f1;
	if (cos_angle >= .9999f) {
		// very close - just linearly interpolate.
		f0 = 1.0f - time;
		f1 = time;
	} else {
		float sin_angle = sqrtf( 1.0f - cos_angle * cos_angle );
		float angle = atan2f( sin_angle, cos_angle );

		float den = 1.0f / sin_angle;
		f0 = sinf( (1.0f - time) * angle ) * den;
		f1 = sinf( time * angle ) * den;
	}

	Quaternion r0 = QuaternionScale( start, f0 );
	Quaternion r1 = QuaternionScale( b, f1 );
	return Quaternion( r0.r + r1.r, r0.i + r1.i );
}*/

/*
//------------------------------------------------------------------------
template <>
Quaternion Lerp( Quaternion const &a, Quaternion const &b, float const &t ) 
{
	return Slerp( a, b, t );
	/ *
	Quaternion diff = QuaternionGetDifference( a, b );
	return a * QuaternionPow( diff, t );
	* /
}*/

//------------------------------------------------------------------------
/*
Quaternion QuaternionRotateTorward( Quaternion const &start, Quaternion const &end, float max_angle_radians ) 
{
	float angle = acosf( QuaternionDot( start, end ) );
	if (angle < 0.0f) {
		angle = -angle;
	}

	if (angle == 0.0f)
	{
		return end;
	}

	float t = ClampFloatZeroToOne( max_angle_radians / angle );
	return Slerp( start, end, t );
}*/

//------------------------------------------------------------------------
bool NearEqual( Quaternion const &a, Quaternion const &b )
{
	float angle = QuaternionAngleInRadians(a, b);
	return angle < 0.001f;
}
