#pragma once
//#include "Engine/Math/IntVector2.hpp"
#include <string>
class IntVector2;
class Vector2
{
public:

	float x;
	float y;

	// Construction/Destruction
	Vector2 () { x = y = 0; }								// default constructor: do nothing (for speed)
	~Vector2() {}											// destructor: do nothing (for speed)
	Vector2 ( const Vector2& copyFrom );					// copy constructor (from another vec2)
	Vector2 (std::string text);
	explicit Vector2( float initialX, float initialY );		// explicit constructor (from x, y)

																		// Operators
	const Vector2   operator	+  (const Vector2& vecToAdd) const;			// vec2 + vec2
	const Vector2   operator	-  (const Vector2& vecToSubtract) const;	// vec2 - vec2
	const Vector2   operator	*  (float uniformScale) const;				// vec2 * float
	const Vector2   operator	/  (float inverseScale) const;				// vec2 / float
	void		    operator	+= (const Vector2& vecToAdd);				// vec2 += vec2
	void		    operator	-= (const Vector2& vecToSubtract);			// vec2 -= vec2
	void		    operator	+= (float value);
	void		    operator	*= (const float uniformScale);				// vec2 *= float
	void		    operator	/= (const float uniformDivisor);			// vec2 /= float
	void		    operator	=  (const Vector2& copyFrom);				// vec2 = vec2
	bool		    operator	== (const Vector2& compare) const;			// vec2 == vec2
	bool		    operator	!= (const Vector2& compare) const;			// vec2 != vec2

	const Vector2   operator	*  (Vector2 vectora) const;

	friend const Vector2 operator*(float uniformScale, const Vector2& vecToScale);


	float			GetLength() const;
	float			GetLengthSquared() const;					// faster than GetLength() since it skips the sqrtf()
	float			NormalizeAndGetLength();					// set my new length to 1.0f; keep my direction
	Vector2			GetNormalized() const;						// return a new vector, which is a normalized copy of me
	float			GetOrientationDegrees() const;			    // return 0 for east (5,0), 90 for north (0,8), etc.
	IntVector2		ToIntVector2();
	bool			SetFromText( const char* text );
	std::string		GetAsString();

	static Vector2  MakeDirectionAtDegrees( float degrees );// const; // create vector at angle
	static Vector2  Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd );
				    
	static Vector2  ZERO;
	static Vector2  ONE;
	static Vector2  ONE_ZERO;
	static Vector2  ZERO_ONE;
	static Vector2  EAST;
	static Vector2  WEST;
	static Vector2  NORTH;
	static Vector2  SOUTH;
				    
	static Vector2  NORTHEAST;
	static Vector2  NORTHWEST;
	static Vector2  SOUTHEAST;
	static Vector2  SOUTHWEST;
};


