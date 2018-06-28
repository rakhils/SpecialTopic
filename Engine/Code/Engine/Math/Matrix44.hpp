#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

class Matrix44
{
public:
	float Ix;
	float Iy;
	float Iz;
	float Iw;
	float Jx;
	float Jy;
	float Jz;
	float Jw;
	float Kx;
	float Ky;
	float Kz;
	float Kw;
	float Tx;
	float Ty;
	float Tz;
	float Tw;    

	Matrix44(); // default-construct to Identity matrix (via variable initialization)
	explicit Matrix44( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	explicit Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation=Vector2(0.f,0.f) );
	explicit Matrix44(const Vector3 R1,const Vector3 R2,const Vector3 R3);
	explicit Matrix44(const Vector4 R1,const Vector4 R2,const Vector4 R3,const Vector4 R4);

	Matrix44 operator *(const Matrix44& matrix) const;
	// Accessors
	Vector2 TransformPosition2D( const Vector2& position2D ); // Written assuming z=0, w=1
	Vector2 TransformDisplacement2D( const Vector2& displacement2D ); // Written assuming z=0, w=0
	float   Get(int column, int row);
	float	Get(int index);
	void	Set(int index,float value);
	float   GetTrace3();

	Vector3 Multiply(Vector3 position);

	Vector3 GetIVector();
	Vector3 GetJVector();
	Vector3 GetKVector();
	Vector3 GetTVector();

	Vector4 GetIVector4();
	Vector4 GetJVector4();
	Vector4 GetKVector4();
	Vector4 GetTVector4();

	Vector3 GetXComponent();
	Vector3 GetYComponent();
	Vector3 GetZComponent();
	Vector3 GetWComponent();
	Vector3 GetDiagonalValues();

	Vector4 GetXComponent4();
	Vector4 GetYComponent4();
	Vector4 GetZComponent4();
	Vector4 GetWComponent4();
																	 
	void	SetIdentity();
	void	SetValues( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	void    SetTranslation(Vector4 translation);
	Vector4 GetTranslation();
	void	Append( const Matrix44& matrixToAppend ); // a.k.a. Concatenate (right-multiply)
	void	RotateDegrees2D( float rotationDegreesAboutZ ); // 
	void	Translate2D( const Vector2& translation );
	void	Translate3D( const Vector3& translation );
	void	ScaleUniform2D( float scaleXY );
	void	Scale3D( float scaleX, float scaleY,float scaleZ);
	void	Scale2D( float scaleX, float scaleY );
	void	Transpose();
	void	InvertFast();
	void	InvertFast1();
	void	Inverse();
	Vector3 GetEulerFromMatrix();
	void	MultiplyAndSet(Matrix44 valueMatrix);
	// Producers
	static Matrix44 MakeRotationDegrees2D( float rotationDegreesAboutZ );
	static Matrix44 MakeRotationDegreesInY( float rotationDegreesAboutZ );
	static Matrix44 MakeRotationDegreesInX( float rotationDegreesAboutX );
	static Matrix44 MakeRotationDegreesInZ(float rotationDegreesAboutZ);
	static Matrix44 MakeTranslation2D( const Vector2& translation );
	static Matrix44 MakeTranslation3D( const Vector3& translation );
	static Matrix44 MakeScaleUniform2D( float scaleXY );
	static Matrix44 MakeScale3D( Vector3 scale);
	static Matrix44 MakeScale2D( float scaleX, float scaleY );
	static Matrix44 MakeOrtho2D( const Vector2& bottomLeft, const Vector2& topRight );
	static Matrix44 MatrixFromEuler(Vector3 angles);
	static Matrix44 MakePerspectiveMatrix(float fovDegrees,float aspect,float nearz,float farz);
	static Matrix44 MakeRotationMatrixFromDirection(Vector3 direction);
	static Matrix44 LookAt(Vector3 forward, Vector3 upvector);
	static Matrix44 LookAt1(Vector3 position,Vector3 forward, Vector3 upvector);
	static Matrix44 LookAt(Vector3 position,Vector3 lookAtPos, Vector3 upvector);
	static Matrix44 LerpTransform(Matrix44 &a, Matrix44 &b, float delta);
	static Matrix44 TurnTowards(Matrix44 &current, Matrix44 &target, float maxTurnPerFrame);
	static Vector4  Multiply(Matrix44 mat44,Vector4 vec4);


};