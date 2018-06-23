#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/EngineCommon.hpp"

Matrix44::Matrix44()
{
	Ix = 1;
	Iy = 0;
	Iz = 0;
	Iw = 0;
	Jx = 0;
	Jy = 1;
	Jz = 0;
	Jw = 0;
	Kx = 0;
	Ky = 0;
	Kz = 1;
	Kw = 0;
	Tx = 0;
	Ty = 0;
	Tz = 0;
	Tw = 1; 
}

Matrix44::Matrix44(const float* sixteenValuesBasisMajor)
{
	Ix = sixteenValuesBasisMajor[0];
	Iy = sixteenValuesBasisMajor[1];
	Iz = sixteenValuesBasisMajor[2];
	Iw = sixteenValuesBasisMajor[3];
	Jx = sixteenValuesBasisMajor[4];
	Jy = sixteenValuesBasisMajor[5];
	Jz = sixteenValuesBasisMajor[6];
	Jw = sixteenValuesBasisMajor[7];
	Kx = sixteenValuesBasisMajor[8];
	Ky = sixteenValuesBasisMajor[9];
	Kz = sixteenValuesBasisMajor[10];
	Kw = sixteenValuesBasisMajor[11];
	Tx = sixteenValuesBasisMajor[12];
	Ty = sixteenValuesBasisMajor[13];
	Tz = sixteenValuesBasisMajor[14];
	Tw = sixteenValuesBasisMajor[15]; 
}

Matrix44::Matrix44(const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation/*=Vector2(0.f,0.f) */)
{
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = 0;
	Iw = 0;
	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = 0;
	Jw = 0;
	Kx = 0;
	Ky = 0;
	Kz = 1;
	Kw = 0;
	Tx = translation.x;
	Ty = translation.y;
	Tz = 0;
	Tw = 1;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Constructed from row values
*
*@param   : Each row values in vec3 format
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44::Matrix44(const Vector3 R1, const Vector3 R2, const Vector3 R3)
{
	Ix = R1.x;
	Jx = R1.y;
	Kx = R1.z;
	Tx = 0;
		
	Iy = R2.x;
	Jy = R2.y;
	Ky = R2.z;
	Ty = 0;
	
	Iz = R3.x;
	Jz = R3.y;
	Kz = R3.z;
	Tz = 0;
	
	Iw = 0;
	Jw = 0;
	Kw = 0;
	Tw = 1;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44::Matrix44(const Vector4 R1, const Vector4 R2, const Vector4 R3, const Vector4 R4)
{
	Ix = R1.x;
	Jx = R1.y;
	Kx = R1.z;
	Tx = R1.w;

	Iy = R2.x;
	Jy = R2.y;
	Ky = R2.z;
	Ty = R2.w;

	Iz = R3.x;
	Jz = R3.y;
	Kz = R3.z;
	Tz = R3.w;

	Iw = R4.x;
	Jw = R4.y;
	Kw = R4.z;
	Tw = R4.w;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : Matrix multiplication
*@param   : Second matrix to multiply
*@return  : Multiplied matrix
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::operator*(const Matrix44& matrix) const
{
	Matrix44 multipliedMatrix =  *this;
	multipliedMatrix.MultiplyAndSet(matrix);
	return multipliedMatrix;
}

Vector2 Matrix44::TransformPosition2D(const Vector2& position2D)
{

	Vector4 IVector(Ix,Jx,Kx,Tx);
	Vector4 JVector(Iy,Jy,Ky,Ty);
	Vector4 KVector(Iz,Jz,Kz,Tz);
	Vector4 TVector(Iw,Jw,Kw,Tw);

	Vector4 Position(position2D.x,position2D.y,0,1);
	float IDotPosition = DotProduct(IVector,Position);
	float JDotPosition = DotProduct(JVector,Position);
	return Vector2(IDotPosition,JDotPosition);
}

Vector2 Matrix44::TransformDisplacement2D(const Vector2& displacement2D)
{
	Vector4 IVector(Ix,Jx,Kx,Tx);
	Vector4 JVector(Iy,Jy,Ky,Ty);
	Vector4 KVector(Iz,Jz,Kz,Tz);
	Vector4 TVector(Iw,Jw,Kw,Tw);

	Vector4 Position(displacement2D.x,displacement2D.y,0,0);
	float IDotPosition = DotProduct(IVector,Position);
	float JDotPosition = DotProduct(JVector,Position);
	return Vector2(IDotPosition,JDotPosition);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
float Matrix44::Get(int column, int row)
{
	int index = row * 4 + column;
	return Get(index);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
float Matrix44::Get(int index)
{
	return *(&Ix + index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Matrix44::Set(int index,float value)
{
	*(&Ix + index) = value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/19
*@purpose : return Sum of diagonals
*@param   : NIL
*@return  : Sum
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Matrix44::GetTrace3()
{
	return (Ix + Jy + Kz );
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::Multiply(Vector3 position)
{
	Vector4 pos(position.x, position.y, position.z, 1);

	float X = DotProduct(GetXComponent4(), pos );
	float Y = DotProduct(GetYComponent4(), pos );
	float Z = DotProduct(GetZComponent4(), pos );
	return Vector3(X, Y, Z);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector4 Matrix44::Multiply(Matrix44 mat44,Vector4 vec4)
{
	float x = DotProduct(mat44.GetXComponent4(),vec4 );
	float y = DotProduct(mat44.GetYComponent4(),vec4 );
	float z = DotProduct(mat44.GetZComponent4(),vec4 );
	float w = DotProduct(mat44.GetWComponent4(),vec4 );
	return Vector4(x, y, z, w);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetIVector()
{
	return Vector3(Ix, Iy, Iz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetJVector()
{
	return Vector3(Jx, Jy, Jz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetKVector()
{
	return Vector3(Kx, Ky, Kz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetTVector()
{
	return Vector3(Tx, Ty, Tz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Matrix44::GetIVector4()
{
	return Vector4(Ix, Iy, Iz,Iw);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Matrix44::GetJVector4()
{
	return Vector4(Jx, Jy, Jz,Jw);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Matrix44::GetKVector4()
{
	return Vector4(Kx, Ky, Kz,Kw);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/17
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector4 Matrix44::GetTVector4()
{
	return Vector4(Tx, Ty, Tz, Tw);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Component
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetXComponent()
{
	return Vector3(Ix, Jx, Kx);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetYComponent()
{
	return Vector3(Iy, Jy, Ky);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetZComponent()
{
	return Vector3(Iz, Jz, Kz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetWComponent()
{
	return Vector3(Iw, Jw, Kw);

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/05/16
*@purpose : Gets all diagonal values
*
*@param   : NIL
*
*@return  : vec3 diagonal values
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetDiagonalValues()
{
	return Vector3(Ix, Jy, Kz);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Matrix44::GetXComponent4()
{
	return Vector4(Ix, Jx, Kx,Tx);

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Matrix44::GetYComponent4()
{
	return Vector4(Iy, Jy, Ky,Ty);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector4 Matrix44::GetZComponent4()
{
	return Vector4(Iz, Jz, Kz,Tz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector4 Matrix44::GetWComponent4()
{
	return Vector4(Iw, Jw, Kw,Tw);
}

void Matrix44::SetIdentity()
{
	Ix = 1;
	Iy = 0;
	Iz = 0;
	Iw = 0;
	Jx = 0;
	Jy = 1;
	Jz = 0;
	Jw = 0;
	Kx = 0;
	Ky = 0;
	Kz = 1;
	Kw = 0;
	Tx = 0;
	Ty = 0;
	Tz = 0;
	Tw = 1; 
}

void Matrix44::SetValues(const float* sixteenValuesBasisMajor)
{
	Ix = sixteenValuesBasisMajor[0];
	Iy = sixteenValuesBasisMajor[1];
	Iz = sixteenValuesBasisMajor[2];
	Iw = sixteenValuesBasisMajor[3];
	Jx = sixteenValuesBasisMajor[4];
	Jy = sixteenValuesBasisMajor[5];
	Jz = sixteenValuesBasisMajor[6];
	Jw = sixteenValuesBasisMajor[7];
	Kx = sixteenValuesBasisMajor[8];
	Ky = sixteenValuesBasisMajor[9];
	Kz = sixteenValuesBasisMajor[10];
	Kw = sixteenValuesBasisMajor[11];
	Tx = sixteenValuesBasisMajor[12];
	Ty = sixteenValuesBasisMajor[13];
	Tz = sixteenValuesBasisMajor[14];
	Tw = sixteenValuesBasisMajor[15]; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/19
*@purpose : Sets the translation part of marix
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Matrix44::SetTranslation(Vector4 translation)
{
	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = translation.w;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/19
*@purpose : Returns the translation values
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector4 Matrix44::GetTranslation()
{
	return GetTVector4();
}

void Matrix44::Append(const Matrix44& matrixToAppend)
{
	MultiplyAndSet(matrixToAppend);
}

void Matrix44::RotateDegrees2D(float rotationDegreesAboutZ)
{
	Vector4 IVector(CosDegrees(rotationDegreesAboutZ),-1*SinDegrees(rotationDegreesAboutZ),0,0);
	Vector4 JVector(SinDegrees(rotationDegreesAboutZ),CosDegrees(rotationDegreesAboutZ),0,0);
	Vector4 KVector(0,0,1,0);
	Vector4 TVector(0,0,0,1);
	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};
	Matrix44 rotationMatrix(points); 

	MultiplyAndSet(rotationMatrix); 

}

void Matrix44::Translate3D(const Vector3& translation)
{
	Vector4 IVector(1,0,0,translation.x);
	Vector4 JVector(0,1,0,translation.y);
	Vector4 KVector(0,0,1,translation.z);
	Vector4 TVector(0,0,0,1);
	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};
	Matrix44 rotationMatrix(points); 

	MultiplyAndSet(rotationMatrix); 
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/13
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Matrix44::Translate2D(const Vector2& translation)
{
	Translate3D(Vector3(translation.x,translation.y,1));
}

void Matrix44::ScaleUniform2D(float scaleXY)
{
	Vector4 IVector(scaleXY,0,0,0);
	Vector4 JVector(0,scaleXY,0,0);
	Vector4 KVector(0,0,1,0);
	Vector4 TVector(0,0,0,1);
	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w };
	Matrix44 rotationMatrix(points); 

	MultiplyAndSet(rotationMatrix); 
}

void Matrix44::Scale3D(float scaleX, float scaleY,float scaleZ)
{
	Vector4 IVector(scaleX,0,0,0);
	Vector4 JVector(0,scaleY,0,0);
	Vector4 KVector(0,0,scaleZ,0);
	Vector4 TVector(0,0,0,1);
	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};
	Matrix44 rotationMatrix(points); 

	MultiplyAndSet(rotationMatrix); 
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/13
*@purpose : Scale in 2D
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Matrix44::Scale2D(float scaleX, float scaleY)
{
	Scale3D(scaleX,scaleY,1.f);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/10
*@purpose : Transpose the given matrix
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Matrix44::Transpose()
{
	float tempvalue = Iy;
	Iy = Jx;
	Jx = tempvalue;

	tempvalue = Iz;
	Iz = Kx;
	Kx = tempvalue;

	tempvalue = Iw;
	Iw = Tx;
	Tx = tempvalue;

	tempvalue = Jz;
	Jz = Ky;
	Ky = tempvalue;

	tempvalue = Jw;
	Jw = Ty;
	Ty = tempvalue;

	tempvalue = Kw;
	Kw = Tz;
	Tz = tempvalue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/11
*@purpose : Inverts matrix
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Matrix44::Inverse()
{
	double inv[16];
	double det;
	double m[16];
	int i;

	for (i = 0; i < 16; ++i) 
	{
		m[i] = (double)Get(i);
	}

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	for (i = 0; i < 16; i++)
	{
		Set(i,(float)(inv[i] * det));
	}
}
//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Vector3 Matrix44::GetEulerFromMatrix()
{
	float xrad;
	float yrad;
	float zrad;

	float sx = -1 * Ky;
	sx = ClampFloat(sx, -1.0f, 1.0f);
	xrad = asinf(sx);

	float cx = cosf(xrad);
	if (!(cx == 0.0f))
	{
		yrad = atan2f(Kx, Kz);
		zrad = atan2f(Iy, Jy);
	}
	else
	{
		// gimble lock
		zrad = 0.0f;
		yrad = atan2f(-1 * Kx, Ix);

	}
	return Vector3(xrad, yrad, zrad);
}

Matrix44 Matrix44::MakeRotationDegrees2D(float rotationDegreesAboutZ)
{
	Vector4 IVector(CosDegrees(rotationDegreesAboutZ),-1*SinDegrees(rotationDegreesAboutZ),0,0);
	Vector4 JVector(SinDegrees(rotationDegreesAboutZ),CosDegrees(rotationDegreesAboutZ),0,0);
	Vector4 KVector(0,0,1,0);
	Vector4 TVector(0,0,0,1); 
	       
	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};

	return Matrix44(points);

}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/18
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakeRotationDegreesInY(float rotationDegreesAboutZ)
{
	Vector4 IVector(CosDegrees(rotationDegreesAboutZ),0, -1 * SinDegrees(rotationDegreesAboutZ), 0);
	Vector4 JVector(0, 1, 0, 0);
	Vector4 KVector(SinDegrees(rotationDegreesAboutZ),0,      CosDegrees(rotationDegreesAboutZ), 0);
	Vector4 TVector(0, 0, 0, 1);

	float points[16] = { IVector.x,JVector.x,KVector.x,TVector.x,
		IVector.y,JVector.y,KVector.y,TVector.y,
		IVector.z,JVector.z,KVector.z,TVector.z,
		IVector.w,JVector.w,KVector.w,TVector.w };

	return Matrix44(points);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakeRotationDegreesInX(float rotationDegreesAboutX)
{
	Vector4 IVector(1, 0, 0, 0);
	Vector4 JVector(0,CosDegrees(rotationDegreesAboutX), -1 * SinDegrees(rotationDegreesAboutX), 0);
	Vector4 KVector(0,SinDegrees(rotationDegreesAboutX), CosDegrees(rotationDegreesAboutX), 0);
	Vector4 TVector(0, 0, 0, 1);

	float points[16] = { IVector.x,JVector.x,KVector.x,TVector.x,
		IVector.y,JVector.y,KVector.y,TVector.y,
		IVector.z,JVector.z,KVector.z,TVector.z,
		IVector.w,JVector.w,KVector.w,TVector.w };

	return Matrix44(points);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakeRotationDegreesInZ(float rotationDegreesAboutZ)
{

	Vector4 IVector(CosDegrees(rotationDegreesAboutZ), -1 * SinDegrees(rotationDegreesAboutZ), 0, 0);
	Vector4 JVector(SinDegrees(rotationDegreesAboutZ), CosDegrees(rotationDegreesAboutZ), 0, 0);
	Vector4 KVector(0, 0, 1, 0);
	Vector4 TVector(0, 0, 0, 1);

	float points[16] = { IVector.x,JVector.x,KVector.x,TVector.x,
		IVector.y,JVector.y,KVector.y,TVector.y,
		IVector.z,JVector.z,KVector.z,TVector.z,
		IVector.w,JVector.w,KVector.w,TVector.w };

	return Matrix44(points);
}

Matrix44 Matrix44::MakeTranslation2D(const Vector2& translation)
{
	Vector4 IVector(1,0,0,translation.x);
	Vector4 JVector(0,1,0,translation.y);
	Vector4 KVector(0,0,1,0);
	Vector4 TVector(0,0,0,1);

	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};

	return Matrix44(points);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakeTranslation3D(const Vector3& translation)
{
	Vector4 IVector(1, 0, 0, translation.x);
	Vector4 JVector(0, 1, 0, translation.y);
	Vector4 KVector(0, 0, 1, translation.z);
	Vector4 TVector(0, 0, 0, 1);

	float points[16] = { IVector.x,JVector.x,KVector.x,TVector.x,
						 IVector.y,JVector.y,KVector.y,TVector.y,
						 IVector.z,JVector.z,KVector.z,TVector.z,
						 IVector.w,JVector.w,KVector.w,TVector.w };

	return Matrix44(points);
}

Matrix44 Matrix44::MakeScaleUniform2D(float scaleXY)
{
	Vector4 IVector(scaleXY,0,0,0);
	Vector4 JVector(0,scaleXY,0,0);
	Vector4 KVector(0,0,1,0);
	Vector4 TVector(0,0,0,1);

	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};

	return Matrix44(points);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : NIL
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakeScale3D(Vector3 scale)
{
	Vector4 IVector(scale.x, 0, 0, 0);
	Vector4 JVector(0, scale.y, 0, 0);
	Vector4 KVector(0, 0, scale.z, 0);
	Vector4 TVector(0, 0, 0, 1);

	float points[16] = { IVector.x,JVector.x,KVector.x,TVector.x,
		IVector.y,JVector.y,KVector.y,TVector.y,
		IVector.z,JVector.z,KVector.z,TVector.z,
		IVector.w,JVector.w,KVector.w,TVector.w };

	return Matrix44(points);
}

Matrix44 Matrix44::MakeScale2D(float scaleX, float scaleY)
{
	Vector4 IVector(scaleX,0,0,0);
	Vector4 JVector(0,scaleY,0,0);
	Vector4 KVector(0,0,1,0);
	Vector4 TVector(0,0,0,1);

	float points[16] = {IVector.x,JVector.x,KVector.x,TVector.x,
						IVector.y,JVector.y,KVector.y,TVector.y,
						IVector.z,JVector.z,KVector.z,TVector.z,
						IVector.w,JVector.w,KVector.w,TVector.w};

	return Matrix44(points);
}

Matrix44 Matrix44::MakeOrtho2D(const Vector2& bottomLeft, const Vector2& topRight)
{
	float farz = 0.0f;
	float nearz = -1.0f;
	Vector2 centre = (bottomLeft + topRight)/2.0f;
	Matrix44 translateMatrix44 = MakeTranslation2D(-1.0f*centre);
	Matrix44 scaleMatrix44 = MakeScale2D(2.0f/(topRight.x-bottomLeft.x),2.0f/(topRight.y-bottomLeft.y));
	Matrix44 orthoMatrix;
	orthoMatrix.SetIdentity();
	
	orthoMatrix.MultiplyAndSet(scaleMatrix44);
	orthoMatrix.MultiplyAndSet(translateMatrix44);
	orthoMatrix.Kz = -2.0f/(farz - nearz);
	orthoMatrix.Tz = nearz;
	
	return orthoMatrix;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/29
*@purpose : Matrix from euler angles
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MatrixFromEuler(Vector3 angles)
{
	float cx = cosf(angles.x);
	float sx = sinf(angles.x);

	float cy = cosf(angles.y);
	float sy = sinf(angles.y);

	float cz = cosf(angles.z);
	float sz = sinf(angles.z);

	float points[16]
	{
		cz*cy + sx*sy*sz,		sz*cx,		sz*sx*cy - cz*sy,		0.f,
		-sz*cy + cz*sx*sy,		cx*cz,		sx*cy*cz + sy*sz,		0.f,
		cx*sy,					-sx,		cx*cy,					0.f,
		0.f,					0.f,		0.f,					1.f
	};
	Matrix44 matrix(points);
	return matrix;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/03/30
*@purpose : Creates a perspective transformation matrix
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakePerspectiveMatrix(float fovDegrees, float aspect, float nearz, float farz)
{
	float d = 1.0f / tanf(fovDegrees );
	float q = 1.0f / (farz - nearz);

	Vector4 i = Vector4(d / aspect, 0, 0, 0);
	Vector4 j = Vector4(0, d, 0, 0);
	Vector4 k = Vector4(0, 0, (nearz + farz) * q, 1);
	Vector4 t = Vector4(0, 0, -2.0f * nearz * farz * q, 0);

	Matrix44 matrix(i, j, k, t);
	matrix.Transpose();
	return matrix;
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/04/20
*@purpose : Makes rotation matrix from a 3d direction vector
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
Matrix44 Matrix44::MakeRotationMatrixFromDirection(Vector3 direction)
{
	Vector2 XZVector(direction.x, direction.z);
	XZVector = XZVector.GetNormalized();
	float rotation = Atan2Degrees(XZVector.y, XZVector.x);

	Vector2 XYVector(direction.x, direction.y);
	XYVector = XYVector.GetNormalized();
	float azimuth = Atan2Degrees(XYVector.y, XYVector.x);

	return MatrixFromEuler(Vector3(0, rotation, azimuth));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/05/24
*@purpose : Calculates a matrix to look to a direction
*@param   : Direction to look at,world up
*@return  : Look At Matrix
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::LookAt(Vector3 forward, Vector3 worldUP)
{
	Vector3 right = CrossProduct(worldUP, forward);
	right = right.GetNormalized();

	Vector3 Up = CrossProduct(forward,right);
	Up = Up.GetNormalized();

	Matrix44 LookAtMatrix = Matrix44(right, Up, forward);

	/*LookAtMatrix.Tx = 1 * DotProduct(Vector3::ZERO, right);
	LookAtMatrix.Ty = 1 * DotProduct(Vector3::ZERO, Up);
	LookAtMatrix.Tz = 1 * DotProduct(Vector3::ZERO, forward);
	LookAtMatrix.Tw = 1;*/

	return LookAtMatrix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/18
*@purpose : Calculates a matrix to look to a direction
*@param   : Direction to look at,world up
*@return  : Look At Matrix
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::LookAt(Vector3 position, Vector3 lookAtPos, Vector3 upvector)
{
	Vector3 direction = (lookAtPos - position);
	return LookAt(direction.GetNormalized(), upvector);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/10
*@purpose : Lerps transforms from matrix a to b by deltatime
*@param   : StartMatrix a End Matrix b , delta time
*@return  : Lerped matrix
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::LerpTransform(Matrix44 &a, Matrix44 &b, float delta)
{
	Vector3 aRight			= a.GetIVector();
	Vector3 bRight			= b.GetIVector();
	Vector3 aUp				= a.GetJVector();
	Vector3 bUp				= b.GetJVector();
	Vector3 aForward		= a.GetKVector();
	Vector3 bForward		= b.GetKVector();
	Vector3 aTranslation	= a.GetTVector();
	Vector3 bTranslation	= b.GetTVector();
	
	Vector3 right			= Slerp(aRight, bRight, delta);
	Vector3 up				= Slerp(aUp, bUp, delta);
	Vector3 forward			= Slerp(aForward, bForward, delta);
	Vector3 translation		= Interpolate(aTranslation, bTranslation, delta);

	return Matrix44(right, up, forward, translation);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/19
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::TurnTowards(Matrix44 &current, Matrix44 &target, float maxTurnPerFrame)
{
	Matrix44 currentCopy  = current;
	Matrix44 currentCopy1 = current;
	currentCopy.Inverse();

	Matrix44 r = currentCopy * target;

	// trace is 1 + 2 cos(theta) == sum of diagonal
	float trace = r.GetTrace3();

	// trace = 1 + 2.* cos(theta)
	// theta = acos( (trace - 1) *.5f ); 
	float inner = (trace - 1.0f) * .5f;
	inner		= ClampFloat(inner, -1.0f, 1.0f);
	float theta = ACosDegrees(inner);

	float t = GetMinOf2(theta / maxTurnPerFrame, 1.0f);
	t = 0.1;
	Matrix44 ret = LerpTransform(currentCopy1, target, t);
	ret.Transpose();
	ret.SetTranslation(currentCopy1.GetTranslation());

	return ret;
}

void Matrix44::MultiplyAndSet(Matrix44 valueMatrix)
{
	Vector4 IVector(Ix,Jx,Kx,Tx);
	Vector4 JVector(Iy,Jy,Ky,Ty);
	Vector4 KVector(Iz,Jz,Kz,Tz);
	Vector4 TVector(Iw,Jw,Kw,Tw);

	Vector4 XValueMatrix(valueMatrix.Ix,valueMatrix.Iy,valueMatrix.Iz,valueMatrix.Iw);
	Vector4 YValueMatrix(valueMatrix.Jx,valueMatrix.Jy,valueMatrix.Jz,valueMatrix.Jw);
	Vector4 ZValueMatrix(valueMatrix.Kx,valueMatrix.Ky,valueMatrix.Kz,valueMatrix.Kw);
	Vector4 TValueMatrix(valueMatrix.Tx,valueMatrix.Ty,valueMatrix.Tz,valueMatrix.Tw);

	Ix = DotProduct(IVector,XValueMatrix);
	Jx = DotProduct(IVector,YValueMatrix);
	Kx = DotProduct(IVector,ZValueMatrix);
	Tx = DotProduct(IVector,TValueMatrix);

	Iy = DotProduct(JVector,XValueMatrix);
	Jy = DotProduct(JVector,YValueMatrix);
	Ky = DotProduct(JVector,ZValueMatrix);
	Ty = DotProduct(JVector,TValueMatrix);

	Iz = DotProduct(KVector,XValueMatrix);
	Jz = DotProduct(KVector,YValueMatrix);
	Kz = DotProduct(KVector,ZValueMatrix);
	Tz = DotProduct(KVector,TValueMatrix);

	Iw = DotProduct(TVector,XValueMatrix);
	Jw = DotProduct(TVector,YValueMatrix);
	Kw = DotProduct(TVector,ZValueMatrix);
	Tw = DotProduct(TVector,TValueMatrix);
}

//////////////////////////////////////////////////////////////
/*DATE    : 2018/02/19
*@purpose : Invert the rotational part and negate the transition
*
*@param   : NIL
*
*@return  : NIL
*/
//////////////////////////////////////////////////////////////
void Matrix44::InvertFast()
{

	/*float temp = Jx;
	Jx = Iy;
	Iy = temp;

	temp = Kx;
	Kx = Iz;
	Iz = temp;

	temp = Ky;
	Ky = Jz;
	Jz = temp;*/

	Tx = -1*Tx;
	Ty = -1*Ty;
	Tz = -1*Tz;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DATE    : 2018/06/20
*@purpose : NIL
*@param   : NIL
*@return  : NIL
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Matrix44::InvertFast1()
{
	float temp = Jx;
	Jx = Iy;
	Iy = temp;

	temp = Kx;
	Kx = Iz;
	Iz = temp;

	temp = Ky;
	Ky = Jz;
	Jz = temp;

	Tx = -1*Tx;
	Ty = -1*Ty;
	Tz = -1*Tz;
}
